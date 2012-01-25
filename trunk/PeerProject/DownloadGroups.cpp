//
// DownloadGroups.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "DownloadGroups.h"
#include "DownloadGroup.h"
#include "Downloads.h"
#include "Download.h"
#include "Schema.h"
#include "Transfers.h"	// Locks

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CDownloadGroups DownloadGroups;


//////////////////////////////////////////////////////////////////////
// CDownloadGroups construction

CDownloadGroups::CDownloadGroups()
	: m_pSuper		( NULL )
	, m_nBaseCookie	( 1 )
	, m_nSaveCookie	( 0 )
	, m_nGroupCookie( 0 )
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

	if ( ! m_pSuper )
		return m_pSuper = Add( (LPCTSTR)strCaption );

	if ( m_pSuper->m_sName != strCaption )
		m_pSuper->m_sName = strCaption;

	return m_pSuper;
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups add group

CDownloadGroup* CDownloadGroups::Add(LPCTSTR pszName, BOOL bTemporary, BOOL bUseExisting)
{
	CQuickLock pLock( m_pSection );

	if ( bUseExisting )
	{
		for ( POSITION pos = m_pList.GetHeadPosition() ; pos ; )
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
// CDownloadGroups reorder tab

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
	ASSUME_LOCK( Transfers.m_pSection );
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
	ASSUME_LOCK( Transfers.m_pSection );
	CQuickLock pLock( m_pSection );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CDownloadGroup* pGroup = GetNext( pos );
		if ( bAndSuper || pGroup != m_pSuper )
			pGroup->Remove( pDownload );
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups default

void CDownloadGroups::CreateDefault()
{
	CQuickLock pLock( m_pSection );

	CDownloadGroup* pGroup	= GetSuperGroup();

	BOOL bEng = Settings.General.Language.Left(2) == _T("en");	// Folder naming fixes

	pGroup = Add( _T("DEFAULT") );			// DEFAULT Names are translated by Schema
	pGroup->SetSchema( CSchema::uriAudio );
	pGroup->SetFolder( pGroup->m_sName );	// Settings.Downloads.CompletePath + "\\Audio"
	pGroup->SetDefaultFilters();

	pGroup = Add( _T("DEFAULT") );
	pGroup->SetSchema( CSchema::uriVideo );
	pGroup->SetFolder( pGroup->m_sName );
	pGroup->SetDefaultFilters();

	pGroup = Add( bEng ? _T("Images") : _T("DEFAULT") );
	pGroup->SetSchema( CSchema::uriImage );
	pGroup->SetFolder( bEng ? _T("Images") : pGroup->m_sName );
	pGroup->SetDefaultFilters();

	pGroup = Add( bEng ? _T("Documents") : _T("DEFAULT") );
	pGroup->SetSchema( CSchema::uriBook );
	pGroup->SetFolder( bEng ? _T("Documents") : pGroup->m_sName );
	pGroup->SetDefaultFilters();

	pGroup = Add( bEng ? _T("Archives") :_T("DEFAULT") );
	pGroup->SetSchema( CSchema::uriArchive );
	pGroup->SetFolder( bEng ? _T("Archives") : pGroup->m_sName );
	pGroup->SetDefaultFilters();

	pGroup = Add( _T("BitTorrent") );
	pGroup->SetSchema( CSchema::uriBitTorrent );
	pGroup->SetFolder( _T("BitTorrent") );
	pGroup->SetDefaultFilters();
	//pGroup->m_bTorrent = TRUE;	// Obsolete, Schema is detected

// ToDo: Popularize Collections and re-add this group
//	pGroup = Add( _T("Collection") );
//	pGroup->SetSchema( CSchema::uriCollection );
//	pGroup->SetFolder( Settings.Downloads.CollectionPath );
//	pGroup->SetDefaultFilters();

	CString str; // "Custom"
	LoadString( str, IDS_GENERAL_CUSTOM );
	pGroup = Add( str );
	pGroup->SetFolder( str );
	//pGroup->AddFilter( _T("PeerProject") );
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups completed path

CString CDownloadGroups::GetCompletedPath(CDownload* pDownload)
{
	if ( Settings.Downloads.CompletePath.GetLength() < 3 )
		Settings.Downloads.CompletePath = Settings.General.Path + _T("\\Downloads");

	CQuickLock pLock( m_pSection );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CDownloadGroup* pGroup = GetNext( pos );

		if ( pGroup != m_pSuper && pGroup->Contains( pDownload ) )
		{
			if ( ! pGroup->m_sFolder.IsEmpty() )
			{
				if ( pGroup->m_sFolder.Find( _T(":\\") ) == 1 )
					return pGroup->m_sFolder;

				return Settings.Downloads.CompletePath + _T("\\") + pGroup->m_sFolder;
			}
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
	const CString strFile = Settings.General.UserPath + _T("\\Data\\DownloadGroups.dat");

	CFile pFile;
	if ( ! pFile.Open( strFile, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan ) )
	{
		theApp.Message( MSG_ERROR, _T("Failed to load download groups: %s"), strFile );
		return FALSE;
	}

	try
	{
		CArchive ar( &pFile, CArchive::load );	// 4 KB buffer
		try
		{
			CQuickLock pTransfersLock( Transfers.m_pSection );
			CQuickLock pLock( m_pSection );

			Serialize( ar );
			ar.Close();
		}
		catch ( CException* pException )
		{
			ar.Abort();
			pFile.Abort();
			pException->Delete();
			theApp.Message( MSG_ERROR, _T("Failed to load download groups: %s"), strFile );
		}
		pFile.Close();
	}
	catch ( CException* pException )
	{
		pFile.Abort();
		pException->Delete();
		theApp.Message( MSG_ERROR, _T("Failed to load download groups: %s"), strFile );
	}

	m_nSaveCookie = m_nBaseCookie;

	return TRUE;
}

BOOL CDownloadGroups::Save(BOOL bForce)
{
	if ( ! bForce && m_nBaseCookie == m_nSaveCookie )
		return FALSE;

	const CString strFile = Settings.General.UserPath + _T("\\Data\\DownloadGroups.dat");
	const CString strTemp = Settings.General.UserPath + _T("\\Data\\DownloadGroups.tmp");

	CFile pFile;
	if ( ! pFile.Open( strTemp, CFile::modeWrite | CFile::modeCreate | CFile::shareExclusive | CFile::osSequentialScan ) )
	{
		DeleteFile( strTemp );
		theApp.Message( MSG_ERROR, _T("Failed to save download groups: %s"), strTemp );
		return FALSE;
	}

	try
	{
		CArchive ar( &pFile, CArchive::store );	// 4 KB buffer
		try
		{
			CQuickLock pTransfersLock( Transfers.m_pSection );
			CQuickLock pLock( m_pSection );

			Serialize( ar );
			ar.Close();

			m_nSaveCookie = m_nBaseCookie;
		}
		catch ( CException* pException )
		{
			ar.Abort();
			pFile.Abort();
			pException->Delete();
			theApp.Message( MSG_ERROR, _T("Failed to save download groups: %s"), strTemp );
			return FALSE;
		}
		pFile.Close();
	}
	catch ( CException* pException )
	{
		pFile.Abort();
		pException->Delete();
		theApp.Message( MSG_ERROR, _T("Failed to save download groups: %s"), strTemp );
		return FALSE;
	}

	if ( ! MoveFileEx( strTemp, strFile, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING ) )
	{
		DeleteFile( strTemp );
		theApp.Message( MSG_ERROR, _T("Failed to save download groups: %s"), strFile );
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadGroups serialize

#define GROUPS_SER_VERSION	1000	// 7
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
	else // Loading
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

		if ( ( nCount = ar.ReadCount() ) != 0 )
			Clear();

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

			m_nBaseCookie++;
			m_nGroupCookie++;

			pos = GetIterator();
		}
	}
}
