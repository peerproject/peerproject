//
// PageExpert.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008-2011
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
#include "TorrentBuilder.h"
#include "PageExpert.h"
#include "PagePackage.h"
#include "PageSingle.h"
#include "PageTracker.h"
#include "PageComment.h"
#include "PageOutput.h"
#include "PageFinished.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CExpertPage, CWizardPage)

BEGIN_MESSAGE_MAP(CExpertPage, CWizardPage)
	//{{AFX_MSG_MAP(CExpertPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnItemChangedFileList)
	ON_BN_CLICKED(IDC_BROWSE_FOLDER, OnBrowseFolder)
	ON_BN_CLICKED(IDC_ADD_FOLDER, OnAddFolder)
	ON_BN_CLICKED(IDC_ADD_FILE, OnAddFile)
	ON_BN_CLICKED(IDC_REMOVE_FILE, OnRemoveFile)
	ON_WM_XBUTTONDOWN()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExpertPage property page

CExpertPage::CExpertPage() : CWizardPage(CExpertPage::IDD)
{
	//{{AFX_DATA_INIT(CExpertPage)
	m_hImageList = NULL;
	//}}AFX_DATA_INIT
}

CExpertPage::~CExpertPage()
{
}

void CExpertPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpertPage)
	DDX_Control(pDX, IDC_FILE_LIST, m_wndList);
	DDX_Control(pDX, IDC_FOLDER, m_wndFolders);
//	DDX_Control(pDX, IDC_TORRENT_NAME, m_wndName);
	DDX_Control(pDX, IDC_REMOVE_FILE, m_wndRemove);
	DDX_Control(pDX, IDC_TRACKER, m_wndTracker);
	DDX_Control(pDX, IDC_TRACKER2, m_wndTracker2);
	DDX_CBString(pDX, IDC_TRACKER, m_sTracker);
	DDX_CBString(pDX, IDC_TRACKER2, m_sTracker2);
	DDX_CBString(pDX, IDC_FOLDER, m_sFolder);
	DDX_Text(pDX, IDC_TORRENT_NAME, m_sName);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_FILECOUNT, m_sFileCount);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CExpertPage message handlers

BOOL CExpertPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	this->DragAcceptFiles(TRUE);

	m_nTotalSize = 0;
	m_sFileCount = _T("No Files.");
	m_sName = _T("");
	m_sFolder = _T("");
	m_sTracker = _T("");
	m_sTracker2 = _T("");
	m_sComment = _T("");

	CRect rc;
	m_wndList.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL );
	m_wndList.SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_LABELTIP|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	m_wndList.InsertColumn( 0, _T("Filename"), LVCFMT_LEFT, rc.right - 64, -1 );
	m_wndList.InsertColumn( 1, _T("Size"), LVCFMT_RIGHT, 64, 0 );
	m_wndList.InsertColumn( 2, _T("Bytes"), LVCFMT_RIGHT, 0, 0 );

	int nCount = theApp.GetProfileInt( _T("Folders"), _T("Count"), 0 );
	for ( int nItem = 0 ; nItem < nCount ; nItem++ )
	{
		CString strName, strURL;
		strName.Format( _T("%.3i.Path"), nItem + 1 );
		strURL = theApp.GetProfileString( _T("Folders"), strName );
		if ( ! strURL.IsEmpty() )
			m_wndFolders.AddString( strURL );
	}

	nCount = theApp.GetProfileInt( _T("Trackers"), _T("Count"), 0 );
	for ( int nItem = 0 ; nItem < nCount ; nItem++ )
	{
		CString strName, strURL;
		strName.Format( _T("%.3i.URL"), nItem + 1 );
		strURL = theApp.GetProfileString( _T("Trackers"), strName );
		if ( strURL.GetLength() )
		{
			m_wndTracker.AddString( strURL );
			m_wndTracker2.AddString( strURL );
		}
	}
	
	m_sTracker = theApp.GetProfileString( _T("Trackers"), _T("Last") );


	UpdateData( FALSE );
	return TRUE;
}

void CExpertPage::OnReset()
{
}

BOOL CExpertPage::OnSetActive()
{
	m_wndRemove.EnableWindow( m_wndList.GetSelectedCount() > 0 );
	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

LRESULT CExpertPage::OnWizardBack()
{
	return IDD_WELCOME_PAGE;
}

LRESULT CExpertPage::OnWizardNext()
{
	UpdateData();

	if ( m_wndList.GetItemCount() == 0 )
	{
		AfxMessageBox( IDS_PACKAGE_NEED_FILES, MB_ICONEXCLAMATION );
		return -1;
	}

	if ( m_sName.IsEmpty() )
	{
		AfxMessageBox( IDS_OUTPUT_NEED_FILE, MB_ICONEXCLAMATION );
		m_wndName.SetFocus();
		return -1;
	}

	if ( m_sFolder.IsEmpty() )
	{
		AfxMessageBox( IDS_OUTPUT_NEED_FOLDER, MB_ICONEXCLAMATION );
		m_wndFolders.SetFocus();
		return -1;
	}

	// Trackers:

	if ( m_sTracker.Find( _T("http") ) != 0 || m_sTracker.GetLength() < 16 )
	{
		if ( IDYES != AfxMessageBox( IDS_TRACKER_NEED_URL, MB_ICONQUESTION|MB_YESNO ) )
		{
			m_wndTracker.SetFocus();
			return -1;
		}
	}
	
	if ( m_sTracker.GetLength() > 15 &&
		m_wndTracker.FindStringExact( -1, m_sTracker ) < 0 )
	{
		m_wndTracker.AddString( m_sTracker );	// Populate Combo-box
		m_wndTracker2.AddString( m_sTracker );
		
		CString strName;
		int nCount = theApp.GetProfileInt( _T("Trackers"), _T("Count"), 0 );
		strName.Format( _T("%.3i.URL"), ++nCount );
		theApp.WriteProfileInt( _T("Trackers"), _T("Count"), nCount );
		theApp.WriteProfileString( _T("Trackers"), strName, m_sTracker );
	}

	if ( m_sTracker2.Find( _T("http") ) == 0 &&
		m_sTracker2.GetLength() > 15 &&
		m_wndTracker2.FindStringExact( -1,m_sTracker2 ) < 0 )
	{
		m_wndTracker.AddString( m_sTracker2 );
		m_wndTracker2.AddString( m_sTracker2 );
		
		CString strName;
		int nCount = theApp.GetProfileInt( _T("Trackers"), _T("Count"), 0 );
		strName.Format( _T("%.3i.URL"), ++nCount );
		theApp.WriteProfileInt( _T("Trackers"), _T("Count"), nCount );
		theApp.WriteProfileString( _T("Trackers"), strName, m_sTracker );
	}

	theApp.WriteProfileString( _T("Trackers"), _T("Last"), m_sTracker );

	// Output

	if ( GetFileAttributes( m_sFolder ) == 0xFFFFFFFF )
	{
		CString strFormat, strMessage;
		strFormat.LoadString( IDS_OUTPUT_CREATE_FOLDER );
		strMessage.Format( strFormat, (LPCTSTR)m_sFolder );

		if ( IDYES != AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) )
			return -1;

		if ( ! CreateDirectory( m_sFolder, NULL ) )
		{
			strFormat.LoadString( IDS_OUTPUT_CANT_CREATE_FOLDER );
			strMessage.Format( strFormat, (LPCTSTR)m_sFolder );

			AfxMessageBox( IDS_OUTPUT_CANT_CREATE_FOLDER, MB_ICONEXCLAMATION );
			m_wndFolders.SetFocus();
			return -1;
		}
	}

	if ( m_sName.Find( _T(".torrent") ) < 0 &&
		 m_sName.Find( _T(".TORRENT") ) < 0 &&
		 m_sName.Find( _T(".Torrent") ) < 0 )
	{
		UINT nResp = AfxMessageBox( IDS_OUTPUT_EXTENSION, MB_ICONQUESTION|MB_YESNOCANCEL );

		if ( nResp == IDYES )
		{
			m_sName += _T(".torrent");
			UpdateData( FALSE );
		}
		else if ( nResp != IDNO )
		{
			m_wndName.SetFocus();
			return -1;
		}
	}

	CString strPath = m_sFolder + '\\' + m_sName;

	if ( GetFileAttributes( strPath ) != INVALID_FILE_ATTRIBUTES )
	{
		CString strMessage;
		strMessage.LoadString( IDS_OUTPUT_REPLACE_FILE );
		strMessage.Format( strMessage, (LPCTSTR)strPath );

		if ( IDYES != AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) )
			return -1;

		DeleteFile( strPath );
	}

	if ( m_wndFolders.FindStringExact( -1, m_sFolder ) < 0 )
	{
		m_wndFolders.AddString( m_sFolder );

		CString strName;
		int nCount = theApp.GetProfileInt( _T("Folders"), _T("Count"), 0 );
		strName.Format( _T("%.3i.Path"), ++nCount );
		theApp.WriteProfileInt( _T("Folders"), _T("Count"), nCount );
		theApp.WriteProfileString( _T("Folders"), strName, m_sFolder );
	}

	GET_PAGE( COutputPage, pOutput );
	pOutput->m_sName = m_sName;
	pOutput->m_sFolder = m_sFolder;
	GET_PAGE( CTrackerPage, pTracker );
	pTracker->m_sTracker = m_sTracker;
	pTracker->m_sTracker2 = m_sTracker2;
	GET_PAGE( CCommentPage, pComment );
	pComment->m_sComment = m_sComment;

	return IDD_FINISHED_PAGE;
}

void CExpertPage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

BOOL CExpertPage::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_DELETE && GetFocus() == &m_wndList )
		{
			OnRemoveFile();
			return TRUE;
		}
	}

	return CWnd::PreTranslateMessage( pMsg );
}

void CExpertPage::OnBrowseFolder()
{
	TCHAR szPath[MAX_PATH];
	LPITEMIDLIST pPath;
	LPMALLOC pMalloc;
	BROWSEINFO pBI;

	ZeroMemory( &pBI, sizeof(pBI) );
	pBI.hwndOwner		= GetSafeHwnd();
	pBI.pszDisplayName	= szPath;
	pBI.lpszTitle		= _T("Select output folder:");
	pBI.ulFlags			= BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	pPath = SHBrowseForFolder( &pBI );
	if ( pPath == NULL ) return;

	SHGetPathFromIDList( pPath, szPath );
	SHGetMalloc( &pMalloc );
	pMalloc->Free( pPath );
	pMalloc->Release();

	UpdateData( TRUE );
	m_sFolder = szPath;
	UpdateData( FALSE );
}

void CExpertPage::OnItemChangedFileList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	m_wndRemove.EnableWindow( m_wndList.GetSelectedCount() > 0 );
}

void CExpertPage::OnDropFiles( HDROP hDropInfo )
{
	UpdateData();

	CString sFilename;

	const int nFiles = DragQueryFile( hDropInfo, (UINT)-1, NULL, 0 );
	for ( int i = 0 ; i < nFiles ; i++ )
	{
		LPWSTR pszFile = sFilename.GetBuffer( _MAX_PATH );
		DragQueryFile( hDropInfo, i, pszFile, _MAX_PATH );

		if ( PathIsDirectory( sFilename ) )
			AddFolder( (LPCTSTR)pszFile, 2 );
		else
			AddFile( (LPCTSTR)pszFile );
	}

	DragFinish( hDropInfo );
}

void CExpertPage::OnAddFolder()
{
	UpdateData();

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

void CExpertPage::OnAddFile()
{
	UpdateData();

	CFileDialog dlg( TRUE, NULL, NULL,
		OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING,
		_T("All Files|*.*||"), this );

	const DWORD nFilesSize( 81920 );
	LPTSTR szFiles = new TCHAR [ nFilesSize ];
	ZeroMemory( szFiles, nFilesSize * sizeof( TCHAR ) );
	dlg.m_ofn.lpstrFile	= szFiles;
	dlg.m_ofn.nMaxFile	= nFilesSize;

	if ( dlg.DoModal() == IDOK )
	{
		CWaitCursor wc;
		CString strFolder	= szFiles;
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

	delete [] szFiles;
}

void CExpertPage::OnRemoveFile()
{
	CWaitCursor wc;

	UpdateData();

	for ( int nItem = m_wndList.GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		if ( m_wndList.GetItemState( nItem, LVIS_SELECTED ) )
		{
			CString strSize = m_wndList.GetItemText( nItem, 2 );

			m_wndList.DeleteItem( nItem );

			if ( m_wndList.GetItemCount() )
			{
				m_nTotalSize -= _wtoi( strSize );
				m_sFileCount.Format( _T("%i Files, %s"), m_wndList.GetItemCount(), SmartSize( m_nTotalSize ) );
			}
			else
			{
				m_sFileCount = _T("No Files.");
				m_nTotalSize = 0;
			}

			UpdateData( FALSE );
			UpdateWindow();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//	Add Files (and Folders)

void CExpertPage::AddFile(LPCTSTR pszFile)
{
	HANDLE hFile = CreateFile( pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		CString strMessage;
		strMessage.LoadString( IDS_PACKAGE_CANT_OPEN );
		strMessage.Format( strMessage, pszFile );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		return;
	}

	// Duplicate Check
	LVFINDINFO lvInfo;
	lvInfo.flags = LVFI_STRING;
	lvInfo.psz = pszFile;
	if ( m_wndList.FindItem( &lvInfo, -1 ) != -1 )
	{
		//CString strMessage;
		//strMessage.Format( _T("Duplicate filename denied:  %s"), pszFile );
		//AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		CloseHandle( hFile );
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

	const int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE, m_wndList.GetItemCount(),
		pszFile, 0, 0, pInfo.iIcon, NULL );

	CString sBytes;
	sBytes.Format( _T("%i"), nSize );

	m_wndList.SetItemText( nItem, 1, SmartSize( nSize ) );
	m_wndList.SetItemText( nItem, 2, sBytes );

	m_nTotalSize += nSize;
	m_sFileCount.Format( _T("%i Files,  %s"), m_wndList.GetItemCount(), SmartSize( m_nTotalSize ) );

	UpdateData( FALSE );
	UpdateWindow();
}

void CExpertPage::AddFolder(LPCTSTR pszPath, int nRecursive)
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
					if ( nResp == IDYES )
						nRecursive = 2;
					else if ( nResp == IDNO )
						nRecursive = 1;
					else
						break;
				}

				if ( nRecursive == 2 )
					AddFolder( strPath, 2 );
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
