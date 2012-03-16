//
// DlgBitziDownload.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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
#include "DlgBitziDownload.h"
#include "BitziDownloader.h"
#include "Colors.h"
#include "ShellIcons.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

BEGIN_MESSAGE_MAP(CBitziDownloadDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CBitziDownloadDlg)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBitziDownloadDlg dialog

CBitziDownloadDlg::CBitziDownloadDlg(CWnd* pParent) : CSkinDialog(CBitziDownloadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBitziDownloadDlg)
	//}}AFX_DATA_INIT
}

void CBitziDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBitziDownloadDlg)
	DDX_Control(pDX, IDC_WEB, m_wndWeb);
	DDX_Control(pDX, IDC_FILES, m_wndFiles);
	DDX_Control(pDX, IDCANCEL, m_wndCancel);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CBitziDownloadDlg message handlers

BOOL CBitziDownloadDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CBitziDownloadDlg"), IDR_MAINFRAME );

	if ( Settings.General.LanguageRTL )
		m_wndProgress.ModifyStyleEx( WS_EX_LAYOUTRTL, 0, 0 );
	m_wndProgress.SetRange( 0, short( m_pDownloader.GetFileCount() * 2 ) );
	m_wndFiles.InsertColumn( 0, _T("Filename"), LVCFMT_LEFT, 190, -1 );
	m_wndFiles.InsertColumn( 1, _T("Status"), LVCFMT_LEFT, 100, 0 );
	ShellIcons.AttachTo( &m_wndFiles, 16 );		// .SetImageList()

	SetTimer( 1, 1000, NULL );
	m_nFailures = 0;

	m_pDownloader.Start( this );

	return TRUE;
}

void CBitziDownloadDlg::AddFile(DWORD nIndex)
{
	m_pDownloader.AddFile( nIndex );
}

void CBitziDownloadDlg::OnNextFile(DWORD /*nIndex*/)
{
	m_wndProgress.OffsetPos( 1 );
}

void CBitziDownloadDlg::OnRequesting(DWORD nIndex, LPCTSTR pszName)
{
	CString strMessage;

	int nImage	= ShellIcons.Get( pszName, 16 );
	int nItem	= m_wndFiles.InsertItem( LVIF_TEXT|LVIF_IMAGE,
					m_wndFiles.GetItemCount(), pszName, 0, 0, nImage, nIndex );

	m_wndFiles.EnsureVisible( nItem, FALSE );
	LoadString( strMessage, IDS_BITZI_REQUESTING );
	m_wndFiles.SetItemText( nItem, 1, strMessage );
}

void CBitziDownloadDlg::OnSuccess(DWORD /*nIndex*/)
{
	CString strMessage;

	LoadString( strMessage, IDS_BITZI_SUCCESS );
	m_wndFiles.SetItemText( m_wndFiles.GetItemCount() - 1, 1, strMessage );
}

void CBitziDownloadDlg::OnFailure(DWORD /*nIndex*/, LPCTSTR pszMessage)
{
	m_wndFiles.SetItemText( m_wndFiles.GetItemCount() - 1, 1, pszMessage );
	m_nFailures++;
}

void CBitziDownloadDlg::OnFinishedFile(DWORD /*nIndex*/)
{
	m_wndProgress.OffsetPos( 1 );
}

void CBitziDownloadDlg::OnTimer(UINT_PTR /*nIDEvent*/)
{
	CString strMessage;

	if ( ! m_pDownloader.IsWorking() )
	{
		KillTimer( 1 );

		LoadString( strMessage, IDS_BITZI_FINISHED );
		int nItem = m_wndFiles.InsertItem( LVIF_TEXT|LVIF_IMAGE,
			m_wndFiles.GetItemCount(), strMessage, 0, 0, -1, 0 );
		m_wndFiles.EnsureVisible( nItem, FALSE );
		LoadString( strMessage, IDS_BITZI_CLOSE );
		m_wndCancel.SetWindowText( strMessage );

		if ( ! m_nFailures )
			PostMessage( WM_COMMAND, IDCANCEL );
	}
}

void CBitziDownloadDlg::OnCancel()
{
	m_pDownloader.Stop();

	CSkinDialog::OnCancel();
}

HBRUSH CBitziDownloadDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( pWnd == &m_wndWeb )
	{
		pDC->SelectObject( &theApp.m_gdiFontLine );
		pDC->SetTextColor( Colors.m_crTextLink );
	}

	return hbr;
}

BOOL CBitziDownloadDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint point;
	CRect rc;

	GetCursorPos( &point );
	m_wndWeb.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}

	return CSkinDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CBitziDownloadDlg::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	CRect rc;

	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );

	if ( rc.PtInRect( point ) )
	{
		ShellExecute( GetSafeHwnd(), _T("open"),
			_T("http://www.bitzi.com/?ref=PeerProject"),
			NULL, NULL, SW_SHOWNORMAL );
	}
}
