//
// WndSettingsSheet.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
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
#include "Colors.h"
#include "WndSettingsSheet.h"
#include "WndSettingsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSettingsSheet, CSkinDialog)

BEGIN_MESSAGE_MAP(CSettingsSheet, CSkinDialog)
	ON_WM_PAINT()
	ON_COMMAND(IDRETRY, OnApply)
	ON_NOTIFY(TVN_ITEMEXPANDINGW, IDC_SETTINGS_TREE, OnTreeExpanding)
	ON_NOTIFY(TVN_ITEMEXPANDINGA, IDC_SETTINGS_TREE, OnTreeExpanding)
	ON_NOTIFY(TVN_SELCHANGEDW, IDC_SETTINGS_TREE, OnSelectPage)
	ON_NOTIFY(TVN_SELCHANGEDA, IDC_SETTINGS_TREE, OnSelectPage)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet construction

CSettingsSheet::CSettingsSheet(CWnd* pParent, UINT nCaptionID) :
	CSkinDialog( 0, pParent, FALSE )
	, m_pPage			( NULL )
	, m_pFirst			( NULL )
	, m_pTemplate		( NULL )
	, m_bModified		( FALSE )
	, m_nLeftMargin		( 0 )
	, m_nTopMargin		( 0 )
	, m_nListWidth		( 120 )
	, m_nListMargin		( 6 )
	, m_nButtonHeight	( 20 )
{

	if ( nCaptionID ) m_sCaption.LoadString( nCaptionID );
}

CSettingsSheet::~CSettingsSheet()
{
}

void CSettingsSheet::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange( pDX );
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet operations

void CSettingsSheet::AddPage(CSettingsPage* pPage, LPCTSTR pszCaption)
{
	if ( pszCaption ) pPage->m_sCaption = pszCaption;
	pPage->m_bGroup = FALSE;
	m_pPages.Add( pPage );
}

void CSettingsSheet::AddGroup(CSettingsPage* pPage, LPCTSTR pszCaption)
{
	if ( pszCaption ) pPage->m_sCaption = pszCaption;
	pPage->m_bGroup = TRUE;
	m_pPages.Add( pPage );
}

CSettingsPage* CSettingsSheet::GetPage(INT_PTR nPage) const
{
	return m_pPages.GetAt( nPage );
}

CSettingsPage* CSettingsSheet::GetPage(CRuntimeClass* pClass) const
{
	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CSettingsPage* pPage = GetPage( nPage );
		if ( pPage->IsKindOf( pClass ) ) return pPage;
	}
	return NULL;
}

CSettingsPage* CSettingsSheet::GetPage(LPCTSTR pszClass) const
{
	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CSettingsPage* pPage = GetPage( nPage );
		if ( _tcscmp( CString( pPage->GetRuntimeClass()->m_lpszClassName ), pszClass ) == 0 ) return pPage;
	}
	return NULL;
}

INT_PTR CSettingsSheet::GetPageIndex(CSettingsPage* pPage) const
{
	for ( INT_PTR nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		if ( pPage == GetPage( nPage ) ) return nPage;
	}
	return -1;
}

INT_PTR CSettingsSheet::GetPageCount() const
{
	return m_pPages.GetSize();
}

CSettingsPage* CSettingsSheet::GetActivePage() const
{
	return m_pPage;
}

BOOL CSettingsSheet::SetActivePage(CSettingsPage* pPage)
{
	if ( pPage == NULL || pPage == m_pPage ) return FALSE;

	ASSERT_KINDOF(CSettingsPage, pPage);

	if ( m_hWnd == NULL )
	{
		m_pFirst = pPage;
		return TRUE;
	}

	if ( m_pPage != NULL )
	{
		if ( ! m_pPage->OnKillActive() ) return FALSE;
		m_pPage->ShowWindow( SW_HIDE );
	}

	if ( pPage->m_hWnd == NULL && ! CreatePage( pPage ) ) return FALSE;
	if ( ! pPage->OnSetActive() ) return FALSE;

	m_pPage = pPage;
	m_pPage->ShowWindow( SW_SHOW );

	CString strCaption;
	m_pPage->GetWindowText( strCaption );
	strCaption.Trim();
	if ( ! strCaption.IsEmpty() )
		SetWindowText( m_sCaption + _T(" - ") + strCaption );
	else
		SetWindowText( m_sCaption );

	for ( HTREEITEM hGroup = m_wndTree.GetRootItem() ; hGroup ; hGroup = m_wndTree.GetNextItem( hGroup, TVGN_NEXT ) )
	{
		if ( m_wndTree.GetItemData( hGroup ) == (DWORD_PTR)m_pPage )
		{
			if ( ( m_wndTree.GetItemState( hGroup, TVIS_SELECTED ) & TVIS_SELECTED ) == 0 )
			{
				m_wndTree.SelectItem( hGroup );
			}
		}
		for ( HTREEITEM hItem = m_wndTree.GetChildItem( hGroup ) ; hItem ; hItem = m_wndTree.GetNextItem( hItem, TVGN_NEXT ) )
		{
			if ( m_wndTree.GetItemData( hItem ) == (DWORD_PTR)m_pPage )
			{
				if ( ( m_wndTree.GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED ) == 0 )
				{
					m_wndTree.SelectItem( hItem );
				}
			}
		}
	}

	return TRUE;
}

BOOL CSettingsSheet::SetActivePage(int nPage)
{
	return SetActivePage( GetPage( nPage ) );
}

BOOL CSettingsSheet::IsModified() const
{
	return m_bModified;
}

void CSettingsSheet::SetModified(BOOL bChanged)
{
	if ( m_bModified == bChanged ) return;
	m_bModified = bChanged;
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet message handlers

INT_PTR CSettingsSheet::DoModal()
{
	m_pTemplate = (DLGTEMPLATE *)new char[ sizeof(DLGTEMPLATE) + 6 ];
	if ( ! m_pTemplate )
		return IDCANCEL;

	ZeroMemory( m_pTemplate, sizeof(DLGTEMPLATE) + 6 );

	DWORD dwExStyle = Settings.General.LanguageRTL ? WS_EX_RTLREADING|WS_EX_RIGHT|WS_EX_LEFTSCROLLBAR|WS_EX_LAYOUTRTL :
		WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR;

	m_pTemplate->style				= WS_POPUPWINDOW|WS_VISIBLE|WS_DLGFRAME|WS_OVERLAPPED|DS_MODALFRAME;
	m_pTemplate->dwExtendedStyle	= dwExStyle|WS_EX_DLGMODALFRAME|WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT;

	m_pTemplate->cdit	= 0;
	m_pTemplate->x		= 0;
	m_pTemplate->y		= 0;
	m_pTemplate->cx		= 100;
	m_pTemplate->cy		= 100;

	m_pPage		= NULL;
	m_bModified	= FALSE;

	CSkinDialog::InitModalIndirect( m_pTemplate, m_pParentWnd );

	INT_PTR nResult = CSkinDialog::DoModal();

	delete [] m_pTemplate;

	m_pTemplate		= NULL;
	m_pParentWnd	= NULL;
	m_pFirst		= m_pPage;
	m_pPage			= NULL;

	return nResult;
}

BOOL CSettingsSheet::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SetWindowText( m_sCaption );

	CRect rect;
	m_wndTree.Create( WS_CHILD|WS_TABSTOP|WS_VISIBLE|/*TVS_PRIVATEIMAGELISTS|*/
		TVS_HASLINES|TVS_SHOWSELALWAYS|TVS_TRACKSELECT|TVS_NOSCROLL, rect, this, IDC_SETTINGS_TREE );
		// ToDo: Use TVS_NOHSCROLL instead to add vertical scrollbar to Settings Tree

	m_wndOK.Create( _T("OK"), WS_CHILD|WS_TABSTOP|WS_VISIBLE|BS_DEFPUSHBUTTON, rect, this, IDOK );
	m_wndOK.SetFont( &theApp.m_gdiFont );
	m_wndCancel.Create( _T("Cancel"), WS_CHILD|WS_TABSTOP|WS_VISIBLE, rect, this, IDCANCEL );
	m_wndCancel.SetFont( &theApp.m_gdiFont );
	m_wndApply.Create( _T("Apply"), WS_CHILD|WS_TABSTOP|WS_VISIBLE, rect, this, IDRETRY );
	m_wndApply.SetFont( &theApp.m_gdiFont );

	Layout();
	CenterWindow();

	if ( m_pFirst == NULL ) m_pFirst = GetPage( INT_PTR(0) );
	SetActivePage( m_pFirst );

	BuildTree();

	return TRUE;
}

void CSettingsSheet::BuildTree()
{
	HTREEITEM hGroup = NULL;

	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CSettingsPage* pPage = GetPage( nPage );

		if ( pPage->m_bGroup ) hGroup = NULL;

		HTREEITEM hItem = m_wndTree.InsertItem(
			TVIF_PARAM|TVIF_TEXT|TVIF_STATE,
			pPage->m_sCaption, 0, 0, TVIS_EXPANDED|(TVIS_BOLD*pPage->m_bGroup),
			TVIS_EXPANDED|TVIS_BOLD, (LPARAM)pPage, hGroup, TVI_LAST );

		if ( pPage->m_bGroup ) hGroup = hItem;

		if ( pPage == m_pPage ) m_wndTree.SelectItem( hItem );
	}
}

void CSettingsSheet::Layout()
{
	TEXTMETRIC txtMetric;

	CDC* pDC = GetDC();
	CFont* pOldFont = pDC->SelectObject( &theApp.m_gdiFont );
	pDC->GetTextMetrics( &txtMetric );
	pDC->SelectObject( pOldFont );
	ReleaseDC( pDC );

	m_nButtonHeight = ( txtMetric.tmHeight + txtMetric.tmExternalLeading ) + 10;

	m_szPages.cx = m_szPages.cy = 0;

	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CSettingsPage* pPage = GetPage( nPage );
		CDialogTemplate pTemplate;

		if ( pPage->GetTemplateName() == NULL )  continue;

		if ( pTemplate.Load( pPage->GetTemplateName() ) )
		{
			CSize size;
			pTemplate.GetSizeInPixels( &size );
			m_szPages.cx = max( m_szPages.cx, size.cx );
			m_szPages.cy = max( m_szPages.cy, size.cy );
		}
	}

	CRect rc( 0, 0, m_szPages.cx, m_szPages.cy );
	rc.right += m_nListWidth + m_nListMargin;
	rc.right += m_nLeftMargin;
	rc.bottom += m_nTopMargin + m_nButtonHeight + 16;

	CalcWindowRect( &rc );
	SetWindowPos( &wndTop, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE|SWP_NOZORDER );

	rc.SetRect( m_nLeftMargin, m_nTopMargin, 0, 0 );
	rc.right	= rc.left + m_nListWidth;
	rc.bottom	= rc.top  + m_szPages.cy;

	m_wndTree.MoveWindow( &rc );

	rc.SetRect( 8, rc.bottom + 8, 76, m_nButtonHeight );
	rc.right += rc.left;
	rc.bottom += rc.top;

	m_wndOK.MoveWindow( &rc );
	rc.OffsetRect( rc.Width() + 8, 0 );
	m_wndCancel.MoveWindow( &rc );
	rc.OffsetRect( rc.Width() + 8, 0 );
	m_wndApply.MoveWindow( &rc );
}

BOOL CSettingsSheet::CreatePage(CSettingsPage* pPage)
{
	CRect rc( m_nLeftMargin, m_nTopMargin, 0, 0 );

	rc.left		+= m_nListWidth + m_nListMargin;
	rc.right	= rc.left + m_szPages.cx;
	rc.bottom	= rc.top  + m_szPages.cy;

	return pPage->Create( rc, this );
}

void CSettingsSheet::OnTreeExpanding(NMHDR* /*pNotify*/, LRESULT *pResult)
{
	*pResult = TRUE;
}

void CSettingsSheet::OnSelectPage(NMHDR* pNotify, LRESULT *pResult)
{
	*pResult = NULL;

	if ( ( ((NM_TREEVIEW*) pNotify )->itemNew.state & TVIS_SELECTED ) == 0 ) return;
	CSettingsPage* pPage = (CSettingsPage*)m_wndTree.GetItemData( m_wndTree.GetSelectedItem() );
	if ( pPage == NULL || pPage == m_pPage ) return;

	SetActivePage( pPage );
}

void CSettingsSheet::OnPaint()
{
	CPaintDC dc( this );
	DoPaint( dc );
}

void CSettingsSheet::DoPaint(CDC& dc)
{
	CRect rc( m_nLeftMargin, m_nTopMargin - 1, 0, 0 );

	rc.left		+= m_nListWidth;
	rc.right	= rc.left + m_nListMargin;
	rc.bottom	= rc.top  + m_szPages.cy + 1;

	dc.FillSolidRect( rc.left, rc.top, 1, rc.Height(), Colors.m_crResizebarEdge );
	dc.FillSolidRect( rc.left + 1, rc.top, 1, rc.Height(), Colors.m_crResizebarHighlight );
	dc.FillSolidRect( rc.right - 1, rc.top, 1, rc.Height(), Colors.m_crResizebarShadow );
	dc.FillSolidRect( rc.left + 2, rc.top, rc.Width() - 3, rc.Height(), Colors.m_crResizebarFace );

	GetClientRect( &rc );
	rc.top = rc.bottom - ( m_nButtonHeight + 16 );

	dc.FillSolidRect( rc.left, rc.top, rc.Width(), 1, Colors.m_crSysBtnFace );
	dc.FillSolidRect( rc.left, rc.top + 1, rc.Width(), 1, Colors.m_crSys3DHighlight );
}

void CSettingsSheet::OnOK()
{
	if ( m_pPage && ! m_pPage->OnKillActive() ) return;

	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CSettingsPage* pPage = GetPage( nPage );
		if ( pPage->m_hWnd ) pPage->OnOK();
	}

	EndDialog( IDOK );
}

void CSettingsSheet::OnCancel()
{
	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CSettingsPage* pPage = GetPage( nPage );
		if ( pPage->m_hWnd ) pPage->OnCancel();
	}

	EndDialog( IDCANCEL );
}

void CSettingsSheet::OnApply()
{
	if ( m_pPage && ! m_pPage->OnKillActive() ) return;

	for ( int nPage = 0 ; nPage < GetPageCount() ; nPage++ )
	{
		CSettingsPage* pPage = GetPage( nPage );
		if ( pPage->m_hWnd && ! pPage->OnApply() ) return;
	}

	SetModified( FALSE );
}

BOOL CSettingsSheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if ( LOWORD( wParam ) == IDOK )
	{
		OnOK();
		return TRUE;
	}
	else if ( LOWORD( wParam ) == IDCANCEL )
	{
		OnCancel();
		return TRUE;
	}

	return CSkinDialog::OnCommand(wParam, lParam);
}
