//
// DlgWarnings.cpp
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
#include "DlgWarnings.h"
#include "Colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWarningsDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CWarningsDlg)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWarningsDlg dialog

CWarningsDlg::CWarningsDlg(CWnd* pParent) : CSkinDialog(CWarningsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWarningsDlg)
	//}}AFX_DATA_INIT
}

void CWarningsDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarningsDlg)
	DDX_Control(pDX, IDC_RESPECT, m_wndRespect);
	DDX_Control(pDX, IDC_WEB, m_wndWeb);
	DDX_Control(pDX, IDC_TITLE, m_wndTitle);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWarningsDlg message handlers

BOOL CWarningsDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CWarningsDlg"), IDR_MAINFRAME );

	return TRUE;
}

HBRUSH CWarningsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = (HBRUSH)CSkinDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( pWnd == &m_wndTitle || pWnd == &m_wndRespect )
	{
		pDC->SelectObject( &theApp.m_gdiFontBold );
	}
	else if ( pWnd == &m_wndWeb )
	{
		pDC->SetTextColor( Colors.m_crTextLink );
		pDC->SelectObject( &theApp.m_gdiFontLine );
	}

	return hbr;
}

BOOL CWarningsDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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

void CWarningsDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CSkinDialog::OnLButtonUp( nFlags, point );

	CRect rc;
	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );

	if ( rc.PtInRect( point ) )
	{
		const CString strWebSite(WEB_SITE_T);

		ShellExecute( GetSafeHwnd(), _T("open"),
			strWebSite + _T("?Version=") + theApp.m_sVersion,
			NULL, NULL, SW_SHOWNORMAL );
	}
}


