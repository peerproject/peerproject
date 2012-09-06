//
// DlgSplash.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "ImageServices.h"
#include "ImageFile.h"
#include "DlgSplash.h"
#include "FragmentBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CSplashDlg, CDialog)

BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
END_MESSAGE_MAP()

//#ifndef WS_EX_LAYERED		// Win9x
//#define WS_EX_LAYERED		0x80000
//#define LWA_COLORKEY		0x01
//#define LWA_ALPHA 		0x02
//#endif
#ifndef AW_HIDE
#define AW_HIDE				0x00010000
#define AW_BLEND			0x00080000
#endif

#define SPLASH_WIDTH		530
#define SPLASH_HEIGHT		236


/////////////////////////////////////////////////////////////////////////////
// CSplashDlg construction

CSplashDlg::CSplashDlg(int nMax, bool bClosing)
	: CDialog( CSplashDlg::IDD, GetDesktopWindow() )
	, m_nWidth		( SPLASH_WIDTH )
	, m_nHeight		( SPLASH_HEIGHT )
	, m_nPos		( 0 )
	, m_nMax		( nMax )
	, m_bClosing	( bClosing )
	, m_sState		( theApp.m_sSmartAgent )
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

//	if ( ! theApp.m_bIsWin2000 )
//		SetClassLongPtr( GetSafeHwnd(), GCL_STYLE, GetClassLongPtr( GetSafeHwnd(), GCL_STYLE ) | CS_SAVEBITS | CS_DROPSHADOW );

	SetWindowText( m_sState );

	CClientDC dcScreen( this );

	//m_bmSplash.Attach( CImageFile::LoadBitmapFromFile( Settings.General.Path + L"\\Data\\Splash.png" ) );

	CImageFile pFile;
	if ( pFile.LoadFromFile( Settings.General.Path + L"\\Data\\Splash.png" ) )
	{
		pFile.EnsureRGB();
		HBITMAP bmHandle = pFile.CreateBitmap();
		m_bmSplash.Attach( bmHandle );
	}
	else if ( pFile.LoadFromResource( AfxGetResourceHandle(), IDR_LARGE_LOGO, RT_PNG ) )
	{
		// ToDo: Built-in media splash currently works as fallback, but this should be commented out if changed. (Note flat-color otherwise)
		HBITMAP bmHandle = pFile.CreateBitmap();
		m_bmSplash.Attach( bmHandle );
	}

	if ( m_bmSplash.m_hObject )
	{
		BITMAP bmInfo;
		m_bmSplash.GetObject( sizeof(BITMAP), &bmInfo );
		if ( bmInfo.bmHeight > 20 && bmInfo.bmWidth > 280 )
		{
			m_nWidth  = bmInfo.bmWidth;
			m_nHeight = bmInfo.bmHeight;
		}
	}

	m_bmBuffer.CreateCompatibleBitmap( &dcScreen, m_nWidth, m_nHeight );
	m_dcBuffer1.CreateCompatibleDC( &dcScreen );
	m_dcBuffer2.CreateCompatibleDC( &dcScreen );

	SetWindowPos( NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE );
	CenterWindow();

	//if ( GetSystemMetrics( SM_REMOTESESSION ) == 0 )	// Why?
		AnimateWindow( 180, AW_BLEND );

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

	if ( IsWindowVisible() )
		Sleep( 60 );	// Allow brief text and progress bar movement  (Waste a second for the appearance of speed)
}

void CSplashDlg::Topmost()
{
	if ( IsWindowVisible() )
		SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
}

void CSplashDlg::Hide(BOOL bAbort)
{
	if ( ! bAbort )
	{
		// Check if m_nMax was set too high during construction, or if not enough steps were run
		//ASSERT( m_nPos == m_nMax );

		m_sState = _T("Ready");
		SetWindowText( m_sState );
		Invalidate();

		// MFC Windows transition effect  (Fade, etc.)
		AnimateWindow( 180, AW_HIDE|AW_BLEND );
	}

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

	if ( m_bmSplash.m_hObject )
		m_dcBuffer2.BitBlt( 0, 0, m_nWidth, m_nHeight, &m_dcBuffer1, 0, 0, SRCCOPY );
	else // Missing File
		m_dcBuffer2.FillSolidRect( 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, RGB( 160, 50, 4 ) );	// Default Splash Color

	CFont* pOld3 = (CFont*)m_dcBuffer2.SelectObject( &theApp.m_gdiFontBold );
	m_dcBuffer2.SetBkMode( TRANSPARENT );

	CRect rc( 8, m_nHeight - 18, m_nWidth - 98, m_nHeight - 2 );		// Text Position
	UINT nFormat = DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX;

	m_dcBuffer2.SetTextColor( RGB( 120, 40, 10 ) );						// Text Outline/Fade
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
	m_dcBuffer2.SetTextColor( RGB( 40, 10, 0 ) );						// Text Outline/Shadow
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.left--;
	rc.top -= 2;

	m_dcBuffer2.SetTextColor( RGB( 255, 255, 255 ) );					// Text Color
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );

	m_dcBuffer2.SelectObject( pOld3 );

	rc.SetRect( m_nWidth - 90, m_nHeight - 14, m_nWidth - 8, m_nHeight - 5 );	// Progress Bar Position ( 440, 222, 522, 231 )
	m_dcBuffer2.Draw3dRect( &rc, RGB( 0x60, 0x40, 0 ), RGB( 0x20, 0, 0 ) );
	rc.DeflateRect( 1, 1 );												// Progress Bar Outline
	m_dcBuffer2.FillSolidRect( &rc, RGB( 0x30, 0x20, 0 ) ); 			// Progress Bar Background

	int nOffset;
	if ( Settings.General.LanguageRTL )
		nOffset = m_nMax - min( m_nPos, m_nMax );
	else
		nOffset = 0;

	CFragmentBar::DrawFragment( &m_dcBuffer2, &rc, m_nMax, nOffset, min( m_nPos, m_nMax ),
		RGB( 244, 140, 10 ), TRUE );									// Progress Bar Color
	m_dcBuffer2.SelectClipRgn( NULL );

	pDC->BitBlt( 0, 0, m_nWidth, m_nHeight, &m_dcBuffer2, 0, 0, SRCCOPY );

	m_dcBuffer2.SelectObject( pOld2 );
	m_dcBuffer1.SelectObject( pOld1 );
}
