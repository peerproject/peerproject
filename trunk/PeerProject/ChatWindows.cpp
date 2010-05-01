//
// ChatWindows.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2007.
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
#include "ChatWindows.h"
#include "ChatSession.h"
#include "CtrlChatFrame.h"
#include "CtrlPrivateChatFrame.h"
#include "Buffer.h"
#include "EDClient.h"
#include "EDClients.h"
#include "Transfers.h"
#include "Neighbours.h"
#include "Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CChatWindows ChatWindows;


//////////////////////////////////////////////////////////////////////
// CChatWindows construction

CChatWindows::CChatWindows()
{
}

CChatWindows::~CChatWindows()
{
}

//////////////////////////////////////////////////////////////////////
// CChatWindows list access

POSITION CChatWindows::GetIterator() const
{
	return m_pList.GetHeadPosition();
}

CChatFrame* CChatWindows::GetNext(POSITION& pos) const
{
	return m_pList.GetNext( pos );
}

//////////////////////////////////////////////////////////////////////
// CChatWindows close all

void CChatWindows::Close()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		GetNext( pos )->GetParent()->DestroyWindow();
	}

	m_pList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CChatWindows private chat windows

CPrivateChatFrame* CChatWindows::FindPrivate(const Hashes::Guid& oGUID)
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPrivateChatFrame* pFrame = static_cast<CPrivateChatFrame*>( GetNext( pos ) );

		if ( pFrame->IsKindOf( RUNTIME_CLASS(CPrivateChatFrame) ) )
		{
			if ( pFrame->m_pSession != NULL &&
				validAndEqual( pFrame->m_pSession->m_oGUID, oGUID ) ) return pFrame;
		}
	}

	return NULL;
}

CPrivateChatFrame* CChatWindows::FindPrivate(IN_ADDR* pAddress)
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPrivateChatFrame* pFrame = static_cast<CPrivateChatFrame*>( GetNext( pos ) );

		if ( pFrame->IsKindOf( RUNTIME_CLASS(CPrivateChatFrame) ) )
		{
			if ( pFrame->m_pSession != NULL )
			{
				if ( pFrame->m_pSession->m_pHost.sin_addr.S_un.S_addr == pAddress->S_un.S_addr )
					return pFrame;	// Regular chat window that matches
				else if ( ( pFrame->m_pSession->m_bMustPush ) &&
					( pFrame->m_pSession->m_nProtocol == PROTOCOL_ED2K ) &&
					( pFrame->m_pSession->m_nClientID == pAddress->S_un.S_addr ) )
					return pFrame;	// ED2K Low ID chat window that matches
			}
		}
	}

	return NULL;
}

CPrivateChatFrame* CChatWindows::FindED2KFrame(SOCKADDR_IN* pAddress)
{
	// For High ID clients
	CString strHighID;

	strHighID.Format( _T("%s:%hu"), (LPCTSTR)CString( inet_ntoa( pAddress->sin_addr ) ), ntohs( pAddress->sin_port ) );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPrivateChatFrame* pFrame = static_cast<CPrivateChatFrame*>( GetNext( pos ) );

		if ( pFrame->IsKindOf( RUNTIME_CLASS(CPrivateChatFrame) ) )
		{
			if ( ( strHighID == pFrame->m_sNick ) && ( pFrame->m_pSession == NULL ) )
				return pFrame;
		}
	}

	return NULL;
}

CPrivateChatFrame* CChatWindows::FindED2KFrame(DWORD nClientID, SOCKADDR_IN* pServerAddress)
{
	// For Low ID clients
	if ( ( nClientID > 0 ) && ( nClientID < 16777216 ) )  // ED2K Low ID
	{
		CString strLowID;
		strLowID.Format( _T("%u@%s:%hu"),
		nClientID,
		(LPCTSTR)CString( inet_ntoa( pServerAddress->sin_addr ) ),
		pServerAddress->sin_port );

		for ( POSITION pos = GetIterator() ; pos ; )
		{
			CPrivateChatFrame* pFrame = static_cast<CPrivateChatFrame*>( GetNext( pos ) );

			if ( pFrame->IsKindOf( RUNTIME_CLASS(CPrivateChatFrame) ) )
			{
				if ( ( strLowID == pFrame->m_sNick ) && ( pFrame->m_pSession == NULL ) )
					return pFrame;
			}
		}
	}

	return NULL;
}

CPrivateChatFrame* CChatWindows::OpenPrivate(const Hashes::Guid& oGUID, IN_ADDR* pAddress, WORD nPort, BOOL bMustPush, PROTOCOLID nProtocol, IN_ADDR* pServerAddress, WORD nServerPort)
{
	SOCKADDR_IN pHost = {};

	pHost.sin_family	= PF_INET;
	pHost.sin_addr		= *pAddress;
	pHost.sin_port		= htons( nPort );

	if ( pServerAddress == NULL )
		return OpenPrivate( oGUID, &pHost, bMustPush, nProtocol, NULL );

	SOCKADDR_IN pServer = {};

	pServer.sin_family	= PF_INET;
	pServer.sin_addr	= *pServerAddress;
	pServer.sin_port	= htons( nServerPort );

	return OpenPrivate( oGUID, &pHost, bMustPush, nProtocol, &pServer );
}

CPrivateChatFrame* CChatWindows::OpenPrivate(const Hashes::Guid& oGUID, SOCKADDR_IN* pHost, BOOL bMustPush, PROTOCOLID nProtocol, SOCKADDR_IN* pServer)
{
	CPrivateChatFrame* pFrame = NULL;

	if ( nProtocol == PROTOCOL_BT || nProtocol == PROTOCOL_FTP )
		return NULL;

	if ( ! MyProfile.IsValid() )
	{
		CString strMessage;
		LoadString( strMessage, IDS_CHAT_NEED_PROFILE );
		if ( AfxMessageBox( strMessage, MB_YESNO|MB_ICONQUESTION ) == IDYES )
			PostMainWndMessage( WM_COMMAND, ID_TOOLS_PROFILE );
		return NULL;
	}

	if ( nProtocol == PROTOCOL_ED2K )
	{
		CEDClient* pClient;

		// First, check if it's a low ID user on another server.
		if ( bMustPush && pServer )
		{
			// It's a firewalled user (Low ID). If they are using another server, we
			// can't (shouldn't) contact them. (It places a heavy load on the ed2k servers)
			CSingleLock pLock1( &Network.m_pSection );
			if ( ! pLock1.Lock( 250 ) ) return NULL;
			if ( Neighbours.Get( &pServer->sin_addr ) == NULL ) return NULL;
			pLock1.Unlock();
		}

		// ED2K chat is handled by the EDClient section. (Transfers)
		// We need to find (or create) an EDClient to handle this chat session, since everything
		// on ed2k shares a TCP link.

		// First, lock the section to prevent a problem with other threads
		CSingleLock pLock( &Transfers.m_pSection );
		if ( ! pLock.Lock( 250 ) ) return NULL;

		// We need to connect to them, so either find or create an EDClient
		if ( pServer )
			pClient = EDClients.Connect(pHost->sin_addr.S_un.S_addr, ntohs( pHost->sin_port ), &pServer->sin_addr, ntohs( pServer->sin_port ), oGUID );
		else
			pClient = EDClients.Connect(pHost->sin_addr.S_un.S_addr, ntohs( pHost->sin_port ), NULL, 0, oGUID );
		// If we weren't able to create a client (Low-id and no server), then exit.
		if ( ! pClient ) return NULL;
		// Have it connect (if it isn't)
		if ( ! pClient->m_bConnected ) pClient->Connect();
		// Tell it to start a chat session as soon as it's able
		pClient->OpenChat();
		pLock.Unlock();

		// Check for / make active any existing window
		pFrame = FindPrivate( &pHost->sin_addr );
		// Check for an empty frame
		if ( pFrame == NULL )
		{
			if ( bMustPush )
				pFrame = FindED2KFrame( pHost->sin_addr.S_un.S_addr, pServer );
			else
				pFrame = FindED2KFrame( pHost );
		}
		if ( pFrame != NULL )
		{
			// Open window if we found one
			CWnd* pParent = pFrame->GetParent();
			if ( pParent->IsIconic() ) pParent->ShowWindow( SW_SHOWNORMAL );
			pParent->BringWindowToTop();
			pParent->SetForegroundWindow();
			// And exit
			return pFrame;
		}
		// Open an empty (blank) chat frame. This is totally unnecessary- The EDClient will open
		// one as required, but it looks better to open one here.
		pFrame = new CPrivateChatFrame();
		// Set name (Also used to match incoming connection)
		if ( bMustPush && pServer ) // Firewalled user (Low ID)
		{
			pFrame->m_sNick.Format( _T("%lu@%s:%hu"),
			pHost->sin_addr.S_un.S_addr,
			(LPCTSTR)CString( inet_ntoa( pServer->sin_addr ) ),
			ntohs( pServer->sin_port ) );
		}
		else	// Regular user (High ID)
		{
			pFrame->m_sNick.Format( _T("%s:%hu"), (LPCTSTR)CString( inet_ntoa( pHost->sin_addr ) ), ntohs( pHost->sin_port ) );
		}

		// Open window
		CWnd* pParent = pFrame->GetParent();
		if ( pParent->IsIconic() ) pParent->ShowWindow( SW_SHOWNORMAL );
		pParent->BringWindowToTop();
		pParent->SetForegroundWindow();
		// Put a 'connecting' message in the window
		CString strMessage, strConnecting;
		LoadString( strConnecting, IDS_CHAT_CONNECTING_TO );
		strMessage.Format( strConnecting, pFrame->m_sNick );
		pFrame->OnStatusMessage( 0, strMessage );

		return pFrame;
	}

	if ( oGUID )
		pFrame = FindPrivate( oGUID );
	if ( pFrame == NULL )
		pFrame = FindPrivate( &pHost->sin_addr );

	if ( pFrame == NULL )
	{
		pFrame = new CPrivateChatFrame();
		pFrame->Initiate( oGUID, pHost, bMustPush );
	}

	pFrame->PostMessage( WM_COMMAND, ID_CHAT_CONNECT );

	CWnd* pParent = pFrame->GetParent();
	if ( pParent->IsIconic() ) pParent->ShowWindow( SW_SHOWNORMAL );
	pParent->BringWindowToTop();
	pParent->SetForegroundWindow();

	return pFrame;
}

//////////////////////////////////////////////////////////////////////
// CChatWindows add and remove

void CChatWindows::Add(CChatFrame* pFrame)
{
	if ( m_pList.Find( pFrame ) == NULL )
		m_pList.AddTail( pFrame );
}

void CChatWindows::Remove(CChatFrame* pFrame)
{
	if ( POSITION pos = m_pList.Find( pFrame ) )
		m_pList.RemoveAt( pos );
}
