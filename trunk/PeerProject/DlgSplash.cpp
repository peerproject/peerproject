//
// DlgSplash.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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
#include "Settings.h"
#include "ImageServices.h"
#include "ImageFile.h"
#include "DlgSplash.h"
#include "FragmentBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CSplashDlg, CDialog)

BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
END_MESSAGE_MAP()

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED		0x80000
#define LWA_ALPHA			0x02
#endif
#define AW_BLEND			0x00080000
#define AW_HIDE				0x00010000

#define SPLASH_WIDTH		530
#define SPLASH_HEIGHT		236


/////////////////////////////////////////////////////////////////////////////
// CSplashDlg construction

CSplashDlg::CSplashDlg(int nMax, bool bClosing) :
	CDialog( CSplashDlg::IDD, GetDesktopWindow() ),
	m_nPos( 0 ),
	m_nMax( nMax ),
	m_bClosing( bClosing ),
	m_sState( theApp.m_sSmartAgent ),
	m_pfnAnimateWindow( NULL )
{
	Create( IDD, GetDesktopWindow() );
}

CSplashDlg::~CSplashDlg()
{
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg message handlers

BOOL CSplashDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText( m_sState );

	CClientDC dcScreen( this );

	CImageFile pFile;
	pFile.LoadFromFile( Settings.General.Path + L"\\Data\\Splash.png" );
	pFile.EnsureRGB();
	HBITMAP m_bmHandle = pFile.CreateBitmap();
	m_bmSplash.Attach( m_bmHandle );

	m_bmBuffer.CreateCompatibleBitmap( &dcScreen, SPLASH_WIDTH, SPLASH_HEIGHT );
	m_dcBuffer1.CreateCompatibleDC( &dcScreen );
	m_dcBuffer2.CreateCompatibleDC( &dcScreen );

	SetWindowPos( NULL, 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, SWP_NOMOVE );
	CenterWindow();

	if ( GetSystemMetrics( SM_REMOTESESSION ) == 0 )
		AnimateWindow( 250, AW_BLEND );

	SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
	UpdateWindow();

	return TRUE;
}

void CSplashDlg::Step(LPCTSTR pszText)
{
	// Check if m_nMax was set high enough during construction to allow another step to take place
	ASSERT( m_nPos < m_nMax );

	m_nPos ++;
	m_sState.Format( m_bClosing ? _T("%s...") : _T("Starting %s..."), pszText );
	SetWindowText( m_sState );

	CClientDC dc( this );
	DoPaint( &dc );
}

void CSplashDlg::Topmost()
{
	if ( IsWindowVisible() )
		SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
}

void CSplashDlg::Hide()
{
	// Check if m_nMax was set too high during construction, or if not enough steps were run
	//ASSERT( m_nPos == m_nMax );

	m_sState = _T("Ready");
	SetWindowText( m_sState );
	Invalidate();

	if ( m_pfnAnimateWindow != NULL )
		(*m_pfnAnimateWindow)( GetSafeHwnd(), 250, AW_HIDE|AW_BLEND );

	::DestroyWindow( m_hWnd );
	delete this;
}

LRESULT CSplashDlg::OnPrintClient(WPARAM wParam, LPARAM /*lParam*/)
{
	LRESULT lResult = Default();

	CDC* pDC = CDC::FromHandle( (HDC)wParam );
	DoPaint( pDC );

	return lResult;
}

void CSplashDlg::OnPaint()
{
	CPaintDC dc( this );
	DoPaint( &dc );
}

void CSplashDlg::DoPaint(CDC* pDC)
{
	CBitmap* pOld1 = (CBitmap*)m_dcBuffer1.SelectObject( &m_bmSplash );
	CBitmap* pOld2 = (CBitmap*)m_dcBuffer2.SelectObject( &m_bmBuffer );

	m_dcBuffer2.BitBlt( 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, &m_dcBuffer1, 0, 0, SRCCOPY );

	CFont* pOld3 = (CFont*)m_dcBuffer2.SelectObject( &theApp.m_gdiFontBold );
	m_dcBuffer2.SetBkMode( TRANSPARENT );

	CRect rc( 8, 216, 520, SPLASH_HEIGHT );						// Text Position
	UINT nFormat = DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX;

	m_dcBuffer2.SetTextColor( RGB( 120, 40, 10 ) );				// Text Outline/Fade
	rc.left--;
	rc.top += 2;
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.top -= 4;
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.left += 2;
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.left++;
	rc.top += 5;
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.left--;
	rc.top--;
	m_dcBuffer2.SetTextColor( RGB( 40, 10, 0 ) );				// Text Outline/Shadow
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.left--;
	rc.top -= 2;

	m_dcBuffer2.SetTextColor( RGB( 255, 255, 255 ) );			// Text Color
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );

	m_dcBuffer2.SelectObject( pOld3 );

	rc.SetRect( 440, 222, 522, 231 );							// Progress Bar Position
	m_dcBuffer2.Draw3dRect( &rc, RGB( 0x60, 0x40, 0 ), RGB( 0x20, 0, 0 ) );
	rc.DeflateRect( 1, 1 );										// Progress Bar Outline
	m_dcBuffer2.FillSolidRect( &rc, RGB( 0x30, 0x20, 0 ) ); 	// Progress Bar Background

	int nOffset;
	if ( Settings.General.LanguageRTL )
		nOffset = m_nMax - min( m_nPos, m_nMax );
	else
		nOffset = 0;

	CFragmentBar::DrawFragment( &m_dcBuffer2, &rc, m_nMax, nOffset, min( m_nPos, m_nMax ),
		RGB( 244, 140, 10 ), TRUE );							// Progress Bar Color
	m_dcBuffer2.SelectClipRgn( NULL );

	pDC->BitBlt( 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, &m_dcBuffer2, 0, 0, SRCCOPY );

	m_dcBuffer2.SelectObject( pOld2 );
	m_dcBuffer1.SelectObject( pOld1 );
}
