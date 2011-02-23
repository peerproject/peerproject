//
// DlgAbout.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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
#include "PeerProject.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "DlgAbout.h"
//#include "Revision.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

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
	m_wndTitle.SetWindowText( theApp.m_sVersionLong );		// PeerProject 1.x.x.x 32/64-bit (date rXXXX) Debug

	DWORD dwSize = GetFileVersionInfoSize( theApp.m_strBinaryPath, &dwSize );
	BYTE* pBuffer = new BYTE[ dwSize ];
	GetFileVersionInfo( theApp.m_strBinaryPath, NULL, dwSize, pBuffer );

	CWnd* pWnd = GetDlgItem( IDC_COPYRIGHT );
	BYTE* pValue = NULL;

	if ( VerQueryValue( pBuffer, L"\\StringFileInfo\\000004b0\\LegalCopyright", (void**)&pValue, (UINT*)&dwSize ) )
		pWnd->SetWindowText( (LPCTSTR)pValue );				// Substitute manifest info

	delete [] pBuffer;

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
		pDC->SetTextColor( Colors.m_crTextLink );
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
		const CString strWebSite( WEB_SITE );

		ShellExecute( GetSafeHwnd(), _T("open"),
			strWebSite + _T("?Version=") + theApp.m_sVersion, NULL, NULL, SW_SHOWNORMAL );
	}
}

void CAboutDlg::OnRButtonDown(UINT /*nFlags*/, CPoint point)
{
	// Shift+Rightclick on link for BugTrap crash testing

//#ifdef _DEBUG
	if ( ! ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) )
		return;

	CRect rc;
	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );
	if ( ! rc.PtInRect( point ) )
		return;

#ifndef _DEBUG
	if ( AfxMessageBox( _T("\nDo you wish to trigger a program crash?"), MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 ) != IDYES )
		return;
#endif

	DWORD* pNullPtr = (DWORD*)NULL;
	*pNullPtr = 0xFFFFFFFF;				// Force program crash
//#endif
}
