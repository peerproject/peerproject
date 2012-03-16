//
// BTInfo.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "BTInfo.h"
#include "BENode.h"
#include "Buffer.h"
#include "Download.h"
#include "Downloads.h"
#include "DownloadTask.h"
#include "FragmentedFile.h"
#include "Transfers.h"
#include "Library.h"
#include "SharedFile.h"
#include "SharedFolder.h"
#include "DlgProgressBar.h"
#include "DownloadWithTorrent.h"	// For scrape m_pPeerId

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

// Check if a string is a valid path/file name.
static bool IsValid(const CString& str)
{
	return ! str.IsEmpty() && ( str.Find( _T('?') ) == -1 ) && ( str != _T("#ERROR#") );
}

//////////////////////////////////////////////////////////////////////
// CBTInfo construction

CBTInfo::CBTInfo()
	: m_nBlockSize		( 0ul )
	, m_nBlockCount		( 0ul )
	, m_pBlockBTH		( NULL )
	, m_nTotalUpload	( 0ull )
	, m_nTotalDownload	( 0ull )
	, m_nTrackerSeeds	( 0 )
	, m_nTrackerPeers	( 0 )
	, m_nTrackerWait	( 90 * 1000 )
	, m_nTrackerIndex	( -1 )
	, m_nTrackerMode	( tNull )
	, m_tTrackerScrape	( 0ul )
	, m_tCreationDate	( 0ul )
	, m_nTestByte		( 0ul )
	, m_nInfoSize		( 0ul )
	, m_nInfoStart		( 0ul )
	, m_bPrivate		( FALSE )
	, m_nStartDownloads	( dtAlways )
	, m_bEncodingError	( false )
	, m_nEncoding		( Settings.BitTorrent.TorrentCodePage )
{
	CBENode::m_nDefaultCP = Settings.BitTorrent.TorrentCodePage;
}

CBTInfo::CBTInfo(const CBTInfo& oSource)
	: m_nBlockSize		( 0ul )
	, m_nBlockCount		( 0ul )
	, m_pBlockBTH		( NULL )
	, m_nTotalUpload	( 0ull )
	, m_nTotalDownload	( 0ull )
	, m_nTrackerIndex	( -1 )
	, m_nTrackerMode	( tNull )
	, m_nTestByte		( 0ul )
	, m_nInfoSize		( 0ul )
	, m_nInfoStart		( 0ul )
	, m_tCreationDate	( 0ul )
	, m_bPrivate		( FALSE )
	, m_nStartDownloads	( dtAlways )
	, m_bEncodingError	( false )
	, m_nEncoding		( Settings.BitTorrent.TorrentCodePage )
{
	*this = oSource;

	CBENode::m_nDefaultCP = Settings.BitTorrent.TorrentCodePage;
}

CBTInfo::~CBTInfo()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CBTFile construction

CBTInfo::CBTFile::CBTFile(const CBTInfo* pInfo, const CBTFile* pBTFile)
	: m_pInfo		( pInfo )
	, m_nOffset		( pBTFile ? pBTFile->m_nOffset : 0 )
{
	if ( pBTFile )
		CPeerProjectFile::operator=( *pBTFile );
}

CString	CBTInfo::CBTFile::FindFile()
{
	CString strFile;

	CSingleLock oLibraryLock( &Library.m_pSection, TRUE );

	// Try find file by hash/size
	const CLibraryFile* pShared = LibraryMaps.LookupFileByHash( this, FALSE, TRUE );
	if ( pShared )
		strFile = pShared->GetPath();
	if ( strFile.GetLength() > MAX_PATH )
		strFile = _T("\\\\?\\") + strFile;
	if ( ! pShared || GetFileSize( strFile ) != m_nSize )
	{
		// Try complete folder
		strFile = Settings.Downloads.CompletePath + _T("\\") + m_sPath;
		if ( strFile.GetLength() > MAX_PATH )
			strFile = _T("\\\\?\\") + strFile;
		if ( GetFileSize( strFile ) != m_nSize )
		{
			// Try folder of original .torrent
			CString strTorrentPath = m_pInfo->m_sPath.Left( m_pInfo->m_sPath.ReverseFind( _T('\\') ) + 1 );
			strFile = strTorrentPath + m_sPath;
			if ( strFile.GetLength() > MAX_PATH )
				strFile = _T("\\\\?\\") + strFile;
			if ( GetFileSize( strFile ) != m_nSize )
			{
				// Try complete folder without outer file directory
				CString strShortPath;
				int nSlash = m_sPath.Find( _T('\\') );
				if ( nSlash != -1 )
					strShortPath = m_sPath.Mid( nSlash + 1 );
				strFile = Settings.Downloads.CompletePath + _T("\\") + strShortPath;
				if ( strFile.GetLength() > MAX_PATH )
					strFile = _T("\\\\?\\") + strFile;
				if ( strShortPath.IsEmpty() || GetFileSize( strFile ) != m_nSize )
				{
					// Try folder of original .torrent without outer file directory
					strFile = strTorrentPath + strShortPath;
					if ( strFile.GetLength() > MAX_PATH )
						strFile = _T("\\\\?\\") + strFile;
					if ( strShortPath.IsEmpty() || GetFileSize( strFile ) != m_nSize )
					{
						// Try find by name only
						pShared = LibraryMaps.LookupFileByName( m_sName, m_nSize, FALSE, TRUE );
						if ( pShared )
						{
							strFile = pShared->GetPath();
							if ( strFile.GetLength() > MAX_PATH )
								strFile = _T("\\\\?\\") + strFile;
						}
						if ( ! pShared || GetFileSize( strFile ) != m_nSize )
							return m_sPath;
					}
				}
			}
		}
	}

	// Refill missed hashes
	if ( ! pShared )
		pShared = LibraryMaps.LookupFileByPath( strFile, FALSE, FALSE );
	if ( pShared )
	{
		if ( ! m_oSHA1 && pShared->m_oSHA1 )
			m_oSHA1 = pShared->m_oSHA1;
		if ( ! m_oTiger && pShared->m_oTiger )
			m_oTiger = pShared->m_oTiger;
		if ( ! m_oED2K && pShared->m_oED2K )
			m_oED2K = pShared->m_oED2K;
		if ( ! m_oBTH && pShared->m_oBTH )
			m_oBTH = pShared->m_oBTH;
		if ( ! m_oMD5 && pShared->m_oMD5 )
			m_oMD5 = pShared->m_oMD5;
	}

	return strFile;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo clear

void CBTInfo::Clear()
{
	delete [] m_pBlockBTH;
	m_pBlockBTH = NULL;

	for ( POSITION pos = m_pFiles.GetHeadPosition() ; pos ; )
		delete m_pFiles.GetNext( pos );
	m_pFiles.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CBTInfo copy

CBTInfo& CBTInfo::operator=(const CBTInfo& oSource)
{
	Clear();

	CPeerProjectFile::operator=( oSource );

	m_sURLs.RemoveAll();
	for ( POSITION pos = oSource.m_sURLs.GetHeadPosition() ; pos ; )
		m_sURLs.AddTail( oSource.m_sURLs.GetNext( pos ) );

	m_nBlockSize		= oSource.m_nBlockSize;
	m_nBlockCount		= oSource.m_nBlockCount;

	if ( oSource.m_pBlockBTH )
	{
		m_pBlockBTH = new Hashes::BtPureHash[ m_nBlockCount ];
		std::copy( oSource.m_pBlockBTH, oSource.m_pBlockBTH + m_nBlockCount,
			stdext::make_checked_array_iterator( m_pBlockBTH, m_nBlockCount ) );
	}

	m_nTotalUpload		= oSource.m_nTotalUpload;
	m_nTotalDownload	= oSource.m_nTotalDownload;

	for ( POSITION pos = oSource.m_pFiles.GetHeadPosition() ; pos ; )
		m_pFiles.AddTail( new CBTFile( this, oSource.m_pFiles.GetNext( pos ) ) );

	m_nEncoding			= oSource.m_nEncoding;
	m_sComment			= oSource.m_sComment;
	m_tCreationDate		= oSource.m_tCreationDate;
	m_sCreatedBy		= oSource.m_sCreatedBy;
	m_bPrivate			= oSource.m_bPrivate;
	m_nStartDownloads	= oSource.m_nStartDownloads;

	m_oTrackers.RemoveAll();
	for ( INT_PTR i = 0 ; i < oSource.m_oTrackers.GetCount() ; ++i )
		m_oTrackers.Add( oSource.m_oTrackers[ i ] );

	m_oNodes.RemoveAll();
	for ( POSITION pos = oSource.m_oNodes.GetHeadPosition() ; pos ; )
		m_oNodes.AddTail( oSource.m_oNodes.GetNext( pos ) );

	m_nTrackerIndex		= oSource.m_nTrackerIndex;
	m_nTrackerMode		= oSource.m_nTrackerMode;
	m_bEncodingError	= oSource.m_bEncodingError;
	m_pTestSHA1			= oSource.m_pTestSHA1;
	m_nTestByte			= oSource.m_nTestByte;

	m_pSource.Clear();
	m_pSource.Add( oSource.m_pSource.m_pBuffer, oSource.m_pSource.m_nLength );
	m_nInfoSize			= oSource.m_nInfoSize;
	m_nInfoStart		= oSource.m_nInfoStart;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo serialize

#define BTINFO_SER_VERSION	1000	//11
// nVersion History:
//  7 - redesigned tracker list (ryo-oh-ki)
//  8 - removed m_nFilePriority (ryo-oh-ki)
//  9 - added m_sName (ryo-oh-ki)
// 10 - added m_pSource (ivan386) (Shareaza 2.5.2)
// 11 - added m_nInfoStart & m_nInfoSize (ivan386)
// 1000 - (PeerProject 1.0) (11)

void CBTInfo::Serialize(CArchive& ar)
{
	int nVersion = BTINFO_SER_VERSION;

	if ( ar.IsStoring() )
	{
		ar << nVersion;

		SerializeOut( ar, m_oBTH );
		if ( ! m_oBTH ) return;

		ar << m_nSize;
		ar << m_nBlockSize;
		ar << m_nBlockCount;
		for ( DWORD i = 0 ; i < m_nBlockCount ; ++i )
		{
			ar.Write( &*m_pBlockBTH[ i ].begin(), m_pBlockBTH->byteCount );
		}

		ar << m_nTotalUpload;
		ar << m_nTotalDownload;

		ar << m_sName;

		ar << m_nEncoding;
		ar << m_sComment;
		ar << m_tCreationDate;
		ar << m_sCreatedBy;
		ar << m_bPrivate;

		ar.WriteCount( m_pFiles.GetCount() );
		for ( POSITION pos = m_pFiles.GetHeadPosition() ; pos ; )
			m_pFiles.GetNext( pos )->Serialize( ar, nVersion );

		ar << m_nTrackerIndex;
		ar << m_nTrackerMode;

		int nTrackers = (int)m_oTrackers.GetCount();
		ar.WriteCount( nTrackers );
		for ( int nTracker = 0 ; nTracker < nTrackers ; nTracker++ )
		{
			m_oTrackers[ nTracker ].Serialize( ar, nVersion );
		}

		if ( m_pSource.m_nLength && m_nInfoSize )
		{
			ar << m_pSource.m_nLength;
			ar.Write( m_pSource.m_pBuffer, m_pSource.m_nLength );
			ar << m_nInfoStart;
			ar << m_nInfoSize;
		}
		else
		{
			ar << (DWORD)0;
		}
	}
	else // Loading
	{
		// ToDo: Are any BTINFO_SER_VERSION nVersions still necessary for Shareaza imports?

		ar >> nVersion;
		if ( nVersion < 7 )	// Shareaza 2.3
			AfxThrowUserException();

		SerializeIn( ar, m_oBTH, nVersion );
		if ( ! m_oBTH ) return;

		ar >> m_nSize;
		ar >> m_nBlockSize;
		ar >> m_nBlockCount;

		if ( m_nBlockCount )
		{
			m_pBlockBTH = new Hashes::BtPureHash[ (DWORD)m_nBlockCount ];

			for ( DWORD i = 0 ; i < m_nBlockCount ; ++i )
			{
				ReadArchive( ar, &*m_pBlockBTH[ i ].begin(), m_pBlockBTH->byteCount );
			}
		}

		ar >> m_nTotalUpload;
		ar >> m_nTotalDownload;

		ar >> m_sName;

		ar >> m_nEncoding;
		ar >> m_sComment;
		ar >> m_tCreationDate;
		ar >> m_sCreatedBy;

		ar >> m_bPrivate;

		int nFiles = (int)ar.ReadCount();
		QWORD nOffset = 0;
		for ( int nFile = 0 ; nFile < nFiles ; nFile++ )
		{
			CAutoPtr< CBTFile >pBTFile( new CBTFile( this ) );
			if ( ! pBTFile )
				AfxThrowUserException();	// Out Of Memory

			pBTFile->Serialize( ar, nVersion );

			pBTFile->m_nOffset = nOffset;
			nOffset += pBTFile->m_nSize;

			m_pFiles.AddTail( pBTFile.Detach() );
		}

		//if ( nVersion < 7 )	// Shareaza 2.3 Import
		//{
		//	CString sTracker;
		//	ar >> sTracker;
		//	SetTracker( sTracker );
		//}

		ar >> m_nTrackerIndex;
		ar >> m_nTrackerMode;

		//if ( nVersion < 7 )	// Shareaza 2.3 Import
		//{
		//	int nTrackers = (int)ar.ReadCount();
		//	if ( nTrackers )
		//	{
		//		CBTTracker oTracker;
		//		oTracker.Serialize( ar, nVersion );
		//		AddTracker( oTracker );
		//	}
		//}

		int nTrackers = (int)ar.ReadCount();
		if ( nTrackers )
		{
			for ( int nTracker = 0 ; nTracker < nTrackers ; nTracker++ )
			{
				CBTTracker oTracker;
				oTracker.Serialize( ar, nVersion );
				AddTracker( oTracker );
			}
		}

		if ( nVersion >= 10 )	// Shareaza 2.5.2.0+ (PeerProject 1.0)
		{
			DWORD nLength;
			ar >> nLength;
			if ( nLength )
			{
				m_pSource.EnsureBuffer( nLength );
				ar.Read( m_pSource.m_pBuffer, nLength );
				m_pSource.m_nLength = nLength;
				//if ( nVersion >= 11 )
				//{
					ar >> m_nInfoStart;
					ar >> m_nInfoSize;
				//}
				//else
				//{
				//	VERIFY( CheckInfoData() );
				//}
			}
		}

		SetTrackerNext();

		// Imported Partial from Shareaza 2.4
		//if ( nVersion < 8 )
		//	ConvertOldTorrents();
	}
}

//void CBTInfo::ConvertOldTorrents()
//{
//	// For Importing Shareaza 2.4 Multifile Partial Only
//	// Reference only. Comment this out for PeerProject 1.0 release
//
//	if ( m_pFiles.GetCount() < 2 )
//		return;
//
//	if ( ! Downloads.IsSpaceAvailable( m_nSize, Downloads.dlPathComplete ) )
//		AfxThrowFileException( CFileException::diskFull );
//
//	CString strSource;
//	strSource.Format( _T("%s\\%s.partial"), Settings.Downloads.IncompletePath, GetFilename() );
//	if ( strSource.GetLength() > MAX_PATH ) strSource = L"\\\\?\\" + strSource;
//
//	if ( GetFileAttributes( strSource ) == INVALID_FILE_ATTRIBUTES )
//		return;
//
//	CFile oSource( strSource, CFile::modeRead | CFile::osSequentialScan | CFile::shareDenyNone );
//
//	const DWORD BUFFER_SIZE = 8ul * 1024ul * 1024ul;
//	BYTE* pBuffer = new BYTE[ BUFFER_SIZE ];
//	if ( ! pBuffer )
//		AfxThrowMemoryException();
//
//	CString strTargetTemplate;
//	strTargetTemplate.Format( _T("%s\\%s"), Settings.Downloads.IncompletePath, GetFilename() );
//	if ( strTargetTemplate.GetLength() > MAX_PATH ) strTargetTemplate = L"\\\\?\\" + strTargetTemplate;
//
//	CString strText;
//	CProgressBarDlg oProgress( CWnd::GetDesktopWindow() );
//	strText.LoadString( IDS_BT_UPDATE_TITLE );
//	oProgress.SetWindowText( strText );
//	strText.LoadString( IDS_BT_UPDATE_CONVERTING );
//	oProgress.SetActionText( strText );
//	oProgress.SetEventText( m_sName );
//	oProgress.SetEventRange( 0, int( m_nSize / 1024ull ) );
//	oProgress.CenterWindow();
//	oProgress.ShowWindow( SW_SHOW );
//	oProgress.UpdateWindow();
//	oProgress.UpdateData( FALSE );
//
//	CString strOf;
//	strOf.LoadString( IDS_GENERAL_OF );
//	QWORD nTotal = 0ull;
//	DWORD nCount = 0ul;
//	for ( POSITION pos = m_pFiles.GetHeadPosition() ; pos ; ++nCount )
//	{
//		CBTFile* pFile = m_pFiles.GetNext( pos );
//
//		CString strTarget;
//		strTarget.Format( _T("%s_%lu.partial"), strTargetTemplate, nCount );
//
//		CFile oTarget( strTarget, CFile::modeCreate | CFile::modeWrite | CFile::osSequentialScan );
//
//		strText.Format( _T("%lu %s %i"), nCount + 1ul, strOf, m_pFiles.GetCount() );
//
//		if ( Settings.General.LanguageRTL )
//			strText = _T("\x202B") + strText;
//
//		oProgress.SetSubActionText( strText );
//		oProgress.SetSubEventText( pFile->m_sPath );
//		oProgress.SetSubEventRange( 0, int( pFile->m_nSize / 1024ull ) );
//		oProgress.UpdateData( FALSE );
//		oProgress.UpdateWindow();
//
//		QWORD nLength = pFile->m_nSize;
//		while ( nLength )
//		{
//			DWORD nBuffer = min( nLength, BUFFER_SIZE );
//
//			nBuffer = oSource.Read( pBuffer, nBuffer );
//			if ( nBuffer )
//				oTarget.Write( pBuffer, nBuffer );
//
//			nLength -= nBuffer;
//			nTotal += nBuffer;
//
//			oProgress.StepSubEvent( int( nBuffer / 1024ul ) );
//			oProgress.SetEventPos( int( nTotal / 1024ull ) );
//			oProgress.UpdateWindow();
//		}
//	}
//	ASSERT( nTotal == m_nSize );
//
//	delete [] pBuffer;
//}

//////////////////////////////////////////////////////////////////////
// CBTInfo::CBTFile serialize

void CBTInfo::CBTFile::Serialize(CArchive& ar, int nVersion)
{
	if ( ar.IsStoring() )
	{
		ar << m_nSize;
		ar << m_sPath;
		ar << m_sName;
		SerializeOut( ar, m_oSHA1 );
		SerializeOut( ar, m_oED2K );
		SerializeOut( ar, m_oTiger );
		SerializeOut( ar, m_oMD5 );
	}
	else // Loading
	{
		ar >> m_nSize;
		ar >> m_sPath;

		if ( nVersion > 8 )
			ar >> m_sName;
		else // Upgrade Shareaza Import?
			m_sName = PathFindFileName( m_sPath );

		SerializeIn( ar, m_oSHA1, nVersion );

		//if ( nVersion >= 4 )
		//{
			SerializeIn( ar, m_oED2K, nVersion );
			SerializeIn( ar, m_oTiger, nVersion );
			if ( nVersion < 8 )
			{
				int nFilePriority;
				ar >> nFilePriority;
			}
		//}

		//if ( nVersion >= 6 )
			SerializeIn( ar, m_oMD5, nVersion );
	}
}

//////////////////////////////////////////////////////////////////////
// CBTInfo load .torrent file

BOOL CBTInfo::LoadTorrentFile(LPCTSTR pszFile)
{
	CFile pFile;

	if ( pFile.Open( pszFile, CFile::modeRead|CFile::shareDenyNone ) )
	{
		DWORD nLength = (DWORD)pFile.GetLength();
		m_sPath = pszFile;

		if ( nLength < 20 * 1024 * 1024 && nLength != 0 )
		{
			m_pSource.Clear();
			if ( m_pSource.EnsureBuffer( nLength ) )
			{
				pFile.Read( m_pSource.m_pBuffer, nLength );
				m_pSource.m_nLength = nLength;

				return LoadTorrentBuffer( &m_pSource );
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo save .torrent file

BOOL CBTInfo::SaveTorrentFile(const CString& sFolder)
{
	if ( ! IsAvailable() )
		return FALSE;

	if ( m_pSource.m_nLength == 0 )
		return FALSE;

	CString strPath = sFolder + _T("\\") + SafeFilename( m_sName + _T(".torrent") );
	if ( m_sPath.CompareNoCase( strPath ) == 0 )
		return TRUE;	// Same file

	CFile pFile;
	if ( ! pFile.Open( strPath, CFile::modeWrite | CFile::modeCreate ) )
		return FALSE;

	pFile.Write( m_pSource.m_pBuffer, m_pSource.m_nLength );
	pFile.Close();

	//m_sPath = strPath;	// ToDo?

	return TRUE;
}

#define MAX_PIECE_SIZE (16 * 1024)
BOOL CBTInfo::LoadInfoPiece(BYTE *pPiece, DWORD nPieceSize, DWORD nInfoSize, DWORD nInfoPiece)
{
	ASSERT( nPieceSize <= MAX_PIECE_SIZE );
	if ( nPieceSize > MAX_PIECE_SIZE )
		return FALSE;

	if ( m_pSource.m_nLength == 0 && nInfoPiece == 0 )
	{
		CBENode oRoot;
		if ( GetTrackerCount() > 0 )
		{
			// Create .torrent file with tracker if needed

			oRoot.Add( "announce" )->SetString( GetTrackerAddress() );	// "8:announce%d:%s"

			if ( GetTrackerCount() > 1 )
			{
				CBENode* pList = oRoot.Add("announce-list")->Add(); 	// "13:announce-listll"
				for ( int i = 0 ; i < GetTrackerCount() ; i++ )
				{
					pList->Add()->SetString(GetTrackerAddress( i ));
				}
			}
		}
		oRoot.Add( "info" )->SetInt(0); 	// "d4:info"
		oRoot.Encode( &m_pSource );
		m_pSource.m_nLength -= 4;
		m_nInfoStart = m_pSource.m_nLength;
	}

	QWORD nPieceStart = nInfoPiece * MAX_PIECE_SIZE;

	if ( nPieceStart == ( m_pSource.m_nLength - m_nInfoStart ) )
	{
		m_pSource.Add( pPiece, nPieceSize );

		if ( m_pSource.m_nLength - m_nInfoStart == nInfoSize )
		{
			m_pSource.Add( "e", 1 );
			return LoadTorrentBuffer( &m_pSource );
		}
	}
	return FALSE;
}

int CBTInfo::NextInfoPiece() const
{
	if ( m_pSource.m_nLength == 0 )
		return 0;

	if ( ! m_nInfoSize && m_pSource.m_nLength > m_nInfoStart )
		return ( m_pSource.m_nLength - m_nInfoStart ) / MAX_PIECE_SIZE;

	return -1;
}

DWORD CBTInfo::GetInfoPiece(DWORD nPiece, BYTE **pInfoPiece) const
{
	const DWORD nPieceStart = MAX_PIECE_SIZE * nPiece;
	if ( m_nInfoSize && m_nInfoStart &&
		m_pSource.m_nLength > m_nInfoStart + m_nInfoSize &&
		nPieceStart < m_nInfoSize )
	{
		*pInfoPiece = &m_pSource.m_pBuffer[ m_nInfoStart + nPieceStart ];
		DWORD nPieceSize = m_nInfoSize - nPieceStart;
		return nPieceSize > MAX_PIECE_SIZE ? MAX_PIECE_SIZE : nPieceSize;
	}
	return 0;
}

DWORD CBTInfo::GetInfoSize() const
{
	return m_nInfoSize;
}

BOOL CBTInfo::CheckInfoData()
{
	ASSERT( m_pSource.m_nLength );

	if ( ! m_pSource.m_nLength ) return FALSE;

	auto_ptr< CBENode > pNode ( CBENode::Decode( &m_pSource ) );
	if ( ! pNode.get() )
		return FALSE;

	const CBENode* pRoot = pNode.get();
	const CBENode* pInfo = pRoot->GetNode( "info" );

	if ( pInfo && pInfo->m_nSize &&
		 pInfo->m_nPosition + pInfo->m_nSize < m_pSource.m_nLength )
	{
		Hashes::BtHash oBTH;
		CSHA pBTH;
		pBTH.Add( &m_pSource.m_pBuffer[pInfo->m_nPosition], pInfo->m_nSize );
		pBTH.Finish();
		pBTH.GetHash( &oBTH[0] );

		if ( oBTH == m_oBTH )
		{
			m_nInfoStart = pInfo->m_nPosition;
			m_nInfoSize	 = pInfo->m_nSize;
			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo load torrent info from buffer

BOOL CBTInfo::LoadTorrentBuffer(const CBuffer* pBuffer)
{
	auto_ptr< CBENode > pNode ( CBENode::Decode( pBuffer ) );
	if ( ! pNode.get() )
	{
		theApp.Message( MSG_ERROR, _T("[BT] Failed to decode torrent data: %s"), pBuffer->ReadString( (size_t)-1 ) );
		return FALSE;
	}

	return LoadTorrentTree( pNode.get() );
}

//////////////////////////////////////////////////////////////////////
// CBTInfo load torrent info from tree

BOOL CBTInfo::LoadTorrentTree(const CBENode* pRoot)
{
	//ASSERT( m_sName.IsEmpty() && m_nSize == SIZE_UNKNOWN );	// Assume empty object
	ASSERT( ! m_pBlockBTH );

	theApp.Message( MSG_DEBUG, _T("[BT] Loading torrent tree: %s"), (LPCTSTR)pRoot->Encode() );

	if ( ! pRoot->IsType( CBENode::beDict ) )
		return FALSE;

	// Get the info node
	const CBENode* pInfo = pRoot->GetNode( "info" );
	if ( ! pInfo || ! pInfo->IsType( CBENode::beDict ) )
		return FALSE;

	if ( m_oBTH )
	{
		CSHA oSHA = pInfo->GetSHA1();
		Hashes::BtHash oBTH;
		oSHA.GetHash( &oBTH[ 0 ] );
		oBTH.validate();

		if ( oBTH != m_oBTH )
			return FALSE;
	}

	// Get the encoding (from torrents that have it)
	m_nEncoding = 0;
	const CBENode* pEncoding = pRoot->GetNode( "codepage" );
	if ( pEncoding && pEncoding->IsType( CBENode::beInt ) )
	{
		// "codepage" style (UNIT giving the exact Windows code page)
		m_nEncoding = (UINT)pEncoding->GetInt();
	}
	else
	{
		// "encoding" style (String representing the encoding to use)
		pEncoding = pRoot->GetNode( "encoding" );
		if ( pEncoding && pEncoding->IsType( CBENode::beString ) )
		{
			CString strEncoding = pEncoding->GetString();

			if ( strEncoding.GetLength() < 3 )
				theApp.Message( MSG_ERROR, _T("Torrent 'encoding' node too short") );
			else if ( _tcsistr( strEncoding.GetString(), _T("UTF-8") ) != NULL ||
					  _tcsistr( strEncoding.GetString(), _T("UTF8") ) != NULL )
				m_nEncoding = CP_UTF8;
			else if ( _tcsistr( strEncoding.GetString(), _T("ANSI") ) != NULL )
				m_nEncoding = CP_ACP;
			else if ( _tcsistr( strEncoding.GetString(), _T("BIG5") ) != NULL )
				m_nEncoding = 950;
			else if ( _tcsistr( strEncoding.GetString(), _T("Korean") ) != NULL )
				m_nEncoding = 949;
			else if ( _tcsistr( strEncoding.GetString(), _T("UHC") ) != NULL )
				m_nEncoding = 949;
			else if ( _tcsistr( strEncoding.GetString(), _T("Chinese") ) != NULL )
				m_nEncoding = 936;
			else if ( _tcsistr( strEncoding.GetString(), _T("GB2312") ) != NULL )
				m_nEncoding = 936;
			else if ( _tcsistr( strEncoding.GetString(), _T("GBK") ) != NULL )
				m_nEncoding = 936;
			else if ( _tcsistr( strEncoding.GetString(), _T("Japanese") ) != NULL )
				m_nEncoding = 932;
			else if ( _tcsistr( strEncoding.GetString(), _T("Shift-JIS") ) != NULL )
				m_nEncoding = 932;
			else if ( _tcsnicmp( strEncoding.GetString(), _T("Windows-"), 8 ) == 0 )
			{
				UINT nEncoding = 0;
				strEncoding = strEncoding.Mid( 8 );
				if ( ( _stscanf( strEncoding, _T("%u"), &nEncoding ) == 1 ) && ( nEncoding > 0 ) )
					m_nEncoding = nEncoding;
			}
			else if ( _tcsnicmp( strEncoding.GetString() , _T("CP"), 2 ) == 0 )
			{
				UINT nEncoding = 0;
				strEncoding = strEncoding.Mid( 2 );
				if ( ( _stscanf( strEncoding, _T("%u"), &nEncoding ) == 1 ) && ( nEncoding > 0 ) )
					m_nEncoding = nEncoding;
			}
		}
	}

	// Get the comments (if present)
	m_sComment = pRoot->GetStringFromSubNode( "comment", m_nEncoding );

	// Get the creation date (if present)
	const CBENode* pDate = pRoot->GetNode( "creation date" );
	if ( pDate && pDate->IsType( CBENode::beInt ) )
		m_tCreationDate = (DWORD)pDate->GetInt();
		// CTime pTime( (time_t)m_tCreationDate );
		// theApp.Message( MSG_NOTICE, pTime.Format( _T("%Y-%m-%d %H:%M:%S") ) );

	// Get the creator (if present)
	m_sCreatedBy = pRoot->GetStringFromSubNode( "created by", m_nEncoding );

	// Get nodes for DHT (if present) BEP 0005
	const CBENode* pNodeList = pRoot->GetNode( "nodes" );
	if ( pNodeList && pNodeList->IsType( CBENode::beList ) )
	{
		for ( int i = 0 ; i < pNodeList->GetCount() ; ++i )
		{
			const CBENode* pNode = pNodeList->GetNode( i );
			if ( pNode && pNode->IsType( CBENode::beList ) && pNode->GetCount() == 2 )
			{
				const CBENode* pHost = pNode->GetNode( 0 );
				const CBENode* pPort = pNode->GetNode( 1 );
				if ( pHost && pHost->IsType( CBENode::beString ) && pPort && pPort->IsType( CBENode::beInt ) )
				{
					CString strHost;
					strHost.Format( _T("%s:%u"), pHost->GetString(), (WORD)pPort->GetInt() );
					m_oNodes.AddTail( strHost );
				//	HostCache.BitTorrent.Add( pHost->GetString(), (WORD)pPort->GetInt() );	// Obsolete
				}
			}
		}
	}

	// Get announce-list (if present)
	CBENode* pAnnounceList = pRoot->GetNode( "announce-list" );
	if ( pAnnounceList && pAnnounceList->IsType( CBENode::beList ) )
	{
		m_nTrackerMode = tMultiFinding;

		CList< CString > pTrackers, pBadTrackers;

		// Loop through all the tiers
		for ( int nTier = 0 ; nTier < pAnnounceList->GetCount() ; nTier++ )
		{
			const CBENode* pSubList = pAnnounceList->GetNode( nTier );
			if ( pSubList && pSubList->IsType( CBENode::beList ) )
			{
				// Read in the trackers
				for ( int nTracker = 0 ; nTracker < pSubList->GetCount() ; nTracker++ )
				{
					const CBENode* pTracker = pSubList->GetNode( nTracker );
					if ( pTracker && pTracker->IsType( CBENode::beString ) )
					{
						CString strTracker = pTracker->GetString();		// Get the tracker

						// Check tracker is valid
						if ( ! StartsWith( strTracker, _T("http://"), 7 ) &&
							 ! StartsWith( strTracker, _T("udp://"), 6 ) )					// ToDo: Handle rare HTTPS etc?
							pBadTrackers.AddTail( BAD_TRACKER_TOKEN + strTracker );			// Store Unknown tracker for display (*https://)
						else if ( strTracker.Find( _T("denis.stalker.h3q.com"), 5 ) > 5 ||
								  strTracker.Find( _T("piratebay.org"), 6 ) > 6 ||
								  strTracker.Find( _T(".1337x."), 8 ) > 8 )
							pBadTrackers.AddTail( BAD_TRACKER_TOKEN + strTracker );			// Store common dead trackers for display
						else
							pTrackers.AddTail( strTracker );								// Store TCP tracker
					}
				}

				if ( ! pTrackers.IsEmpty() )
				{
					// Randomize the tracker order in this tier
					if ( pTrackers.GetCount() > 1 )
					{
						for ( POSITION pos = pTrackers.GetHeadPosition() ; pos ; )
						{
							if ( GetRandomNum( 0, 1 ) )
							{
								CString strTemp = pTrackers.GetAt( pos );
								pTrackers.RemoveAt( pos );

								if ( GetRandomNum( 0, 1 ) )
									pTrackers.AddHead( strTemp );
								else
									pTrackers.AddTail( strTemp );
							}
							pTrackers.GetNext( pos );
						}
					}

					// Store the trackers
					for ( POSITION pos = pTrackers.GetHeadPosition() ; pos ; )
					{
						// Create the tracker and add it to the list
						AddTracker( CBTTracker( pTrackers.GetNext( pos ), nTier ) );
					}
					// Delete temporary storage
					pTrackers.RemoveAll();
				}
			}
		}

		// Catch unsupported trackers for display at end of list.
		if ( ! pBadTrackers.IsEmpty() )
		{
			for ( POSITION pos = pBadTrackers.GetHeadPosition() ; pos ; )
			{
				// Create the tracker and add it to the list
				AddTracker( CBTTracker( pTrackers.GetNext( pos ), 99 ) );

			//	CBTTracker oTracker;
			//	oTracker.m_sAddress	= BAD_TRACKER_TOKEN + pTrackers.GetNext( pos );				// Mark for display only: *https://...
			//	oTracker.m_nFailures = 1;
			//	oTracker.m_nTier = 99;
			//	AddTracker( oTracker );
			}
		}

		SetTrackerNext();
	}

	// Announce node is ignored by multi-tracker torrents
	if ( m_oTrackers.IsEmpty() )
	{
		// Get announce
		const CBENode* pAnnounce = pRoot->GetNode( "announce" );
		if ( pAnnounce && pAnnounce->IsType( CBENode::beString ) )
		{
			// Get the tracker
			CString strTracker = pAnnounce->GetString();

			// Store it if it's valid. (Some torrents have invalid trackers)
			if ( StartsWith( strTracker, _T("http://"), 7 ) ||
				 StartsWith( strTracker, _T("udp://"), 6 ) )
			{
				// Catch common defunct TCP trackers
				if ( strTracker.Find( _T("piratebay.org"), 7 ) > 7 )
					strTracker = _T("http://tracker.publicbt.com/announce");	// Settings.BitTorrent.DefaultTracker ?

				// Set the torrent to be a single-tracker torrent
				m_nTrackerMode = tSingle;
				SetTracker( strTracker );

				// Backup tracker
				//CBTTracker oTracker;
				//oTracker.m_sAddress = _T("http://tracker.publicbt.com/announce");
				//oTracker.m_nTier = 0;
				//m_nTrackerMode = tMultiFinding;
				//AddTracker( oTracker );
			}
			else
			{
				// Torrents should always have a valid announce node, other is unlikely.  Try public TCP tracker. (Should get Private tag first...)
				strTracker = _T("http://tracker.publicbt.com/announce");	// Settings.BitTorrent.DefaultTracker ?
				SetTracker( strTracker );
				m_nTrackerMode = tSingle;
			}
		}
	}

	// Get the info node
	//CBENode* pInfo = pRoot->GetNode( "info" );
	//if ( ! pInfo || ! pInfo->IsType( CBENode::beDict ) ) return FALSE;

	// Get the private flag (if present)
	const CBENode* pPrivate = pInfo->GetNode( "private" );
	if ( pPrivate && pPrivate->IsType( CBENode::beInt ) )
		m_bPrivate = pPrivate->GetInt() != 0;

	// Get the name
	m_sName = pInfo->GetStringFromSubNode( "name", m_nEncoding );

	// If we still don't have a name, generate one
	if ( m_sName.IsEmpty() )
		m_sName.Format( _T("Unnamed_Torrent_%i"), GetRandomNum( 0i32, _I32_MAX ) );

	// Get the piece stuff
	const CBENode* pPL = pInfo->GetNode( "piece length" );
	if ( ! pPL || ! pPL->IsType( CBENode::beInt ) ) return FALSE;
	m_nBlockSize = (DWORD)pPL->GetInt();
	if ( ! m_nBlockSize ) return FALSE;

	const CBENode* pHash = pInfo->GetNode( "pieces" );
	if ( ! pHash || ! pHash->IsType( CBENode::beString ) ) return FALSE;
	if ( pHash->m_nValue % Hashes::Sha1Hash::byteCount ) return FALSE;
	m_nBlockCount = (DWORD)( pHash->m_nValue / Hashes::Sha1Hash::byteCount );
	if ( ! m_nBlockCount || m_nBlockCount > 209716 ) return FALSE;

	m_pBlockBTH = new Hashes::BtPureHash[ m_nBlockCount ];

	std::copy( static_cast< const Hashes::BtHash::RawStorage* >( pHash->m_pValue ),
		static_cast< const Hashes::BtHash::RawStorage* >( pHash->m_pValue ) + m_nBlockCount,
		stdext::make_checked_array_iterator( m_pBlockBTH, m_nBlockCount ) );

	// Hash info
	if ( const CBENode* pSHA1 = pInfo->GetNode( "sha1" ) )
	{
		if ( ! pSHA1->IsType( CBENode::beString ) || pSHA1->m_nValue != Hashes::Sha1Hash::byteCount ) return FALSE;
		m_oSHA1 = *static_cast< const Hashes::BtHash::RawStorage* >( pSHA1->m_pValue );
	}
	else if ( const CBENode* pSHA1Base16 = pInfo->GetNode( "filehash" ) )
	{
		if ( ! pSHA1Base16->IsType( CBENode::beString ) ||
			pSHA1Base16->m_nValue != Hashes::BtGuid::byteCount ) return FALSE;
		m_oSHA1 = *static_cast< const Hashes::BtGuid::RawStorage* >( pSHA1Base16->m_pValue );
	}

	if ( const CBENode* pED2K = pInfo->GetNode( "ed2k" ) )
	{
		if ( ! pED2K->IsType( CBENode::beString ) || pED2K->m_nValue != Hashes::Ed2kHash::byteCount ) return FALSE;
		m_oED2K = *static_cast< const Hashes::Ed2kHash::RawStorage* >( pED2K->m_pValue );
	}

	if ( const CBENode* pMD5 = pInfo->GetNode( "md5sum" ) )
	{
		if ( ! pMD5->IsType( CBENode::beString ) ) return FALSE;

		else if ( pMD5->m_nValue == Hashes::Md5Hash::byteCount )
		{
			m_oMD5 = *static_cast< const Hashes::Md5Hash::RawStorage* >( pMD5->m_pValue );
		}
		else if ( pMD5->m_nValue == Hashes::Md5Hash::byteCount * 2 )
		{
			CStringA tmp;
			tmp.Append( (const char*)pMD5->m_pValue, (int)pMD5->m_nValue );
			m_oMD5.fromString( CA2W( tmp ) );
		}
		else
		{
			return FALSE;
		}
	}

	if ( const CBENode* pTiger = pInfo->GetNode( "tiger" ) )
	{
		if ( ! pTiger->IsType( CBENode::beString ) || pTiger->m_nValue != Hashes::TigerHash::byteCount ) return FALSE;
		m_oTiger = *static_cast< const Hashes::TigerHash::RawStorage* >( pTiger->m_pValue );
	}

	// Details on file (or files).
	if ( const CBENode* pSingleLength = pInfo->GetNode( "length" ) )
	{
		if ( ! pSingleLength->IsType( CBENode::beInt ) )
			return FALSE;
		m_nSize = (QWORD)pSingleLength->GetInt();
		if ( ! m_nSize )
			return FALSE;

		CAutoPtr< CBTFile >pBTFile( new CBTFile( this ) );
		if ( ! pBTFile )		// Out of memory
			return FALSE;

		pBTFile->m_sPath = m_sName;
		pBTFile->m_sName = PathFindFileName( m_sName );
		pBTFile->m_nSize = m_nSize;
		pBTFile->m_oSHA1 = m_oSHA1;
		pBTFile->m_oTiger = m_oTiger;
		pBTFile->m_oED2K = m_oED2K;
		pBTFile->m_oMD5 = m_oMD5;
		m_pFiles.AddTail( pBTFile.Detach() );

		// Add sources from torrents - DWK
		const CBENode* pSources = pRoot->GetNode( "sources" );
		if ( pSources && pSources->IsType( CBENode::beList ) )
		{
			int m_nSources = pSources->GetCount();
			for ( int nSource = 0 ; nSource < m_nSources ; nSource++ )
			{
				CBENode* pSource = pSources->GetNode( nSource );
				if ( ! pSource || ! pSource->IsType(CBENode::beString) ) continue;
				m_sURLs.AddTail( pSource->GetString() );
			}
		}
	}
	else if ( const CBENode* pFiles = pInfo->GetNode( "files" ) )
	{
		CString strPath;

		if ( ! pFiles->IsType( CBENode::beList ) ) return FALSE;
		int nFiles = pFiles->GetCount();
		if ( ! nFiles || nFiles > 8192 * 8 ) return FALSE;

		m_nSize = 0;

		QWORD nOffset = 0;
		for ( int nFile = 0 ; nFile < nFiles ; nFile++ )
		{
			CAutoPtr< CBTFile > pBTFile( new CBTFile( this ) );
			if ( ! pBTFile )		// Out of Memory
				return FALSE;

			const CBENode* pFile = pFiles->GetNode( nFile );
			if ( ! pFile || ! pFile->IsType( CBENode::beDict ) ) return FALSE;

			const CBENode* pLength = pFile->GetNode( "length" );
			if ( ! pLength || ! pLength->IsType( CBENode::beInt ) ) return FALSE;
			pBTFile->m_nSize = (QWORD)pLength->GetInt();

			pBTFile->m_nOffset = nOffset;

			strPath.Empty();

			// Try path.utf8 if it's set  (Was Settings.BitTorrent.TorrentExtraKeys)
			const CBENode* pPath = pFile->GetNode( "path.utf-8" );
			if ( pPath )
			{
				if ( pPath->IsType( CBENode::beList ) && pPath->GetCount() > 32 )
				{
					CBENode* pPart = pPath->GetNode( 0 );
					if ( pPart && pPart->IsType( CBENode::beString ) )
						strPath = pPart->GetString();
				}
			}

			// Get the regular path
			pPath = pFile->GetNode( "path" );

			if ( ! pPath ) return FALSE;
			if ( ! pPath->IsType( CBENode::beList ) ) return FALSE;

			const CBENode* pPathPart = pPath->GetNode( 0 );
			if ( pPathPart && pPathPart->IsType( CBENode::beString ) )
			{
				if ( ! IsValid( strPath ) )
				{
					strPath = pPathPart->GetString();	// Get the path
				}
				else
				{
					// Check the path matches the .utf path
					CString strCheck = pPathPart->GetString();
					if ( strPath != strCheck )
						m_bEncodingError = true;
					// Switch back to the UTF-8 path
					pPath = pFile->GetNode( "path.utf-8" );
				}
			}

			// If that didn't work, try decoding the path
			if ( ! IsValid( strPath ) )
			{
				// There was an error reading the path
				m_bEncodingError = true;
				// Open path node
				pPath = pFile->GetNode( "path" );
				if ( pPath )
				{
					const CBENode* pPart = pPath->GetNode( 0 );
					if ( pPart->IsType( CBENode::beString ) )
						strPath = pPart->DecodeString(m_nEncoding);
				}
			}

			if ( ! pPath ) return FALSE;
			if ( ! pPath->IsType( CBENode::beList ) ) return FALSE;
			if ( pPath->GetCount() > 32 ) return FALSE;
			if ( _tcsicmp( strPath.GetString() , _T("#ERROR#") ) == 0 ) return FALSE;

			pBTFile->m_sName = PathFindFileName( strPath );

			// Hack to prefix all
			pBTFile->m_sPath = SafeFilename( m_sName );

			for ( int nPath = 0 ; nPath < pPath->GetCount() ; nPath++ )
			{
				const CBENode* pPart = pPath->GetNode( nPath );
				if ( ! pPart || ! pPart->IsType( CBENode::beString ) ) return FALSE;

				if ( ! pBTFile->m_sPath.IsEmpty() )
					pBTFile->m_sPath += '\\';

				// Get the path

				// Check for encoding error
				if ( pPart->GetString().CompareNoCase( _T("#ERROR#") ) == 0 )
					strPath = SafeFilename( pPart->DecodeString( m_nEncoding ), true );
				else
					strPath = SafeFilename( pPart->GetString(), true );

				pBTFile->m_sPath += strPath;
			}

			if ( const CBENode* pSHA1 = pFile->GetNode( "sha1" ) )
			{
				if ( ! pSHA1->IsType( CBENode::beString ) ||
					   pSHA1->m_nValue != Hashes::Sha1Hash::byteCount ) return FALSE;
				pBTFile->m_oSHA1 =
					*static_cast< Hashes::Sha1Hash::RawStorage* >( pSHA1->m_pValue );
			}

			if ( const CBENode* pED2K = pFile->GetNode( "ed2k" ) )
			{
				if ( ! pED2K->IsType( CBENode::beString ) ||
					   pED2K->m_nValue != Hashes::Ed2kHash::byteCount ) return FALSE;
				pBTFile->m_oED2K =
					*static_cast< Hashes::Ed2kHash::RawStorage* >( pED2K->m_pValue );
			}

			if ( const CBENode* pMD5 = pFile->GetNode( "md5sum" ) )
			{
				if ( ! pMD5->IsType( CBENode::beString ) )
				{
					return FALSE;
				}
				else if ( pMD5->m_nValue == Hashes::Md5Hash::byteCount )
				{
					pBTFile->m_oMD5 =
						*static_cast< const Hashes::Md5Hash::RawStorage* >( pMD5->m_pValue );
				}
				else if ( pMD5->m_nValue == Hashes::Md5Hash::byteCount * 2 )
				{
					CStringA tmp;
					tmp.Append( (const char*)pMD5->m_pValue, (int)pMD5->m_nValue );
					pBTFile->m_oMD5.fromString( CA2W( tmp ) );
				}
				else
				{
					return FALSE;
				}
			}

			if ( const CBENode* pTiger = pFile->GetNode( "tiger" ) )
			{
				if ( ! pTiger->IsType( CBENode::beString ) ||
					   pTiger->m_nValue != Hashes::TigerHash::byteCount ) return FALSE;
				pBTFile->m_oTiger =
					*static_cast< Hashes::TigerHash::RawStorage* >( pTiger->m_pValue );
			}

			m_nSize += pBTFile->m_nSize;
			nOffset += pBTFile->m_nSize;

			m_pFiles.AddTail( pBTFile.Detach() );
		}

		if ( nFiles == 1 )
		{
			// Single file in a multi-file torrent

			// Reset the name
			m_sName = strPath;

			// Set data/file hashes (if they aren't)
			if ( m_pFiles.GetHead()->m_oSHA1 )
				m_oSHA1 = m_pFiles.GetHead()->m_oSHA1;
			else if ( m_oSHA1 )
				m_pFiles.GetHead()->m_oSHA1 = m_oSHA1;

			if ( m_pFiles.GetHead()->m_oED2K )
				m_oED2K = m_pFiles.GetHead()->m_oED2K;
			else if ( m_oED2K )
				m_pFiles.GetHead()->m_oED2K = m_oED2K;

			if ( m_pFiles.GetHead()->m_oMD5 )
				m_oMD5 = m_pFiles.GetHead()->m_oMD5;
			else if ( m_oMD5 )
				m_pFiles.GetHead()->m_oMD5 = m_oMD5;

			if ( m_pFiles.GetHead()->m_oTiger )
				m_oTiger = m_pFiles.GetHead()->m_oTiger;
			else if ( m_oTiger )
				m_pFiles.GetHead()->m_oTiger = m_oTiger;
		}
	}
	else
	{
		return FALSE;
	}

	if ( ( m_nSize + m_nBlockSize - 1 ) / m_nBlockSize != m_nBlockCount )
		return FALSE;

	if ( ! CheckFiles() ) return FALSE;

	CSHA oSHA = pInfo->GetSHA1();
	oSHA.GetHash( &m_oBTH[ 0 ] );
	m_oBTH.validate();

	if ( m_pSource.m_nLength > 0 && pInfo->m_nSize
		 && pInfo->m_nPosition + pInfo->m_nSize < m_pSource.m_nLength )
	{
		Hashes::BtHash oBTH;
		CSHA pBTH;
		pBTH.Add( &m_pSource.m_pBuffer[pInfo->m_nPosition], pInfo->m_nSize );
		pBTH.Finish();
		pBTH.GetHash( &oBTH[0] );

		if ( oBTH == m_oBTH )
		{
			m_nInfoStart = pInfo->m_nPosition;
			m_nInfoSize	 = pInfo->m_nSize;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo load torrent info from tree

BOOL CBTInfo::CheckFiles()
{
	for ( POSITION pos = m_pFiles.GetHeadPosition() ; pos ; )
	{
		CBTFile* pBTFile = m_pFiles.GetNext( pos );
		pBTFile->m_sPath.Trim();

		LPCTSTR pszPath = pBTFile->m_sPath;

		if ( pszPath == NULL || *pszPath == 0 ) return FALSE;
		if ( pszPath[1] == ':' ) return FALSE;
		if ( *pszPath == '\\' || *pszPath == '/' ) return FALSE;
		if ( _tcsstr( pszPath, _T("..\\") ) != NULL ) return FALSE;
		if ( _tcsstr( pszPath, _T("../") ) != NULL ) return FALSE;
	}

	return m_pFiles.GetCount() > 0;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo block testing

void CBTInfo::BeginBlockTest()
{
	ASSERT( IsAvailable() );
	ASSERT( m_pBlockBTH != NULL );

	m_pTestSHA1.Reset();
	m_nTestByte = 0;
}

void CBTInfo::AddToTest(LPCVOID pInput, DWORD nLength)
{
	if ( nLength == 0 ) return;

	ASSERT( IsAvailable() );
	ASSERT( m_pBlockBTH != NULL );
	ASSERT( m_nTestByte + nLength <= m_nBlockSize );

	m_pTestSHA1.Add( pInput, nLength );
	m_nTestByte += nLength;
}

BOOL CBTInfo::FinishBlockTest(DWORD nBlock)
{
	ASSERT( IsAvailable() );

	if ( m_pBlockBTH == NULL || nBlock >= m_nBlockCount )
		 return FALSE;

	Hashes::BtHash oBTH;
	m_pTestSHA1.Finish();
	m_pTestSHA1.GetHash( &oBTH[ 0 ] );
	oBTH.validate();

	return m_pBlockBTH[ nBlock ] == oBTH;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo tracker handling

void CBTInfo::SetTrackerAccess(DWORD tNow)
{
	// Check that there should be a tracker
	if ( m_oTrackers.IsEmpty() ) return;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	// Set the current tracker's access time
	m_oTrackers[ m_nTrackerIndex ].m_tLastAccess = tNow;
}

void CBTInfo::SetTrackerSucceeded(DWORD tNow)
{
	// Check that there should be a tracker
	if ( m_oTrackers.IsEmpty() ) return;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	// Set the current tracker's success time
	m_oTrackers[ m_nTrackerIndex ].m_tLastSuccess = tNow;

	// Reset the failure count
	m_oTrackers[ m_nTrackerIndex ].m_nFailures = 0;
}

void CBTInfo::SetTrackerRetry(DWORD tTime)
{
	// Check that there should be a tracker
	if ( ! HasTracker() ) return;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	// Set the current tracker's next allowable access attempt time
	m_oTrackers[ m_nTrackerIndex ].m_tNextTry = tTime;
}

void CBTInfo::SetTrackerNext(DWORD tTime)
{
	if ( m_oTrackers.IsEmpty() )
	{
		m_nTrackerMode = tNull;
		m_nTrackerIndex = -1;
		return;
	}

	if ( m_nTrackerMode == tNull || m_nTrackerMode == tSingle )
		return;

	// Make sure this is a multitracker torrent
	if ( m_oTrackers.GetCount() < 2 )
	{
		m_nTrackerMode = tSingle;
		m_nTrackerIndex = 0;
		return;
	}

	// Get current time
	if ( ! tTime )
		tTime = GetTickCount();

	// Set current mode to searching
	m_nTrackerMode = tMultiFinding;

	// Start with the first tracker in the list
	m_nTrackerIndex = 0;

	// Search through the list for an available tracker, or the first one that will become available
	for ( int nTracker = 0 ; nTracker < m_oTrackers.GetCount() ; ++nTracker )
	{
		if ( m_oTrackers[ nTracker ].m_sAddress.GetAt( 0 ) == BAD_TRACKER_TOKEN )	// *https://
			continue;	//break;	// Reached bad trackers displayed at end of list (but user-added may follow)

		// Get the next tracker in the list
		CBTTracker& oTracker = m_oTrackers[ nTracker ];

		// If it's available, reset the retry time
		if ( oTracker.m_tNextTry < tTime )
			oTracker.m_tNextTry = 0;

		// If this tracker will become available before the current one, make it the current tracker
		if ( m_oTrackers[ m_nTrackerIndex ].m_tNextTry > oTracker.m_tNextTry )
			m_nTrackerIndex = nTracker;
	}
}

CString CBTInfo::GetTrackerAddress(int nTrackerIndex) const
{
	if ( m_oTrackers.IsEmpty() )
		return CString();

	if ( nTrackerIndex == -1 )
		nTrackerIndex = m_nTrackerIndex;

	if ( nTrackerIndex == -1 )
		return CString();

	ASSERT( nTrackerIndex >= 0 && nTrackerIndex < m_oTrackers.GetCount() );

	return m_oTrackers[ nTrackerIndex ].m_sAddress;
}

TRISTATE CBTInfo::GetTrackerStatus(int nTrackerIndex) const
{
	if ( m_oTrackers.IsEmpty() )
		return TRI_UNKNOWN;

	if ( nTrackerIndex == -1 )
		nTrackerIndex = m_nTrackerIndex;

	if ( nTrackerIndex == -1 )
		return TRI_UNKNOWN;

	ASSERT( nTrackerIndex >= 0 && nTrackerIndex < m_oTrackers.GetCount() );

	if ( ! m_oTrackers[ nTrackerIndex ].m_tNextTry &&
		 ! m_oTrackers[ nTrackerIndex ].m_tLastSuccess )
		return TRI_UNKNOWN;

	if ( m_oTrackers[ nTrackerIndex ].m_tNextTry >
		m_oTrackers[ nTrackerIndex ].m_tLastSuccess )
		return TRI_FALSE;

	return TRI_TRUE;
}

int CBTInfo::GetTrackerTier(int nTrackerIndex) const
{
	if ( m_oTrackers.IsEmpty() )
		return 0;

	if ( nTrackerIndex == -1 )
		nTrackerIndex = m_nTrackerIndex;

	if ( nTrackerIndex == -1 )
		return 0;

	ASSERT( nTrackerIndex >= 0 && nTrackerIndex < m_oTrackers.GetCount() );

	return m_oTrackers[ nTrackerIndex ].m_nTier;
}

DWORD CBTInfo::GetTrackerNextTry() const
{
	if ( ! HasTracker() )
		return (DWORD)-1;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	return m_oTrackers[ m_nTrackerIndex ].m_tNextTry;
}

DWORD CBTInfo::GetTrackerFailures() const
{
	if ( ! HasTracker() )
		return 0;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	// Return the # of failures
	return m_oTrackers[ m_nTrackerIndex ].m_nFailures;
}

void CBTInfo::OnTrackerFailure()
{
	if ( ! HasTracker() )
		return;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	m_oTrackers[ m_nTrackerIndex ].m_nFailures++;
}

void CBTInfo::SetTracker(const CString& sTracker)
{
	m_nTrackerIndex = AddTracker( CBTTracker( sTracker ) );
}

void CBTInfo::SetNode(const CString& sNode)
{
	m_oNodes.AddTail( sNode );
}

void CBTInfo::SetTrackerMode(int nTrackerMode)
{
	// Check it's valid
	INT_PTR nCount = m_oTrackers.GetCount();
	if ( ( nTrackerMode == CBTInfo::tMultiFound		&& nCount > 1 ) ||
		 ( nTrackerMode == CBTInfo::tMultiFinding	&& nCount > 1 ) ||
		 ( nTrackerMode == CBTInfo::tSingle			&& nCount > 0 ) ||
		   nTrackerMode == CBTInfo::tNull )
	{
		m_nTrackerMode = nTrackerMode;

		if ( nTrackerMode == CBTInfo::tNull )
			m_nTrackerIndex = -1;
		else if ( m_nTrackerIndex == -1 )
			SetTrackerNext();
	}
}

int CBTInfo::AddTracker(const CBTTracker& oTracker)
{
	for ( int i = 0 ; i < (int)m_oTrackers.GetCount() ; ++i )
	{
		if ( m_oTrackers[ i ] == oTracker )
			return i;	// Already have
	}

	return (int)m_oTrackers.Add( oTracker );
}

void CBTInfo::RemoveAllTrackers()
{
	m_nTrackerIndex = -1;
	m_nTrackerMode = CBTInfo::tNull;
	m_oTrackers.RemoveAll();
}

BOOL CBTInfo::ScrapeTracker()
{
	if ( m_tTrackerScrape )
	{
		// Support rare min_request_interval flag,  Low default throttle is enough in practice
		if ( ( GetTickCount() - m_tTrackerScrape ) < m_nTrackerWait )
			return ( m_nTrackerSeeds > 0 || m_nTrackerPeers > 0 );
	}

	m_tTrackerScrape = GetTickCount();

	CString strURL = GetTrackerAddress();
	if ( strURL.Find( _T("http") ) != 0 )
		return FALSE;	// ToDo: Support UDP Tracker scrape & handle rare HTTPS

	if ( strURL.Replace( _T("/announce"), _T("/scrape") ) != 1 )
		return FALSE;

	// Fetch scrape only for the given info hash
	strURL = strURL.TrimRight( _T('&') ) + ( ( strURL.Find( _T('?') ) != -1 ) ? _T('&') : _T('?') ) + _T("info_hash=");

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 500 ) ) return FALSE;

	strURL += CBTTrackerRequest::Escape( m_oBTH );
		// + _T("&peer_id=") + CBTTrackerRequest::Escape( pDownload.m_pPeerID ); 	// ToDo: Is this needed?

	oLock.Unlock();

	CHttpRequest pRequest;
	pRequest.SetURL( strURL );
	pRequest.AddHeader( _T("Accept-Encoding"), _T("deflate, gzip") );
	pRequest.EnableCookie( false );
	pRequest.SetUserAgent( Settings.SmartAgent() );

	// Wait for thread
	if ( ! pRequest.Execute( FALSE ) || ! pRequest.InflateResponse() )
		return FALSE;

	CBuffer* pResponse = pRequest.GetResponseBuffer();
	if ( pResponse == NULL || pResponse->m_pBuffer == NULL )
		return FALSE;

	if ( CBENode* pNode = CBENode::Decode( pResponse ) )
	{
		theApp.Message( MSG_DEBUG | MSG_FACILITY_INCOMING, _T("[BT] Recieved BitTorrent tracker response: %s"), pNode->Encode() );

		if ( ! oLock.Lock( 300 ) ) return FALSE;

		LPBYTE nKey = &m_oBTH[ 0 ];

		oLock.Unlock();

		CBENode* pFiles = pNode->GetNode( "files" );
		CBENode* pFile = pFiles->GetNode( nKey, Hashes::BtHash::byteCount );
		if ( ! pFile->IsType( CBENode::beDict ) ) return FALSE;

		if ( CBENode* pSeeds = pFile->GetNode( "complete" ) )
		{
			if ( pSeeds->IsType( CBENode::beInt ) )
				m_nTrackerSeeds = (int)( pSeeds->GetInt() & ~0xFFFF0000 );	// QWORD Caution: Don't get negative values from buggy trackers
		}

		if ( CBENode* pPeers = pFile->GetNode( "incomplete" ) )
		{
			if ( pPeers->IsType( CBENode::beInt ) )
				m_nTrackerPeers = (int)( pPeers->GetInt() & ~0xFFFF0000 );
		}

		//if ( CBENode* pHistory = pFile->GetNode( "downloaded" ) )			// ToDo: Use stat of all completed downloads ?
		//{
		//	if ( pHistory->IsType( CBENode::beInt ) )
		//		m_nTrackerHistory = (int)( pHistory->GetInt() & ~0xFFFF0000 );
		//}

		// Unofficial min_request_interval
		if ( m_nTrackerWait < 200 * 1000 )
		{
			if ( CBENode* pFlags = pNode->GetNode( "flags" ) )
			{
				if ( CBENode* pWait = pFlags->GetNode( "min_request_interval" ) )
				{
					if ( pWait->IsType( CBENode::beInt ) )
						m_nTrackerWait = pWait->GetInt() * 1000;

					if ( m_nTrackerWait < 60 * 1000 )
						m_nTrackerWait = 90 * 1000;
					else if ( m_nTrackerWait > 6 * 60 * 60 * 1000 )
						m_nTrackerWait = 30 * 60 * 1000;
				}
			}
		}

		delete pNode;
	}

	return ( m_nTrackerSeeds > 0 || m_nTrackerPeers > 0 );
}

CString CBTInfo::GetTrackerHash() const
{
	// Produce encoded tracker list for LT_TEX extension.
	CStringA sAddress;

	// Get concatenated tracker URLs list sorted in alphabetical order
	string_set oAddr;
	const int nCount = (int)m_oTrackers.GetCount();
	for ( int i = 0 ; i < nCount ; ++i )
	{
		oAddr.insert( m_oTrackers[ i ].m_sAddress );
	}
	for ( string_set::const_iterator i = oAddr.begin() ; i != oAddr.end() ; i++ )
	{
		sAddress += CT2A( (*i) );
	}

	// Get SHA1 of it
	CSHA oSHA;
	oSHA.Add( (LPCSTR)sAddress, sAddress.GetLength() );
	oSHA.Finish();

	Hashes::Sha1Hash oSHA1;
	oSHA.GetHash( &oSHA1[ 0 ] );
	oSHA1.validate();

	// Return hex-encoded hash
	return oSHA1.toString< Hashes::base16Encoding >();
}


//////////////////////////////////////////////////////////////////////
// CBTInfo::CBTTracker construction

CBTInfo::CBTTracker::CBTTracker(LPCTSTR szAddress, INT nTier)
	: m_sAddress		( szAddress ? szAddress : _T("") )
	, m_tLastAccess		( 0 )
	, m_tLastSuccess	( 0 )
	, m_tNextTry		( 0 )
	, m_nFailures		( 0 )
	, m_nType			( 0 )
	, m_nTier			( nTier )
{
}

CBTInfo::CBTTracker::CBTTracker(const CBTTracker& oSource)
	: m_sAddress		( oSource.m_sAddress )
	, m_tLastAccess		( oSource.m_tLastAccess )
	, m_tLastSuccess	( oSource.m_tLastSuccess )
	, m_tNextTry		( oSource.m_tNextTry )
	, m_nFailures		( oSource.m_nFailures )
	, m_nType			( oSource.m_nType )
	, m_nTier			( oSource.m_nTier )
{
}

CBTInfo::CBTTracker& CBTInfo::CBTTracker::operator=(const CBTTracker& oSource)
{
	m_sAddress		= oSource.m_sAddress;
	m_tLastAccess	= oSource.m_tLastAccess;
	m_tLastSuccess	= oSource.m_tLastSuccess;
	m_tNextTry		= oSource.m_tNextTry;
	m_nFailures		= oSource.m_nFailures;
	m_nType			= oSource.m_nType;
	m_nTier 		= oSource.m_nTier;

	return *this;
}

bool CBTInfo::CBTTracker::operator==(const CBTTracker& oSource)
{
	return ( m_sAddress == oSource.m_sAddress );
}


//////////////////////////////////////////////////////////////////////
// CBTInfo::CBTTracker serialize

void CBTInfo::CBTTracker::Serialize(CArchive& ar, int /*nVersion*/)
{
	if ( ar.IsStoring() )
	{
		ar << m_sAddress;
		ar << m_tLastAccess;
		ar << m_tLastSuccess;
		ar << m_tNextTry;
		ar << m_nFailures;
		ar << m_nTier;
		ar << m_nType;
	}
	else // Loading
	{
		ar >> m_sAddress;
		ar >> m_tLastAccess;
		ar >> m_tLastSuccess;
		ar >> m_tNextTry;
		ar >> m_nFailures;
		ar >> m_nTier;
		ar >> m_nType;
	}
}
