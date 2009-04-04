//
// PagePackage.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#include "StdAfx.h"
#include "TorrentWizard.h"
#include "PagePackage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPackagePage, CWizardPage)

BEGIN_MESSAGE_MAP(CPackagePage, CWizardPage)
	//{{AFX_MSG_MAP(CPackagePage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnItemChangedFileList)
	ON_BN_CLICKED(IDC_ADD_FOLDER, OnAddFolder)
	ON_BN_CLICKED(IDC_ADD_FILE, OnAddFile)
	ON_BN_CLICKED(IDC_REMOVE_FILE, OnRemoveFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPackagePage property page

CPackagePage::CPackagePage() : CWizardPage(CPackagePage::IDD)
{
	//{{AFX_DATA_INIT(CPackagePage)
	//}}AFX_DATA_INIT
	m_hImageList = NULL;
}

CPackagePage::~CPackagePage()
{
}

void CPackagePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPackagePage)
	DDX_Control(pDX, IDC_REMOVE_FILE, m_wndRemove);
	DDX_Control(pDX, IDC_FILE_LIST, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPackagePage message handlers

BOOL CPackagePage::OnInitDialog() 
{
	CWizardPage::OnInitDialog();
	
	CRect rc;
	m_wndList.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL );
	m_wndList.InsertColumn( 0, _T("Filename"), LVCFMT_LEFT, rc.right - 80, -1 );
	m_wndList.InsertColumn( 1, _T("Size"), LVCFMT_RIGHT, 80, 0 );
	
	return TRUE;
}

void CPackagePage::OnReset() 
{
}

BOOL CPackagePage::OnSetActive() 
{
	m_wndRemove.EnableWindow( m_wndList.GetSelectedCount() > 0 );
	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

LRESULT CPackagePage::OnWizardBack() 
{
	return IDD_WELCOME_PAGE;
}

LRESULT CPackagePage::OnWizardNext() 
{
	if ( m_wndList.GetItemCount() == 0 )
	{
		AfxMessageBox( IDS_PACKAGE_NEED_FILES, MB_ICONEXCLAMATION );
		return -1;
	}
	
	return IDD_TRACKER_PAGE;
}

void CPackagePage::OnItemChangedFileList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	m_wndRemove.EnableWindow( m_wndList.GetSelectedCount() > 0 );
}

void CPackagePage::OnAddFolder() 
{
	TCHAR szPath[MAX_PATH];
	LPITEMIDLIST pPath;
	LPMALLOC pMalloc;
	BROWSEINFO pBI;
	
	ZeroMemory( &pBI, sizeof(pBI) );
	pBI.hwndOwner		= GetSafeHwnd();
	pBI.pszDisplayName	= szPath;
	pBI.lpszTitle		= _T("Add a folder:");
	pBI.ulFlags			= BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	
	pPath = SHBrowseForFolder( &pBI );
	if ( pPath == NULL ) return;
	
	SHGetPathFromIDList( pPath, szPath );
	SHGetMalloc( &pMalloc );
	pMalloc->Free( pPath );
	pMalloc->Release();
	
	CWaitCursor wc;
	AddFolder( szPath, 0 );
}

void CPackagePage::OnAddFile() 
{
	CFileDialog dlg( TRUE, NULL, NULL,
		OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING,
		_T("All Files|*.*||"), this );
	
	TCHAR szFiles[81920] = { 0 };
	dlg.m_ofn.lpstrFile	= szFiles;
	dlg.m_ofn.nMaxFile	= 81920;
	
	if ( dlg.DoModal() != IDOK ) return;
	
	CWaitCursor wc;
	CString strFolder	= CString( szFiles );
	LPCTSTR pszFile		= szFiles + strFolder.GetLength() + 1;
	
	if ( *pszFile )
	{
		for ( strFolder += '\\' ; *pszFile ; )
		{
			AddFile( strFolder + pszFile );
			pszFile += _tcslen( pszFile ) + 1;
		}
	}
	else
	{
		AddFile( strFolder );
	}
}

void CPackagePage::OnRemoveFile() 
{
	CWaitCursor wc;

	for ( int nItem = m_wndList.GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		if ( m_wndList.GetItemState( nItem, LVIS_SELECTED ) )
		{
			m_wndList.DeleteItem( nItem );
			UpdateWindow();
		}
	}
}

void CPackagePage::AddFile(LPCTSTR pszFile)
{
	HANDLE hFile = CreateFile( pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		CString strFormat, strMessage;
		strFormat.LoadString( IDS_PACKAGE_CANT_OPEN );
		strMessage.Format( strFormat, pszFile );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		return;
	}
	
	DWORD nLow, nHigh;
	nLow = GetFileSize( hFile, &nHigh );
	QWORD nSize = ( (QWORD)nHigh << 32 ) + (QWORD)nLow;
	CloseHandle( hFile );
	
	SHFILEINFO pInfo;
	ZeroMemory( &pInfo, sizeof(pInfo) );
	
	HIMAGELIST hIL = (HIMAGELIST)SHGetFileInfo( pszFile, 0, &pInfo, sizeof(pInfo),
		SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
	
	if ( hIL != NULL && m_hImageList == NULL )
	{
		m_hImageList = hIL;
		CImageList pTemp;
		pTemp.Attach( hIL );
		m_wndList.SetImageList( &pTemp, LVSIL_SMALL );
		pTemp.Detach();
	}
	
	int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE, m_wndList.GetItemCount(),
		pszFile, 0, 0, pInfo.iIcon, NULL );
	
	m_wndList.SetItemText( nItem, 1, SmartSize( nSize ) );

	UpdateWindow();
}

void CPackagePage::AddFolder(LPCTSTR pszPath, int nRecursive)
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	HANDLE hSearch;
	
	strPath.Format( _T("%s\\*.*"), pszPath );
	
	hSearch = FindFirstFile( strPath, &pFind );
	
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( pFind.cFileName[0] == '.' ||
				 pFind.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) 
				 continue;
			
			strPath.Format( _T("%s\\%s"), pszPath, pFind.cFileName );
			
			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( nRecursive == 0 )
				{
					UINT nResp = AfxMessageBox( IDS_PACKAGE_RECURSIVE, MB_ICONQUESTION|MB_YESNOCANCEL );
					if ( nResp == IDYES ) nRecursive = 2;
					else if ( nResp == IDNO ) nRecursive = 1;
					else break;
				}
				
				if ( nRecursive == 2 ) AddFolder( strPath, 2 );
			}
			else
			{
				AddFile( strPath );
			}
		}
		while ( FindNextFile( hSearch, &pFind ) );
		
		FindClose( hSearch );
	}
}
