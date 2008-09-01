//
// PageProfileFiles.cpp
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
#include "Library.h"
#include "LibraryFolders.h"
#include "AlbumFolder.h"
#include "SharedFile.h"
#include "Schema.h"
#include "ShellIcons.h"
#include "PageProfileFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CFilesProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CFilesProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CFilesProfilePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFilesProfilePage property page

CFilesProfilePage::CFilesProfilePage() : CSettingsPage( CFilesProfilePage::IDD )
{
	//{{AFX_DATA_INIT(CFilesProfilePage)
	//}}AFX_DATA_INIT
}

CFilesProfilePage::~CFilesProfilePage()
{
}

void CFilesProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilesProfilePage)
	DDX_Control(pDX, IDC_FILE_LIST, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFilesProfilePage message handlers

BOOL CFilesProfilePage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	CRect rc;
	m_wndList.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL ) + 1;
	m_wndList.InsertColumn( 0, _T("File"), LVCFMT_LEFT, rc.right, -1 );
	m_wndList.SetImageList( ShellIcons.GetObject( 16 ), LVSIL_SMALL );

	{
		CQuickLock oLock( Library.m_pSection );

		CAlbumFolder* pFolder = LibraryFolders.GetAlbumTarget( CSchema::uriFavouritesFolder, _T("Title"), NULL );

		if ( pFolder != NULL )
		{
			for ( POSITION pos = pFolder->GetFileIterator() ; pos ; )
			{
				CLibraryFile* pFile = pFolder->GetNextFile( pos );

				if ( pFile->IsShared() )
				{
					m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, m_wndList.GetItemCount(),
						pFile->m_sName, 0, 0, ShellIcons.Get( pFile->m_sName, 16 ), pFile->m_nIndex );
				}
			}
		}
	}

	UpdateData( FALSE );

	return TRUE;
}

void CFilesProfilePage::OnOK()
{
	CSettingsPage::OnOK();
}
