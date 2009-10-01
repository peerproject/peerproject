//
// WndHashProgressBar.cpp
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
#include "WndHashProgressBar.h"
#include "Settings.h"
#include "Library.h"
#include "LibraryBuilder.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Skin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CHashProgressBar, CWnd)

BEGIN_MESSAGE_MAP(CHashProgressBar, CWnd)
	//{{AFX_MSG_MAP(CHashProgressBar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define WINDOW_WIDTH		310
#define WINDOW_HEIGHT		46


/////////////////////////////////////////////////////////////////////////////
// CHashProgressBar construction

CHashProgressBar::CHashProgressBar() :
	m_pParent( NULL ),
	m_hIcon( NULL ),
	m_nFlash( 0 )
{
}

/////////////////////////////////////////////////////////////////////////////
// CHashProgressBar operations

void CHashProgressBar::Create(CWnd* pParent)
{
	m_pParent = pParent;
}

void CHashProgressBar::Run()
{
	if ( Settings.Library.HashWindow && LibraryBuilder.GetRemaining() )
	{
		m_sCurrent = LibraryBuilder.GetCurrent();
		int nPos = m_sCurrent.ReverseFind( '\\' );
		if ( nPos > 0 ) m_sCurrent = m_sCurrent.Mid( nPos + 1 );

		if ( m_hWnd == NULL )
		{
			try
			{
				CreateEx( WS_EX_TOPMOST | WS_EX_TOOLWINDOW,	AfxRegisterWndClass( CS_SAVEBITS |
					// Use CS_DROPSHADOW on Windows XP and above
					( ( theApp.m_bIsWin2000 == true /*|| theApp.m_nWindowsVersion < 5*/ ) ? 0 : CS_DROPSHADOW ) ),
					_T("PeerProject Hashing..."), WS_POPUP, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
					NULL, 0 );
			}
			catch (CResourceException* pEx)
			{
				pEx->Delete();
			}
		}
		if ( m_hWnd != NULL )
		{
			Update();
		}
	}
	else
	{
		if ( m_hWnd != NULL )
		{
			DestroyWindow();
			m_sCurrent.Empty();
			m_sPrevious.Empty();
		}
	}
}

void CHashProgressBar::Update()
{
	if ( m_sCurrent.GetLength() && m_sCurrent != m_sPrevious )
	{
		m_sPrevious = m_sCurrent;

		CClientDC dc( this );
		CFont* pOld = (CFont*)dc.SelectObject( &CoolInterface.m_fntCaption );
		CSize sz = dc.GetTextExtent( m_sPrevious );
		dc.SelectObject( pOld );

		int nWidth = 4 + 32 + sz.cx + 3;
		nWidth = max( nWidth, WINDOW_WIDTH );
		nWidth = min( nWidth, GetSystemMetrics( SM_CXSCREEN ) / 2 );
		Show( nWidth, FALSE );

		Invalidate( FALSE );
	}
}

void CHashProgressBar::Show(int nWidth, BOOL /*bShow*/)
{
	CRect rc;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rc, 0 );
	rc.left	= rc.right - nWidth - 4;
	rc.top	= rc.bottom - WINDOW_HEIGHT - 4;
	SetWindowPos( &wndTopMost, rc.left, rc.top, nWidth, WINDOW_HEIGHT,
		SWP_SHOWWINDOW | SWP_NOACTIVATE );
}

/////////////////////////////////////////////////////////////////////////////
// CHashProgressBar message handlers

int CHashProgressBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	OnSkinChange();

	m_nFlash = 0;
	SetTimer( 1, 50, NULL );

	return 0;
}

void CHashProgressBar::OnDestroy()
{
	KillTimer( 1 );
}

void CHashProgressBar::OnSkinChange()
{
	m_crFill	= Colors.m_crTipBack;
	m_crBorder	= Colors.m_crTipBorder;
	m_crText	= Colors.m_crTipText;

	HBITMAP hBitmap = Skin.GetWatermark( _T("CHashProgressBar") );
	if ( m_bmImage.m_hObject != NULL ) m_bmImage.DeleteObject();
	if ( hBitmap != NULL )	m_bmImage.Attach( hBitmap );

	m_hIcon = (HICON)LoadImage( AfxGetResourceHandle(),
		MAKEINTRESOURCE(IDI_SEARCH_FOLDER), IMAGE_ICON, 32, 32, 0 );
}

void CHashProgressBar::OnPaint()
{
	CRect rcClient;
	CPaintDC dc( this );

	GetClientRect( &rcClient );

	dc.Draw3dRect( &rcClient, m_crBorder, m_crBorder );
	rcClient.DeflateRect( 1, 1 );

	if ( ! CoolInterface.DrawWatermark( &dc, &rcClient, &m_bmImage ) )
		dc.FillSolidRect( &rcClient, m_crFill );

	dc.SetBkMode( TRANSPARENT );

	// Icon
	DrawIconEx( dc, rcClient.left + 5, rcClient.top + 4, m_hIcon, 32, 32,
		0, NULL, DI_NORMAL );

	// Text
	CFont* pOld = dc.SelectObject( &CoolInterface.m_fntNormal );

	CString strText = _T("x");
	CRect rcX( rcClient.right - 20, rcClient.top + 1,
		rcClient.right - 5, rcClient.top + 20 );
	dc.SetTextColor( m_crText );
	dc.DrawText( strText, rcX, DT_RIGHT | DT_SINGLELINE );

	if ( m_nFlash++ % 30 > 15 )
		dc.SetTextColor( Colors.m_crTextStatus );

	strText.Format( IDS_HASH_MESSAGE, LibraryBuilder.GetRemaining() );

	CSize sz = dc.GetTextExtent( strText );
	CRect rcText( rcClient.left + 32 + 12, rcClient.top + 3,
		rcClient.right - 6, rcClient.top + 3 + sz.cy );
	dc.DrawText( strText, rcText, DT_LEFT | DT_SINGLELINE );

//	dc.SelectObject( &CoolInterface.m_fntCaption );
	rcText.top = rcText.bottom + 4;
	rcText.bottom = rcClient.bottom - 10;
	dc.DrawText( m_sPrevious, rcText,
		DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS );

	dc.SelectObject( pOld );

	// Progress bar
	CRect rcProgress = rcClient;
	rcProgress.DeflateRect( 3, 40, 2, 2 );
	rcProgress.top = rcProgress.bottom - 2;
	float nPercentage = LibraryBuilder.GetProgress() / 100;
	if ( nPercentage < 0 || nPercentage > 1 ) nPercentage = 1;
	rcProgress.right = rcProgress.left + (INT)( rcProgress.Width() * nPercentage );

	dc.Draw3dRect( &rcProgress, Colors.m_crFragmentPass, Colors.m_crFragmentPass );
	rcProgress.top--;
	dc.Draw3dRect( &rcProgress, Colors.m_crFragmentPass, m_crText );
}

BOOL CHashProgressBar::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CHashProgressBar::OnTimer(UINT_PTR /*nIDEvent*/)
{
	CRect rcClient;
	GetClientRect( &rcClient );
	if ( m_nFlash % 15 == 1 )	// Cycle text 3x per 2 seconds
		rcClient.DeflateRect( 40, 3, 2, 2 );
	else						// Update only progress bar 20x per second
		rcClient.DeflateRect( 4, 40, 2, 2 );
	InvalidateRect( rcClient, FALSE );
}

void CHashProgressBar::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	if ( point.y < 14 && point.x > 296 )
		Settings.Library.HashWindow = FALSE;

	ShowWindow( SW_HIDE );
	KillTimer( 1 );
}
