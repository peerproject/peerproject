//
// LibraryFolders.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "LibraryFolders.h"

#include "AlbumFolder.h"
#include "Application.h"
#include "CollectionFile.h"
#include "DlgHelp.h"
#include "Settings.h"
#include "Library.h"
#include "LibraryMaps.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "SharedFile.h"
#include "SharedFolder.h"
#include "ShellIcons.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CLibraryFolders, CComObject)

BEGIN_INTERFACE_MAP(CLibraryFolders, CComObject)
	INTERFACE_PART(CLibraryFolders, IID_ILibraryFolders, LibraryFolders)
END_INTERFACE_MAP()

CLibraryFolders LibraryFolders;


//////////////////////////////////////////////////////////////////////
// CLibraryFolders construction

CLibraryFolders::CLibraryFolders()
	: m_pAlbumRoot	( NULL )
{
	EnableDispatch( IID_ILibraryFolders );
}

CLibraryFolders::~CLibraryFolders()
{
	delete m_pAlbumRoot;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders physical folder enumeration

POSITION CLibraryFolders::GetFolderIterator() const
{
	//ASSUME_LOCK( Library.m_pSection );

	return m_pFolders.GetHeadPosition();
}

CLibraryFolder* CLibraryFolders::GetNextFolder(POSITION& pos) const
{
	//ASSUME_LOCK( Library.m_pSection );

	return m_pFolders.GetNext( pos );
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders physical folder search

CLibraryFolder* CLibraryFolders::GetFolder(LPCTSTR pszPath) const
{
	ASSUME_LOCK( Library.m_pSection );

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CLibraryFolder* pFolder = GetNextFolder( pos )->GetFolderByPath( pszPath );
		if ( pFolder != NULL ) return pFolder;
	}

	return NULL;
}

BOOL CLibraryFolders::CheckFolder(CLibraryFolder* pFolder, BOOL bRecursive) const
{
	ASSUME_LOCK( Library.m_pSection );

	if ( m_pFolders.Find( pFolder ) != NULL ) return TRUE;
	if ( ! bRecursive ) return FALSE;

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		if ( GetNextFolder( pos )->CheckFolder( pFolder, TRUE ) ) return TRUE;
	}

	return FALSE;
}

CLibraryFolder* CLibraryFolders::GetFolderByName(LPCTSTR pszName) const
{
	ASSUME_LOCK( Library.m_pSection );

	CString strName( pszName );
	ToLower( strName );

	CString strNextName;
	int nPos = strName.Find( _T('\\') );
	if ( nPos != -1 )
	{
		strNextName = strName.Mid( nPos + 1 );
		strName = strName.Left( nPos );
	}

	for ( POSITION pos = GetFolderIterator() ; pos; )
	{
		CLibraryFolder* pFolder = GetNextFolder( pos );

		if ( pFolder->m_sName.CompareNoCase( strName ) == 0 )
		{
			if ( ! strNextName.IsEmpty() )
				pFolder = pFolder->GetFolderByName( strNextName );

			return pFolder;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders add a root physical folder

CLibraryFolder* CLibraryFolders::AddFolder(LPCTSTR pszPath)
{

	CString strPath = pszPath;

	if ( strPath.GetLength() == 3 && strPath.GetAt( 2 ) == '\\' )
		strPath = strPath.Left( 2 );

	CQuickLock oLock( Library.m_pSection );

	if ( IsFolderShared( strPath ) ) return NULL;
	if ( IsSubFolderShared( strPath ) ) return NULL;

	CLibraryFolder* pFolder = new CLibraryFolder( NULL, strPath );
	if ( ! pFolder ) return NULL;

	BOOL bAdded = FALSE;

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		POSITION posAdd = pos;

		if ( GetNextFolder( pos )->m_sName.CompareNoCase( pFolder->m_sName ) >= 0 )
		{
			m_pFolders.InsertBefore( posAdd, pFolder );
			bAdded = TRUE;
			break;
		}
	}

	if ( ! bAdded ) m_pFolders.AddTail( pFolder );

	pFolder->Maintain( TRUE );

	Library.Update( true );

	return pFolder;
}

CLibraryFolder* CLibraryFolders::AddFolder(LPCTSTR pszPath, BOOL bShared)
{
	CLibraryFolder* pFolder = AddFolder( pszPath );

	if( pFolder )
		pFolder->SetShared( bShared ? TRI_TRUE : TRI_FALSE );

	return pFolder;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders add a shared folder to a List Control

bool CLibraryFolders::AddSharedFolder(CListCtrl& oList)
{
	// Store the last path selected
	static CString strLastPath;
	if ( strLastPath.IsEmpty() )
		strLastPath = oList.GetItemText( 0, 0 );

	// Let user select a path to share
	CString strPath( BrowseForFolder( _T("Select folder to share:"), strLastPath ) );
	if ( strPath.IsEmpty() )
		return false;

	// Convert path to lowercase
	CString strPathLC( strPath );
	ToLower( strPathLC );

	// Check if path is valid
	if ( ! IsShareable( strPathLC ) )
	{
		CHelpDlg::Show( _T("ShareHelp.BadShare") );
		return false;
	}

	// Check if path is already shared
	bool bForceAdd( false );
	for ( int nItem( 0 ) ; nItem < oList.GetItemCount() ; ++nItem )
	{
		bool bSubFolder( false );
		CString strOldLC( oList.GetItemText( nItem, 0 ) );
		ToLower( strOldLC );

		if ( strPathLC == strOldLC )
		{
			// Matches exactly
		}
		else if ( strPathLC.GetLength() > strOldLC.GetLength() )
		{
			if ( strPathLC.Left( strOldLC.GetLength() + 1 ) != strOldLC + '\\' )
				continue;
		}
		else if ( strPathLC.GetLength() < strOldLC.GetLength() )
		{
			bSubFolder = true;
			if ( strOldLC.Left( strPathLC.GetLength() + 1 ) != strPathLC + _T('\\') )
				continue;
		}
		else
		{
			continue;
		}

		if ( bSubFolder )
		{
			CString strMessage;
			strMessage.Format( LoadString( IDS_LIBRARY_SUBFOLDER_IN_LIBRARY ), strPath );

			if ( bForceAdd || AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) == IDYES )
			{
				// Don't bother asking again- remove all sub-folders
				bForceAdd = true;

				// Remove the sub-folder
				oList.DeleteItem( nItem );
				--nItem;
			}
			else
			{
				return false;
			}
		}
		else
		{
			CString strMessage;
			strMessage.Format( LoadString( IDS_WIZARD_SHARE_ALREADY ), strOldLC );
			AfxMessageBox( strMessage, MB_ICONINFORMATION );
			return false;
		}
	}

	//Add path to shared list
	oList.InsertItem( LVIF_TEXT|LVIF_IMAGE, oList.GetItemCount(), strPath, 0,
		0, SHI_FOLDER_OPEN, 0 );

	// Return success
	return true;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders remove a root physical folder

BOOL CLibraryFolders::RemoveFolder(CLibraryFolder* pFolder)
{
	CWaitCursor pCursor;
	CQuickLock pLock( Library.m_pSection );

	POSITION pos = m_pFolders.Find( pFolder );
	if ( pos == NULL ) return FALSE;

	pFolder->Maintain( FALSE );

	pFolder->OnDelete( Settings.Library.CreateGhosts ? TRI_TRUE : TRI_FALSE );
	m_pFolders.RemoveAt( pos );

	Library.Update( true );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders check if a physical folder is part of the library

CLibraryFolder* CLibraryFolders::IsFolderShared(const CString& strPath) const
{
	//CQuickLock oLock( Library.m_pSection );

	// Convert path to lowercase
	CString strPathLC( strPath );
	ToLower( strPathLC );

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CLibraryFolder* pFolder = GetNextFolder( pos );

		CString strOldLC( pFolder->m_sPath );
		ToLower( strOldLC );

		if ( strPathLC.GetLength() > strOldLC.GetLength() )
		{
			int nLength = strOldLC.GetLength();
			if ( strPathLC.Left( nLength ) == strOldLC &&
				 strPathLC.GetAt( nLength ) == _T('\\') )
				return pFolder;
		}
		else
		{
			if ( strPathLC == strOldLC ) return pFolder;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders check if a subfolder of a physical folder is part of the library

CLibraryFolder* CLibraryFolders::IsSubFolderShared(const CString& strPath) const
{
	//CQuickLock oLock( Library.m_pSection );

	// Convert path to lowercase
	CString strPathLC( strPath );
	ToLower( strPathLC );

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CLibraryFolder* pFolder = GetNextFolder( pos );

		CString strOldLC( pFolder->m_sPath );
		ToLower( strOldLC );

		if ( strPathLC.GetLength() < strOldLC.GetLength() )
		{
			int nLength = strPathLC.GetLength();
			if ( strOldLC.Left( nLength ) == strPathLC &&
				 strOldLC.GetAt( nLength ) == _T('\\') )
				 return pFolder;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders check if folder is not a system directory, incomplete folder etc...

bool CLibraryFolders::IsShareable(const CString& strPath)
{
	// Convert path to lowercase
	CString strPathLC( strPath );
	ToLower( strPathLC );

	//Get system paths (to compare)
	CString strWindowsLC( theApp.GetWindowsFolder() );
	ToLower( strWindowsLC );

	CString strProgramsLC( theApp.GetProgramFilesFolder() );
	ToLower( strProgramsLC );

	//Get various PeerProject paths (to compare)
	CString strIncompletePathLC = Settings.Downloads.IncompletePath;
	ToLower( strIncompletePathLC );

	CString strGeneralPathLC = Settings.General.Path;
	ToLower( strGeneralPathLC );

	CString strUserPathLC = Settings.General.UserPath;
	ToLower( strUserPathLC );

	return !( strPathLC == _T( "" ) ||
		 strPathLC == strWindowsLC.Left( 3 ) ||
		 strPathLC == strProgramsLC ||
		 strPathLC == strWindowsLC ||
		 strPathLC == strGeneralPathLC ||
		 strPathLC == strGeneralPathLC + _T("\\data") ||
		 strPathLC == strUserPathLC ||
		 strPathLC == strUserPathLC + _T("\\data") ||
		 strPathLC == strIncompletePathLC );
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders virtual album list access

CAlbumFolder* CLibraryFolders::GetAlbumRoot() const
{
	//ASSUME_LOCK( Library.m_pSection );

	return m_pAlbumRoot;
}

BOOL CLibraryFolders::CheckAlbum(CAlbumFolder* pFolder) const
{
	ASSUME_LOCK( Library.m_pSection );

	return m_pAlbumRoot && m_pAlbumRoot->CheckFolder( pFolder, TRUE );
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders virtual album target search

CAlbumFolder* CLibraryFolders::GetAlbumTarget(LPCTSTR pszSchemaURI, LPCTSTR pszMember, LPCTSTR pszValue) const
{
	if ( m_pAlbumRoot == NULL ) return NULL;

	CSchemaPtr pSchema = SchemaCache.Get( pszSchemaURI );
	if ( pSchema == NULL ) return NULL;

	CSchemaMember* pMember = pSchema->GetMember( pszMember );

	if ( pMember == NULL )
	{
		if ( pSchema->GetMemberCount() == 0 ) return NULL;
		POSITION pos = pSchema->GetMemberIterator();
		pMember = pSchema->GetNextMember( pos );
	}

	if ( pszValue != NULL )
	{
		CString strValue( pszValue );
		CXMLNode::UniformString( strValue );
		return m_pAlbumRoot->GetTarget( pMember, strValue );
	}
	else
	{
		return m_pAlbumRoot->GetTarget( pMember, NULL );
	}
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders virtual album collection search

CAlbumFolder* CLibraryFolders::GetCollection(const Hashes::Sha1Hash& oSHA1)
{
	ASSUME_LOCK( Library.m_pSection );

	return m_pAlbumRoot ? m_pAlbumRoot->FindCollection( oSHA1 ) : NULL;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders mount a collection

BOOL CLibraryFolders::MountCollection(const Hashes::Sha1Hash& oSHA1, CCollectionFile* pCollection)
{
	CSingleLock pLock( &Library.m_pSection );
	BOOL bSuccess = FALSE;

	if ( ! pLock.Lock( 500 ) ) return FALSE;

	if ( pCollection->GetThisURI().GetLength() )
		bSuccess |= m_pAlbumRoot->MountCollection( oSHA1, pCollection );

	//if ( pCollection->GetParentURI().GetLength() )
	//{
	//	if ( CAlbumFolder* pFolder = GetAlbumTarget( pCollection->GetParentURI(), NULL, NULL ) )
	//		bSuccess |= pFolder->MountCollection( oSHA1, pCollection, TRUE );
	//}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders virtual album default tree

CAlbumFolder* CLibraryFolders::CreateAlbumTree()
{
	ASSUME_LOCK( Library.m_pSection );

	if ( m_pAlbumRoot == NULL )
		m_pAlbumRoot = new CAlbumFolder( NULL, CSchema::uriLibrary );

	DWORD nCount = m_pAlbumRoot->GetFolderCount();

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriAllFiles ) == NULL )
	{
		/*CAlbumFolder* pAllFiles		=*/ m_pAlbumRoot->AddFolder( CSchema::uriAllFiles );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriApplicationRoot ) == NULL )
	{
		CAlbumFolder* pAppRoot			= m_pAlbumRoot->AddFolder( CSchema::uriApplicationRoot );
		/*CAlbumFolder* pAppAll			=*/ pAppRoot->AddFolder( CSchema::uriApplicationAll );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriArchiveRoot ) == NULL )
	{
		CAlbumFolder* pArchiveRoot		= m_pAlbumRoot->AddFolder( CSchema::uriArchiveRoot );
		/*CAlbumFolder* pArchiveAll		=*/ pArchiveRoot->AddFolder( CSchema::uriArchiveAll );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriBookRoot ) == NULL )
	{
		CAlbumFolder* pBookRoot			= m_pAlbumRoot->AddFolder( CSchema::uriBookRoot );
		/*CAlbumFolder* pBookAll		=*/ pBookRoot->AddFolder( CSchema::uriBookAll );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriImageRoot ) == NULL )
	{
		CAlbumFolder* pImageRoot		= m_pAlbumRoot->AddFolder( CSchema::uriImageRoot );
		/*CAlbumFolder* pImageAll		=*/ pImageRoot->AddFolder( CSchema::uriImageAll );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriMusicRoot ) == NULL )
	{
		CAlbumFolder* pMusicRoot		= m_pAlbumRoot->AddFolder( CSchema::uriMusicRoot );
		/*CAlbumFolder* pMusicAll		=*/ pMusicRoot->AddFolder( CSchema::uriMusicAll );
		/*CAlbumFolder* pMusicAlbum		=*/ pMusicRoot->AddFolder( CSchema::uriMusicAlbumCollection );
		/*CAlbumFolder* pMusicArtist	=*/ pMusicRoot->AddFolder( CSchema::uriMusicArtistCollection );
		/*CAlbumFolder* pMusicGenre		=*/ pMusicRoot->AddFolder( CSchema::uriMusicGenreCollection );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriVideoRoot ) == NULL )
	{
		CAlbumFolder* pVideoRoot		= m_pAlbumRoot->AddFolder( CSchema::uriVideoRoot );
		/*CAlbumFolder* pVideoAll		=*/ pVideoRoot->AddFolder( CSchema::uriVideoAll );
		/*CAlbumFolder* pVideoSeries	=*/ pVideoRoot->AddFolder( CSchema::uriVideoSeriesCollection );
		/*CAlbumFolder* pVideoFilm		=*/ pVideoRoot->AddFolder( CSchema::uriVideoFilmCollection );
		/*CAlbumFolder* pVideoMusic		=*/ pVideoRoot->AddFolder( CSchema::uriVideoMusicCollection );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriFavouritesFolder ) == NULL )
	{
		/*CAlbumFolder* pFavourites		=*/ m_pAlbumRoot->AddFolder( CSchema::uriFavouritesFolder );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriCollectionsFolder ) == NULL )
	{
		/*CAlbumFolder* pCollections	=*/ m_pAlbumRoot->AddFolder( CSchema::uriCollectionsFolder );
	//	/*CAlbumFolder* pCollections	=*/ pCollections->AddFolder( CSchema::uriBitTorrent );	// ToDo: Add .torrents
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriDocumentRoot ) == NULL )
	{
		CAlbumFolder* pDocumentRoot		= m_pAlbumRoot->AddFolder( CSchema::uriDocumentRoot );
		/*CAlbumFolder* pDocumentAll	=*/ pDocumentRoot->AddFolder( CSchema::uriDocumentAll );
	}

	if ( m_pAlbumRoot->GetFolderByURI( CSchema::uriGhostFolder ) == NULL )
	{
		/*CAlbumFolder* pGhostFolder	=*/ m_pAlbumRoot->AddFolder( CSchema::uriGhostFolder );
	}

	if ( m_pAlbumRoot->GetFolderCount() != nCount )
	{
		for ( POSITION pos = LibraryMaps.GetFileIterator() ; pos ; )
		{
			CLibraryFile* pFile = LibraryMaps.GetNextFile( pos );
			if ( pFile->IsAvailable() )
				m_pAlbumRoot->OrganiseFile( pFile );
		}
	}

	return m_pAlbumRoot;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders file delete handler

BOOL CLibraryFolders::OnFileDelete(CLibraryFile* pFile, BOOL bDeleteGhost)
{
	ASSUME_LOCK( Library.m_pSection );

	if ( m_pAlbumRoot )
		m_pAlbumRoot->OnFileDelete( pFile, bDeleteGhost );

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		if ( GetNextFolder( pos )->OnFileDelete( pFile ) )
			return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders clear

void CLibraryFolders::Clear()
{
	ASSUME_LOCK( Library.m_pSection );

	for ( POSITION pos = GetFolderIterator() ; pos ; )
		delete GetNextFolder( pos );
	m_pFolders.RemoveAll();

	delete m_pAlbumRoot;
	m_pAlbumRoot = NULL;
}

void CLibraryFolders::ClearGhosts()
{
	ASSUME_LOCK( Library.m_pSection );

	if ( m_pAlbumRoot )
	{
		if ( CAlbumFolder* pGhosts = m_pAlbumRoot->GetFolderByURI( CSchema::uriGhostFolder ) )
		{
			for ( POSITION pos = pGhosts->GetFileIterator(); pos; )
			{
				CLibraryFile* pFile = pGhosts->GetNextFile( pos );
				ASSERT( ! pFile->IsAvailable() );
				pFile->Delete( TRUE );
			}
		}
	}
}

DWORD CLibraryFolders::GetGhostCount() const
{
	CQuickLock oLock( Library.m_pSection );

	if ( m_pAlbumRoot )
	{
		if ( CAlbumFolder* pGhosts = m_pAlbumRoot->GetFolderByURI( CSchema::uriGhostFolder ) )
			return pGhosts->GetFileCount();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders thread scan

BOOL CLibraryFolders::ThreadScan(const BOOL bForce)
{
	ASSUME_LOCK( Library.m_pSection );

	BOOL bChanged = FALSE;

	for ( POSITION pos = GetFolderIterator() ; pos && Library.IsThreadEnabled() ; )
	{
		CLibraryFolder* pFolder = GetNextFolder( pos );

		if ( GetFileAttributes( pFolder->m_sPath ) != INVALID_FILE_ATTRIBUTES )
		{
			if ( pFolder->SetOnline() ) bChanged = TRUE;

			if ( pFolder->IsChanged() || bForce )
			{
				if ( pFolder->ThreadScan() ) bChanged = TRUE;
			}
		}
		else
		{
			if ( pFolder->SetOffline() ) bChanged = TRUE;
		}
	}

	return bChanged;
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders serialize

void CLibraryFolders::Serialize(CArchive& ar, int nVersion)
{
	ASSUME_LOCK( Library.m_pSection );

	if ( m_pAlbumRoot == NULL )
		m_pAlbumRoot = new CAlbumFolder( NULL, CSchema::uriLibrary );

	if ( ar.IsStoring() )
	{
		ar.WriteCount( GetFolderCount() );

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			GetNextFolder( pos )->Serialize( ar, nVersion );
		}
	}
	else // Loading
	{
		for ( DWORD_PTR nCount = ar.ReadCount() ; nCount > 0 ; nCount-- )
		{
			CLibraryFolder* pFolder = new CLibraryFolder( NULL );
			pFolder->Serialize( ar, nVersion );
			m_pFolders.AddTail( pFolder );

			pFolder->Maintain( TRUE );
		}
	}

	//if ( nVersion > 6 )
		m_pAlbumRoot->Serialize( ar, nVersion );
}

void CLibraryFolders::Maintain()
{
	//CQuickLock oLock( Library.m_pSection );

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		GetNextFolder( pos )->Maintain( TRUE );
	}
}

//////////////////////////////////////////////////////////////////////
// CLibraryFolders ILibraryFolders

IMPLEMENT_DISPATCH(CLibraryFolders, LibraryFolders)

STDMETHODIMP CLibraryFolders::XLibraryFolders::get_Application(IApplication FAR* FAR* ppApplication)
{
	METHOD_PROLOGUE( CLibraryFolders, LibraryFolders )
	return CApplication::GetApp( ppApplication );
}

STDMETHODIMP CLibraryFolders::XLibraryFolders::get_Library(ILibrary FAR* FAR* ppLibrary)
{
	METHOD_PROLOGUE( CLibraryFolders, LibraryFolders )
	*ppLibrary = (ILibrary*)Library.GetInterface( IID_ILibrary, TRUE );
	return S_OK;
}

STDMETHODIMP CLibraryFolders::XLibraryFolders::get__NewEnum(IUnknown FAR* FAR* /*ppEnum*/)
{
	METHOD_PROLOGUE( CLibraryFolders, LibraryFolders )
	return E_NOTIMPL;
}

STDMETHODIMP CLibraryFolders::XLibraryFolders::get_Item(VARIANT vIndex, ILibraryFolder FAR* FAR* ppFolder)
{
	METHOD_PROLOGUE( CLibraryFolders, LibraryFolders )

	//CQuickLock oLock( Library.m_pSection );

	CLibraryFolder* pFolder = NULL;
	*ppFolder = NULL;

	if ( vIndex.vt == VT_BSTR )
	{
		CString strName( vIndex.bstrVal );
		pFolder = pThis->GetFolder( strName );
	}
	else
	{
		VARIANT va;
		VariantInit( &va );

		if ( FAILED( VariantChangeType( &va, (VARIANT FAR*)&vIndex, 0, VT_I4 ) ) )
			return E_INVALIDARG;
		if ( va.lVal < 0 || va.lVal >= pThis->GetFolderCount() )
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

STDMETHODIMP CLibraryFolders::XLibraryFolders::get_Count(LONG FAR* pnCount)
{
	METHOD_PROLOGUE( CLibraryFolders, LibraryFolders )

	//CQuickLock oLock( Library.m_pSection );

	*pnCount = static_cast< LONG >( pThis->GetFolderCount() );

	return S_OK;
}
