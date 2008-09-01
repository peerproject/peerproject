//
// ChatCore.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
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
#include "EDClient.h"
#include "ChatSession.h"
#include "ChatCore.h"
#include "Buffer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CChatCore ChatCore;


//////////////////////////////////////////////////////////////////////
// CChatCore construction

CChatCore::CChatCore()
{
}

CChatCore::~CChatCore()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
// CChatCore session access

POSITION CChatCore::GetIterator() const
{
	return m_pSessions.GetHeadPosition();
}

CChatSession* CChatCore::GetNext(POSITION& pos) const
{
	return m_pSessions.GetNext( pos );
}

BOOL CChatCore::Check(CChatSession* pSession) const
{
	return m_pSessions.Find( pSession ) != NULL;
}

//////////////////////////////////////////////////////////////////////
// CChatCore accept new connections

void CChatCore::OnAccept(CConnection* pConnection, PROTOCOLID nProtocol)
{
	CSingleLock pLock( &m_pSection );
	if ( ! pLock.Lock( 250 ) ) return;
	
	CChatSession* pSession = new CChatSession();

	pSession->m_nProtocol = nProtocol;

	pSession->AttachTo( pConnection );
}

BOOL CChatCore::OnPush(const Hashes::Guid& oGUID, CConnection* pConnection)
{
	CSingleLock pLock( &m_pSection );
	if ( ! pLock.Lock( 250 ) ) return FALSE;
	
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CChatSession* pSession = GetNext( pos );
		if ( pSession->OnPush( oGUID, pConnection ) ) return TRUE;
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CChatCore ED2K chat handling

void CChatCore::OnED2KMessage(CEDClient* pClient, CEDPacket* pPacket)
{
	ASSERT ( pClient != NULL );
	// Note: Null packet is valid- in that case we have no pending message, but want to open
	// a chat window.

	CSingleLock pLock( &m_pSection );
	if ( ! pLock.Lock( 250 ) ) return;
	
	CChatSession* pSession = FindSession( pClient );

	pSession->OnED2KMessage( pPacket );
}

CChatSession* CChatCore::FindSession(CEDClient* pClient)
{
	CChatSession* pSession;

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		pSession = GetNext( pos );

		// If we already have a session
		if ( ( ( ! pSession->m_oGUID ) || validAndEqual( pSession->m_oGUID, pClient->m_oGUID ) ) &&
			 ( pSession->m_pHost.sin_addr.S_un.S_addr == pClient->m_pHost.sin_addr.S_un.S_addr ) &&
			 ( pSession->m_nProtocol == PROTOCOL_ED2K ) )
		{
			// Update details
			pSession->m_oGUID		= pClient->m_oGUID;
			pSession->m_pHost		= pClient->m_pHost;
			pSession->m_sAddress	= pClient->m_sAddress;
			pSession->m_sUserNick	= pClient->m_sNick;
			pSession->m_sUserAgent	= pClient->m_sUserAgent;
			pSession->m_bUnicode	= pClient->m_bEmUnicode;
			pSession->m_nClientID	= pClient->m_nClientID;
			pSession->m_pServer		= pClient->m_pServer;

			pSession->m_bMustPush	= ( ( pClient->m_nClientID > 0 ) && ( pClient->m_nClientID < 16777216 ) );

			// return existing session
			return pSession;
		}
	}

	// Create a new chat session
	pSession = new CChatSession();

	pSession->m_nProtocol	= PROTOCOL_ED2K;
	pSession->m_hSocket		= INVALID_SOCKET;			// Should always remain invalid- has no real connection
	pSession->m_nState		= cssActive;
	pSession->m_bConnected	= TRUE;
	pSession->m_tConnected	= GetTickCount();

	// Set details
	pSession->m_oGUID		= pClient->m_oGUID;
	pSession->m_pHost		= pClient->m_pHost;
	pSession->m_sAddress	= pClient->m_sAddress;
	pSession->m_sUserNick	= pClient->m_sNick;
	pSession->m_sUserAgent	= pClient->m_sUserAgent;
	pSession->m_bUnicode	= pClient->m_bEmUnicode;
	pSession->m_nClientID	= pClient->m_nClientID;
	pSession->m_pServer		= pClient->m_pServer;

	pSession->m_bMustPush	= ( ( pClient->m_nClientID > 0 ) && ( pClient->m_nClientID < 16777216 ) );

	// Make new input and output buffer objects
	pSession->CreateBuffers();

	Add( pSession );

	return pSession;
}

//////////////////////////////////////////////////////////////////////
// CChatCore session add and remove

void CChatCore::Add(CChatSession* pSession)
{
	CSingleLock pLock( &m_pSection, TRUE );
	if ( m_pSessions.Find( pSession ) == NULL ) m_pSessions.AddTail( pSession );
	if ( pSession->IsValid() )
		WSAEventSelect( pSession->m_hSocket, GetWakeupEvent(),
			FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );
	StartThread();
}

void CChatCore::Remove(CChatSession* pSession)
{
	CSingleLock pLock( &m_pSection, TRUE );
	POSITION pos = m_pSessions.Find( pSession );
	if ( pos != NULL ) m_pSessions.RemoveAt( pos );
	if ( pSession->IsValid() )
		WSAEventSelect( pSession->m_hSocket, GetWakeupEvent(), 0 );
}

void CChatCore::Close()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		GetNext( pos )->Close();
	}
	
	StopThread();
}

//////////////////////////////////////////////////////////////////////
// CChatCore thread control

void CChatCore::StartThread()
{
	if ( GetCount() == 0 ) return;
	
	BeginThread( "ChatCore" );
}

void CChatCore::StopThread()
{
	CloseThread();
}

//////////////////////////////////////////////////////////////////////
// CChatCore thread run

void CChatCore::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( IsThreadEnabled() )
	{
		Sleep( 50 );
		Doze( 100 );
		
		if ( pLock.Lock( 250 ) )
		{
			if ( GetCount() == 0 ) break;
			
			for ( POSITION pos = GetIterator() ; pos ; )
			{
				GetNext( pos )->DoRun();
			}
			
			pLock.Unlock();
		}
	}
}
