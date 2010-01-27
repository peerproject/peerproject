//
// AlbumFolder.cpp
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
#include "Library.h"
#include "LibraryFolders.h"
#include "SharedFile.h"
#include "AlbumFolder.h"
#include "CollectionFile.h"
#include "Schema.h"
#include "SchemaChild.h"
#include "SchemaCache.h"
#include "Settings.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CAlbumFolder construction

CAlbumFolder::CAlbumFolder(CAlbumFolder* pParent, LPCTSTR pszSchemaURI, LPCTSTR pszName, BOOL bAutoDelete)
{
	m_pParent = pParent;

	m_sSchemaURI	= pszSchemaURI ? pszSchemaURI : NULL;
	m_pSchema		= pszSchemaURI ? SchemaCache.Get( pszSchemaURI ) : NULL;
	m_pXML			= NULL;

	if ( pszName > (LPCTSTR)1 )
	{
		m_sName = pszName;
	}
	else if ( pszName != (LPCTSTR)1 )
	{
		if ( m_pSchema != NULL )
		{
			int nColon = m_pSchema->m_sTitle.Find( ':' );
			if ( nColon >= 0 ) m_sName = m_pSchema->m_sTitle.Mid( nColon + 1 ).Trim();
		}

		if ( m_sName.IsEmpty() ) m_sName = _T("New Folder");
	}

	m_bExpanded		= pParent == NULL || pParent->m_pParent == NULL;
	m_bAutoDelete	= bAutoDelete;

	m_nUpdateCookie	= 0;
	m_nSelectCookie	= 0;
	m_nListCookie	= 0;
	m_pCollection	= NULL;

	RenewGUID();
}

CAlbumFolder::~CAlbumFolder()
{
	Clear();
}

void CAlbumFolder::RenewGUID()
{
	CoCreateGuid( reinterpret_cast< GUID* > ( m_oGUID.begin() ) );
	m_oGUID.validate();
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder folder list

CAlbumFolder* CAlbumFolder::AddFolder(LPCTSTR pszSchemaURI, LPCTSTR pszName, BOOL bAutoDelete)
{
	if ( pszSchemaURI == NULL && m_pSchema != NULL )
		pszSchemaURI = m_pSchema->GetContainedURI( CSchema::stFolder );

	if ( pszSchemaURI == NULL )
		pszSchemaURI = CSchema::uriFolder;

	CAlbumFolder* pFolder = new CAlbumFolder( this, pszSchemaURI, pszName, bAutoDelete );

	m_pFolders.AddTail( pFolder );

	return pFolder;
}

POSITION CAlbumFolder::GetFolderIterator() const
{
	return m_pFolders.GetHeadPosition();
}

CAlbumFolder* CAlbumFolder::GetNextFolder(POSITION& pos) const
{
	return m_pFolders.GetNext( pos );
}

CAlbumFolder* CAlbumFolder::GetFolder(LPCTSTR pszName) const
{
	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CAlbumFolder* pCheck = GetNextFolder( pos );
		if ( pCheck->m_sName.CompareNoCase( pszName ) == 0 ) return pCheck;
	}

	return NULL;
}

CAlbumFolder* CAlbumFolder::GetFolderByURI(LPCTSTR pszURI) const
{
	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CAlbumFolder* pCheck = GetNextFolder( pos );
		if ( pCheck->m_pSchema != NULL &&
			 pCheck->m_pSchema->CheckURI( pszURI ) ) return pCheck;
	}

	return NULL;
}

BOOL CAlbumFolder::CheckFolder(CAlbumFolder* pFolder, BOOL bRecursive) const
{
	if ( this == pFolder ) return TRUE;
	if ( *this == *pFolder ) return TRUE;
	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CAlbumFolder* pCheck = GetNextFolder( pos );
		if ( pCheck == pFolder ) return TRUE;
		if ( *pCheck == *pFolder ) return TRUE;
		if ( bRecursive && pCheck->CheckFolder( pFolder, TRUE ) ) return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder search for objects

CAlbumFolder* CAlbumFolder::GetTarget(CSchemaMember* pMember, LPCTSTR pszValue) const
{
	if ( m_pSchema == pMember->m_pSchema )
	{
		if ( pszValue == NULL )
		{
			return (CAlbumFolder*)this;
		}
		else if ( m_pXML != NULL )
		{
			CString strValue = pMember->GetValueFrom( m_pXML, NULL, TRUE );
			CXMLNode::UniformString( strValue );
			if ( strValue.CompareNoCase( pszValue ) == 0 ) return (CAlbumFolder*)this;
		}
	}

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CAlbumFolder* pCheck	= GetNextFolder( pos );
		CAlbumFolder* pResult	= pCheck->GetTarget( pMember, pszValue );
		if ( pResult ) return pResult;
	}

	return NULL;
}

CAlbumFolder* CAlbumFolder::FindCollection(const Hashes::Sha1Hash& oSHA1)
{
	if ( validAndEqual( m_oCollSHA1, oSHA1 ) ) return this;

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		CAlbumFolder* pFolder = GetNextFolder( pos );
		if ( CAlbumFolder* pFind = pFolder->FindCollection( oSHA1 ) ) return pFind;
	}

	return NULL;
}

CAlbumFolder* CAlbumFolder::FindFolder(const Hashes::Guid& oGUID)
{
	if ( m_oGUID == oGUID )
		return this;	// Its me!

	// Find between children
	POSITION pos = m_pFolders.GetHeadPosition();
	while ( pos )
	{
		CAlbumFolder* pTemp = m_pFolders.GetNext( pos )->FindFolder( oGUID );
		if ( pTemp )
			return pTemp;	// Found
	}
	return NULL;
}

bool CAlbumFolder::OnFolderDelete(CAlbumFolder* pFolder)
{
	ASSUME_LOCK( Library.m_pSection );

	// Find by pointer (direct)
	POSITION pos = m_pFolders.Find( pFolder );
	if ( pos == NULL )
		return false;

	m_pFolders.RemoveAt( pos );

	ASSERT( pFolder->m_pParent == this );
	ASSERT( ! LibraryFolders.CheckAlbum( pFolder ) ) ;

	pFolder->m_pParent = NULL;

	Library.m_nUpdateCookie++;
	m_nUpdateCookie++;
	Delete( TRUE );
	return true;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder file list

void CAlbumFolder::AddFile(CLibraryFile* pFile)
{
	ASSUME_LOCK( Library.m_pSection );

	if ( pFile == NULL ) return;

	POSITION pos = m_pFiles.Find( pFile );
	if ( pos != NULL ) return;

	m_pFiles.AddTail( pFile );

	if ( m_oCollSHA1 )
	{
		if ( CLibraryFile* pCollection = LibraryMaps.LookupFileBySHA1( m_oCollSHA1, FALSE, TRUE ) )
			pFile->m_nCollIndex = pCollection->m_nIndex;
		else
			m_oCollSHA1.clear();
	}

	m_nUpdateCookie++;
	Library.m_nUpdateCookie++;
}

POSITION CAlbumFolder::GetFileIterator() const
{
	ASSUME_LOCK( Library.m_pSection );

	return m_pFiles.GetHeadPosition();
}

CLibraryFile* CAlbumFolder::GetNextFile(POSITION& pos) const
{
	ASSUME_LOCK( Library.m_pSection );

	return m_pFiles.GetNext( pos );
}

int CAlbumFolder::GetSharedCount() const
{
	ASSUME_LOCK( Library.m_pSection );

	int nCount = 0;

	for ( POSITION pos = GetFileIterator() ; pos ; )
	{
		CLibraryFile* pFile = GetNextFile( pos );
		if ( pFile->IsShared() ) nCount++;
	}

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		nCount += GetNextFolder( pos )->GetSharedCount();
	}

	return nCount;
}

void CAlbumFolder::RemoveFile(CLibraryFile* pFile)
{
	ASSUME_LOCK( Library.m_pSection );

	if ( POSITION pos = m_pFiles.Find( pFile ) )
	{
		m_pFiles.RemoveAt( pos );
		m_nUpdateCookie++;
		Library.m_nUpdateCookie++;
		Delete( TRUE );
	}
}

void CAlbumFolder::OnFileDelete(CLibraryFile* pFile, BOOL bDeleteGhost)
{
	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		GetNextFolder( pos )->OnFileDelete( pFile, bDeleteGhost );
	}

	if ( ! bDeleteGhost && CheckURI( m_sSchemaURI, CSchema::uriGhostFolder ) )
	{
		m_nUpdateCookie++;
		Library.m_nUpdateCookie++;
		return;
	}

	if ( POSITION pos = m_pFiles.Find( pFile ) )
	{
		m_pFiles.RemoveAt( pos );
		m_nUpdateCookie++;
		Library.m_nUpdateCookie++;
		Delete( TRUE );
	}
}

CAlbumFolder* CAlbumFolder::FindFile(CLibraryFile* pFile)
{
	if ( m_pFiles.Find( pFile ) != NULL ) return this;

	POSITION pos = GetFolderIterator();
	CAlbumFolder* pFirst = pos ? GetNextFolder( pos ) : NULL;

	if ( GetFolderCount() > 1 )
	{
		while ( pos )
		{
			CAlbumFolder* pFolder = GetNextFolder( pos )->FindFile( pFile );
			if ( pFolder != NULL ) return pFolder;
		}

		CAlbumFolder* pFolder = pFirst->FindFile( pFile );
		if ( pFolder != NULL ) return pFolder;
	}
	else if ( pFirst != NULL )
	{
		CAlbumFolder* pFolder = pFirst->FindFile( pFile );
		if ( pFolder != NULL ) return pFolder;
	}

	return NULL;
}

int CAlbumFolder::GetFileList(CLibraryList* pList, BOOL bRecursive) const
{
	ASSUME_LOCK( Library.m_pSection );

	int nCount = 0;

	for ( POSITION pos = GetFileIterator() ; pos ; )
	{
		pList->CheckAndAdd( GetNextFile( pos ) );
		nCount++;
	}

	if ( bRecursive )
	{
		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			GetNextFolder( pos )->GetFileList( pList, bRecursive );
		}
	}

	return nCount;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder clear

void CAlbumFolder::Delete(BOOL bIfEmpty)
{
	if ( m_pParent == NULL ) return;

	if ( bIfEmpty )
	{
		if ( ! m_bAutoDelete ) return;
		if ( m_oCollSHA1 ) return;
		if ( GetFolderCount() ) return;
		if ( GetFileCount() ) return;
	}

	m_pParent->OnFolderDelete( this );
	delete this;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder metadata

BOOL CAlbumFolder::SetMetadata(CXMLElement* pXML)
{
	m_nUpdateCookie++;
	Library.m_nUpdateCookie++;

	if ( m_pXML != NULL )
	{
		delete m_pXML;
		m_pXML		= NULL;
		m_pSchema	= NULL;
		m_sSchemaURI.Empty();
	}

	if ( pXML == NULL ) return TRUE;

	m_sSchemaURI	= pXML->GetAttributeValue( CXMLAttribute::schemaName );
	m_pSchema		= SchemaCache.Get( m_sSchemaURI );
	m_pXML			= pXML->GetFirstElement();

	if ( m_pSchema == NULL || m_pXML == NULL )
	{
		m_pXML		= NULL;
		m_pSchema	= NULL;
		m_sSchemaURI.Empty();
		return FALSE;
	}

	m_pXML->Detach();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder metadata synchronisation

BOOL CAlbumFolder::MetaFromFile(CLibraryFile* pFile)
{
	if ( m_pSchema == NULL || pFile->m_pMetadata == NULL ) return FALSE;

	CSchemaChild* pChild = m_pSchema->GetContained( pFile->m_pSchema->GetURI() );
	if ( pChild == NULL ) return FALSE;

	if ( m_pXML == NULL ) m_pXML = new CXMLElement( NULL, m_pSchema->m_sSingular );

	pChild->MemberCopy( m_pXML, pFile->m_pMetadata );

	m_nUpdateCookie++;
	Library.m_nUpdateCookie++;

	return TRUE;
}

BOOL CAlbumFolder::MetaToFiles(BOOL bAggressive)
{
	if ( m_pSchema == NULL || m_pXML == NULL ) return FALSE;

	for ( POSITION pos = GetFileIterator() ; pos ; )
	{
		CLibraryFile* pFile	= GetNextFile( pos );
		CSchema* pSchema	= pFile->m_pSchema;

		if ( pSchema == NULL ) continue;

		if ( CSchemaChild* pChild = m_pSchema->GetContained( pSchema->GetURI() ) )
		{
			CXMLElement* pXML = pFile->m_pMetadata->Clone();

			if ( pChild->MemberCopy( m_pXML, pXML, TRUE, bAggressive ) )
			{
				CXMLElement* pRoot = pSchema->Instantiate( TRUE );
				pRoot->AddElement( pXML );
				pFile->SetMetadata( pRoot );
				delete pRoot;
			}
			else
			{
				delete pXML;
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder select the best view

CString CAlbumFolder::GetBestView() const
{
	if ( m_sBestView.GetLength() > 0 ) return m_sBestView;

	if ( m_oCollSHA1 ) return _T("CLibraryCollectionView");

	if ( m_pSchema != NULL && m_pSchema->m_sLibraryView.GetLength() > 0 )
		return m_pSchema->m_sLibraryView;

	return CString();
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder mount a collection

BOOL CAlbumFolder::MountCollection(const Hashes::Sha1Hash& oSHA1, CCollectionFile* pCollection, BOOL bForce)
{
	BOOL bResult = FALSE;
	bool bGoingDeeper = false;

	// If folder doesn't contain any schema defined then don't go deeper
	if ( m_pSchema == NULL ) return FALSE;

	// Must we mount the collection here?
	// The parent may be absent thus we will try to mount it somewhere.
	// Otherwise, if the validation succeeds we will mount it at the exact parent.
	// If parent can not hold object like this we will mount at the collection root only.
	CString strParentURI = pCollection->GetParentURI();
	TRISTATE bMountHere = TRI_UNKNOWN;

	if ( strParentURI.GetLength() )
		bMountHere = m_sSchemaURI == strParentURI ? TRI_TRUE : TRI_FALSE;

	// If this folder is a collection or simple folder don't mount it
	// (some collections are folder types which in turn can hold folders)
	if ( m_oCollSHA1 || CheckURI( m_sSchemaURI, CSchema::uriFolder ) )
		bMountHere = TRI_FALSE;

	if ( bMountHere != TRI_FALSE &&
	// If the folder schema allows to hold objects having URIs of the collection
		 m_pSchema->GetContained( pCollection->GetThisURI() ) != NULL ||
	// or when the folder URI is the root collection folder
		 CheckURI( m_sSchemaURI, CSchema::uriCollectionsFolder ) )
	{
		CAlbumFolder* pFolder = NULL;

		if ( ! bForce )
		{
			bGoingDeeper = true;

			for ( POSITION pos = GetFolderIterator() ; pos ; )
			{
				CAlbumFolder* pSubFolder = GetNextFolder( pos );
				// Mount it deeper if we can
				bResult |= pSubFolder->MountCollection( oSHA1, pCollection, bForce );

				// Check if the same collection exists
				if ( validAndEqual( pSubFolder->m_oCollSHA1, oSHA1 ) )
					pFolder = pSubFolder; 
			}
		}

		// If the collection doesn't exist or we are forcing, mount it and update Library
		if ( pFolder == NULL )
		{
			pFolder = AddFolder( pCollection->GetThisURI(), pCollection->GetTitle() );
			if ( pFolder )
			{
				pFolder->SetCollection( oSHA1, pCollection );

				m_nUpdateCookie++;
				Library.m_nUpdateCookie++;
				bResult = TRUE;
			}
		}
	}

	// If the criteria for the mounting didn't match and we haven't iterated subfolders
	if ( !bGoingDeeper )
	{
		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			bResult |= GetNextFolder( pos )->MountCollection( oSHA1, pCollection, bForce );
		}
	}
	return bResult;
}

void CAlbumFolder::SetCollection(const Hashes::Sha1Hash& oSHA1, CCollectionFile* pCollection)
{
	m_oCollSHA1 = oSHA1;
	m_sBestView.Empty();

	if ( m_pCollection != NULL )
	{
		delete m_pCollection;
		m_pCollection = NULL;
	}

	if ( CXMLElement* pMetadata = pCollection->GetMetadata() )
	{
		pMetadata = pMetadata->Clone();
		SetMetadata( pMetadata );
		delete pMetadata;
	}

	for ( POSITION pos = LibraryMaps.GetFileIterator() ; pos ; )
	{
		CLibraryFile* pFile = LibraryMaps.GetNextFile( pos );

		if ( pFile->IsAvailable() )
		{
			if ( validAndEqual( m_oCollSHA1, pFile->m_oSHA1 ) ||
				 pCollection->FindFile( pFile, TRUE ) ) AddFile( pFile );
		}
	}

	m_nUpdateCookie++;
	Library.m_nUpdateCookie++;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder fetch a colleciton

CCollectionFile* CAlbumFolder::GetCollection()
{
	if ( ! m_oCollSHA1 ) return NULL;
	if ( m_pCollection != NULL ) return m_pCollection;

	if ( CLibraryFile* pFile = LibraryMaps.LookupFileBySHA1( m_oCollSHA1, FALSE, TRUE ) )
	{
		m_pCollection = new CCollectionFile();

		if ( m_pCollection->Open( pFile->GetPath() ) )
		{
			return m_pCollection;
		}
		else
		{
			delete m_pCollection;
			m_pCollection = NULL;
		}
	}

	m_oCollSHA1.clear();
	m_nUpdateCookie++;
	Library.m_nUpdateCookie++;

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder organising

BOOL CAlbumFolder::OrganiseFile(CLibraryFile* pFile)
{
	BOOL bResult = FALSE;

	if ( pFile->IsGhost() )
	{
		if ( CheckURI( m_sSchemaURI, CSchema::uriGhostFolder ) )
		{
			AddFile( pFile );
			return TRUE;
		}
		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			bResult |= GetNextFolder( pos )->OrganiseFile( pFile );
		}
		return bResult;
	}

	if ( CheckURI( m_sSchemaURI, CSchema::uriAllFiles ) )
	{
		AddFile( pFile );
		return TRUE;
	}

	if ( m_oCollSHA1 && ( m_pCollection != NULL || GetCollection() ) )
	{
		if ( validAndEqual( m_oCollSHA1, pFile->m_oSHA1 ) ||
			 m_pCollection->FindFile( pFile, TRUE ) )
		{
			AddFile( pFile );
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	if ( pFile->m_pMetadata == NULL && m_pParent != NULL )
		return FALSE;

	if ( CheckURI( m_sSchemaURI, CSchema::uriMusicRoot ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) ) return FALSE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriMusicAlbumCollection ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) ) return FALSE;

		CString strAlbum = pFile->m_pMetadata->GetAttributeValue( _T("album") );
		CXMLNode::UniformString( strAlbum );

		if ( strAlbum.IsEmpty() ) return FALSE;
		if ( _tcsicmp( strAlbum, _T("tba") ) == 0 ) return FALSE;
		if ( _tcsicmp( strAlbum, _T("na") ) == 0 ) return FALSE;
		if ( _tcsicmp( strAlbum, _T("n/a") ) == 0 ) return FALSE;
		if ( _tcsicmp( strAlbum, _T("none") ) == 0 ) return FALSE;
		if ( _tcsicmp( strAlbum, _T("empty") ) == 0 ) return FALSE;
		if ( _tcsicmp( strAlbum, _T("unknown") ) == 0 ) return FALSE;
		if ( _tcsistr( strAlbum, _T("uploaded by") ) ) return FALSE;
		if ( _tcsistr( strAlbum, _T("ripped by") ) ) return FALSE;
		if ( _tcsistr( strAlbum, _T("downloaded") ) ) return FALSE;
		if ( _tcsistr( strAlbum, _T("http") ) ) return FALSE;
		if ( _tcsistr( strAlbum, _T("mp3") ) ) return FALSE;
		if ( _tcsistr( strAlbum, _T("www.mp3sfinder.com") ) ) return FALSE;
		if ( _tcsistr( strAlbum, _T("single") ) ) strAlbum = _T("Singles");

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			CAlbumFolder* pAlbum = GetNextFolder( pos );

			if ( pAlbum->m_sName.CompareNoCase( strAlbum ) == 0 )
				bResult = pAlbum->OrganiseFile( pFile );
			else if ( pAlbum->m_bAutoDelete )
				pAlbum->RemoveFile( pFile );
		}

		if ( bResult ) return TRUE;

		CAlbumFolder* pAlbum = AddFolder( CSchema::uriMusicAlbum, strAlbum, TRUE );

		return pAlbum->OrganiseFile( pFile );
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriMusicAlbum ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) ) return FALSE;

		CString strAlbum = pFile->m_pMetadata->GetAttributeValue( _T("album") );
		CXMLNode::UniformString( strAlbum );
		if ( _tcsistr( strAlbum, _T("single") ) ) strAlbum = _T("Singles");
		if ( strAlbum.CompareNoCase( m_sName ) ) return FALSE;

		AddFile( pFile );

		// ToDo: Scrap artist specific info !
		//if ( _tcsistr( m_sName, _T("soundtrack") ) != NULL || _tcsistr( m_sName, _T("ost") ) != NULL )
		//	MetaFromFile( pFile );
		//else
			MetaFromFile( pFile );

		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriMusicArtistCollection ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) ) return FALSE;

		CString strArtist = pFile->m_pMetadata->GetAttributeValue( _T("artist") );
		CXMLNode::UniformString( strArtist );

		strArtist.Replace( _T(" (www.mp3sfinder.com)"), _T("") );
		if ( strArtist.IsEmpty() ) return FALSE;

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			CAlbumFolder* pAlbum = GetNextFolder( pos );

			if ( pAlbum->m_sName.CompareNoCase( strArtist ) == 0 )
				bResult = pAlbum->OrganiseFile( pFile );
			else if ( pAlbum->m_bAutoDelete )
				pAlbum->RemoveFile( pFile );
		}

		if ( bResult ) return TRUE;

		CAlbumFolder* pAlbum = AddFolder( CSchema::uriMusicArtist, strArtist, TRUE );

		return pAlbum->OrganiseFile( pFile );
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriMusicArtist ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) ) return FALSE;

		CString strArtist = pFile->m_pMetadata->GetAttributeValue( _T("artist") );
		CXMLNode::UniformString( strArtist );
		if ( strArtist.CompareNoCase( m_sName ) ) return FALSE;

		AddFile( pFile );
		MetaFromFile( pFile );

		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriMusicGenreCollection ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) ) return FALSE;

		CString strGenre = pFile->m_pMetadata->GetAttributeValue( _T("genre") );
		if ( strGenre.IsEmpty() ) return FALSE;

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			CAlbumFolder* pAlbum = GetNextFolder( pos );

			if ( pAlbum->m_sName.CompareNoCase( strGenre ) == 0 )
				bResult = pAlbum->OrganiseFile( pFile );
			else if ( pAlbum->m_bAutoDelete )
				pAlbum->RemoveFile( pFile );
		}

		if ( bResult ) return TRUE;

		CAlbumFolder* pAlbum = AddFolder( CSchema::uriMusicGenre, strGenre, TRUE );

		return pAlbum->OrganiseFile( pFile );
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriMusicGenre ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) ) return FALSE;

		CString strGenre = pFile->m_pMetadata->GetAttributeValue( _T("genre") );
		if ( strGenre.CompareNoCase( m_sName ) ) return FALSE;

		AddFile( pFile );
		MetaFromFile( pFile );

		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriMusicAll ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriAudio ) ) return FALSE;
		AddFile( pFile );
		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriVideoRoot ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriVideo ) ) return FALSE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriVideoSeriesCollection ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriVideo ) ) return FALSE;

		CString strSeries = pFile->m_pMetadata->GetAttributeValue( _T("series") );
		CXMLNode::UniformString( strSeries );
		if ( strSeries.IsEmpty() )
		{
			using namespace regex;
			try
			{
				const rpattern firstPattern( L"(.*)(\\bse?a?s?o?n?)\\s*([0-9]+)\\s*(ep?i?s?o?d?e?)\\s*([0-9]+)[^0-9]+", NOCASE, MODE_SAFE );
				const rpattern secondPattern( L"(.*[^0-9]+\\b)([0-9]+)\\s*[xX]\\s*([0-9]+)[^0-9]+", NOFLAGS, MODE_SAFE );

				split_results splitResults;

				CString strNormalizedFileName = (LPCTSTR)pFile->m_sName;
				CXMLNode::UniformString( strNormalizedFileName );
				std::wstring sFileName( strNormalizedFileName );

				size_t nCount = firstPattern.split( sFileName, splitResults, 0 );
				std::vector<std::wstring> results = splitResults.strings();
				if ( nCount >= 6 &&
					 _tcsicmp( _tcsistr( L"season", results[2].c_str() ), L"season" ) == 0 &&
					 _tcsicmp( _tcsistr( L"episode", results[4].c_str() ), L"episode" ) == 0 )
				{
					std::vector<std::wstring>::iterator it =
						std::find( results.begin(), results.end(), results[2] );
					results.erase( it );
					it = std::find( results.begin(), results.end(), results[3] );
					results.erase( it );
					nCount -= 2;
				}
				else
					nCount = 0;

				if ( nCount < 4 && Settings.Library.SmartSeriesDetection )
				{
					nCount = secondPattern.split( sFileName, splitResults, 0 );
					results = splitResults.strings();
				}

				if ( nCount >= 4 )
				{
					CString strTemp( results[1].c_str() );
					strTemp.TrimRight( L"- " );

					if ( strTemp.IsEmpty() )
						return FALSE;

					pFile->m_pMetadata->AddAttribute( _T("series"), strTemp );
					strSeries = strTemp;

					CXMLAttribute* pAttribute = pFile->m_pMetadata->GetAttribute( _T("seriesnumber") );
					if ( pAttribute == NULL )
						pFile->m_pMetadata->AddAttribute( _T("seriesnumber"), results[2].c_str() );

					pAttribute = pFile->m_pMetadata->GetAttribute( _T("episodenumber") );
					if ( pAttribute == NULL )
						pFile->m_pMetadata->AddAttribute( _T("episodenumber"), results[3].c_str() );
				}
				else
				{
					return FALSE;
				}
			}
			catch (...)	{ return FALSE; };
		}

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			CAlbumFolder* pAlbum = GetNextFolder( pos );

			if ( pAlbum->m_sName.CompareNoCase( strSeries ) == 0 )
				bResult = pAlbum->OrganiseFile( pFile );
			else if ( pAlbum->m_bAutoDelete )
				pAlbum->RemoveFile( pFile );
		}

		if ( bResult ) return TRUE;

		CAlbumFolder* pAlbum = AddFolder( CSchema::uriVideoSeries, strSeries, TRUE );

		return pAlbum->OrganiseFile( pFile );
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriVideoSeries ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriVideo ) ) return FALSE;

		CString strSeries = pFile->m_pMetadata->GetAttributeValue( _T("series") );
		CXMLNode::UniformString( strSeries );
		if ( strSeries.CompareNoCase( m_sName ) ) return FALSE;

		AddFile( pFile );
		MetaFromFile( pFile );

		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriVideoFilmCollection ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriVideo ) ) return FALSE;

		CString strType = pFile->m_pMetadata->GetAttributeValue( _T("type") );
		if ( strType.CompareNoCase( _T("film") ) ) return FALSE;

		CString strTitle = pFile->m_pMetadata->GetAttributeValue( _T("title") );
		CXMLNode::UniformString( strTitle );
		if ( strTitle.IsEmpty() ) return FALSE;

		for ( POSITION pos = GetFolderIterator() ; pos ; )
		{
			CAlbumFolder* pAlbum = GetNextFolder( pos );

			if ( pAlbum->m_sName.CompareNoCase( strTitle ) == 0 )
				bResult = pAlbum->OrganiseFile( pFile );
			else if ( pAlbum->m_bAutoDelete )
				pAlbum->RemoveFile( pFile );
		}

		if ( bResult ) return TRUE;

		CAlbumFolder* pAlbum = AddFolder( CSchema::uriVideoFilm, strTitle, TRUE );

		return pAlbum->OrganiseFile( pFile );
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriVideoFilm ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriVideo ) ) return FALSE;

		CString strType = pFile->m_pMetadata->GetAttributeValue( _T("type") );
		if ( strType.CompareNoCase( _T("film") ) ) return FALSE;

		CString strTitle = pFile->m_pMetadata->GetAttributeValue( _T("title") );
		CXMLNode::UniformString( strTitle );
		if ( strTitle.CompareNoCase( m_sName ) ) return FALSE;

		AddFile( pFile );
		MetaFromFile( pFile );

		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriVideoMusicCollection ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriVideo ) ) return FALSE;

		CString strType = pFile->m_pMetadata->GetAttributeValue( _T("type") );
		if ( strType.CompareNoCase( _T("music video") ) ) return FALSE;

		AddFile( pFile );

		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriVideoAll ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriVideo ) ) return FALSE;
		AddFile( pFile );
		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriImageRoot ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriImage ) ) return FALSE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriImageAll ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriImage ) ) return FALSE;
		AddFile( pFile );
		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriApplicationRoot ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriApplication ) ) return FALSE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriApplicationAll ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriApplication ) ) return FALSE;
		AddFile( pFile );
		return TRUE;
	}

	else if ( CheckURI( m_sSchemaURI, CSchema::uriArchiveRoot ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriArchive ) ) return FALSE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriArchiveAll ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriArchive ) ) return FALSE;
		AddFile( pFile );
		return TRUE;
	}

	else if ( CheckURI( m_sSchemaURI, CSchema::uriBookRoot ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriBook ) ) return FALSE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriBookAll ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriBook ) ) return FALSE;
		AddFile( pFile );
		return TRUE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriDocumentRoot ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriDocument ) &&
			 ! pFile->IsSchemaURI( CSchema::uriSpreadsheet ) &&
			 ! pFile->IsSchemaURI( CSchema::uriPresentation ) ) return FALSE;
	}
	else if ( CheckURI( m_sSchemaURI, CSchema::uriDocumentAll ) )
	{
		if ( ! pFile->IsSchemaURI( CSchema::uriDocument ) &&
			 ! pFile->IsSchemaURI( CSchema::uriSpreadsheet ) &&
			 ! pFile->IsSchemaURI( CSchema::uriPresentation ) ) return FALSE;
		AddFile( pFile );
		return TRUE;
	}

	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		bResult |= GetNextFolder( pos )->OrganiseFile( pFile );
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder serialize

void CAlbumFolder::Serialize(CArchive& ar, int nVersion)
{
	POSITION pos;

	if ( ar.IsStoring() )
	{
		ar << m_sSchemaURI;

		ar.WriteCount( m_pXML != NULL ? 1 : 0 );
		if ( m_pXML ) m_pXML->Serialize( ar );
		SerializeOut( ar, m_oCollSHA1 );

		SerializeOut( ar, m_oGUID );

		ar << m_sName;
		ar << m_bExpanded;
		ar << m_bAutoDelete;
		ar << m_sBestView;

		ar.WriteCount( GetFolderCount() );

		for ( pos = GetFolderIterator() ; pos ; )
		{
			CAlbumFolder* pFolder = GetNextFolder( pos );
			pFolder->Serialize( ar, nVersion );
		}

		ar.WriteCount( GetFileCount() );

		for ( pos = GetFileIterator() ; pos ; )
		{
			CLibraryFile* pFile = GetNextFile( pos );
			ar << pFile->m_nIndex;
		}
	}
	else
	{
		CLibraryFile* pCollection = NULL;

		if ( m_pParent != NULL )
		{
			ar >> m_sSchemaURI;
			m_pSchema = SchemaCache.Get( m_sSchemaURI );
		}
		else
		{
			CString str;
			ar >> str;
		}

		if ( ar.ReadCount() )
		{
			ASSERT( m_pXML == NULL );
			m_pXML = new CXMLElement();
			m_pXML->Serialize( ar );
		}

		if ( nVersion >= 19 )
		{
			SerializeIn( ar, m_oCollSHA1, nVersion );
			pCollection = LibraryMaps.LookupFileBySHA1( m_oCollSHA1, FALSE, TRUE );
			// ToDo: Needs better validation.
			// Some collections are bound to URIs which assign the whole library as one big collection.
			if ( pCollection == NULL ||
				 m_pSchema && ( m_pSchema->CheckURI( CSchema::uriAllFiles ) ||
								m_pSchema->CheckURI( CSchema::uriGhostFolder ) ||
								m_pSchema->CheckURI( CSchema::uriApplicationRoot ) ||
								m_pSchema->CheckURI( CSchema::uriArchiveRoot ) ||
								m_pSchema->CheckURI( CSchema::uriBookRoot ) ||
								m_pSchema->CheckURI( CSchema::uriDocumentRoot ) ||
								m_pSchema->CheckURI( CSchema::uriImageRoot ) ||
								m_pSchema->CheckURI( CSchema::uriMusicRoot ) ||
								m_pSchema->CheckURI( CSchema::uriVideoRoot ) ||
								m_pSchema->CheckURI( CSchema::uriLibrary )
							  ) )
				m_oCollSHA1.clear();
		}

		if ( nVersion >= 24 )
			SerializeIn( ar, m_oGUID, nVersion );

		ar >> m_sName;
		ar >> m_bExpanded;
		ar >> m_bAutoDelete;

		if ( nVersion >= 9 ) ar >> m_sBestView;

		DWORD_PTR nCount = ar.ReadCount();

		while ( nCount-- > 0 )
		{
			auto_ptr< CAlbumFolder > pFolder( new CAlbumFolder( this, NULL, (LPCTSTR)1 ) );
			pFolder->Serialize( ar, nVersion );
			m_pFolders.AddTail( pFolder.release() );
		}

		nCount = ar.ReadCount();

		while ( nCount-- > 0 )
		{
			DWORD nIndex;
			ar >> nIndex;

			if ( CLibraryFile* pFile = Library.LookupFile( nIndex ) )
			{
				m_pFiles.AddTail( pFile );
				if ( pCollection != NULL )
					pFile->m_nCollIndex = pCollection->m_nIndex;
			}
		}
	}
}

bool CAlbumFolder::operator==(const CAlbumFolder& val) const
{
	return ( m_oGUID == val.m_oGUID );
}

//////////////////////////////////////////////////////////////////////
// CAlbumFolder clear

void CAlbumFolder::Clear()
{
	for ( POSITION pos = GetFolderIterator() ; pos ; )
	{
		delete GetNextFolder( pos );
	}

	m_pFolders.RemoveAll();
	m_pFiles.RemoveAll();

	if ( m_pXML != NULL ) delete m_pXML;
	m_pXML = NULL;

	if ( m_pCollection != NULL ) delete m_pCollection;
	m_pCollection = NULL;
}
