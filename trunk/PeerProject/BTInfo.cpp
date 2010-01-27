//
// BTInfo.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "BTInfo.h"
#include "BENode.h"
#include "Buffer.h"
#include "DownloadTask.h"
#include "Download.h"
#include "Downloads.h"
#include "FragmentedFile.h"
#include "Transfers.h"
#include "SharedFile.h"
#include "SharedFolder.h"
#include "Library.h"
#include "DlgProgressBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
	, m_nTrackerIndex	( -1 )
	, m_nTrackerMode	( tNull )
	, m_nTestByte		( 0ul )
	, m_tCreationDate	( 0ul )
	, m_bPrivate		( FALSE )
	, m_nStartDownloads	( dtAlways )
	, m_bEncodingError	( false )
	, m_nEncoding		( Settings.BitTorrent.TorrentCodePage )
{
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
	, m_tCreationDate	( 0ul )
	, m_bPrivate		( FALSE )
	, m_nStartDownloads	( dtAlways )
	, m_bEncodingError	( false )
	, m_nEncoding		( Settings.BitTorrent.TorrentCodePage )
{
	*this = oSource;
}

CBTInfo::~CBTInfo()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CBTFile construction

CBTInfo::CBTFile::CBTFile(const CBTInfo* pInfo, const CBTFile* pBTFile) :
	m_pInfo				( pInfo )
,	m_nOffset			( pBTFile ? pBTFile->m_nOffset : 0 )
{
	if ( pBTFile )
		CPeerProjectFile::operator=( *pBTFile );
}

CString	CBTInfo::CBTFile::FindFile()
{
	CSingleLock oLibraryLock( &Library.m_pSection, TRUE );

	// Try find file by hash/size
	CString strFile;
	CLibraryFile* pShared = LibraryMaps.LookupFileByHash( this, FALSE, TRUE );
	if ( pShared )
		strFile = pShared->GetPath();
	if ( ! pShared ||
		 GetFileSize( CString( _T("\\\\?\\") ) + strFile ) != m_nSize )
	{
		// Try complete folder
		strFile = Settings.Downloads.CompletePath + _T("\\") + m_sPath;
		if ( GetFileSize( CString( _T("\\\\?\\") ) + strFile ) != m_nSize )
		{
			// Try folder of original .torrent
			CString strTorrentPath = m_pInfo->m_sPath.Left(
				m_pInfo->m_sPath.ReverseFind( _T('\\') ) + 1 );
			strFile = strTorrentPath + m_sPath;
			if ( GetFileSize( CString( _T("\\\\?\\") ) + strFile ) != m_nSize )
			{
				// Try complete folder without outer file directory
				CString strShortPath;
				int nSlash = m_sPath.Find( _T('\\') );
				if ( nSlash != -1 )
					strShortPath = m_sPath.Mid( nSlash + 1 );
				strFile = Settings.Downloads.CompletePath + _T("\\") + strShortPath;
				if ( strShortPath.IsEmpty() ||
					 GetFileSize( CString( _T("\\\\?\\") ) + strFile ) != m_nSize )
				{
					// Try folder of original .torrent without outer file directory
					strFile = strTorrentPath + strShortPath;
					if ( strShortPath.IsEmpty() ||
						GetFileSize( CString( _T("\\\\?\\") ) + strFile ) != m_nSize )
					{
						// Try find by name only
						pShared = LibraryMaps.LookupFileByName( m_sName, m_nSize, FALSE, TRUE );
						if ( pShared )
							strFile = pShared->GetPath();
						if ( ! pShared || GetFileSize( CString( _T("\\\\?\\") ) + strFile ) != m_nSize )
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

	for ( POSITION pos = m_pFiles.GetHeadPosition(); pos; )
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
	for ( POSITION pos = oSource.m_sURLs.GetHeadPosition(); pos; )
		m_sURLs.AddTail( oSource.m_sURLs.GetNext( pos ) );

	m_nBlockSize		= oSource.m_nBlockSize;
	m_nBlockCount		= oSource.m_nBlockCount;

	if ( oSource.m_pBlockBTH )
	{
		m_pBlockBTH = new Hashes::BtPureHash[ m_nBlockCount ];
		std::copy( oSource.m_pBlockBTH, oSource.m_pBlockBTH + m_nBlockCount, m_pBlockBTH );
	}

	m_nTotalUpload		= oSource.m_nTotalUpload;
	m_nTotalDownload	= oSource.m_nTotalDownload;

	for ( POSITION pos = oSource.m_pFiles.GetHeadPosition(); pos; )
		m_pFiles.AddTail( new CBTFile( this, oSource.m_pFiles.GetNext( pos ) ) );

	m_nEncoding			= oSource.m_nEncoding;
	m_sComment			= oSource.m_sComment;
	m_tCreationDate		= oSource.m_tCreationDate;
	m_sCreatedBy		= oSource.m_sCreatedBy;
	m_bPrivate			= oSource.m_bPrivate;
	m_nStartDownloads	= oSource.m_nStartDownloads;

	m_oTrackers.RemoveAll();
	for ( INT_PTR i = 0; i < oSource.m_oTrackers.GetCount(); ++i )
		m_oTrackers.Add( oSource.m_oTrackers[ i ] );

	m_nTrackerIndex		= oSource.m_nTrackerIndex;
	m_nTrackerMode		= oSource.m_nTrackerMode;
	m_bEncodingError	= oSource.m_bEncodingError;
	m_pTestSHA1			= oSource.m_pTestSHA1;
	m_nTestByte			= oSource.m_nTestByte;

	m_pSource.Clear();
	m_pSource.Add( oSource.m_pSource.m_pBuffer, oSource.m_pSource.m_nLength );

	return *this;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo serialize

#define BTINFO_SER_VERSION	1000	//9
// nVersion History:
// 7 - redesigned tracker list (ryo-oh-ki)
// 8 - removed m_nFilePriority (ryo-oh-ki)
// 9 - added m_sName (ryo-oh-ki)
// 1000 - (PeerProject 1.0) (9)

void CBTInfo::Serialize(CArchive& ar)
{
	int nVersion = BTINFO_SER_VERSION;

	if ( ar.IsStoring() )
	{
		ar << nVersion;

		SerializeOut( ar, m_oBTH );
		if ( !m_oBTH ) return;

		ar << m_nSize;
		ar << m_nBlockSize;
		ar << m_nBlockCount;
		for ( DWORD i = 0; i < m_nBlockCount; ++i )
		{
			ar.Write( m_pBlockBTH[ i ].begin(), Hashes::BtPureHash::byteCount );
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
		for ( POSITION pos = m_pFiles.GetHeadPosition(); pos ; )
			m_pFiles.GetNext( pos )->Serialize( ar, nVersion );

		ar << m_nTrackerIndex;
		ar << m_nTrackerMode;

		int nTrackers = (int)m_oTrackers.GetCount();
		ar.WriteCount( nTrackers );
		for ( int nTracker = 0 ; nTracker < nTrackers ; nTracker++ )
		{
			m_oTrackers[ nTracker ].Serialize( ar, nVersion );
		}
	}
	else
	{
		// ToDo: What BTINFO_SER_VERSION nVersions are necessary for PeerProject? (Imports)

		ar >> nVersion;
		if ( nVersion < 5 ) AfxThrowUserException();

		SerializeIn( ar, m_oBTH, nVersion );
		if ( !m_oBTH ) return;

		ar >> m_nSize;
		ar >> m_nBlockSize;
		ar >> m_nBlockCount;

		m_pBlockBTH = new Hashes::BtPureHash[ (DWORD)m_nBlockCount ];
		for ( DWORD i = 0; i < m_nBlockCount; ++i )
		{
			ReadArchive( ar, m_pBlockBTH[ i ].begin(), Hashes::BtPureHash::byteCount );
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

		if ( nVersion < 7 )	// Shareaza 2.3 Import?
		{
			CString sTracker;
			ar >> sTracker;
			SetTracker( sTracker );
		}

		ar >> m_nTrackerIndex;
		ar >> m_nTrackerMode;

		if ( nVersion < 7 )	// Shareaza 2.3 Import?
		{
			int nTrackers = (int)ar.ReadCount();
			if ( nTrackers )
			{
				CBTTracker oTracker;
				oTracker.Serialize( ar, nVersion );
				AddTracker( oTracker );
			}
		}

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

		SetTrackerNext();

		//Imported Partial from Shareaza 2.4
		if ( nVersion < 8 )
			ConvertOldTorrents();
	}
}

void CBTInfo::ConvertOldTorrents()
{
	//For Importing Shareaza 2.4 Multifile Partial Only

	if ( m_pFiles.GetCount() < 2 )
		return;

	if ( !Downloads.IsSpaceAvailable( m_nSize, Downloads.dlPathComplete ) )
		AfxThrowFileException( CFileException::diskFull );

	CString strSource;
	strSource.Format( _T("\\\\?\\%s\\%s.partial"),
		Settings.Downloads.IncompletePath, GetFilename());

	if ( GetFileAttributes( strSource ) == INVALID_FILE_ATTRIBUTES )
		return;

	CFile oSource( strSource,
		CFile::modeRead | CFile::osSequentialScan | CFile::shareDenyNone);

	const DWORD BUFFER_SIZE = 8ul * 1024ul * 1024ul;
	BYTE* pBuffer = new BYTE[ BUFFER_SIZE ];
	if ( !pBuffer )
		AfxThrowMemoryException();

	CString strTargetTemplate;
	strTargetTemplate.Format( _T("\\\\?\\%s\\%s"),
		Settings.Downloads.IncompletePath, GetFilename() );

	CString strText;
	CProgressBarDlg oProgress( CWnd::GetDesktopWindow() );
	strText.LoadString( IDS_BT_UPDATE_TITLE );
	oProgress.SetWindowText( strText );
	strText.LoadString( IDS_BT_UPDATE_CONVERTING );
	oProgress.SetActionText( strText );
	oProgress.SetEventText( m_sName );
	oProgress.SetEventRange( 0, int( m_nSize / 1024ull ) );
	oProgress.CenterWindow();
	oProgress.ShowWindow( SW_SHOW );
	oProgress.UpdateWindow();
	oProgress.UpdateData( FALSE );

	CString strOf;
	strOf.LoadString( IDS_GENERAL_OF );
	QWORD nTotal = 0ull;
	DWORD nCount = 0ul;
	for ( POSITION pos = m_pFiles.GetHeadPosition() ; pos ; ++nCount )
	{
		CBTFile* pFile = m_pFiles.GetNext( pos );

		CString strTarget;
		strTarget.Format( _T("%s_%lu.partial"), strTargetTemplate, nCount );

		CFile oTarget( strTarget,
			CFile::modeCreate | CFile::modeWrite | CFile::osSequentialScan );

		strText.Format( _T("%lu %s %i"), nCount + 1ul, strOf,
			m_pFiles.GetCount() );

		if ( Settings.General.LanguageRTL )
			strText = _T("\x202B") + strText;

		oProgress.SetSubActionText( strText );
		oProgress.SetSubEventText( pFile->m_sPath );
		oProgress.SetSubEventRange( 0, int( pFile->m_nSize / 1024ull ) );
		oProgress.UpdateData( FALSE);
		oProgress.UpdateWindow();

		QWORD nLength = pFile->m_nSize;
		while ( nLength )
		{
			DWORD nBuffer = min( nLength, BUFFER_SIZE );

			nBuffer = oSource.Read( pBuffer, nBuffer );
			if ( nBuffer )
				oTarget.Write( pBuffer, nBuffer );

			nLength -= nBuffer;
			nTotal += nBuffer;

			oProgress.StepSubEvent( int( nBuffer / 1024ul ) );
			oProgress.SetEventPos( int( nTotal / 1024ull ) );
			oProgress.UpdateWindow();
		}
	}
	ASSERT( nTotal == m_nSize );

	delete [] pBuffer;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo::CBTFile serialize

void CBTInfo::CBTFile::Serialize(CArchive& ar, int nVersion)
{
	// ToDo: What BTINFO_SER_VERSION nVersions are necessary for PeerProject? (Imports)

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
	else
	{
		ar >> m_nSize;
		ar >> m_sPath;

		if ( nVersion >= 9 )
			ar >> m_sName;
		else // Upgrade Shareaza Import
			m_sName = PathFindFileName( m_sPath );

		SerializeIn( ar, m_oSHA1, nVersion );

	//	if ( nVersion >= 4 )
	//	{
			SerializeIn( ar, m_oED2K, nVersion );
			SerializeIn( ar, m_oTiger, nVersion );
			//if ( nVersion < 8 )
			//{
			//	int nFilePriority;
			//	ar >> nFilePriority;
			//}
	//	}

	//	if ( nVersion >= 6 )
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

BOOL CBTInfo::SaveTorrentFile(LPCTSTR pszPath)
{
	ASSERT( pszPath != NULL );
	if ( ! IsAvailable() ) return FALSE;
	if ( m_pSource.m_nLength == 0 ) return FALSE;

	CString strPath;
	strPath.Format( _T("%s\\%s.torrent"), pszPath,
		(LPCTSTR)CDownloadTask::SafeFilename( m_sName ) );

	CFile pFile;
	if ( ! pFile.Open( strPath, CFile::modeWrite | CFile::modeCreate ) )
		return FALSE;

	pFile.Write( m_pSource.m_pBuffer, m_pSource.m_nLength );
	pFile.Close();

	return TRUE;
}

#define MAX_PIECE_SIZE (16 * 1024)
BOOL CBTInfo::LoadInfoPiece(DWORD nInfoSize, DWORD nInfoPiece, BYTE *pPacketBuffer, DWORD nPacketLength)
{
	if ( m_pSource.m_nLength == 0 && nInfoPiece == 0 )
	{
		m_pSource.Add( "d", 1 );
		if ( GetTrackerCount() > 0 )
		{
			//Create .torrent file with tracker if needed

			CString sAddress = GetTrackerAddress();
			if ( sAddress.GetLength() > 0 )
			{
				sAddress.Format( _T("8:announce%d:%s" ), sAddress.GetLength(), sAddress );
				CStringA sAnnounce = UTF8Encode( sAddress );
				m_pSource.Add( sAnnounce.GetBuffer(), sAnnounce.GetLength() );
				sAnnounce.ReleaseBuffer();
			}

			if ( GetTrackerCount() > 1 )
			{
				m_pSource.Add( "13:announce-listll", 18 );
				for ( int i = 0; i < GetTrackerCount(); i++ )
				{
					CString sAddress = GetTrackerAddress( i );
					if ( sAddress.GetLength() > 0 )
					{
						sAddress.Format( _T("%d:%s"), sAddress.GetLength(), sAddress );
						CStringA sAnnounce = UTF8Encode( sAddress );
						m_pSource.Add( sAnnounce.GetBuffer(), sAnnounce.GetLength() );
						sAnnounce.ReleaseBuffer();
					}
				}
				m_pSource.Add( "ee", 2 );
			}
		}

		m_pSource.Add("d4:info", 6);
		m_nInfoSize = nInfoSize;
		m_nInfoStart = m_pSource.m_nLength;
	}

	ASSERT( m_nInfoSize == nInfoSize );

	QWORD nPieceStart = nInfoPiece * MAX_PIECE_SIZE;
	QWORD nPieceSize = m_nInfoSize - nPieceStart;
	if ( nPieceSize > MAX_PIECE_SIZE ) nPieceSize = MAX_PIECE_SIZE;

	if ( nPieceStart == ( m_pSource.m_nLength - m_nInfoStart ) && nPacketLength > nPieceSize )
	{
		m_pSource.Add( &pPacketBuffer[ nPacketLength - nPieceSize ], nPieceSize );

		if ( m_pSource.m_nLength - m_nInfoStart == m_nInfoSize )
		{
			m_pSource.Add( "e", 1 );
			return LoadTorrentBuffer( &m_pSource );
		}
	}
	return FALSE;
}

int CBTInfo::NextInfoPiece()
{
	if ( m_pSource.m_nLength == 0 )
		return 0;
	else if ( m_nInfoSize > ( m_pSource.m_nLength - m_nInfoStart ) )
		return ( m_pSource.m_nLength - m_nInfoStart ) / MAX_PIECE_SIZE;

	return -1;
}

DWORD CBTInfo::GetInfoPiece(DWORD nPiece, BYTE *pInfoPiece)
{
	DWORD nPiceStart = MAX_PIECE_SIZE * nPiece;
	if ( m_nInfoSize && m_nInfoStart &&
		m_pSource.m_nLength - m_nInfoStart > m_nInfoSize &&
		nPiceStart < m_nInfoSize )
	{
		pInfoPiece = &m_pSource.m_pBuffer[ m_nInfoStart + nPiceStart ];
		DWORD nPiceSize = m_nInfoSize - nPiceStart;
		return nPiceSize > MAX_PIECE_SIZE ? MAX_PIECE_SIZE : nPiceSize;
	}
	return 0;
}

DWORD CBTInfo::GetInfoSize()
{
	return m_nInfoSize;
}

BOOL CBTInfo::CheckInfoData(const CBuffer* pSource)
{
	ASSERT( pSource->m_nLength );

	DWORD nBlock = pSource->m_nLength;
	BYTE *pBuffer = pSource->m_pBuffer;

	BOOL bValidTorrent = TRUE;
	int nDetectInfo = 0;	// 0 - Nothing to do,  1 - DetectStart, 2 - DetectEnd, 3 - Info found
	char* sInfo = "info";
	DWORD nSkip = 0;		//Skip string value
	DWORD nInfoStart = 0;	//Start point of info in Block
	DWORD nInfoLen = 0;		//Len of info in Block
	void* pDigitsBuff = NULL;
	DWORD nDigitsBuff = 0;
	int nLevel = 0;			//Tree level. For Info must be 1 on start and 2 before end.
	int nDigitsStart = -1;	//Start of len digits for String

	if ( bValidTorrent )
	{
		nInfoStart = 0;
		nInfoLen = nBlock;
		for ( DWORD i=0; i < nBlock; i++ )
		{
			if ( nSkip > 0 )
			{
				if ( nDetectInfo == 1 && nSkip <= 4 && nLevel == 1 )
				{
					if ( (((char*)pBuffer)[ i ] ) == sInfo [ 4 - nSkip ] )
					{
						if ( nSkip == 1 )
						{
							nInfoStart = i + 1;
							nInfoLen = nBlock - nInfoStart;
							nDetectInfo = 2;
							if ( nInfoLen == 0 ) break;
						}
					}
					else
					{
						nDetectInfo = 0;
					}
					nSkip-=1;
					continue;
				}
				else if ( ( nBlock - i ) > nSkip )
				{
					i += nSkip;
					nSkip = 0;
				}
				else
				{
					nSkip -= ( nBlock - i );
					break;
				}
			}

			if ( ((char*)pBuffer)[i] == 'd' ||
				 ((char*)pBuffer)[i] == 'l' ||
				 ((char*)pBuffer)[i] == 'i' )
			{
				nLevel += 1;
			}
			else if ( ((char*)pBuffer)[i] == 'e' )
			{
				if ( nDetectInfo == 2 && nLevel == 2 )
				{
					nInfoLen = i - nInfoStart + 1;
					nDetectInfo = 3;
				}
				nDigitsStart = -1;
				nLevel -= 1;

				if (nLevel == 0) break; //end of main dictionary
			}
			else if ( (((char*)pBuffer)[i]) >= '0' && (((char*)pBuffer)[i]) <= '9' )
			{
				if ( nDigitsStart == -1 )
					nDigitsStart = i;
			}
			else if ( ((char*)pBuffer)[i] == ':' )
			{
				if ( nDigitsStart >= 0 )
				{
					//if ( nDigitsBuff > 0 && nDigitsStart == 0 && pDigitsBuff )
					//{
					//	DWORD nDigitsBuff2 = nDigitsBuff + i + 1;
					//	BYTE* pDigitsBuff2 = (BYTE*) malloc( nDigitsBuff2 );
					//	memcpy( pDigitsBuff2, pDigitsBuff, nDigitsBuff );
					//	memcpy( pDigitsBuff2 + nDigitsBuff, pBuffer, i + 1 );
					//	free( pDigitsBuff );
					//	pDigitsBuff = pDigitsBuff2;
					//	nDigitsBuff = nDigitsBuff2;
					//}
					//else
					//{
						nDigitsBuff = (i-nDigitsStart)+1;
						pDigitsBuff = malloc(nDigitsBuff);
						memcpy( pDigitsBuff,((BYTE*) pBuffer)+nDigitsStart, nDigitsBuff);
					//}
					((BYTE*) pDigitsBuff)[nDigitsBuff-1]=0;
					if ( sscanf( (LPCSTR)pDigitsBuff, "%i", &nSkip ) != 1 )
						bValidTorrent = FALSE;
					if ( nDetectInfo==0 && nLevel==1 && nSkip == 4 )
						nDetectInfo = 1;
					free( pDigitsBuff );
					nDigitsBuff = 0;
					nDigitsStart = -1;
				}
			}
			else
			{
				bValidTorrent = FALSE;
			}
		}
	}

	//if ( bValidTorrent && nDigitsStart >= 0 )
	//{
	//	DWORD nDigitsBuff = nBlock-nDigitsStart;
	//	BYTE* pDigitsBuff = (BYTE*)malloc( nDigitsBuff );
	//	memcpy( pDigitsBuff, ((BYTE*)pBuffer) + nDigitsStart, nDigitsBuff );
	//	nDigitsStart = 0;
	//}

	Hashes::BtHash oBTH;

	if ( bValidTorrent && nDetectInfo == 3 )
	{
		CSHA pBTH;
		pBTH.Add( &pBuffer[nInfoStart], nInfoLen );
		pBTH.Finish();
		pBTH.GetHash( &oBTH[0] );
		bValidTorrent = oBTH.validate();
	}
	else
		bValidTorrent = FALSE;

	if ( bValidTorrent && m_oBTH )
	{
		bValidTorrent = ( oBTH == m_oBTH );
	}

	if ( bValidTorrent && pSource == &m_pSource )
	{
		m_nInfoStart = nInfoStart;
		m_nInfoSize  = nInfoLen;
	}

	return bValidTorrent;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo load torrent info from buffer

BOOL CBTInfo::LoadTorrentBuffer(const CBuffer* pBuffer)
{
	if ( ! CheckInfoData( pBuffer ) )
		return FALSE;

	auto_ptr< CBENode > pNode ( CBENode::Decode( pBuffer ) );
	if ( ! pNode.get() )
		return FALSE;

	return LoadTorrentTree( pNode.get() );
}

//////////////////////////////////////////////////////////////////////
// CBTInfo load torrent info from tree

BOOL CBTInfo::LoadTorrentTree(const CBENode* pRoot)
{
	//ASSERT( m_sName.IsEmpty() && m_nSize == SIZE_UNKNOWN );	// Assume empty object
	ASSERT( !m_pBlockBTH );

	theApp.Message( MSG_DEBUG, _T("[BT] Loading torrent tree: %s"), (LPCTSTR)pRoot->Encode() );

	if ( ! pRoot->IsType( CBENode::beDict ) ) return FALSE;

	// Get the info node
	CBENode* pInfo = pRoot->GetNode( "info" );
	if ( ! pInfo || ! pInfo->IsType( CBENode::beDict ) ) return FALSE;

	if ( m_oBTH )
	{
		Hashes::BtHash oBTH;
		pInfo->GetBth( oBTH );
		if ( oBTH != m_oBTH ) return FALSE;
	}

	// Get the encoding (from torrents that have it)
	m_nEncoding = 0;
	CBENode* pEncoding = pRoot->GetNode( "codepage" );
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
			else if ( _tcsistr( strEncoding.GetString() , _T("UTF-8") ) != NULL ||
					  _tcsistr( strEncoding.GetString() , _T("UTF8") ) != NULL )
				m_nEncoding = CP_UTF8;
			else if ( _tcsistr( strEncoding.GetString() , _T("ANSI") ) != NULL )
				m_nEncoding = CP_ACP;
			else if ( _tcsistr( strEncoding.GetString() , _T("BIG5") ) != NULL )
				m_nEncoding = 950;
			else if ( _tcsistr( strEncoding.GetString() , _T("Korean") ) != NULL )
				m_nEncoding = 949;
			else if ( _tcsistr( strEncoding.GetString() , _T("UHC") ) != NULL )
				m_nEncoding = 949;
			else if ( _tcsistr( strEncoding.GetString() , _T("Chinese") ) != NULL )
				m_nEncoding = 936;
			else if ( _tcsistr( strEncoding.GetString() , _T("GB2312") ) != NULL )
				m_nEncoding = 936;
			else if ( _tcsistr( strEncoding.GetString() , _T("GBK") ) != NULL )
				m_nEncoding = 936;
			else if ( _tcsistr( strEncoding.GetString() , _T("Japanese") ) != NULL )
				m_nEncoding = 932;
			else if ( _tcsistr( strEncoding.GetString() , _T("Shift-JIS") ) != NULL )
				m_nEncoding = 932;
			else if ( _tcsnicmp( strEncoding.GetString() , _T("Windows-"), 8 ) == 0 )
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
	m_sComment = pRoot->GetStringFromSubNode( "comment", m_nEncoding, m_bEncodingError );

	// Get the creation date (if present)
	CBENode* pDate = pRoot->GetNode( "creation date" );
	if ( ( pDate ) &&  ( pDate->IsType( CBENode::beInt )  ) )
	{
		m_tCreationDate = (DWORD)pDate->GetInt();
		// CTime pTime( (time_t)m_tCreationDate );
		// theApp.Message( MSG_NOTICE, pTime.Format( _T("%Y-%m-%d %H:%M:%S") ) );
	}

	// Get the creator (if present)
	m_sCreatedBy = pRoot->GetStringFromSubNode( "created by", m_nEncoding, m_bEncodingError );

	// Get announce-list (if present)
	CBENode* pAnnounceList = pRoot->GetNode( "announce-list" );
	if ( ( pAnnounceList ) && ( pAnnounceList->IsType( CBENode::beList ) ) )
	{
		m_nTrackerMode = tMultiFinding;

		bool bUnsupported = false;	// UDP tracker display

		// Loop through all the tiers
		for ( int nTier = 0 ; nTier < pAnnounceList->GetCount() ; nTier++ )
		{
			CBENode* pSubList = pAnnounceList->GetNode( nTier );
			if ( ( pSubList ) && ( pSubList->IsType( CBENode::beList ) ) )
			{
				CList< CString > pTrackers;
				// Read in the trackers
				for ( int nTracker = 0 ; nTracker < pSubList->GetCount() ; nTracker++ )
				{
					CBENode* pTracker = pSubList->GetNode( nTracker );
					if ( pTracker && pTracker->IsType( CBENode::beString ) )
					{
						CString strTracker = pTracker->GetString();	// Get the tracker

						// Check tracker is valid
						if ( _tcsncicmp( (LPCTSTR)strTracker, _T("http://"), 7 ) == 0 )
							pTrackers.AddTail( strTracker );		// Store HTTP tracker
						else if ( _tcsncicmp( (LPCTSTR)strTracker, _T("udp://"), 6 ) == 0 )
							bUnsupported = true;		// Store below for display, ToDo: Add UDP tracker support
						else if ( _tcsncicmp( (LPCTSTR)strTracker, _T("https://"), 8 ) == 0 )
							bUnsupported = true;		// Store below, ToDo: Verify https or try converting to http?
						//else unknown tracker protocol
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
								CString strTemp;
								strTemp = pTrackers.GetAt( pos );
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
						CBTTracker oTracker;
						oTracker.m_sAddress	= pTrackers.GetNext( pos );
						oTracker.m_nTier = nTier;
						AddTracker( oTracker );
					}
					// Delete temporary storage
					pTrackers.RemoveAll();
				}
			}
		}

		// ToDo: Remove this when UDP trackers are supported!
		// Catch unsupported trackers for display at end of list.
		if ( bUnsupported == true )
		{
			// Loop through all tiers again
			for ( int nTier = 0 ; nTier < pAnnounceList->GetCount() ; nTier++ )
			{
				CBENode* pSubList = pAnnounceList->GetNode( nTier );
				if ( ( pSubList ) && ( pSubList->IsType( CBENode::beList ) ) )
				{
					CList< CString > pTrackers;
					for ( int nTracker = 0 ; nTracker < pSubList->GetCount() ; nTracker++ )
					{
						CBENode* pTracker = pSubList->GetNode( nTracker );
						if ( ( pTracker ) && ( pTracker->IsType( CBENode::beString ) ) )
						{
							CString strTracker = pTracker->GetString();

							if ( _tcsncicmp( (LPCTSTR)strTracker, _T("udp://"), 6 ) == 0 )
								pTrackers.AddTail( strTracker );		// Store for display, ToDo: Add UDP tracker support
							else if ( _tcsncicmp( (LPCTSTR)strTracker, _T("https://"), 8 ) == 0 )
								pTrackers.AddTail( strTracker );		// Rare for display, ToDo: Verify https or try converting to http?
						}
					}

					// Store unsupported trackers at end of list
					for ( POSITION pos = pTrackers.GetHeadPosition() ; pos ; )
					{
						CBTTracker oTracker;
						oTracker.m_sAddress	= pTrackers.GetNext( pos );
						oTracker.m_nTier = nTier + 10;
						AddTracker( oTracker );
					}
					// Delete temporary storage again
					pTrackers.RemoveAll();
				}
			}
		}

		SetTrackerNext();
	}

	// Announce node is ignored by multi-tracker torrents
	if ( m_oTrackers.IsEmpty() )
	{
		// Get announce
		CBENode* pAnnounce = pRoot->GetNode( "announce" );
		if ( pAnnounce && pAnnounce->IsType( CBENode::beString ) )
		{
			// Get the tracker
			CString strTracker = pAnnounce->GetString();

			// Store it if it's valid. (Some torrents have invalid trackers)
			if ( _tcsncicmp( (LPCTSTR)strTracker, _T("http://"), 7 ) == 0 )
			{
				// Set the torrent to be a single-tracker torrent
				m_nTrackerMode = tSingle;
				SetTracker( strTracker );

				// Backup defunct PirateBay tracker
				if ( strTracker.Find( _T("piratebay") ) > 6 )
				{
					CBTTracker oTracker;
					oTracker.m_sAddress	= _T("http://tracker.openbittorrent.com/announce");
					oTracker.m_nTier	= 0;
					m_nTrackerMode = tMultiFinding;
					AddTracker( oTracker );
				}
			}
			//else	// Torrents should always have a valid announce node, udp:// is unlikely.
		}
	}

	// Get the info node
	//CBENode* pInfo = pRoot->GetNode( "info" );
	//if ( ! pInfo || ! pInfo->IsType( CBENode::beDict ) ) return FALSE;

	// Get the private flag (if present)
	CBENode* pPrivate = pInfo->GetNode( "private" );
	if ( ( pPrivate ) &&  ( pPrivate->IsType( CBENode::beInt )  ) )
		m_bPrivate = pPrivate->GetInt() > 0;

	// Get the name
	m_sName = pInfo->GetStringFromSubNode( "name", m_nEncoding, m_bEncodingError );

	// If we still don't have a name, generate one
	if ( m_sName.IsEmpty() )
		m_sName.Format( _T("Unnamed_Torrent_%i"), GetRandomNum( 0i32, _I32_MAX ) );

	// Get the piece stuff
	CBENode* pPL = pInfo->GetNode( "piece length" );
	if ( ! pPL || ! pPL->IsType( CBENode::beInt ) ) return FALSE;
	m_nBlockSize = (DWORD)pPL->GetInt();
	if ( ! m_nBlockSize ) return FALSE;

	CBENode* pHash = pInfo->GetNode( "pieces" );
	if ( ! pHash || ! pHash->IsType( CBENode::beString ) ) return FALSE;
	if ( pHash->m_nValue % Hashes::Sha1Hash::byteCount ) return FALSE;
	m_nBlockCount = (DWORD)( pHash->m_nValue / Hashes::Sha1Hash::byteCount );
	if ( ! m_nBlockCount || m_nBlockCount > 209716 ) return FALSE;

	m_pBlockBTH = new Hashes::BtPureHash[ m_nBlockCount ];

	std::copy( static_cast< const Hashes::BtHash::RawStorage* >( pHash->m_pValue ),
		static_cast< const Hashes::BtHash::RawStorage* >( pHash->m_pValue ) + m_nBlockCount,
		m_pBlockBTH );

	// Hash info
	if ( CBENode* pSHA1 = pInfo->GetNode( "sha1" ) )
	{
		if ( ! pSHA1->IsType( CBENode::beString ) || pSHA1->m_nValue != Hashes::Sha1Hash::byteCount ) return FALSE;
		m_oSHA1 = *static_cast< const Hashes::BtHash::RawStorage* >( pSHA1->m_pValue );
	}
	else if ( CBENode* pSHA1Base16 = pInfo->GetNode( "filehash" ) )
	{
		if ( ! pSHA1Base16->IsType( CBENode::beString ) ||
			pSHA1Base16->m_nValue != Hashes::BtGuid::byteCount ) return FALSE;
		m_oSHA1 = *static_cast< const Hashes::BtGuid::RawStorage* >( pSHA1Base16->m_pValue );
	}

	if ( CBENode* pED2K = pInfo->GetNode( "ed2k" ) )
	{
		if ( ! pED2K->IsType( CBENode::beString ) || pED2K->m_nValue != Hashes::Ed2kHash::byteCount ) return FALSE;
		m_oED2K = *static_cast< const Hashes::Ed2kHash::RawStorage* >( pED2K->m_pValue );
	}

	if ( CBENode* pMD5 = pInfo->GetNode( "md5sum" ) )
	{
		if ( ! pMD5->IsType( CBENode::beString ) )
		{
			return FALSE;
		}
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

	if ( CBENode* pTiger = pInfo->GetNode( "tiger" ) )
	{
		if ( ! pTiger->IsType( CBENode::beString ) || pTiger->m_nValue != Hashes::TigerHash::byteCount ) return FALSE;
		m_oTiger = *static_cast< const Hashes::TigerHash::RawStorage* >( pTiger->m_pValue );
	}

	// Details on file (or files).
	if ( CBENode* pLength = pInfo->GetNode( "length" ) )
	{
		if ( ! pLength->IsType( CBENode::beInt ) )
			return FALSE;
		m_nSize = pLength->GetInt();
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
		CBENode* pSources = pRoot->GetNode( "sources" );
		if( pSources && pSources->IsType( CBENode::beList ) )
		{
			int m_nSources = pSources->GetCount();
			for( int nSource = 0 ; nSource < m_nSources; nSource++)
			{
				CBENode* pSource = pSources->GetNode( nSource );
				if( ! pSource || ! pSource->IsType(CBENode::beString) ) continue;
				m_sURLs.AddTail( pSource->GetString() );
			}
		}
	}
	else if ( CBENode* pFiles = pInfo->GetNode( "files" ) )
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

			CBENode* pFile = pFiles->GetNode( nFile );
			if ( ! pFile || ! pFile->IsType( CBENode::beDict ) ) return FALSE;

			CBENode* pLength = pFile->GetNode( "length" );
			if ( ! pLength || ! pLength->IsType( CBENode::beInt ) ) return FALSE;
			pBTFile->m_nSize = pLength->GetInt();

			pBTFile->m_nOffset = nOffset;

			strPath.Empty();
			CBENode* pPath;
			// Try path.utf8 if it's set
			if ( Settings.BitTorrent.TorrentExtraKeys )
			{
				pPath = pFile->GetNode( "path.utf-8" );
				if ( pPath )
				{
					if ( ! pPath->IsType( CBENode::beList ) ) return FALSE;
					if ( pPath->GetCount() > 32 ) return FALSE;
					CBENode* pPart = pPath->GetNode( 0 );
					if ( pPart && pPart->IsType( CBENode::beString ) ) strPath = pPart->GetString();
				}
			}

			// Get the regular path
			pPath = pFile->GetNode( "path" );

			if ( ! pPath ) return FALSE;
			if ( ! pPath->IsType( CBENode::beList ) ) return FALSE;

			CBENode* pPart = pPath->GetNode( 0 );
			if ( pPart && pPart->IsType( CBENode::beString ) )
			{
				if ( ! IsValid( strPath ) )
				{
					strPath = pPart->GetString();	// Get the path
				}
				else
				{
					// Check the path matches the .utf path
					CString strCheck =  pPart->GetString();
					if ( strPath != strCheck )
						m_bEncodingError = TRUE;
					// Switch back to the UTF-8 path
					pPath = pFile->GetNode( "path.utf-8" );
				}
			}

			// If that didn't work, try decoding the path
			if ( ( ! IsValid( strPath ) )  )
			{
				// There was an error reading the path
				m_bEncodingError = TRUE;
				// Open path node
				pPath = pFile->GetNode( "path" );
				if ( pPath )
				{
					CBENode* pPart = pPath->GetNode( 0 );
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
			pBTFile->m_sPath = CDownloadTask::SafeFilename( m_sName );

			for ( int nPath = 0 ; nPath < pPath->GetCount() ; nPath++ )
			{
				CBENode* pPart = pPath->GetNode( nPath );
				if ( ! pPart || ! pPart->IsType( CBENode::beString ) ) return FALSE;

				if ( pBTFile->m_sPath.GetLength() )
					pBTFile->m_sPath += '\\';

				// Get the path
				strPath = pPart->GetString();
				strPath = CDownloadTask::SafeFilename( pPart->GetString() );
				// Check for encoding error
				if ( _tcsicmp( strPath.GetString() , _T("#ERROR#") ) == 0 )
					strPath = CDownloadTask::SafeFilename( pPart->DecodeString( m_nEncoding ) );

				pBTFile->m_sPath += strPath;
			}

			if ( CBENode* pSHA1 = pFile->GetNode( "sha1" ) )
			{
				if ( ! pSHA1->IsType( CBENode::beString ) ||
					   pSHA1->m_nValue != Hashes::Sha1Hash::byteCount ) return FALSE;
				pBTFile->m_oSHA1 =
					*static_cast< Hashes::Sha1Hash::RawStorage* >( pSHA1->m_pValue );
			}

			if ( CBENode* pED2K = pFile->GetNode( "ed2k" ) )
			{
				if ( ! pED2K->IsType( CBENode::beString ) ||
					   pED2K->m_nValue != Hashes::Ed2kHash::byteCount ) return FALSE;
				pBTFile->m_oED2K =
					*static_cast< Hashes::Ed2kHash::RawStorage* >( pED2K->m_pValue );
			}

			if ( CBENode* pMD5 = pFile->GetNode( "md5sum" ) )
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

			if ( CBENode* pTiger = pFile->GetNode( "tiger" ) )
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

	pInfo->GetBth( m_oBTH );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTInfo load torrent info from tree

BOOL CBTInfo::CheckFiles()
{
	for ( POSITION pos = m_pFiles.GetHeadPosition(); pos ; )
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
	ASSERT( m_pBlockBTH != NULL );

	if ( nBlock >= m_nBlockCount ) return FALSE;

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
	if ( m_oTrackers.IsEmpty() )
		return;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	// Set the current tracker's access time
	m_oTrackers[ m_nTrackerIndex ].m_tLastAccess = tNow;
}

void CBTInfo::SetTrackerSucceeded(DWORD tNow)
{
	// Check that there should be a tracker
	if ( m_oTrackers.IsEmpty() )
		return;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	// Set the current tracker's success time
	m_oTrackers[ m_nTrackerIndex ].m_tLastSuccess = tNow;

	// Reset the failure count
	m_oTrackers[ m_nTrackerIndex ].m_nFailures = 0;
}

void CBTInfo::SetTrackerRetry(DWORD tTime)
{
	// Check that there should be a tracker
	if ( m_oTrackers.IsEmpty() )
		return;

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

	// Search through the list for an available tracker
	// or the first one that will become available
	for ( int nTracker = 0; nTracker < m_oTrackers.GetCount(); ++nTracker )
	{
		// Get the next tracker in the list
		CBTTracker& oTracker = m_oTrackers[ nTracker ];

		// If it's available, reset the retry time
		if ( oTracker.m_tNextTry < tTime )
			oTracker.m_tNextTry = 0;

		// If this tracker will become available before the current one,
		// make it the current tracker
		if ( m_oTrackers[ m_nTrackerIndex ].m_tNextTry > oTracker.m_tNextTry )
			m_nTrackerIndex = nTracker;
	}
}

DWORD CBTInfo::GetTrackerFailures() const
{
	if ( ! HasTracker() )
		return 0;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	// Return the # of failures
	return m_oTrackers[ m_nTrackerIndex ].m_nFailures;
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
	else if ( m_oTrackers[ nTrackerIndex ].m_tNextTry >
		m_oTrackers[ nTrackerIndex ].m_tLastSuccess )
		return TRI_FALSE;
	else
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

void CBTInfo::OnTrackerFailure()
{
	if ( ! HasTracker() )
		return;

	ASSERT( m_nTrackerIndex >= 0 && m_nTrackerIndex < m_oTrackers.GetCount() );

	m_oTrackers[ m_nTrackerIndex ].m_nFailures++;
}

void CBTInfo::SetTracker(const CString& sTracker)
{
	CBTTracker oTracker;
	oTracker.m_sAddress = sTracker;
	m_nTrackerIndex = AddTracker( oTracker );
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
	for ( int i = 0; i < (int)m_oTrackers.GetCount(); ++i )
		if ( m_oTrackers[ i ].m_sAddress == oTracker.m_sAddress )
			return i;	// Already have

	return (int)m_oTrackers.Add( oTracker );
}

//////////////////////////////////////////////////////////////////////
// CBTInfo::CBTTracker construction

CBTInfo::CBTTracker::CBTTracker() :
	m_tLastAccess		( 0 )
,	m_tLastSuccess		( 0 )
,	m_tNextTry			( 0 )
,	m_nFailures			( 0 )
,	m_nTier				( 0 )
,	m_nType				( 0 )
{
}

CBTInfo::CBTTracker::CBTTracker(const CBTTracker& oSource) :
	m_sAddress			( oSource.m_sAddress )
,	m_tLastAccess		( oSource.m_tLastAccess )
,	m_tLastSuccess		( oSource.m_tLastSuccess )
,	m_tNextTry			( oSource.m_tNextTry )
,	m_nFailures			( oSource.m_nFailures )
,	m_nTier				( oSource.m_nTier )
,	m_nType				( oSource.m_nType )
{
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
	else
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
