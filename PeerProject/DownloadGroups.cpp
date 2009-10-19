//
// DownloadGroups.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008
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
#include "DownloadGroup.h"
#include "DownloadGroups.h"
#include "Downloads.h"
#include "Download.h"
#include "Schema.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDownloadGroups DownloadGroups;


//////////////////////////////////////////////////////////////////////
// CDownloadGroups construction

CDownloadGroups::CDownloadGroups() :
	m_pSuper		( NULL ),
	m_nBaseCookie	( 1 ),
	m_nSaveCookie	( 0 ),
	m_nGroupCookie	( 0 )
{
}

CDownloadGroups::~CDownloadGroups()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups supergroup

CDownloadGroup* CDownloadGroups::GetSuperGroup()
{
	CQuickLock pLock( m_pSection );

	CString strCaption;
	LoadString( strCaption, IDS_GENERAL_ALL );

	if ( m_pSuper != NULL )
	{
		if ( m_pSuper->m_sName != strCaption )
			m_pSuper->m_sName = strCaption;
		return m_pSuper;
	}

	return m_pSuper = Add( (LPCTSTR)strCaption );
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups add group

CDownloadGroup* CDownloadGroups::Add(LPCTSTR pszName, BOOL bTemporary, BOOL bUseExisting)
{
	CQuickLock pLock( m_pSection );

	if ( bUseExisting )
	{
		for ( POSITION pos = m_pList.GetHeadPosition(); pos; )
		{
			CDownloadGroup* pGroup = m_pList.GetNext( pos );
			if ( ! pGroup->m_sName.CompareNoCase( pszName ) )
				return pGroup;
		}
	}

	CDownloadGroup* pGroup = new CDownloadGroup( pszName, bTemporary );
	m_pList.AddTail( pGroup );

	m_nBaseCookie ++;
	m_nGroupCookie ++;

	return pGroup;
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups remove group

void CDownloadGroups::Remove(CDownloadGroup* pGroup)
{
	CQuickLock pLock( m_pSection );

	if ( POSITION pos = m_pList.Find( pGroup ) )
	{
		if ( pGroup == m_pSuper ) return;
		m_pList.RemoveAt( pos );
		delete pGroup;
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups move group

void CDownloadGroups::MoveLeft(CDownloadGroup* pGroup)
{
	CSingleLock pLock( &m_pSection, TRUE );

	if ( POSITION pos = m_pList.Find( pGroup ) )
	{
		if ( pGroup == m_pSuper ) return;
		if ( pGroup == m_pList.GetAt( m_pList.FindIndex(1) ) ) return;

		POSITION pos2 = pos;
		m_pList.GetPrev( pos2 );
		m_pList.InsertBefore( pos2, pGroup );
		m_pList.RemoveAt( pos );

		Save();
		Load();
	}
}

void CDownloadGroups::MoveRight(CDownloadGroup* pGroup)
{
	CSingleLock pLock( &m_pSection, TRUE );

	if ( POSITION pos = m_pList.Find( pGroup ) )
	{
		if ( pGroup == m_pSuper ) return;
		if ( pGroup == m_pList.GetTail() ) return;

		// Simple move to end of list:
		//m_pList.AddTail( pGroup );
		//m_pList.RemoveAt( pos );

		POSITION pos2 = pos;
		m_pList.GetNext( pos2 );
		m_pList.InsertAfter( pos2, pGroup );
		m_pList.RemoveAt( pos );

		Save();
		Load();
	}
}


//////////////////////////////////////////////////////////////////////
// CDownloadGroups link a download to the appropriate groups

void CDownloadGroups::Link(CDownload* pDownload)
{
	CQuickLock pLock( m_pSection );

	GetSuperGroup()->Add( pDownload );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CDownloadGroup* pGroup = GetNext( pos );
		pGroup->Link( pDownload );
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups unlink a download from all groups

void CDownloadGroups::Unlink(CDownload* pDownload, BOOL bAndSuper)
{
	CQuickLock pLock( m_pSection );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CDownloadGroup* pGroup = GetNext( pos );
		if ( bAndSuper || pGroup != m_pSuper ) pGroup->Remove( pDownload );
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups default

void CDownloadGroups::CreateDefault()
{
	CQuickLock pLock( m_pSection );

	CDownloadGroup* pGroup	= GetSuperGroup();

	pGroup = Add( _T("Music") );
	pGroup->SetSchema( CSchema::uriAudio );
	pGroup->SetDefaultFilters();
	pGroup->SetFolder( Settings.Downloads.CompletePath + "\\Music" );

	pGroup = Add( _T("Video") );
	pGroup->SetSchema( CSchema::uriVideo );
	pGroup->SetDefaultFilters();
	pGroup->SetFolder( Settings.Downloads.CompletePath + "\\Video" );

//	pGroup = Add( _T("Images") );
//	pGroup->SetSchema( CSchema::uriImage );
//	pGroup->SetDefaultFilters();
//	pGroup->SetFolder( Settings.Downloads.CompletePath + "\\Images" );

	pGroup = Add( _T("BitTorrent") );
	pGroup->m_bTorrent = TRUE;
	pGroup->SetSchema( CSchema::uriBitTorrent );
	pGroup->SetFolder( Settings.Downloads.CompletePath + "\\BitTorrent" );

//	pGroup = Add( _T("Collection") );
//	pGroup->SetSchema( CSchema::uriCollection );
//	pGroup->SetDefaultFilters();
//	pGroup->SetFolder( Settings.Downloads.CollectionPath );

	pGroup = Add( _T("Custom") );
	pGroup->SetFolder( Settings.Downloads.CompletePath + "\\Custom" );
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups completed path

CString CDownloadGroups::GetCompletedPath(CDownload* pDownload)
{
	CQuickLock pLock( m_pSection );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CDownloadGroup* pGroup = GetNext( pos );

		if ( pGroup != m_pSuper && pGroup->Contains( pDownload ) )
		{
			if ( pGroup->m_sFolder.GetLength() ) return pGroup->m_sFolder;
		}
	}

	return Settings.Downloads.CompletePath;
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups clear

void CDownloadGroups::Clear()
{
	CQuickLock pLock( m_pSection );

	for ( POSITION pos = GetIterator() ; pos ; )
		delete GetNext( pos );
	m_pList.RemoveAll();

	m_pSuper = NULL;
	m_nBaseCookie ++;
	m_nGroupCookie ++;
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups load and save

BOOL CDownloadGroups::Load()
{
	CQuickLock pLock( m_pSection );

	CFile pFile;
	CString strPath = Settings.General.UserPath + _T("\\Data\\DownloadGroups.dat");
	if ( ! pFile.Open( strPath, CFile::modeRead ) ) return FALSE;

	try
	{
		CArchive ar( &pFile, CArchive::load );
		Serialize( ar );
	}
	catch ( CException* pException )
	{
		pException->Delete();
		return FALSE;
	}

	m_nSaveCookie = m_nBaseCookie;

	return TRUE;
}

BOOL CDownloadGroups::Save(BOOL bForce)
{
	CQuickLock pLock( m_pSection );

	if ( ! bForce && m_nBaseCookie == m_nSaveCookie ) return FALSE;
	m_nSaveCookie = m_nBaseCookie;

	CString strPath = Settings.General.UserPath + _T("\\Data\\DownloadGroups.dat");
	DeleteFileEx( strPath + _T(".tmp"), FALSE, FALSE, FALSE );

	CFile pFile;
	if ( ! pFile.Open( strPath + _T(".tmp"), CFile::modeWrite | CFile::modeCreate ) ) return FALSE;

	BYTE* pBuffer = new BYTE[ 4096 ];

	try
	{
		CArchive ar( &pFile, CArchive::store, 4096, pBuffer );
		Serialize( ar );
	}
	catch ( CException* pException )
	{
		delete [] pBuffer;
		pException->Delete();
		return FALSE;
	}

	delete [] pBuffer;

	pFile.Close();

	DeleteFileEx( strPath, FALSE, FALSE, FALSE );
	MoveFile( strPath + _T(".tmp"), strPath );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups serialize

#define GROUPS_SER_VERSION	1000	//7
// nVersion History:
// 4 - Added m_bTemporary (ryo-oh-ki)
// 5 - New download groups added (Image, Collection, etc.)
// 6 - ?
// 7 - Added m_bTorrent (ryo-oh-ki), fixed collection schema
// 1000 - (PeerProject 1.0) (7)

void CDownloadGroups::Serialize(CArchive& ar)
{
	int nVersion = GROUPS_SER_VERSION;
	BYTE nState;

	if ( ar.IsStoring() )
	{
		CleanTemporary();

		ar << nVersion;

		ar.WriteCount( Downloads.GetCount() );

		for ( POSITION pos = Downloads.GetIterator() ; pos ; )
		{
			ar << Downloads.GetNext( pos )->m_nSerID;
		}

		ar.WriteCount( GetCount() );

		for ( POSITION pos = GetIterator() ; pos ; )
		{
			CDownloadGroup* pGroup = GetNext( pos );

			nState = ( pGroup == m_pSuper ) ? 1 : 0;
			ar << nState;

			pGroup->Serialize( ar, nVersion );
		}
	}
	else
	{
		ar >> nVersion;
		if ( nVersion < 2 || nVersion > GROUPS_SER_VERSION ) AfxThrowUserException();

		DWORD_PTR nCount = ar.ReadCount();

		for ( ; nCount > 0 ; nCount-- )
		{
			DWORD nDownload;
			ar >> nDownload;
			if ( CDownload* pDownload = Downloads.FindBySID( nDownload ) )
				Downloads.Reorder( pDownload, NULL );
		}

		if ( ( nCount = ar.ReadCount() ) != 0 ) Clear();

		for ( ; nCount > 0 ; nCount-- )
		{
			CDownloadGroup* pGroup = Add();

			ar >> nState;
			if ( nState == 1 ) m_pSuper = pGroup;

			pGroup->Serialize( ar, nVersion );
		}

		GetSuperGroup();

		for ( POSITION pos = Downloads.GetIterator() ; pos ; )
		{
			m_pSuper->Add( Downloads.GetNext( pos ) );
		}
	}
}

void CDownloadGroups::CleanTemporary()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		POSITION posCurrent = pos;
		CDownloadGroup* pGroup = GetNext( pos );
		if ( pGroup->IsTemporary() )
		{
			ASSERT( pGroup != m_pSuper );

			m_pList.RemoveAt( posCurrent );
			delete pGroup;

			m_nBaseCookie ++;
			m_nGroupCookie ++;

			pos = GetIterator();
		}
	}
}
