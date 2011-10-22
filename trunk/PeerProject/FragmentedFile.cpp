//
// FragmentedFile.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2007.
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
#include "FragmentedFile.h"
#include "TransferFile.h"
#include "BTInfo.h"
#include "Library.h"
#include "SharedFile.h"
#include "DlgSelect.h"
#include "Download.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE( CFragmentedFile, CObject )

CFragmentedFile::CVirtualFilePart::CVirtualFilePart()
	: m_pFile		( NULL )
	, m_nOffset 	( 0 )
	, m_bWrite		( FALSE )
	, m_nPriority	( CFragmentedFile::prNormal )
{
}

CFragmentedFile::CVirtualFilePart::CVirtualFilePart(const CVirtualFilePart& pFile)
	: CPeerProjectFile( pFile )
	, m_pFile		( pFile.m_pFile )
	, m_nOffset		( pFile.m_nOffset )
	, m_bWrite		( pFile.m_bWrite )
	, m_nPriority	( pFile.m_nPriority )
{
}

CFragmentedFile::CVirtualFilePart& CFragmentedFile::CVirtualFilePart::operator=(const CVirtualFilePart& pFile)
{
	CPeerProjectFile::operator=( pFile );
	m_pFile = pFile.m_pFile;
	m_nOffset = pFile.m_nOffset;
	m_bWrite = pFile.m_bWrite;
	m_nPriority = pFile.m_nPriority;
	return *this;
}

void CFragmentedFile::CVirtualFilePart::Release()
{
	if ( m_pFile )
	{
		m_pFile->Release();
		m_pFile = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile construction

CFragmentedFile::CFragmentedFile()
	: m_nUnflushed	( 0 )
	, m_oFList		( 0 )
	, m_nFileError	( ERROR_SUCCESS )
	, m_pDownload	( NULL )
	, m_dwRef		( 1 )
{
}

void CFragmentedFile::SetDownload(const CDownload* pDownload)
{
	m_pDownload = pDownload;
}

CFragmentedFile::~CFragmentedFile()
{
	ASSERT( m_dwRef == 0 );

	Close();
}

ULONG CFragmentedFile::AddRef()
{
	return (ULONG)InterlockedIncrement( &m_dwRef );
}

ULONG CFragmentedFile::Release()
{
	ULONG ref_count = (ULONG)InterlockedDecrement( &m_dwRef );
	if ( ref_count )
		return ref_count;
	delete this;
	return 0;
}

#ifdef _DEBUG

void CFragmentedFile::AssertValid() const
{
	CObject::AssertValid();

	if ( m_oFile.size() != 0 )
	{
		ASSERT( m_oFile.front().m_nOffset == 0 );
		CVirtualFile::const_iterator j;
		for ( CVirtualFile::const_iterator i = m_oFile.begin() ; i != m_oFile.end() ; ++i )
		{
			if ( i != m_oFile.begin() )
				ASSERT( (*j).m_nOffset + (*j).m_nSize == (*i).m_nOffset );
			j = i;
		}
	}
}

void CFragmentedFile::Dump(CDumpContext& dc) const
{
	CObject::Dump( dc );

	int n = 1;
	for ( CVirtualFile::const_iterator i = m_oFile.begin() ; i != m_oFile.end() ; ++i, ++n )
		dc << n << _T(". File offset ") << (*i).m_nOffset << _T(", ")
			<< (*i).m_nSize << _T(" bytes, ")
			<< ( (*i).m_bWrite ? _T("RW") : _T("RO") )
			<< _T(" \"") << (*i).m_sPath << _T("\"\n");
}

#endif // Debug

//////////////////////////////////////////////////////////////////////
// CFragmentedFile open

BOOL CFragmentedFile::Open(LPCTSTR pszFile, QWORD nOffset, QWORD nLength, BOOL bWrite, LPCTSTR pszName, int nPriority)
{
	if ( ! pszFile || ! *pszFile )
	{
		// Bad file name
		m_nFileError = ERROR_FILE_NOT_FOUND;
		return FALSE;
	}

	CQuickLock oLock( m_pSection );

	m_nFileError = ERROR_SUCCESS;

	CVirtualFile::iterator pItr = std::find( m_oFile.begin(), m_oFile.end(), pszFile );
	if ( pItr == m_oFile.end() )	// bNew
	{
		// Use new
		CVirtualFilePart part;
		part.m_sPath = pszFile;
		part.m_nOffset = nOffset;
		part.m_bWrite = bWrite;
		part.m_nPriority = nPriority;

		if ( pszName )
			part.m_sName = SafeFilename( pszName, true );

		m_oFile.push_back( part );
		pItr = --m_oFile.end();
	}
	else if ( (*pItr).m_pFile )		// Already opened
	{
		// Use existing
		return ! bWrite || (*pItr).m_pFile->EnsureWrite();
	}

	CTransferFile* pFile = NULL;
	QWORD nRealLength = SIZE_UNKNOWN;
	// Allow fallback loading methods, currently none available  (Previously checked non-default path)
	//for ( int nMethod = 0 ; nMethod < 2 ; ++nMethod )
	//{
		pFile = TransferFiles.Open( (*pItr).m_sPath, bWrite );
		if ( pFile )
		{
			m_nFileError = ERROR_SUCCESS;
			nRealLength = pFile->GetSize();
			if ( pFile->IsExists() && nLength == SIZE_UNKNOWN )
			{
				nLength = nRealLength;
			}
			else if ( ! pFile->IsWritable() && nRealLength != nLength )
			{
				// Wrong file
				pFile->Release();
				pFile = NULL;
				m_nFileError = ERROR_FILE_INVALID;
			}
			//break;
		}
		else
		{
			m_nFileError = ::GetLastError();
	//		if ( ! bWrite )
	//			break;	// Do nothing for read only files
		}

	//	CString strPath( pszFile );
	//	switch( nMethod )
	//	{
	//	case 0:
	//		// Redundant: Try to open file for write from current incomplete folder (in case of changed folder)
	//		(*pItr).m_sPath = Settings.Downloads.IncompletePath + strPath.Mid( strPath.ReverseFind( _T('\\') ) );
	//		break;
	//
	//	// ToDo: Other methods
	//	}
	//}

	(*pItr).m_nSize = nLength;
	(*pItr).m_pFile = pFile;

	std::sort( m_oFile.begin(), m_oFile.end(), Less() );

	// Set minimum size
	QWORD nLastBlockLength = m_oFile.back().m_nSize;
	m_oFList.ensure( ( nLastBlockLength == SIZE_UNKNOWN ) ? SIZE_UNKNOWN :
		( m_oFile.back().m_nOffset + nLastBlockLength ) );

	// Add empty fragment for new file
	if ( ! pFile || ! pFile->IsExists() || ( m_oFList.empty() && nRealLength != nLength ) )
		InvalidateRange( nOffset, nLength );

	ASSERT_VALID( this );

	return pFile && ( m_nFileError == ERROR_SUCCESS );
}

BOOL CFragmentedFile::Open(CPeerProjectFile& oSHFile, BOOL bWrite)
{
	CString sUniqueName = oSHFile.GetFilename();

	CString strSource;
	if ( ! m_oFile.empty() )
	{
		// Reopen file
		strSource = m_oFile.front().m_sPath;
	}
	else if ( bWrite )
	{
		// Generate new filename (inside incomplete folder)
		strSource.Format( _T("%s\\%s.partial"),
			(LPCTSTR)Settings.Downloads.IncompletePath, sUniqueName );
	}
	else if ( GetFileAttributes( oSHFile.m_sPath ) != INVALID_FILE_ATTRIBUTES )
	{
		// Use specified file path
		strSource = oSHFile.m_sPath;
	}
	else
	{
		// Open existing file from library
		CSingleLock oLock( &Library.m_pSection, TRUE );
		if ( CLibraryFile* pFile = LibraryMaps.LookupFileByHash( &oSHFile, TRUE, TRUE ) )
			strSource = pFile->GetPath();
	}

	//ASSERT( lstrcmpi( PathFindExtension( strSource ), _T(".pd") ) != 0 );

	if ( oSHFile.m_sPath.IsEmpty() )
		oSHFile.m_sPath = strSource;	// For seeded file uploading via Gnutella

	if ( ! Open( strSource, 0, oSHFile.m_nSize, bWrite, oSHFile.m_sName ) )
	{
		CString strMessage;
		strMessage.Format( LoadString( bWrite ?
			IDS_DOWNLOAD_FILE_CREATE_ERROR : IDS_DOWNLOAD_FILE_OPEN_ERROR ), (LPCTSTR)strSource );
		theApp.Message( MSG_ERROR, _T("%s %s"),
			strMessage, (LPCTSTR)GetErrorString( m_nFileError ) );

		Close();
		return FALSE;
	}

	TRACE( _T("Fragmented File : Opened from disk \"%s\"\n"), sUniqueName );

	return TRUE;
}

BOOL CFragmentedFile::Open(const CBTInfo& oInfo, const BOOL bWrite,	CString& strErrorMessage)
{
	CString sUniqueName = oInfo.GetFilename();
	const size_t nCount = m_oFile.size();
	QWORD nOffset = 0;
	size_t i = 0;

	for ( POSITION pos = oInfo.m_pFiles.GetHeadPosition() ; pos ; ++i )
	{
		CBTInfo::CBTFile* pBTFile = oInfo.m_pFiles.GetNext( pos );
		ASSERT( pBTFile->m_nSize != SIZE_UNKNOWN );

		CString strSource;
		if ( i < nCount )
		{
			// Reopen partial file
			strSource = m_oFile[ i ].m_sPath;
			if ( ! PathFileExists( strSource ) && ! StartsWith( strSource, Settings.Downloads.IncompletePath ) )
			{
				strSource = Settings.Downloads.IncompletePath + L"\\" + PathFindFileName( strSource );
				if ( ! PathFileExists( strSource ) )
					strSource = m_oFile[ i ].m_sPath;	// Fallback failed too, use proper path for error message below
			}
		}
		else if ( bWrite )
		{
			// Generate new temp filename (inside incomplete folder)
			strSource.Format( _T("%s\\%s_%d.partial"),
				(LPCTSTR)Settings.Downloads.IncompletePath, sUniqueName, i );
		}
		else
		{
			// Open existing file from library
			strSource = pBTFile->FindFile();
		}

		if ( ! Open( strSource, nOffset, pBTFile->m_nSize, bWrite, pBTFile->m_sPath ) )
		{
			strErrorMessage.Format( LoadString( bWrite ?
				IDS_DOWNLOAD_FILE_CREATE_ERROR : IDS_BT_SEED_SOURCE_LOST ), strSource );
			strErrorMessage += _T(" ");
			strErrorMessage += GetErrorString( m_nFileError );
			theApp.Message( MSG_ERROR, _T("%s"), strErrorMessage );

			Close();
			return FALSE;
		}

		nOffset += pBTFile->m_nSize;
	}

	return TRUE;
}

BOOL CFragmentedFile::FindByPath(const CString& sPath) const
{
	CQuickLock oLock( m_pSection );

	for ( CVirtualFile::const_iterator i = m_oFile.begin() ; i != m_oFile.end() ; ++i )
	{
		if ( ! (*i).m_sPath.CompareNoCase( sPath ) )
			return TRUE;	// Our subfile
	}

	return FALSE;
}

BOOL CFragmentedFile::IsOpen() const
{
	CQuickLock oLock( m_pSection );

	if ( m_oFile.empty() )
		return FALSE;		// No subfiles

	for ( CVirtualFile::const_iterator i = m_oFile.begin() ; i != m_oFile.end() ; ++i )
	{
		if ( ! (*i).m_pFile || ! (*i).m_pFile->IsOpen() )
			return FALSE;	// Closed subfile
	}

	return TRUE;
}

QWORD CFragmentedFile::GetOffset(DWORD nIndex) const
{
	CQuickLock oLock( m_pSection );

	return ( nIndex < m_oFile.size() ) ? m_oFile[ nIndex ].m_nOffset : 0;
}

QWORD CFragmentedFile::GetLength(DWORD nIndex) const
{
	CQuickLock oLock( m_pSection );

	return ( nIndex < m_oFile.size() ) ? m_oFile[ nIndex ].m_nSize : SIZE_UNKNOWN;
}

CString CFragmentedFile::GetPath(DWORD nIndex) const
{
	CQuickLock oLock( m_pSection );

	return ( nIndex < m_oFile.size() ) ? m_oFile[ nIndex ].m_sPath : CString();
}

void CFragmentedFile::SetPath(DWORD nIndex, LPCTSTR szPath)
{
	CQuickLock oLock( m_pSection );

	if ( nIndex < m_oFile.size() )
		m_oFile[ nIndex ].m_sPath = szPath;
}

CString CFragmentedFile::GetName(DWORD nIndex) const
{
	CQuickLock oLock( m_pSection );

	return ( nIndex < m_oFile.size() ) ? m_oFile[ nIndex ].m_sName : CString();
}

void CFragmentedFile::SetName(DWORD nIndex, LPCTSTR szName)
{
	CQuickLock oLock( m_pSection );

	if ( nIndex < m_oFile.size() )
		m_oFile[ nIndex ].m_sName = SafeFilename( szName, true );
}

int CFragmentedFile::GetPriority(DWORD nIndex) const
{
	CQuickLock oLock( m_pSection );

	return ( nIndex < m_oFile.size() ) ? m_oFile[ nIndex ].m_nPriority : prUnwanted;
}

void CFragmentedFile::SetPriority(DWORD nIndex, int nPriority)
{
	CQuickLock oLock( m_pSection );

	if ( nIndex < m_oFile.size() )
		m_oFile[ nIndex ].m_nPriority = nPriority;
}

float CFragmentedFile::GetProgress(DWORD nIndex) const
{
	CQuickLock oLock( m_pSection );

	if ( nIndex >= m_oFile.size() )
		return -1.f;
	if ( m_oFile[ nIndex ].m_nSize == 0 )
		return 100.f;

	return ( (float)GetCompleted( m_oFile[ nIndex ].m_nOffset,
		m_oFile[ nIndex ].m_nSize ) * 100.f ) / (float)m_oFile[ nIndex ].m_nSize;
}

Fragments::List CFragmentedFile::GetFullFragmentList() const
{
	CQuickLock oLock( m_pSection );

	Fragments::List oList( m_oFList.limit() );
	CVirtualFile::const_iterator pItr = m_oFile.begin();
	const CVirtualFile::const_iterator pEnd = m_oFile.end();
	for ( ; pItr != pEnd ; ++pItr )
	{
		if ( (*pItr).m_nPriority != prUnwanted )
			oList.insert( Fragments::Fragment( (*pItr).m_nOffset, (*pItr).m_nOffset + (*pItr).m_nSize ) );
	}

	return oList;
}

Fragments::List CFragmentedFile::GetWantedFragmentList() const
{
	CQuickLock oLock( m_pSection );

	// ToDo: Implement fragment priorities
	// ToDo: Optimize this by caching

	// Exclude unwanted files
	Fragments::List oList( m_oFList );
	for ( CVirtualFile::const_iterator i = m_oFile.begin() ; i != m_oFile.end() ; ++i )
		if ( (*i).m_nPriority == prUnwanted )
			oList.erase( Fragments::Fragment( (*i).m_nOffset, (*i).m_nOffset + (*i).m_nSize ) );

	return oList;
}

QWORD CFragmentedFile::GetCompleted(QWORD nOffset, QWORD nLength) const
{
	CQuickLock oLock( m_pSection );

	// ToDo: Optimize this
	Fragments::List oList( m_oFList );
	oList.insert( Fragments::Fragment( 0, nOffset ) );
	oList.insert( Fragments::Fragment( nOffset + nLength, m_oFList.limit() ) );

	return oList.missing();
}

QWORD CFragmentedFile::GetCompleted(DWORD nIndex) const
{
	CQuickLock oLock( m_pSection );

	return ( nIndex < m_oFile.size() ) ?
		GetCompleted( m_oFile[ nIndex ].m_nOffset, m_oFile[ nIndex ].m_nSize ) : 0;
}

int CFragmentedFile::SelectFile(CSingleLock* pLock) const
{
	const int nCount = GetCount();

	if ( nCount == 1 )
		return 0;	// Single file download

	if ( nCount == 2 )
	{
		// Special case single file detection (disregard tracker spam)
		if ( m_oFile[ 1 ].m_nSize < 100 &&
			StartsWith( m_oFile[ 1 ].m_sName.Mid( m_oFile[ 1 ].m_sName.Find( '\\' ) + 1 ), _T("Torrent downloaded from"), 23 ) )
			return 0;
		if ( m_oFile[ 0 ].m_nSize < 100 &&
			StartsWith( m_oFile[ 0 ].m_sName.Mid( m_oFile[ 0 ].m_sName.Find( '\\' ) + 1 ), _T("Torrent downloaded from"), 23 ) )
			return 1;
	}

	if ( nCount > 1 )
	{
		CSelectDialog dlg;

		{
			CQuickLock oLock( m_pSection );

			int index = 0;
			for ( CVirtualFile::const_iterator i = m_oFile.begin() ; i != m_oFile.end() ; ++i, ++index )
			{
				if ( GetCompleted( (*i).m_nOffset, (*i).m_nSize ) > 0 )
					dlg.Add( (*i).m_sName, index );
			}
		}

		if ( pLock ) pLock->Unlock();

		INT_PTR nResult = dlg.DoModal();

		if ( pLock ) pLock->Lock();

		if ( nResult != IDOK )
			return -1;

		return (int)dlg.Get();
	}

	return -1;		// File closed
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile delete

void CFragmentedFile::Delete()
{
	CVirtualFile oPurge;

	{
		CQuickLock oLock( m_pSection );

		// Enumerate all subfiles
		CVirtualFile::const_iterator pItr = m_oFile.begin();
		const CVirtualFile::const_iterator pEnd = m_oFile.end();
		for ( ; pItr != pEnd ; ++pItr )
		{
			oPurge.push_back( *pItr );
		}

		// Close own handles
		std::for_each( m_oFile.begin(), m_oFile.end(), Releaser() );

		m_oFile.clear();

		m_nUnflushed = 0;
	}

	CVirtualFile::const_iterator pItr = oPurge.begin();
	const CVirtualFile::const_iterator pEnd = oPurge.end();
	for ( ; pItr != pEnd ; ++pItr )
	{
		// Delete subfile
		BOOL bToRecycleBin = !(*pItr).m_bWrite;
		DeleteFileEx( (*pItr).m_sPath, TRUE, bToRecycleBin, TRUE );
	}
}

DWORD CFragmentedFile::Move(DWORD nIndex, LPCTSTR pszDestination, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData)
{
	CString sPath, sName;
	bool bSkip;

	// Get subfile attributes
	{
		CQuickLock oLock( m_pSection );

		if ( nIndex >= m_oFile.size() )
			return ERROR_FILE_NOT_FOUND;

		sPath = m_oFile[ nIndex ].m_sPath;
		sName = m_oFile[ nIndex ].m_sName;
		bSkip = ( m_oFile[ nIndex ].m_nPriority == prUnwanted );

		// Close our handle
		m_oFile[ nIndex ].Release();

		// Make read-only
		m_oFile[ nIndex ].m_bWrite = FALSE;
	}

	ASSERT( ! sName.IsEmpty() );

	CString strTarget( CString( pszDestination ) + _T("\\") + sName );
	CString strTargetDir = strTarget.Left( strTarget.ReverseFind( _T('\\') ) + 1 );

	if ( ! strTarget.CompareNoCase( sPath ) )
		return ERROR_SUCCESS;		// Already moved

	if ( bSkip )
		theApp.Message( MSG_DEBUG, _T("Skipping \"%s\"..."), sPath );
	else
		theApp.Message( MSG_DEBUG, _T("Moving \"%s\" to \"%s\"..."),
			(LPCTSTR)sPath, (LPCTSTR)strTargetDir );

	// Close chained uploads
	theApp.OnRename( sPath );

	// Create directory for file recursively
	BOOL bSuccess = CreateDirectory( strTarget.Left( strTarget.ReverseFind( _T('\\') ) ) );
	DWORD dwError = ::GetLastError();
	if ( bSuccess )
	{
		if ( bSkip )
		{
			bSuccess = DeleteFileEx( sPath, FALSE, TRUE, TRUE );
		}
		else
		{
			// Move/copy file using very long filenames
			bSuccess = MoveFileWithProgress( CString( _T("\\\\?\\") ) + sPath,
				CString( _T("\\\\?\\") ) + strTarget, lpProgressRoutine, lpData,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH );
		}

		dwError = ::GetLastError();
	}

	if ( ! bSuccess )
		theApp.Message( MSG_DEBUG, _T("Moving \"%s\" failed with error: %s"),
			(LPCTSTR)sPath, (LPCTSTR)GetErrorString( dwError ) );

	// Set subfile new attributes
	if ( bSuccess )
		SetPath( nIndex, strTarget );

	// ReEnable uploads
	if ( ! bSkip )
		theApp.OnRename( sPath, bSuccess ? strTarget : sPath );

	Library.Update( true );

	return ( bSuccess ? ERROR_SUCCESS : dwError );
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile flush

BOOL CFragmentedFile::Flush()
{
	CQuickLock oLock( m_pSection );

	if ( m_nUnflushed == 0 )
		return FALSE;	// No unflushed data left

	if ( m_oFile.empty() )
		return FALSE;	// File not opened

	ASSERT_VALID( this );

	std::for_each( m_oFile.begin(), m_oFile.end(), Flusher() );

	m_nUnflushed = 0;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile close

void CFragmentedFile::Close()
{
	if ( m_oFile.empty() )
		return;

	CQuickLock oLock( m_pSection );

	// Close own handles
	std::for_each( m_oFile.begin(), m_oFile.end(), Releaser() );

	m_nUnflushed = 0;
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile clear

//void CFragmentedFile::Clear()
//{
//	if ( m_oFile.empty() )
//		return;
//
//	CQuickLock oLock( m_pSection );
//
//	Close();
//
//	m_oFile.clear();
//}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile make complete

BOOL CFragmentedFile::MakeComplete()
{
	CQuickLock oLock( m_pSection );

	ASSERT_VALID( this );

	if ( m_oFList.empty() )
		return TRUE;	// No incomplete parts left

	if ( m_oFile.empty() )
		return FALSE;	// File is not opened

	m_oFList.clear();

	if ( m_oFList.limit() == SIZE_UNKNOWN )
		return TRUE;	// Unknown size

	std::for_each( m_oFile.begin(), m_oFile.end(), Completer() );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile serialize

void CFragmentedFile::Serialize(CArchive& ar, int nVersion)
{
	CQuickLock oLock( m_pSection );

	if ( ar.IsStoring() )
	{
		SerializeOut1( ar, m_oFList );

		ar << (DWORD)m_oFile.size();
		for ( CVirtualFile::const_iterator i = m_oFile.begin() ; i != m_oFile.end() ; ++i )
		{
			ASSERT( ! (*i).m_sPath.IsEmpty() );
			ar << (*i).m_sPath;
			ar << (*i).m_nOffset;
			ar << (*i).m_nSize;
			ar << (*i).m_bWrite;
			ASSERT( ! (*i).m_sName.IsEmpty() );
			ar << (*i).m_sName;
			ar << (*i).m_nPriority;
		}
	}
	else // Loading
	{
		SerializeIn1( ar, m_oFList, nVersion );

		if ( nVersion < 40 )
			return;		// Old Shareaza import?

		DWORD count = 0;
		QWORD nOffset = 0;
		QWORD nLength = 0;
		BOOL bWrite = FALSE;
		CString strPath;
		CString strName( m_pDownload ? m_pDownload->m_sName : CString() );
		int nPriority = prNormal;

		ar >> count;
		for ( DWORD i = 0 ; i < count ; ++i )
		{
			ar >> strPath;
			ar >> nOffset;
			ar >> nLength;
			ar >> bWrite;
			ar >> strName;
			ar >> nPriority;

			if ( strPath.IsEmpty() || strName.IsEmpty() ||
				 bWrite < FALSE || bWrite > TRUE ||
				 nPriority < prUnwanted || nPriority > prHigh )
				AfxThrowArchiveException( CArchiveException::genericException );

			if ( ! StartsWith( strPath, Settings.Downloads.IncompletePath ) )
			{
				// Update path for user-imported partials
				const CString strNewPath = Settings.Downloads.IncompletePath + L"\\" + PathFindFileName( strPath );
				if ( PathFileExists( strNewPath ) )
					strPath = strNewPath;
			}

			if ( ! Open( strPath, nOffset, nLength, bWrite, strName, nPriority ) )
			{
				theApp.Message( MSG_ERROR, IDS_DOWNLOAD_FILE_OPEN_ERROR, strPath );
				if ( nPriority != prUnwanted )		// Allow partial seeds
					AfxThrowFileException( CFileException::fileNotFound );
			}

			theApp.KeepAlive();		// Large torrents otherwise get "Program Not Responding"
		}

		ASSERT_VALID( this );
	}
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile write some data to a range

BOOL CFragmentedFile::Write(QWORD nOffset, LPCVOID pData, QWORD nLength, QWORD* pnWritten)
{
	ASSERT_VALID( this );

	if ( nLength == 0 )
		return TRUE;	// No data to write

	CQuickLock oLock( m_pSection );

	Fragments::Fragment oMatch( nOffset, nOffset + nLength );
	Fragments::List::const_iterator_pair pMatches = m_oFList.equal_range( oMatch );
	if ( pMatches.first == pMatches.second )
		return FALSE;	// Empty range

	QWORD nProcessed = 0;
	for ( ; pMatches.first != pMatches.second ; ++pMatches.first )
	{
		QWORD nStart = max( pMatches.first->begin(), oMatch.begin() );
		QWORD nToWrite = min( pMatches.first->end(), oMatch.end() ) - nStart;

		const char* pSource
			= static_cast< const char* >( pData ) + ( nStart - oMatch.begin() );

		QWORD nWritten = 0;
		if ( ! VirtualWrite( nStart, pSource, nToWrite, &nWritten ) )
			return FALSE;	// Write error

		if ( pnWritten )
			*pnWritten += nWritten;

		nProcessed += nWritten;
	}

	m_nUnflushed += nProcessed;
	m_oFList.erase( oMatch );
	return nProcessed > 0;
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile read some data from a range

BOOL CFragmentedFile::Read(QWORD nOffset, LPVOID pData, QWORD nLength, QWORD* pnRead)
{
	ASSERT_VALID( this );

	if ( nLength == 0 )
		return TRUE;	// No data to read

	CQuickLock oLock( m_pSection );

	if ( DoesRangeOverlap( nOffset, nLength ) )
		return FALSE;	// No data available yet

	return VirtualRead( nOffset, (char*)pData, nLength, pnRead );
}

BOOL CFragmentedFile::VirtualRead(QWORD nOffset, char* pBuffer, QWORD nBuffer, QWORD* pnRead)
{
	ASSERT( nBuffer != 0 && nBuffer != SIZE_UNKNOWN );
	ASSERT( pBuffer != NULL && AfxIsValidAddress( pBuffer, nBuffer ) );

	// Find first file
	CVirtualFile::const_iterator i = std::find_if( m_oFile.begin(), m_oFile.end(),
		bind2nd( Greater(), nOffset ) );
	if ( i != m_oFile.begin() )
		--i;

	if ( pnRead )
		*pnRead = 0;

	for ( ; nBuffer ; ++i )
	{
		if ( i == m_oFile.end() )
			return FALSE;	// EOF

		const CVirtualFilePart& file = (*i);

		if ( file.m_nOffset > nOffset )
			return FALSE;	// EOF

		QWORD nPartOffset = ( nOffset - file.m_nOffset );
		if ( file.m_nSize < nPartOffset )
			return FALSE;	// EOF

		QWORD nPartLength = min( nBuffer, file.m_nSize - nPartOffset );
		if ( ! nPartLength )
			continue;		// Skip zero length files

		QWORD nRead = 0;
		if ( ! file.m_pFile )
			return FALSE;
		if ( ! file.m_pFile->Read( nPartOffset, pBuffer, nPartLength, &nRead ) )
			return FALSE;

		pBuffer += nRead;
		nOffset += nRead;
		nBuffer -= nRead;
		if ( pnRead )
			*pnRead += nRead;

		if ( nRead != nPartLength )
			return FALSE;	// EOF
	}

	return TRUE;
}

BOOL CFragmentedFile::VirtualWrite(QWORD nOffset, const char* pBuffer, QWORD nBuffer, QWORD* pnWritten)
{
	ASSERT( nBuffer != 0 && nBuffer != SIZE_UNKNOWN );
	ASSERT( pBuffer != NULL && AfxIsValidAddress( pBuffer, nBuffer ) );

	// Find first file
	CVirtualFile::const_iterator i = std::find_if( m_oFile.begin(), m_oFile.end(),
		bind2nd( Greater(), nOffset ) );
	if ( i != m_oFile.begin() )
		--i;

	if ( pnWritten )
		*pnWritten = 0;

	for ( ; nBuffer ; ++i )
	{
		if ( i == m_oFile.end() )
			return FALSE;	// EOF

		const CVirtualFilePart& file = (*i);

		if ( file.m_nOffset > nOffset )
			return FALSE;	// EOF

		QWORD nPartOffset = ( nOffset - file.m_nOffset );
		if ( file.m_nSize < nPartOffset )
			return FALSE;	// EOF

		QWORD nPartLength = min( nBuffer, file.m_nSize - nPartOffset );
		if ( ! nPartLength )
			continue;		// Skip zero length files

		QWORD nWritten = 0;
		if ( ! file.m_bWrite )
			nWritten = nPartLength;	// Skip read only files
		if ( ! file.m_pFile )
			return FALSE;
		if ( ! file.m_pFile->Write( nPartOffset, pBuffer, nPartLength, &nWritten ) )
			return FALSE;

		pBuffer += nWritten;
		nOffset += nWritten;
		nBuffer -= nWritten;

		if ( pnWritten )
			*pnWritten += nWritten;

		if ( nWritten != nPartLength )
			return FALSE;	// EOF
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile invalidate a range

QWORD CFragmentedFile::InvalidateRange(QWORD nOffset, QWORD nLength)
{
	CQuickLock oLock( m_pSection );

	return m_oFList.insert( Fragments::Fragment( nOffset, nOffset + nLength ) );
}

BOOL CFragmentedFile::EnsureWrite()
{
	CQuickLock oLock( m_pSection );

	return ( std::count_if( m_oFile.begin(), m_oFile.end(),
		EnsureWriter() ) == (int)m_oFile.size() );
}
