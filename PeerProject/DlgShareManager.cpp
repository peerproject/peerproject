//
// DlgShareManager.cpp
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

#include "DlgShareManager.h"

#include "DlgFolderScan.h"
#include "Library.h"
#include "LibraryFolders.h"
#include "SharedFolder.h"
#include "ShellIcons.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CShareManagerDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CShareManagerDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CShareManagerDlg)
	ON_BN_CLICKED(IDC_SHARE_ADD, OnShareAdd)
	ON_BN_CLICKED(IDC_SHARE_REMOVE, OnShareRemove)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SHARE_FOLDERS, OnItemChangedShareFolders)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CShareManagerDlg dialog

CShareManagerDlg::CShareManagerDlg(CWnd* pParent) : CSkinDialog( CShareManagerDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CShareManagerDlg)
	//}}AFX_DATA_INIT
}

void CShareManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShareManagerDlg)
	DDX_Control(pDX, IDC_SHARE_REMOVE, m_wndRemove);
	DDX_Control(pDX, IDC_SHARE_FOLDERS, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CShareManagerDlg message handlers

BOOL CShareManagerDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( NULL, IDR_LIBRARYFRAME );

	CRect rc;
	m_wndList.GetClientRect( &rc );
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP|LVS_EX_CHECKBOXES );
	m_wndList.InsertColumn( 0, _T("Folder"), LVCFMT_LEFT, rc.right - GetSystemMetrics( SM_CXVSCROLL ) );
	m_wndList.SetImageList( ShellIcons.GetObject( 16 ), LVSIL_SMALL );

	m_wndList.EnableToolTips( TRUE );

	{
		CQuickLock oLock( Library.m_pSection );

		for ( POSITION pos = LibraryFolders.GetFolderIterator() ; pos ; )
		{
			CLibraryFolder* pFolder = LibraryFolders.GetNextFolder( pos );

			m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE, m_wndList.GetItemCount(),
				pFolder->m_sPath, 0, 0, SHI_FOLDER_OPEN, 0 );

			m_wndList.SetItemState( m_wndList.GetItemCount() - 1 ,
				UINT( ( pFolder->IsShared() != TRUE ? 1 : 2 ) << 12 ), LVIS_STATEIMAGEMASK );
		}
	}

	m_wndRemove.EnableWindow( FALSE );

	return TRUE;
}

void CShareManagerDlg::OnItemChangedShareFolders(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;
	m_wndRemove.EnableWindow( m_wndList.GetSelectedCount() > 0 );
}

void CShareManagerDlg::OnShareAdd()
{
	LibraryFolders.AddSharedFolder( m_wndList );

	m_wndList.SetItemState( m_wndList.GetItemCount() - 1 , 2 << 12, LVIS_STATEIMAGEMASK );	// Checked box
}

void CShareManagerDlg::OnShareRemove()
{
	for ( int nItem = 0 ; nItem < m_wndList.GetItemCount() ; nItem++ )
	{
		if ( m_wndList.GetItemState( nItem, LVIS_SELECTED ) )
			m_wndList.DeleteItem( nItem-- );
	}
}

void CShareManagerDlg::OnOK()
{
	{
		CQuickLock oLock( Library.m_pSection );

		for ( POSITION pos = LibraryFolders.GetFolderIterator() ; pos ; )
		{
			CLibraryFolder* pFolder = LibraryFolders.GetNextFolder( pos );

			int nItem = 0;
			for ( ; nItem < m_wndList.GetItemCount() ; nItem++ )
			{
				CString strFolder = m_wndList.GetItemText( nItem, 0 );
				if ( strFolder.CompareNoCase( pFolder->m_sPath ) == 0 )
				{	
					if ( m_wndList.GetCheck(nItem) && ! pFolder->IsShared() )
						pFolder->SetShared( TRI_TRUE );
					else if ( ! m_wndList.GetCheck(nItem) && pFolder->IsShared() )
						pFolder->SetShared( TRI_FALSE );
					
					break;
				}
			}

			if ( nItem >= m_wndList.GetItemCount() )
			{
				LibraryFolders.RemoveFolder( pFolder );
			}
		}

		for ( int nItem = 0 ; nItem < m_wndList.GetItemCount() ; nItem++ )
		{
			LibraryFolders.AddFolder( m_wndList.GetItemText( nItem, 0 ) );
		}
	}

	CFolderScanDlg dlgScan;
	dlgScan.DoModal();

	CDialog::OnOK();
}
