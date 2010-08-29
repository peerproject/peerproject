//
// CtrlIRCFrame.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2005-2008.
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
// Original Author: peer_l_@hotmail.com
//

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"

#include "Skin.h"
#include "CtrlIRCFrame.h"
#include "RichElement.h"
#include "RichFragment.h"
//#include "PeerProjectURL.h"
#include "DlgSettingsManager.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Buffer.h"
#include "Network.h"
#include "WndMain.h"
#include "DlgIrcInput.h"
#include "GProfile.h"
//#include "Emoticons.h"
#include "Plugins.h"	// IChatPlugin Capture

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SIZE_INTERNAL	1982
#define SIZE_BARSLIDE	1983
#define NEWLINE_FORMAT	_T("2")
#define DEST_PORT		6667
#define HISTORY_SIZE	20		// Sent messages

#define CHANCOUNTUPDATE	100000	// 1.5 minutes refresh /list

IMPLEMENT_DYNAMIC(CIRCFrame, CWnd)

BEGIN_MESSAGE_MAP(CIRCFrame, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_SYSCOMMAND()

	ON_UPDATE_COMMAND_UI(ID_IRC_CONNECT, OnUpdateIrcConnect)
	ON_COMMAND(ID_IRC_CONNECT, OnIrcConnect)
	ON_UPDATE_COMMAND_UI(ID_IRC_DISCONNECT, OnUpdateIrcDisconnect)
	ON_COMMAND(ID_IRC_DISCONNECT, OnIrcDisconnect)
	ON_UPDATE_COMMAND_UI(ID_IRC_CLOSETAB, OnUpdateIrcCloseTab)
	ON_COMMAND(ID_IRC_CLOSETAB, OnIrcCloseTab)
	ON_UPDATE_COMMAND_UI(ID_IRC_SENDTEXT, OnUpdateIrcSendText)
	ON_COMMAND(ID_IRC_SENDTEXT, OnIrcSendText)

	ON_COMMAND(ID_IRC_SETTINGS, OnIrcShowSettings)
	ON_UPDATE_COMMAND_UI(ID_IRC_SETTINGS, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_QUERY, UserListDblClick)
	ON_UPDATE_COMMAND_UI(ID_IRC_QUERY, OnUpdateIrcUserCmd)
 	ON_COMMAND(ID_IRC_TIME, OnIrcUserCmdTime)
	ON_UPDATE_COMMAND_UI(ID_IRC_TIME, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_VERSION, OnIrcUserCmdVersion)
	ON_UPDATE_COMMAND_UI(ID_IRC_VERSION, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_BROWSE, OnIrcUserCmdBrowse)
	ON_UPDATE_COMMAND_UI(ID_IRC_BROWSE, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_WHOIS, OnIrcUserCmdWhois)
	ON_UPDATE_COMMAND_UI(ID_IRC_WHOIS, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_IGNORE, OnIrcUserCmdIgnore)
	ON_UPDATE_COMMAND_UI(ID_IRC_IGNORE, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_UNIGNORE, OnIrcUserCmdUnignore)
	ON_UPDATE_COMMAND_UI(ID_IRC_UNIGNORE, OnUpdateIrcUserCmd)

	ON_COMMAND(ID_IRC_OP, OnIrcUserCmdOp)
	ON_UPDATE_COMMAND_UI(ID_IRC_OP, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_DEOP, OnIrcUserCmdDeop)
	ON_UPDATE_COMMAND_UI(ID_IRC_DEOP, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_VOICE, OnIrcUserCmdVoice)
	ON_UPDATE_COMMAND_UI(ID_IRC_VOICE, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_DEVOICE, OnIrcUserCmdDevoice)
	ON_UPDATE_COMMAND_UI(ID_IRC_DEVOICE, OnUpdateIrcUserCmd)

	ON_COMMAND(ID_IRC_KICK, OnIrcUserCmdKick)
	ON_UPDATE_COMMAND_UI(ID_IRC_KICK, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_KICKWHY, OnIrcUserCmdKickWhy)
	ON_UPDATE_COMMAND_UI(ID_IRC_KICKWHY, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_BAN, OnIrcUserCmdBan)
	ON_UPDATE_COMMAND_UI(ID_IRC_BAN, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_UNBAN, OnIrcUserCmdUnban)
	ON_UPDATE_COMMAND_UI(ID_IRC_UNBAN, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_BANKICK, OnIrcUserCmdBanKick)
	ON_UPDATE_COMMAND_UI(ID_IRC_BANKICK, OnUpdateIrcUserCmd)
	ON_COMMAND(ID_IRC_BANKICKWHY, OnIrcUserCmdBanKickWhy)
	ON_UPDATE_COMMAND_UI(ID_IRC_BANKICKWHY, OnUpdateIrcUserCmd)

	ON_NOTIFY(RVN_CLICK, IDC_CHAT_TEXT, OnRichClk)
	ON_NOTIFY(RVN_DBLCLICK, IDC_CHAT_TEXT, OnRichDblClk)
	ON_NOTIFY(RVN_SETCURSOR, IDC_CHAT_TEXT, OnRichCursorMove)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CHAT_TABS, OnClickTab)
END_MESSAGE_MAP()

CIRCFrame* CIRCFrame::g_pIrcFrame = NULL;

/////////////////////////////////////////////////////////////////////////////
// CIRCFrame construction

CIRCFrame::CIRCFrame()
	: m_bConnected		( FALSE )
	, m_nSelectedTab	( 0 )
	, m_nMsgsInSec		( 0 )
	, m_nTimerVal		( 0 )
	, m_nSelectedTabType( 0 )
	, m_nRSelectedTab	( 0 )
	, m_bFloodProtectionRunning ( FALSE )
	, m_nFloodLimit 	( 0 )
	, m_nFloodingDelay	( 4000 )
	, m_nUpdateFrequency( 40 )
//	, m_nUpdateChanListFreq ( 100000 )
	, m_nBufferCount	( 0 )
	, m_nHeaderIcon 	( 0 )
	, m_hBuffer 		( NULL )
//	, m_ptCursor		( 0, 0 )
//	, m_nListWidth		( 170 )
	, m_nSocket 		( INVALID_SOCKET )
	, m_nLocalTextLimit	( 300 )
	, m_nLocalLinesLimit( 14 )
{
	if ( g_pIrcFrame == NULL ) g_pIrcFrame = this;

	for ( int nChannel = 0; nChannel < MAX_CHANNELS; ++nChannel )
		m_nCurrentPosLineBuffer[ nChannel ] = -1;
}

CIRCFrame::~CIRCFrame()
{
	if ( g_pIrcFrame == this )
		g_pIrcFrame = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CIRCFrame system message handlers

BOOL CIRCFrame::Create(CWnd* pParentWnd)
{
	CRect rect;
	return CWnd::Create( NULL, _T("CIRCFrame"), WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,
		rect, pParentWnd, IDC_IRC_FRAME );
}

int CIRCFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	if ( ! m_wndPanel.Create( this ) ) return -1;
	CRect rectDefault;
	SetOwner( GetParent() );

	m_wndTab.Create( WS_CHILD | WS_VISIBLE | TCS_FLATBUTTONS | TCS_HOTTRACK | TCS_OWNERDRAWFIXED,
		rectDefault, this, IDC_CHAT_TABS );
	if ( ! theApp.m_bIsWin2000 )
		m_wndTab.ModifyStyleEx( 0, WS_EX_COMPOSITED );	//Stop control flickering XP+

	FillChanList();
	m_wndView.Create( WS_CHILD|WS_VISIBLE, rectDefault, this, IDC_CHAT_TEXT );

	m_pContent.m_crBackground = Settings.IRC.Colors[ ID_COLOR_CHATWINDOW ];
	m_wndView.SetDocument( &m_pContent );
	m_wndView.SetSelectable( TRUE );
	m_wndView.SetFollowBottom( TRUE );

	if ( ! m_wndMainBar.Create( this, WS_CHILD | WS_VISIBLE | CBRS_NOALIGN, AFX_IDW_TOOLBAR ) )
		return -1;
	m_wndMainBar.SetBarStyle( m_wndMainBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_BORDER_TOP );
	m_wndMainBar.SetOwner( GetOwner() );

	if ( Settings.General.LanguageRTL )
		m_wndMainBar.ModifyStyleEx( 0, WS_EX_LAYOUTRTL, 0 );

	m_wndEdit.Create( WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, rectDefault, this, IDC_CHAT_EDIT );
	m_wndEdit.SetLimitText( m_nLocalTextLimit );
	m_wndEdit.ModifyStyleEx( WS_EX_LAYOUTRTL, 0, 0 );
	m_wndView.ModifyStyleEx( WS_EX_LAYOUTRTL, 0, 0 );
	m_wndTab.ModifyStyleEx( WS_EX_LAYOUTRTL, 0, 0 );

	SetFonts();

	// Create "Status" tab
	LoadString( m_sStatus, IDS_TIP_STATUS );
	m_sStatus.Remove( ':' );
	AddTab( m_sStatus, ID_KIND_CLIENT );

	// Initial Text
	//StatusMessage( LoadString( IDS_IRC_HEADERTITLE ), ID_COLOR_TEXT );
	//StatusMessage( LoadString( IDS_IRC_HEADERSUBTITLE ), ID_COLOR_TEXT );
	StatusMessage( _T("...") + LoadString( IDS_NETWORK_DISCONNECTED ), ID_COLOR_TEXTLOCAL );
	// ToDo: Fix lost few leading characters  ("..." workaround spacer)

	// Reload saved channels
	OnIrcChanCmdOpen();

	return 0;
}

void CIRCFrame::ClearChanListCount()
{
	CListCtrl& wndChanList = m_wndPanel.m_boxChans.m_wndChanList;
	for ( int i = 0 ; i < wndChanList.GetItemCount() ; ++i )
	{
		wndChanList.SetItemText( i, 1, _T("") );
	}
}

// strUserCount = "0" will increase the current number
// strUserCount = "-1" will decrease current number
// Otherwise set the number
void CIRCFrame::FillChanListCount(const CString& strUserCount, const CString& strChannelName)
{
	BOOL bFound = FALSE;
	CString strCurrentChannel, strList, strUsers, strCount, strDisplay;
	int nCount = _tstoi( strUserCount ), nList, nIndex, nCountWnd;
	CListCtrl& wndChanList = m_wndPanel.m_boxChans.m_wndChanList;
	nIndex = m_pChanList.GetIndexOfName( strChannelName );
	if ( nIndex == -1 ) return;

	strDisplay = m_pChanList.GetDisplayOfIndex( nIndex );

	for ( nList = 0 ; nList < wndChanList.GetItemCount() ; nList++ )
	{
		strList = wndChanList.GetItemText( nList, 0 );
		if ( strDisplay.CompareNoCase( strList ) == 0 )
		{
			bFound = TRUE;
			break;
		}
	}

	if ( bFound )
		strList = wndChanList.GetItemText( nList, 1 );
	else
		strList = strUserCount;

	if ( strUserCount == _T("0") )
		nCountWnd = _tstoi( strList ) + 1;
	else if ( strUserCount == _T("-1") )
		nCountWnd = _tstoi( strList ) - 1;
	else
		nCountWnd = nCount;

	strCount.Format( _T("%d"), nCountWnd );
	if ( ! bFound )
		nList = wndChanList.InsertItem( wndChanList.GetItemCount() , strDisplay );
	wndChanList.SetItemText( nList, 1, strCount );
}

void CIRCFrame::FillChanList()
{
	m_pChanList.RemoveAll();
	m_pChanList.AddChannel( _T(" PeerProject Main"), _T("#PeerProject") );
	m_pChanList.AddChannel( _T(" PeerProject Chat"), _T("#PeerProjectChat") );
	m_pChanList.AddChannel( _T(" PeerProject Help"), _T("#PeerProjectHelp") );
	m_pChanList.AddChannel( _T("Development"), _T("#PeerProjectDev") );
	m_pChanList.AddChannel( _T("FileShare"), _T("#PeerProjectFiles") );
	m_pChanList.AddChannel( _T("Afrikaans"), _T("#PeerProject-af") );
	m_pChanList.AddChannel( _T("Arabic"), _T("#PeerProject-ar") );
	m_pChanList.AddChannel( _T("Čeština"), _T("#PeerProject-cz") );
	m_pChanList.AddChannel( _T("Deutsch"), _T("#PeerProject-de") );
	m_pChanList.AddChannel( _T("Español"), _T("#PeerProject-es") );
	m_pChanList.AddChannel( _T("Français"), _T("#PeerProject-fr") );
	m_pChanList.AddChannel( _T("Ελληνικά"), _T("#PeerProject-gr") );
	m_pChanList.AddChannel( _T("Hebrew"), _T("#PeerProject-heb") );
	m_pChanList.AddChannel( _T("Hrvatski"), _T("#PeerProject-hr") );
	m_pChanList.AddChannel( _T("Magyar"), _T("#PeerProject-hu") );
	m_pChanList.AddChannel( _T("Italiano"), _T("#PeerProject-it") );
	m_pChanList.AddChannel( _T("日本語"), _T("#PeerProject-ja") );
	m_pChanList.AddChannel( _T("한국어"), _T("#PeerProject-ko") );
	m_pChanList.AddChannel( _T("Lietuvių"), _T("#PeerProject-lt") );
	m_pChanList.AddChannel( _T("Nederlands"), _T("#PeerProject-nl") );
	m_pChanList.AddChannel( _T("Norsk"), _T("#PeerProject-no") );
	m_pChanList.AddChannel( _T("Polski"), _T("#PeerProject-po") );
	m_pChanList.AddChannel( _T("Português"), _T("#PeerProject-pt") );
	m_pChanList.AddChannel( _T("Pусский"), _T("#PeerProject-ru") );
	m_pChanList.AddChannel( _T("Slovenščina"), _T("#PeerProject-sl") );
	m_pChanList.AddChannel( _T("Shqip"), _T("#PeerProject-sq") );
	m_pChanList.AddChannel( _T("Srpski"), _T("#PeerProject-sr") );
	m_pChanList.AddChannel( _T("Svenska"), _T("#PeerProject-sv") );
	m_pChanList.AddChannel( _T("Suomi"), _T("#PeerProject-fi") );
	m_pChanList.AddChannel( _T("Türkçe"), _T("#PeerProject-tr") );
	m_pChanList.AddChannel( _T("中文"), _T("#PeerProject-zh") );
	m_pChanList.AddChannel( _T("Shareaza Chat"), _T("#Shareaza-Chat") );
	m_pChanList.AddChannel( _T("Shareaza Español"), _T("#Shareaza-Spanish") );
	m_pChanList.AddChannel( _T("Shareaza Français"), _T("#Shareaza-French") );
	m_pChanList.AddChannel( _T("Shareaza Deutsch"), _T("#Shareaza-German") );
	m_pChanList.AddChannel( _T("Shareaza Italiano"), _T("#Shareaza-Italian") );
	m_pChanList.AddChannel( _T("Shareaza Japanese"), _T("#Shareaza-Japanese") );
	m_pChanList.AddChannel( _T("Shareaza Nederlands"), _T("#Shareaza-Dutch") );
	m_pChanList.AddChannel( _T("Shareaza Polski"), _T("#Shareaza-Polish") );
	m_pChanList.AddChannel( _T("Shareaza Português"), _T("#Shareaza-Portuguese") );
	m_pChanList.AddChannel( _T("Shareaza Pусский"), _T("#Shareaza-Russian") );
	m_pChanList.AddChannel( _T("Shareaza"), _T("#shareaza") );
}

void CIRCFrame::SetFonts()
{
	m_fntEdit.DeleteObject();
	m_pContent.m_fntNormal.DeleteObject();

	// Obsolete:
	//int nHeight = (int)Settings.IRC.FontSize;
	//
	// Find optimal default font sizes for different languages
	//if ( nHeight < 6 || nHeight > 100 )
	//{
	//	nHeight = 10;
	//	TEXTMETRIC txtMetric;
	//	CDC* pDC = GetDC();
	//
	//	LOGFONT lf = {};
	//	lf.lfCharSet = DEFAULT_CHARSET;
	//	lf.lfQuality = theApp.m_nFontQuality;
	//	lstrcpy( lf.lfFaceName, Settings.IRC.ScreenFont );
	//
	//	CFont pFont;
	//	pFont.CreateFontIndirect( &lf );
	//	pDC->SelectObject( pFont );
	//	pDC->GetTextMetrics( &txtMetric );
	//
	//	// 19, 16 and 13 are Tahoma font measurements
	//	if ( txtMetric.tmInternalLeading )
	//	{
	//		if ( ( txtMetric.tmPitchAndFamily & TMPF_VECTOR ) &&
	//			 ( txtMetric.tmPitchAndFamily & TMPF_TRUETYPE ) || txtMetric.tmCharSet < 2 )
	//		{
	//			int nMainChar = txtMetric.tmAscent - txtMetric.tmInternalLeading;
	//			float nPercentage = (float)nMainChar / (float)txtMetric.tmAscent;
	//			if ( nPercentage < 0.45 )
	//			{
	//				nMainChar = txtMetric.tmInternalLeading;
	//				nPercentage = 1 - nPercentage;
	//			}
	//			if ( nPercentage < 0.55 )
	//				nHeight =  (int)( 10.0f * 19.0f / txtMetric.tmAscent + 0.5 );
	//			else if ( nPercentage > 0.85 )
	//				nHeight =  (int)( 10.0f * 19.0f / txtMetric.tmHeight + 0.44 );
	//			else if ( nPercentage > 0.69 )
	//				nHeight =  (int)( 10.0f * 13.0f / (float)nMainChar + 0.1 );
	//			else
	//				nHeight =  (int)( 10.0f * 13.0f / ( 16.0f / txtMetric.tmAscent * nMainChar ) + 0.5 );
	//		}
	//		else nHeight = 10;
	//	}
	//	else
	//	{
	//		if ( txtMetric.tmPitchAndFamily & TMPF_FIXED_PITCH )
	//			nHeight =  (int)( 10.0 * 16.0 / txtMetric.tmAscent + 0.5 );
	//		else
	//			nHeight = 11;
	//	}
	//
	//	nHeight = MulDiv( nHeight, GetDeviceCaps( pDC->m_hDC, LOGPIXELSY ), 72 );
	//
	//	ReleaseDC( pDC );
	//}

	m_pContent.m_fntNormal.CreateFont( -(int)Settings.IRC.FontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, Settings.IRC.ScreenFont );

	m_fntEdit.CreateFont( -(int)( Settings.Fonts.FontSize + 1 ), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, Settings.Fonts.DefaultFont );

	m_wndEdit.SetFont( &m_fntEdit, TRUE );
	m_wndTab.SetFont( &theApp.m_gdiFont, TRUE );
}

void CIRCFrame::OnDestroy()
{
	OnIrcChanCmdSave();

	OnIrcDisconnect();

	CWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CIRCFrame presentation message handlers

void CIRCFrame::OnSkinChange()
{
	OnSize( 0, 0, 0 );
	m_wndPanel.OnSkinChange();

	Skin.CreateToolBar( _T("CIRCFrame"), &m_wndMainBar );
	if ( m_bmWatermark.m_hObject != NULL ) m_bmWatermark.DeleteObject();
	if ( HBITMAP hMark = Skin.GetWatermark( _T("CIRCHeaderPanel") ) )
		m_bmWatermark.Attach( hMark );
	else if ( Colors.m_crBannerBack == RGB( 122, 160, 230 ) )
		m_bmWatermark.LoadBitmap( IDB_BANNER_MARK );

	SetFonts();
	m_nHeaderIcon = CoolInterface.ExtractIconID( IDR_IRCFRAME, FALSE, LVSIL_BIG );
	m_pContent.m_crBackground = Settings.IRC.Colors[ ID_COLOR_CHATWINDOW ];
	m_wndView.SetDocument( &m_pContent );
	TabClick();
}

void CIRCFrame::OnUpdateCmdUI()
{
	m_wndMainBar.OnUpdateCmdUI( (CFrameWnd*)GetOwner(), TRUE );
}

void CIRCFrame::OnSize(UINT nType, int cx, int cy)
{
	if ( nType != SIZE_INTERNAL && nType != SIZE_BARSLIDE )
		CWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	rc.right++;

	if ( rc.Width() < 32 || rc.Height() < 32 ) return;

	m_wndPanel.SetWindowPos( NULL, 0, 0, Skin.m_nSidebarWidth, rc.Height(), SWP_NOZORDER );

	rc.left   += Skin.m_nSidebarWidth;
	rc.bottom -= Skin.m_nToolbarHeight;
	if ( m_bConnected )
	{
		m_wndTab.SetWindowPos( NULL, rc.left, rc.top,
			rc.Width(), TABBAR_HEIGHT, SWP_NOZORDER|SWP_SHOWWINDOW );
		rc.top += TABBAR_HEIGHT;
	}
	else // Header instead when idle
	{
		m_wndTab.SetWindowPos( NULL, NULL, NULL,
			NULL, NULL, SWP_NOZORDER|SWP_HIDEWINDOW );
		rc.top += Skin.m_nHeaderbarHeight;
	}

	m_wndMainBar.SetWindowPos( NULL, rc.left, rc.bottom, rc.Width(),
		Skin.m_nToolbarHeight, SWP_NOZORDER|SWP_SHOWWINDOW );
	m_wndEdit.SetWindowPos( NULL, rc.left, rc.bottom - EDITBOX_HEIGHT,
		rc.Width(), EDITBOX_HEIGHT, SWP_NOZORDER|SWP_SHOWWINDOW );
	rc.bottom -= EDITBOX_HEIGHT;
	m_wndView.SetWindowPos( NULL, rc.left, rc.top,
		rc.Width(), rc.Height() - SEPERATOR_HEIGHT - SMALLHEADER_HEIGHT, SWP_NOZORDER|SWP_SHOWWINDOW );
	if ( nType != SIZE_BARSLIDE )
		Invalidate();
	if ( m_wndTab.GetItemCount() > 0 )
		ReloadViewText();
}

void CIRCFrame::OnPaint()
{
	CPaintDC dc( this );
	CRect rcClient;
	GetClientRect( &rcClient );

	dc.SetTextColor( Colors.m_crBannerText );
	dc.SetBkMode( TRANSPARENT );

	CRect rcComponent;

	// Header Area (when disconnected only)
	if ( ! m_bConnected )
	{
		rcComponent.right = rcClient.right;
		rcComponent.left = rcClient.left + Skin.m_nSidebarWidth;
		rcComponent.top = rcClient.top;
		rcComponent.bottom = rcComponent.top + Skin.m_nHeaderbarHeight;
		PaintHeader( rcComponent, dc );
		rcComponent.DeflateRect( 14, 0 );
		CoolInterface.DrawEx( &dc, m_nHeaderIcon,
			CPoint( rcComponent.left + 4, rcComponent.top + 4 ), CSize( 48, 48 ),
			CLR_NONE, CLR_NONE, ILD_NORMAL, LVSIL_BIG );
		rcComponent.DeflateRect( 44, 0 );
		rcComponent.DeflateRect( 10, 12 );

		CString pszTitle;
		LoadString( pszTitle, IDS_IRC_HEADERTITLE );
		CString pszSubtitle;
		LoadString( pszSubtitle, IDS_IRC_HEADERSUBTITLE );
		dc.SelectObject( &CoolInterface.m_fntCaption );
		DrawText( &dc, rcComponent.left, rcComponent.top, pszTitle );
		rcComponent.DeflateRect( 0, 14 );
		dc.SelectObject( &CoolInterface.m_fntNormal );
		DrawText( &dc, rcComponent.left, rcComponent.top, pszSubtitle );
	}

	// "Chat:" Divider Area
	rcComponent.right = rcClient.right;
	rcComponent.left = rcClient.left + Skin.m_nSidebarWidth;
	rcComponent.top = rcClient.bottom - Skin.m_nToolbarHeight -
		EDITBOX_HEIGHT - SMALLHEADER_HEIGHT;
	rcComponent.bottom = rcComponent.top + SMALLHEADER_HEIGHT;
	PaintHeader( rcComponent, dc );
	rcComponent.DeflateRect( 8, 2 );
	dc.SelectObject( &CoolInterface.m_fntCaption );
	CString str;
	LoadString( str, IDS_IRC_HEADERINPUT );
	DrawText( &dc, rcComponent.left, rcComponent.top, str );

	rcComponent.right = rcClient.right;
	rcComponent.left = rcClient.left + Skin.m_nSidebarWidth;
	rcComponent.top = rcClient.bottom - Skin.m_nToolbarHeight -
		EDITBOX_HEIGHT - SMALLHEADER_HEIGHT - SEPERATOR_HEIGHT;
	rcComponent.bottom = rcComponent.top + SEPERATOR_HEIGHT;
	dc.FillSolidRect( rcComponent.left, rcComponent.top, 1,
		rcComponent.Height(), Colors.m_crSysBtnFace );
	dc.FillSolidRect( rcComponent.left + 1, rcComponent.top, 1,
		rcComponent.Height(), Colors.m_crSys3DHighlight );
	dc.FillSolidRect( rcComponent.right - 1, rcComponent.top, 1,
		rcComponent.Height(), Colors.m_crSys3DShadow );
	dc.FillSolidRect( rcComponent.left + 2, rcComponent.top,
		rcComponent.Width() - 3, rcComponent.Height(), Colors.m_crSysBtnFace );
	dc.ExcludeClipRect( &rcComponent );
}

void CIRCFrame::PaintHeader(CRect rcHeader, CDC &dc)
{
	CRect rcClient;
	GetWindowRect( &rcClient );
	ScreenToClient( &rcClient );
	if ( rcClient.IsRectEmpty() ) return;
	if ( rcHeader.IsRectEmpty() ) return;
	if ( m_bmBuffer.m_hObject != NULL )
	{
		m_dcBuffer.SelectObject( m_hBuffer );
		m_dcBuffer.DeleteDC();
		m_bmBuffer.DeleteObject();
	}
	m_bmBuffer.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	m_dcBuffer.CreateCompatibleDC( &dc );
	m_hBuffer = (HBITMAP)m_dcBuffer.SelectObject( &m_bmBuffer )->m_hObject;

	if ( ! CoolInterface.DrawWatermark( &m_dcBuffer, &rcClient, &m_bmWatermark ) )
		m_dcBuffer.FillSolidRect( &rcHeader, Colors.m_crBannerBack );

	dc.BitBlt( rcHeader.left, rcHeader.top, rcHeader.Width(),
		rcHeader.Height(), &m_dcBuffer, 0, 0, SRCCOPY );
}

void CIRCFrame::DrawText(CDC* pDC, int nX, int nY, LPCTSTR pszText)
{
	CSize sz = pDC->GetTextExtent( pszText );

	CRect rc( nX - 2, nY - 2, nX + sz.cx + 2, nY + sz.cy + 2 );

	DWORD nOptions = ETO_CLIPPED | ( Settings.General.LanguageRTL ? ETO_RTLREADING : 0 );
	pDC->ExtTextOut( nX, nY, nOptions, &rc, pszText, static_cast< UINT >( _tcslen( pszText ) ), NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CIRCFrame interaction message handlers

void CIRCFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if ( pWnd->m_hWnd == m_wndView.m_hWnd )
	{
		CString strText = GetTextFromRichPoint();
		if ( strText.IsEmpty() ) return;
		int nIndex = IsUserInList( strText );
		if ( nIndex != -1 )
		{
			SetSelectedUser( nIndex );
			Skin.TrackPopupMenu( _T("CIRCUserList"), point );
		}
	}
	if ( pWnd->m_hWnd == m_wndTab.m_hWnd )
	{
		CRect rcTab;
		for ( int nTab = 0 ; nTab < m_wndTab.GetItemCount() ; nTab++ )
		{
			m_wndTab.GetItemRect( nTab, rcTab );
			m_wndTab.ClientToScreen( rcTab );
			if ( rcTab.PtInRect( point ) )
			{
				m_wndTab.SetCurSel( nTab );
				TabClick();
				Skin.TrackPopupMenu( _T("CIRCTabRClick"), point );
				return;
			}
		}
	}
}

void CIRCFrame::OnIrcConnect()
{
	if ( m_bConnected )
		return;	// Already Connected

	m_sWsaBuffer.Empty();

	//CString Settings.IRC.ServerName
	//DWORD   Settings.IRC.ServerPort

	CString strMessage;
	strMessage.Format( LoadString( IDS_CHAT_CONNECTING_TO ), Settings.IRC.ServerName );
	StatusMessage( strMessage, ID_COLOR_TEXT );

	struct hostent* host = gethostbyname( (LPCSTR)CT2A( Settings.IRC.ServerName ) );
	if ( host == NULL )				// Unknown host
	{
		strMessage.Format( LoadString( IDS_CHAT_CANT_CONNECT ), Settings.IRC.ServerName );
		StatusMessage( strMessage );
		return;
	}

	SOCKADDR_IN dest_addr = {};
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = (u_short)ntohs( (WORD)Settings.IRC.ServerPort );
	dest_addr.sin_addr.s_addr = *(ULONG*)host->h_addr;

	m_nSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	int RetVal = WSAConnect(
		m_nSocket, 					// Our socket
		(SOCKADDR*)&dest_addr,		// The remote IP address and port number
		sizeof(SOCKADDR_IN),		// How many bytes the function can read
		NULL, NULL, NULL, NULL );	// No advanced features

	if ( RetVal == SOCKET_ERROR )
	{
		strMessage.Format( LoadString( IDS_CHAT_CANT_CONNECT ), Settings.IRC.ServerName );
		StatusMessage( strMessage );
		CNetwork::CloseSocket( m_nSocket, false );
		return;
	}

	StatusMessage( LoadString( IDS_CHAT_CONNECTED ), ID_COLOR_TEXT );

	m_sNickname	= Settings.IRC.Nick;
	if ( m_sNickname.IsEmpty() )
	{
		m_sNickname = MyProfile.GetNick();
		if ( m_sNickname.IsEmpty() )
			m_sNickname.Format( L"PeerIrc%.06u", GetRandomNum( 0ui32, _UI32_MAX ) );

		Settings.IRC.Nick = m_sNickname;
	}

	SendString( _T("NICK ") + m_sNickname );

	SendString( _T("USER ") + Settings.IRC.UserName +
		_T(" UserHost UserServ :") + Settings.IRC.RealName );

	m_pWakeup.ResetEvent();
	WSAEventSelect( m_nSocket, m_pWakeup, FD_READ | FD_CLOSE );
	OnStatusMessage( _T("Activating Connection..."), ID_COLOR_NOTICE );
	m_nTimerVal = 0;
	m_nMsgsInSec = 0;
	m_bConnected = TRUE;
	m_bFloodProtectionRunning = FALSE;

	SetTimer( 9, m_nUpdateFrequency, NULL );
	SetTimer( 7, CHANCOUNTUPDATE, NULL );	// m_nUpdateChanListFreq
	m_pLastLineBuffer->RemoveAll();

	OnSize( 0, 0, 0 );	// Hide Header & Show Tabs

	if ( Settings.IRC.OnConnect.GetLength() > 2 )
		OnLocalText( Settings.IRC.OnConnect );
}

void CIRCFrame::OnUpdateIrcConnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( ! m_bConnected );
	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( ! m_bConnected );
	else
		pCmdUI->SetCheck( m_bConnected );
}

void CIRCFrame::OnUpdateIrcUserCmd(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( TRUE );
	pCmdUI->SetCheck( FALSE );
}

void CIRCFrame::OnIrcUserCmdWhois()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/whois ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdOp()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/mode ") + GetTabText() + _T(" +o ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdDeop()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/mode ") + GetTabText() + _T(" -o ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcChanCmdOpen()
{
	CString strPath = Settings.General.UserPath + _T("\\Data\\ChatChanlist.dat");

	CFile pFile;
	if ( ! pFile.Open( strPath, CFile::modeRead ) ) return;

	CBuffer pBuffer;
	pBuffer.EnsureBuffer( (DWORD)pFile.GetLength() );
	pBuffer.m_nLength = (DWORD)pFile.GetLength();
	pFile.Read( pBuffer.m_pBuffer, pBuffer.m_nLength );
	pFile.Close();

	CString strItem;
	while ( pBuffer.ReadLine( strItem ) )
	{
		strItem.Trim();

		if ( strItem.GetLength() && strItem.GetAt( 0 ) == _T('#') )
		{
			CListCtrl& wndChanList = m_wndPanel.m_boxChans.m_wndChanList;
			m_pChanList.AddChannel( strItem.Mid( 1 ), strItem, TRUE );
			wndChanList.InsertItem( wndChanList.GetItemCount(), strItem.Mid( 1 ) );
		}
	}
}

void CIRCFrame::OnIrcChanCmdSave()
{
	CFile pFile;
	if ( ! pFile.Open( Settings.General.UserPath + _T("\\Data\\ChatChanlist.dat"),
		CFile::modeWrite | CFile::modeCreate ) )
		return;

	CListCtrl* pChannelList = &(m_wndPanel.m_boxChans.m_wndChanList);
	for ( int nIndex = 0 ; nIndex < pChannelList->GetItemCount() ; nIndex++ )
	{
		int n_mpChanListIndex = m_pChanList.GetIndexOfDisplay( pChannelList->GetItemText( nIndex, 0 ) );
		if ( n_mpChanListIndex != -1 && m_pChanList.GetType( m_pChanList.GetDisplayOfIndex( n_mpChanListIndex ) )  )
		{
			CT2A pszFile( m_pChanList.GetNameOfIndex( n_mpChanListIndex ) + _T("\r\n") );
			pFile.Write( (LPCSTR)pszFile, (DWORD)strlen( pszFile ) );
		}
	}
}

void CIRCFrame::OnIrcUserCmdVoice()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/mode ") + GetTabText() + _T(" +v ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcShowSettings()
{
	CSettingsManagerDlg::Run( _T("CIRCSettingsPage") );
}

void CIRCFrame::OnIrcUserCmdBan()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/mode ") + GetTabText() + _T(" +b ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdUnban()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/mode ") + GetTabText() + _T(" -b ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdKick()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/kick ") + GetTabText() + _T(" ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdKickWhy()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	CIrcInputDlg dlg( this, 1, TRUE );	// 1 = select the second caption
	if ( dlg.DoModal() != IDOK ) return;
	OnLocalText( _T("/kick ") + GetTabText() + _T(" ") + RemoveModeOfNick( strUser ) + _T(" ") + dlg.m_sAnswer );
}

void CIRCFrame::OnIrcUserCmdBanKick()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/mode ") + GetTabText() + _T(" +b ") + RemoveModeOfNick( strUser ) );
	OnLocalText( _T("/kick ") + GetTabText() + _T(" ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdBanKickWhy()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	CIrcInputDlg dlg( this, 1, FALSE );	// 1 = select the second caption
	if ( dlg.DoModal() != IDOK ) return;
	OnLocalText( _T("/mode ") + GetTabText() + _T(" +b ") + RemoveModeOfNick( strUser ) );
	OnLocalText( _T("/kick ") + GetTabText() + _T(" ") + RemoveModeOfNick( strUser ) + _T(" ") + dlg.m_sAnswer );
}

void CIRCFrame::OnIrcUserCmdDevoice()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/mode ") + GetTabText() + _T(" -v ") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdIgnore()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/SILENCE +") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdUnignore()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/SILENCE -") + RemoveModeOfNick( strUser ) );
}

void CIRCFrame::OnIrcUserCmdTime()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/PRIVMSG ") + RemoveModeOfNick( strUser ) + _T(" :\x01TIME\x01") );
}

void CIRCFrame::OnIrcUserCmdVersion()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/PRIVMSG ") + RemoveModeOfNick( strUser ) + _T(" :\x01VERSION\x01") );
}

void CIRCFrame::OnIrcUserCmdBrowse()
{
	CString strUser = GetSelectedUser();
	if ( strUser.IsEmpty() ) return;
	OnLocalText( _T("/PRIVMSG ") + RemoveModeOfNick( strUser ) + _T(" :\x01USERINFO\x01") );
}

void CIRCFrame::OnUpdateIrcSendText(CCmdUI* pCmdUI)
{
	m_wndEdit.GetWindowText( m_sCurrent );
	pCmdUI->Enable( ! m_sCurrent.IsEmpty() );
	pCmdUI->SetCheck( FALSE );
}

void CIRCFrame::OnIrcSendText()
{
	m_wndEdit.GetWindowText( m_sCurrent );
	if ( m_sCurrent.IsEmpty() ) return;

	if ( m_wndEdit.GetLineCount() > m_nLocalLinesLimit )
	{
		AfxMessageBox( _T("Exceeded Max number of lines allowed.") );
		return;
	}

	if ( m_sCurrent.GetLength() > m_nLocalTextLimit )
	{
		AfxMessageBox( _T("Exceeded Max length of text allowed.") );
		return;
	}

	int nTab = m_wndTab.GetCurSel();
	if ( nTab == -1 ) return;

	// Send Text on Enter Key:
	m_pLastLineBuffer[ nTab ].InsertAt( 0, m_sCurrent );
	if ( m_pLastLineBuffer[ nTab ].GetCount() > HISTORY_SIZE )
		m_pLastLineBuffer[ nTab ].RemoveAt( HISTORY_SIZE );
	m_nCurrentPosLineBuffer[ nTab ] = -1;
	OnLocalText( m_sCurrent );

	m_sCurrent.Empty();
	m_wndEdit.SetWindowText( m_sCurrent );
}

void CIRCFrame::OnUpdateIrcCloseTab(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndTab.GetCurSel() > 0 );
	pCmdUI->SetCheck( FALSE );
}

void CIRCFrame::OnIrcCloseTab()
{
	int nTab = m_wndTab.GetCurSel(), nOldTab( nTab );

	ClearUserList();
	m_pContent.Clear();

	CString strChannelName = GetTabText( nTab );
	FillChanListCount( _T("-1"), strChannelName );
	if ( strChannelName == m_sStatus ) return;
	if ( strChannelName.Left( 1 ) == _T("#") )
		SendString( _T("PART ") + strChannelName );

	m_wndTab.DeleteItem( nTab );
	m_pIrcBuffer[ nTab ].RemoveAll();
	m_pIrcUsersBuffer[ nTab ].RemoveAll();

	for ( ; nTab < m_nBufferCount - 1 ; nTab++ )
	{
		m_pIrcBuffer[ nTab ].Append( m_pIrcBuffer[ nTab + 1 ] );
		m_pIrcUsersBuffer[ nTab ].Append( m_pIrcUsersBuffer[ nTab + 1 ] );
	}

	m_nBufferCount--;

	m_wndTab.SetCurSel( nOldTab > 1 ? nOldTab - 1 : 0 );

	TabClick();
}

void CIRCFrame::OnUpdateIrcDisconnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_bConnected );
	pCmdUI->SetCheck( FALSE );

	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( m_bConnected );
}

void CIRCFrame::OnIrcDisconnect()
{
	ClearUserList();
	ClearChanListCount();

	OnStatusMessage( LoadString( IDS_NETWORK_DISCONNECTED ), ID_COLOR_NOTICE );
	SendString( _T("QUIT") );

	CNetwork::CloseSocket( m_nSocket, false );

	KillTimer( 7 );
	KillTimer( 9 );

	// Delete all tabs except Status
	for ( int nChannel = 1 ; nChannel < MAX_CHANNELS ; nChannel++ )
	{
		m_wndTab.DeleteItem( nChannel );
		m_pIrcBuffer[ nChannel ].RemoveAll();
		m_nCurrentPosLineBuffer[ nChannel ] = -1;
		m_pIrcUsersBuffer[ nChannel ].RemoveAll();
		m_pLastLineBuffer[ nChannel ].RemoveAll();
	}
	m_nBufferCount = 1;

	m_bConnected = FALSE;

	OnSize( 0, 0, 0 );	// Hide Tabs & Show Header
}

//  Return the title of a tab. (Current tab unless specified)
CString CIRCFrame::GetTabText(int nTabIndex) const
{
	if ( nTabIndex == -1 )
		nTabIndex = m_wndTab.GetCurSel();

	CString sBuffer;
	TCITEM item = { TCIF_IMAGE | TCIF_PARAM | TCIF_TEXT };

	item.pszText = sBuffer.GetBuffer( 1024 );
	item.cchTextMax = 1023;

	BOOL ret = m_wndTab.GetItem( nTabIndex, &item );
	sBuffer.ReleaseBuffer();

	return ret ? sBuffer : CString();
}

void CIRCFrame::OnLocalText(LPCTSTR pszText)
{
	CString strMessage = pszText, strSend, strBufferMsg, strStatusMsg;
	if ( strMessage.IsEmpty() ) return; 	// Crash prevention.  ToDo: Fix properly
	CString strTabTitle = GetTabText();
	BOOL bMeMsg = FALSE;	// /me Action

	// -1 if no CRLF, otherwise the position of CRLF
	strMessage.TrimRight( _T("\r\n") );

	// Expand command shortcuts
	if ( strMessage.GetLength() > 4 )
	{
		if ( strMessage.Left( 3 ) == _T("/j ") )
			strMessage = _T("/JOIN") + strMessage.Mid( 2 );
		else if ( strMessage.Left( 4 ) == _T("/ns ") )
			strMessage = _T("/NICKSERV") + strMessage.Mid( 3 );
		else if ( strMessage.Left( 4 ) == _T("/cs ") )
			strMessage = _T("/CHANSERV") + strMessage.Mid( 3 );
		else if ( strMessage.Left( 4 ) == _T("/bs ") )
			strMessage = _T("/BOTSERV") + strMessage.Mid( 3 );
		else if ( strMessage.Left( 4 ) == _T("/me ") )
			bMeMsg = TRUE;

		// Channel Format Correction
		if ( strMessage.Left( 6 ) == _T("/join ") && strMessage.Left( 7 ) != _T("/join #") )
			strMessage = _T("/join #") + strMessage.Mid( 6 );
	}

	// Line Formatting
	if ( bMeMsg )	// Action *Nick
	{
		strMessage = strMessage.Mid( 4 );
		strStatusMsg = _T("*") + m_sNickname + _T(" ") + strMessage;
	}
	else // Normal <Nick>
	{
		strStatusMsg = m_sNickname + _T(":  ") + strMessage;	// <NICK>
	}

// START local display line

	if ( Settings.IRC.Timestamp )
	{
		CTime pNow = CTime::GetCurrentTime();
		strStatusMsg.Format( _T("[%.2i:%.2i] %s"),
			pNow.GetHour(), pNow.GetMinute(), strStatusMsg );
	}

	strBufferMsg = CHAR( bMeMsg ? ID_COLOR_ME : ID_COLOR_TEXTLOCAL ) + strStatusMsg;

// END local display line

	// If it's a command, remove the '/'
	if ( strMessage.Left( 1 ).Compare( _T("/") ) == 0 )
	{
		strSend = strMessage.Mid( 1 );
	}
	else
	{
		// Not a command, but in the status window?
		if ( strTabTitle.Compare( m_sStatus ) == 0 )
		{
			OnNewMessage( _T(":localhost 400 You are not currently in a channel") );
			return;
		}
		else
		{
			// Display the line locally, in the appropriate color
			OnStatusMessage( strStatusMsg, bMeMsg ? ID_COLOR_ME : ID_COLOR_TEXTLOCAL );
			m_pIrcBuffer[ m_wndTab.GetCurSel() ].Add( strBufferMsg );

			if ( bMeMsg )
				strSend = _T("\x01 ACTION ") + strMessage + _T("\x01");
			else
				strSend = strMessage;

			strSend = _T("PRIVMSG ") + strTabTitle + _T(" :") + strSend;

			// Notify chat plugins about new local message
			Plugins.OnChatMessage( strTabTitle, TRUE, m_sNickname, strTabTitle, pszText );
		}
	}

	// Send 'strSend' to the server
	SendString( strSend );
}

void CIRCFrame::OnTimer(UINT_PTR nIDEvent)
{
	// Refresh channel list
	if ( nIDEvent == 7 )
	{
		//CListCtrl* pChannelList = (CListCtrl*)&(m_wndPanel.m_boxChans.m_wndChanList);
		//for ( int nList = 0 ; nList < pChannelList->GetItemCount() ; nList++ )
		//{
		//	if ( ! pChannelList->GetItemText( nList, 1 ).IsEmpty() )
		//		pChannelList->SetItemText( nList, 1, _T("0") );
		//}
		OnLocalText( _T("/list *PeerProject*") );
		OnLocalText( _T("/list *shareaza*") );
		return;
	}

	// Stop flood protection
	if ( nIDEvent == 8 )
	{
		m_bFloodProtectionRunning = FALSE;
		int nTargetWindow = GetTabIndex( m_sStatus );
		if ( m_wndTab.GetCurSel() == nTargetWindow )
			OnStatusMessage( _T("Stopped flood protection."), ID_COLOR_SERVERMSG );	// ToDo: Translate?
		m_pIrcBuffer[ nTargetWindow ].Add( char(ID_COLOR_SERVERMSG) + CString( _T("Stopped flood protection.") ) );
		KillTimer( 8 );
		m_nTimerVal = 0;
		return;
	}

	// Flood-check trigger + ???
	if ( nIDEvent == 9 )
	{
		m_nTimerVal++;
		if ( m_nTimerVal > 3000 / m_nUpdateFrequency && ! m_bFloodProtectionRunning )
		{
			m_nTimerVal = 0;
			m_nFloodLimit = Settings.IRC.FloodLimit * 3;

			if ( m_nMsgsInSec > m_nFloodLimit )
			{
				SetTimer( 8, m_nFloodingDelay, NULL );
				m_bFloodProtectionRunning = TRUE;
				int nTargetWindow = GetTabIndex( m_sStatus );
				if ( m_wndTab.GetCurSel() != nTargetWindow )
					m_pIrcBuffer[ m_wndTab.GetCurSel() ].Add( char(ID_COLOR_SERVERMSG ) + CString( _T("Running flood protection.") ) );
				OnStatusMessage( _T("Running flood protection."), ID_COLOR_SERVERMSG );	// ToDo: Translate?
			}
			m_nMsgsInSec = 0;
		}

		if ( WaitForSingleObject( m_pWakeup, 0 ) == WAIT_OBJECT_0 )
		{
			auto_array< char > pszData( new char[ 4096 ] );
			pszData[ 0 ] = '\0';
			int nRetVal = CNetwork::Recv( m_nSocket, pszData.get(), 4095 );
			int nError = WSAGetLastError();
			if ( nRetVal > 0 )
				pszData[ nRetVal ] = '\0';
			//CString strTmp = TrimString( m_sWsaBuffer + UTF8Decode( pszData.get() ) );
			CStringA strTmp = m_sWsaBuffer + pszData.get();
			m_sWsaBuffer.Empty();

			switch ( nRetVal )
			{
			case 0:	// Connection has been gracefully closed
				OnStatusMessage( TrimString( UTF8Decode( strTmp ) ), ID_COLOR_NOTICE );
				OnIrcDisconnect();
				return;

			case SOCKET_ERROR:
				KillTimer( 9 );
				if ( nError == WSAETIMEDOUT )
					OnStatusMessage( _T("QUIT: Connection reset by peer." ), ID_COLOR_NOTICE );
				else if ( nError == WSAENOTCONN )
					OnStatusMessage( _T("QUIT: Connection dropped."), ID_COLOR_NOTICE );
				else
					OnStatusMessage( _T("QUIT: Server is busy, please wait."), ID_COLOR_NOTICE );
				OnIrcDisconnect();
				return;
			}

			if ( ! strTmp.IsEmpty() )
			{
				m_nMsgsInSec++;

				// If it's not a complete line, add it to the buffer until we get the rest
				int nIndex = strTmp.Find( "\r\n" );
				if ( nIndex == -1 )
				{
					m_sWsaBuffer = strTmp;
				}
				else
				{
					while ( nIndex != -1 && ! strTmp.IsEmpty() )
					{
						CStringA strMessage = strTmp.Left( nIndex );
						strMessage.TrimLeft();

						if ( ! strMessage.IsEmpty() )
							OnNewMessage( TrimString( UTF8Decode( strMessage ) ) );

						strTmp = strTmp.Mid( nIndex + 2 );
						nIndex = strTmp.Find( "\r\n" );
					}

					if ( ! strTmp.IsEmpty() )
						m_sWsaBuffer = strTmp;
				}
			}
		}
	}
	CWnd::OnTimer( nIDEvent );
}

void CIRCFrame::SendString(const CString& strMessage)
{
	CStringA strEncoded = UTF8Encode( strMessage + _T("\r\n") );
	CNetwork::Send( m_nSocket, (LPCSTR)strEncoded, strEncoded.GetLength() );
}

void CIRCFrame::StatusMessage(LPCTSTR pszText, int nFlags)
{
	if ( ! *pszText )
		return;

	if ( Settings.IRC.Timestamp )
	{
		CString strMessage;
		CTime tNow = CTime::GetCurrentTime();
		strMessage.Format( _T("[%.2i:%.2i]  %s"),
			tNow.GetHour(), tNow.GetMinute(), pszText );
		OnStatusMessage( strMessage, nFlags );
		m_pIrcBuffer[ 0 ].Add( char( nFlags ) + strMessage );
	}
	else
	{
		OnStatusMessage( pszText, nFlags );
		m_pIrcBuffer[ 0 ].Add( char( nFlags ) + CString( pszText ) );
	}
}

void CIRCFrame::OnStatusMessage(LPCTSTR pszText, int nFlags)
{
	CString strMessage = pszText;
	COLORREF cRGB = Settings.IRC.Colors[ nFlags ];
	int nIndex = strMessage.Find( _T("\x03") ); //Find a color code indicator (Ctrl+K)
	int nSize;
	// Find color codes and remove them.  ToDo: Support Text Colors
	while ( nIndex != -1 )
	{
		nSize = 1;
		// If character (nIndex+1) is a digit nSize = 2
		if ( ( (int)( strMessage.GetAt( nIndex + 1 ) ) - 48 ) > -1 &&
			 ( (int)( strMessage.GetAt( nIndex + 1 ) ) - 48 < 10 ) )
		{
			nSize++;
			// If character (nIndex+2) is a digit = 3
			if ( ( (int)( strMessage.GetAt( nIndex + 2 ) ) - 48 ) > -1 &&
				 ( (int)( strMessage.GetAt( nIndex + 2 ) ) - 48 < 10 ) ) nSize++;

			if ( strMessage.GetAt( nIndex + 2 ) == _T(',') ||
				 strMessage.GetAt( nIndex + 3 ) == _T(',') )
			{
				nSize++;
				if ( ( (int)( strMessage.GetAt( nIndex + 4 ) ) - 48 ) > -1 &&
					 ( (int)( strMessage.GetAt( nIndex + 4 ) ) - 48 < 10 ) ) nSize++;

				if ( ( (int)( strMessage.GetAt( nIndex + 5 ) ) - 48 ) > -1 &&
					 ( (int)( strMessage.GetAt( nIndex + 5 ) ) - 48 < 10 ) ) nSize++;
			}
		}
		strMessage = strMessage.Mid( 0, nIndex ) + strMessage.Mid( nIndex + nSize + 1 );
		nIndex = strMessage.Find( _T("\x03") );
	}

	// Remove invalid characters (0x01 - 0x1F)
	for ( nIndex = 1 ; nIndex < 32 ; nIndex++ )
		strMessage.Remove( char(nIndex) );

	// Fit the text into the window -- UI libraries handle this automatically...

//	CRect rectView;
//	m_wndView.GetWindowRect( &rectView );
//	int nViewSize = int( rectView.Width() / 14 * 1.84 );
//	int nCurrentChar, nOldChar = 0, nCurrentLength, nCheckLength;
//	CString strMsgTemp, strCurrentWord;
//	CStringArray pWordDivide;
//	BOOL bStartedSplit = FALSE;
//	nCurrentChar = strMessage.Find( ' ', nOldChar );
//	strMessage.Trim();
//	while ( nCurrentChar != -1 )
//	{
//		pWordDivide.Add( strMessage.Mid( nOldChar, nCurrentChar - nOldChar ) );
//		nOldChar = nCurrentChar + 1;
//		nCurrentChar = strMessage.Find( ' ', nOldChar );
//	}
//	pWordDivide.Add( strMessage.Mid( nOldChar ) );
//	nCurrentLength = 0;
//	strMsgTemp.Empty();
//	for ( int nWord = 0 ; nWord < pWordDivide.GetCount() ; nWord++ )
//	{
//		strCurrentWord = pWordDivide.GetAt( nWord ).SpanExcluding( _T("\r\n") );
//		nCurrentLength += strCurrentWord.GetLength();
//		if ( nCurrentLength > nViewSize )
//		{
//			nCurrentLength = strMsgTemp.GetLength();
//			nCheckLength = nCurrentLength +
//			   int( strCurrentWord.GetLength() * 0.3 );
//			nOldChar = 0;
//			nCurrentChar = 0;
//			if ( strCurrentWord.GetLength() > nViewSize && bStartedSplit == TRUE )
//			{
//				bStartedSplit = FALSE;
//				while ( nViewSize < strCurrentWord.GetLength() - nOldChar )
//				{
//					strMsgTemp += strCurrentWord.Mid( nOldChar, nViewSize );
//					nOldChar += nViewSize;
//					strMsgTemp = _T('\x200D') + strMsgTemp;
//					m_pContent.Add( retText, strMsgTemp.GetBuffer(), NULL, retfColor )->m_cColor = cRGB;
//					m_pContent.Add( retNewline, NEWLINE_FORMAT );
//					strMsgTemp.Empty();
//				}
//				strMsgTemp = m_pszLineJoiner + strCurrentWord.Mid( nOldChar );
//				nCurrentLength = strMsgTemp.GetLength();
//			}
//			else if  ( nCheckLength <= nViewSize )
//			{
//				strMsgTemp += strCurrentWord.Left( nViewSize - nCurrentLength );
//				m_pContent.Add( retText, strMsgTemp.GetBuffer(), NULL, retfColor )->m_cColor = cRGB;
//				m_pContent.Add( retNewline, NEWLINE_FORMAT );
//				strMsgTemp.Empty();
//				pWordDivide.SetAt( nWord, m_pszLineJoiner + strCurrentWord.Mid( nViewSize - nCurrentLength ) );
//				nWord--;
//				nCurrentLength = 0;
//				bStartedSplit = TRUE;
//				// Add chars up to 30% and the rest to the next line.
//			}
//			else if ( nCheckLength > nViewSize ) // Move to the next line.
//			{
//				m_pContent.Add( retText, strMsgTemp.GetBuffer(), NULL, retfColor )->m_cColor = cRGB;
//				m_pContent.Add( retNewline, NEWLINE_FORMAT );
//				nWord--;
//				nCurrentLength = 0;
//				strMsgTemp.Empty();
//				bStartedSplit = TRUE;
//			}
//		}
//		else
//		{
//			strMsgTemp += pWordDivide.GetAt( nWord ) + " ";
//		}
//	}
//	// Display here what left.
//	pWordDivide.RemoveAll();
//	strMsgTemp.Trim();

//	if ( ! strMsgTemp.IsEmpty() )
//	{
//		m_pContent.Add( retText, strMsgTemp.GetBuffer(), NULL, retfColor )->m_cColor = cRGB;
//		m_pContent.Add( retNewline, NEWLINE_FORMAT );
//	}

	m_pContent.Add( retText, strMessage.GetBuffer(), NULL, retfColor )->m_cColor = cRGB;
	m_pContent.Add( retNewline, NEWLINE_FORMAT );

	m_wndView.InvalidateIfModified();
}

void CIRCFrame::ReloadViewText()
{
	LoadBufferForWindow( m_wndTab.GetCurSel() );
}

BOOL CIRCFrame::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_NOTIFY )
	{
		if ( pMsg->wParam == IDC_IRC_DBLCLKCHANNELS )
		{
			ChanListDblClick();
			return TRUE;
		}
		if ( pMsg->wParam == IDC_IRC_DBLCLKUSERS )
		{
			UserListDblClick();
			return TRUE;
		}
		if ( pMsg->wParam == IDC_IRC_MENUUSERS )
		{
			Skin.TrackPopupMenu( _T("CIRCUserList"), pMsg->pt );
			return TRUE;
		}
	}
	else if ( pMsg->message == WM_MOUSEMOVE )
	{
		// Workaround to clear channel-tab hovers
		if ( m_wndTab.m_nHoverTab > -1 )
			m_wndTab.PreTranslateMessage( pMsg );
	}
	else if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_RETURN && m_bConnected )
		{
			// Send textbox on Enter key
			OnIrcSendText();
			return TRUE;
		}

		if ( GetFocus() == &m_wndEdit ) 	// View textbox sent-message history
		{
			int nTab = m_wndTab.GetCurSel();

			switch ( pMsg->wParam )
			{
			// Clear text
			case VK_ESCAPE:
				m_nCurrentPosLineBuffer[ nTab ] = -1;
				m_wndEdit.SetWindowText( NULL );
				return TRUE;
			// Show previous message
			case VK_UP:
			case VK_PRIOR:
				if ( m_nCurrentPosLineBuffer[ nTab ] + 1 == m_pLastLineBuffer[ nTab ].GetCount() ) return TRUE;
				if ( m_nCurrentPosLineBuffer[ nTab ] == -1 ) m_wndEdit.GetWindowText( m_sTemp );
				m_nCurrentPosLineBuffer[ nTab ]++;
				m_wndEdit.SetWindowText( m_pLastLineBuffer[ nTab ].GetAt( m_nCurrentPosLineBuffer[ nTab ] ) );
				return TRUE;
			// Show next message (or current)
			case VK_DOWN:
			case VK_NEXT:
				if ( m_nCurrentPosLineBuffer[ nTab ] < 0 ) return TRUE;
				m_nCurrentPosLineBuffer[ nTab ]--;
				m_wndEdit.SetWindowText( ( m_nCurrentPosLineBuffer[ nTab ] == -1 ) ? m_sTemp :
					m_pLastLineBuffer[ nTab ].GetAt( m_nCurrentPosLineBuffer[ nTab ] ) );
				return TRUE;
			// Show current typed message
			case VK_END:
			case VK_HOME:
				if ( m_nCurrentPosLineBuffer[ nTab ] < 0 ) return TRUE;
				m_nCurrentPosLineBuffer[ nTab ] = -1;
				m_wndEdit.SetWindowText( m_sTemp );
				return TRUE;
			// Skip common control keys
			case VK_LEFT:
			case VK_RIGHT:
			case VK_SHIFT:
			case VK_CONTROL:
			case VK_CAPITAL:
			case VK_MENU:
			case VK_PRINT:
			case VK_LWIN:
			case VK_RWIN:
				break;
			// Assume character key changed history, so set current  	// ToDo: Use real change detection
			default:
				if ( pMsg->wParam < VK_F1 && m_nCurrentPosLineBuffer[ nTab ] >= 0 )
					m_nCurrentPosLineBuffer[ nTab ] = -1;
				// Pass key forward
			}
		}
		else if ( GetFocus() == &m_wndView )	// Keyscroll channel window
		{
			switch ( pMsg->wParam )
			{
			case VK_UP:
			case VK_LEFT:
				m_wndView.OnVScroll( SB_LINEUP, GetScrollPos( SB_VERT ) );
				return TRUE;
			case VK_DOWN:
			case VK_RIGHT:
				m_wndView.OnVScroll( SB_LINEDOWN, GetScrollPos( SB_VERT ) );
				return TRUE;
			case VK_PRIOR:
				m_wndView.OnVScroll( SB_PAGEUP, GetScrollPos( SB_VERT ) );
				return TRUE;
			case VK_NEXT:
				m_wndView.OnVScroll( SB_PAGEDOWN, GetScrollPos( SB_VERT ) );
				return TRUE;
			case VK_HOME:
				m_wndView.OnVScroll( SB_TOP, GetScrollPos( SB_VERT ) );
				return TRUE;
			case VK_END:
				m_wndView.OnVScroll( SB_BOTTOM, GetScrollPos( SB_VERT ) );
				return TRUE;
			}
		}
	}
	else if ( pMsg->wParam == IDC_IRC_CHANNELS )
	{
		if ( pMsg->message == WM_ADDCHANNEL )
		{
			CString strChan = m_wndPanel.m_boxChans.m_sPassedChannel;
			m_wndPanel.m_boxChans.m_sPassedChannel.Empty();
			if ( strChan.GetAt( 0 ) != '#' )
				strChan = _T("#") + strChan;
			m_pChanList.AddChannel( strChan, strChan, TRUE );
			OnIrcChanCmdSave();
			return TRUE;
		}
		else if ( pMsg->message == WM_REMOVECHANNEL )
		{
			int nItem = m_wndPanel.m_boxChans.m_wndChanList.GetNextItem( -1, LVNI_SELECTED );
			if ( nItem == -1 ) return TRUE;

			CString strItem = m_wndPanel.m_boxChans.m_wndChanList.GetItemText( nItem, 0 );
			if ( m_pChanList.GetType( strItem ) )
				m_wndPanel.m_boxChans.m_wndChanList.DeleteItem( nItem );
			OnIrcChanCmdSave();
			return TRUE;
		}
	}

	return CWnd::PreTranslateMessage( pMsg );
}

void CIRCFrame::ChanListDblClick()
{
	int nItem = m_wndPanel.m_boxChans.m_wndChanList.GetNextItem( -1, LVNI_SELECTED );

	if ( nItem < 0 ) return;

	CString strDisplay = m_wndPanel.m_boxChans.m_wndChanList.GetItemText( nItem, 0 );
	int nIndex = m_pChanList.GetIndexOfDisplay( strDisplay );
	if ( nIndex >= 0 )
	{
		CString strChannelName = m_pChanList.GetNameOfIndex( nIndex );
		int nTab = GetTabIndex( strChannelName );
		if ( nTab < 1 )
		{
			// Join it
			SendString( _T("JOIN ") + strChannelName );
		}
		else if ( nTab != m_wndTab.GetCurSel() )
		{
			// Show it
			m_wndTab.SetCurSel( nTab );
			TabClick();
		}
	}
}

BOOL CIRCFrame::OnNewMessage(const CString& strMessage)
{
	CIRCNewMessage oNewMessage;
	ParseString( strMessage, oNewMessage );

	for ( int nMessage = 0 ; nMessage < oNewMessage.m_pMessages.GetCount() ; nMessage++ )
	{
		int nColorID = oNewMessage.m_pMessages[ nMessage ].nColorID;
		int nTargetWindow = GetTabIndex( oNewMessage.m_pMessages[ nMessage ].sTargetName );
		if ( nTargetWindow != -1 )
		{
			CString strLine;
			if ( Settings.IRC.Timestamp )
			{
				CTime pNow = CTime::GetCurrentTime();
				strLine.Format( _T("[%.2i:%.2i] "), pNow.GetHour(), pNow.GetMinute() );
			}
			strLine += oNewMessage.m_pMessages[ nMessage ].sMessage;

			if ( m_wndTab.GetCurSel() == nTargetWindow )
				OnStatusMessage( strLine, nColorID );

			m_pIrcBuffer[ nTargetWindow ].Add( char( nColorID ) + strLine );
		}
	}

	//oNewMessage.m_pMessages.RemoveAll();
	return TRUE;
}

void CIRCFrame::HighlightTab(int nTab, BOOL bHighlight)
{
	//TCITEM it = {};
	//it.mask = TCIF_IMAGE;
	//it.iImage = bHighlight ? 0 : -1;
	//m_wndTab.SetItem( nTab, &it );

	TC_ITEM tci = { TCIF_PARAM };
	tci.lParam = bHighlight ? Settings.IRC.Colors[ ID_COLOR_NEWMSG ] : RGB(0,0,0);
	m_wndTab.SetItem( nTab, &tci );
	m_wndTab.RedrawWindow();
}

void CIRCFrame::ActivateMessageByID(CIRCNewMessage& oNewMessage, int nMessageType)
{
	switch ( nMessageType )
	{
		case NULL:
		{
			int nTab = GetTabIndex( m_sStatus );
			if ( nTab != -1 && nTab != m_wndTab.GetCurSel() )
				HighlightTab( nTab );
			oNewMessage.Add( GetStringAfterParsedItem( 0 ), m_sStatus, ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_SERVER_MSG:
		{
			int nTab = GetTabIndex( m_sStatus );
			if ( nTab != -1 && nTab != m_wndTab.GetCurSel() )
				HighlightTab( nTab );
			oNewMessage.Add( GetStringAfterParsedItem( 3 ), m_sStatus, ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_SERVER_ERROR:
		{
			int nTab = GetTabIndex( m_sStatus );
			if ( nTab != -1 && nTab != m_wndTab.GetCurSel() )
				HighlightTab( nTab );
			oNewMessage.Add( GetStringAfterParsedItem( 3 ), m_sStatus, ID_COLOR_SERVERERROR );
			return;
		}
		case ID_MESSAGE_IGNORE:
		{
		//	oNewMessage->m_sTargetName = m_sStatus;
			return;
		}
		case ID_MESSAGE_SERVER_PING:
		{
			SendString( _T("PONG ") + GetStringAfterParsedItem( 1 ) );
#ifdef _DEBUG
			oNewMessage.Add ( _T("Ping? Pong."), m_sStatus, ID_COLOR_SERVERMSG );
#endif
			return;
		}
		case ID_MESSAGE_SERVER_CONNECTED:
		{
			m_sNickname = m_pWords.GetAt( 2 );
			oNewMessage.Add( _T("Connection Established."), m_sStatus, ID_COLOR_NOTICE );
			OnIrcChanCmdOpen();
			OnLocalText( _T("/list *PeerProject*") );
			if ( Settings.General.Language != _T("en") )
				OnLocalText( _T("/join PeerProject-") + Settings.General.Language.Left(2) );
			OnLocalText( _T("/list *shareaza*") );
			return;
		}
		case ID_MESSAGE_USER_MESSAGE:
		{
			int nTab = AddTab( m_pWords.GetAt( 0 ), ID_KIND_PRIVATEMSG );
			if ( nTab == -1 )
				nTab = GetTabIndex( m_pWords.GetAt( 0 ) );
			if ( nTab != m_wndTab.GetCurSel() )
				HighlightTab( nTab );
			CString strSender = m_pWords.GetAt( 0 ) + _T(":  ");	// <NICK>
			CString strText = GetStringAfterParsedItem( 7 );

			// Desktop Message Window
			if ( CMainWnd* pWnd = theApp.CPeerProjectApp::SafeMainWnd() )
			{
				if ( ! pWnd->IsForegroundWindow() )
					pWnd->ShowTrayPopup( strText, m_pWords.GetAt( 0 ), NIIF_NONE, 10 );	// Seconds  ToDo: Make Setting?
			}

			oNewMessage.Add( strSender + GetStringAfterParsedItem( 7 ), m_pWords.GetAt( 0 ), ID_COLOR_TEXT );

			// Notify chat plugins about new remote message
			Plugins.OnChatMessage( GetTabText( nTab ), FALSE, m_pWords.GetAt( 0 ), m_sNickname, strText );

			return;
		}
		case ID_MESSAGE_USER_AWAY:
		{
			CString strSender = _T("*") + m_pWords.GetAt( 3 ) + _T(" is away:  ") + GetStringAfterParsedItem( 4 );
			oNewMessage.Add( strSender, GetTabText( m_wndTab.GetCurSel() ), ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_USER_KICK:
		{
			if ( m_pWords.GetAt( 7 ) != m_sNickname )
			{
				int nTab = GetTabIndex( m_pWords.GetAt( 6 ) );
				if ( nTab != -1 )
				{
					int nListUser = FindInList( m_pWords.GetAt( 7 ) );
					if ( nTab == m_wndTab.GetCurSel() && nListUser != -1 )
						DeleteUser( nListUser );
					nListUser = FindInList( m_pWords.GetAt( 7 ), 2, nTab );
					if ( nListUser != -1 )
						m_pIrcUsersBuffer[ nTab ].RemoveAt( nListUser );
					m_wndPanel.m_boxUsers.UpdateCaptionCount();
					FillChanListCount( _T("-1"), m_pWords.GetAt( 6 ) );
				}
				CString strSender = _T("*") + m_pWords.GetAt( 7 ) + _T(" was kicked by ") + m_pWords.GetAt( 0 )
					+ _T("  (") + GetStringAfterParsedItem( 8 ) + _T(")");
				oNewMessage.Add( strSender, GetTabText( m_wndTab.GetCurSel() ), ID_COLOR_SERVERMSG );
			}
			else
			{
				int nTab = GetTabIndex( m_pWords.GetAt( 6 ) );
				if ( nTab != -1 )
				{
					m_wndTab.SetCurSel( nTab );
					OnIrcCloseTab();
				}
				m_wndTab.SetCurSel( 0 );
				TabClick();
				CString strSender = _T("*You were kicked from ") + m_pWords.GetAt( 6 ) + _T(" by ") + m_pWords.GetAt( 0 ) + _T(" (") + GetStringAfterParsedItem( 8 ) + _T(")");
				oNewMessage.Add( strSender, m_sStatus, ID_COLOR_SERVERMSG );
			}
			return;
		}
		case ID_MESSAGE_SETAWAY:
		{
			oNewMessage.Add( _T("*You are now set as away"), GetTabText( m_wndTab.GetCurSel() ), ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_STOPAWAY:
		{
			oNewMessage.Add( _T("*You are back  (no longer set as away)"), GetTabText( m_wndTab.GetCurSel() ), ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_USER_ME:
		{
			int nTab = AddTab( m_pWords.GetAt( 0 ), ID_KIND_PRIVATEMSG );
			if ( nTab != m_wndTab.GetCurSel() )
				HighlightTab( nTab );
			CString strSender = _T("*") + m_pWords.GetAt( 0 ) + _T(" ");
			oNewMessage.Add( strSender + GetStringAfterParsedItem( 8 ), m_pWords.GetAt( 0 ), ID_COLOR_ME );
			return;
		}
		case ID_MESSAGE_USER_INVITE:
		{
			CString strSender = _T("*You have been invited to channel ") + m_pWords.GetAt( 8 ) + _T(" by ") + m_pWords.GetAt( 6 );
			oNewMessage.Add( strSender, GetTabText( m_wndTab.GetCurSel() ), ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_CHANNEL_MESSAGE:
		{
			int nTab = GetTabIndex( m_pWords.GetAt( 6 ) );
			if ( nTab == -1 ) return;
			if ( nTab != m_wndTab.GetCurSel() )
				HighlightTab( nTab );
			CString strSender = m_pWords.GetAt( 0 ) + _T(":  ");	// <NICK>
			CString strText = GetStringAfterParsedItem( 7 );		// Isolate for IChatPlugin capture
			oNewMessage.Add( strSender + strText, m_pWords.GetAt( 6 ), ID_COLOR_TEXT );

			// Notify chat plugins about new remote message
			Plugins.OnChatMessage( GetTabText( nTab ), FALSE, m_pWords.GetAt( 0 ), m_sNickname, strText );

			return;
		}
		case ID_MESSAGE_CHANNEL_ME:
		{
			int nTab = GetTabIndex( m_pWords.GetAt( 6 ) );
			if ( nTab == -1 ) return;
			if ( nTab != m_wndTab.GetCurSel() )
				HighlightTab( nTab );
			CString strSender = _T("*") + m_pWords.GetAt( 0 ) + _T(" ");
			oNewMessage.Add( strSender + GetStringAfterParsedItem( 8 ), m_pWords.GetAt( 6 ), ID_COLOR_ME );
			return;
		}

		case ID_MESSAGE_CHANNEL_NOTICE:
		{
			oNewMessage.Add( _T("-") + m_pWords.GetAt( 0 ) + _T(" - ") + GetStringAfterParsedItem ( 8 ), m_pWords.GetAt( 6 ), ID_COLOR_NOTICE );
			return;
		}
		case ID_MESSAGE_CLIENT_INVITE:
		{
			CString strSender = _T("*You have invited ") + m_pWords.GetAt( 3 ) + _T(" to channel ") + m_pWords.GetAt( 4 );
			oNewMessage.Add( strSender, GetTabText( m_wndTab.GetCurSel() ), ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_CLIENT_WHOWAS:
		{
			CString strSender = m_pWords.GetAt( 2 ) + _T(" was ") + m_pWords.GetAt( 5 ) + m_pWords.GetAt( 6 ) + GetStringAfterParsedItem( 7 );
			oNewMessage.Add( strSender, m_sStatus, ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_CLIENT_WHOIS:
		{
			CString strSender = GetStringAfterParsedItem( 3 );
			oNewMessage.Add( strSender, m_sStatus, ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_CLIENT_NOTICE:
		{
			int nTab = m_wndTab.GetCurSel();
			CString strChannelName;

			TCITEM item = { TCIF_IMAGE | TCIF_PARAM | TCIF_TEXT };
			item.pszText = strChannelName.GetBuffer( 1024 );
			item.cchTextMax = 1023;
			m_wndTab.GetItem( nTab, &item );
			strChannelName.ReleaseBuffer();

			oNewMessage.Add( _T("-") + m_pWords.GetAt( 0 ) + _T("- ") + GetStringAfterParsedItem ( 7 ), strChannelName, ID_COLOR_NOTICE );
			return;
		}
		case ID_MESSAGE_USER_CTCPTIME:
		{
			CTime time = CTime::GetCurrentTime();
			TIME_ZONE_INFORMATION tzi = {};
			int nTZBias;
			if ( GetTimeZoneInformation( &tzi ) == TIME_ZONE_ID_DAYLIGHT )
				nTZBias = tzi.Bias + tzi.DaylightBias;
			else
				nTZBias = tzi.Bias;
			CString strReply;
			strReply.Format( _T("/NOTICE %s :\x01TIME %s %+.2d%.2d\x01"),
				(LPCTSTR)m_pWords.GetAt( 0 ),
				(LPCTSTR)time.Format( _T("%Y-%m-%d %H:%M:%S") ),
				- nTZBias / 60, nTZBias % 60 );
			OnLocalText( strReply );
			oNewMessage.Add( _T("*") + m_pWords.GetAt( 0 ) + _T(" TIME'd you."), m_sStatus, ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_USER_CTCPVERSION:
		{
			CString strReply;
			strReply.Format( _T("/NOTICE %s :\x01VERSION %s:%s:Microsoft Windows %u.%u\x01"),
				(LPCTSTR)m_pWords.GetAt( 0 ), CLIENT_NAME,
				(LPCTSTR)theApp.m_sVersionLong, theApp.m_nWindowsVersion, theApp.m_nWindowsVersionMinor );
			OnLocalText( strReply );
			oNewMessage.Add( _T("*") + m_pWords.GetAt( 0 ) + _T(" VERSIONed you."), m_sStatus, ID_COLOR_SERVERMSG );
			return;
		}
		case ID_MESSAGE_USER_CTCPBROWSE:
		{
			if ( Settings.Community.ServeFiles )
			{
				CString strReply;
				strReply.Format( _T("/NOTICE %s :\x01USERINFO :Browse my files by double-clicking this link:  peerproject:browse:%s:%u\x01"),
					(LPCTSTR)m_pWords.GetAt( 0 ), (LPCTSTR)CString( inet_ntoa( Network.m_pHost.sin_addr ) ), htons( Network.m_pHost.sin_port ) );
				OnLocalText( strReply );
				oNewMessage.Add( _T("* ") + m_pWords.GetAt( 0 ) + _T(" just USERINFOed you."), m_sStatus, ID_COLOR_SERVERMSG );
			}
			return;
		}
		case ID_MESSAGE_SERVER_NOTICE:
		{
			oNewMessage.Add( GetStringAfterParsedItem ( FindParsedItem( _T(":"), 2 ) ) + _T(" (") + m_pWords.GetAt( 2 ) + _T(")"), m_sStatus, ID_COLOR_NOTICE );
			return;
		}
		case ID_MESSAGE_CHANNEL_LIST:
		{
			FillChanListCount( m_pWords.GetAt( 4 ), m_pWords.GetAt( 3 ) );
			return;
		}
		case ID_MESSAGE_CHANNEL_LISTEND:
		{
			CListCtrl* pChannelList = (CListCtrl*)&(m_wndPanel.m_boxChans.m_wndChanList);
			for ( int nList = 0 ; nList < pChannelList->GetItemCount() ; nList++ )
				if ( pChannelList->GetItemText( nList, 1 ) == _T("0") )
					pChannelList->DeleteItem( nList );
			return;
		}
		case ID_MESSAGE_CLIENT_JOIN_ENDNAMES:
		{
			m_wndPanel.m_boxUsers.UpdateCaptionCount();
			return;
		}
		case ID_MESSAGE_CLIENT_JOIN_USERLIST:
		{
			CString strChannelName = m_pWords.GetAt( 4 ), strTemp, nModeStr;
			int nMode, nWord, nModeColumn, nTab = m_wndTab.GetCurSel();
			for ( nWord = 6 ; nWord < m_pWords.GetCount() - 1 ; nWord++ )
			{
				strTemp = m_pWords.GetAt( nWord );
				nModeColumn = 0;
				nMode = 48;
				if ( strTemp.Left( 1 ) == _T("+") )
					nModeColumn = 1;
				else if ( strTemp.Left( 1 ) == _T("@") )
					nModeColumn = 2;
				else if ( strTemp.Left( 1 ) == _T("%") )
					nModeColumn = 4;
				nMode += nModeColumn;
				if ( nMode != 48 ) strTemp = strTemp.Mid( 1 );
				AddUser( m_pWords.GetAt( nWord ) );
				strTemp = char(nMode) + strTemp;
				//Add new user to userlist
				m_pIrcUsersBuffer[ nTab ].Add( strTemp );
			}
			m_wndPanel.m_boxUsers.UpdateCaptionCount();
			oNewMessage.Add( _T("*You have joined channel ") + strChannelName, strChannelName, ID_COLOR_CHANNELACTION );
			return;
		}
		case ID_MESSAGE_CHANNEL_TOPICSHOW:
		{
			CString strChannelName = m_pWords.GetAt( 3 );
			m_pContent.Clear();
			oNewMessage.Add( _T("*Topic is:  ") + GetStringAfterParsedItem ( 4 ), strChannelName, ID_COLOR_TOPIC );
			return;
		}
		case ID_MESSAGE_CHANNEL_TOPICSETBY:
		{
			CString strChannelName = m_pWords.GetAt( 3 );
			oNewMessage.Add( _T("*Topic set by ") + m_pWords.GetAt( 4 ), strChannelName, ID_COLOR_TOPIC );
			return;
		}
		case ID_MESSAGE_CHANNEL_PART:
		{
			CString strNick( m_pWords.GetAt( 0 ) ), strChannelName( m_pWords.GetAt( 6 ) );
			int nTab = GetTabIndex( strChannelName );
			if ( nTab != -1 )
			{
				int nListUser = FindInList( strNick );
				if ( nTab == m_wndTab.GetCurSel() && nListUser != -1 )
					DeleteUser( nListUser );
				nListUser = FindInList( strNick, 2, nTab );
				if ( nListUser != -1 )
					m_pIrcUsersBuffer[ nTab ].RemoveAt( nListUser );
				m_wndPanel.m_boxUsers.UpdateCaptionCount();
				FillChanListCount( _T("-1"), strChannelName );

				oNewMessage.Add( _T("*") + strNick + _T(" has left ") + strChannelName, strChannelName, ID_COLOR_CHANNELACTION );

				//ToDo: Merge consecutive joined/left lines
				//int nLine = m_pIrcBuffer[ nTab ].GetUpperBound();
				//CString strPrior = m_pIrcBuffer[ nTab ].GetAt( nLine );

				//if ( strPrior.Find( _T("*") + strNick + _T(" has joined") ) != 0 )
				//	oNewMessage.Add( _T("*") + strNick + _T(" has left ") + strChannelName, strChannelName, ID_COLOR_CHANNELACTION );
				//else
				//	oNewMessage.Add( _T("*") + strNick + _T(" then left..."), strChannelName, ID_COLOR_CHANNELACTION );
				//	m_pIrcBuffer[ nTab ].RemoveAt( nLine );
				//	oNewMessage.Add( strPrior + _T(" ...and left."), strChannelName, ID_COLOR_CHANNELACTION );
				//	oNewMessage.m_pMessages[ oNewMessage.m_pMessages.GetUpperBound() ].sMessage = strPrior + _T(" ...and left.");
			}
			return;
		}
		case ID_MESSAGE_CHANNEL_PART_FORCED:
		{
			CString strChannelName = m_pWords.GetAt( 6 );	// Get the channel we're being kicked from
			if ( strChannelName == m_sStatus ) return;		// The status window is not a channel
			int nTab = GetTabIndex( strChannelName );		// Get the tab number of the channel
			if( nTab < 0 ) return;							// Can't leave a channel we're not in, ignore

			// ToDo: Show kick notification in status window ?

			ClearUserList();
			m_pContent.Clear();

			FillChanListCount( _T("-1"), strChannelName );
			m_wndTab.DeleteItem( nTab );					// Remove the tab
			m_pIrcBuffer[ nTab ].RemoveAll();				// Delete the contents of the tab
			m_pIrcUsersBuffer[ nTab ].RemoveAll();

			int oldTab = nTab;
			//ToDo: m_pIrcBuffer should be an expandable array.  Seriously.
			for ( ; nTab < m_nBufferCount - 1 ; nTab++ )
			{
				m_pIrcBuffer[ nTab ].Append( m_pIrcBuffer[ nTab + 1 ] );
				m_pIrcUsersBuffer[ nTab ].Append( m_pIrcUsersBuffer[ nTab + 1 ] );
			}

			if ( strChannelName.Left( 1 ) == _T("#") )
				SendString( _T("PART ") + strChannelName );

			m_nBufferCount--;

			// Set the tab to the left as the active tab.
			if ( m_nSelectedTab != oldTab )
			{
				m_wndTab.SetCurSel( oldTab > 1 ? oldTab - 1 : 0 );
				TabClick();
			}

			return;
		}
		case ID_MESSAGE_CHANNEL_QUIT:
		{
			CString strNick = m_pWords.GetAt( 0 );
			CString strUserMsg = GetStringAfterParsedItem ( 6 );
			for ( int nTab = 1 ; nTab < m_nBufferCount ; nTab++ )
			{
				int nListUser = FindInList( strNick, 0, nTab );
				if ( nTab == m_wndTab.GetCurSel() && nListUser != -1 )
					DeleteUser( nListUser );

				CString strTabName = GetTabText( nTab );
				nListUser = FindInList( strNick, 2, nTab );

				if ( nListUser != -1 )
				{
					// Remove user from user list
					m_pIrcUsersBuffer[ nTab ].RemoveAt( nListUser );
					FillChanListCount( _T("-1"), strTabName );

					oNewMessage.Add( _T("*") + strNick + _T(" has quit:  ( ") + strUserMsg + _T(" )"), strTabName, ID_COLOR_CHANNELACTION );
				}
				else if ( strNick == strTabName )
				{
					oNewMessage.Add( _T("*") + strNick + _T(" has quit:  ( ") + strUserMsg + _T(" )"), strTabName, ID_COLOR_CHANNELACTION );
				}
			}
			m_wndPanel.m_boxUsers.UpdateCaptionCount();
			return;
		}
		case ID_MESSAGE_CLIENT_JOIN:
		{
			// Open New Channel Tab
			CString strChannelName = m_pWords.GetAt( 7 );
			int nTab = AddTab( strChannelName, ID_KIND_CHANNEL );
			if ( nTab == -1 ) return;
			m_wndTab.SetCurSel( nTab );
			ClearUserList();
			m_pIrcUsersBuffer[ nTab ].RemoveAll();
			m_pContent.Clear();
			FillChanListCount( _T("0"), strChannelName );
			return;
		}
		case ID_MESSAGE_CHANNEL_JOIN:
		{
			CString strNick( m_pWords.GetAt( 0 ) ), strChannelName( m_pWords.GetAt( 7 ) );
			int nTab = GetTabIndex( strChannelName );
			if ( nTab == -1 ) return;
			m_pIrcUsersBuffer[ nTab ].Add( char( 48 ) + strNick );
			if ( nTab == m_wndTab.GetCurSel() )
				AddUser( strNick );
			FillChanListCount( _T("0"), strChannelName );
			m_wndPanel.m_boxUsers.UpdateCaptionCount();
			oNewMessage.Add( _T("*") + strNick + _T(" has joined ") + strChannelName, strChannelName, ID_COLOR_CHANNELACTION );
			return;
		}
		case ID_MESSAGE_SERVER_DISCONNECT:
		{
			OnStatusMessage( GetStringAfterParsedItem( FindParsedItem( _T(":"), 2 ) ), ID_COLOR_SERVERMSG );
			OnIrcDisconnect();
			return;
		}
		case ID_MESSAGE_CHANNEL_SETMODE:
		{
			CString strMode = m_pWords.GetAt( 7 );
			BOOL bSign = ( strMode.Left( 1 ) == _T("+") );
			int nCurNick = 8;
			for ( int nChar = 1 ; nChar < strMode.GetLength() ; nChar++ )
			{
				if ( m_pWords.GetCount() - 1 < nCurNick ) break;

				if ( strMode.Mid( nChar, 1 ) == _T("+") ||
					 strMode.Mid( nChar, 1 ) == _T("-") )
				{
					bSign = ( strMode.Mid( nChar, 1 ) == _T("+") );
					nCurNick--;
				}
				else if ( strMode.Mid( nChar, 1 ) == _T("o") ||
						  strMode.Mid( nChar, 1 ) == _T("v") ||
						  strMode.Mid( nChar, 1 ) == _T("h") )
				{
					int nSign = 1;

					if ( strMode.Mid( nChar, 1 ) == _T("o") )
						nSign = 2;
					else if ( strMode.Mid( nChar, 1 ) == _T("h") )
						nSign = 4;

					int nInUserList = FindInList( m_pWords.GetAt( nCurNick ) );
					if ( nInUserList <= -1 ) return;
					int nTab = GetTabIndex( m_pWords.GetAt( 6 ) );
					int nInBufferList = FindInList( m_pWords.GetAt( nCurNick ), 2, nTab );
					if ( nInBufferList <= -1 ) return;

					CString strCurItem = m_pIrcUsersBuffer[nTab].GetAt( nInBufferList );
					int nMode = int(strCurItem.GetAt( 0 ) - 48);
					strCurItem = strCurItem.Mid( 1 );
					CString strCurUser( strCurItem );

					nMode = nMode + nSign * ( bSign ? 1 : -1 );
					if ( nMode == 2 || nMode == 3 || nMode == 6 || nMode == 7 )
						strCurUser = _T("@") + strCurUser;
					else if ( nMode == 1 )
						strCurUser = _T("+") + strCurUser;
					else if ( nMode == 4 || nMode == 5 )
						strCurUser = _T("%") + strCurUser;

					DeleteUser( nInUserList );
					AddUser( strCurUser );
					m_pIrcUsersBuffer[ nTab ].RemoveAt( nInBufferList );
					nMode += 48;
					m_pIrcUsersBuffer[ nTab ].Add( char(nMode) + strCurItem );
					nCurNick++;
				}
			}
			CString strMessage = m_pWords.GetAt( 0 );
			strMessage = strMessage + _T(" sets mode: ") + GetStringAfterParsedItem( 6 );
			m_wndPanel.m_boxUsers.UpdateCaptionCount();
			oNewMessage.Add( strMessage, m_pWords.GetAt( 6 ), ID_COLOR_CHANNELACTION );
			return;
		}
		case ID_MESSAGE_NICK:
		{
			CString strNick = m_pWords.GetAt( 0 );
			CString strChannelName = GetTabText();
			CString strTmp;
			CString strCurUser;
			int nListUser, nTab;
			for ( nTab = 0 ; nTab < m_wndTab.GetItemCount() ; nTab++ )
			{
				nListUser = FindInList( strNick, 2, nTab );
				if ( nListUser != -1 )
				{
					CString strCurItem = m_pIrcUsersBuffer[ nTab ].GetAt( nListUser );
					int nMode = int( strCurItem.GetAt( 0 ) - 48 );
					strCurItem = strCurItem.Mid( 1 );
					strCurUser = m_pWords.GetAt( 7 );
					if ( nMode == 2 || nMode == 3 || nMode == 6 || nMode == 7 )
						strCurUser = _T("@") + m_pWords.GetAt( 7 );
					else if ( nMode == 1 )
						strCurUser = _T("+") + m_pWords.GetAt( 7 );
					else if ( nMode == 4 || nMode == 5 )
						strCurUser = _T("%") + m_pWords.GetAt( 7 );
					nMode += 48;
					m_pIrcUsersBuffer[ nTab ].SetAt( nListUser, char(nMode) + m_pWords.GetAt( 7 ) );
					break;
				}
			}

			if ( strNick.CompareNoCase( m_sNickname ) == 0 )
				m_sNickname = m_pWords.GetAt( 7 );
			nListUser = FindInList( strNick );
			if ( nListUser != -1 )
			{
				DeleteUser( nListUser );
				AddUser( strCurUser );
				oNewMessage.Add( _T("*") + strNick + _T(" is now known as ") + m_pWords.GetAt( 7 ), strChannelName, ID_COLOR_CHANNELACTION );
			}
			return;
		}
	}
}

int CIRCFrame::ParseMessageID()
{
	int nWordCount = m_pWords.GetCount();
	int nMessageType = NULL;
	if ( nWordCount > 1 )
	{
		int nServerErrNum = _tstoi( (LPCTSTR)m_pWords.GetAt( 1 ) );

		switch ( nServerErrNum )
		{
		case 0:
			if ( m_pWords.GetAt( 0 ) == _T("PING") )
				nMessageType = ID_MESSAGE_SERVER_PING;
			else if ( m_pWords.GetAt( 0 ) == _T("ERROR") )
				nMessageType = ID_MESSAGE_SERVER_DISCONNECT;
			if ( m_pWords.GetAt( 1 ) == _T("NOTICE") )
				nMessageType = ID_MESSAGE_SERVER_NOTICE;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 372:
		case 375:
			nMessageType = ID_MESSAGE_SERVER_MSG;
			break;
		case 301:
			nMessageType = ID_MESSAGE_USER_AWAY;
			break;
		case 305:
			nMessageType = ID_MESSAGE_STOPAWAY;
			break;
		case 306:
			nMessageType = ID_MESSAGE_SETAWAY;
			break;
		case 322:
			nMessageType = ID_MESSAGE_CHANNEL_LIST;
			break;
		case 323:
			nMessageType = ID_MESSAGE_CHANNEL_LISTEND;
			break;
		case 332:
			nMessageType = ID_MESSAGE_CHANNEL_TOPICSHOW;
			break;
		case 333:
			nMessageType = ID_MESSAGE_CHANNEL_TOPICSETBY;
			break;
		case 353:
			nMessageType = ID_MESSAGE_CLIENT_JOIN_USERLIST;
			break;
		case 366:
			nMessageType = ID_MESSAGE_CLIENT_JOIN_ENDNAMES;
			break;
		case 376:
			nMessageType = ID_MESSAGE_SERVER_CONNECTED;
			break;
		case 341:
			nMessageType = ID_MESSAGE_CLIENT_INVITE;
			break;
		case 314:
			nMessageType = ID_MESSAGE_CLIENT_WHOWAS;
			break;
		case 311:
		case 313:
		case 378:
		case 312:
		case 319:
			nMessageType = ID_MESSAGE_CLIENT_WHOIS;
			break;
		case 5:
		case 300:
		case 302:
		case 303:
		case 317:
		case 318:
		case 321:
		case 331:
		case 342:
		case 351:
		case 369:
			nMessageType = ID_MESSAGE_IGNORE;
			break;
		case 400:
			nMessageType = ID_MESSAGE_SERVER_ERROR;
			break;
		default:
			if ( nServerErrNum > 250 && nServerErrNum < 270 )
				nMessageType = ID_MESSAGE_SERVER_MSG;
		}
	}
	if ( nWordCount > 6 )
	{
		const CString strCommand( m_pWords.GetAt( 5 ) );
		const CString strOrigin( m_pWords.GetAt( 0 ) );

		if ( strCommand == _T("JOIN") )
			nMessageType = strOrigin == m_sNickname ? ID_MESSAGE_CLIENT_JOIN : ID_MESSAGE_CHANNEL_JOIN;
		else if ( strCommand == _T("PART") )
			nMessageType = strOrigin == m_sNickname ? ID_MESSAGE_IGNORE : ID_MESSAGE_CHANNEL_PART;
		else if ( strCommand == _T("QUIT") && strOrigin != m_sNickname )
			nMessageType = ID_MESSAGE_CHANNEL_QUIT;
		else if ( strCommand == _T("MODE") )
			nMessageType = ID_MESSAGE_CHANNEL_SETMODE;
		else if ( strCommand == _T("NICK") )
			nMessageType = ID_MESSAGE_NICK;
		else if ( strCommand == _T("KICK") )
			nMessageType = ID_MESSAGE_USER_KICK;
	}
	if ( nWordCount > 8 )
	{
		const CString strCommand( m_pWords.GetAt( 5 ) );
		const CString strOrigin( m_pWords.GetAt( 0 ) );

		if ( strCommand == _T("INVITE") )
		{
			nMessageType = ID_MESSAGE_USER_INVITE;
		}
		else if ( strCommand == _T("NOTICE") && strOrigin != Settings.IRC.ServerName )
		{
			if ( m_pWords.GetAt( 6 ) == m_sNickname )
				nMessageType = ID_MESSAGE_CLIENT_NOTICE;
			else
				nMessageType = ID_MESSAGE_CHANNEL_NOTICE;
		}
		else if ( strCommand == _T("PRIVMSG") )
		{
			CString str = m_pWords.GetAt( 8 );
			char pszFirst = str.GetAt( 0 );
			str = str.Mid( 1, str.GetLength() - 2 ).MakeLower();
			// 0x01 indicates a CTCP message, including '/me'
			if ( pszFirst == char('\x01') )
			{
				if( m_pWords.GetAt( 6 ).CompareNoCase( m_sNickname ) == 0 )
				{
					if ( str == _T("version") )
						nMessageType = ID_MESSAGE_USER_CTCPVERSION;
					else if ( str == _T("time") )
						nMessageType = ID_MESSAGE_USER_CTCPTIME;
					else if ( str == _T("userinfo") )
						nMessageType = ID_MESSAGE_USER_CTCPBROWSE;
					else if ( str == _T("action") || str == "actio" )
						nMessageType = ID_MESSAGE_USER_ME;
				}
				else if ( str == _T("action") || str == "actio" )
				{
					nMessageType = ID_MESSAGE_CHANNEL_ME;
				}
			}
			else
			{
				nMessageType = m_pWords.GetAt( 6 ) == m_sNickname ?
					ID_MESSAGE_USER_MESSAGE : ID_MESSAGE_CHANNEL_MESSAGE;
			}
		}
	}

	return nMessageType;
}

void CIRCFrame::ParseString(const CString& strMessage, CIRCNewMessage& oNewMessage)
{
 	CString str, strWord;
	CStringArray incomingWords;
	m_pWords.RemoveAll();

	int nPos = strMessage.Find( _T(' ') );
	int nOldPos = strMessage.GetAt( 0 ) == _T(':') ? 0 : -1;

	// Tokens in user ID: nick!nick@domain.com :
	int nFirstToken, nSecondToken, nThirdToken = strMessage.ReverseFind( _T(':') );
	if ( nThirdToken == 0 ) nThirdToken = strMessage.GetLength() - 1;

	while ( nPos != -2 )
	{
		if ( nPos == -1 )
			str = strMessage.Mid( nOldPos + 1 );
		else
			str = strMessage.Mid( nOldPos + 1, nPos - nOldPos - 1 );

		nFirstToken  = str.Find( _T('!'), 1 );
		nSecondToken = str.Find( _T('@'), 1 );

		if ( str.GetAt( 0 ) == _T(':') && nOldPos <= nThirdToken )
		{
			incomingWords.Add( _T(":") );
			str = str.Mid( 1 );
		}

		if ( nFirstToken != -1 && nFirstToken < nSecondToken &&
			( nSecondToken < nThirdToken || nThirdToken == -1 ) && nOldPos <= nThirdToken )
		{
			strWord = str.Mid( 0, nFirstToken );
			incomingWords.Add( strWord );
			incomingWords.Add( _T("!") );
			strWord = str.Mid( nFirstToken, nSecondToken - nFirstToken - 1 );
			incomingWords.Add( strWord );
			incomingWords.Add( _T("@") );
			strWord = str.Mid( nSecondToken );
			incomingWords.Add( strWord );
		}
		else
			incomingWords.Add( str );

		if ( nPos == -1 ) break;

		nOldPos = nPos;
		nPos = strMessage.Find( _T(' '), nPos + 1 );
	}

	for( int index = 0; index < incomingWords.GetCount(); index++ )
	{
		m_pWords.Add( incomingWords.GetAt( index ) );
	}

	int nMessageID = ParseMessageID();
	ActivateMessageByID( oNewMessage, nMessageID );

	if ( m_bFloodProtectionRunning && nMessageID > 203 )
		oNewMessage.m_pMessages.RemoveAll();
}

int CIRCFrame::FindParsedItem(LPCTSTR szMessage, int nFirst)
{
	for ( int nItem = nFirst ; nItem < m_pWords.GetCount() - 1 ; nItem++ )
	{
		if ( m_pWords.GetAt( nItem ).Compare( szMessage ) == 0 )
			return nItem;
	}
	return -1;
}

CString CIRCFrame::GetStringAfterParsedItem(int nItem) const
{
	CString strMessage = _T("");
	for ( int nWord = nItem + 1 ; nWord < m_pWords.GetCount() ; nWord++ )
		strMessage = strMessage + _T(" ") + m_pWords.GetAt( nWord );
	strMessage.Trim();
	return strMessage;
}

CString CIRCFrame::TrimString(CString strMessage) const
{
	// If "\x000A" exists as the first character, remove it from the string.
	if ( strMessage.GetAt( 0 ) == _T('\x000A') )
		strMessage = strMessage.Mid( 1 );

	// If character is \x05BC stop adding characters
	int nPos = strMessage.Find( _T('\x05BC') );

	return ( nPos == -1 ) ? strMessage : strMessage.Left( nPos );
}

int CIRCFrame::GetTabIndex(const CString& strTabName) const
{
	for ( int nTab = 0 ; nTab < m_wndTab.GetItemCount() ; nTab++ )
	{
		if ( GetTabText( nTab ).CompareNoCase( strTabName ) == 0 )
			return nTab;
	}

	return -1;
}

void CIRCFrame::LoadBufferForWindow(int nTab)
{
	if ( nTab == -1 ) return;
	m_pContent.Clear();
	CString str;
	int nFlag;
	for ( int nLine = 1 ; nLine < m_pIrcBuffer[ nTab ].GetCount() ; nLine++ )
	{
		str = m_pIrcBuffer[ nTab ].GetAt( nLine );
		nFlag = int( CHAR( str.GetAt( 0 ) ) );
		str = str.Mid( 1 );
		OnStatusMessage( str, nFlag );
		m_wndView.InvalidateIfModified();
	}
}

void CIRCFrame::OnClickTab(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;
	TabClick();
}

//void CIRCFrame::SortUserList()
//{
//	// Why? User list is sorted automatically...
//
//	int nCmpResult, nBiggest, nUser1 , nUser2;
//	CString strUser1, strUser2;
//	for ( nUser1 = 0 ; nUser1 < m_wndPanel.m_boxUsers.m_wndUserList.GetCount() ; nUser1++ )
//	{
//		nBiggest = nUser1;
//		for ( nUser2 = nUser1 + 1 ; nUser2 < m_wndPanel.m_boxUsers.m_wndUserList.GetCount() ; nUser2++ )
//		{
//			m_wndPanel.m_boxUsers.m_wndUserList.GetText( nBiggest, strUser1 );
//			m_wndPanel.m_boxUsers.m_wndUserList.GetText( nUser2, strUser2 );
//			nCmpResult = CompareUsers( strUser1, strUser2 );
//			if ( nCmpResult > 0 ) nBiggest = nUser2;
//		}
//		if ( nBiggest != nUser1 )
//		{
//			m_wndPanel.m_boxUsers.m_wndUserList.GetText( nBiggest, strUser1 );
//			m_wndPanel.m_boxUsers.m_wndUserList.GetText( nUser1, strUser2 );
//			m_wndPanel.m_boxUsers.m_wndUserList.DeleteString( nUser1 );
//			m_wndPanel.m_boxUsers.m_wndUserList.InsertString( nUser1, strUser1.GetBuffer() );
//			m_wndPanel.m_boxUsers.m_wndUserList.DeleteString( nBiggest );
//			m_wndPanel.m_boxUsers.m_wndUserList.InsertString( nBiggest, strUser2.GetBuffer() );
//		}
//	}
//	// Update caption
//	m_wndPanel.m_boxUsers.UpdateCaptionCount();
//}

//int CIRCFrame::CompareUsers(const CString& str1, const CString& str2) const
//{
//	int nModeColumn1 = 0, nModeColumn2 = 0;
//	CString strUser1 = str1, strUser2 = str2;
//
//	switch ( strUser1.GetAt( 0 ) )
//	{
//		case _T('+'): nModeColumn1 = 1; break;
//		case _T('%'): nModeColumn1 = 2; break;
//		case _T('@'): nModeColumn1 = 3;
//	}
//	switch ( strUser2.GetAt( 0 ) )
//	{
//		case _T('+'): nModeColumn2 = 1; break;
//		case _T('%'): nModeColumn2 = 2; break;
//		case _T('@'): nModeColumn2 = 3;
//	}
//	if ( nModeColumn1 == nModeColumn2 )
//	{
//		if ( nModeColumn1 != 0 ) strUser1 = strUser1.Mid( 1 );
//		if ( nModeColumn2 != 0 ) strUser2 = strUser2.Mid( 1 );
//		return strUser1.CompareNoCase( strUser2 );
//	}
//	return ( nModeColumn2 - nModeColumn1 );
//}

void CIRCFrame::TabClick()
{
	m_pContent.Clear();
	m_wndView.InvalidateIfModified();
	ClearUserList();

	CString str;
	int nTab = m_wndTab.GetCurSel(), nMode;
	HighlightTab( nTab, FALSE );

	for ( int nUser = 0 ; nUser < m_pIrcUsersBuffer[ nTab ].GetCount() ; nUser++ )
	{
		str = m_pIrcUsersBuffer[ nTab ].GetAt( nUser );
		nMode = int( str.GetAt( 0 ) ) - 48;
		if ( nMode == 0 )
			str = str.Mid( 1 );
		else if ( nMode == 2 || nMode == 3 || nMode == 6 || nMode == 7 )
			str = _T("@") + str.Mid( 1 );
		else if ( nMode == 1 )
			str = _T("+") + str.Mid( 1 );
		else if ( nMode == 4 || nMode == 5 )
			str = _T("%") + str.Mid( 1 );
		AddUser( str );
	}

	m_wndPanel.m_boxUsers.UpdateCaptionCount();
	LoadBufferForWindow( nTab );

	RedrawWindow();
}

int CIRCFrame::AddTab(CString strTabName, int nKindOfTab)
{
	if ( m_wndTab.GetItemCount() == MAX_CHANNELS )
	{
		OnStatusMessage( _T("Reached maximum number of open channels."), ID_COLOR_NOTICE );
		return -1;
	}

	int nTab = GetTabIndex( strTabName );
	if ( nTab > -1 ) return -1;

	nTab = m_wndTab.InsertItem( TCIF_TEXT | ( Settings.General.LanguageRTL ? TCIF_RTLREADING : 0 ),
		m_nBufferCount, strTabName, NULL, NULL );
	if ( nTab < 0 ) return -1;

	m_pIrcBuffer[ m_nBufferCount ].RemoveAll();
	m_pIrcUsersBuffer[ m_nBufferCount ].RemoveAll();
	m_pIrcBuffer[ m_nBufferCount ].Add( CString( (char)nKindOfTab ) );
	m_nCurrentPosLineBuffer[ m_nBufferCount ] = -1;
	m_nBufferCount++;
	HighlightTab( nTab, FALSE );

	return nTab;
}

// Events

void CIRCFrame::OnRichCursorMove(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	AFX_NOTIFY* pNotify = (AFX_NOTIFY*)pResult;
	CString strText = GetTextFromRichPoint();
	if ( strText.IsEmpty() ) return;

	if ( IsUserInList( strText ) > -1 )
	{
		pNotify->pResult = (LRESULT*)theApp.LoadCursor( IDC_HAND );
	}
	else if ( strText.Left( 1 ) == _T("#") && strText.GetLength() > 3 &&
		strText.CompareNoCase( GetTabText() ) != 0 )
	{
		pNotify->pResult = (LRESULT*)theApp.LoadCursor( IDC_HAND );
	}
	else if ( strText.GetLength() > 12 && (
		strText.Left( 4 ) == _T("www.") ||
		strText.Left( 7 ) == _T("http://") ||
		strText.Left( 8 ) == _T("https://") ||
		strText.Left( 7 ) == _T("magnet:") ||
		strText.Left( 4 ) == _T("ed2k:") ||
		strText.Left( 7 ) == _T("HTTP://") ||
		strText.Left( 4 ) == _T("WWW.") ||
		strText.Left( 7 ) == _T("MAGNET:") ||
		strText.Left( 4 ) == _T("ED2K:") ||
		strText.Mid( 1, 7 ) == _T("http://") ||
		strText.Mid( 1, 4 ) == _T("www.") ) )
	{
		pNotify->pResult = (LRESULT*)theApp.LoadCursor( IDC_HAND );
	}
	else
	{
		pNotify->pResult = NULL;
	}
}

void CIRCFrame::OnRichDblClk(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	CString strText = GetTextFromRichPoint();
	if ( strText.IsEmpty() ) return;
	int nIndex = IsUserInList( strText );

	if ( nIndex > -1 )
	{
		SetSelectedUser( nIndex );
		UserListDblClick();
	}
	else if ( strText.Left( 1 ) == _T("#") && strText.GetLength() > 3 &&
		strText.Compare( GetTabText() ) != 0 )
	{
		int nTab = GetTabIndex( strText );
		if ( nTab > 0 )
		{
			m_wndTab.SetCurSel( nTab );
		}
		else
		{
			CString strJoinChan = _T("JOIN ") + strText;
			SendString( strJoinChan );
		}
	}
	else if ( strText.GetLength() < 12 )
	{
		// Too short for valid URL
	}
	else if ( strText.Left( 7 ) == _T("http://") ||
		strText.Left( 8 ) == _T("https://") ||
		strText.Left( 4 ) == _T("www.") ||
		strText.Left( 7 ) == _T("HTTP://") ||
		strText.Left( 4 ) == _T("WWW.") ||
		strText.Mid( 1, 7 ) == _T("http://") ||
		strText.Mid( 1, 4 ) == _T("www.") )
	{
		if ( strText.Mid( 1, 7 ) == _T("http://") || strText.Mid( 1, 4 ) == _T("www.") )
			strText = strText.Mid( 1 );
		if ( strText.Left( 4 ) == _T("www.") || strText.Left( 4 ) == _T("WWW.") )
			strText = _T("http://") + strText;

		theApp.InternalURI(	strText );
	}
	else if ( strText.Left( 7 ) == _T("magnet:") ||
		strText.Left( 4 ) == _T("ed2k:") ||
		strText.Left( 7 ) == _T("MAGNET:") ||
		strText.Left( 4 ) == _T("ED2K:") ||
		strText.Mid( 1, 7 ) == _T("magnet:") ||
		strText.Mid( 1, 4 ) == _T("ed2k:") )
	{
		if ( strText.Mid( 1, 7 ) == _T("magnet:") || strText.Mid( 1, 4 ) == _T("ed2k:") )
			strText = strText.Mid( 1 );

		theApp.InternalURI(	strText );
	}
	*pResult = 0;
}

void CIRCFrame::OnRichClk(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	CString strText = GetTextFromRichPoint();
	if ( strText.IsEmpty() ) return;
	int nIndex = IsUserInList( strText );
	if ( nIndex != -1 )
	{
		SetSelectedUser( nIndex );
		m_wndPanel.m_boxUsers.m_wndUserList.PostMessage( WM_LBUTTONUP );
	}
	*pResult = 0;
}

// Operations

CString CIRCFrame::GetTextFromRichPoint() const
{
	CPoint point;
	GetCursorPos( &point );

	CString strTemp = m_wndView.GetWordFromPoint( point, _T(" \r\n\t�<>[](){}`\'\"!,;*") );

// Obsolete:
//	{
//		CRect rc, rc2;
//		m_wndView.GetWindowRect( &rc2 );
//		ScreenToClient( &rc2 );
//		rc.left = point.x;
//		rc.top = point.y;
//		ScreenToClient( &rc );
//		point.x = rc.left - rc2.left;
//		point.y = rc.top - rc2.top;
//	}
//	// ToDo: Fix wrapped multiline detection
//	RICHPOSITION rp = m_wndView.PointToPosition( point );
//
//	if ( rp.nFragment < 0 )	return "";
//	CRichFragment* pFragment = (CRichFragment*)m_wndView.m_pFragments.GetAt( rp.nFragment );
//	if ( rp.nOffset == 0 ) return "";
//	CString strText = pFragment->m_pElement->m_sText;
//	CString strTemp = strText.Mid( 0, rp.nOffset );
//	if ( strText.IsEmpty() ) return "";
//	if ( strText.Mid( rp.nOffset, 1 ) == _T(" ") ) return "";
//	int nLength  = strText.GetLength();
//	int nStart   = strTemp.ReverseFind( _T(' ') );
//	int nEnd     = strText.Find( _T(' '), rp.nOffset );
//	INT_PTR nFragCnt = m_wndView.m_pFragments.GetCount();
//	strTemp = strTemp.Mid( nStart == -1 ? 0 : nStart + 1 );
//	if ( nEnd == -1 )
//		strTemp += strText.Mid( rp.nOffset );
//	else
//		strTemp += strText.Mid( rp.nOffset, nEnd - rp.nOffset );
//	int nFrag = rp.nFragment - 1;
//	while ( nStart == -1 && nFrag != -1 )
//	{
//		if ( strTemp[ 0 ] != _T('\x200D') ) break;
//		pFragment = (CRichFragment*)m_wndView.m_pFragments.GetAt( nFrag );
//		strText = pFragment->m_pElement->m_sText;
//		if ( strText.IsEmpty() ) break;
//		strTemp = strTemp.Mid( 1 );
//		nStart  = strText.ReverseFind( _T(' ') );
//		strTemp = strText.Mid( nStart == -1 ? 0 : nStart + 1 ) + strTemp;
//		nFrag--;
//	}
//	nFrag = rp.nFragment + 1;
//	while ( ( nEnd == -1 || nEnd == nLength - 1 ) && nFragCnt != nFrag )
//	{
//		pFragment = (CRichFragment*)m_wndView.m_pFragments.GetAt( nFrag );
//		strText = pFragment->m_pElement->m_sText;
//		if ( strText.IsEmpty() ) break;
//		if ( strText[ 0 ] != _T('\x200D') ) break;
//		strText = strText.Mid( 1 );
//		nEnd = strText.Find( _T(' ') );
//		strTemp += strText.Mid( nEnd == -1 ? 0 : nEnd );
//		nFrag++;
//	}
//	if ( strTemp.IsEmpty() ) return "";
//
//	//strTemp.TrimRight( _T(":") );
//	//strTemp.TrimLeft( _T("*") );
//
//	//if ( strTemp.Left( 1 ) == _T("-") && strTemp.Right( 1 ) == _T("-") )
//	//	strTemp = strTemp.Mid( 1, strTemp.GetLength() - 2 );
//
//	if ( strTemp.Left( 1 ) == _T("*") || strTemp.Left( 1 ) == _T("(") )
//		strTemp = strTemp.Mid( 1 );

	if ( strTemp.Right( 1 ) == _T(":") )
		strTemp = strTemp.Left( strTemp.GetLength() - 1 );

	return strTemp;
}

void CIRCFrame::UserListDblClick()
{
	CString strQueryUser = GetSelectedUser();
	if ( strQueryUser.IsEmpty() ) return;

	strQueryUser = RemoveModeOfNick( strQueryUser );
	int nTab = GetTabIndex( strQueryUser );
	if ( nTab == -1 )
		nTab = AddTab( strQueryUser, ID_KIND_PRIVATEMSG );
	else if ( nTab != m_wndTab.GetCurSel() )
		m_wndTab.SetCurSel( nTab );
	TabClick();
}

//static const wchar_t IrcModeChars[] = { '@', '%', '+', '~', '&' };

CString CIRCFrame::RemoveModeOfNick(CString strNick) const
{
	return strNick.TrimLeft( _T("@%+~&") );
}

int CIRCFrame::IsUserInList(CString strUser) const
{
	strUser = RemoveModeOfNick( strUser );

	int nCount = GetUserCount() - 1;
	for ( int nUser = 0 ; nUser <= nCount ; nUser++ )
	{
		CString strNick = RemoveModeOfNick( GetUser( nUser ) );
		if ( strNick.CompareNoCase( strUser ) == 0 )
			return nUser;
	}
	return -1;
}

int CIRCFrame::FindInList(CString strName, int nList, int nTab)
{
	strName = RemoveModeOfNick( strName );

	INT_PTR nListCount = 0;
	if ( nList == 0 ) nListCount = GetUserCount();
	if ( nList == 1 ) nListCount = m_wndPanel.m_boxChans.m_wndChanList.GetItemCount();
	if ( nList == 2 ) nListCount = m_pIrcUsersBuffer[ nTab ].GetCount();

	for ( int nItem = 0 ; nItem < nListCount ; nItem++ )
	{
		CString strNick;
		if ( nList == 0 ) strNick = GetUser( nItem );
		if ( nList == 1 ) strNick = m_wndPanel.m_boxChans.m_wndChanList.GetItemText( nItem, 1 );
		if ( nList == 2 ) strNick = m_pIrcUsersBuffer[ nTab ].GetAt( nItem ).Mid( 1 );

		if ( RemoveModeOfNick( strNick ).CompareNoCase( strName ) == 0 )
			return nItem;
	}

	return -1;
}

// Obsolete: Use WndMain Instead
//BOOL CIRCFrame::ShowTrayPopup(LPCTSTR szText, LPCTSTR szTitle, DWORD dwIcon, UINT uTimeout)
//{
//	BOOL bMinimized = ( (CMainWnd*)AfxGetMainWnd() )->m_bTrayHide;
//	if ( ! bMinimized ) return FALSE;

//	//OSVERSIONINFO pVersion;
//	//pVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//	//GetVersionEx( &pVersion );
//	//if ( pVersion.dwMajorVersion < 5 )
//	//	return FALSE; // Do not support Win9x/Me/NT

//	// Verify input parameters
//	// Balloon tooltip text can be up to 255 chars long.
//	CString strText( szText );
//	if ( strText.GetLength() > 255 )
//	{
//		if ( strText.GetAt( 256 ) == ' ' )
//			strText = strText.Left( 255 );
//		else
//		{
//			if ( strText.GetAt( 255 ) == ' ' )
//				strText = strText.Left( 254 ) + _T("\x2026");
//			else
//			{
//				strText = strText.Left( 254 );
//				int nLastWord = strText.ReverseFind( ' ' );
//				strText = strText.Left( nLastWord ) + _T("\x2026");
//			}
//		}
//	}
//	// Balloon title text can be up to 63 chars long.
//	CString strTitle;
//	if ( szTitle )
//	{
//		strTitle.SetString( szTitle, 63 );
//		strTitle.ReleaseBuffer( 63 );
//	}
//	// dwBalloonIcon must be valid.
//	if ( NIIF_NONE		!= dwIcon && NIIF_INFO	!= dwIcon &&
//		NIIF_WARNING	!= dwIcon && NIIF_ERROR	!= dwIcon ) return FALSE;
//	// Timeout must be between 10 and 30 seconds.
//	if ( uTimeout < 10 || uTimeout > 30 ) return FALSE;
//
//	m_pTray.uFlags = NIF_INFO;
//	_tcsncpy( m_pTray.szInfo, strText.GetBuffer(), 256 );
//	if ( szTitle )
//		_tcsncpy( m_pTray.szInfoTitle, strTitle.GetBuffer(), 64 );
//	else
//		m_pTray.szInfoTitle[0] = _T('\0');
//	m_pTray.dwInfoFlags = dwIcon;
//	m_pTray.uTimeout = uTimeout * 1000;   // convert time to ms
//	m_pTray.szInfo[0] = _T('\0');
//
//	BOOL bSuccess = Shell_NotifyIcon( NIM_MODIFY, &m_pTray );
//	return bSuccess;
//}


/////////////////////////////////////////////////////////////////////////////
// CIRCTabCtrl construction

BEGIN_MESSAGE_MAP(CIRCTabCtrl, CTabCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CIRCTabCtrl::CIRCTabCtrl()
	: m_hTheme( NULL )
{
}

CIRCTabCtrl::~CIRCTabCtrl()
{
	if ( m_hTheme && theApp.m_pfnCloseThemeData )
		theApp.m_pfnCloseThemeData( m_hTheme );
}

int CIRCTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CTabCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	if ( theApp.m_pfnOpenThemeData )
		m_hTheme = theApp.m_pfnOpenThemeData( m_hWnd, _T("Tab") );

	return 0;
}

//BOOL CIRCTabCtrl::OnEraseBkgnd(CDC* pDC)
//{
//	CRect rect;
//	COLORREF m_cBorder = Settings.IRC.Colors[ ID_COLOR_TABS ];
//	CBrush brBorder = m_cBorder;
//	pDC->GetWindow()->GetWindowRect( &rect );
//	pDC->GetWindow()->ScreenToClient( &rect );
//	pDC->SetBkMode( OPAQUE );
//	pDC->SetBkColor( m_cBorder );
//	pDC->FillRect( &rect, &brBorder );
//	return TRUE;
//}

HRESULT CIRCTabCtrl::DrawThemesPart(HDC dc, int nPartID, int nStateID, LPRECT prcBox)
{
	HRESULT hr = E_FAIL;
	if ( m_hTheme != NULL &&
		theApp.m_pfnIsThemeActive && theApp.m_pfnIsThemeActive() &&
		theApp.m_pfnDrawThemeBackground )
	{
		hr = theApp.m_pfnDrawThemeBackground( m_hTheme, dc, nPartID, nStateID, prcBox, NULL );
	}
	return hr;
}

void CIRCTabCtrl::DrawTabThemed(HDC dc, int nItem, const RECT& rcItem, UINT flags)
{
	if ( m_hTheme == NULL ||
		 theApp.m_pfnIsThemeActive == NULL || ! theApp.m_pfnIsThemeActive() ||
		 theApp.m_pfnDrawThemeBackground == NULL )
	{
		COLORREF crBack = Colors.m_crBackNormal;
		COLORREF crBorder = Colors.m_crSysBtnFace;
		COLORREF crShadow = Colors.m_crSys3DShadow;
		CBrush brBack = crBack;
		CBrush brBorder = crBorder;
		CBrush brShadow = crShadow;
		int oldMode = SetBkMode( dc, OPAQUE );
		SetBkColor( dc, crBorder );
		RECT rc = rcItem;
		FillRect( dc, &rc, (HBRUSH)brBack.m_hObject );
		rc.left += 2;
		rc.top += 2;
		FillRect( dc, &rc, (HBRUSH)brShadow.m_hObject );
		rc.right -= 2;
		FillRect( dc, &rc, (HBRUSH)brBorder.m_hObject );

		TC_ITEM tciText = { TCIF_PARAM };
		GetItem( nItem, &tciText );
		SetTextColor( dc, tciText.lParam ); 	// COLORREF Black/Highlight (ID_COLOR_NEWMSG)

		TC_ITEM tci = { TCIF_TEXT | TCIF_IMAGE };
		TCHAR pszBuffer[ 128 + 4 ] = {};
		tci.pszText = pszBuffer;
		tci.cchTextMax = 127;
		if ( ! TabCtrl_GetItem( m_hWnd, nItem, &tci ) ) return;
		HFONT oldFont = (HFONT)SelectObject( dc, (HFONT)SendMessage( WM_GETFONT, 0, 0 ) );
		DrawText( dc, pszBuffer, (int)_tcslen( pszBuffer ), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER );
		SelectObject( dc, oldFont );
		SetBkMode( dc, oldMode );
		return;
	}

	int nWidth = rcItem.right - rcItem.left;
	int nHeight = rcItem.bottom - rcItem.top;

	// Draw the background
	HDC dcMem = CreateCompatibleDC( dc );
	HBITMAP bmpMem = CreateCompatibleBitmap( dc, nWidth, nHeight );
	ASSERT( dcMem != NULL );
	ASSERT( bmpMem != NULL );

	HBITMAP oldBmp = (HBITMAP)SelectObject( dcMem, bmpMem );
	RECT rcMem;
	SetRect( &rcMem, 0, 0, nWidth, nHeight );

	// TABP_PANE = 9, 0, "TAB"
	// TABP_TABITEM = 1, TIS_SELECTED = 3 : TIS_HOT = 2 : TIS_NORMAL = 1, "TAB"
	if ( flags & paintBody )
	{
		rcMem.bottom += 3;	// Hide lower line
		DrawThemesPart( dcMem, 9, 0, &rcMem );
		rcMem.bottom -= 3;
	}
	else if ( flags & paintSelected )
	{
		rcMem.bottom++;
		DrawThemesPart( dcMem, 1, TIS_SELECTED, &rcMem );
		rcMem.bottom--;
	}
	else if ( flags & paintHotTrack )
	{
		DrawThemesPart( dcMem, 1, TIS_HOT, &rcMem );
	}
	else
	{
		DrawThemesPart( dcMem, 1, TIS_NORMAL, &rcMem );
	}

	BITMAPINFO bmiOut = {};
	BITMAPINFOHEADER& bmihOut = bmiOut.bmiHeader;
	//ZeroMemory( &bmiOut, sizeof( BITMAPINFO ) );
	bmihOut.biSize = sizeof( BITMAPINFOHEADER );
	bmihOut.biCompression = BI_RGB;
	bmihOut.biPlanes = 1;
	bmihOut.biBitCount = 24;
	bmihOut.biWidth = nWidth;
	bmihOut.biHeight = nHeight;

	if ( nItem >= 0 )
		DrawTabItem( dcMem, nItem, rcMem, flags );

	// Blit image to the screen.
	BitBlt( dc, rcItem.left, rcItem.top, nWidth, nHeight, dcMem, 0, 0, SRCCOPY );
	SelectObject( dcMem, oldBmp );

	DeleteObject( bmpMem );
	DeleteDC( dcMem );
}

void CIRCTabCtrl::DrawTabItem(HDC dc, int nItem, const RECT& rcItem, UINT /*flags*/)
{
	TCHAR pszBuffer[ 128 + 4 ] = {};
	TC_ITEM item = { TCIF_TEXT | TCIF_IMAGE };
	item.pszText = pszBuffer;
	item.cchTextMax = 127;
	TabCtrl_GetItem( m_hWnd, nItem, &item );

	//BOOL bSel = flags & paintSelected;

	RECT rc = rcItem;
	//rc.bottom -= ( bSel ? 1 : 2 );
	//rc.top  += 2 + ( bSel ? 1 : 3 );
	rc.left += 6;	// Text & icon.
	rc.top++;

	int oldMode = SetBkMode( dc, TRANSPARENT );
	//HIMAGELIST imageList = (HIMAGELIST)TabCtrl_GetImageList( m_hWnd );
	//if ( imageList && item.iImage >= 0 )
	//{
	//	ImageList_Draw( imageList, item.iImage, dc,
	//		rc.left + ( bSel ? 2 : 0 ),
	//		rc.top + ( bSel ? 0 : -2 ), ILD_TRANSPARENT );
	//	rc.left += 19;
	//}
	//else
		OffsetRect( &rc, -2, 2 );

	int nLen = (int)_tcslen( pszBuffer );
	if ( nLen > 0 )
	{
		HFONT oldFont = (HFONT)SelectObject( dc, (HFONT)SendMessage( WM_GETFONT, 0, 0 ) );
		rc.right -= 3;
		RECT r;
		SetRect( &r, 0, 0, rc.right - rc.left, 20 );

		TC_ITEM tci = { TCIF_PARAM };
		GetItem( nItem, &tci );
		SetTextColor( dc, tci.lParam );		// COLORREF Black/Highlight (ID_COLOR_NEWMSG)
		DrawText( dc, pszBuffer, nLen, &r, DT_CALCRECT | DT_SINGLELINE | DT_MODIFYSTRING | DT_END_ELLIPSIS );

		DrawText( dc, pszBuffer, nLen, &rc, DT_NOPREFIX | DT_CENTER );
		SelectObject( dc, oldFont );
	}
	SetBkMode( dc, oldMode );
}

//void CIRCTabCtrl::SetTabColor(int nItem, COLORREF cRGB)	// Highlight Text
//{
//	TC_ITEM tci = { TCIF_PARAM };
//	tci.lParam = cRGB;
//	SetItem( nItem, &tci );
//	RedrawWindow();
//}
//
//COLORREF CIRCTabCtrl::GetTabColor(int nItem)	// Highlight or Normal Text?
//{
//	TC_ITEM tci = { TCIF_PARAM };
//	GetItem( nItem, &tci );
//	return tci.lParam;
//}

BOOL CIRCTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_THEMECHANGED )
	{
		if ( m_hTheme && theApp.m_pfnCloseThemeData )
			theApp.m_pfnCloseThemeData( m_hTheme );

		if ( theApp.m_pfnOpenThemeData )
			m_hTheme = theApp.m_pfnOpenThemeData( m_hWnd, _T("Tab") );

		return TRUE;
	}

	if ( pMsg->message == WM_MOUSEMOVE )
	{
		TCHITTESTINFO htInfo = {};
		GetCursorPos( &htInfo.pt );
		ScreenToClient( &htInfo.pt );
		int nHover = TabCtrl_HitTest( m_hWnd, &htInfo );
		if ( m_nHoverTab != nHover )
		{
			m_nHoverTab = nHover;
			Invalidate( FALSE );

		//	RECT rcClient;
		//	GetClientRect( &rcClient );
		//	rcClient.bottom--;
		//	rcClient.top += 2;
		//	RedrawWindow( &rcClient );
		}
	}

	return CTabCtrl::PreTranslateMessage(pMsg);
}

//LRESULT CIRCTabCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if ( message == WM_PAINT )
//	{
//		PAINTSTRUCT ps = {};
//		if ( CDC* pDC = BeginPaint( &ps ) )
//			DrawTabControl( pDC );
//		EndPaint( &ps );
//		return 0;
//	}
//	return CTabCtrl::WindowProc(message, wParam, lParam);
//}

void CIRCTabCtrl::OnPaint()
{
	PAINTSTRUCT ps = {};
	if ( CDC* pDC = BeginPaint( &ps ) )
	{
		DrawTabControl( pDC );
		EndPaint( &ps );
	}
}

void CIRCTabCtrl::DrawTabControl(CDC* pDC)
{
	// Paint the tab bar body.
	RECT rcPage, rcItem, rcClient;
	GetClientRect( &rcClient );
	rcPage = rcClient;
	TabCtrl_AdjustRect( m_hWnd, FALSE, &rcPage );
	rcClient.top = rcPage.top - 2;

	DrawTabThemed( pDC->m_hDC, -1, rcClient, paintBody );

	int tabCount = TabCtrl_GetItemCount( m_hWnd );
	if( tabCount == 0 ) return;

	// Paint inactive tabs.
	//TCHITTESTINFO htInfo = {};
	//GetCursorPos( &htInfo.pt );
	//ScreenToClient( &htInfo.pt );
	//m_nHoverTab = TabCtrl_HitTest( m_hWnd, &htInfo );
	int nSel = TabCtrl_GetCurSel( m_hWnd );

	for ( int nTab = 0; nTab < tabCount; nTab++ )
	{
		if ( nTab == nSel ) continue;
		TabCtrl_GetItemRect( m_hWnd, nTab, &rcItem );
		DrawTabThemed( pDC->m_hDC, nTab, rcItem, nTab == m_nHoverTab ? paintHotTrack : paintNone );
	}

	// Paint the active selected tab.
	TabCtrl_GetItemRect( m_hWnd, nSel, &rcItem );
	InflateRect( &rcItem, 2, 2 );
	rcItem.bottom--;
	DrawTabThemed( pDC->m_hDC, nSel, rcItem, paintSelected );
}


/////////////////////////////////////////////////////////////////////////////
// CIRCChannelList construction

CIRCChannelList::CIRCChannelList()
	: m_nCountUserDefined( 0 )
	, m_nCount( 0 )
{
}

void CIRCChannelList::AddChannel(LPCTSTR strDisplayName, LPCTSTR strName, BOOL bUserDefined)
{
	m_bUserDefined.Add( bUserDefined );
	m_sChannelDisplayName.Add( strDisplayName );
	m_sChannelName.Add( strName );
	m_nCount++;
	if ( bUserDefined ) m_nCountUserDefined++;
}

int CIRCChannelList::GetCount(int nType) const
{
	if ( nType == -1 ) return m_nCount;
	if ( nType == 0  ) return m_nCount - m_nCountUserDefined;
	if ( nType == 1  ) return m_nCountUserDefined;
	return -1;
}
void CIRCChannelList::RemoveAll(int nType)
{
	if ( nType == -1 )
	{
		m_nCount = 0;
		m_nCountUserDefined = 0;
		m_bUserDefined.RemoveAll();
		m_sChannelDisplayName.RemoveAll();
		m_sChannelName.RemoveAll();
	}
	else
	{
		for ( int nChannel = 0 ; nChannel < m_bUserDefined.GetCount() ; nChannel++ )
		{
			if ( m_bUserDefined.GetAt( nChannel ) && nType == 1 ||
				! m_bUserDefined.GetAt( nChannel ) && nType == 0 )
			{
				m_nCount--;
				if ( m_bUserDefined.GetAt( nChannel ) ) m_nCountUserDefined--;
				m_bUserDefined.RemoveAt( nChannel );
				m_sChannelDisplayName.RemoveAt( nChannel );
				m_sChannelName.RemoveAt( nChannel );
				nChannel--;
			}
		}
	}
}

BOOL CIRCChannelList::GetType(const CString& strDisplayName) const
{
	int nIndex = GetIndexOfDisplay( strDisplayName );
	if ( nIndex == -1 ) return FALSE;
	return m_bUserDefined.GetAt( nIndex );
}

void CIRCChannelList::RemoveChannel(const CString& strDisplayName)
{
	int nIndex = GetIndexOfDisplay( strDisplayName );
	if ( nIndex == -1 ) return;
	m_nCount--;
	if ( m_bUserDefined.GetAt( nIndex ) ) m_nCountUserDefined--;
	m_bUserDefined.RemoveAt( nIndex );
	m_sChannelDisplayName.RemoveAt( nIndex );
	m_sChannelName.RemoveAt( nIndex );
}

int CIRCChannelList::GetIndexOfDisplay(const CString& strDisplayName) const
{
	int nChannel, nCount = GetCount();
	BOOL bFoundChannel = FALSE;
	for ( nChannel = 0 ; nChannel < nCount ; nChannel++ )
	{
		if ( strDisplayName.CompareNoCase( GetDisplayOfIndex( nChannel ) ) == 0 )
		{
			bFoundChannel = TRUE;
			break;
		}
	}
	if ( ! bFoundChannel ) return -1;
	return nChannel;
}

int CIRCChannelList::GetIndexOfName(const CString& strName) const
{
	int nChannel;
	int ChannelCount = GetCount();
	BOOL bFoundChannel = FALSE;
	for ( nChannel = 0 ; nChannel < ChannelCount ; nChannel++ )
	{
		if ( strName.CompareNoCase( GetNameOfIndex( nChannel ) ) == 0 )
		{
			bFoundChannel = TRUE;
			break;
		}
	}
	if ( ! bFoundChannel ) return -1;
	return nChannel;
}

CString CIRCChannelList::GetDisplayOfIndex(int nIndex) const
{
	return m_sChannelDisplayName.GetAt( nIndex );
}

CString CIRCChannelList::GetNameOfIndex(int nIndex) const
{
	return m_sChannelName.GetAt( nIndex );
}
