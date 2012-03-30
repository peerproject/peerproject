//
// WndChat.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "WndChat.h"
#include "WndMain.h"
#include "ChatWindows.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Emoticons.h"
#include "EDClients.h"
#include "EDPacket.h"
#include "Plugins.h"
#include "Security.h"
#include "GProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CChatWnd, CChildWnd)	//CPanelWnd

BEGIN_MESSAGE_MAP(CChatWnd, CChildWnd)	//CPanelWnd
	//{{AFX_MSG_MAP(CChatWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_SETFOCUS()
//	ON_WM_SETCURSOR()
	ON_WM_CONTEXTMENU()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_CHAT_BOLD, &CChatWnd::OnUpdateChatBold)
	ON_COMMAND(ID_CHAT_BOLD, &CChatWnd::OnChatBold)
	ON_UPDATE_COMMAND_UI(ID_CHAT_ITALIC, &CChatWnd::OnUpdateChatItalic)
	ON_COMMAND(ID_CHAT_ITALIC, &CChatWnd::OnChatItalic)
	ON_UPDATE_COMMAND_UI(ID_CHAT_UNDERLINE, &CChatWnd::OnUpdateChatUnderline)
	ON_COMMAND(ID_CHAT_UNDERLINE, &CChatWnd::OnChatUnderline)
	ON_COMMAND(ID_CHAT_COLOR, &CChatWnd::OnChatColor)
	ON_COMMAND(ID_CHAT_CLEAR, &CChatWnd::OnChatClear)
	ON_COMMAND(ID_CHAT_EMOTICONS, &CChatWnd::OnChatEmoticons)
	ON_UPDATE_COMMAND_UI(ID_CHAT_TIMESTAMP, &CChatWnd::OnUpdateChatTimestamp)
	ON_COMMAND(ID_CHAT_TIMESTAMP, &CChatWnd::OnChatTimestamp)
//	ON_MESSAGE(WM_CHAT_MESSAGE, &CChatWnd::OnChatMessage)
//	ON_MESSAGE(WM_CHAT_ADD_USER, &CChatWnd::OnChatAddUser)
//	ON_MESSAGE(WM_CHAT_DELETE_USER, &CChatWnd::OnChatDeleteUser)
	ON_NOTIFY(RVN_CLICK, IDC_CHAT_TEXT, &CChatWnd::OnClickView)
//	ON_COMMAND_RANGE(1, 200, &CChatWnd::OnEmoticons)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define NEWLINE_FORMAT	_T("2")
#define EDIT_HISTORY	256
#define EDIT_HEIGHT		32
//#define TOOLBAR_HEIGHT 30	// Skin.m_nToolbarHeight
//#define SPLIT_SIZE	6	// Skin.m_nSplitter

/////////////////////////////////////////////////////////////////////////////
// CChatWnd construction

CChatWnd::CChatWnd()
	: m_nHistory	( 0 )
//	, m_nUsersSize	( Settings.Community.UserPanelSize )
{
//	Create( IDR_CHATFRAME, FALSE );
}

CChatWnd::~CChatWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CChatWnd desktop mode (ToDo:?)

//void CChatFrame::SetDesktopMode(BOOL bDesktop)
//{
//	if ( bDesktop && m_pDesktopWnd != NULL ) return;
//	if ( ! bDesktop && m_pChildWnd != NULL ) return;
//
//	ShowWindow( SW_HIDE );
//	SetParent( NULL );
//
//	if ( m_pDesktopWnd != NULL )
//	{
//		// m_pDesktopWnd->m_pFrame = NULL;
//		m_pDesktopWnd->DestroyWindow();
//		m_pDesktopWnd = NULL;
//	}
//
//	if ( m_pChildWnd != NULL )
//	{
//		m_pChildWnd->m_pFrame = NULL;
//		m_pChildWnd->DestroyWindow();
//		m_pChildWnd = NULL;
//	}
//
//	if ( ! bDesktop )
//		m_pChildWnd = new CChatWnd( this );
//	//else
//		// ToDo: m_pDesktopWnd = new CChatDesktopWnd( this );
//}

/////////////////////////////////////////////////////////////////////////////
// CChatWnd text input controller

BOOL CChatWnd::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		switch ( pMsg->wParam )
		{
		case VK_RETURN:
			{
				CString strCurrent;
				m_wndEdit.GetWindowText( strCurrent );

				for ( ; strCurrent.GetLength() ; strCurrent = strCurrent.Mid( 1 ) )
				{
					CString strLine = strCurrent.SpanExcluding( _T("\r\n") );
					if ( strLine.GetLength() )
					{
						if ( ! OnLocalText( strLine ) )
							break;	// Leave rest of string in edit box
						strCurrent = strCurrent.Mid( strLine.GetLength() );
					}
				}

				m_wndEdit.SetWindowText( strCurrent );
				return TRUE;
			}
		case VK_ESCAPE:
			m_wndEdit.SetWindowText( _T("") );
			m_nHistory = static_cast< int >( m_pHistory.GetSize() );
			return TRUE;
		case VK_UP:
			MoveHistory( -1 );
			return TRUE;
		case VK_DOWN:
			MoveHistory( 1 );
			return TRUE;
		case VK_PRIOR:
			m_wndView.PostMessage( WM_VSCROLL, MAKELONG( SB_PAGEUP, 0 ), NULL );
			return TRUE;
		case VK_NEXT:
			m_wndView.PostMessage( WM_VSCROLL, MAKELONG( SB_PAGEDOWN, 0 ), NULL );
			return TRUE;
		case VK_HOME:
			if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
				m_wndView.PostMessage( WM_VSCROLL, MAKELONG( SB_TOP, 0 ), NULL );
				return TRUE;
			}
			break;
		case VK_END:
			if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
				m_wndView.PostMessage( WM_VSCROLL, MAKELONG( SB_BOTTOM, 0 ), NULL );
				return TRUE;
			}
			break;
		case 'B':
			if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
				OnChatBold();
				return TRUE;
			}
			break;
		case 'I':
			if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
				OnChatItalic();
				return TRUE;
			}
			break;
		case 'U':
			if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
				OnChatUnderline();
				return TRUE;
			}
			break;
		case 'O':
		case 'K':
			if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
				OnChatColor();
				return TRUE;
			}
			break;
		case 'E':
			if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
				OnChatEmoticons();
				return TRUE;
			}
			break;
		}
	}

//	return CPanelWnd::PreTranslateMessage( pMsg );
	return CChildWnd::PreTranslateMessage( pMsg );
}

void CChatWnd::MoveHistory(int nDelta)
{
	CString strCurrent;
	if ( m_nHistory == m_pHistory.GetSize() )
		m_wndEdit.GetWindowText( strCurrent );

	m_nHistory += nDelta;
	m_nHistory = max( 0, min( (int)m_pHistory.GetSize(), m_nHistory ) );

	if ( m_nHistory == m_pHistory.GetSize() )
		m_wndEdit.SetWindowText( strCurrent );
	else
		m_wndEdit.SetWindowText( m_pHistory.GetAt( m_nHistory ) );

	int nLen = m_wndEdit.GetWindowTextLength();
	m_wndEdit.SetSel( nLen, nLen );
}

BOOL CChatWnd::IsInRange(LPCTSTR pszToken)
{
	CString strRange, strToken;
	int nStart, nEnd;

	m_wndEdit.GetSel( nStart, nEnd );
	if ( nStart != nEnd ) return FALSE;

	m_wndEdit.GetWindowText( strRange );
	if ( nStart <= 0 ) return FALSE;
	if ( nStart < strRange.GetLength() )
		strRange = strRange.Left( nStart );

	ToLower( strRange );
	strRange.MakeReverse();
	strToken.Format( _T("]%s["), pszToken );
	nStart = strRange.Find( strToken );
	strToken.Format( _T("]%s/["), pszToken );
	nEnd = strRange.Find( strToken );

	if ( nStart < 0 ) return FALSE;
	if ( nEnd < 0 ) return TRUE;

	return ( nEnd > nStart );
}

void CChatWnd::InsertText(LPCTSTR pszToken)
{
	int nStart, nEnd;
	m_wndEdit.GetSel( nStart, nEnd );

	if ( nStart == nEnd )
	{
		m_wndEdit.ReplaceSel( pszToken );
	}
	else
	{
		CString strIn, strOut;
		m_wndEdit.GetWindowText( strIn );
		if ( nEnd < nStart )
			m_wndEdit.GetSel( nEnd, nStart );
		strOut.Format( _T("%s%s[/%c]"), pszToken,
			(LPCTSTR)strIn.Mid( nStart, nEnd - nStart ), pszToken[1] );
		m_wndEdit.ReplaceSel( strOut );
	}

	m_wndEdit.SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CChatWnd text view controller

void CChatWnd::AddTimestamp()
{
	if ( ! Settings.Community.Timestamp )
		return;

	CTime tNow = CTime::GetCurrentTime();

	CString str;
	str.Format( _T("[%.2i:%.2i:%.2i] "), tNow.GetHour(), tNow.GetMinute(), tNow.GetSecond() );
	m_pContent.Add( retText, str, NULL, retfColor )->m_cColor = Colors.m_crChatNull;
}

void CChatWnd::AddLogin(LPCTSTR pszText)
{
	AddTimestamp();

	m_pContent.Add( retText, LoadString( IDS_CHAT_PROFILE_ACCEPTED ), NULL, retfColor )->m_cColor = Colors.m_crChatNull;
	m_pContent.Add( retLink, pszText, _T("command:ID_CHAT_BROWSE") );
	m_pContent.Add( retNewline, NEWLINE_FORMAT );
	m_wndView.InvalidateIfModified();
}

void CChatWnd::AddBitmap(HBITMAP hBitmap)
{
	m_pContent.Add( hBitmap );
	m_pContent.Add( retNewline, NEWLINE_FORMAT );
	m_wndView.InvalidateIfModified();
}

void CChatWnd::AddText(LPCTSTR pszText)
{
	AddTimestamp();

	m_pContent.Add( retText, pszText );
	m_pContent.Add( retNewline, NEWLINE_FORMAT );
	m_wndView.InvalidateIfModified();
}

void CChatWnd::AddText(bool bAction, bool bOutgoing, LPCTSTR pszNick, LPCTSTR pszBody)
{
	AddTimestamp();

	CString str;
	str.Format( bAction ? _T("* %s ") : _T("<%s> "), pszNick );
	m_pContent.Add( retText, str, NULL, retfBold | retfColor )->m_cColor
		= ( bOutgoing ? Colors.m_crChatIn : Colors.m_crChatOut );

	Emoticons.FormatText( &m_pContent, pszBody );

	m_pContent.Add( retNewline, NEWLINE_FORMAT );
	m_wndView.InvalidateIfModified();
}

void CChatWnd::Open()
{
	if ( IsIconic() ) ShowWindow( SW_SHOWNORMAL );
	BringWindowToTop();
	SetForegroundWindow();
}

void CChatWnd::OnSkinChange()
{
//	CPanelWnd::OnSkinChange();
	CChildWnd::OnSkinChange();

	Skin.CreateToolBar( CString( GetRuntimeClass()->m_lpszClassName ), &m_wndToolBar );
}

/////////////////////////////////////////////////////////////////////////////
// CChatWnd event handlers

void CChatWnd::OnStatusMessage(int nFlags, const CString& sText)
{
	if ( sText.IsEmpty() )
		return;

	AddTimestamp();

	m_pContent.Add( retText, sText, NULL, retfColor )->m_cColor
		= ( nFlags == 1 ) ? Colors.m_crChatOut : Colors.m_crChatNull;
	m_pContent.Add( retNewline, NEWLINE_FORMAT );
	m_wndView.InvalidateIfModified();
}

BOOL CChatWnd::OnLocalText(const CString& sText)
{
	// Save history
	m_pHistory.Add( sText );
	if ( m_pHistory.GetSize() > EDIT_HISTORY ) m_pHistory.RemoveAt( 0 );
	m_nHistory = static_cast< int >( m_pHistory.GetSize() );

	if ( sText.GetAt( 0 ) == _T('/') )
	{
		CString strCommand = sText.SpanExcluding( _T(" \t") ).Trim();
		if ( strCommand.CompareNoCase( _T("/me") ) == 0 )
			return OnLocalMessage( true, sText.Mid( 4 ) );	// Action text

		if ( OnLocalCommand( strCommand, sText.Mid( strCommand.GetLength() + 1 ).Trim() ) )
			return TRUE;	// Handled command 
	}
	else if ( sText.GetAt( 0 ) == _T('*') || sText.GetAt( 0 ) == _T('•') )
	{
		if ( sText.GetAt( 1 ) == _T(' ') || sText.GetAt( 1 ) == _T('\t') )
			return OnLocalMessage( true, sText.Mid( 2 ) );	// Action text
	}

	// Regular text
	return OnLocalMessage( false, sText );
}

BOOL CChatWnd::OnLocalCommand(const CString& sCommand, const CString& /*sArgs*/)
{
	if ( sCommand.CompareNoCase( _T("/clear") ) == 0 )
	{
		PostMessage( WM_COMMAND, ID_CHAT_CLEAR );
		return TRUE;
	}

	if ( sCommand.CompareNoCase( _T("/close") ) == 0 ||
		 sCommand.CompareNoCase( _T("/exit")  ) == 0 ||
		 sCommand.CompareNoCase( _T("/quit")  ) == 0 )
	{
		PostMessage( WM_CLOSE );
		return TRUE;
	}

	if ( sCommand.CompareNoCase( _T("/disconnect") ) == 0 )
	{
		PostMessage( WM_COMMAND, ID_CHAT_DISCONNECT );
		return TRUE;
	}

	// Not a command
	return FALSE;
}

void CChatWnd::OnMessage(bool bAction, const CString& sChatID, bool bOutgoing, const CString& sFrom, const CString& sTo, const CString& sText)
{
	// Check incoming message spam filter (if enabled)
	if ( ! bOutgoing && MessageFilter.IsFiltered( sText ) )
		return;

	// Notify chat plugins about new message
	Plugins.OnChatMessage( sChatID, bOutgoing, sFrom, sTo, sText );

	// Adult filter (if enabled)
	CString sCensoredText( sText );
	if ( Settings.Community.ChatCensor )
		AdultFilter.Censor( sCensoredText );

	AddText( bAction, bOutgoing, sFrom, sCensoredText );

	SetAlert();

	if ( CMainWnd* pWnd = theApp.CPeerProjectApp::SafeMainWnd() )
	{
		if ( GetForegroundWindow() != pWnd )
		{
			// pWnd->ShowTrayPopup( sText, sFrom, NIIF_NONE, 30 );

			FLASHWINFO pFWX =
			{
				sizeof( pFWX ),
				pWnd->GetSafeHwnd(),
				FLASHW_ALL | FLASHW_TIMERNOFG,
				1000
			};
			::FlashWindowEx( &pFWX );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChatWnd message handlers

int CChatWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

//	ASSERT_VALID( m_pFrame );
//
//	m_pFrame->SetParent( this );
//
//	LoadState( _T("CChatWnd"), FALSE );

	CRect rc;
	GetClientRect( &rc );
//	m_pFrame->MoveWindow( &rc, TRUE );

	m_wndView.Create( WS_CHILD|WS_VISIBLE, rc, this, IDC_CHAT_TEXT );
	m_wndView.SetDocument( &m_pContent );
	m_wndView.SetSelectable( TRUE );
	m_wndView.SetFollowBottom( TRUE );

	m_pContent.m_szMargin = CSize( 8, 4 );

	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM );

	if ( ! m_wndEdit.Create( WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL, rc, this, IDC_CHAT_EDIT ) ) return -1;
	m_wndEdit.SetFont( &theApp.m_gdiFont );

	ChatWindows.Add( this );

	LoadState( _T("CChatWnd"), FALSE );

//	SetAlert();

	return 0;
}

void CChatWnd::OnDestroy()
{
	SaveState( _T("CChatWnd") );

	ChatWindows.Remove( this );

//	if ( m_pFrame != NULL )
//	{
//		m_pFrame->DestroyWindow();
//		delete m_pFrame;
//	}

	CChildWnd::OnDestroy();
}
void CChatWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	Skin.TrackPopupMenu( CString( GetRuntimeClass()->m_lpszClassName ), point );
}

BOOL CChatWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
//	return CPanelWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
	return CChildWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CChatWnd::OnSize(UINT nType, int cx, int cy)
{
//	CPanelWnd::OnSize( nType, cx, cy );
	CChildWnd::OnSize( nType, cx, cy );
//	if ( m_pFrame )
//		m_pFrame->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER|SWP_SHOWWINDOW );

	CRect rc;
	GetClientRect( &rc );

	HDWP hDWP = BeginDeferWindowPos( 3 );

	DeferWindowPos( hDWP, m_wndView, NULL, rc.left, rc.top,
		rc.Width(), rc.Height() - Skin.m_nToolbarHeight - EDIT_HEIGHT, SWP_NOZORDER );

	DeferWindowPos( hDWP, m_wndToolBar, NULL,
		rc.left, rc.bottom - Skin.m_nToolbarHeight - EDIT_HEIGHT,
		rc.Width(), Skin.m_nToolbarHeight, SWP_NOZORDER );

	DeferWindowPos( hDWP, m_wndEdit, NULL, rc.left, rc.bottom - EDIT_HEIGHT,
		rc.Width(), EDIT_HEIGHT, SWP_NOZORDER );

	EndDeferWindowPos( hDWP );
}

//void CChatWnd::OnPaint()
//{
//	CPaintDC dc( this );
//
//	CRect rcClient;
//	GetClientRect( &rcClient );
//
//	CRect rc( rcClient.right - m_nUsersSize - Skin.m_nSplitter,
//			  rcClient.top,
//			  rcClient.right - m_nUsersSize,
//			  rcClient.bottom - Skin.m_nToolbarHeight - EDIT_HEIGHT );
//
//	dc.FillSolidRect( rc.left, rc.top, 1, rc.Height(), Colors.m_crResizebarEdge );
//	dc.FillSolidRect( rc.left + 1, rc.top, 1, rc.Height(), Colors.m_crResizebarHighlight );
//	dc.FillSolidRect( rc.right - 1, rc.top, 1, rc.Height(), Colors.m_crResizebarShadow );
//	dc.FillSolidRect( rc.left + 2, rc.top, rc.Width() - 3, rc.Height(),	Colors.m_crResizebarFace );
//}

void CChatWnd::OnUpdateChatBold(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( IsInRange( _T("b") ) );
}

void CChatWnd::OnChatBold()
{
	if ( IsInRange( _T("b") ) )
		InsertText( _T("[/b]") );
	else
		InsertText( _T("[b]") );
}

void CChatWnd::OnUpdateChatItalic(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( IsInRange( _T("i") ) );
}

void CChatWnd::OnChatItalic()
{
	if ( IsInRange( _T("i") ) )
		InsertText( _T("[/i]") );
	else
		InsertText( _T("[i]") );
}

void CChatWnd::OnUpdateChatUnderline(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( IsInRange( _T("u") ) );
}

void CChatWnd::OnChatUnderline()
{
	if ( IsInRange( _T("u") ) )
		InsertText( _T("[/u]") );
	else
		InsertText( _T("[u]") );
}

void CChatWnd::OnChatColor()
{
	CColorDialog dlg( 0, CC_ANYCOLOR | CC_FULLOPEN );
	if ( dlg.DoModal() != IDOK ) return;

	COLORREF cr = dlg.GetColor();

	CString str;
	str.Format( _T("[c:#%.2x%.2x%.2x]"), GetRValue( cr ), GetGValue( cr ), GetBValue( cr ) );
	InsertText( str );
}

void CChatWnd::OnChatEmoticons()
{
	CAutoPtr< CMenu > pIconMenu( Emoticons.CreateMenu() );
	if ( UINT nID = m_wndToolBar.ThrowMenu( ID_CHAT_EMOTICONS, pIconMenu, this, TRUE ) )
	{
		if ( LPCTSTR pszToken = Emoticons.GetText( nID - 1 ) )
			InsertText( _T(" ") + (CString)pszToken + _T(" ") );
	}
}

void CChatWnd::OnChatClear()
{
	m_pContent.Clear();
	m_wndView.InvalidateIfModified();
}

void CChatWnd::OnUpdateChatTimestamp(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( Settings.Community.Timestamp );
}

void CChatWnd::OnChatTimestamp()
{
	Settings.Community.Timestamp = ! Settings.Community.Timestamp;
}

/////////////////////////////////////////////////////////////////////////////
// CChatWnd message handlers

void CChatWnd::OnSetFocus(CWnd* pOldWnd)
{
//	CPanelWnd::OnSetFocus( pOldWnd );
	CChildWnd::OnSetFocus( pOldWnd );

	m_wndEdit.SetFocus();
}

void CChatWnd::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemWidth	= 20;
	lpMeasureItemStruct->itemHeight	= 22;
}

void CChatWnd::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect rc( &lpDrawItemStruct->rcItem );

	Emoticons.Draw( pDC, lpDrawItemStruct->itemID - 1, rc.left + 8, rc.top + 3 );

	if ( lpDrawItemStruct->itemState & ODS_SELECTED )
	{
		pDC->Draw3dRect( &rc, Colors.m_crHighlight, Colors.m_crHighlight );
		rc.DeflateRect( 1, 1 );
		pDC->Draw3dRect( &rc, Colors.m_crHighlight, Colors.m_crHighlight );
	}
	else
	{
		pDC->Draw3dRect( &rc, Colors.m_crWindow, Colors.m_crWindow );
		rc.DeflateRect( 1, 1 );
		pDC->Draw3dRect( &rc, Colors.m_crWindow, Colors.m_crWindow );
	}
}

void CChatWnd::OnClickView(NMHDR* pNotify, LRESULT* /*pResult*/)
{
	if ( CRichElement* pElement = ((RVN_ELEMENTEVENT*)pNotify)->pElement )
	{
		theApp.InternalURI( pElement->m_sLink );
	}
}

void CChatWnd::OnEmoticons(UINT /*nID*/)
{
	// Used to enable emoticons menu items
}

// Legacy, ToDo:?
//void CChatWnd::OnTimer(UINT_PTR nIDEvent)
//{
//	if ( nIDEvent == 1 )
//	{
//		if ( m_pChildWnd != NULL ) m_pChildWnd->SetAlert();	// PostMessage( WM_TIMER, 1 );
//		//if ( m_pDesktopWnd != NULL ) m_pDesktopWnd->SetAlert();	// PostMessage( WM_TIMER, 1 );
//	}
//	else if ( nIDEvent == 4 )
//	{
//		if ( GetForegroundWindow() != GetTopLevelParent() )
//		{
//			CWnd* pParentWnd = GetTopLevelParent();
//
//			FLASHWINFO pFWX;
//			pFWX.cbSize		= sizeof(pFWX);
//			pFWX.dwFlags	= FLASHW_ALL | FLASHW_TIMERNOFG;
//			pFWX.uCount		= 3;
//			pFWX.dwTimeout	= 0;
//			pFWX.hwnd		= pParentWnd->GetSafeHwnd();
//
//			::FlashWindowEx( &pFWX );
//		}
//	}
//}

/////////////////////////////////////////////////////////////////////////////
// CChatWnd Users List

//BOOL CChatWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
//{
//	CPoint point;
//	GetCursorPos( &point );
//
//	CRect rcClient;
//	GetClientRect( &rcClient );
//	ClientToScreen( &rcClient );
//
//	CRect rc( rcClient.right - m_nUsersSize - Skin.m_nSplitter,
//		rcClient.top,
//		rcClient.right - m_nUsersSize,
//		rcClient.bottom - Skin.m_nToolbarHeight - EDIT_HEIGHT );
//
//	if ( rc.PtInRect( point ) )
//	{
//		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEWE ) );
//		return TRUE;
//	}
//
//	return CPanelWnd::OnSetCursor( pWnd, nHitTest, message );
//}

//void CChatWnd::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	CRect rcClient;
//	GetClientRect( &rcClient );
//
//	CRect rc( rcClient.right - m_nUsersSize - Skin.m_nSplitter,
//		rcClient.top,
//		rcClient.right - m_nUsersSize,
//		rcClient.bottom - Skin.m_nToolbarHeight - EDIT_HEIGHT );
//
//	if ( rc.PtInRect( point ) )
//	{
//		DoSizeView();
//		return;
//	}
//
//	CPanelWnd::OnLButtonDown( nFlags, point );
//}

//BOOL CChatWnd::DoSizeView()
//{
//	MSG* pMsg = &AfxGetThreadState()->m_msgCur;
//
//	CRect rcClient;
//	GetClientRect( &rcClient );
//	ClientToScreen( &rcClient );
//	ClipCursor( &rcClient );
//	SetCapture();
//
//	GetClientRect( &rcClient );
//
//	int nOffset = 0xFFFF;
//
//	while ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 )
//	{
//		while ( ::PeekMessage( pMsg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) );
//
//		if ( ! AfxGetThread()->PumpMessage() )
//		{
//			AfxPostQuitMessage( 0 );
//			break;
//		}
//
//		CPoint point;
//		GetCursorPos( &point );
//		ScreenToClient( &point );
//
//		int nSplit = rcClient.right - point.x;
//
//		if ( nOffset == 0xFFFF ) nOffset = m_nUsersSize - nSplit;
//		nSplit += nOffset;
//
//		nSplit = max( nSplit, 0 );
//		nSplit = min( nSplit, (int)rcClient.right - Skin.m_nSplitter );
//
//		if ( nSplit < 8 )
//			nSplit = 0;
//		if ( nSplit > rcClient.right - Skin.m_nSplitter - 8 )
//			nSplit = rcClient.right - Skin.m_nSplitter;
//
//		if ( nSplit != m_nUsersSize )
//		{
//			m_nUsersSize = nSplit;
//			OnSize( 1982, 0, 0 );
//			Invalidate();
//		}
//	}
//
//	ReleaseCapture();
//	ClipCursor( NULL );
//
//	return TRUE;
//}

//LRESULT CChatWnd::OnChatMessage(WPARAM /*wParam*/, LPARAM lParam)
//{
//	CAutoPtr< CChatMessage > pMsg( (CChatMessage*)lParam );
//
//	if ( pMsg->m_hBitmap )
//
//		CChatWnd::AddBitmap( pMsg->m_hBitmap );
//
//	switch ( pMsg->m_bType )
//	{
//	case cmtProfile:
//		Open();
//		SetAlert();
//		CChatWnd::AddLogin( pMsg->m_sFrom );
//		break;
//
//	case cmtError:
//		SetAlert();
//	case cmtStatus:
//	case cmtInfo:
//		CChatWnd::OnStatusMessage( (int)pMsg->m_bType - (int)cmtStatus, pMsg->m_sMessage );
//		break;
//
//	case cmtMessage:
//		CChatWnd::OnMessage( false, GetChatID(), false, pMsg->m_sFrom, MyProfile.GetNick(), pMsg->m_sMessage );
//		break;
//
//	case cmtAction:
//		CChatWnd::OnMessage( false, GetChatID(), false, pMsg->m_sFrom, MyProfile.GetNick(), pMsg->m_sMessage );
//		break;
//
//	case cmtCaption:
//		m_sCaption = _T(" : ") + pMsg->m_sMessage;
//		break;
//
//	//default:
//	//	;
//	}
//
//	SetWindowText( GetCaption() + m_sCaption );
//
//	return 0;
//}

//LRESULT CChatWnd::OnChatAddUser(WPARAM /*wParam*/, LPARAM lParam)
//{
//	CAutoPtr< CChatUser > pUser( (CChatUser*)lParam );
//
//	LVFINDINFO lvfi = { LVFI_STRING, pUser->m_sNick };
//	int index = -1;
//	for (;;)
//	{
//		index = m_wndUsers.FindItem( &lvfi, index );
//		if ( index == -1 )
//			break;
//
//		CChatUser* pCurrent = (CChatUser*)m_wndUsers.GetItemData( index );
//		if ( pCurrent->m_bType == pUser->m_bType )
//		{
//			// Update existing user
//			pCurrent->m_sDescription = pUser->m_sDescription;
//
//			// ToDo: Other user properties
//
//			return 0;
//		}
//	}
//
//	// New User
//	int i = m_wndUsers.InsertItem( 0, pUser->m_sNick, pUser->m_bType );
//	m_wndUsers.SetItemData( i, (DWORD_PTR)pUser.Detach() );
//
//	return 0;
//}

//LRESULT CChatWnd::OnChatDeleteUser(WPARAM /*wParam*/, LPARAM lParam)
//{
//	CAutoPtr< CString > psNick( (CString*)lParam );
//
//	if ( ! psNick )
//	{
//		DeleteAllUsers();
//		return 0;
//	}
//
//	LVFINDINFO lvfi = { LVFI_STRING, *psNick };
//	int index = -1;
//	for (;;)
//	{
//		index = m_wndUsers.FindItem( &lvfi, index );
//		if ( index == -1 )
//			break;
//
//		CChatUser* pCurrent = (CChatUser*)m_wndUsers.GetItemData( index );
//		if ( pCurrent->m_bType != cutMe )	// Except me
//		{
//			delete pCurrent;
//			m_wndUsers.DeleteItem( index );
//		}
//	}
//
//	return 0;
//}

//void CChatWnd::DeleteAllUsers()
//{
//	int nCount = m_wndUsers.GetItemCount();
//	for ( int i = 0 ; i < nCount ; ++i )
//	{
//		delete (CChatUser*)m_wndUsers.GetItemData( i );
//	}
//	m_wndUsers.DeleteAllItems();
//}


/////////////////////////////////////////////////////////////////////////////
// CChatWnd Legacy

//void CChatWnd::OnSkinChange()
//{
//	CChildWnd::OnSkinChange();
//	Skin.CreateToolBar( CString( GetRuntimeClass()->m_lpszClassName ), &m_wndToolBar );
//	//if ( m_pFrame != NULL ) m_pFrame->OnSkinChange();
//}

//BOOL CChatWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
//{
//	if ( m_pFrame != NULL )
//	{
//		if ( m_pFrame->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
//			return TRUE;
//	}
//
//	return CPanelWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
//
//	return CChildWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
//}

//void CChatWnd::OnSize(UINT nType, int cx, int cy)
//{
//	CChildWnd::OnSize( nType, cx, cy );
//
//	CRect rc;
//	GetClientRect( &rc );
//
//	HDWP hDWP = BeginDeferWindowPos( 3 );
//
//	DeferWindowPos( hDWP, m_wndView, NULL, rc.left, rc.top, rc.Width(), rc.Height() - Skin.m_nToolbarHeight - EDIT_HEIGHT, SWP_NOZORDER );
//	DeferWindowPos( hDWP, m_wndToolBar, NULL, rc.left, rc.bottom - Skin.m_nToolbarHeight - EDIT_HEIGHT, rc.Width(), Skin.m_nToolbarHeight, SWP_NOZORDER );
//	DeferWindowPos( hDWP, m_wndEdit, NULL, rc.left, rc.bottom - EDIT_HEIGHT, rc.Width(), EDIT_HEIGHT, SWP_NOZORDER );
//	EndDeferWindowPos( hDWP );
//
//	//if ( m_pFrame != NULL )
//	//	m_pFrame->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER|SWP_SHOWWINDOW );
//}

//void CChatWnd::OnTimer(UINT_PTR nIDEvent)
//{
//	if ( nIDEvent == 1 && m_pFrame != NULL && IsActive( TRUE ) )
//	{
//		m_pFrame->SetFocus();
//	}
//	else if ( nIDEvent == 2 && m_pFrame != NULL )
//	{
//		if ( Settings.General.Language.Left(2) != _T("en") )
//		{
//			CString str;
//			m_pFrame->GetWindowText( str );
//			if ( str.Find( _T("Chat : ") ) == 0 )
//				str = LoadString( IDR_CHATFRAME ) + str.Mid( 4 );
//			SetWindowText( str );
//		}
//	}
//
//	CChildWnd::OnTimer( nIDEvent );
//}
