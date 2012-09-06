//
// WizardSheet.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Images.h"
#include "GProfile.h"
#include "WizardSheet.h"

#include "WizardWelcomePage.h"
#include "WizardConnectionPage.h"
#include "WizardNetworksPage.h"
#include "WizardFoldersPage.h"
#include "WizardSharePage.h"
#include "WizardInterfacePage.h"
#include "WizardProfilePage.h"
#include "WizardFinishedPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

#define CONTROLBAR_HEIGHT	44

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet

IMPLEMENT_DYNAMIC(CWizardSheet, CPropertySheetAdv)

BEGIN_MESSAGE_MAP(CWizardSheet, CPropertySheetAdv)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_XBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet run wizard

BOOL CWizardSheet::RunWizard(CWnd* pParent)
{
	BOOL bSuccess = FALSE;

	CWizardSheet pSheet( pParent );

	CWizardWelcomePage		pWelcome;
	CWizardConnectionPage	pConnection;
	CWizardNetworksPage		pNetworks;
	CWizardFoldersPage		pFolders;
	CWizardSharePage		pShare;
	CWizardInterfacePage	pInterface;
	CWizardProfilePage		pProfile;
	CWizardFinishedPage		pFinished;

	pSheet.AddPage( &pWelcome );
	pSheet.AddPage( &pConnection );
	pSheet.AddPage( &pNetworks );
	pSheet.AddPage( &pFolders );
	pSheet.AddPage( &pShare );
	pSheet.AddPage( &pInterface );
	pSheet.AddPage( &pProfile );
	pSheet.AddPage( &pFinished );

	bSuccess = ( pSheet.DoModal() == IDOK );
	Settings.Save();

	return bSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CWizardSheet construction

CWizardSheet::CWizardSheet(CWnd *pParentWnd, UINT iSelectPage)
{
	Construct( _T(""), pParentWnd, iSelectPage );

	SetWizardMode();
}

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet message handlers

BOOL CWizardSheet::OnInitDialog()
{
	CPropertySheetAdv::OnInitDialog();

	CRect rc;
	GetClientRect( &rc );

	CString strMessage;

	SetIcon( CoolInterface.ExtractIcon( IDR_MAINFRAME, FALSE ), FALSE );
	SetFont( &theApp.m_gdiFont );
//	LoadString( strMessage, IDS_WIZARD );
//	SetWindowText( _T( strMessage ) );

	GetDlgItem( ID_WIZBACK )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.OffsetRect( 95 + 6 - rc.left, -1 );
	GetDlgItem( ID_WIZBACK )->MoveWindow( &rc );

	GetDlgItem( ID_WIZNEXT )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.OffsetRect( 95 + 88 - rc.left, -1 );
	GetDlgItem( ID_WIZNEXT )->MoveWindow( &rc );

	LoadString( strMessage, IDS_GENERAL_FINISH );
	GetDlgItem( ID_WIZFINISH )->MoveWindow( &rc );
	GetDlgItem( ID_WIZFINISH )->SetWindowText( strMessage );

	LoadString( strMessage, IDS_GENERAL_BACK );
	if ( GetDlgItem( ID_WIZBACK ) )
		GetDlgItem( ID_WIZBACK )->SetWindowText( L"< " + strMessage );
	LoadString( strMessage, IDS_GENERAL_NEXT );
	if ( GetDlgItem( ID_WIZNEXT ) )
		GetDlgItem( ID_WIZNEXT )->SetWindowText( strMessage + L" >" );

	GetDlgItem( IDCANCEL )->GetWindowRect( &rc );
	ScreenToClient( &rc );
	rc.OffsetRect( 95 + 170 - rc.left, -1 );
	GetDlgItem( IDCANCEL )->MoveWindow( &rc );
	LoadString( strMessage, IDS_WIZARD_EXIT );
	GetDlgItem( IDCANCEL )->SetWindowText( strMessage );

	if ( GetDlgItem( IDHELP ) ) GetDlgItem( IDHELP )->ShowWindow( SW_HIDE );
	// ATL_IDC_STATIC1?
	if ( GetDlgItem( 0x3026 ) ) GetDlgItem( 0x3026 )->ShowWindow( SW_HIDE );

	//m_bmHeader.Attach( Skin.GetWatermark( _T("Banner") ) );	// Use Images.m_bmBanner

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
			pWnd->SetFont( &theApp.m_gdiFont, FALSE );
			pWnd = pWnd->GetWindow( GW_CHILD );
		}
	}

	while ( pWnd != NULL )
	{
		TCHAR szName[32];

		GetClassName( pWnd->GetSafeHwnd(), szName, 32 );

		if ( !_tcscmp( szName, _T("Static") ) )
			pWnd->SetFont( &theApp.m_gdiFont, FALSE );
		else if ( _tcscmp( szName, _T("RICHEDIT") ) )
			pWnd->SetFont( &theApp.m_gdiFont, TRUE );

		pWnd = pWnd->GetNextWindow();
	}

	return CPropertySheetAdv::OnChildNotify( message, wParam, lParam, pLResult );
}

void CWizardSheet::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheetAdv::OnSize( nType, cx, cy );

	if ( CWnd* pWnd = GetWindow( GW_CHILD ) )
	{
		GetClientRect( &m_rcPage );

		m_rcPage.top += Skin.m_nBanner;
		m_rcPage.bottom -= CONTROLBAR_HEIGHT + 2;

		pWnd->SetWindowPos( NULL, m_rcPage.left, m_rcPage.top, m_rcPage.Width(), m_rcPage.Height(), SWP_NOSIZE );
	}
}

void CWizardSheet::OnPaint()
{
	CPaintDC dc( this );

	CRect rc;
	GetClientRect( &rc );
	//rc.bottom = Images.m_nBanner;

	// ToDo: Use Images.DrawButtonState( &dc, rc, IMAGE_BANNER );

	CDC mdc;
	mdc.CreateCompatibleDC( &dc );
	CBitmap* pOldBitmap = (CBitmap*)mdc.SelectObject( &Images.m_bmBanner );
	dc.BitBlt( 0, 0, rc.right + 1, Skin.m_nBanner, &mdc, 0, 0, SRCCOPY );
	mdc.SelectObject( pOldBitmap );
	mdc.DeleteDC();

	//dc.Draw3dRect( 0, Skin.m_nBanner, rc.Width() + 1, 1, RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );

	//GetClientRect( &rc );
	rc.top = rc.bottom - CONTROLBAR_HEIGHT;

	dc.Draw3dRect( 0, rc.top - 2, rc.Width() + 1, 2, RGB( 142, 141, 140 ), RGB( 255, 255, 255 ) );	// ToDo: Skinned bevel color?

	if ( Images.m_bmDialog.m_hObject )
		CoolInterface.DrawWatermark( &dc, &rc, &Images.m_bmDialog );
	else
		dc.FillSolidRect( rc.left, rc.top, rc.Width(), CONTROLBAR_HEIGHT, Colors.m_crSysBtnFace );	// Colors.m_crDialog?
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

IMPLEMENT_DYNCREATE(CWizardPage, CPropertyPageAdv)

BEGIN_MESSAGE_MAP(CWizardPage, CPropertyPageAdv)
	ON_WM_SIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardPage construction

CWizardPage::CWizardPage(UINT nID) : CPropertyPageAdv( nID )
{
}

/////////////////////////////////////////////////////////////////////////////
// CWizardPage message handlers

void CWizardPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageAdv::OnSize(nType, cx, cy);

	CWizardSheet* pSheet = (CWizardSheet*)GetParent();

	if ( cx != pSheet->m_rcPage.Width() )
		MoveWindow( &pSheet->m_rcPage );
}

/////////////////////////////////////////////////////////////////////////////
// CWizardPage operations

CWizardSheet* CWizardPage::GetSheet()
{
	return (CWizardSheet*)GetParent();
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

		if ( _tcscmp( szName, _T("Static") ) ) continue;

		CString strText;
		pChild->GetWindowText( strText );

		for ( ;; )
		{
			int nPos = strText.Find( pszSearch );
			if ( nPos < 0 ) break;
			strText	= strText.Left( nPos ) + CString( pszReplace )
					+ strText.Mid( nPos + static_cast< int >( _tcslen( pszSearch ) ) );
		}

		pChild->SetWindowText( strText );
	}
}

BOOL CWizardPage::IsConnectionCapable()
{
	return ( ! theApp.m_bLimitedConnections || Settings.General.IgnoreXPsp2 )	// The connection rate limiting (XPsp2) makes multi-network performance awful
		&& ( Settings.Connection.InSpeed > 256 )								// Must have a decent connection to be worth it. (Or extra traffic will slow downloads)
		&& ( Settings.GetOutgoingBandwidth() > 16 );							// If your outbound bandwidth is too low, the ED2K ratio will throttle you anyway
}
