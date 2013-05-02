//
// Handshakes.cpp
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

// CHandshakes listens for remote computers that want to connect to us
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CHandshakes
// http://peerproject.org/shareazawiki/Developers.Code.CHandshakes.html

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "Handshakes.h"
#include "Handshake.h"
#include "Connection.h"
#include "Network.h"
#include "Security.h"
#include "Datagrams.h"
#include "DiscoveryServices.h"
#include "Transfers.h"
#include "Uploads.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

CHandshakes Handshakes;

//////////////////////////////////////////////////////////////////////
// CHandshakes construction

CHandshakes::CHandshakes()
	: m_nStableCount	( 0 )
	, m_tStableTime 	( 0 )
	, m_hSocket 		( INVALID_SOCKET )
{
}

CHandshakes::~CHandshakes()
{
	// Have the Disconnect method put everything away
	//Disconnect();

	// Make sure it set the socket to invalid, and the thread to null
	ASSERT( ! IsValid() );
	ASSERT( m_pList.GetCount() == 0 );
}

//////////////////////////////////////////////////////////////////////
// CHandshakes listen

// Use the socket in the CHandshakes object to listen for remote computers who want to connect to us
// Returns true if we're listening, false if it didn't work
BOOL CHandshakes::Listen()
{
	// Setup m_hSocket as a new TCP socket
	if ( IsValid() )		// Make sure the socket hasn't been created yet
	{
		theApp.Message( MSG_ERROR, _T("Re-connection too fast, waiting 2 seconds.") );
		Sleep(2000);		// Too fast re-connection after disconnection, wait 2 sec
		if ( IsValid() )
			return FALSE;
	}

	// Make sure only one thread can execute the code of this method at a time
	// When the method exits, local pLock will be destructed, and the lock released
	CSingleLock pLock( &m_pSection, TRUE );

	m_hSocket = socket( 	// Create a socket
		PF_INET,			// Specify the Internet address family
		SOCK_STREAM,		// Use TCP and not UDP
		IPPROTO_TCP );
	if ( ! IsValid() )		// Now, make sure it has been created
	{
		theApp.Message( MSG_ERROR, _T("Failed to create socket. (1st Try)") );
		// Second attempt
		m_hSocket = socket(	// Create a socket
			PF_INET,		// Specify the Internet address family
			SOCK_STREAM,	// Use TCP and not UDP
			IPPROTO_TCP );
		if ( ! IsValid() )
		{
			theApp.Message( MSG_ERROR, _T("Failed to create socket. (2nd Try)") );
			return FALSE;
		}
	}

	// Get our computer's Internet IP address and port number from the network object
	SOCKADDR_IN saListen = Network.m_pHost; 	// This is the address of our computer as visible to remote computers on the Internet

	// If the program connection settings disallow binding, zero the 4 bytes of the IP address
	if ( ! Settings.Connection.InBind )
	{
		saListen.sin_addr.s_addr = INADDR_ANY;	// s_addr is the IP address formatted as a single u_long
	}
	else
	{
		// Set the exclusive address option
		BOOL bVal = TRUE;
		setsockopt( m_hSocket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&bVal, sizeof( bVal ) );
	}

	BOOL bBound = FALSE;	// We're not bound to this socket yet

	// Loop to try 5 times since the socket might not be reused immediately
	for ( int nAttempt = 0 ; nAttempt < 5 ; nAttempt++ )
	{
		// Bind our IP address to the socket, Windows Firewall may pop a message to the user when a program calls bind
		bBound = bind(				// Call bind to associate our local address wiht this socket
			m_hSocket,				// The socket in this CHandshakes object
			(SOCKADDR*)&saListen,	// Our Internet IP address, the one we want to listen on, and how big it is
			sizeof( saListen )
			) == 0;					// If bind succeeds, it returns 0, and bBound becomes true
		if ( bBound ) break;		// We're done trying to bind, leave the loop

		// Record that we weren't able to listen on this port
		theApp.Message( MSG_ERROR, IDS_NETWORK_CANT_LISTEN, (LPCTSTR)CString( inet_ntoa( saListen.sin_addr ) ), htons( saListen.sin_port ) );

		// If this is not our first attempt
		if ( nAttempt )
		{
			if ( Network.m_bUPnPPortsForwarded != TRI_TRUE )
			{
				int nPort = Network.RandomPort();
				Network.m_pHost.sin_port = saListen.sin_port = htons( u_short( nPort ) );
			}
		}
		else // This is still our first time here in this loop
		{
			// We set s_addr to INADDR_ANY, which allows it to be any IP address,
			// since we don�t really care about the IP address if we are just telling
			// WinSock which port we want our side of the connection to be.
			saListen.sin_addr.s_addr = INADDR_ANY;
		}
	}

	// Set it up so that when a remote computer connects to us, the m_pWakeup event is fired
	WSAEventSelect( m_hSocket, GetWakeupEvent(), FD_ACCEPT );
		// Specify an event object to associate with the specified set of FD_XXX network events
		// Our listening socket; Our CEvent object member variable; network event to trigger this is our accepting a remote computer's connection

	// Have the socket wait, listening for remote computer on the Internet to connect to it
	listen( m_hSocket, 256 );
		// Place a socket in a state in which it is listening for an incoming connection
		// Our socket; Maximum length of the queue of pending connections, let 256 computers try to call us at once (do)

	Network.AcquireLocalAddress( m_hSocket );

	// If we were able to bind the socket to our local address
	if ( bBound )	// Report that we are now listening on our IP address
		theApp.Message( MSG_INFO, IDS_NETWORK_LISTENING_TCP, (LPCTSTR)CString( inet_ntoa( Network.m_pHost.sin_addr ) ), htons( Network.m_pHost.sin_port ) );

	// Create a new thread to run the ThreadStart method, passing it a pointer to this C
	return BeginThread( "Handshakes" );
}

//////////////////////////////////////////////////////////////////////
// CHandshakes disconnect

// Closes the socket and kills the thread
void CHandshakes::Disconnect()
{
	CNetwork::CloseSocket( m_hSocket, false );

	CloseThread();

	// Make sure only one thread can execute the remaining code of this method at a time
	// When the method exits, local pLock will be destructed, and the lock released
	CSingleLock pLock( &m_pSection, TRUE );

	// Zero the statistics about how many connections we've received
	m_nStableCount	= 0;	// Reset our count of how many connections we've accepted
	m_tStableTime	= 0;

	// Delete all the handshake objects in the list, and the list itself
	for ( POSITION pos = m_pList.GetHeadPosition() ; pos ; )
		delete m_pList.GetNext( pos );	// Delete each handshake object
	m_pList.RemoveAll();				// Remove all the pointers
}

//////////////////////////////////////////////////////////////////////
// CHandshakes push connection

// Takes an IP address and port number, and the Gnutella index (do)
// Connects to the remote computer, and makes a CHandshake object about it in m_pList
// Returns true if the connection is made, false if we couldn't connect
BOOL CHandshakes::PushTo(IN_ADDR* pAddress, WORD nPort, DWORD nIndex)
{
	CSingleLock pLock1( &Transfers.m_pSection );
	if ( pLock1.Lock( 250 ) && Uploads.AllowMoreTo( pAddress ) )
	{
		pLock1.Unlock();

		// Make a new CHandshake object, and open a connection to the computer at pAddress and pPort
		if ( CHandshake* pHandshake = new CHandshake() )
		{
			if ( pHandshake->Push( pAddress, nPort, nIndex ) )
			{
				Add( pHandshake );
				return TRUE;
			}

			delete pHandshake;
		}
	}
	else
		theApp.Message( MSG_ERROR, IDS_UPLOAD_PUSH_BUSY, (LPCTSTR)CString( inet_ntoa( *pAddress ) ) );

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CHandshakes connection test

// Takes an IP address
// Searches the list of handshake objects to see if we are connected to it
// Returns true if its in the list, false if we couldn't find it
BOOL CHandshakes::IsConnectedTo(const IN_ADDR* pAddress) const
{
	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION pos = m_pList.GetHeadPosition() ; pos ; )
	{
		const CHandshake* pHandshake = m_pList.GetNext( pos );

		// If the IP address in the list handshake object matches the one given this method, we've found it
		if ( pHandshake->m_pHost.sin_addr.S_un.S_addr == pAddress->S_un.S_addr )
			return TRUE;
	}

	return FALSE;	// Not found
}

//////////////////////////////////////////////////////////////////////
// CHandshakes list modification

void CHandshakes::Add(CHandshake* pHandshake)
{
	CSingleLock pLock( &m_pSection, TRUE );

	ASSERT( m_pList.Find( pHandshake ) == NULL );
	m_pList.AddTail( pHandshake );
}

// Takes a pointer to a handshake object in the list
// Removes the pointer from the list and deletes the object
void CHandshakes::Remove(CHandshake* pHandshake)
{
	CSingleLock pLock( &m_pSection, TRUE );

	POSITION pos = m_pList.Find( pHandshake );
	ASSERT( pos != NULL );
	if ( pos )
		m_pList.RemoveAt( pos );

	delete pHandshake;
}

//////////////////////////////////////////////////////////////////////
// CHandshakes thread run

// The thread runs this method
// Accept incoming connections from remote computers and figure out what they want
void CHandshakes::OnRun()
{
	// Loop while the socket is valid
	while ( IsThreadEnabled() && IsValid() )
	{
		// Wait for a computer to call us, which fires the wakeup event
		Doze( 1000 );	// Give up after a second

		// Accept the connection from the remote computer, making a new CHandshake object for it in the list
		while ( AcceptConnection() );

		// Send and receive data with each remote computer in the list
		RunHandshakes();

		// If we've listened for and accepted at least one connection, update the discovery services
		RunStableUpdate();
	}
}

//////////////////////////////////////////////////////////////////////
// CHandshakes run the handshake

// Send and receive data with each remote computer in the list
void CHandshakes::RunHandshakes()
{
	// Make sure only one thread can execute the code of this method at a time
	CSingleLock pLock( &m_pSection );
	if ( ! pLock.Lock( 250 ) )
		return;

	// Loop through each CHandshake object in the m_pList list of pointers to them
	for ( POSITION posNext = m_pList.GetHeadPosition() ; posNext ; )
	{
		CHandshake* pHandshake = m_pList.GetNext( posNext );

		// Send and receive data write with the remote computer, deleting the handshake object if we loose the connection
		if ( ! pHandshake->DoRun() )
			Remove( pHandshake );
	}
}

//////////////////////////////////////////////////////////////////////
// CHandshakes accept a connection

// We're listening on m_hSocket, a remote computer calls us, this method accepts the connection-
// Making a new CHandshake object in the list for it.  Returns true if we accepted the connection.
BOOL CHandshakes::AcceptConnection()
{
	SOCKADDR_IN pHost = { AF_INET };
	SOCKET hSocket = CNetwork::AcceptSocket( m_hSocket, &pHost, AcceptCheck );
	if ( hSocket == INVALID_SOCKET )
		return FALSE;

	Network.AcquireLocalAddress( hSocket );

	// We've listened for and accepted one more stable connection
	// Use an interlocked function to do this in a thread-safe way
	InterlockedIncrement( (PLONG)&m_nStableCount );

	// Skip if remote computer's IP address is blocked  (Redundant check?)
	//if ( Security.IsDenied( &pHost.sin_addr ) )
	//{
	//	CNetwork::CloseSocket( hSocket, true );
	//
	//	// Report that this connection was denied for security reasons
	//	theApp.Message( MSG_ERROR, IDS_NETWORK_SECURITY_DENIED, (LPCTSTR)CString( inet_ntoa( pHost.sin_addr ) ) );
	//	return TRUE;
	//}

	// Setup the socket so when there is data to read or write, or it closes, the m_pWakeup event happens
	WSAEventSelect(							// Associate the m_pWakeup event with the FD_READ, FD_WRITE, and FD_CLOSE events
		hSocket,							// The local socket we just made when accepting a new connection
		GetWakeupEvent(),					// The handshakes object's wakeup event
		FD_READ | FD_WRITE | FD_CLOSE );	// Make the event happen when the socket is ready to read, write, or has closed

	if ( CHandshake* pHandshake = new CHandshake( hSocket, &pHost ) )
	{
		Add( pHandshake );
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CHandshakes inbound connection security check callback

// This is the WSAAccept condition function, that's why it has so many weird parameters
// Makes sure we know the remote IP address, and it's not on the security watch list
// Returns CF_ACCEPT or CF_REJECT to tell WSAAccept what to do
int CALLBACK CHandshakes::AcceptCheck(IN LPWSABUF lpCallerId,
	IN LPWSABUF /*lpCallerData*/, IN OUT LPQOS /*lpSQOS*/, IN OUT LPQOS /*lpGQOS*/,
	IN LPWSABUF /*lpCalleeId*/, IN LPWSABUF /*lpCalleeData*/, OUT GROUP FAR * /*g*/,
	IN DWORD_PTR /*dwCallbackData*/)
{
	// If the address of the remote computer is unknown or too short, reject the connection
	if ( lpCallerId == NULL ) return CF_REJECT;							// WSAAccept didn't get the remote computer's IP and port
	if ( lpCallerId->len < sizeof( SOCKADDR_IN ) ) return CF_REJECT;	// The IP and port aren't long enough

	// Copy out the IP address and port number of the remote computer
	const SOCKADDR_IN* pHost = (const SOCKADDR_IN*)lpCallerId->buf;

	// If the remote computer's IP address is on the list of blocked IPs
	if ( Security.IsDenied( &pHost->sin_addr ) )
	{
		// Record we are rejecting this connection because it is on the watch list, and tell WSAAccept to not connect
		theApp.Message( MSG_ERROR, IDS_NETWORK_SECURITY_DENIED, (LPCTSTR)CString( inet_ntoa( pHost->sin_addr ) ) );
		return CF_REJECT;
	}

	return CF_ACCEPT;
}

//////////////////////////////////////////////////////////////////////
// CHandshakes update stable state

// If we've accepted at least one connection, update the discovery services (do)
void CHandshakes::RunStableUpdate()
{
	// We've listened for and accepted at least one stable connection
	if ( m_nStableCount < 1 )
		return;

	// If there's no record yet of when we first connected, set the current time.
	if ( m_tStableTime == 0 )
		m_tStableTime = (DWORD)time( NULL );	// The function time( NULL ) resolves to the number of seconds since 1970

	// Update the discovery services (do)
	DiscoveryServices.Update();
}
