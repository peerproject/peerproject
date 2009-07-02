//
// DlgAbout.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2009
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
#include "CoolInterface.h"
#include "DlgAbout.h"
#include "Revision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAboutDlg, CSkinDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

CAboutDlg::CAboutDlg(CWnd* pParent) : CSkinDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_WEB, m_wndWeb);
	DDX_Control(pDX, IDC_TITLE, m_wndTitle);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CAboutDlg"), IDR_MAINFRAME );

	CString strCaption;

	GetWindowText( strCaption );
	strCaption += _T(" v");
	strCaption += theApp.m_sVersion;
	SetWindowText( strCaption );

	m_wndTitle.GetWindowText( strCaption );
	strCaption += theApp.m_sVersion;
#ifdef _WIN64
	strCaption += _T("  64-bit  (") + theApp.m_sBuildDate;
#else
	strCaption += _T("  32-bit  (") + theApp.m_sBuildDate;
#endif
#ifdef __REVISION__
	strCaption += _T(" r") _T(__REVISION__) _T(")");
#else
	strCaption += _T(")");
#endif
#ifdef __MODAUTHOR__
	strCaption += _T(" ") _T(__MODAUTHOR__);	// YOUR NAME (Edit in Revision.h)
#endif
#ifdef LAN_MODE
	strCaption += _T(" LAN Mod");
#endif
	m_wndTitle.SetWindowText( strCaption );

	DWORD dwSize = GetFileVersionInfoSize( theApp.m_strBinaryPath, &dwSize );
	BYTE* pBuffer = new BYTE[ dwSize ];
	GetFileVersionInfo( theApp.m_strBinaryPath, NULL, dwSize, pBuffer );

	BYTE* pValue = NULL;
	CString strCopyRight;

	if ( VerQueryValue( pBuffer, L"\\StringFileInfo\\000004b0\\LegalCopyright",
		 (void**)&pValue, (UINT*)&dwSize ) )
		strCopyRight = (LPCTSTR)pValue;

	delete [] pBuffer;

	CWnd* pWnd = GetDlgItem( IDC_COPYRIGHT );
	pWnd->SetWindowText( (LPCTSTR)strCopyRight );

	return TRUE;
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = (HBRUSH)CSkinDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( pWnd == &m_wndTitle )
	{
		pDC->SelectObject( &CoolInterface.m_fntBold );
	}
	else if ( pWnd == &m_wndWeb )
	{
		pDC->SetTextColor( CoolInterface.m_crTextLink );
		pDC->SelectObject( &CoolInterface.m_fntUnder );
	}

	return hbr;
}

BOOL CAboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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

	return CSkinDialog::OnSetCursor( pWnd, nHitTest, message );
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
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

void CAboutDlg::OnRButtonDown(UINT /*nFlags*/, CPoint point)
{
	CRect rc;

	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );

	if ( rc.PtInRect( point ) && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) )
	{
		DWORD* pNullPtr = (DWORD*)NULL;
		*pNullPtr = 0xFFFFFFFF;
	}
}
