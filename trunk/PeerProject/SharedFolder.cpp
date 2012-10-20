//
// SharedFolder.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "SharedFolder.h"
#include "SharedFile.h"
#include "Library.h"
#include "LibraryDictionary.h"
#include "LibraryFolders.h"
#include "Application.h"
#include "Skin.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CLibraryFolder, CComObject)

BEGIN_INTERFACE_MAP(CLibraryFolder, CComObject)
	INTERFACE_PART(CLibraryFolder, IID_ILibraryFolder, LibraryFolder)
	INTERFACE_PART(CLibraryFolder, IID_ILibraryFolders, LibraryFolders)
	INTERFACE_PART(CLibraryFolder, IID_ILibraryFiles, LibraryFiles)
END_INTERFACE_MAP()

//////////////////////////////////////////////////////////////////////
// CLibraryFolder construction

CLibraryFolder::CLibraryFolder(CLibraryFolder* pParent, LPCTSTR pszPath)
	: m_nFiles		( 0 )
	, m_nVolume 	( 0 )
	, m_nScanCookie	( 0 )
	, m_nUpdateCookie ( 0 )
	, m_nSelectCookie ( 0 )
	, m_pParent 	( pParent )
	, m_sPath		( pszPath )
	, m_bShared 	( pParent ? TRI_UNKNOWN : TRI_TRUE )
	, m_bExpanded	( pParent ? FALSE : TRUE )
	, m_hMonitor	( INVALID_HANDLE_VALUE )
	, m_bForceScan	( TRUE )
	, m_bOffline	( FALSE )
{
	EnableDispatch( IID_ILibraryFolder );
	EnableDispatch( IID_ILibraryFolders );
	EnableDispatch( IID_ILibraryFiles );

	PathToName();

	RenewGUID();
}

CLibraryFolder::~CLibraryFolder()
{
	Clear();
}

void CLibraryFolder::CloseMonitor()
{
	if ( m_hMonitor != INVALID_HANDLE_VALUE )
	{
		FindCloseChangeNotification( m_hMonitor );
		m_hMonitor = INVALID_HANDLE_VALUE;
		m_bForceScan = FALSE;
	}
}

bool CLibraryFolder::operator==(const CLibraryFolder& val) const
{
	return ( m_oGUID == val.m_oGUID );
}

void CLibraryFolder::RenewGUID()
{
	CoCreateGuid( reinterpret_cast< GUID* > ( &*m_oGUID.begin() ) );
	m_oGUID.validate();
}

CXMLElement* CLibraryFolder::CreateXML(CXMLElement* pRoot, BOOL bSharedOnly, XmlType nType) const
{
	if ( bSharedOnly && ! IsShared() )
		return NULL;

	CXMLElement* pFolder;

	switch ( nType )
	{
	case xmlDC:
		pFolder = pRoot->AddElement( _T("directory") );
		if ( pFolder )
			pFolder->AddAttribute( _T("name"), m_sName );
		break;

	default:
		pFolder = pRoot->AddElement( _T("folder") );
		if ( pFolder )
			pFolder->AddAttribute( _T("name"), m_sName );
	}

	if ( pFolder )
	{
		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			GetNextFolder( pos )->CreateXML( pFolder, bSharedOnly, nType );
		}

		for ( POSITION pos = GetFileIterator() ; pos ; )
		{
			GetNextFile( pos )->CreateXML( pFolder, bSharedOnly, nType );
		}
	}

	return pFolder;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder folder list

POSITION CLibraryFolder::GetFolderIterator() const
{
	return m_pFolders.GetStartPosition();
}

CLibraryFolder* CLibraryFolder::GetNextFolder(POSITION& pos) const
{
	CLibraryFolder* pOutput = NULL;
	CString strName;
	m_pFolders.GetNextAssoc( pos, strName, pOutput );
	return pOutput;
}

CLibraryFolder* CLibraryFolder::GetFolderByName(LPCTSTR pszName) const
{
	CString strName( pszName );
	ToLower( strName );

	CString strNextName;
	int nPos = strName.Find( _T('\\') );
	if ( nPos != -1 )
	{
		strNextName = strName.Mid( nPos + 1 );
		strName = strName.Left( nPos );
	}

	CLibraryFolder* pOutput = NULL;
	if ( ! m_pFolders.Lookup( strName, pOutput ) )
		return NULL;

	if ( strNextName.IsEmpty() )
		return pOutput;

	return pOutput->GetFolderByName( strNextName );
}

CLibraryFolder* CLibraryFolder::GetFolderByPath(LPCTSTR pszPath) const
{
	if ( m_sPath.CompareNoCase( pszPath ) == 0 )
		return (CLibraryFolder*)this;

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CLibraryFolder* pFolder = GetNextFolder( pos )->GetFolderByPath( pszPath );
		if ( pFolder ) return pFolder;
	}

	return NULL;
}

BOOL CLibraryFolder::CheckFolder(CLibraryFolder* pFolder, BOOL bRecursive) const
{
	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CLibraryFolder* pCheck = GetNextFolder( pos );
		if ( pCheck == pFolder ) return TRUE;
		if ( bRecursive && pCheck->CheckFolder( pFolder, TRUE ) ) return TRUE;
	}

	return FALSE;
}

DWORD CLibraryFolder::GetFolderCount() const
{
	ASSUME_LOCK( Library.m_pSection );
	ASSERT_VALID( this );

	return (DWORD)m_pFolders.GetCount();
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder file list

POSITION CLibraryFolder::GetFileIterator() const
{
	ASSUME_LOCK( Library.m_pSection );
	ASSERT_VALID( this );

	return m_pFiles.GetStartPosition();
}

CLibraryFile* CLibraryFolder::GetNextFile(POSITION& pos) const
{
	ASSUME_LOCK( Library.m_pSection );
	ASSERT_VALID( this );

	CLibraryFile* pOutput = NULL;
	CString strName;
	m_pFiles.GetNextAssoc( pos, strName, pOutput );
	return pOutput;
}

CLibraryFile* CLibraryFolder::GetFile(LPCTSTR pszName) const
{
	ASSUME_LOCK( Library.m_pSection );
	ASSERT_VALID( this );

	CLibraryFile* pOutput = NULL;
	CString strName( pszName );
	ToLower( strName );
	return ( m_pFiles.Lookup( strName, pOutput ) ) ? pOutput : NULL;
}

DWORD CLibraryFolder::GetFileCount() const
{
	ASSUME_LOCK( Library.m_pSection );
	ASSERT_VALID( this );

	return (DWORD)m_pFiles.GetCount();
}

DWORD CLibraryFolder::GetFileList(CLibraryList* pList, BOOL bRecursive) const
{
	DWORD nCount = 0;

	for ( POSITION pos = GetFileIterator() ; pos ; )
	{
		const CLibraryFile* pFile = GetNextFile( pos );
		pList->CheckAndAdd( pFile );
		nCount++;
	}

	if ( bRecursive )
	{
		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			const CLibraryFolder* pFolder = GetNextFolder( pos );
			nCount += pFolder->GetFileList( pList, bRecursive );
		}
	}

	return nCount;
}

DWORD CLibraryFolder::GetSharedCount(BOOL bRecursive) const
{
	ASSUME_LOCK( Library.m_pSection );
	ASSERT_VALID( this );

	DWORD nCount = 0;

	for ( POSITION pos = GetFileIterator() ; pos ; )
	{
		const CLibraryFile* pFile = GetNextFile( pos );
		if ( pFile->IsShared() ) nCount++;
	}

	if ( bRecursive )
	{
		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			const CLibraryFolder* pFolder = GetNextFolder( pos );
			nCount += pFolder->GetSharedCount( bRecursive );
		}
	}

	return nCount;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder clear

void CLibraryFolder::Clear()
{
	CloseMonitor();

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		delete GetNextFolder( pos );
	}

	for ( POSITION pos = GetFileIterator() ; pos ; )
	{
		delete GetNextFile( pos );
	}

	m_pFolders.RemoveAll();
	m_pFiles.RemoveAll();

	m_nFiles	= 0;
	m_nVolume	= 0;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder serialize

void CLibraryFolder::Serialize(CArchive& ar, int nVersion)
{
	if ( ar.IsStoring() )
	{
		SaveGUID( m_sPath, m_oGUID );

		ar << m_sPath;
		ar << m_bShared;
		ar << m_bExpanded;

		ar.WriteCount( GetFolderCount() );

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			GetNextFolder( pos )->Serialize( ar, nVersion );
		}

		ar.WriteCount( GetFileCount() );

		for ( POSITION pos = GetFileIterator() ; pos ; )
		{
			GetNextFile( pos )->Serialize( ar, nVersion );
		}
	}
	else // Loading
	{
		Clear();

		ar >> m_sPath;

		LoadGUID( m_sPath, m_oGUID );

		//if ( nVersion >= 5 )
		//{
			ar >> m_bShared;
		//}
		//else
		//{
		//	BYTE bShared;
		//	ar >> bShared;
		//	m_bShared = bShared ? TRI_UNKNOWN : TRI_FALSE;
		//}

		//if ( nVersion >= 3 )
			ar >> m_bExpanded;

		PathToName();

		DWORD_PTR nCount = ar.ReadCount();

		m_pFolders.InitHashTable( GetBestHashTableSize( nCount ), FALSE );

		for ( ; nCount > 0 ; nCount-- )
		{
			CLibraryFolder* pFolder = new CLibraryFolder( this );
			if ( ! pFolder )
				break;		// theApp.Message( MSG_DEBUG, _T("Memory allocation error in CLibraryFolder::Serialize") );

			pFolder->Serialize( ar, nVersion );

			m_pFolders.SetAt( pFolder->m_sNameLC, pFolder );

			m_nFiles  += pFolder->m_nFiles;
			m_nVolume += pFolder->m_nVolume;
		}

		nCount = ar.ReadCount();
		m_pFiles.InitHashTable( GetBestHashTableSize( nCount ), FALSE );

		for ( ; nCount > 0 ; nCount-- )
		{
			//CLibraryFile* pFile = new CLibraryFile( this );
			CAutoPtr< CLibraryFile > pFile( new CLibraryFile( this ) );
			if ( ! pFile )
				AfxThrowMemoryException();
				// theApp.Message( MSG_DEBUG, _T("Memory allocation error in CLibraryFolder::Serialize") );

			pFile->Serialize( ar, nVersion );

			m_pFiles.SetAt( pFile->GetNameLC(), pFile );

			m_nFiles++;
			m_nVolume += pFile->m_nSize;

			Library.AddFile( pFile.Detach() );

			theApp.KeepAlive();
		}
	}
}

void CLibraryFolder::PathToName()
{
	m_sName = m_sPath;
	int nPos = m_sName.ReverseFind( '\\' );
	if ( nPos >= 0 && nPos < m_sName.GetLength() - 1 )
		m_sName = m_sName.Mid( nPos + 1 );
	m_sNameLC = m_sName;
	ToLower( m_sNameLC );
}

CString CLibraryFolder::GetRelativeName() const
{
	if ( m_pParent )
		return m_pParent->GetRelativeName() + _T("\\") + m_sName;

	return m_sName;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder threaded scan

BOOL CLibraryFolder::ThreadScan(DWORD nScanCookie)
{
	CSingleLock pLock( &Library.m_pSection );
	WIN32_FIND_DATA pFind;
	HANDLE hSearch;

	if ( m_sPath.CompareNoCase( Settings.Downloads.IncompletePath ) == 0 ) return FALSE;

	hSearch = FindFirstFile( SafePath( m_sPath + _T("\\*.*") ), &pFind );

	pLock.Lock();
	m_nScanCookie	= nScanCookie;
	nScanCookie		= Library.GetScanCookie();
	pLock.Unlock();

	BOOL bChanged = FALSE;

	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( CLibrary::IsBadFile( pFind.cFileName, m_sPath, pFind.dwFileAttributes ) )
				 continue;

			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				CLibraryFolder* pFolder = GetFolderByName( pFind.cFileName );
				if ( pFolder != NULL )
				{
					m_nFiles  -= pFolder->m_nFiles;
					m_nVolume -= pFolder->m_nVolume;

					if ( pFolder->m_sName.CompareNoCase( pFind.cFileName ) )
					{
						pLock.Lock();
						m_pFolders.RemoveKey( pFolder->m_sNameLC );
						pFolder->OnDelete();
						pFolder = new CLibraryFolder( this, m_sPath + _T("\\") + pFind.cFileName );
						m_pFolders.SetAt( pFolder->m_sNameLC, pFolder );
						bChanged = TRUE;
						m_nUpdateCookie++;
						pLock.Unlock();
					}
				}
				else
				{
					pFolder = new CLibraryFolder( this, m_sPath + _T("\\") + pFind.cFileName );
					pLock.Lock();
					m_pFolders.SetAt( pFolder->m_sNameLC, pFolder );
					bChanged = TRUE;
					m_nUpdateCookie++;
					pLock.Unlock();
				}

				if ( pFolder->ThreadScan( nScanCookie ) )
					bChanged = TRUE;

				m_nFiles	+= pFolder->m_nFiles;
				m_nVolume	+= pFolder->m_nVolume;
			}
			else
			{
				pLock.Lock();

				BOOL bNew;
				CLibraryFile* pFile = AddFile( pFind.cFileName, bNew );
				if ( ! bNew )
				{
					m_nVolume -= pFile->m_nSize;
					if ( pFile->m_sName.CompareNoCase( pFind.cFileName ) )
					{
						Library.RemoveFile( pFile );
						pFile->m_sName = pFind.cFileName;
						Library.AddFile( pFile );
						bChanged = TRUE;
					}
				}
				else
					bChanged = TRUE;

				pLock.Unlock();

				QWORD nLongSize = (QWORD)pFind.nFileSizeLow |
					( (QWORD)pFind.nFileSizeHigh << 32 );

				if ( pFile->ThreadScan( pLock, nScanCookie, nLongSize,
					&pFind.ftLastWriteTime ) )
					bChanged = TRUE;

				m_nVolume += pFile->m_nSize;
			}
		}
		while ( Library.IsThreadEnabled() && FindNextFile( hSearch, &pFind ) );

		FindClose( hSearch );
	}

	if ( ! Library.IsThreadEnabled() ) return FALSE;

	pLock.Lock();

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CLibraryFolder* pFolder = GetNextFolder( pos );

		if ( pFolder->m_nScanCookie != nScanCookie )
		{
			m_nFiles	-= pFolder->m_nFiles;
			m_nVolume	-= pFolder->m_nVolume;

			m_pFolders.RemoveKey( pFolder->m_sNameLC );
			pFolder->OnDelete();

			bChanged = TRUE;
			m_nUpdateCookie++;
		}
	}

	for ( POSITION pos = GetFileIterator() ; pos ; )
	{
		CLibraryFile* pFile = GetNextFile( pos );

		if ( pFile->m_nScanCookie != nScanCookie )
		{
			pFile->OnDelete( TRUE );
			bChanged = TRUE;
		}
	}

	pLock.Unlock();

	return bChanged;
}

CLibraryFile* CLibraryFolder::AddFile(LPCTSTR szName, BOOL& bNew)
{
	CQuickLock oLock( Library.m_pSection );

	CLibraryFile* pFile = GetFile( szName );
	if ( pFile == NULL )
	{
		pFile = new CLibraryFile( this, szName );
		m_pFiles.SetAt( pFile->GetNameLC(), pFile );
		m_nFiles++;
		m_nUpdateCookie++;
		bNew = TRUE;
	}
	else
	{
		bNew = FALSE;
	}
	return pFile;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder monitor

BOOL CLibraryFolder::IsChanged()
{
	BOOL bChanged = FALSE;

	// Monitor changes
	if ( m_hMonitor != INVALID_HANDLE_VALUE )
	{
		switch ( WaitForSingleObject( m_hMonitor, 0 ) )
		{
		case WAIT_TIMEOUT:		// No changes
			break;

		case WAIT_OBJECT_0:		// Changes detected
			bChanged = TRUE;

			// Reset monitor
			if ( FindNextChangeNotification( m_hMonitor ) )
				break;

		default:				// Errors
			CloseMonitor();
		}
	}

	if ( m_hMonitor == INVALID_HANDLE_VALUE && Settings.Library.WatchFolders )
	{
		// Enable monitor
		m_hMonitor = FindFirstChangeNotification( m_sPath, TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_LAST_WRITE );
	}
	else if ( m_hMonitor != INVALID_HANDLE_VALUE && ! Settings.Library.WatchFolders )
	{
		// Disable monitor
		CloseMonitor();
	}

	if ( m_bForceScan )
	{
		// Change status forced
		bChanged = TRUE;
		m_bForceScan = FALSE;
	}

	return bChanged;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder scan

void CLibraryFolder::Scan()
{
	CLibraryFolder* pFolder = this;
	for ( ; pFolder->m_pParent ; pFolder = pFolder->m_pParent );
	if ( pFolder ) pFolder->m_bForceScan = TRUE;
	Library.Wakeup();
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder shared status managing

BOOL CLibraryFolder::IsShared() const
{
	if ( m_bOffline )
		return FALSE;

	for ( const CLibraryFolder* pFolder = this ; pFolder ; pFolder = pFolder->m_pParent )
	{
		if ( pFolder->m_bShared )
		{
			if ( pFolder->m_bShared == TRI_TRUE )
				return TRUE;
			if ( pFolder->m_bShared == TRI_FALSE )
				return FALSE;
			//if ( pFolder->m_bShared == TRI_UNKNOWN )
			//	return FALSE;
		}
	}

	return TRUE;
}

void CLibraryFolder::GetShared(BOOL& bShared) const
{
	if ( m_bOffline )
		bShared = FALSE;
	else if ( m_bShared == TRI_TRUE )
		bShared = TRUE;
	else if ( m_bShared == TRI_FALSE )
		bShared = FALSE;
}

void CLibraryFolder::SetShared(TRISTATE bShared)
{
	if ( m_bShared != bShared )
	{
		m_bShared = bShared;
		m_nUpdateCookie++;

		LibraryDictionary.Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder offline status managing

BOOL CLibraryFolder::IsOffline() const
{
	return m_bOffline;
}

BOOL CLibraryFolder::SetOffline()
{
	if ( ! m_bOffline )
	{
		m_bOffline = TRUE;
		m_nUpdateCookie++;

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			GetNextFolder( pos )->SetOffline();
		}

		LibraryDictionary.Invalidate();

		return TRUE;
	}
	return FALSE;
}

BOOL CLibraryFolder::SetOnline()
{
	if ( m_bOffline )
	{
		m_bOffline = FALSE;
		m_nUpdateCookie++;

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			GetNextFolder( pos )->SetOnline();
		}

		LibraryDictionary.Invalidate();

		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder callbacks

void CLibraryFolder::OnDelete(TRISTATE bCreateGhost)
{
	while ( ! m_pFiles.IsEmpty() )
	{
		POSITION pos = m_pFiles.GetStartPosition();
		CLibraryFile* pFile = GetNextFile( pos );
		m_pFiles.RemoveKey( pFile->GetNameLC() );
		pFile->OnDelete( FALSE, bCreateGhost );
	}

	while ( ! m_pFolders.IsEmpty() )
	{
		POSITION pos = m_pFolders.GetStartPosition();
		CLibraryFolder* pFolder = GetNextFolder( pos );
		m_pFolders.RemoveKey( pFolder->m_sNameLC );
		pFolder->OnDelete( bCreateGhost );
	}

	delete this;
}

BOOL CLibraryFolder::OnFileDelete(CLibraryFile* pRemovingFile)
{
	for ( POSITION pos = GetFileIterator() ; pos ; )
	{
		CLibraryFile* pFile = GetNextFile( pos );
		if ( pFile == pRemovingFile )
		{
			m_nFiles --;
			m_nVolume -= pFile->m_nSize;
			m_pFiles.RemoveKey( pFile->GetNameLC() );
			m_nUpdateCookie++;
			return TRUE;
		}
	}

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CLibraryFolder* pFolder = GetNextFolder( pos );
		if ( pFolder->OnFileDelete( pRemovingFile ) )
			return TRUE;
	}

	return FALSE;
}

void CLibraryFolder::OnFileRename(CLibraryFile* pFile)
{
	for ( POSITION pos = m_pFiles.GetStartPosition() ; pos ; )
	{
		CLibraryFile* pOld = NULL;
		CString strName;

		m_pFiles.GetNextAssoc( pos, strName, pOld );

		if ( pFile == pOld )
		{
			m_pFiles.RemoveKey( strName );
			m_pFiles.SetAt( pFile->GetNameLC(), pFile );
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders maintain physical folder

void CLibraryFolder::Maintain(BOOL bAdd)
{
	ASSERT_VALID( this );

#ifndef WIN64
	if ( theApp.m_nWinVer < WIN_XP ) return;	// Not Win2K icons?
#endif

	CString strDesktopINI( m_sPath + _T("\\desktop.ini") );
	DWORD dwDesktopINIAttr = GetFileAttributes( strDesktopINI );
	const BOOL bExists = ( dwDesktopINIAttr != INVALID_FILE_ATTRIBUTES );

	// Check if this is our desktop.ini
	CString strPath;
	if ( bExists )
	{
		GetPrivateProfileString( _T(".ShellClassInfo"), _T("IconFile"), _T(""),
			strPath.GetBuffer( MAX_PATH ), MAX_PATH, strDesktopINI );
		strPath.ReleaseBuffer();
		if ( strPath.IsEmpty() )
			GetPrivateProfileString( _T(".ShellClassInfo"), _T("IconResource"), _T(""),
				strPath.GetBuffer( MAX_PATH ), MAX_PATH, strDesktopINI );
		strPath.ReleaseBuffer();
		strPath.MakeLower();
	}
	const BOOL bOur = ( strPath.Find( _T("peerproject") ) > 1 );

	if ( ! Settings.Library.UseCustomFolders )
		bAdd = FALSE;

	if ( bAdd && ( bOur || ! bExists ) )
	{
		// Remove Hidden and System attributes
		BOOL bChanged = FALSE;
		if ( bExists && ( dwDesktopINIAttr & ( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM ) ) )
			bChanged = SetFileAttributes( strDesktopINI, dwDesktopINIAttr & ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM ) );

		CString strIconFile = Settings.General.Path + ( theApp.m_nWinVer >= WIN_VISTA ?
			_T("\\Schemas\\WindowsFolder.ico") : _T("\\Schemas\\WindowsFolder.Safe.ico") );
		if ( PathFileExists( strIconFile ) )
		{
			WritePrivateProfileString( _T(".ShellClassInfo"), _T("ConfirmFileOp"), _T("0"), strDesktopINI );
			WritePrivateProfileString( _T(".ShellClassInfo"), _T("InfoTip"), LoadString( IDS_FOLDER_TIP ), strDesktopINI );
			WritePrivateProfileString( _T(".ShellClassInfo"), _T("IconFile"), strIconFile, strDesktopINI );
			WritePrivateProfileString( _T(".ShellClassInfo"), _T("IconIndex"), _T("0"), strDesktopINI );
		}
		else
		{
			strIconFile = Skin.GetImagePath( IDI_COLLECTION );
			//CString strIconIndex( _T("0") );
			//int nPos = strIconFile.ReverseFind( _T(',') );
			//if ( nPos != -1 && nPos > strIconFile.ReverseFind( _T('\\') ) )
			//{
			//	strIconIndex = strIconFile.Mid( nPos + 1 );
			//	strIconFile  = strIconFile.Left( nPos );
			//}
			WritePrivateProfileString( _T(".ShellClassInfo"), _T("ConfirmFileOp"), _T("0"), strDesktopINI );
			WritePrivateProfileString( _T(".ShellClassInfo"), _T("InfoTip"), LoadString( IDS_FOLDER_TIP ), strDesktopINI );
			WritePrivateProfileString( _T(".ShellClassInfo"), _T("IconResource"), strIconFile, strDesktopINI );
		}

		if ( bChanged )
			SetFileAttributes( strDesktopINI, dwDesktopINIAttr | ( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM ) );

		PathMakeSystemFolder( m_sPath );
	}
	else if ( bOur )
	{
		PathUnmakeSystemFolder( m_sPath );

		SetFileAttributes( strDesktopINI, dwDesktopINIAttr & ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM ) );

		DeleteFileEx( strDesktopINI, FALSE, FALSE, FALSE );
	}
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder ILibraryFolder

IMPLEMENT_DISPATCH(CLibraryFolder, LibraryFolder)

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_Application(IApplication FAR* FAR* ppApplication)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	return CApplication::GetApp( ppApplication );
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_Library(ILibrary FAR* FAR* ppLibrary)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	*ppLibrary = (ILibrary*)Library.GetInterface( IID_ILibrary, TRUE );
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_Parent(ILibraryFolder FAR* FAR* ppFolder)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	if ( pThis->m_pParent )
		*ppFolder = (ILibraryFolder*)pThis->m_pParent->GetInterface( IID_ILibraryFolder, TRUE );
	else
		*ppFolder = NULL;
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_Path(BSTR FAR* psPath)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	*psPath = CComBSTR( pThis->m_sPath ).Detach();
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_Name(BSTR FAR* psPath)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	*psPath = CComBSTR( pThis->m_sName ).Detach();
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_Shared(TRISTATE FAR* pnValue)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	*pnValue = pThis->m_bShared;
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::put_Shared(TRISTATE nValue)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	pThis->m_bShared = nValue;
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_EffectiveShared(VARIANT_BOOL FAR* pbValue)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	*pbValue = pThis->IsShared() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_Folders(ILibraryFolders FAR* FAR* ppFolders)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	*ppFolders = (ILibraryFolders*)pThis->GetInterface( IID_ILibraryFolders, TRUE );
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolder::get_Files(ILibraryFiles FAR* FAR* ppFiles)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolder )
	*ppFiles = (ILibraryFiles*)pThis->GetInterface( IID_ILibraryFiles, TRUE );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder ILibraryFolders

IMPLEMENT_DISPATCH(CLibraryFolder, LibraryFolders)

STDMETHODIMP CLibraryFolder::XLibraryFolders::get_Application(IApplication FAR* FAR* ppApplication)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolders )
	return CApplication::GetApp( ppApplication );
}

STDMETHODIMP CLibraryFolder::XLibraryFolders::get_Library(ILibrary FAR* FAR* ppLibrary)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolders )
	*ppLibrary = (ILibrary*)Library.GetInterface( IID_ILibrary, TRUE );
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolders::get__NewEnum(IUnknown FAR* FAR* /*ppEnum*/)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolders )
	return E_NOTIMPL;
}

STDMETHODIMP CLibraryFolder::XLibraryFolders::get_Item(VARIANT vIndex, ILibraryFolder FAR* FAR* ppFolder)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolders )

	CQuickLock oLock( Library.m_pSection );

	CLibraryFolder* pFolder = NULL;
	*ppFolder = NULL;

	if ( vIndex.vt == VT_BSTR )
	{
		CString strName( vIndex.bstrVal );
		if ( strName.Find( '\\' ) >= 0 )
			pFolder = pThis->GetFolderByPath( strName );
		else
			pFolder = pThis->GetFolderByName( strName );
	}
	else
	{
		VARIANT va;
		VariantInit( &va );

		if ( FAILED( VariantChangeType( &va, (VARIANT FAR*)&vIndex, 0, VT_I4 ) ) )
			return E_INVALIDARG;
		if ( va.lVal < 0 || va.lVal >= (LONG)pThis->GetFolderCount() )
			return E_INVALIDARG;

		for ( POSITION pos = pThis->GetFolderIterator() ; pos ; )
		{
			pFolder = pThis->GetNextFolder( pos );
			if ( va.lVal-- == 0 ) break;
			pFolder = NULL;
		}
	}

	*ppFolder = pFolder ? (ILibraryFolder*)pFolder->GetInterface( IID_ILibraryFolder, TRUE ) : NULL;

	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFolders::get_Count(LONG FAR* pnCount)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFolders )

	CQuickLock oLock( Library.m_pSection );

	*pnCount = static_cast< LONG >( pThis->GetFolderCount() );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolder ILibraryFiles

IMPLEMENT_DISPATCH(CLibraryFolder, LibraryFiles)

STDMETHODIMP CLibraryFolder::XLibraryFiles::get_Application(IApplication FAR* FAR* ppApplication)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFiles )
	return CApplication::GetApp( ppApplication );
}

STDMETHODIMP CLibraryFolder::XLibraryFiles::get_Library(ILibrary FAR* FAR* ppLibrary)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFiles )
	*ppLibrary = (ILibrary*)Library.GetInterface( IID_ILibrary, TRUE );
	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFiles::get__NewEnum(IUnknown FAR* FAR* /*ppEnum*/)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFiles )
	return E_NOTIMPL;
}

STDMETHODIMP CLibraryFolder::XLibraryFiles::get_Item(VARIANT vIndex, ILibraryFile FAR* FAR* ppFile)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFiles )

	CQuickLock oLock( Library.m_pSection );

	CLibraryFile* pFile = NULL;
	*ppFile = NULL;

	if ( vIndex.vt == VT_BSTR )
	{
		CString strName( vIndex.bstrVal );
		pFile = pThis->GetFile( strName );
	}
	else
	{
		VARIANT va;
		VariantInit( &va );

		if ( FAILED( VariantChangeType( &va, (VARIANT FAR*)&vIndex, 0, VT_I4 ) ) )
			return E_INVALIDARG;
		if ( va.lVal < 0 || va.lVal >= (LONG)pThis->GetFileCount() )
			return E_INVALIDARG;

		for ( POSITION pos = pThis->GetFileIterator() ; pos ; )
		{
			pFile = pThis->GetNextFile( pos );
			if ( va.lVal-- == 0 ) break;
			pFile = NULL;
		}
	}

	*ppFile = pFile ? (ILibraryFile*)pFile->GetInterface( IID_ILibraryFile, TRUE ) : NULL;

	return S_OK;
}

STDMETHODIMP CLibraryFolder::XLibraryFiles::get_Count(LONG FAR* pnCount)
{
	METHOD_PROLOGUE( CLibraryFolder, LibraryFiles )

	CQuickLock oLock( Library.m_pSection );

	*pnCount = static_cast< LONG >( pThis->GetFileCount() );

	return S_OK;
}
