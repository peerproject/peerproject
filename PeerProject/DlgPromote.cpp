//
// DlgPromote.cpp
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
#include "DlgPromote.h"
#include "Colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPromoteDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CPromoteDlg)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPromoteDlg dialog

CPromoteDlg::CPromoteDlg(CWnd* pParent) : CSkinDialog(CPromoteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPromoteDlg)
	//}}AFX_DATA_INIT
}

void CPromoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(CPromoteDlg)
	DDX_Control(pDX, IDC_WEB, m_wndWeb);
	DDX_Control(pDX, IDC_TITLE, m_wndTitle);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPromoteDlg message handlers

BOOL CPromoteDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CPromoteDlg"), IDR_MAINFRAME );

	return TRUE;
}

HBRUSH CPromoteDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = (HBRUSH)CSkinDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( pWnd == &m_wndTitle )
	{
		pDC->SelectObject( &theApp.m_gdiFontBold );
	}
	else if ( pWnd == &m_wndWeb )
	{
		pDC->SetTextColor(  Colors.m_crTextLink );
		pDC->SelectObject( &theApp.m_gdiFontLine );
	}

	return hbr;
}

BOOL CPromoteDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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

void CPromoteDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CSkinDialog::OnLButtonUp( nFlags, point );

	CRect rc;
	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );

	if ( rc.PtInRect( point ) )
	{
		const CString strWebSite( WEB_SITE );

		ShellExecute( GetSafeHwnd(), _T("open"),
			strWebSite + _T("?Version=") + theApp.m_sVersion,
			NULL, NULL, SW_SHOWNORMAL );
	}
}
