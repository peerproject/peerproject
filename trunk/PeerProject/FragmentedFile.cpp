//
// FragmentedFile.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
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
#endif

IMPLEMENT_DYNCREATE( CFragmentedFile, CObject )

CFragmentedFile::CVirtualFilePart::CVirtualFilePart() :
	m_pFile( NULL ),
	m_nOffset( 0 ),
	m_bWrite( FALSE ),
	m_nPriority( CFragmentedFile::prNormal )
{
}

CFragmentedFile::CVirtualFilePart::CVirtualFilePart(const CVirtualFilePart& pFile) :
	CPeerProjectFile( pFile ),
	m_pFile( pFile.m_pFile ),
	m_nOffset( pFile.m_nOffset ),
	m_bWrite( pFile.m_bWrite ),
	m_nPriority( pFile.m_nPriority )
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

CFragmentedFile::CFragmentedFile() :
	m_nUnflushed	( 0 )
,	m_oFList		( 0 )
,	m_nFileError	( ERROR_SUCCESS )
,	m_dwRef			( 1 )
,	m_pDownload		( NULL )
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
		for ( CVirtualFile::const_iterator i = m_oFile.begin(); i != m_oFile.end(); ++i )
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
	for ( CVirtualFile::const_iterator i = m_oFile.begin(); i != m_oFile.end(); ++i, ++n )
		dc << n << _T(". File offset ") << (*i).m_nOffset << _T(", ")
			<< (*i).m_nSize << _T(" bytes, ")
			<< ( (*i).m_bWrite ? _T("RW") : _T("RO") )
			<< _T(" \"") << (*i).m_sPath << _T("\"\n");
}

#endif

//////////////////////////////////////////////////////////////////////
// CFragmentedFile open

BOOL CFragmentedFile::Open(LPCTSTR pszFile, QWORD nOffset, QWORD nLength,
	BOOL bWrite, LPCTSTR pszName, int nPriority)
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
	bool bNew = ( pItr == m_oFile.end() );
	if ( bNew )
	{
		// Use new
		CVirtualFilePart part;
		part.m_sPath = pszFile;
		part.m_nOffset = nOffset;
		part.m_bWrite = bWrite;
		part.m_nPriority = nPriority;

		if ( pszName )
			part.m_sName = pszName;

		m_oFile.push_back( part );
		pItr = --m_oFile.end();
	}
	else
	{
		// Use existing
		if ( (*pItr).m_pFile )
			// Already opened
			return ! bWrite || (*pItr).m_pFile->EnsureWrite();
	}

	CTransferFile* pFile = NULL;
	QWORD nRealLength = SIZE_UNKNOWN;
	for ( int nMethod = 0 ; nMethod < 2 ; ++nMethod )
	{
		pFile = TransferFiles.Open( (*pItr).m_sPath, bWrite );
		if ( pFile )
		{
			m_nFileError = ERROR_SUCCESS;
			nRealLength = pFile->GetSize();
			if ( pFile->IsExists() && nLength == SIZE_UNKNOWN )
			{
				nLength = nRealLength;
			}
			else if ( ! bWrite && nRealLength != nLength )
			{
				// Wrong file
				pFile->Release();
				pFile = NULL;
				m_nFileError = ERROR_FILE_INVALID;
			}
			break;
		}

		m_nFileError = ::GetLastError();
		if ( ! bWrite )
			// Do nothing for read only files
			break;

		CString strPath( pszFile );
		switch( nMethod )
		{
		case 0:
			// Try to open file for write from current incomplete folder
			// (in case of changed folder)
			(*pItr).m_sPath = Settings.Downloads.IncompletePath +
				strPath.Mid( strPath.ReverseFind( _T('\\') ) );
			break;

		// TODO: Other methods
		}
	}

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

BOOL CFragmentedFile::Open(const CPeerProjectFile& oSHFile, BOOL bWrite)
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
			Settings.Downloads.IncompletePath, sUniqueName );
	}
	else
	{
		// Open existing file from library
		CSingleLock oLock( &Library.m_pSection, TRUE );
		if ( CLibraryFile* pFile = LibraryMaps.LookupFileByHash(
			oSHFile.m_oSHA1, oSHFile.m_oTiger, oSHFile.m_oED2K,
			oSHFile.m_oBTH, oSHFile.m_oMD5, oSHFile.m_nSize,
			oSHFile.m_nSize, TRUE, TRUE ) )
		{
			strSource = pFile->GetPath();
		}
	}

	if ( ! Open( strSource, 0, oSHFile.m_nSize, bWrite, oSHFile.m_sName ) )
	{
		CString strMessage;
		strMessage.Format( bWrite ? IDS_DOWNLOAD_FILE_CREATE_ERROR :
			IDS_DOWNLOAD_FILE_OPEN_ERROR, (LPCTSTR)strSource );
		theApp.Message( MSG_ERROR, _T("%s %s"),
			strMessage, (LPCTSTR)GetErrorString( m_nFileError ) );

		Close();
		return FALSE;
	}

	TRACE( _T("Fragmented File : Opened from disk \"%s\"\n"), sUniqueName );

	return TRUE;
}

BOOL CFragmentedFile::Open(const CBTInfo& oInfo, const BOOL bWrite,
	CString& strErrorMessage)
{
	CString sUniqueName = oInfo.GetFilename();
	int nCount = 0;
	CVirtualFile::const_iterator pItr = m_oFile.begin();
	bool bReopen = ! m_oFile.empty();
	QWORD nOffset = 0;

	for ( POSITION pos = oInfo.m_pFiles.GetHeadPosition() ; pos ; ++nCount )
	{
		CBTInfo::CBTFile* pBTFile = oInfo.m_pFiles.GetNext( pos );
		ASSERT( pBTFile->m_nSize != SIZE_UNKNOWN );

		CString strSource;
		if ( bReopen )
		{
			// Reopen file
			strSource = (*pItr).m_sPath;
			++pItr;
		}
		else if ( bWrite )
		{
			// Generate new filename (inside incomplete folder)
			strSource.Format( _T("%s\\%s_%d.partial"),
				Settings.Downloads.IncompletePath, sUniqueName, nCount );
		}
		else
		{
			// Open existing file from library
			strSource = pBTFile->FindFile();
		}

		if ( ! Open( strSource, nOffset, pBTFile->m_nSize, bWrite, pBTFile->m_sPath ) )
		{
			strErrorMessage.Format( bWrite ? IDS_DOWNLOAD_FILE_CREATE_ERROR :
				IDS_BT_SEED_SOURCE_LOST, strSource );
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

	for ( CVirtualFile::const_iterator i = m_oFile.begin(); i != m_oFile.end(); ++i )
		if ( ! (*i).m_sPath.CompareNoCase( sPath ) )
			// Our subfile
			return TRUE;

	return FALSE;
}

BOOL CFragmentedFile::IsOpen() const
{
	CQuickLock oLock( m_pSection );

	if ( m_oFile.empty() )
		// No subfiles
		return FALSE;

	for ( CVirtualFile::const_iterator i = m_oFile.begin(); i != m_oFile.end(); ++i )
		if ( ! (*i).m_pFile || ! (*i).m_pFile->IsOpen() )
			// Closed subfile
			return FALSE;

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
		m_oFile[ nIndex ].m_sName = szName;
}

int CFragmentedFile::GetPriority(DWORD nIndex) const
{
	CQuickLock oLock( m_pSection );

	return ( nIndex < m_oFile.size() ) ? m_oFile[ nIndex ].m_nPriority : prDiscarded;
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
	else if ( m_oFile[ nIndex ].m_nSize == 0 )
		return 100.f;
	else
		return ( (float)GetCompleted( m_oFile[ nIndex ].m_nOffset,
			m_oFile[ nIndex ].m_nSize ) * 100.f ) / (float)m_oFile[ nIndex ].m_nSize;
}

Fragments::List CFragmentedFile::GetWantedFragmentList() const
{
	CQuickLock oLock( m_pSection );

	// ToDo: Implement several priorities
	// ToDo: Optimize this by caching

	// Exclude unwanted files
	Fragments::List oList( m_oFList );
	for ( CVirtualFile::const_iterator i = m_oFile.begin(); i != m_oFile.end(); ++i )
		if ( (*i).m_nPriority == prDiscarded )
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
	if ( GetCount() > 1 )
	{
		CSelectDialog dlg;

		{
			CQuickLock oLock( m_pSection );
			int index = 0;
			for( CVirtualFile::const_iterator i = m_oFile.begin(); i != m_oFile.end(); ++i, ++index )
				if ( GetCompleted( (*i).m_nOffset, (*i).m_nSize ) > 0 )
					dlg.Add( (*i).m_sName, index );
		}

		if ( pLock ) pLock->Unlock();

		INT_PTR nResult = dlg.DoModal();

		if ( pLock ) pLock->Lock();

		if ( nResult != IDOK )
			return -1;

		return (int)dlg.Get();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile delete

void CFragmentedFile::Delete()
{
	CVirtualFile oFoo;

	{
		CQuickLock oLock( m_pSection );

		// Enumerate all subfiles
		CVirtualFile::const_iterator pItr = m_oFile.begin();
		const CVirtualFile::const_iterator pEnd = m_oFile.end();
		for ( ; pItr != pEnd; ++pItr )
			oFoo.push_back( *pItr );

		// Close own handles
		std::for_each( m_oFile.begin(), m_oFile.end(), Releaser() );

		m_oFile.clear();

		m_nUnflushed = 0;
	}

	CVirtualFile::const_iterator pItr = oFoo.begin();
	const CVirtualFile::const_iterator pEnd = oFoo.end();
	for( ; pItr != pEnd; ++pItr )
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
		bSkip = ( m_oFile[ nIndex ].m_nPriority == prDiscarded );

		// Close our handle
		m_oFile[ nIndex ].Release();

		// Make read-only
		m_oFile[ nIndex ].m_bWrite = FALSE;
	}

	ASSERT( ! sName.IsEmpty() );

	CString strTarget( CString( pszDestination ) + _T("\\") + sName );
	CString strTargetDir = strTarget.Left( strTarget.ReverseFind( _T('\\') ) + 1 );

	if ( ! strTarget.CompareNoCase( sPath ) )
		// Already moved
		return ERROR_SUCCESS;

	if ( bSkip )
		theApp.Message( MSG_DEBUG, _T("Skipping \"%s\"..."), sPath );
	else
		theApp.Message( MSG_DEBUG, _T("Moving \"%s\" to \"%s\"..."),
			(LPCTSTR)sPath, (LPCTSTR)strTargetDir );

	// Close chained uploads
	theApp.OnRename( sPath );

	// Create directory for file recursively
	BOOL bSuccess = CreateDirectory( strTargetDir );
	DWORD dwError = ::GetLastError();
	if ( bSuccess )
	{
		if ( bSkip )
			bSuccess = DeleteFileEx( sPath, FALSE, TRUE, TRUE );
		else
			// Move/copy file using very long filenames
			bSuccess = MoveFileWithProgress( CString( _T("\\\\?\\") ) + sPath,
				CString( _T("\\\\?\\") ) + strTarget, lpProgressRoutine, lpData,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH );

		dwError = ::GetLastError();
	}

	if ( ! bSuccess )
		theApp.Message( MSG_DEBUG, _T("Moving \"%s\" failed with error: %s"),
			(LPCTSTR)sPath, (LPCTSTR)GetErrorString( dwError ) );

	// Set subfile new attributes
	if ( bSuccess )
		SetPath( nIndex, strTarget );

	if ( ! bSkip )
		// ReEnable uploads
		theApp.OnRename( sPath, bSuccess ? strTarget : sPath );

	return ( bSuccess ? ERROR_SUCCESS : dwError );
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile flush

BOOL CFragmentedFile::Flush()
{
	CQuickLock oLock( m_pSection );

	if ( m_nUnflushed == 0 )
		// No unflushed data left
		return FALSE;

	if ( m_oFile.empty() )
		// File not opened
		return FALSE;

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
// CFragmentedFile make complete

BOOL CFragmentedFile::MakeComplete()
{
	CQuickLock oLock( m_pSection );

	ASSERT_VALID( this );

	if ( m_oFList.empty() )
		// No incomplete parts left
		return TRUE;

	if ( m_oFile.empty() )
		// File is not opened
		return FALSE;

	m_oFList.clear();

	if ( m_oFList.limit() == SIZE_UNKNOWN )
		// Unknown size
		return TRUE;

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
		for ( CVirtualFile::const_iterator i = m_oFile.begin();
			i != m_oFile.end(); ++i )
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
	else
	{
		SerializeIn1( ar, m_oFList, nVersion );

		if ( nVersion >= 40 )
		{
			DWORD count = 0;
			ar >> count;
			for ( DWORD i = 0; i < count; ++i )
			{
				CString sPath;
				ar >> sPath;
				QWORD nOffset = 0;
				ar >> nOffset;
				QWORD nLength = 0;
				ar >> nLength;
				BOOL bWrite = FALSE;
				ar >> bWrite;
				CString sName( m_pDownload ? m_pDownload->m_sName : CString() );
				int nPriority = prNormal;
				if ( nVersion >= 41 )
				{
					ar >> sName;
					ar >> nPriority;
				}

				if ( sPath.IsEmpty() || sName.IsEmpty() ||
					bWrite < FALSE || bWrite > TRUE ||
					nPriority < prDiscarded || nPriority > prHigh )
					AfxThrowArchiveException( CArchiveException::genericException );

				if ( ! Open( sPath, nOffset, nLength, bWrite, sName, nPriority ) )
				{
					theApp.Message( MSG_ERROR, IDS_DOWNLOAD_FILE_OPEN_ERROR, sPath );
					AfxThrowFileException( CFileException::fileNotFound );
				}
			}

			ASSERT_VALID( this );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CFragmentedFile write some data to a range

BOOL CFragmentedFile::Write(QWORD nOffset, LPCVOID pData, QWORD nLength, QWORD* pnWritten)
{
	ASSERT_VALID( this );

	if ( nLength == 0 )
		// No data to write
		return TRUE;

	CQuickLock oLock( m_pSection );

	Fragments::Fragment oMatch( nOffset, nOffset + nLength );
	Fragments::List::const_iterator_pair pMatches = m_oFList.equal_range( oMatch );
	if ( pMatches.first == pMatches.second )
		// Empty range
		return FALSE;

	QWORD nProcessed = 0;
	for ( ; pMatches.first != pMatches.second; ++pMatches.first )
	{
		QWORD nStart = max( pMatches.first->begin(), oMatch.begin() );
		QWORD nToWrite = min( pMatches.first->end(), oMatch.end() ) - nStart;

		const char* pSource
			= static_cast< const char* >( pData ) + ( nStart - oMatch.begin() );

		QWORD nWritten = 0;
		if ( ! VirtualWrite( nStart, pSource, nToWrite, &nWritten ) )
			// Write error
			return FALSE;

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
		// No data to read
		return TRUE;

	CQuickLock oLock( m_pSection );

	if ( DoesRangeOverlap( nOffset, nLength ) )
		// No data available yet
		return FALSE;

	return VirtualRead( nOffset, (char*)pData, nLength, pnRead );
}

BOOL CFragmentedFile::VirtualRead(QWORD nOffset, char* pBuffer, QWORD nBuffer, QWORD* pnRead)
{
	ASSERT( nBuffer != 0 && nBuffer != SIZE_UNKNOWN );
	ASSERT( pBuffer != NULL && AfxIsValidAddress( pBuffer, nBuffer ) );

	// Find first file
	CVirtualFile::const_iterator i = std::find_if( m_oFile.begin(), m_oFile.end(),
		bind2nd( Greater(), nOffset ) );
	ASSERT( i != m_oFile.begin() );
	--i;

	if ( pnRead )
		*pnRead = 0;

	for ( ; nBuffer; ++i )
	{
		if( i == m_oFile.end() )
			// EOF
			return FALSE;
		ASSERT( (*i).m_nOffset <= nOffset );
		QWORD nPartOffset = ( nOffset - (*i).m_nOffset );
		if( (*i).m_nSize < nPartOffset )
			// EOF
			return FALSE;
		QWORD nPartLength = min( nBuffer, (*i).m_nSize - nPartOffset );
		if ( ! nPartLength )
			// Skip zero length files
			continue;

		QWORD nRead = 0;
		if ( ! (*i).m_pFile ||
			 ! (*i).m_pFile->Read( nPartOffset, pBuffer, nPartLength, &nRead ) )
			return FALSE;

		pBuffer += nRead;
		nOffset += nRead;
		nBuffer -= nRead;
		if ( pnRead )
			*pnRead += nRead;

		if ( nRead != nPartLength )
			// EOF
			return FALSE;
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
	ASSERT( i != m_oFile.begin() );
	--i;

	if ( pnWritten )
		*pnWritten = 0;

	for ( ; nBuffer; ++i )
	{
		ASSERT( i != m_oFile.end() );
		ASSERT( (*i).m_nOffset <= nOffset );
		QWORD nPartOffset = ( nOffset - (*i).m_nOffset );
		ASSERT( (*i).m_nSize >= nPartOffset );
		QWORD nPartLength = min( nBuffer, (*i).m_nSize - nPartOffset );
		if ( ! nPartLength )
			// Skip zero length files
			continue;

		QWORD nWritten = 0;
		if ( ! (*i).m_bWrite )
			// Skip read only files
			nWritten = nPartLength;
		else if ( ! (*i).m_pFile ||
			 ! (*i).m_pFile->Write( nPartOffset, pBuffer, nPartLength, &nWritten ) )
			return FALSE;

		pBuffer += nWritten;
		nOffset += nWritten;
		nBuffer -= nWritten;

		if ( pnWritten )
			*pnWritten += nWritten;

		if ( nWritten != nPartLength )
			// EOF
			return FALSE;
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
