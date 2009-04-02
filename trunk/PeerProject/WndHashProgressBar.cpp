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
#include "Library.h"
#include "LibraryBuilder.h"
#include "CoolInterface.h"
#include "WndHashProgressBar.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CHashProgressBar, CWnd)

BEGIN_MESSAGE_MAP(CHashProgressBar, CWnd)
	//{{AFX_MSG_MAP(CHashProgressBar)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define WINDOW_WIDTH		320
#define WINDOW_HEIGHT		48


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
				CreateEx( WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
					AfxRegisterWndClass( CS_SAVEBITS |
					// Use CS_DROPSHADOW on Windows XP and above
					( ( theApp.m_bIsWin2000 == true || theApp.m_nWindowsVersion < 5 ) ? 0 : CS_DROPSHADOW ) ),
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

		int nWidth = 4 + 32 + sz.cx + 4;
		nWidth = max( nWidth, WINDOW_WIDTH );
		nWidth = min( nWidth, GetSystemMetrics( SM_CXSCREEN ) / 2 );
		Show( nWidth, FALSE );
	}

	Invalidate( FALSE );
}

void CHashProgressBar::Show(int nWidth, BOOL /*bShow*/)
{
	CRect rc;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rc, 0 );
	rc.left	= rc.right - nWidth;
	rc.top	= rc.bottom - WINDOW_HEIGHT;
	SetWindowPos( &wndTopMost, rc.left - 4, rc.top - 4, rc.Width(), rc.Height(),
		SWP_SHOWWINDOW | SWP_NOACTIVATE );
}

/////////////////////////////////////////////////////////////////////////////
// CHashProgressBar message handlers

int CHashProgressBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	m_hIcon = (HICON)LoadImage( AfxGetResourceHandle(),
		MAKEINTRESOURCE(IDI_SEARCH_FOLDER), IMAGE_ICON, 32, 32, 0 );

	m_crFill	= CoolInterface.m_crTipBack ;
	m_crBorder	= CoolInterface.m_crTipBorder;
	m_crText	= CoolInterface.m_crTipText;

	if ( m_brFill.m_hObject != NULL ) m_brFill.DeleteObject();
	m_brFill.CreateSolidBrush( m_crFill );

	return 0;
}

BOOL CHashProgressBar::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CHashProgressBar::OnPaint()
{
	CRect rcClient;
	CPaintDC dc( this );

	GetClientRect( &rcClient );

	dc.Draw3dRect( &rcClient, m_crBorder, m_crBorder );
	rcClient.DeflateRect( 1, 1 );
	dc.FillSolidRect( &rcClient, m_crFill );

	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( ( m_nFlash++ & 1 ) ? CoolInterface.m_crTextStatus : m_crText );

	// Icon
	DrawIconEx( dc, rcClient.left + 4, rcClient.top + 4, m_hIcon, 32, 32,
		0, m_brFill, DI_NORMAL );

	// Text
	CString strText;
	strText.Format( IDS_HASH_MESSAGE, LibraryBuilder.GetRemaining() );
	
	CFont* pOld = dc.SelectObject( &CoolInterface.m_fntNormal );
	CSize sz = dc.GetTextExtent( strText );
	CRect rcText( rcClient.left + 4 + 32 + 8, rcClient.top + 4,
		rcClient.right - 8, rcClient.top + 4 + sz.cy );
	dc.DrawText( strText, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	dc.SelectObject( &CoolInterface.m_fntCaption );
	rcText.top = rcText.bottom + 4;
	rcText.bottom = rcClient.bottom - 8;
	dc.DrawText( m_sPrevious, rcText,
		DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );

	dc.SelectObject( pOld );

	// Progress bar
	CRect rcProgress = rcClient;
	rcProgress.DeflateRect( 1, 1 );
	rcProgress.top = rcProgress.bottom - 3;
	float nPercentage = LibraryBuilder.GetProgress() / 100.0f;
	if ( ( nPercentage < 0 ) || ( nPercentage > 1 ) ) nPercentage = 1;
	rcProgress.right = rcProgress.left + (LONG)( rcProgress.Width() * nPercentage );
	dc.Draw3dRect( &rcProgress, m_crText, m_crText );
}

void CHashProgressBar::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/)
{
	Settings.Library.HashWindow = FALSE;
	ShowWindow( SW_HIDE );
}
