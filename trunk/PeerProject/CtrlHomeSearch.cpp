//
// CtrlHomeSearch.cpp
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
#include "Schema.h"
#include "SchemaCache.h"
#include "QuerySearch.h"
#include "WndSearch.h"
#include "Colors.h"
#include "CoolInterface.h"
#include "CtrlHomeSearch.h"
#include "DlgNewSearch.h"
#include "Skin.h"
#include "DlgHelp.h"
#include "Security.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CHomeSearchCtrl, CWnd)

BEGIN_MESSAGE_MAP(CHomeSearchCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_CBN_CLOSEUP(IDC_SEARCH_TEXT, OnCloseUpText)
	ON_CBN_SELCHANGE(IDC_SEARCH_TEXT, OnSelChangeText)
	ON_COMMAND(IDC_SEARCH_START, OnSearchStart)
	ON_COMMAND(IDC_SEARCH_ADVANCED, OnSearchAdvanced)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHomeSearchCtrl construction

CHomeSearchCtrl::CHomeSearchCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHomeSearchCtrl message handlers

BOOL CHomeSearchCtrl::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		PostMessage( WM_COMMAND, MAKELONG( IDC_SEARCH_START, BN_CLICKED ), (LPARAM)m_wndSearch.GetSafeHwnd() );
		return TRUE;
	}
	return CWnd::PreTranslateMessage( pMsg );
}

BOOL CHomeSearchCtrl::Create(CWnd* pParentWnd, UINT nID)
{
	CRect rect( 0, 0, 0, 0 );
	return CWnd::CreateEx( WS_EX_CONTROLPARENT, NULL, _T("CHomeSearchCtrl"),
		WS_CHILD|WS_CLIPCHILDREN, rect, pParentWnd, nID );
}

int CHomeSearchCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	CRect rc( 0, 0, 0, 0 );

	if ( ! m_wndText.Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_GROUP|WS_VSCROLL|CBS_AUTOHSCROLL|CBS_DROPDOWN,
		rc, this, IDC_SEARCH_TEXT ) ) return -1;

	m_wndText.SetFont( &theApp.m_gdiFont );

	if ( ! m_wndSchema.Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP, rc, this, IDC_SCHEMAS ) )
		return -1;

	m_wndSchema.SetDroppedWidth( 200 );
	LoadString( m_wndSchema.m_sNoSchemaText, IDS_SEARCH_PANEL_AFT );
	m_wndSchema.Load( Settings.Search.LastSchemaURI );

	m_wndSearch.Create( rc, this, IDC_SEARCH_START, WS_TABSTOP | BS_DEFPUSHBUTTON );
	m_wndSearch.SetHandCursor( TRUE );

	m_wndAdvanced.Create( rc, this, IDC_SEARCH_ADVANCED, WS_TABSTOP );
	m_wndAdvanced.SetHandCursor( TRUE );

	OnSkinChange( Colors.m_crWindow );

	return 0;
}

void CHomeSearchCtrl::OnSkinChange(COLORREF crWindow)
{
	CString strCaption;

	m_crWindow = crWindow;

	LoadString( strCaption, IDS_SEARCH_PANEL_START );
	m_wndSearch.SetWindowText( strCaption );
	m_wndSearch.SetCoolIcon( ID_SEARCH_SEARCH, FALSE );

	LoadString( strCaption, IDS_SEARCH_PANEL_ADVANCED );
	m_wndAdvanced.SetWindowText( strCaption + _T('\x2026') );
	m_wndAdvanced.SetCoolIcon( ID_SEARCH_DETAILS, FALSE );

	LoadString( m_wndSchema.m_sNoSchemaText, IDS_SEARCH_PANEL_AFT );

	FillHistory();
}

void CHomeSearchCtrl::FillHistory()
{
	m_wndText.ResetContent();

	// Load all
	for ( int i = 0; ; i++ )
	{
		CString strEntry;
		strEntry.Format( _T("Search.%.2i"), i + 1 );
		CString strValue( theApp.GetProfileString( _T("Search"), strEntry ) );
		if ( strValue.IsEmpty() )
			break;
		int lf = strValue.Find( _T('\n') );
		int nIndex = m_wndText.InsertString( i,
			( lf != -1 ) ? strValue.Left( lf ) : strValue );
		CSchema* pSchema = ( lf != -1 ) ? SchemaCache.Get( strValue.Mid( lf + 1 ) ) : NULL;
		m_wndText.SetItemData( nIndex, (DWORD_PTR)pSchema );
	}

	CString strClear;
	LoadString( strClear, IDS_SEARCH_PAD_CLEAR_HISTORY );
	m_wndText.SetItemData( m_wndText.AddString( strClear ), 0 );
}

void CHomeSearchCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize( nType, cx, cy );

	CRect rcClient( 0, 0, cx, cy );
	CRect rcItem;

	rcClient.DeflateRect( 1, 1 );

	rcClient.top += 18;
	rcItem.SetRect( rcClient.left, rcClient.top, rcClient.right - 104, rcClient.top + 256 );
	m_wndText.MoveWindow( &rcItem );

	rcItem.SetRect( rcClient.right - 92, rcClient.top - 2, rcClient.right, rcClient.top + 22 );
	m_wndSearch.MoveWindow( &rcItem );

	rcClient.top += 32;

	rcItem.SetRect( rcClient.right - 104 - 160, rcClient.top, rcClient.right - 104, rcClient.top + 256 );
	rcItem.left = max( rcItem.left, rcClient.left );
	m_wndSchema.MoveWindow( &rcItem );

	rcItem.SetRect( rcClient.right - 92, rcClient.top, rcClient.right, rcClient.top + 24 );
	m_wndAdvanced.MoveWindow( &rcItem );
}

void CHomeSearchCtrl::OnPaint()
{
	CRect rcClient, rcItem;
	CPaintDC dc( this );
	CString str;

	GetClientRect( &rcClient );
	rcClient.DeflateRect( 1, 1 );

	CFont* pOldFont = (CFont*)dc.SelectObject( &CoolInterface.m_fntBold );
	dc.SetBkMode( OPAQUE );
	dc.SetBkColor( m_crWindow );
	dc.SetTextColor( 0 );

	LoadString( str, IDS_SEARCH_PAD_WORDS );

	rcItem.SetRect( rcClient.left, rcClient.top, rcClient.right, rcClient.top + 16 );
	dc.ExtTextOut( rcItem.left + 2, rcItem.top + 2, ETO_CLIPPED|ETO_OPAQUE, &rcItem, str, NULL );
	dc.ExcludeClipRect( &rcItem );

	rcClient.top += 18;
	rcClient.top += 32;

	rcItem.SetRect( rcClient.left, rcClient.top,
		rcClient.right - 104 - 160 - 8, rcClient.top + 22 );

	LoadString( str, IDS_SEARCH_PAD_TYPE );
	CSize sz = dc.GetTextExtent( str );
	dc.ExtTextOut( rcItem.right - sz.cx, ( rcItem.top + rcItem.bottom ) / 2 - sz.cy / 2,
		ETO_CLIPPED|ETO_OPAQUE, &rcItem, str, NULL );
	dc.ExcludeClipRect( &rcItem );

	dc.SelectObject( pOldFont );
	GetClientRect( &rcClient );
	dc.FillSolidRect( &rcClient, m_crWindow );
}

void CHomeSearchCtrl::OnCloseUpText()
{
	int nSel = m_wndText.GetCurSel();
	if ( nSel < 0 ) return;

	if ( nSel == m_wndText.GetCount() - 1 )
	{
		m_wndText.SetWindowText( _T("") );

		// Delete all
		for ( int i = 0; ; i++ )
		{
			CString strEntry;
			strEntry.Format( _T("Search.%.2i"), i + 1 );
			CString strValue( theApp.GetProfileString( _T("Search"), strEntry ) );
			if ( strValue.IsEmpty() )
				break;
			theApp.WriteProfileString( _T("Search"), strEntry, NULL );
		}

		m_wndSchema.Select( (CSchema*)NULL );
		FillHistory();
	}
	else
	{
		m_wndSchema.Select( (CSchema*)m_wndText.GetItemData( nSel ) );
	}
}

void CHomeSearchCtrl::OnSelChangeText()
{
	OnCloseUpText();
}

void CHomeSearchCtrl::Search(bool bAutostart)
{
	CString strText, strURI, strEntry, strClear;

	m_wndText.GetWindowText( strText );
	strText.TrimLeft();
	strText.TrimRight();

	LoadString( strClear, IDS_SEARCH_PAD_CLEAR_HISTORY );
	if ( _tcscmp ( strClear , strText ) == 0 ) return;

	// Check if user mistakenly pasted download link to search input box
	if ( CPeerProjectApp::OpenURL( strText, TRUE, TRUE ) )
	{
		m_wndText.SetWindowText( _T("") );
		return;
	}

	CSchema* pSchema = m_wndSchema.GetSelected();
	if ( pSchema != NULL ) strURI = pSchema->GetURI();

	Settings.Search.LastSchemaURI = strURI;

	auto_ptr< CQuerySearch > pSearch( new CQuerySearch() );
	if ( pSearch.get() )
	{
		pSearch->m_bAutostart	= bAutostart;
		pSearch->m_sSearch		= strText;
		pSearch->m_pSchema		= pSchema;
		BOOL bValid = pSearch->CheckValid( false );
		if ( ! bValid && bAutostart )
		{
			// Invalid search, open help window
			CQuerySearch::SearchHelp();
		}
		else if ( AdultFilter.IsSearchFiltered( pSearch->m_sSearch ) && bAutostart )
		{
			// Adult search blocked, open help window
			CHelpDlg::Show( _T("SearchHelp.AdultSearch") );
		}
		else
		{
			if ( bValid )
			{
				// Load all
				CStringList oList;
				for ( int i = 0; ; i++ )
					{
					strEntry.Format( _T("Search.%.2i"), i + 1 );
					CString strValue( theApp.GetProfileString( _T("Search"), strEntry ) );
					if ( strValue.IsEmpty() )
						break;
					int lf = strValue.Find( _T('\n') );
					if ( strText.CompareNoCase( ( lf != -1 ) ? strValue.Left( lf ) : strValue ) )
						oList.AddTail( strValue );
					}

				// Cut to 200 items
				while ( oList.GetCount() >= 200 )
					oList.RemoveTail();

				// New one (at top)
				oList.AddHead( strURI.IsEmpty() ? strText : ( strText + _T('\n') + strURI ) );

				// Save list
				POSITION pos = oList.GetHeadPosition();
				for ( int i = 0; pos; ++i )
				{
					strEntry.Format( _T("Search.%.2i"), i + 1 );
					theApp.WriteProfileString( _T("Search"), strEntry, oList.GetNext( pos ) );
				}

				FillHistory();
			}

			new CSearchWnd( pSearch );
		}
	}

	m_wndText.SetWindowText( _T("") );
}

void CHomeSearchCtrl::OnSearchStart()
{
	Search( true );
}

void CHomeSearchCtrl::OnSearchAdvanced()
{
//	Search( false );
	GetTopLevelParent()->PostMessage( WM_COMMAND, ID_TAB_SEARCH );
}

void CHomeSearchCtrl::Activate()
{
	FillHistory();
	m_wndText.SetFocus();
}
