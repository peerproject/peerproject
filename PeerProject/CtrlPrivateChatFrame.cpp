//
// CtrlPrivateChatFrame.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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
#include "CtrlPrivateChatFrame.h"
#include "ChatSession.h"
#include "RichElement.h"
#include "Transfers.h"
#include "Uploads.h"
#include "UploadTransfer.h"
#include "WndBrowseHost.h"
#include "Colors.h"
#include "Skin.h"
#include "Security.h"
#include "Plugins.h"	// IChatPlugin Capture

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CPrivateChatFrame, CChatFrame)

BEGIN_MESSAGE_MAP(CPrivateChatFrame, CChatFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_CHAT_BROWSE, OnUpdateChatBrowse)
	ON_COMMAND(ID_CHAT_BROWSE, OnChatBrowse)
	ON_UPDATE_COMMAND_UI(ID_CHAT_PRIORITY, OnUpdateChatPriority)
	ON_COMMAND(ID_CHAT_PRIORITY, OnChatPriority)
END_MESSAGE_MAP()

#define EDIT_HEIGHT		32
//define TOOLBAR_HEIGHT	30	// Skin.m_nToolbarHeight


/////////////////////////////////////////////////////////////////////////////
// CPrivateChatFrame construction

CPrivateChatFrame::CPrivateChatFrame()
{
	CRect rc( 0, 0, 0, 0 );
	Create( NULL, _T("Chat"), WS_CHILD|WS_CLIPCHILDREN, rc, AfxGetMainWnd(), 100 );
}

CPrivateChatFrame::~CPrivateChatFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPrivateChatFrame operations

void CPrivateChatFrame::Initiate(const Hashes::Guid& oGUID, SOCKADDR_IN* pHost, BOOL bMustPush)
{
	ASSERT( m_pSession == NULL );

	m_pSession = new CChatSession( this );
	m_pSession->Setup( oGUID, pHost, bMustPush );
}

BOOL CPrivateChatFrame::Accept(CChatSession* pSession)
{
	if ( m_pSession != NULL )
	{
		if ( m_pSession->m_nState > cssConnecting ) return FALSE;
		m_pSession->OnCloseWindow();
	}

	m_pSession = pSession;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPrivateChatFrame message handlers

int CPrivateChatFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CChatFrame::OnCreate( lpCreateStruct ) == -1 ) return -1;

	OnSkinChange();

	return 0;
}

void CPrivateChatFrame::OnSkinChange()
{
	OnSize( 0, 0, 0 );
	CChatFrame::OnSkinChange();
	Skin.CreateToolBar( _T("CPrivateChatFrame"), &m_wndToolBar );
}

void CPrivateChatFrame::OnSize(UINT nType, int cx, int cy)
{
	CChatFrame::OnSize( nType, cx, cy );

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

void CPrivateChatFrame::OnPaint()
{
	CPaintDC dc( this );
	CRect rc;

	GetClientRect( &rc );
}

void CPrivateChatFrame::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if ( point.x == -1 && point.y == -1 ) 	// Keyboard fix
		ClientToScreen( &point );

	Skin.TrackPopupMenu( _T("CPrivateChatFrame"), point );
}

/////////////////////////////////////////////////////////////////////////////
// CPrivateChatFrame event handlers

void CPrivateChatFrame::OnProfileReceived()
{
	m_sNick = m_pSession->m_sUserNick;

	AddTimestamp();

	m_pContent.Add( retText, LoadString( IDS_CHAT_PROFILE_ACCEPTED ), NULL, retfColor )->m_cColor = Colors.m_crChatNull;
	m_pContent.Add( retLink, m_sNick, _T("command:ID_CHAT_BROWSE") );
	m_pContent.Add( retNewline, NEWLINE_FORMAT );
	m_wndView.InvalidateIfModified();

	CString strCaption;
	LoadString( strCaption, IDR_CHATFRAME );
	if ( Settings.General.LanguageRTL ) strCaption = _T("\x200F") + strCaption + _T("\x202E");
	strCaption += _T(" : ");
	if ( Settings.General.LanguageRTL ) strCaption += _T("\x202B");
	strCaption += m_sNick;
	CString strAddress;
	strAddress.Format( _T(" (%s:%lu)"),
		(LPCTSTR)CString( inet_ntoa( m_pSession->m_pHost.sin_addr ) ),
		ntohs( m_pSession->m_pHost.sin_port ) );
	if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
	strCaption += strAddress;
	if ( ! m_pSession->m_sUserAgent.IsEmpty() )
	{
		if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
		strCaption = strCaption + _T(" - ") + m_pSession->m_sUserAgent;
	}

	SetWindowText( strCaption );
	GetParent()->PostMessage( WM_TIMER, 2 );

	SetAlert();
}

void CPrivateChatFrame::OnRemoteMessage(BOOL bAction, LPCTSTR pszText)
{
	// Check message spam filter (if enabled)
	if ( MessageFilter.IsFiltered( pszText ) )
		return;

	DWORD nIdle = (DWORD)time( NULL ) - theApp.m_nLastInput;

	if ( nIdle > Settings.Community.AwayMessageIdleTime )
	{
		CString strTime;
		if ( nIdle > 86400 )
			strTime.Format( _T("%u:%.2u:%.2u:%.2u"), nIdle / 86400, ( nIdle / 3600 ) % 24, ( nIdle / 60 ) % 60, nIdle % 60 );
		else
			strTime.Format( _T("%u:%.2u:%.2u"), nIdle / 3600, ( nIdle / 60 ) % 60, nIdle % 60 );

		CString strMessage;
		strMessage.Format( LoadString( IDS_CHAT_PRIVATE_AWAY ), _T(""), strTime );
		m_pSession->SendPrivateMessage( true, strMessage );
	}

	// Adult filter (if enabled)
	if ( AdultFilter.IsChatFiltered( pszText ) )
		AdultFilter.Censor( (TCHAR*)pszText );

	// Notify chat plugins about new remote message
	CString sChatID;
	sChatID.Format( _T("%s:%hu"),
		(LPCTSTR)CString( inet_ntoa( m_pSession->m_pHost.sin_addr ) ),
		ntohs( m_pSession->m_pHost.sin_port ) );
	Plugins.OnChatMessage( sChatID, FALSE, m_sNick, MyProfile.GetNick(), pszText );

	AddText( FALSE, bAction, m_sNick, pszText );
	SetAlert();
	PostMessage( WM_TIMER, 4 );
}

void CPrivateChatFrame::OnLocalMessage(bool bAction, LPCTSTR pszText)
{
	TRISTATE bConnected = m_pSession->GetConnectedState();

	if ( bConnected != TRI_TRUE )
	{
		if ( ( m_pSession->m_nProtocol == PROTOCOL_ED2K ) || ( bConnected != TRI_FALSE ) )
		{
			m_pSession->StatusMessage( 1, IDS_CHAT_NOT_CONNECTED_2 );
		}
		else
		{
			m_pSession->StatusMessage( 1, IDS_CHAT_NOT_CONNECTED_1 );
			PostMessage( WM_COMMAND, ID_CHAT_CONNECT );
		}
		return;
	}

	// Adult filter (if enabled)
	if ( AdultFilter.IsChatFiltered( pszText ) )
		AdultFilter.Censor( (TCHAR*)pszText );

	// Notify chat plugins about new local message
	CString sChatID;
	sChatID.Format( _T("%s:%hu"),
		(LPCTSTR)CString( inet_ntoa( m_pSession->m_pHost.sin_addr ) ),
		ntohs( m_pSession->m_pHost.sin_port ) );
	Plugins.OnChatMessage( sChatID, TRUE, MyProfile.GetNick(), m_sNick, pszText );

	AddText( TRUE, bAction, MyProfile.GetNick(), pszText );
	m_pSession->SendPrivateMessage( bAction, pszText );
}

void CPrivateChatFrame::OnLocalCommand(LPCTSTR pszCommand, LPCTSTR pszArgs)
{
	if ( _tcsicmp( pszCommand, _T("/browse") ) == 0 )
		PostMessage( WM_COMMAND, ID_CHAT_BROWSE );
	else
		CChatFrame::OnLocalCommand( pszCommand, pszArgs );
}

/////////////////////////////////////////////////////////////////////////////
// CPrivateChatFrame commands

void CPrivateChatFrame::OnUpdateChatBrowse(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_pSession != NULL );
}

void CPrivateChatFrame::OnChatBrowse()
{
	if ( m_pSession != NULL )
		new CBrowseHostWnd( m_pSession->m_nProtocol, &m_pSession->m_pHost, m_pSession->m_oGUID );
}

void CPrivateChatFrame::OnUpdateChatPriority(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_pSession != NULL && m_pSession->GetConnectedState() == TRI_TRUE );
}

void CPrivateChatFrame::OnChatPriority()
{
	CSingleLock pLock( &Transfers.m_pSection );
	if ( ! pLock.Lock( 500 ) ) return;

	const DWORD nAddress = m_pSession->m_pHost.sin_addr.S_un.S_addr;
	int nCount = 0;

	for ( POSITION pos = Uploads.GetIterator() ; pos ; )
	{
		CUploadTransfer* pUpload = Uploads.GetNext( pos );

		if ( pUpload->m_pHost.sin_addr.S_un.S_addr == nAddress && pUpload->m_nState == upsQueued )
			pUpload->Promote( nCount < 2 ); 	// First couple get unlimited, others dequeued

		nCount++;
	}

	m_pSession->StatusMessage( 2, IDS_CHAT_PRIORITY_GRANTED, (LPCTSTR)m_pSession->m_sAddress, nCount );
}
