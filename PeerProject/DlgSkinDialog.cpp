//
// DlgSkinDialog.cpp
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
#include "Settings.h"
#include "PeerProject.h"
#include "DlgSkinDialog.h"
#include "CoolInterface.h"
#include "Colors.h"
//#include "Images.h"
#include "Skin.h"
#include "SkinWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CSkinDialog, CDialog)

BEGIN_MESSAGE_MAP(CSkinDialog, CDialog)
	//{{AFX_MSG_MAP(CSkinDialog)
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_NCACTIVATE()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCMOUSEMOVE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_HELPINFO()
//	ON_WM_UPDATEUISTATE()	// Alt-keypress
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSkinDialog dialog

CSkinDialog::CSkinDialog(UINT nResID, CWnd* pParent, BOOL bAutoBanner)
	: CDialog		( nResID, pParent )
	, m_pSkin		( NULL )
	, m_bAutoBanner	( bAutoBanner )
{
}

void CSkinDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

//	if ( m_oBanner.m_hWnd ) DDX_Control(pDX, IDC_BANNER, m_oBanner);
}

/////////////////////////////////////////////////////////////////////////////
// CSkinDialog operations

void CSkinDialog::EnableBanner(BOOL bEnable)
{
	if ( ! bEnable && m_oBanner.m_hWnd )
	{
		// Remove banner
		m_oBanner.DestroyWindow();

		// Move all controls up
		for ( CWnd* pChild = GetWindow( GW_CHILD ) ; pChild ;
			pChild = pChild->GetNextWindow() )
		{
			CRect rc;
			pChild->GetWindowRect( &rc );
			ScreenToClient( &rc );
			rc.MoveToY( rc.top - Skin.m_nBanner );
			pChild->MoveWindow( &rc );
		}

		// Resize window
		CRect rcWindow;
		GetWindowRect( &rcWindow );
		SetWindowPos( NULL, 0, 0, rcWindow.Width(), rcWindow.Height() - Skin.m_nBanner,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );
	}
	else if ( bEnable && ! m_oBanner.m_hWnd && Skin.m_bmBanner.m_hObject )
	{
		// Resize window
		CRect rcWindow;
		GetWindowRect( &rcWindow );
		SetWindowPos( NULL, 0, 0, rcWindow.Width(), rcWindow.Height() + Skin.m_nBanner,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );

		// Move all controls down
		for ( CWnd* pChild = GetWindow( GW_CHILD ) ; pChild ;
			pChild = pChild->GetNextWindow() )
		{
			CRect rc;
			pChild->GetWindowRect( &rc );
			ScreenToClient( &rc );
			rc.MoveToY( rc.top + Skin.m_nBanner );
			pChild->MoveWindow( &rc );
		}

		// Add banner
		CRect rcBanner;
		GetClientRect( &rcBanner );
		if ( Settings.General.LanguageRTL )
			rcBanner.left -= Skin.m_bmBanner.GetBitmapDimension().cx - rcBanner.Width();
		rcBanner.right = rcBanner.left + Skin.m_bmBanner.GetBitmapDimension().cx;
		rcBanner.bottom = rcBanner.top + Skin.m_nBanner;
		VERIFY( m_oBanner.Create( NULL, WS_CHILD | WS_VISIBLE |
			SS_BITMAP | SS_REALSIZEIMAGE, rcBanner, this, IDC_BANNER ) );
		m_oBanner.SetBitmap( (HBITMAP)Skin.m_bmBanner.m_hObject );
	}
}

BOOL CSkinDialog::SkinMe(LPCTSTR pszSkin, UINT nIcon, BOOL bLanguage)
{
	if ( m_bAutoBanner )
		EnableBanner( TRUE );

	BOOL bSuccess ( FALSE );
	CRect rc;
	GetClientRect( &rc );

	CString strSkin = ( pszSkin ? pszSkin : (LPCTSTR)GetRuntimeClass()->m_lpszClassName );

	m_pSkin = ::Skin.GetWindowSkin( strSkin );
	if ( NULL == m_pSkin ) m_pSkin = ::Skin.GetWindowSkin( this );

	if ( bLanguage )
		bSuccess = ::Skin.Apply( strSkin, this, nIcon );

	if ( nIcon )
		CoolInterface.SetIcon( nIcon, m_pSkin && Settings.General.LanguageRTL, FALSE, this );

	CoolInterface.EnableTheme( this, ( m_pSkin == NULL ) );

	if ( m_pSkin != NULL )
	{
		if ( GetStyle() & WS_CAPTION )
			ModifyStyle( WS_CAPTION, 0 );

		m_pSkin->CalcWindowRect( &rc );

		SetWindowRgn( NULL, FALSE );
		SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(),
			SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED );

		m_pSkin->OnSize( this );
	}
	else
	{
		if ( ( GetStyle() & WS_CAPTION ) == 0 )
			ModifyStyle( 0, WS_CAPTION );

		CalcWindowRect( &rc );
		SetWindowRgn( NULL, FALSE );
		SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(),
			SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED );
	}

	return bSuccess || m_pSkin != NULL;
}

BOOL CSkinDialog::SelectCaption(CWnd* pWnd, int nIndex)
{
	return ::Skin.SelectCaption( pWnd, nIndex );
}

/////////////////////////////////////////////////////////////////////////////
// CSkinDialog message handlers

void CSkinDialog::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	if ( m_pSkin && ! theApp.m_bClosing )
		m_pSkin->OnNcCalcSize( this, bCalcValidRects, lpncsp );
	else
		CDialog::OnNcCalcSize( bCalcValidRects, lpncsp );
}

LRESULT CSkinDialog::OnNcHitTest(CPoint point)
{
	if ( m_pSkin && ! theApp.m_bClosing )
		return m_pSkin->OnNcHitTest( this, point, ( GetStyle() & WS_THICKFRAME ) ? TRUE : FALSE );

	return CDialog::OnNcHitTest( point );
}

BOOL CSkinDialog::OnNcActivate(BOOL bActive)
{
	if ( m_pSkin && ! theApp.m_bClosing )
	{
		m_pSkin->OnNcActivate( this, IsWindowEnabled() && ( bActive || ( m_nFlags & WF_STAYACTIVE ) ) );
		return TRUE;
	}

	return CDialog::OnNcActivate( bActive );
}

void CSkinDialog::OnNcPaint()
{
	if ( m_pSkin && ! theApp.m_bClosing )
		m_pSkin->OnNcPaint( this );
	else
		CDialog::OnNcPaint();
}

void CSkinDialog::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if ( m_pSkin && m_pSkin->OnNcLButtonDown( this, nHitTest, point ) ) return;
	CDialog::OnNcLButtonDown(nHitTest, point);
}

void CSkinDialog::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if ( m_pSkin && m_pSkin->OnNcLButtonUp( this, nHitTest, point ) ) return;
	CDialog::OnNcLButtonUp( nHitTest, point );
}

void CSkinDialog::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if ( m_pSkin && m_pSkin->OnNcLButtonDblClk( this, nHitTest, point ) ) return;
	CDialog::OnNcLButtonDblClk( nHitTest, point );
}

void CSkinDialog::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if ( m_pSkin ) m_pSkin->OnNcMouseMove( this, nHitTest, point );
	CDialog::OnNcMouseMove( nHitTest, point );
}

void CSkinDialog::OnSize(UINT nType, int cx, int cy)
{
	if ( m_pSkin ) m_pSkin->OnSize( this );
	CDialog::OnSize( nType, cx, cy );
}

LRESULT CSkinDialog::OnSetText(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	LRESULT lResult = Default();

	if ( m_pSkin )
	{
		if ( IsWindowVisible() )
		{
			ModifyStyle( WS_VISIBLE, 0 );
			lResult = Default();
			ModifyStyle( 0, WS_VISIBLE );
		}
		m_pSkin->OnSetText( this );
	}

	return lResult;
}

BOOL CSkinDialog::OnEraseBkgnd(CDC* pDC)
{
	if ( m_pSkin && ! theApp.m_bClosing && m_pSkin->OnEraseBkgnd( this, pDC ) ) return TRUE;

	CRect rc;
	GetClientRect( &rc );
	rc.top = Skin.m_nBanner;

	if ( Skin.m_bmDialog.m_hObject )
		CoolInterface.DrawWatermark( pDC, &rc, &Skin.m_bmDialog );
	else
		pDC->FillSolidRect( &rc, Colors.m_crDialog );

	return TRUE;
}

HBRUSH CSkinDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	// Skinned dialog controls
	if ( Skin.m_bmDialog.m_hObject && ( nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN ) )
	{
	// Obsolete Handling:
	//	if ( pWnd->GetDlgCtrlID() != IDC_STATIC )					// Named controls  (Dynamic handling)
	//	{
	//		//if ( ! pWnd->IsWindowEnabled() || ( pWnd->GetStyle() & ES_READONLY ) )	// (Note SS_REALSIZEIMAGE conflict)
	//		//	return Skin.m_brDialog;								// Skip disabled edit boxes
	//
	//		// Handle exceptions (Icons)
	//		const int nCtrlID = pWnd->GetDlgCtrlID();
	//		if ( nCtrlID == IDC_MONITOR_ICON || nCtrlID == IDC_INFO_ICON )	// || nCtrlID == IDC_BANDWIDTH_SLIDER )
	//			return Skin.m_brDialog;								// Dynmic controls (UploadQueue slider, variable icon, etc.)
	//
	//		//TCHAR szName[24];
	//		//GetClassName( pWnd->GetSafeHwnd(), szName, 24 );		// Alt detection method
	//		//if ( _tcsistr( szName, _T("Static") ) )				// "Button" "ListBox" "ComboBox" "Edit" "RICHEDIT" etc
	//
	//		// Skinning fix for checkbox labels, dynamic text, etc.
	//		CRect rc;
	//		pWnd->GetWindowRect( &rc );
	//		CRect rcPos = rc;
	//		pWnd->ScreenToClient( &rc );
	//		ScreenToClient( &rcPos );
	//		rcPos.top -= Skin.m_nBanner;
	//
	//		CoolInterface.DrawWatermark( pDC, &rc, &Skin.m_bmDialog, FALSE, -rcPos.left, -rcPos.top );
	//		//pDC->BitBlt( 0, 0, rc.right, rc.bottom, pWnd->GetDC(), 0, 0, SRCCOPY );
	//	}
	//	else if ( pWnd->GetStyle() & SS_ICON )						// Static icon handling 32
	//		return Skin.m_brDialog;

		if ( pWnd->GetDlgCtrlID() != IDC_STATIC || ( pWnd->GetStyle() & SS_ICON ) )		// || ( pWnd->GetStyle() & SS_REALSIZEIMAGE ) )
		{
			// Offset background image brush to mimic transparency
			CRect rc;
			pWnd->GetWindowRect( &rc );
			ScreenToClient( &rc );
			rc.top -= Skin.m_nBanner;
			pDC->SetBrushOrg( -rc.left, -rc.top );

			hbr = Skin.m_brDialog;
		}
		else	// Static text
			hbr = (HBRUSH)GetStockObject( NULL_BRUSH );

		pDC->SetTextColor( Colors.m_crDialogText );
		pDC->SetBkMode( TRANSPARENT );
	}
	else if ( nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_DLG )	// Unskinned default behavior
	{
		pDC->SetTextColor( Colors.m_crDialogText );
		pDC->SetBkColor( Colors.m_crDialog );
		if ( Skin.m_brDialog.m_hObject )
			hbr = Skin.m_brDialog;
		else	// Pre-run message boxes (startup help/warning screens initial null used as white brush in some areas)
			hbr = CreateSolidBrush( Colors.m_crDialog );
	}

	return hbr;
}

//void CSkinDialog::OnUpdateUIState(UINT nAction, UINT nUIElement)
//{
//	CDialog::OnUpdateUIState( nAction, nUIElement );
//
//	// Obsolete workaround fix for skinned repaint bug when Alt key is first pressed (Accelerators activated)
//	if ( nAction == 2 && Skin.m_bmDialog.m_hObject )
//		Invalidate();
//}

#define SNAP_SIZE 6

void CSkinDialog::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging( lpwndpos );

	HMONITOR hMonitor = MonitorFromWindow( GetSafeHwnd(), MONITOR_DEFAULTTOPRIMARY );

	MONITORINFO oMonitor = {0};
	oMonitor.cbSize = sizeof( MONITORINFO );
	GetMonitorInfo( hMonitor, &oMonitor );

	if ( abs( lpwndpos->x - oMonitor.rcWork.left ) < SNAP_SIZE )
		lpwndpos->x = oMonitor.rcWork.left;
	if ( abs( lpwndpos->y - oMonitor.rcWork.top ) < SNAP_SIZE )
		lpwndpos->y = oMonitor.rcWork.top;
	if ( abs( lpwndpos->x + lpwndpos->cx - oMonitor.rcWork.right ) < SNAP_SIZE )
		lpwndpos->x = oMonitor.rcWork.right - lpwndpos->cx;
	if ( abs( lpwndpos->y + lpwndpos->cy - oMonitor.rcWork.bottom ) < SNAP_SIZE )
		lpwndpos->y = oMonitor.rcWork.bottom - lpwndpos->cy;

	// ToDo: Verify this is needed to fix a banner resize bug  (+ DoDataExchange above)
	//if ( m_oBanner.m_hWnd )
	//{
	//	if ( HBITMAP hBitmap = m_oBanner.GetBitmap() )
	//	{
	//		BITMAP bm = {};
	//		GetObject( hBitmap, sizeof( BITMAP ), &bm );
	//		CRect rcBanner;
	//		GetClientRect( &rcBanner );
	//		if ( Settings.General.LanguageRTL )
	//			rcBanner.left -= bm.bmWidth - rcBanner.Width();
	//		rcBanner.right = rcBanner.left + bm.bmWidth;
	//		rcBanner.bottom = rcBanner.top + bm.bmHeight;
	//		m_oBanner.MoveWindow( rcBanner );
	//	}
	//}
}

int CSkinDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CDialog::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( Settings.General.LanguageRTL )
		ModifyStyleEx( 0, WS_EX_LAYOUTRTL | WS_EX_RTLREADING );
	else
		ModifyStyleEx( WS_EX_LAYOUTRTL | WS_EX_RTLREADING, 0 );

	return 0;
}

BOOL CSkinDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

BOOL CSkinDialog::OnHelpInfo(HELPINFO* /*pHelpInfo*/)
{
	return FALSE;
}
