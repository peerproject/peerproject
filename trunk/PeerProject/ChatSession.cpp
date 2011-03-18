//
// ChatSession.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
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
#include "PeerProject.h"
#include "Settings.h"
#include "GProfile.h"
#include "G2Packet.h"
#include "EDPacket.h"
#include "EDClient.h"
#include "EDClients.h"
#include "Transfers.h"
#include "Network.h"
#include "Buffer.h"
#include "XML.h"

#include "ChatCore.h"
#include "ChatSession.h"

#include "ChatWindows.h"
#include "CtrlChatFrame.h"
#include "CtrlPrivateChatFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename


//////////////////////////////////////////////////////////////////////
// CChatSession construction

CChatSession::CChatSession(CChatFrame* pFrame)
	: m_nState		( cssNull )
	, m_bMustPush	( FALSE )
	, m_bUnicode	( FALSE )
	, m_bOld		( FALSE )
	, m_nClientID	( 0 )
	, m_tPushed		( 0 )
	, m_pProfile	( NULL )
	, m_pWndPublic	( NULL )
	, m_pWndPrivate	( ( pFrame && pFrame->IsKindOf( RUNTIME_CLASS( CPrivateChatFrame ) ) ) ?
		static_cast< CPrivateChatFrame* >( pFrame ) : NULL )
{
	ZeroMemory( &m_pServer, sizeof( m_pServer ) );
	m_pServer.sin_family = AF_INET;

	//if ( pFrame != NULL )
	//{
	//	m_pWndPrivate = ( pFrame->IsKindOf( RUNTIME_CLASS(CPrivateChatFrame) ) )
	//		? static_cast<CPrivateChatFrame*>(pFrame) : NULL;
	//}

	ChatCore.Add( this );
}

CChatSession::~CChatSession()
{
	ASSERT( ! IsValid() );

	delete m_pProfile;

	ChatCore.Remove( this );
}

//////////////////////////////////////////////////////////////////////
// CChatSession setup

void CChatSession::Setup(const Hashes::Guid& oGUID, SOCKADDR_IN* pHost, BOOL bMustPush)
{
	CSingleLock pLock( &ChatCore.m_pSection, TRUE );

	m_oGUID 	= oGUID;

	m_pHost		= *pHost;
	m_bMustPush	= bMustPush;

	m_sUserNick = inet_ntoa( m_pHost.sin_addr );
}

//////////////////////////////////////////////////////////////////////
// CChatSession connect

BOOL CChatSession::Connect()
{
	CSingleLock pLock1( &ChatCore.m_pSection, TRUE );

	// ED2K Clients have their connection controlled by ED2KClient. (One connection used for many things)
	if ( m_nProtocol == PROTOCOL_ED2K )
		return TRUE;

	// If we are already connected/handshaking/connecting, don't try again.
	if ( m_nState > cssNull ) return FALSE;

	if ( m_bMustPush )
	{
		if ( ! SendPush( FALSE ) )
		{
			StatusMessage( 1, IDS_CHAT_CANT_PUSH, (LPCTSTR)CString( inet_ntoa( m_pHost.sin_addr ) ) );
			return FALSE;
		}
	}
	else
	{
		if ( CConnection::ConnectTo( &m_pHost ) )
		{
			ChatCore.Add( this );
			StatusMessage( 0, IDS_CHAT_CONNECTING_TO, (LPCTSTR)m_sAddress );
		}
		else
		{
			StatusMessage( 1, IDS_CHAT_CANT_CONNECT, (LPCTSTR)m_sAddress );
			return FALSE;
		}
	}

	m_nState = cssConnecting;

	return TRUE;
}

TRISTATE CChatSession::GetConnectedState() const
{
	if ( m_nState == cssNull ) return TRI_FALSE;
	if ( m_nState >= cssActive ) return TRI_TRUE;
	return TRI_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////
// CChatSession handle an incoming ED2K Message

void CChatSession::OnED2KMessage(CEDPacket* pPacket)
{
	CSingleLock pLock( &ChatCore.m_pSection, TRUE );

	// Open a window (if one is not already open)
	PostOpenWindow();

	// Put the packet into the input buffer so it can be 'received' (and displayed) later.
	if ( pPacket && IsInputExist() )
	{
		CLockedBuffer pInput( GetInput() );
		pPacket->ToBuffer( pInput );
	}

	MakeActive();
}

//////////////////////////////////////////////////////////////////////
// If this client wasn't active, it is now.

void CChatSession::MakeActive()
{
	if ( m_nState != cssActive )
	{
		StatusMessage( 2, IDS_CHAT_PRIVATE_ONLINE, (LPCTSTR)m_sUserNick );
		m_nState = cssActive;
		m_tConnected = GetTickCount();
	}
}

//////////////////////////////////////////////////////////////////////
// CChatSession attach to (accept) an incoming connection

void CChatSession::AttachTo(CConnection* pConnection)
{
	CConnection::AttachTo( pConnection );

	m_nState = cssRequest1;
	ChatCore.Add( this );
}

//////////////////////////////////////////////////////////////////////
// CChatSession push functionality

BOOL CChatSession::SendPush(BOOL /*bAutomatic*/)
{
	if ( ! m_oGUID ) return FALSE;

	if ( m_nProtocol == PROTOCOL_ED2K ) return FALSE;

	if ( Network.SendPush( m_oGUID, 0 ) )
	{
		m_nState = cssNull;
		CConnection::Close();

		m_tConnected = m_tPushed = GetTickCount();
		StatusMessage( 0, IDS_CHAT_PUSH_SENT, (LPCTSTR)CString( inet_ntoa( m_pHost.sin_addr ) ) );

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CChatSession::OnPush(const Hashes::Guid& oGUID, CConnection* pConnection)
{
	if ( m_tPushed == 0 ) return FALSE;
	if ( ! m_oGUID || validAndUnequal( m_oGUID, oGUID ) ) return FALSE;
	if ( m_nState > cssConnecting ) return FALSE;
	if ( m_nProtocol == PROTOCOL_ED2K ) return FALSE;

	if ( m_nState > cssNull ) Close();

	CConnection::AttachTo( pConnection );

	StatusMessage( 0, IDS_CHAT_PUSH_DONE, (LPCTSTR)m_sAddress );
	ChatCore.Add( this );
	OnConnected();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession close

void CChatSession::Close(UINT nError)
{
	// ED2K Clients have their connection controlled by ED2KClient.
	if ( m_nProtocol == PROTOCOL_ED2K ) return;

	CSingleLock pLock( &ChatCore.m_pSection );
	pLock.Lock( 250 );

	if ( m_nState != cssNull )
	{
		m_nState = cssNull;
		StatusMessage( 0, IDS_CHAT_CLOSED );
	}

	CConnection::Close( nError );

	if ( m_pWndPrivate == NULL && m_pWndPublic == NULL ) delete this;
}

//////////////////////////////////////////////////////////////////////
// CChatSession connection handler

BOOL CChatSession::OnConnected()
{
	StatusMessage( 0, IDS_CHAT_CONNECTED );

	if ( m_nProtocol == PROTOCOL_ED2K )
	{
		// ED2K connections aren't handled here- they are in ED2KClient
	}
	else
	{
		CConnection::OnConnected();

		Write( _P("CHAT CONNECT/0.2\r\n"
			"Accept: text/plain,application/x-gnutella2\r\n"
			"User-Agent: ") );
		Write( Settings.SmartAgent() );
		Write( _P("\r\n") );
		if ( m_bInitiated ) SendMyAddress();
		Write( _P("\r\n") );

		m_nState		= cssRequest2;
		m_tConnected	= GetTickCount();

		OnWrite();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession disconnection handler

void CChatSession::OnDropped()
{
	if ( ! IsValid() ) return;

	if ( m_nState == cssConnecting )
	{
		StatusMessage( 1, IDS_CHAT_CANT_CONNECT, (LPCTSTR)m_sAddress );
		if ( m_tPushed == 0 && SendPush( TRUE ) ) return;
	}
	else
	{
		StatusMessage( 1, IDS_CHAT_DROPPED, (LPCTSTR)m_sAddress );
	}

	Close();
}

//////////////////////////////////////////////////////////////////////
// CChatSession run handler

BOOL CChatSession::OnRun()
{
	if ( m_nProtocol == PROTOCOL_ED2K )
	{
		// ed2k chat sessions don't have real connections, ED2K Client just puts the packets into the buffer.
		return ( ReadPacketsED2K() && SendPacketsED2K() );
	}
	else if ( m_nState > cssNull && m_nState < cssActive )
	{
		DWORD nDelay = GetTickCount() - m_tConnected;

		if ( nDelay >= ( m_nState == cssConnecting ?
			Settings.Connection.TimeoutConnect : Settings.Connection.TimeoutHandshake ) )
		{
			theApp.Message( MSG_ERROR, IDS_HANDSHAKE_TIMEOUT, (LPCTSTR)m_sAddress );
			Close();
			return FALSE;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession read handler

BOOL CChatSession::OnRead()
{
	// ED2K connections aren't handled here, this shouldn't ever be called for ed2k sessions
	if ( m_nProtocol == PROTOCOL_ED2K ) return TRUE;

	CConnection::OnRead();

	switch ( m_nState )
	{
	case cssRequest1:
	case cssRequest2:
	case cssRequest3:
		return ReadHandshake();
	case cssHeaders1:
	case cssHeaders2:
	case cssHeaders3:
		return ReadHeaders();
	case cssHandshake:
	case cssActive:
	case cssAway:
		if ( m_nProtocol == PROTOCOL_G2 )
			return ReadPackets();
		else
			return ReadText();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession handshake processer

BOOL CChatSession::ReadHandshake()
{
	CString strLine;

	if ( ! Read( strLine ) ) return TRUE;
	if ( strLine.IsEmpty() ) return TRUE;

	theApp.Message( MSG_DEBUG | MSG_FACILITY_INCOMING, _T("%s >> CHAT HANDSHAKE: %s"), (LPCTSTR)m_sAddress, (LPCTSTR)strLine );

	m_bOld = strLine.Find( _T("/0.1") ) > 0;

	if ( ::StartsWith( strLine, _PT("CHAT CONNECT/") ) && m_nState == cssRequest1 )
	{
		m_nState = cssHeaders1;
		return TRUE;
	}
	else if ( ::StartsWith( strLine, _PT("CHAT/") ) )
	{
		if ( _tcsistr( strLine, _T("200 OK") ) )
		{
			if ( m_nState == cssRequest2 )
			{
				m_nState = cssHeaders2;
				return TRUE;
			}
			else if ( m_nState == cssRequest3 )
			{
				m_nState = cssHeaders3;
				return TRUE;
			}
		}
		//else // Rejected
	}

	StatusMessage( 1, IDS_CHAT_PRIVATE_REFUSED, (LPCTSTR)m_sAddress );
	Close();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession header processer

BOOL CChatSession::OnHeaderLine(CString& strHeader, CString& strValue)
{
	ASSERT ( m_nProtocol != PROTOCOL_ED2K );

	if ( ! CConnection::OnHeaderLine( strHeader, strValue ) )
		return FALSE;

	if ( strHeader.CompareNoCase( _T("X-Nickname") ) == 0 )
		m_sUserNick = strValue;

	return TRUE;
}

BOOL CChatSession::OnHeadersComplete()
{
	if ( m_nState != cssHeaders3 )
	{
		Write( _P("CHAT/0.2 200 OK\r\n") );

		if ( m_nProtocol == PROTOCOL_G2 )
		{
			Write( _P("Accept: application/x-gnutella2\r\n"
				"Content-Type: application/x-gnutella2\r\n") );
		}
		else if ( MyProfile.IsValid() )
		{
			Write( _P("X-Nickname: ") );
			Write( MyProfile.GetNick().Left( 255 ) );
			Write( _P("\r\n") );
		}

		Write( _P("User-Agent: ") );
		Write( Settings.SmartAgent() );
		Write( _P("\r\n\r\n") );

		LogOutgoing();

		OnWrite();
	}

	if ( m_nState == cssHeaders1 )
	{
		// Sent second handshake
		m_nState = cssRequest3;
		return TRUE;
	}
	else if ( m_nState == cssHeaders2 )
	{
		// Sent third handshake
		m_nState = cssHandshake;
		return OnEstablished();
	}
	else if ( m_nState == cssHeaders3 )
	{
		// Connected
		m_nState = cssHandshake;
		return OnEstablished();
	}

	ASSERT( FALSE );
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession startup

BOOL CChatSession::OnEstablished()
{
	ASSERT ( m_nProtocol != PROTOCOL_ED2K );

	m_tConnected = GetTickCount();

	if ( m_nProtocol == PROTOCOL_G2 )
	{
		StatusMessage( 0, IDS_CHAT_HANDSHAKE_G2 );
		Send( CG2Packet::New( G2_PACKET_PROFILE_CHALLENGE ) );
	}
	else
	{
		m_nState = cssActive;
		StatusMessage( 2, IDS_CHAT_HANDSHAKE_G1, m_bOld ? _T("0.1") : _T("0.2") );
		if ( m_pWndPrivate != NULL ) m_pWndPrivate->OnProfileReceived();
		//StatusMessage( 0, 0 );
		PostOpenWindow();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession ED2K packet interface

// The ED2K packets are put into the input/output buffers, then handled by ReadPacketsED2K()
// and SendPacketsED2K().  Those functions are called by the OnRun(), since there isn't a valid
// connection/socket associated with an ED2K client chat session. (It's handled by the EDClient)

BOOL CChatSession::ReadPacketsED2K()
{
	BOOL bSuccess = TRUE;

	ASSERT( IsInputExist() );

	CLockedBuffer pInput( GetInput() );

	while ( CEDPacket* pPacket = CEDPacket::ReadBuffer( pInput ) )
	{
		try
		{
			// Note: This isn't a "real" packet parser. Message packets are simply dumped into
			// the input buffer by the EDClient, so all packets should be valid ED2K chat messages.
			if ( ( pPacket->m_nEdProtocol == ED2K_PROTOCOL_EDONKEY ) &&
				( pPacket->m_nType == ED2K_C2C_MESSAGE ) )
			{
				bSuccess = OnChatMessage( pPacket );
			}
			else
			{
				CString str;
				str.Format( _T("Unrecognised packet - IP: %s - in CChatSession::ReadPacketsED2K"),
					LPCTSTR( m_sAddress ) );
				pPacket->Debug( str );
			}
		}
		catch ( CException* pException )
		{
			pException->Delete();
			if ( ! m_oGUID )
				bSuccess = FALSE;
		}

		pPacket->Release();
		if ( ! bSuccess ) break;
	}

	return bSuccess;
}

BOOL CChatSession::SendPacketsED2K()
{
	ASSERT( IsOutputExist() );

	CLockedBuffer pOutput( GetOutput() );

	while ( CEDPacket* pPacket = CEDPacket::ReadBuffer( pOutput ) )
	{
		ASSERT ( pPacket != NULL );

		// Send the message to the appropriate ED2K Client
		if ( SendChatMessage ( pPacket ) )
		{
			// Packet was sent (or exired and should be removed from the queue).
			// Release it and continue processing other packets.
			pPacket->Release();
		}
		else
		{
			// The packet could not be sent. Either a lock couldn't be made,
			// or the client is currently connecting.

			// Put the packet back into the buffer until we are ready to deal with it
			Write( pPacket );
			// We're done with the packet (for now), so release it.
			pPacket->Release();
			// Exit this function now. We can't do anything futher, so would get stuck in a loop
			return TRUE;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession ED2K packet handlers

BOOL CChatSession::SendChatMessage(CEDPacket* pPacket)
{
	// Lock the transfers while we send a message (We need the EDClient)
	CSingleLock pLock( &Transfers.m_pSection );
	if ( ! pLock.Lock( 250 ) ) return FALSE;

	// Try to find an ed2k client
	CEDClient* pClient = EDClients.GetByIP( &m_pHost.sin_addr );

	if ( ( pClient ) && validAndEqual( pClient->m_oGUID, m_oGUID ) )	// Found a client
	{
		if ( pClient->IsOnline() )	// We found a client that's ready to go
		{
			MakeActive();

			// Send the packet to the ed2k client, and report the packet should be removed
			pClient->Send ( pPacket, FALSE );
			return TRUE;
		}
		else if ( ( m_nState != cssConnecting ) && ( pClient->Connect() ) )	// We found a client we need to connect to
		{
			// Set the 'connection' state while we wait for EDClient to do it's job
			m_nState = cssConnecting;
			m_tConnected = GetTickCount();
			StatusMessage( 0, IDS_CHAT_CONNECTING_TO, (LPCTSTR)pClient->m_sAddress );
			// Return false to out the packet back into the buffer until we're ready to send it
			return FALSE;
		}
		else
		{	// We found a client but couldn't start a connection.

			if ( m_nState == cssConnecting )	// We are connecting
			{
				// Check time-out
				if ( ( GetTickCount() - m_tConnected ) >= Settings.Connection.TimeoutConnect )
				{
					// We've timed out. Display an error and drop the message
					StatusMessage( 1, IDS_CHAT_CANT_CONNECT, (LPCTSTR)pClient->m_sAddress );
					m_nState = cssNull;
					return TRUE;
				}
				else
				{
					// Waiting to connect. Put the packet back into the buffer and try later.
					return FALSE;
				}
			}
			else	// We can't connect
			{
				// There is a problem.  Inform the user and drop the message.
				StatusMessage( 1, IDS_CHAT_CANT_CONNECT, (LPCTSTR)pClient->m_sAddress );
				m_nState = cssNull;
				return TRUE;
			}
		}
	}
	else // We don't seem to have a client that matches.
	{
	//	// Make a new client/connection if we can
	//	if ( m_nState != cssConnecting )
	//	{
	//		// If we aren't connecting, try making a new connection
	//		// First, lock the section to prevent a problem with other threads
	//		CSingleLock pLock( &Transfers.m_pSection );
	//		if ( ! pLock.Lock( 250 ) ) return NULL;

	//		// We need to connect to them, so either find or create an EDClient
	//		if ( m_bMustPush )
	//			pClient = EDClients.Connect(m_pHost.sin_addr.S_un.S_addr, m_pHost.sin_port, &m_pServer.sin_addr, m_pServer.sin_port, &m_pGUID );
	//		else
	//			pClient = EDClients.Connect(m_pHost.sin_addr.S_un.S_addr, m_pHost.sin_port, NULL, 0, &m_pGUID );
	//		// If we weren't able to create a client (Low-id and no server), then exit.

	//		if ( ( pClient ) && ( pClient->Connect() ) )
	//		{
	//			pClient->OpenChat();
	//			pLock.Unlock();
	//			// Set the 'connection' state while we wait for EDClient to do it's job
	//			m_nState = cssConnecting;
	//			m_tConnected = GetTickCount();
	//			StatusMessage( 0, IDS_CHAT_NOT_CONNECTED_1 );
	//			// Return false to out the packet back into the buffer until we're ready to send it
	//			return FALSE;
	//		}
	//	}

		// Inform the user and drop the message.
		StatusMessage( 1, IDS_CHAT_DROPPED );
		m_nState = cssNull;
		return TRUE;
	}
}

BOOL CChatSession::OnChatMessage(CEDPacket* pPacket)
{
	DWORD nMessageLength;
	CString sMessage;

	// Note: The message packet has already been validated by the EDClient.

	// Read message length
	nMessageLength = pPacket->ReadShortLE();

	// Read in message
	if ( m_bUnicode )
		sMessage = pPacket->ReadStringUTF8( nMessageLength );
	else
		sMessage = pPacket->ReadStringASCII( nMessageLength );

	// Display message
	if ( m_pWndPrivate != NULL ) m_pWndPrivate->OnRemoteMessage( false, sMessage.GetBuffer() );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession text interface

void CChatSession::Print(LPCTSTR pszString, size_t nLength)
{
	ASSERT( m_nProtocol != PROTOCOL_G2 );
	ASSERT( m_nState >= cssHandshake );

	Write( pszString, nLength );
	OnWrite();
}

BOOL CChatSession::ReadText()
{
	CString strLine;

	while ( Read( strLine ) )
	{
		if ( ! OnText( strLine ) )
		{
			Close();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CChatSession::OnText(const CString& str)
{
	if ( m_pWndPrivate == NULL ) return TRUE;

	if ( m_bOld )
	{
		if ( ::StartsWith( str, _PT("\001ACTION ") ) )
			m_pWndPrivate->OnRemoteMessage( true, str.Mid( 8 ) );
		else
			m_pWndPrivate->OnRemoteMessage( false, str );
	}
	else if ( ::StartsWith( str, _PT("MESSAGE ") ) )
	{
		if ( ::StartsWith( str, _PT("MESSAGE \001ACTION ") ) )
			m_pWndPrivate->OnRemoteMessage( true, str.Mid( 16 ) );
		else
			m_pWndPrivate->OnRemoteMessage( false, str.Mid( 8 ) );
	}
	else if ( ::StartsWith( str, _PT("NICK ") ) )
	{
		// ToDo: New nick is : str.Mid( 5 )
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession Gnutella2 packet interface

void CChatSession::Send(CG2Packet* pPacket, BOOL bRelease)
{
	ASSERT( m_nProtocol == PROTOCOL_G2 );
	ASSERT( m_nState >= cssHandshake );

	Write( pPacket );
	if ( bRelease ) pPacket->Release();

	OnWrite();
}

BOOL CChatSession::ReadPackets()
{
	CLockedBuffer pInput( GetInput() );

	BOOL bSuccess = TRUE;
	for ( ; bSuccess && pInput->m_nLength ; )
	{
		BYTE nInput = *( pInput->m_pBuffer );

		if ( nInput == 0 )
		{
			pInput->Remove( 1 );
			continue;
		}

		BYTE nLenLen	= ( nInput & 0xC0 ) >> 6;
		BYTE nTypeLen	= ( nInput & 0x38 ) >> 3;
		BYTE nFlags		= ( nInput & 0x07 );

	//	if ( nLenLen == 0 )
	//	{
	//		Close();
	//		return FALSE;
	//	}

		if ( (DWORD)pInput->m_nLength < (DWORD)nLenLen + nTypeLen + 2 ) break;

		DWORD nLength = 0;

		if ( nFlags & G2_FLAG_BIG_ENDIAN )
		{
			BYTE* pLenIn = pInput->m_pBuffer + 1;

			for ( BYTE nIt = nLenLen ; nIt ; nIt-- )
			{
				nLength <<= 8;
				nLength |= *pLenIn++;
			}
		}
		else
		{
			BYTE* pLenIn	= pInput->m_pBuffer + 1;
			BYTE* pLenOut	= (BYTE*)&nLength;
			for ( BYTE nLenCnt = nLenLen ; nLenCnt-- ; ) *pLenOut++ = *pLenIn++;
		}

		if ( nLength >= Settings.Gnutella.MaximumPacket )
		{
			Close();
			return FALSE;
		}

		if ( (DWORD)pInput->m_nLength < (DWORD)nLength + nLenLen + nTypeLen + 2 ) break;

		CG2Packet* pPacket = CG2Packet::New( pInput->m_pBuffer );

		pInput->Remove( nLength + nLenLen + nTypeLen + 2 );

		try
		{
			bSuccess = OnPacket( pPacket );
		}
		catch ( CException* pException )
		{
			pException->Delete();
			bSuccess = TRUE;
		}

		pPacket->Release();
	}

	if ( ! bSuccess ) Close();

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CChatSession Gnutella2 packet handlers

BOOL CChatSession::OnPacket(CG2Packet* pPacket)
{
	switch( pPacket->m_nType )
	{
	case G2_PACKET_CHAT_MESSAGE:
		return OnChatMessage( pPacket );
	case G2_PACKET_PROFILE_CHALLENGE:
		return OnProfileChallenge( pPacket );
	case G2_PACKET_PROFILE_DELIVERY:
		return OnProfileDelivery( pPacket );
	case G2_PACKET_CHAT_REQUEST:
		return OnChatRequest( pPacket );
	case G2_PACKET_CHAT_ANSWER:
		return OnChatAnswer( pPacket );
	//case G2_PACKET_CHAT_AWAY:
	//	return TRUE;	// ToDo: Away?
	}

	CString tmp;
	tmp.Format( _T("Received unexpected G2 Chat packet from %s:%u"),
		(LPCTSTR)CString( inet_ntoa( m_pHost.sin_addr ) ),
		htons( m_pHost.sin_port ) );
	pPacket->Debug( tmp );

	return TRUE;
}

BOOL CChatSession::OnProfileChallenge(CG2Packet* /*pPacket*/)
{
	if ( ! MyProfile.IsValid() ) return TRUE;

	CG2Packet* pProfile = CG2Packet::New( G2_PACKET_PROFILE_DELIVERY, TRUE );
	CString strXML = MyProfile.GetPublicXML( /*m_sUserAgent*/ NULL, TRUE )->ToString( TRUE );

	pProfile->WritePacket( G2_PACKET_XML, pProfile->GetStringLen( strXML ) );
	pProfile->WriteString( strXML, FALSE );

	Send( pProfile, TRUE );

	return TRUE;
}

BOOL CChatSession::OnProfileDelivery(CG2Packet* pPacket)
{
	if ( ! pPacket->m_bCompound ) return TRUE;

	delete m_pProfile;
	m_pProfile = NULL;

	G2_PACKET nType;
	DWORD nLength;

	while ( pPacket->ReadPacket( nType, nLength ) )
	{
		DWORD nOffset = pPacket->m_nPosition + nLength;

		if ( nType == G2_PACKET_XML )
		{
			CXMLElement* pXML = CXMLElement::FromString( pPacket->ReadString( nLength ), TRUE );

			if ( pXML != NULL )
			{
				m_pProfile = new CGProfile();

				if ( m_pProfile == NULL )
				{
					//theApp.Message( MSG_ERROR, _T("Error in CChatSession::OnProfileDelivery()") );
					delete pXML;
				}
				else if ( ! m_pProfile->FromXML( pXML ) || ! m_pProfile->IsValid() )
				{
					delete pXML;
					delete m_pProfile;
					m_pProfile = NULL;
				}
			}
		}

		pPacket->m_nPosition = nOffset;
	}

	if ( m_pProfile == NULL ) return TRUE;

	m_sUserNick = m_pProfile->GetNick();

	m_oGUID = m_pProfile->oGUID;

	if ( m_pWndPrivate != NULL )
	{
		m_pWndPrivate->OnProfileReceived();

		CG2Packet* pReqPacket = CG2Packet::New( G2_PACKET_CHAT_REQUEST, TRUE );
		pReqPacket->WritePacket( G2_PACKET_USER_GUID, 16 );
		pReqPacket->Write( m_oGUID );

		Send( pReqPacket, TRUE );
	}

	return TRUE;
}

BOOL CChatSession::OnChatRequest(CG2Packet* pPacket)
{
	if ( ! pPacket->m_bCompound ) return TRUE;

	Hashes::Guid oGUID;

	G2_PACKET nType;
	DWORD nLength;

	while ( pPacket->ReadPacket( nType, nLength ) )
	{
		DWORD nOffset = pPacket->m_nPosition + nLength;

		if ( nType == G2_PACKET_USER_GUID && nLength >= 16 )
			pPacket->Read( oGUID );

		pPacket->m_nPosition = nOffset;
	}

	CG2Packet* pAnswer = CG2Packet::New( G2_PACKET_CHAT_ANSWER, TRUE );
	pAnswer->WritePacket( G2_PACKET_USER_GUID, 16 );
	const Hashes::Guid oMyGUID = MyProfile.oGUID;
	pAnswer->Write( oMyGUID );

	if ( validAndEqual( oGUID, oMyGUID ) )
	{
		DWORD nIdle = (DWORD)time( NULL ) - theApp.m_nLastInput;

		if ( nIdle > Settings.Community.AwayMessageIdleTime )
		{
			CString strTime;
			if ( nIdle > 86400 )
				strTime.Format( _T("%u:%.2u:%.2u:%.2u"), nIdle / 86400, ( nIdle / 3600 ) % 24, ( nIdle / 60 ) % 60, nIdle % 60 );
			else
				strTime.Format( _T("%u:%.2u:%.2u"), nIdle / 3600, ( nIdle / 60 ) % 60, nIdle % 60 );

			pAnswer->WritePacket( G2_PACKET_CHAT_AWAY, pAnswer->GetStringLen( strTime ) );
			pAnswer->WriteString( strTime, FALSE );
		}
		else
		{
			pAnswer->WritePacket( G2_PACKET_CHAT_ACCEPT, 0 );
		}

		PostOpenWindow();
	}
	else
	{
		pAnswer->WritePacket( G2_PACKET_CHAT_DENY, 0 );
	}

	Send( pAnswer, TRUE );

	return TRUE;
}

BOOL CChatSession::OnChatAnswer(CG2Packet* pPacket)
{
	if ( ! pPacket->m_bCompound )
		return TRUE;

	BOOL ret = TRUE;
	G2_PACKET nType;
	DWORD nLength;

	while ( pPacket->ReadPacket( nType, nLength ) )
	{
		DWORD nOffset = pPacket->m_nPosition + nLength;

		switch ( nType )
		{
		case G2_PACKET_USER_GUID:
			if ( nLength >= 16 )
			{
				Hashes::Guid oGUID;
				pPacket->Read( oGUID );
				if ( oGUID )
					m_oGUID = oGUID;
			}
			break;

		case G2_PACKET_CHAT_ACCEPT:
			m_nState = cssActive;
			StatusMessage( 2, IDS_CHAT_PRIVATE_ONLINE, m_sUserNick );
			break;

		case G2_PACKET_CHAT_DENY:
			StatusMessage( 1, IDS_CHAT_PRIVATE_REFUSED, m_sUserNick );
			ret = FALSE; // Close connection
			break;

		case G2_PACKET_CHAT_AWAY:
			m_nState = cssAway;
			StatusMessage( 1, IDS_CHAT_PRIVATE_AWAY, m_sUserNick,
				pPacket->ReadString( nLength ) );
			break;
		}
		pPacket->m_nPosition = nOffset;
	}

	return ret;
}

BOOL CChatSession::OnChatMessage(CG2Packet* pPacket)
{
	if ( ! pPacket->m_bCompound )
		return TRUE;

	bool bAction = false;
	CString strBody;
	G2_PACKET nType;
	DWORD nLength;

	while ( pPacket->ReadPacket( nType, nLength ) )
	{
		DWORD nOffset = pPacket->m_nPosition + nLength;

		switch ( nType )
		{
		case G2_PACKET_BODY:
			strBody = pPacket->ReadString( nLength );
			break;

		case G2_PACKET_CHAT_ACTION:
			bAction = TRUE;
			break;
		}

		pPacket->m_nPosition = nOffset;
	}

	if ( strBody.IsEmpty() )
		return TRUE;

	if ( m_pWndPrivate != NULL )
		m_pWndPrivate->OnRemoteMessage( bAction, strBody );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChatSession message interface

bool CChatSession::SendPrivateMessage(bool bAction, const CString& strText)
{
	CSingleLock pLock( &ChatCore.m_pSection, TRUE );

	if ( m_nState < cssActive )
		return false;

	CString strMessage( strText );
	//if ( m_nState == cssAway )
	//{
	//	CString strNick = MyProfile.GetNick();
	//	if ( strNick.IsEmpty() ) strNick = CLIENT_NAME _T(" User");
	//	strMessage.Format( LoadString( IDS_CHAT_PRIVATE_AWAY ), strNick, strText );
	//}

	if ( m_nProtocol == PROTOCOL_G2 )
	{
		CG2Packet* pPacket = CG2Packet::New( G2_PACKET_CHAT_MESSAGE, TRUE );

		if ( bAction )
			pPacket->WritePacket( G2_PACKET_CHAT_ACTION, 0 );

		pPacket->WritePacket( G2_PACKET_BODY, pPacket->GetStringLen( strMessage ) );
		pPacket->WriteString( strMessage, FALSE );

		Send( pPacket, TRUE );
	}
	else if ( m_nProtocol == PROTOCOL_ED2K )
	{
		// Limit outgoing ed2k messages to shorter than ED2K_MESSAGE_MAX characters, just in case
		strMessage = strText.Left( ED2K_MESSAGE_MAX - 50 );

		// Create an ed2k packet holding the message
		CEDPacket* pPacket = CEDPacket::New( ED2K_C2C_MESSAGE, ED2K_PROTOCOL_EDONKEY );

		if ( m_bUnicode )
		{
			pPacket->WriteShortLE( WORD( pPacket->GetStringLenUTF8( strMessage ) ) );
			pPacket->WriteStringUTF8( strMessage, FALSE );
		}
		else
		{
			pPacket->WriteShortLE( WORD( pPacket->GetStringLen( strMessage ) ) );
			pPacket->WriteString( strMessage, FALSE );
		}

		// A few asserts for debug purposes
		ASSERT( m_nProtocol == PROTOCOL_ED2K );
		ASSERT( pPacket != NULL );
		ASSERT( pPacket->m_nEdProtocol == ED2K_PROTOCOL_EDONKEY );

		// Put the packet into the output buffer
		Write( pPacket );
		pPacket->Release();
	}
	else // PROTOCOL_G1
	{
		if ( bAction )
			strMessage = _T("\001ACTION ") + strText;
		else if ( ! m_bOld )
			strMessage = _T("MESSAGE ") + strText;

		strMessage += _T("\r\n");

		Print( strMessage, strMessage.GetLength() );
	}

	return true;
}

//bool CChatSession::SendAwayMessage(const CString& strText)
//{
//	int nOldState = m_nState;
//	m_nState = cssAway;
//	bool bResult = SendPrivateMessage( false, strText );
//	m_nState = nOldState;
//
//	return bResult;
//}

//////////////////////////////////////////////////////////////////////
// CChatSession status message

void CChatSession::StatusMessage(int nFlags, UINT nID, ...)
{
	CString strMessage, strFormat = LoadString( nID );
	va_list pArgs;

	va_start( pArgs, nID );

	if ( strFormat.Find( _T("%1") ) >= 0 )
	{
		LPTSTR lpszTemp = NULL;
		if ( ::FormatMessage( FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			strFormat, 0, 0, (LPTSTR)&lpszTemp, 0, &pArgs ) != 0 && lpszTemp != NULL )
		{
			strMessage = lpszTemp;
			LocalFree( lpszTemp );
		}
	}
	else
	{
		strMessage.FormatV( strFormat, pArgs );
	}

	va_end( pArgs );

	if ( m_pWndPrivate != NULL )
		m_pWndPrivate->OnStatusMessage( nFlags, strMessage );
}

//////////////////////////////////////////////////////////////////////
// CChatSession chat window interface

void CChatSession::PostOpenWindow()
{
	if ( m_pWndPrivate != NULL || m_pWndPublic != NULL ) return;

	PostMainWndMessage( WM_OPENCHAT, (WPARAM)this );
}

void CChatSession::OnOpenWindow()
{
	ASSERT( m_pWndPrivate == NULL && m_pWndPublic == NULL );

	if ( m_oGUID )
		m_pWndPrivate = ChatWindows.FindPrivate( m_oGUID );
	else
		m_pWndPrivate = ChatWindows.FindPrivate( &m_pHost.sin_addr );

	if ( m_pWndPrivate == NULL && m_nProtocol == PROTOCOL_ED2K )
	{
		if ( m_bMustPush )
			m_pWndPrivate = ChatWindows.FindED2KFrame( m_nClientID, &m_pServer );
		else
			m_pWndPrivate = ChatWindows.FindED2KFrame( &m_pHost );
	}

	if ( m_pWndPrivate == NULL )
		m_pWndPrivate = new CPrivateChatFrame();

	if ( ! m_pWndPrivate->Accept( this ) )
	{
		m_pWndPrivate = new CPrivateChatFrame();
		m_pWndPrivate->Accept( this );
	}

	m_pWndPrivate->OnProfileReceived();

	StatusMessage( 2, IDS_CHAT_PRIVATE_ONLINE, (LPCTSTR)m_sUserNick );
	//StatusMessage( 0, 0 );

	PlaySound( _T("Sound_IncomingChat"), NULL, SND_APPLICATION|SND_ALIAS|SND_ASYNC );

	m_nState = cssActive;

	// Hack to open it
	CWnd* pParent = m_pWndPrivate->GetParent();
	if ( pParent->IsIconic() )
		pParent->ShowWindow( SW_SHOWNORMAL );
	pParent->BringWindowToTop();
	pParent->SetForegroundWindow();
}

void CChatSession::OnCloseWindow()
{
	m_pWndPrivate = NULL;
	m_pWndPublic = NULL;

	Close();

	if ( m_nProtocol == PROTOCOL_ED2K )
		delete this;
}
