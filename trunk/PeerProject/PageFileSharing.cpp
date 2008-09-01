//
// PageFileSharing.cpp
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
#include "Library.h"
#include "SharedFile.h"
#include "PageFileSharing.h"

#include "Transfers.h"
#include "UploadQueue.h"
#include "UploadQueues.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CFileSharingPage, CFilePropertiesPage)

BEGIN_MESSAGE_MAP(CFileSharingPage, CFilePropertiesPage)
	//{{AFX_MSG_MAP(CFileSharingPage)
	ON_BN_CLICKED(IDC_SHARE_OVERRIDE_0, OnShareOverride0)
	ON_BN_CLICKED(IDC_SHARE_OVERRIDE_1, OnShareOverride1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileSharingPage property page

CFileSharingPage::CFileSharingPage() : 
	CFilePropertiesPage(CFileSharingPage::IDD), m_bOverride( -1 ), 
	m_bShare( FALSE ), m_sTags()
{
}

CFileSharingPage::~CFileSharingPage()
{
}

void CFileSharingPage::DoDataExchange(CDataExchange* pDX)
{
	CFilePropertiesPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileSharingPage)
	DDX_Control(pDX, IDC_FILE_TAGS, m_wndTags);
	DDX_Control(pDX, IDC_FILE_SHARE, m_wndShare);
	DDX_Control(pDX, IDC_FILE_NETWORKS, m_wndNetworks);
	DDX_Radio(pDX, IDC_SHARE_OVERRIDE_0, m_bOverride);
	DDX_Check(pDX, IDC_FILE_SHARE, m_bShare);
	DDX_CBString(pDX, IDC_FILE_TAGS, m_sTags);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFileSharingPage message handlers

BOOL CFileSharingPage::OnInitDialog()
{
	CFilePropertiesPage::OnInitDialog();

	m_wndTags.AddString( _T("") );

	if ( UploadQueues.m_pSection.Lock() )
	{
		CList< CString > pAdded;

		for ( POSITION pos = UploadQueues.GetIterator() ; pos ; )
		{
			CUploadQueue* pQueue = UploadQueues.GetNext( pos );

			if ( pQueue->m_sShareTag.GetLength() )
			{
				if ( pAdded.Find( pQueue->m_sShareTag ) == NULL )
				{
					pAdded.AddTail( pQueue->m_sShareTag );
					m_wndTags.AddString( pQueue->m_sShareTag );
				}
			}
		}

		UploadQueues.m_pSection.Unlock();

		if ( pAdded.IsEmpty() )
		{
			m_wndTags.AddString( _T("Release") );
			m_wndTags.AddString( _T("Popular") );
		}
	}

	{
		CQuickLock oLock( Library.m_pSection );

		if ( CLibraryFile* pFile = GetFile() )
		{
			m_bOverride	= pFile->m_bShared != TRI_UNKNOWN;
			m_bShare	= pFile->IsShared();
			m_sTags		= pFile->m_sShareTags;
		}
		else if ( CLibraryList* pList = GetList() )
		{
			for ( POSITION pos = pList->GetIterator() ; pos ; )
			{
				if ( CLibraryFile* pFile = pList->GetNextFile( pos ) )
				{
					m_bOverride	= pFile->m_bShared != TRI_UNKNOWN;
					m_bShare	= pFile->IsShared();
					m_sTags		= pFile->m_sShareTags;
				}
			}

		}
	}

	UpdateData( FALSE );
	m_wndShare.EnableWindow( m_bOverride );

	return TRUE;
}

void CFileSharingPage::OnShareOverride0()
{
	UpdateData();

	m_wndShare.EnableWindow( m_bOverride );

	if ( ! m_bOverride )
	{
		CSingleLock oLock( &Library.m_pSection, TRUE );
		if ( CLibraryFile* pFile = GetFile() )
		{
			TRISTATE bSave = pFile->m_bShared;
			pFile->m_bShared = TRI_UNKNOWN;
			m_bShare = pFile->IsShared();
			pFile->m_bShared = bSave;

			oLock.Unlock();
			UpdateData( FALSE );
		}
	}
}

void CFileSharingPage::OnShareOverride1()
{
	OnShareOverride0();
}

void CFileSharingPage::OnOK()
{
	UpdateData();

	if ( CLibraryList* pList = GetList() )
	{
		CQuickLock oLock( Library.m_pSection );

		for ( POSITION pos = pList->GetIterator() ; pos ; )
		{
			if ( CLibraryFile* pFile = pList->GetNextFile( pos ) )
			{
				if ( m_bOverride )
				{
					pFile->m_bShared = m_bShare ? TRI_TRUE : TRI_FALSE;
				}
				else
				{
					pFile->m_bShared = TRI_UNKNOWN;
				}

				pFile->m_sShareTags = m_sTags;
			}
		}

	}

	CFilePropertiesPage::OnOK();
}
