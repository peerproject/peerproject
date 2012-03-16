//
// WizardSheet.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008-2012
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
#include "WizardSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PAGE_COLOR		RGB( 255, 255, 255 )	// White areas
//#define BANNER_SIZE	50	// m_nBannerHeight
#define MENU_SIZE		48


/////////////////////////////////////////////////////////////////////////////
// CWizardSheet

IMPLEMENT_DYNAMIC(CWizardSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CWizardSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CWizardSheet)
	ON_WM_PAINT()
	ON_WM_SIZE()
//	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_NCLBUTTONUP()
	ON_WM_XBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardSheet construction

CWizardSheet::CWizardSheet(CWnd *pParentWnd, UINT iSelectPage)
{
	BITMAP bm;
	m_bmHeader.LoadBitmap( IDB_BANNER );
	m_bmHeader.GetBitmap( &bm );
	m_nBannerHeight = bm.bmHeight;

	m_psh.dwFlags &= ~PSP_HASHELP;
	Construct( _T(""), pParentWnd, iSelectPage );
	SetWizardMode();
}

//CWizardSheet::~CWizardSheet()
//{
//}

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet page lookup

CWizardPage* CWizardSheet::GetPage(CRuntimeClass* pClass)
{
	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CWizardPage* pPage = (CWizardPage*)CPropertySheet::GetPage( nPage );
		ASSERT_KINDOF(CWizardPage, pPage);
		if ( pPage->IsKindOf( pClass ) ) return pPage;
	}

	return NULL;
}

void CWizardSheet::DoReset()
{
	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CWizardPage* pPage = (CWizardPage*)CPropertySheet::GetPage( nPage );
		ASSERT_KINDOF(CWizardPage, pPage);
		if ( pPage->m_hWnd != NULL ) pPage->OnReset();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet message handlers

BOOL CWizardSheet::OnInitDialog()
{
	CPropertySheet::OnInitDialog();

	SetIcon( theApp.LoadIcon( IDR_MAINFRAME ), TRUE );
	SetFont( &theApp.m_fntNormal );

	// ToDo: Fix Minimize Button Properly
	ModifyStyle( 0, WS_MINIMIZEBOX );

	CRect rc;
	GetWindowRect( &rc );
	ScreenToClient( &rc );
	const UINT nWidth = rc.Width();

	// Set Back
	GetDlgItem( 0x3023 )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.OffsetRect( 16 - rc.left, -1 );
	GetDlgItem( 0x3023 )->MoveWindow( &rc );

	// Set Next
	GetDlgItem( 0x3024 )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.OffsetRect( 26 + rc.Width() - rc.left, -1 );
	GetDlgItem( 0x3024 )->MoveWindow( &rc );
	GetDlgItem( 0x3025 )->MoveWindow( &rc );	// Ready button

	// Set Cancel
	GetDlgItem( 2 )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.OffsetRect( nWidth - 20 - rc.Width() - rc.left, -1 );
	GetDlgItem( 2 )->MoveWindow( &rc );
	GetDlgItem( 2 )->SetWindowText( _T("E&xit") );

	// Set Help
	//GetDlgItem( 0x0009 )->GetWindowRect( &rc );
	//ScreenToClient( &rc );
	//rc.OffsetRect( 414 - rc.left - 84, -1 );
	//GetDlgItem( 0x0009 )->MoveWindow( &rc );

	if ( GetDlgItem( 0x0009 ) ) GetDlgItem( 0x0009 )->ShowWindow( SW_HIDE );
	if ( GetDlgItem( 0x3026 ) ) GetDlgItem( 0x3026 )->ShowWindow( SW_HIDE );

	return TRUE;
}

BOOL CWizardSheet::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	CWnd* pWnd = GetActivePage();

	if ( pWnd != NULL )
	{
		if ( GetWindowLongPtr( pWnd->GetSafeHwnd(), GWLP_USERDATA ) == TRUE )
		{
			pWnd = NULL;
		}
		else
		{
			SetWindowLongPtr( pWnd->GetSafeHwnd(), GWLP_USERDATA, TRUE );
			pWnd->SetFont( &theApp.m_fntNormal, FALSE );
			pWnd = pWnd->GetWindow( GW_CHILD );
		}
	}

	while ( pWnd != NULL )
	{
		TCHAR szName[32];

		GetClassName( pWnd->GetSafeHwnd(), szName, 32 );

		if ( !_tcscmp( szName, _T("Static") ) )
			pWnd->SetFont( &theApp.m_fntNormal, FALSE );
		else if ( _tcscmp( szName, _T("RICHEDIT") ) )
			pWnd->SetFont( &theApp.m_fntNormal, TRUE );

		pWnd = pWnd->GetNextWindow();
	}

	return CPropertySheet::OnChildNotify( message, wParam, lParam, pLResult );
}

void CWizardSheet::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize( nType, cx, cy );

	if ( CWnd* pWnd = GetWindow( GW_CHILD ) )
	{
		GetClientRect( &m_rcPage );

		m_rcPage.top += m_nBannerHeight + 1;
		m_rcPage.bottom -= MENU_SIZE;

		pWnd->SetWindowPos( NULL, m_rcPage.left, m_rcPage.top, m_rcPage.Width(), m_rcPage.Height(), SWP_NOSIZE );
	}
}

void CWizardSheet::OnPaint()
{
	CPaintDC dc( this );

	CRect rc;
	GetClientRect( &rc );

	// Banner
	CRect rcHeader = rc;
	rcHeader.bottom = rcHeader.top + m_nBannerHeight;

	CDC mdc;
	mdc.CreateCompatibleDC( &dc );
	CBitmap* pOldBitmap = (CBitmap*)mdc.SelectObject( &m_bmHeader );
	dc.BitBlt( rcHeader.left, rcHeader.top, rcHeader.Width(), m_nBannerHeight, &mdc, 0, 0, SRCCOPY );
	mdc.SelectObject( pOldBitmap );
	mdc.DeleteDC();

	// Bevels
	dc.Draw3dRect( 0, m_nBannerHeight, rc.Width() + 1, 1,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
	dc.Draw3dRect( 0, rc.bottom - MENU_SIZE, rc.Width() + 1, 2,
		RGB( 128, 128, 128 ), RGB( 255, 255, 255 ) );

	rc.top = rc.bottom - MENU_SIZE + 2;

	// Version Text
	CFont* pOldFont = (CFont*)dc.SelectObject( &theApp.m_fntTiny );
	CString str = _T("v") + theApp.m_sVersion;
	CSize sz = dc.GetTextExtent( str );

	dc.SetBkMode( OPAQUE );
	dc.SetBkColor( GetSysColor( COLOR_BTNFACE ) );
	dc.SetTextColor( GetSysColor( COLOR_BTNSHADOW ) );
	dc.ExtTextOut( rc.right - sz.cx - 2, rc.bottom - sz.cy - 1, ETO_CLIPPED|ETO_OPAQUE, &rc, str, NULL );
	dc.SelectObject( pOldFont );
}

//BOOL CWizardSheet::OnEraseBkgnd(CDC* /*pDC*/)
//{
//	return TRUE;
//}

BOOL CWizardSheet::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint pt;
	CRect rc;

	GetCursorPos( &pt );
	GetClientRect( &rc );
	ScreenToClient( &pt );

	if ( rc.PtInRect( pt ) && pt.y < m_nBannerHeight )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}

	return CPropertySheet::OnSetCursor( pWnd, nHitTest, message );
}

void CWizardSheet::OnLButtonUp(UINT /*nFlags*/, CPoint point)
{
	if ( point.y < m_nBannerHeight )
		ShellExecute( NULL, NULL, _T("http://peerproject.org/TorrentWizard/"), NULL, NULL, SW_SHOWNORMAL );
}

void CWizardSheet::OnNcLButtonUp(UINT nHitTest, CPoint /*point*/)
{
	// Broken Minimize Button Workaround.  ToDo: Fix properly & Remove this
	if ( nHitTest == HTCLOSE )
		PostMessage( WM_SYSCOMMAND, SC_CLOSE );
	else if ( nHitTest == HTMINBUTTON )
		ShowWindow( SW_MINIMIZE );
}

void CWizardSheet::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		this->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		this->PressButton( PSBTN_NEXT );
}


/////////////////////////////////////////////////////////////////////////////
// CWizardPage

IMPLEMENT_DYNCREATE(CWizardPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CWizardPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPage)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_PRESSBUTTON, OnPressButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardPage construction

CWizardPage::CWizardPage(UINT nID) : CPropertyPage( nID )
{
	m_brPageColor.CreateSolidBrush( PAGE_COLOR );
}

//CWizardPage::~CWizardPage()
//{
//}

/////////////////////////////////////////////////////////////////////////////
// CWizardPage message handlers

HBRUSH CWizardPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT /*nCtlColor*/)
{
	if ( pWnd != NULL && pWnd->GetDlgCtrlID() == IDC_TITLE )
		pDC->SelectObject( &theApp.m_fntBold );

	pDC->SetBkColor( PAGE_COLOR );
	return (HBRUSH)m_brPageColor.GetSafeHandle();
}

void CWizardPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	CWizardSheet* pSheet = (CWizardSheet*)GetParent();

	if ( cx != pSheet->m_rcPage.Width() )
		MoveWindow( &pSheet->m_rcPage );
}

/////////////////////////////////////////////////////////////////////////////
// CWizardPage operations

void CWizardPage::Next()
{
	PostMessage( WM_PRESSBUTTON, PSBTN_NEXT );
}

LRESULT CWizardPage::OnPressButton(WPARAM wParam, LPARAM /*lParam*/)
{
	UpdateWindow();

	Sleep( 250 );

	GetSheet()->PressButton( (int)wParam );

	return 0;
}

CWizardSheet* CWizardPage::GetSheet()
{
	return (CWizardSheet*)GetParent();
}

CWizardPage* CWizardPage::GetPage(CRuntimeClass* pClass)
{
	return GetSheet()->GetPage( pClass );
}

void CWizardPage::SetWizardButtons(DWORD dwFlags)
{
	GetSheet()->SetWizardButtons( dwFlags );
}

void CWizardPage::StaticReplace(LPCTSTR pszSearch, LPCTSTR pszReplace)
{
	for ( CWnd* pChild = GetWindow( GW_CHILD ) ; pChild ; pChild = pChild->GetNextWindow() )
	{
		TCHAR szName[32];
		GetClassName( pChild->GetSafeHwnd(), szName, 32 );

		if ( _tcscmp( szName, _T("Static") ) != 0 )
			continue;

		CString strText;
		pChild->GetWindowText( strText );

		for (;;)
		{
			int nPos = strText.Find( pszSearch );
			if ( nPos < 0 ) break;
			strText	= strText.Left( nPos ) + CString( pszReplace )
					+ strText.Mid( nPos + static_cast< int >( _tcslen( pszSearch ) ) );
		}

		pChild->SetWindowText( strText );
	}
}
