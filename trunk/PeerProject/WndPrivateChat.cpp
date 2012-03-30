//
// WndPrivateChat.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2011.
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
#include "WndPrivateChat.h"
#include "WndBrowseHost.h"
#include "ChatCore.h"
#include "ChatSession.h"
#include "CoolInterface.h"
#include "RichElement.h"
#include "Skin.h"
#include "GProfile.h"
#include "Transfers.h"
#include "UploadTransfer.h"
#include "Uploads.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CPrivateChatWnd, CChatWnd)

BEGIN_MESSAGE_MAP(CPrivateChatWnd, CChatWnd)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_CHAT_CONNECT, &CPrivateChatWnd::OnUpdateChatConnect)
	ON_COMMAND(ID_CHAT_CONNECT, &CPrivateChatWnd::OnChatConnect)
	ON_UPDATE_COMMAND_UI(ID_CHAT_DISCONNECT, &CPrivateChatWnd::OnUpdateChatDisconnect)
	ON_COMMAND(ID_CHAT_DISCONNECT, &CPrivateChatWnd::OnChatDisconnect)
	ON_UPDATE_COMMAND_UI(ID_CHAT_BROWSE, &CPrivateChatWnd::OnUpdateChatBrowse)
	ON_COMMAND(ID_CHAT_BROWSE, &CPrivateChatWnd::OnChatBrowse)
	ON_UPDATE_COMMAND_UI(ID_CHAT_PRIORITY, &CPrivateChatWnd::OnUpdateChatPriority)
	ON_COMMAND(ID_CHAT_PRIORITY, &CPrivateChatWnd::OnChatPriority)
	// Temp:
	ON_MESSAGE(WM_CHAT_REMOTE_MESSAGE, &CPrivateChatWnd::OnRemoteMessage)
	ON_MESSAGE(WM_CHAT_STATUS_MESSAGE, &CPrivateChatWnd::OnStatusMessage)
	ON_MESSAGE(WM_CHAT_BITMAP_MESSAGE, &CPrivateChatWnd::OnBitmapMessage)
	ON_MESSAGE(WM_CHAT_PROFILE_RECEIVED, &CPrivateChatWnd::OnProfileReceived)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrivateChatWnd construction

CPrivateChatWnd::CPrivateChatWnd()
	: m_pSession ( NULL )
{
	Create( IDR_CHATFRAME, TRUE );
}

CPrivateChatWnd::~CPrivateChatWnd()
{
	ASSERT( m_pSession == NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CPrivateChatWnd operations

CString CPrivateChatWnd::GetChatID() const
{
	return m_pSession ? HostToString( &m_pSession->m_pHost ) : ( CString( _T("@") ) + m_sNick );
}

CString CPrivateChatWnd::GetCaption() const
{
	CString strCaption;
	LoadString( strCaption, IDR_CHATFRAME );
	if ( Settings.General.LanguageRTL ) strCaption = _T("\x200F") + strCaption + _T("\x202E");
	strCaption += _T(" : ");
	if ( Settings.General.LanguageRTL ) strCaption += _T("\x202B");
	strCaption += m_sNick;
	if ( m_pSession )
	{
		if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
		strCaption += _T(" (") + HostToString( &m_pSession->m_pHost ) + _T(")");
		if ( ! m_pSession->m_sUserAgent.IsEmpty() )
		{
			if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
			strCaption += _T(" ") + m_pSession->m_sUserAgent;
		}
	}
	return strCaption;
}

void CPrivateChatWnd::Setup(LPCTSTR szNick)
{
	m_sNick = szNick;

	// Open window
	Open();

	// Put a 'connecting' message in the window
	CString strMessage;
	strMessage.Format( LoadString( IDS_CHAT_CONNECTING_TO ), m_sNick );
	CChatWnd::OnStatusMessage( 0, strMessage );
}

void CPrivateChatWnd::Setup(const Hashes::Guid& oGUID, const SOCKADDR_IN* pHost, BOOL bMustPush, PROTOCOLID nProtocol)
{
	ASSERT( m_pSession == NULL );

	m_pSession = new CChatSession( nProtocol, this );
	m_pSession->m_oGUID 	= oGUID;
	m_pSession->m_pHost		= *pHost;
	m_pSession->m_sUserNick	= HostToString( pHost );
	m_pSession->m_bMustPush	= bMustPush;
}

BOOL CPrivateChatWnd::Accept(CChatSession* pSession)
{
	if ( m_pSession )
	{
		if ( m_pSession->m_nState > cssConnecting )
			return FALSE;
		m_pSession->OnCloseWindow();
	}

	m_pSession = pSession;

	return TRUE;
}

BOOL CPrivateChatWnd::Find(const SOCKADDR_IN* pAddress) const
{
	if ( ! m_pSession ) return FALSE;

	// Regular chat window that matches, or ED2K Low ID chat window that matches
	return ( m_pSession->m_pHost.sin_addr.s_addr == pAddress->sin_addr.s_addr &&
			 m_pSession->m_pHost.sin_port == pAddress->sin_port ) ||
		   ( m_pSession->m_bMustPush &&
			 m_pSession->m_nProtocol == PROTOCOL_ED2K &&
			 m_pSession->m_nClientID == pAddress->sin_addr.s_addr );
}

BOOL CPrivateChatWnd::Find(const Hashes::Guid& oGUID, bool bLive) const
{
	if ( m_pSession && validAndEqual( m_pSession->m_oGUID, oGUID ) )
		return ( bLive == m_pSession->m_nState > cssConnecting );

	return FALSE;
}

BOOL CPrivateChatWnd::Find(const CString& sNick) const
{
	return ( sNick == m_sNick && m_pSession == NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CPrivateChatWnd event handlers

void CPrivateChatWnd::OnDestroy()
{
	CChatWnd::OnDestroy();

	{
		CQuickLock pLock( ChatCore.m_pSection );
		if ( CChatSession* pSession = m_pSession )
		{
			m_pSession = NULL;
			pSession->OnCloseWindow();
		}
	}
}

BOOL CPrivateChatWnd::OnLocalMessage(bool bAction, const CString& sText)
{
	CChatWnd::OnMessage( bAction, GetChatID(), true, MyProfile.GetNick(), m_sNick, sText );
	if ( ! m_pSession )
		return FALSE;
	return m_pSession->SendPrivateMessage( bAction, sText );
}

BOOL CPrivateChatWnd::OnLocalCommand(const CString& sCommand, const CString& sArgs)
{
	if ( sCommand.CompareNoCase( _T("/connect") ) == 0 )
		PostMessage( WM_COMMAND, ID_CHAT_CONNECT );
	else if ( sCommand.CompareNoCase( _T("/disconnect") ) == 0 )
		PostMessage( WM_COMMAND, ID_CHAT_DISCONNECT );
	else if ( sCommand.CompareNoCase( _T("/browse") ) == 0 )
		PostMessage( WM_COMMAND, ID_CHAT_BROWSE );
	else
		return CChatWnd::OnLocalCommand( sCommand, sArgs );

	return TRUE;
}

// Temp:

LRESULT CPrivateChatWnd::OnProfileReceived(WPARAM, LPARAM)
{
	if ( m_pSession )
		m_sNick = m_pSession->m_sUserNick;

	CChatWnd::AddLogin( m_sNick );

	CString strCaption;
	LoadString( strCaption, IDR_CHATFRAME );
	if ( Settings.General.LanguageRTL ) strCaption = _T("\x200F") + strCaption + _T("\x202E");
	strCaption += _T(" : ");
	if ( Settings.General.LanguageRTL ) strCaption += _T("\x202B");
	strCaption += m_sNick;

	if ( m_pSession )
	{
		CString strAddress;
		strAddress.Format( _T(" (%s)"),
			(LPCTSTR)HostToString( &m_pSession->m_pHost ) );
		if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
		strCaption += strAddress;
		if ( ! m_pSession->m_sUserAgent.IsEmpty() )
		{
			if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
			strCaption = strCaption + _T(" - ") + m_pSession->m_sUserAgent;
		}
	}

	SetWindowText( strCaption );

	Open();

	SetAlert();

	return 0;
}

LRESULT CPrivateChatWnd::OnRemoteMessage(WPARAM wParam, LPARAM lParam)
{
	bool bAction = ( wParam != 0 );
	CAutoPtr< CString > psText( (CString*)lParam );

	CString sChatID;
	if ( m_pSession )
	{
		m_sNick = m_pSession->m_sUserNick;
		sChatID = HostToString( &m_pSession->m_pHost );
	}
	else
		sChatID = _T("Private:") + m_sNick;

	CChatWnd::OnMessage( bAction, sChatID, false, m_sNick, MyProfile.GetNick(), *psText );

	if ( ! m_pSession )
		return 0;

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

	return 0;
}

LRESULT CPrivateChatWnd::OnStatusMessage(WPARAM wParam, LPARAM lParam)
{
	CAutoPtr< CString > psText( (CString*)lParam );

	CChatWnd::OnStatusMessage( (int)wParam, *psText );

	return 0;
}

LRESULT CPrivateChatWnd::OnBitmapMessage(WPARAM /*wParam*/, LPARAM lParam)
{
	CChatWnd::AddBitmap( (HBITMAP)lParam );

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPrivateChatWnd commands

void CPrivateChatWnd::OnUpdateChatConnect(CCmdUI* pCmdUI)
{
	BOOL bState = m_pSession && m_pSession->GetConnectedState() == TRI_FALSE;
	if ( CCoolBarItem* pItem = CCoolBarItem::FromCmdUI( pCmdUI ) ) pItem->Show( bState );
	pCmdUI->Enable( bState );
}

void CPrivateChatWnd::OnChatConnect()
{
	if ( m_pSession && m_pSession->GetConnectedState() == TRI_FALSE )
	{
		Open();
		m_pSession->Connect();
	}
}

void CPrivateChatWnd::OnUpdateChatDisconnect(CCmdUI* pCmdUI)
{
	BOOL bState = m_pSession && m_pSession->GetConnectedState() != TRI_FALSE;
	if ( CCoolBarItem* pItem = CCoolBarItem::FromCmdUI( pCmdUI ) ) pItem->Show( bState );
	pCmdUI->Enable( bState );
}

void CPrivateChatWnd::OnChatDisconnect()
{
	if ( m_pSession )
		m_pSession->Close();
}

void CPrivateChatWnd::OnUpdateChatBrowse(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_pSession && m_pSession->m_nProtocol != PROTOCOL_DC );
}

void CPrivateChatWnd::OnChatBrowse()
{
	if ( m_pSession && m_pSession->m_nProtocol != PROTOCOL_DC )
		new CBrowseHostWnd( m_pSession->m_nProtocol, &m_pSession->m_pHost, m_pSession->m_oGUID );
		//	&m_pSession->m_pHost, FALSE, m_pSession->m_oGUID, m_pSession->m_sUserNick );
}

void CPrivateChatWnd::OnUpdateChatPriority(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_pSession && m_pSession->m_nProtocol != PROTOCOL_DC && m_pSession->GetConnectedState() == TRI_TRUE );
}

void CPrivateChatWnd::OnChatPriority()
{
	if ( ! m_pSession )
		return;

	CSingleLock pLock( &Transfers.m_pSection );
	if ( ! pLock.Lock( 500 ) )
		return;

	DWORD nAddress = m_pSession->m_pHost.sin_addr.s_addr;
	for ( POSITION pos = Uploads.GetIterator() ; pos ; )
	{
		CUploadTransfer* pUpload = Uploads.GetNext( pos );
		if ( pUpload->m_pHost.sin_addr.s_addr == nAddress &&
			 pUpload->m_nState == upsQueued )
		{
			pUpload->Promote();
		}
	}

	pLock.Unlock();

	CString strMessage;
	strMessage.Format( LoadString( IDS_CHAT_PRIORITY_GRANTED ), (LPCTSTR)HostToString( &m_pSession->m_pHost ) );
	CChatWnd::OnStatusMessage( 2, strMessage );
}
