//
// CtrlTaskPanel.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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
#include "CtrlTaskPanel.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CTaskPanel, CWnd)

BEGIN_MESSAGE_MAP(CTaskPanel, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CTaskBox, CButton)

BEGIN_MESSAGE_MAP(CTaskBox, CButton)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_NCACTIVATE()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_SYSCOMMAND()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()

#define CAPTION_HEIGHT	25
//#define MARGIN_WIDTH	12	// Settings.Skin.SidebarPadding, was m_nMargin


/////////////////////////////////////////////////////////////////////////////
// CTaskPanel construction

CTaskPanel::CTaskPanel()
	: m_pStretch	( NULL )
//	, m_nMargin 	( 12 )
//	, m_nCurve		( 2 )
	, m_bLayout		( FALSE )
{
}

/////////////////////////////////////////////////////////////////////////////
// CTaskPanel create

BOOL CTaskPanel::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CreateEx( WS_EX_CONTROLPARENT, NULL, lpszWindowName,
		dwStyle | WS_CHILD | WS_CLIPCHILDREN, rect, pParentWnd, nID, NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CTaskPanel box list management

CTaskBox* CTaskPanel::AddBox(CTaskBox* pBox, POSITION posBefore)
{
	ASSERT( pBox != NULL );
	if ( posBefore )
		m_pBoxes.InsertBefore( posBefore, pBox );
	else
		m_pBoxes.AddTail( pBox );

	OnChanged();

	return pBox;
}

POSITION CTaskPanel::GetBoxIterator() const
{
	return m_pBoxes.GetHeadPosition();
}

CTaskBox* CTaskPanel::GetNextBox(POSITION& pos) const
{
	return m_pBoxes.GetNext( pos );
}

INT_PTR CTaskPanel::GetBoxCount() const
{
	return m_pBoxes.GetCount();
}

void CTaskPanel::RemoveBox(CTaskBox* pBox)
{
	if ( POSITION pos = m_pBoxes.Find( pBox ) )
	{
		m_pBoxes.RemoveAt( pos );
		if ( m_pStretch == pBox ) m_pStretch = NULL;
		OnChanged();
	}
}

void CTaskPanel::ClearBoxes(BOOL bDelete)
{
	if ( bDelete )
	{
		for ( POSITION pos = GetBoxIterator() ; pos ; )
			delete GetNextBox( pos );
	}

	m_pBoxes.RemoveAll();
	m_pStretch = NULL;

	OnChanged();
}

void CTaskPanel::SetStretchBox(CTaskBox* pBox)
{
	m_pStretch = pBox;
}

//void CTaskPanel::SetMargin(int nMargin, int nCurve)
//{
//	m_nMargin = nMargin;
//	m_nCurve  = nCurve;
//}

void CTaskPanel::SetWatermark(LPCTSTR szWatermark)
{
	if ( m_bmWatermark.m_hObject != NULL ) m_bmWatermark.DeleteObject();

	HBITMAP hBitmap = Skin.GetWatermark( szWatermark );
	if ( hBitmap != NULL ) m_bmWatermark.Attach( hBitmap );
}

void CTaskPanel::SetFooter(LPCTSTR szWatermark)
{
	if ( m_bmFooter.m_hObject != NULL ) m_bmFooter.DeleteObject();

	HBITMAP hBitmap = Skin.GetWatermark( szWatermark );
	if ( hBitmap != NULL)
		m_bmFooter.Attach( hBitmap );
	else if ( Colors.m_crTaskPanelBack == RGB( 122, 160, 230 ) )	// Not RGB_DEFAULT_CASE
		m_bmFooter.LoadBitmap( IDB_TASKPANEL_FOOTER );
}

/////////////////////////////////////////////////////////////////////////////
// CTaskPanel message handlers

int CTaskPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	SetOwner( GetParent() );
	return 0;
}

void CTaskPanel::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize( nType, cx, cy );

	if ( m_pStretch != NULL && m_pStretch->GetOuterHeight() )
	{
		m_bLayout = TRUE;
		Invalidate();
	}
}

BOOL CTaskPanel::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CTaskPanel::OnPaint()
{
	CPaintDC dc( this );
	CRect rc;

	GetClientRect( &rc );

	if ( m_bLayout ) Layout( rc );

	if ( m_bmFooter.m_hObject != NULL )
	{
		BITMAP pInfo;
		m_bmFooter.GetBitmap( &pInfo );

		CRect rcFooter( &rc );
		rc.bottom = rcFooter.top = rcFooter.bottom - pInfo.bmHeight;

		CoolInterface.DrawWatermark( &dc, &rcFooter, &m_bmFooter );
	}

	if ( ! CoolInterface.DrawWatermark( &dc, &rc, &m_bmWatermark ) )
	{
		dc.FillSolidRect( &rc, Colors.m_crTaskPanelBack );
		CRect rcShadow( rc );
		rcShadow.bottom = rcShadow.top + 1;
		dc.FillSolidRect( &rcShadow, RGB( 92, 131, 200 ) );		// The top pixel row from BannerMark.bmp (?)

		GetWindowRect( &rc );
		ScreenToClient( &rc );

		CRect rcVertLine( rc );
		rcVertLine.left = rcVertLine.right - 1;
		dc.FillSolidRect( &rcVertLine, RGB( 92, 131, 200 ) );
	}
}

void CTaskPanel::OnChanged()
{
	m_bLayout = TRUE;
	if ( m_hWnd ) Invalidate();
}

void CTaskPanel::Layout(CRect& rcClient)
{
	CRect rcBox( &rcClient );

	rcBox.DeflateRect( Settings.Skin.SidebarPadding, Settings.Skin.SidebarPadding );

	int nStretch = rcBox.Height();

	if ( m_pStretch && m_pStretch->GetOuterHeight() )
	{
		for ( POSITION pos = GetBoxIterator() ; pos ; )
		{
			CTaskBox* pBox = GetNextBox( pos );

			if ( pBox->m_bVisible && pBox->m_nHeight && pBox != m_pStretch )
				nStretch -= pBox->GetOuterHeight() + Settings.Skin.SidebarPadding;
		}
	}

	// Prevent stretch boxes from having negative height
	nStretch = max( nStretch, CAPTION_HEIGHT * 2 );

	for ( POSITION pos = GetBoxIterator() ; pos ; )
	{
		CTaskBox* pBox = GetNextBox( pos );

		int nHeight = pBox->GetOuterHeight();

		if ( nHeight )
		{
			if ( pBox == m_pStretch && pBox->m_bOpen )
				nHeight = nStretch;

			rcBox.bottom = rcBox.top + nHeight;

			pBox->SetWindowPos( NULL, rcBox.left, rcBox.top, rcBox.Width(), rcBox.Height(),
				SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE );

			for ( CWnd* pChild = pBox->GetWindow( GW_CHILD ) ; pChild ;
				pChild = pChild->GetWindow( GW_HWNDNEXT ) )
			{
				pChild->ShowWindow( pBox->m_bOpen ? SW_SHOW : SW_HIDE );
			}

			rcBox.OffsetRect( 0, nHeight + Settings.Skin.SidebarPadding );
		}
		else if ( pBox->IsWindowVisible() )
		{
			pBox->ShowWindow( SW_HIDE );
		}
	}

	m_bLayout = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTaskBox construction

CTaskBox::CTaskBox()
	: m_pPanel		( NULL )
	, m_hIcon		( NULL )
	, m_nHeight		( 0 )
	, m_bVisible	( TRUE )
	, m_bOpen		( TRUE )
	, m_bHover		( FALSE )
	, m_bPrimary	( FALSE )
	, m_bCaptionCurve ( TRUE )
{
}

CTaskBox::~CTaskBox()
{
	if ( m_hIcon ) DestroyIcon( m_hIcon );
}

/////////////////////////////////////////////////////////////////////////////
// CTaskBox operations

BOOL CTaskBox::Create(CTaskPanel* pPanel, int nHeight, LPCTSTR pszCaption, UINT nIDIcon, UINT nID)
{
	CRect rect( 0, 0, 0, 0 );

	m_pPanel  = pPanel;
	m_nHeight = nHeight;

	if ( pPanel->m_hWnd )
	{
		pPanel->GetClientRect( &rect );
		rect.DeflateRect( Settings.Skin.SidebarPadding, 0 );
		rect.bottom = 0;
	}

	if ( ! CreateEx( WS_EX_CONTROLPARENT, NULL, pszCaption,
		WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, pPanel, nID ) ) return FALSE;

	if ( nIDIcon )
		CoolInterface.SetIcon( nIDIcon, Settings.General.LanguageRTL, FALSE, this );

	CString strKey;
	strKey.Format( _T("%s.Open"), (LPCTSTR)CString( GetRuntimeClass()->m_lpszClassName ) );
	m_bOpen = theApp.GetProfileInt( _T("Interface"), strKey, TRUE );

	return TRUE;
}

CTaskPanel* CTaskBox::GetPanel() const
{
	ASSERT( m_pPanel != NULL );
	ASSERT_KINDOF( CTaskPanel, m_pPanel );
	return m_pPanel;
}

void CTaskBox::SetCaption(LPCTSTR pszCaption)
{
	CString strOld;
	GetWindowText( strOld );

	if ( strOld != pszCaption )
	{
		SetWindowText( pszCaption );
		InvalidateNonclient();
	}
}

void CTaskBox::SetIcon(HICON hIcon)
{
	if ( m_hIcon && m_hIcon != hIcon )
		DestroyIcon( m_hIcon );

	m_hIcon = hIcon;

	CWnd::SetIcon( m_hIcon, FALSE );
	InvalidateNonclient();
}

void CTaskBox::SetSize(int nHeight)
{
	if ( m_nHeight == nHeight ) return;
	m_nHeight = nHeight;
	if ( m_pPanel ) m_pPanel->OnChanged();
}

void CTaskBox::SetPrimary(BOOL bPrimary)
{
	if ( m_bPrimary == bPrimary ) return;
	m_bPrimary = bPrimary;
	if ( m_pPanel ) m_pPanel->OnChanged();
}

void CTaskBox::SetWatermark(LPCTSTR szWatermark)
{
	if ( m_bmWatermark.m_hObject != NULL ) m_bmWatermark.DeleteObject();

	HBITMAP hBitmap = Skin.GetWatermark( szWatermark );
	if ( hBitmap != NULL ) m_bmWatermark.Attach( hBitmap );
}

void CTaskBox::SetCaptionmark(LPCTSTR szWatermark)
{
	if ( m_bmCaptionmark.m_hObject != NULL ) m_bmCaptionmark.DeleteObject();

	HBITMAP hBitmap = Skin.GetWatermark( szWatermark );

	if ( hBitmap == NULL )
		hBitmap = Skin.GetWatermark( _T("CTaskBox.Caption") );

	if ( hBitmap != NULL )
		m_bmCaptionmark.Attach( hBitmap );
	else if ( m_bPrimary && Colors.m_crTaskBoxPrimaryBack == RGB_DEFAULT_CASE )		// Was RGB( 30, 87, 199 )
		m_bmCaptionmark.LoadBitmap( IDB_TASKBOX_CAPTION );

	m_bCaptionCurve = TRUE;

	if ( m_bmCaptionmark.m_hObject != NULL )
	{
		BITMAP pInfo;
		m_bmCaptionmark.GetBitmap( &pInfo );
		m_bCaptionCurve = pInfo.bmWidth < 176;
	}
}

void CTaskBox::Expand(BOOL bOpen)
{
	if ( m_bOpen == bOpen ) return;
	m_bOpen = bOpen;

	if ( m_pPanel != NULL )
	{
		m_pPanel->OnChanged();
		OnExpanded( m_bOpen );
	}

	CString strKey;
	strKey.Format( _T("%s.Open"), (LPCTSTR)CString( GetRuntimeClass()->m_lpszClassName ) );
	theApp.WriteProfileInt( _T("Interface"), strKey, m_bOpen );

	InvalidateNonclient();
}

void CTaskBox::OnExpanded(BOOL /*bOpen*/)
{
}

int CTaskBox::GetOuterHeight() const
{
	if ( ! m_bVisible || ! m_nHeight ) return 0;
	return CAPTION_HEIGHT + ( m_bOpen ? m_nHeight + 1 : 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CTaskBox message handlers

void CTaskBox::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	NCCALCSIZE_PARAMS* pSize = (NCCALCSIZE_PARAMS*)lpncsp;

	pSize->rgrc[0].left ++;
	pSize->rgrc[0].top += CAPTION_HEIGHT;
	pSize->rgrc[0].right --;
	pSize->rgrc[0].bottom --;
}

LRESULT CTaskBox::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		if ( point.y < rc.top + CAPTION_HEIGHT ) return HTCAPTION;
		return HTCLIENT;
	}

	return HTNOWHERE;
}

BOOL CTaskBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( nHitTest == HTCAPTION )
	{
		if ( ! m_bHover )
		{
			m_bHover = TRUE;
			PaintBorders();
			SetTimer( 1, 50, NULL );
		}

		::SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}

	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CTaskBox::OnNcLButtonDown(UINT /*nHitTest*/, CPoint /*point*/)
{
}

void CTaskBox::OnNcLButtonUp(UINT nHitTest, CPoint /*point*/)
{
	if ( nHitTest == HTCAPTION )
		Expand( ! m_bOpen );
}

void CTaskBox::OnTimer(UINT_PTR /*nIDEvent*/)
{
	CPoint point;
	GetCursorPos( &point );

	if ( OnNcHitTest( point ) != HTCAPTION )
	{
		KillTimer( 1 );
		m_bHover = FALSE;
		PaintBorders();
	}
}

BOOL CTaskBox::OnNcActivate(BOOL /*bActive*/)
{
	PaintBorders();
	return TRUE;
}

void CTaskBox::OnNcPaint()
{
	PaintBorders();
}

void CTaskBox::PaintBorders()
{
	CWindowDC dc( this );
	CString strCaption;
	CRect rc, rcc;

	GetWindowRect( &rc );
	rc.OffsetRect( -rc.left, -rc.top );

	if ( m_bCaptionCurve )	// && m_pPanel->m_nCurve != 0
	{
		dc.SetPixel( 0, 0, Colors.m_crTaskPanelBack );
		dc.SetPixel( 1, 0, Colors.m_crTaskPanelBack );
		dc.SetPixel( 0, 1, Colors.m_crTaskPanelBack );
		dc.SetPixel( rc.right - 1, 0, Colors.m_crTaskPanelBack );
		dc.SetPixel( rc.right - 2, 0, Colors.m_crTaskPanelBack );
		dc.SetPixel( rc.right - 1, 1, Colors.m_crTaskPanelBack );

		dc.ExcludeClipRect( 0, 0, 2, 1 );
		dc.ExcludeClipRect( 0, 1, 1, 2 );
		dc.ExcludeClipRect( rc.right - 2, 0, rc.right, 1 );
		dc.ExcludeClipRect( rc.right - 1, 1, rc.right, 2 );
	}

	rcc.SetRect( 0, 0, rc.right, CAPTION_HEIGHT );

	CSize size= rcc.Size();
	CDC* pBuffer = CoolInterface.GetBuffer( dc, size );

	if ( m_bmCaptionmark.m_hObject != NULL )
		CoolInterface.DrawWatermark( pBuffer, &rcc, &m_bmCaptionmark );
	else
		pBuffer->FillSolidRect( &rcc, m_bPrimary ?
			Colors.m_crTaskBoxPrimaryBack : Colors.m_crTaskBoxCaptionBack );

	CPoint ptIcon( 6, rcc.Height() / 2 - 7 );

	DrawIconEx( pBuffer->GetSafeHdc(), ptIcon.x, ptIcon.y, CWnd::GetIcon( FALSE ),
		16, 16, 0, NULL, DI_NORMAL );

	GetWindowText( strCaption );

	CFont* pOldFont = (CFont*)pBuffer->SelectObject( &CoolInterface.m_fntBold );
	CSize sz = pBuffer->GetTextExtent( strCaption );

	pBuffer->SetBkMode( TRANSPARENT );
	pBuffer->SetTextColor( m_bHover ? Colors.m_crTaskBoxCaptionHover :
		( m_bPrimary ? Colors.m_crTaskBoxPrimaryText : Colors.m_crTaskBoxCaptionText ) );

	pBuffer->ExtTextOut( ptIcon.x * 2 + 16 + 1, rcc.Height() / 2 - sz.cy / 2,
		ETO_CLIPPED, &rcc, strCaption, NULL );

	pBuffer->SelectObject( pOldFont );

	dc.BitBlt( rc.left, rc.top, rcc.Width(), rcc.Height(), pBuffer, 0, 0, SRCCOPY );

	dc.ExcludeClipRect( &rcc );

	if ( m_bOpen )
	{
		rc.top = rcc.bottom - 1;
		dc.Draw3dRect( &rc, Colors.m_crTaskBoxCaptionBack, Colors.m_crTaskBoxCaptionBack );
	}

	dc.SelectStockObject( SYSTEM_FONT );	// GDI font leak fix
}

void CTaskBox::InvalidateNonclient()
{
	if ( m_hWnd )
	{
		CRect rc;
		GetWindowRect( &rc );
		rc.bottom = rc.top + CAPTION_HEIGHT;
		ScreenToClient( &rc );
		RedrawWindow( &rc, NULL, RDW_FRAME|RDW_INVALIDATE );
	}
}

void CTaskBox::OnPaint()
{
	CPaintDC dc( this );

	CRect rc;
	GetClientRect( &rc );

	if ( ! CoolInterface.DrawWatermark( &dc, &rc, &m_bmWatermark ) )
	{
		dc.FillSolidRect( &rc, Colors.m_crTaskBoxClient );
		dc.SetBkColor( Colors.m_crTaskBoxClient );
	}
	else
	{
		dc.SetBkColor( CLR_NONE );
	}

	dc.SetTextColor( Colors.m_crTaskBoxText );
}

void CTaskBox::OnSysCommand(UINT /*nID*/, LPARAM /*lParam*/)
{
}
