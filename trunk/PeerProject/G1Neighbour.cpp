//
// G1Neighbour.cpp
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

// A CG1Neighbour object represents a remote computer running Gnutella software with which we are exchanging Gnutella packets
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CG1Neighbour

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "Neighbours.h"
#include "G1Neighbour.h"
#include "G1Packet.h"
#include "G2Packet.h"
#include "Network.h"
#include "Handshakes.h"
#include "HostCache.h"
#include "RouteCache.h"
#include "PacketBuffer.h"
#include "Buffer.h"
#include "Security.h"
#include "PongCache.h"
#include "VendorCache.h"
#include "QuerySearch.h"
#include "QueryHit.h"
#include "QueryHashTable.h"
#include "LocalSearch.h"
#include "SearchManager.h"
#include "Downloads.h"
#include "Uploads.h"
#include "Library.h"
#include "WndMain.h"
#include "WndChild.h"
#include "WndSearchMonitor.h"
#include "Statistics.h"
#include "GProfile.h"
#include "GGEP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

//////////////////////////////////////////////////////////////////////
// CG1Neighbour construction

// Takes a CNeighbour object to base this new CG1Neighbour object on
// Creates a new CG1Neighbour object
CG1Neighbour::CG1Neighbour(CNeighbour* pBase)
	: CNeighbour( PROTOCOL_G1, pBase )	// First, call the CNeighbour constructor
{
	CLockedBuffer pOutput( GetOutput() );

	// The member variable m_nPongNeeded is just an array of 32 bytes, start them each out as 0
	ZeroMemory( m_nPongNeeded, PONG_NEEDED_BUFFER );
	// Say we sent a ping packet when we last got any packet from the remote computer (do)
	m_tLastPingOut = m_tLastPacket;
	m_nLastPingHops = 0;

	// Set the hops flow byte to be all 1s (do)
	m_nHopsFlow = 0xFF;

	// Create a new packet buffer for sending packets, giving it the m_pZOutput buffer if we're compressing, or just m_pOutput if we're not
	m_pOutbound = new CG1PacketBuffer( m_pZOutput ? m_pZOutput : pOutput );		// m_pZOutput is where to write data the program will compress

	// Report that a Gnutella connection with the remote computer has been successfully established
	theApp.Message( MSG_INFO, IDS_HANDSHAKE_ONLINE, (LPCTSTR)m_sAddress, 0, 6, m_sUserAgent.IsEmpty() ? _T("Unknown") : (LPCTSTR)m_sUserAgent );

	// Send the remote computer a new Gnutella ping packet
	Send( CG1Packet::New( G1_PACKET_PING ) );
	Statistics.Current.Gnutella1.PingsSent++;

	// If the remote computer told us it supports vendor-spcific messages, and settings allow them
	if ( Settings.Gnutella1.VendorMsg && m_bVendorMsg )
	{
		// Create a new Gnutella packet of type G1_PACKET_VENDOR
		CG1Packet* pVendor = CG1Packet::New( G1_PACKET_VENDOR, 1 );

		// Write the bytes of the packet, text is backwards here to be sent as a group of 4 ASCII bytes in network order
		pVendor->WriteLongLE( 0 );
		pVendor->WriteShortLE( 0 );
		pVendor->WriteShortLE( 0 );
		pVendor->WriteShortLE( 6 );
		pVendor->WriteLongLE( 'RAEB' );	// "BEAR" for BearShare, the bytes are backwards here to be sent in network order
		pVendor->WriteShortLE( 0x0004 );
		pVendor->WriteShortLE( 1 );
		pVendor->WriteLongLE( 'RAEB' );
		pVendor->WriteShortLE( 0x000B );
		pVendor->WriteShortLE( 1 );
		pVendor->WriteLongLE( 'RAEB' );
		pVendor->WriteShortLE( 0x000C );
		pVendor->WriteShortLE( 1 );
		pVendor->WriteLongLE( 'AZAR' );	// "RAZA" for Shareaza
		pVendor->WriteShortLE( 0x0001 );
		pVendor->WriteShortLE( 1 );
		pVendor->WriteLongLE( 'AZAR' );
		pVendor->WriteShortLE( 0x0002 );
		pVendor->WriteShortLE( 1 );
		pVendor->WriteLongLE( 'AZAR' );
		pVendor->WriteShortLE( 0x0003 );
		pVendor->WriteShortLE( 1 );
		pVendor->WriteLongLE( 'REEP' );	// "PEER" for PeerProject
		pVendor->WriteShortLE( 0x0001 );
		pVendor->WriteShortLE( 1 );
		pVendor->WriteLongLE( 'REEP' );
		pVendor->WriteShortLE( 0x0002 );
		pVendor->WriteShortLE( 1 );
		pVendor->WriteLongLE( 'REEP' );
		pVendor->WriteShortLE( 0x0003 );
		pVendor->WriteShortLE( 1 );

		// Send the vendor-specific packet	// ToDo: Is this right?
		Send( pVendor );
	}
}

// Delete this CG1Neighbour object
CG1Neighbour::~CG1Neighbour()
{
	// If we made an outbound packet buffer, delete it
	if ( m_pOutbound ) delete m_pOutbound;
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour read and write events

// Process the packets the remote computer sent us
// Returns false if the remote computer sent us a bad packet
BOOL CG1Neighbour::OnRead()
{
	// Read in data the remote computer sent, and decompress it
	CNeighbour::OnRead();	// Call CNeighbour's OnRead, which calls CConnection's OnRead

	// Have ProcessPackets look at the packets we got, and return the result it returns
	return ProcessPackets();
}

// Sends all the packets in the outbound packet buffer to the remote computer
// Always returns true
BOOL CG1Neighbour::OnWrite()
{
	CLockedBuffer pOutputLocked( GetOutput() );

	// Point pOutput at the buffer where we should write data for the remote computer
	CBuffer* pOutput = m_pZOutput ? m_pZOutput : pOutputLocked;	// If we're sending compressed data, we'll put readable bytes in m_pZOutput and then compress them to m_pOutput

	// Record when OnWrite was called
	DWORD nExpire = GetTickCount();

	// Call CNeighbour::OnWrite to compress the data, and then CConnection::OnWrite to send it into the socket
	CNeighbour::OnWrite();

	// Loop while the output buffer is empty, but the outbound packet buffer still has packets to send
	while ( pOutput->m_nLength == 0 && m_pOutbound->m_nTotal > 0 )
	{
		// Get a packet from the outbound packet buffer
		CG1Packet* pPacket = m_pOutbound->GetPacketToSend( nExpire );	// Tell GetPacketToSend when OnWrite was called
		if ( ! pPacket ) break;	// If the outbound packet buffer didn't give us anything, leave the while loop

		// Write the packet into the output buffer, and release it
		pPacket->ToBuffer( pOutput );
		pPacket->Release();

		// Tell the outbound packet buffer that it's holding one fewer packet
		m_pOutbound->m_nTotal--;

		// Call CNeighbour::OnWrite to compress the data, and then CConnection::OnWrite to send it into the socket
		CNeighbour::OnWrite();
	}

	// Save statistics from the outbound packet buffer into this CG1Neighbour object
	m_nOutbound  = m_pOutbound->m_nTotal;		// Number of packets added in empty array spots
	m_nLostCount = m_pOutbound->m_nDropped;		// Number of packets overwritten

	// Always reports success
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour run event

// CConnection::DoRun calls this
// Makes sure the remote computer hasn't been silent too long, and sends a query patch table
// Returns false if we should disconnect from this remote computer
BOOL CG1Neighbour::OnRun()
{
	// Make sure the remote computer hasn't been silent too long, and send a query patch table
	if ( ! CNeighbour::OnRun() )
		return FALSE;

	// Send a ping if we haven't sent one in awhile
	const DWORD tNow = GetTickCount();
	if ( tNow > m_tLastPingOut + Settings.Gnutella1.PingRate )
		SendPing();

	// Stay connected
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour send packet

// Takes a packet to send, and by default bRelease true to call release on it, and bBuffered false to pass to m_pOutbound->Add
// Returns true if we sent the packet, false if we didn't send it
BOOL CG1Neighbour::Send(CPacket* pPacket, BOOL bRelease, BOOL bBuffered)
{
	// Cast the packet as a Gnutella packet
	CG1Packet* pPacketG1 = (CG1Packet*)pPacket;

	// This method will report if it successfully sent the packet or not
	BOOL bSuccess = FALSE;

	// If we're done with the handshake, the packet protocol is for Gnutella, and it still has time to live
	if ( m_nState >= nrsConnected && pPacket->m_nProtocol == PROTOCOL_G1 )
	{
		// Count that we sent one more packet
		m_nOutputCount++;							// To this remote computer
		Statistics.Current.Gnutella1.Outgoing++;	// Total

		// Add the packet to the outbound packet buffer, and send all the data to the remote computer soon
		m_pOutbound->Add( pPacketG1, bBuffered );
		QueueRun(); // (do)

		// Show this packet to all the windows on the tab bar
		pPacketG1->SmartDump( &m_pHost, FALSE, TRUE, (DWORD_PTR)this );

		// Record that we sent the packet
		bSuccess = TRUE;
	}

	// Release the packet by default
	if ( bRelease ) pPacket->Release();

	// Return true if we sent the packet, false if we didn't
	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour packet dispatch

// Looks at the data the remote computer sent as a bunch of Gnutella packets, and processes and removes them
// Returns false if the remote computer did something weird and we closed the connection, true to keep talking
BOOL CG1Neighbour::ProcessPackets()
{
	CLockedBuffer pInputLocked( GetInput() );

	// Point pInput at the buffer that has readable data from the remote computer
	CBuffer* pInput = m_pZInput ? m_pZInput : pInputLocked;

	// Start out with bSuccess true and loop until it gets set to false
	BOOL bSuccess = TRUE;
	for ( ; bSuccess ; )	// This is the same thing as while ( bSuccess )
	{
		// Look at the input buffer as a Gnutella packet
		GNUTELLAPACKET* pPacket = (GNUTELLAPACKET*)pInput->m_pBuffer;	// Hopefully a packet starts right there
		if ( pInput->m_nLength < sizeof(*pPacket) ) break;				// If there aren't enough bytes in the buffer for a packet, leave the loop

		// Calculate how big this packet is
		DWORD nLength =
			sizeof(*pPacket) +									// The size of a Gnutella packet header, which is the same for all Gnutella packets,
			pPacket->m_nLength; 								// plus the length written in the packet

		// If the length written in the packet is negative or too big
		if ( pPacket->m_nLength < 0 || nLength >= Settings.Gnutella.MaximumPacket )
		{
			// Close our connection to this remote computer
			Close( IDS_PROTOCOL_TOO_LARGE );
			return FALSE;
		}

		// If the whole packet hasn't arrived in the buffer yet, leave the loop
		if ( pInput->m_nLength < nLength ) break;

		// Process the packet
		CG1Packet* pPacketObject = CG1Packet::New( pPacket );	// Look at the start of the buffer as a CG1Packet object
		bSuccess = OnPacket( pPacketObject );					// Send it to OnPacket, and get the result
		pPacketObject->Release();								// Call release on the packet

		// Remove the bytes of the packet from the start of the buffer
		pInput->Remove( nLength );
	}

	// If the loop read all the packets from the input buffer without an error, report success
	if ( bSuccess ) return TRUE;

	// Something in the loop set bSuccess to false
	Close( 0 );													// Close the connection to this remote computer
	return FALSE;												// Report error
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour packet handler

// Takes a pointer to the bytes of a packet sitting in the input buffer that the remote computer just sent us
// Reads it and takes action because of it
// Returns false if the packet is weird and we should disconnect from the remote computer (do)
BOOL CG1Neighbour::OnPacket(CG1Packet* pPacket)
{
	// Count the packet
	m_nInputCount++;											// Count this as one more packet from the remote computer
	m_tLastPacket = GetTickCount();								// Record that we most recently got a packet from this remote computer right now
	Statistics.Current.Gnutella1.Incoming++;					// Count this as one more Gnutella packet the program has received

	// Make sure the packet's time to live count isn't too high
	if ( pPacket->m_nTTL != 0 &&								// It can be sent across the Internet some more, but
		(DWORD)pPacket->m_nTTL + pPacket->m_nHops > Settings.Gnutella1.MaximumTTL &&	// The packet's time to live and hops numbers added together are bigger than settings allow, and
		pPacket->m_nType != G1_PACKET_PUSH && pPacket->m_nType != G1_PACKET_HIT )		// This isn't a push or hit packet
	{
		// Record that the packet has a time to live too high, and set it to 1
		theApp.Message( MSG_ERROR, IDS_PROTOCOL_HIGH_TTL, (LPCTSTR)m_sAddress, pPacket->m_nTTL, pPacket->m_nHops );
		pPacket->m_nTTL = 1;
	}

	// Show this packet to all the windows on the tab bar
	pPacket->SmartDump( &m_pHost, FALSE, FALSE, (DWORD_PTR)this );

	// Sort the packet by type, hand it to the correct packet handler, and return the result from that
	switch ( pPacket->m_nType )
	{
	case G1_PACKET_PING:		return OnPing( pPacket );			// Ping
	case G1_PACKET_PONG:		return OnPong( pPacket );			// Pong, response to a ping
	case G1_PACKET_BYE:			return OnBye( pPacket );			// Bye message
	case G1_PACKET_QUERY_ROUTE:	return OnCommonQueryHash( pPacket ); // Common query hash
	case G1_PACKET_VENDOR:
	case G1_PACKET_VENDOR_APP:	return OnVendor( pPacket );			// Vendor-specific message
	case G1_PACKET_PUSH:		return OnPush( pPacket );			// Push open a connection
	case G1_PACKET_QUERY:		return OnQuery( pPacket );			// Search query
	case G1_PACKET_HIT: 		return OnHit( pPacket );			// Hit, a search result
	}

	// If control makes it here, the Gnutella packet had an unkown type, document it
	theApp.Message( MSG_ERROR, IDS_PROTOCOL_UNKNOWN, (LPCTSTR)m_sAddress, pPacket->m_nType );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour PING packet handlers

// Takes the time right before this is called, and the same if this is called in a loop, and a fake GUID from the network object
// Makes a ping packet and sends it to the remote computer, returns false on error
BOOL CG1Neighbour::SendPing(const Hashes::Guid& oGUID)
{
	// We are a Gnutella ultrapeer and this connection is to a leaf below us, and we have a Gnutella ID GUID, report error
	if ( m_nNodeType == ntLeaf && bool( oGUID ) )
		return FALSE;

	// If the CNeighbours object says we need more Gnutella hubs or leaves, set bNeedPeers to true
	bool bNeedHubs  = Neighbours.NeedMoreHubs( PROTOCOL_G1 ) == TRUE;
	bool bNeedLeafs = Neighbours.NeedMoreLeafs( PROTOCOL_G1 ) == TRUE;
	bool bNeedPeers = bNeedHubs || bNeedLeafs;

	// Send the remote computer a new Gnutella ping packet
	if ( CG1Packet* pPacket = CG1Packet::New( G1_PACKET_PING, ( bool( oGUID ) || bNeedPeers ) ? 0 : 1, oGUID ) )
	{
		// Send "Supports Cached Pongs" extension along with a packet, to receive G1 hosts for cache
		if ( Settings.Gnutella1.EnableGGEP )
		{
			CGGEPBlock pBlock;
			if ( CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_SUPPORT_CACHE_PONGS ) )
			{
				pItem->WriteByte( Neighbours.IsG1Ultrapeer() ? GGEP_SCP_ULTRAPEER : GGEP_SCP_LEAF );
			}
			if ( Settings.Experimental.EnableDIPPSupport )
				pBlock.Add( GGEP_HEADER_SUPPORT_GDNA );
			pBlock.Write( pPacket );
		}

		Send( pPacket, TRUE, TRUE );

		Statistics.Current.Gnutella1.PingsSent++;

		m_tLastPingOut = GetTickCount();
	}

	return TRUE;
}

// Takes a pointer to the bytes of a ping packet from the remote computer, sitting in the input buffer
// Responds to it with a pong packet (Always returns true)
BOOL CG1Neighbour::OnPing(CG1Packet* pPacket)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	Statistics.Current.Gnutella1.PingsReceived++;

	if ( ! Neighbours.AddPingRoute( pPacket->m_oGUID, this ) )
	{
		// Add the ping's GUID to the neighbours route cache, and if it returns false
		// Record this as a dropped packet, but don't report error
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;
	}

	// Note here, a ping packet is just a header, and shouldn't have length

	// If the packet has 1 hop left to live, and has traveled 0 hops yet somehow was sent to us, it must be a keep alive packet
	const BOOL bIsKeepAlive = ( pPacket->m_nTTL == 1 && pPacket->m_nHops == 0 );

	const DWORD tNow = GetTickCount();

	// If we got the most recent ping less than 3 seconds ago, and this isn't a keep alive packet
	if ( tNow <= m_tLastPingIn + Settings.Gnutella1.PingFlood && ! bIsKeepAlive )
	{
		// Drop it, but stay connected
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;
	}

	bool bSCP = false;
	bool bDNA = false;

	// If this ping packet strangely has length, and the remote computer does GGEP blocks
	if ( pPacket->m_nLength && m_bGGEP )
	{
		// There is a GGEP block here, and checking and adjusting the TTL and hops counts worked
		CGGEPBlock pGGEP;
		if ( pGGEP.ReadFromPacket( pPacket ) )
		{
			if ( CGGEPItem* pItem = pGGEP.Find( GGEP_HEADER_SUPPORT_CACHE_PONGS ) )
				bSCP = true;

			if ( CGGEPItem* pItem = pGGEP.Find( GGEP_HEADER_SUPPORT_GDNA ) )
				bDNA = true;

			if ( pPacket->Hop() )	// Calling Hop makes sure TTL is 2+ and then moves a count from TTL to hops
			{
				// Broadcast the packet to the computers we are connected to
				int nCount = Neighbours.Broadcast( pPacket, this, TRUE );
				if ( nCount )
				{
					Statistics.Current.Gnutella1.Routed++;	// Record we routed one more packet
					Statistics.Current.Gnutella1.PingsSent += nCount;
				}

				// Undo what calling Hop did, making the packet's TTL and hop counts are the same as before we called Hop
				pPacket->m_nHops--;
				pPacket->m_nTTL++;
			}
		}
		else
		{
			// It's not, drop the packet, but stay connected
			theApp.Message( MSG_ERROR, IDS_PROTOCOL_GGEP_REQUIRED, (LPCTSTR)m_sAddress );
			Statistics.Current.Gnutella1.Dropped++;
			m_nDropCount++;
			return TRUE;
		}
	}

	CGGEPBlock pGGEP;

	// ToDo: Gnutella 1 DHT
	//if ( Settings.Gnutella1.EnableDHT )
	//{
	//	if ( CGGEPItem* pItem = pGGEP.Add( GGEP_HEADER_DHT_SUPPORT ) )
	//	{
	//		pItem->Write( , 3 );
	//	}
	//}

	if ( CGGEPItem* pItem = pGGEP.Add( GGEP_HEADER_DAILY_AVERAGE_UPTIME ) )
	{
		pItem->WriteVary( Network.GetStableTime() );
	}

	// ToDo: Gnutella 1 Client Locale
	//if ( CGGEPItem* pItem = pGGEP.Add( GGEP_HEADER_CLIENT_LOCALE ) )
	//{
	//	pItem->Write( "en#", 4 );
	//}

	// ToDo: Gnutella 1 TLS Encyption
	//if ( Settings.Gnutella1.EnableTLS )
	//	pGGEP.Add( GGEP_HEADER_TLS_SUPPORT ) );

	if ( CGGEPItem* pItem = pGGEP.Add( GGEP_HEADER_VENDOR_INFO ) )
	{
		pItem->Write( VENDOR_CODE, 4 );
		pItem->WriteByte( ( theApp.m_nVersion[ 0 ] << 4 ) | theApp.m_nVersion[ 1 ] );
	}

	if ( bSCP )
		CG1Packet::GGEPWriteRandomCache( pGGEP, GGEP_HEADER_PACKED_IPPORTS );

	if ( bDNA && Settings.Experimental.EnableDIPPSupport )
		CG1Packet::GGEPWriteRandomCache( pGGEP, GGEP_HEADER_GDNA_PACKED_IPPORTS );

	// Get statistics about how many files we are sharing
	QWORD nMyVolume = 0;
	DWORD nMyFiles = 0;
	LibraryMaps.GetStatistics( &nMyFiles, &nMyVolume );

	// Save information from this ping packet in the CG1Neighbour object
	m_tLastPingIn	= tNow; 						// Record that we last got a ping packet from this remote computer right now
	m_nLastPingHops	= pPacket->m_nHops + 1; 		// Save the hop count from the packet, making it one more (do)
	m_pLastPingID	= pPacket->m_oGUID; 			// Save the packet's GUID

	// If the ping can travel 2 more times, and hasn't traveled at all yet
	if ( pPacket->m_nTTL == 2 && pPacket->m_nHops == 0 )
	{
		// Loop once for each computer we are connected to
		for ( POSITION pos = Neighbours.GetIterator() ; pos ; )
		{
			// Get a pointer to a computer we are connected to
			CNeighbour* pConnection = Neighbours.GetNext( pos );
			if ( pConnection->m_nState != nrsConnected ) continue;		// If that didn't work out, try the loop with the next computer

			// Make a new pong packet, the response to a ping (with same hops count and GUID)
			CG1Packet* pPong = CG1Packet::New( G1_PACKET_PONG, m_nLastPingHops, m_pLastPingID );

			// Tell the remote computer it's IP address and port number in the payload bytes of the pong packet
			pPong->WriteShortLE( htons( pConnection->m_pHost.sin_port ) );		// Port number, 2 bytes reversed
			pPong->WriteLongLE( pConnection->m_pHost.sin_addr.S_un.S_addr );	// IP address, 4 bytes

			// Then, write in the information about how many files we are sharing
			pPong->WriteLongLE( nMyFiles );
			pPong->WriteLongLE( (DWORD)nMyVolume );

			if ( ! pGGEP.IsEmpty() && m_bGGEP )
				pGGEP.Write( pPong );				// Write GGEP stuff

			// Send the pong packet to the remote computer we are currently looping on
			Send( pPong );
			Statistics.Current.Gnutella1.PongsSent++;
		}

		return TRUE;	// We're done
	}

	// The ping can only once more or is dead, or it has already traveled across the Internet, and
	// either this is a keep alive packet, or we're listening for connections and this remote computer is a Gnutella
	if ( bIsKeepAlive || ( Network.IsListening() && ! Neighbours.IsG1Leaf() ) )
	{
		// Make a new pong packet, the response to a ping (with same hops count and GUID)
		CG1Packet* pPong = CG1Packet::New( G1_PACKET_PONG, m_nLastPingHops, m_pLastPingID );

		// Start the pong's payload with the IP address and port number from the Network object (do)
		pPong->WriteShortLE( htons( Network.m_pHost.sin_port ) );
		pPong->WriteLongLE( Network.m_pHost.sin_addr.S_un.S_addr );

		// Then, write in the information about how many files we are sharing
		pPong->WriteLongLE( nMyFiles );
		pPong->WriteLongLE( (DWORD)nMyVolume );

		if ( ! pGGEP.IsEmpty() && m_bGGEP )
			pGGEP.Write( pPong );					// Write GGEP stuff

		// Send the pong packet to the remote computer we are currently looping on
		Send( pPong );
		Statistics.Current.Gnutella1.PongsSent++;
	}

	// Hop the packet, or determine that we are done and leave returning true
	if ( bIsKeepAlive ||							// If this is a keep alive packet
		 m_nNodeType == ntHub ||					// Or, we are a leaf, and the remote computer is to a ultrapeer above us
		 ! pPacket->Hop() )							// Or, the packet is dead or can only travel 1 more time
		return TRUE;								// We're done

	// Tell the neighbours object we just got a Gnutella ping (do)
	Neighbours.OnG1Ping();

	// Make a local CPtrList, the MFC collection class
	CList< CPongItem* > pIgnore;

	// Zero the 32 bytes of the m_nPongNeeded buffer
	ZeroMemory( m_nPongNeeded, PONG_NEEDED_BUFFER );

	// Loop nHops from 1 through the packet's TTL
	for ( BYTE nHops = 1 ; nHops <= pPacket->m_nTTL ; nHops++ )
	{
		// Store ratios in the pong needed array based on the ping's TTL (do)
		m_nPongNeeded[ nHops ] = BYTE(				// Set the byte at the nHops position in the array to
			Settings.Gnutella1.PongCount /			// 10 by default
			pPacket->m_nTTL );						// The number of more hops this packet can travel

		// Respond to the packet with a pong item object (do)
		CPongItem* pCache = NULL;
		while (	( m_nPongNeeded[ nHops ] > 0 ) &&	// While that ratio is positive, and
				( pCache = Neighbours.LookupPong( this, nHops, &pIgnore ) ) != NULL )	// Lookup can find this ping
		{
			// Have the pong item prepare a packet, and send it to the remote computer
			Send( pCache->ToPacket( m_nLastPingHops, m_pLastPingID ) );
			Statistics.Current.Gnutella1.PongsSent++;

			// Add this pong item to the ignore list, and adjust the value in the pong needed array (do)
			pIgnore.AddTail( pCache );				// Add a pointer to this CPongItem to the local pointer list of them
			m_nPongNeeded[ nHops ]--;				// Record there is one less pong needed for packets of this hops count (do)
		}
	}

	return TRUE;	// This method only returns true
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour PONG packet handlers

// Takes a pointer to the bytes of a pong packet from the remote computer, and reads information from it
// Always returns true
BOOL CG1Neighbour::OnPong(CG1Packet* pPacket)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	Statistics.Current.Gnutella1.PongsReceived++;

	if ( pPacket->m_nLength < 14 )
	{
		// Pong packets should be 14 bytes long, drop this strange one
		theApp.Message( MSG_ERROR, IDS_PROTOCOL_SIZE_PACKET, (LPCTSTR)m_sAddress, _T("pong") );
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;	// Don't disconnect, though
	}

	// Read information from the pong packet
	WORD  nPort 	= pPacket->ReadShortLE();	// 2 bytes, port number (do) of us? the remote computer? the computer that sent the packet?
	DWORD nAddress	= pPacket->ReadLongLE();	// 4 bytes, IP address
	DWORD nFiles	= pPacket->ReadLongLE();	// 4 bytes, the number of files the source computer is sharing
	DWORD nVolume	= pPacket->ReadLongLE();	// 4 bytes, the total size of all those files

	if ( Security.IsDenied( (IN_ADDR*)&nAddress ) )
	{
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;
	}

	CString strVendorCode;
	CHAR nVersion[ 4 ] = {};
	DWORD nUptime = 0;
	bool bUltrapeer = false;
	bool bGDNA = false;
	int nCachedHostsCount = -1;

	// If the pong is bigger than 14 bytes, and handshake said it supports GGEP blocks
	if ( pPacket->m_nLength > 14 && m_bGGEP )
	{
		// There could be a GGEP block here, try checking and adjusting TTL and hops counts
		CGGEPBlock pGGEP;
		if ( pGGEP.ReadFromPacket( pPacket ) )
		{
			// Read vendor code
			if ( CGGEPItem* pVC = pGGEP.Find( GGEP_HEADER_VENDOR_INFO, 4 ) )
			{
				CHAR szaVendor[ 5 ] = {};
				pVC->Read( szaVendor, 4 );
				strVendorCode = szaVendor;
				if ( pVC->m_nLength == 5 )
				{
					BYTE nVersionPacked = pVC->ReadByte();
					nVersion[ 0 ] = ( nVersionPacked >> 4 ) & 0x0f;
					nVersion[ 1 ] = nVersionPacked & 0x0f;
				}
			}

			// Read daily uptime
			if ( CGGEPItem* pDU = pGGEP.Find( GGEP_HEADER_DAILY_AVERAGE_UPTIME, 1 ) )
				pDU->Read( (void*)&nUptime, 4 );

			if ( CGGEPItem* pUP = pGGEP.Find( GGEP_HEADER_UP_SUPPORT ) )
				bUltrapeer = true;

			if ( CGGEPItem* pGDNA = pGGEP.Find( GGEP_HEADER_SUPPORT_GDNA ) )
				bGDNA = true;

			nCachedHostsCount = CG1Packet::GGEPReadCachedHosts( pGGEP );
		}
		else
		{
			// It's not, drop the packet, but stay connected
			theApp.Message( MSG_ERROR, IDS_PROTOCOL_GGEP_REQUIRED, (LPCTSTR)m_sAddress );
			Statistics.Current.Gnutella1.Dropped++;
			m_nDropCount++;
			return TRUE;
		}
	}

	if ( pPacket->Hop() )	// Calling Hop makes sure TTL is 2+ and then moves a count from TTL to hops
	{
		// Find the CG1Neighbour object that created this pong packet (do)
		CG1Neighbour* pOrigin = Neighbours.GetPingRoute( pPacket->m_oGUID );

		// If we're connected to that computer, and it supports GGEP extension blocks
		if ( pOrigin && pOrigin->m_bGGEP )
		{
			// Send this pong to it
			Statistics.Current.Gnutella1.Routed++;		// Record one more packet was routed
			pOrigin->Send( pPacket, FALSE, TRUE );
			Statistics.Current.Gnutella1.PongsSent++;
		}

		// Calling Hop above moved 1 from TTL to Hops, put the numbers back the way we got them
		pPacket->m_nHops--;
	}

	// If the pong said it's port number is 0, or we know it's IP address is firewalled, set bLocal to true
	BOOL bLocal = ! nPort || Network.IsFirewalledAddress( (IN_ADDR*)&nAddress );

	// If the packet has traveled across the Internet, but the computer that made it is firewalled
	if ( pPacket->m_nHops != 0 && bLocal )
	{
		// Report a zero pong and don't do anything else with this packet
		if ( pPacket->m_nHops ) theApp.Message( MSG_DEBUG, IDS_PROTOCOL_ZERO_PONG, (LPCTSTR)m_sAddress );
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;
	}

	// If the IP address and port number in the pong is reachable
	if ( ! bLocal && ! Network.IsFirewalledAddress( (IN_ADDR*)&nAddress, TRUE ) )
	{
		// If the pong hasn't hopped at all yet, and the address in it is the address of this remote computer
		if ( pPacket->m_nHops == 0 && nAddress == m_pHost.sin_addr.s_addr )
		{
			// Copy the number of files and their total size into this CG1Neighbour object
			m_nFileCount  = nFiles;
			m_nFileVolume = nVolume;

			// Add the IP address and port number to the Gnutella host cache of computers we can try to connect to
			HostCache.Gnutella1.Add( (IN_ADDR*)&nAddress, nPort, 0, strVendorCode, nUptime );

			if ( bGDNA )
				HostCache.G1DNA.Add( (IN_ADDR*)&nAddress, nPort, 0, strVendorCode, nUptime );
		}
		else if ( bUltrapeer )
		{
			// This pong packet wasn't made by the remote computer, just sent to us by it

			// Add the IP address and port number to the Gnutella host cache of computers we can try to connect to
			HostCache.Gnutella1.Add( (IN_ADDR*)&nAddress, nPort, 0, strVendorCode, nUptime );

			if ( bGDNA )
				HostCache.G1DNA.Add( (IN_ADDR*)&nAddress, nPort, 0, strVendorCode, nUptime );
		}
	}

	// Tell the neighbours object about this pong packet (do)
	BYTE nHops = (BYTE)min( pPacket->m_nHops + 1, PONG_NEEDED_BUFFER - 1 );
	if ( ! bLocal )
		Neighbours.OnG1Pong( this, (IN_ADDR*)&nAddress, nPort, nHops + 1, nFiles, nVolume );

	return TRUE;	// Always returns true
}

// Called by CNeighboursWithG1::OnG1Pong (do)
// Takes a pointer to a CPongItem object (do)
// If the pong is needed, sends it to the remote computer (do)
void CG1Neighbour::OnNewPong(const CPongItem* pPong)
{
	// If we need a pong with the number of hops that this one has (do)
	if ( m_nPongNeeded[ pPong->m_nHops ] > 0 )
	{
		// Have the CPongItem object make a packet, and send it to the remote computer
		CG1Neighbour::Send( pPong->ToPacket( m_nLastPingHops, m_pLastPingID ) );
		Statistics.Current.Gnutella1.PongsSent++;

		// Record one less pong with that many hops is needed (do)
		m_nPongNeeded[ pPong->m_nHops ]--;
	}
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour BYE packet handler

// Takes a pointer to packet data the remote computer sent us
// Reads the reason text and number, and records them
// Always returns false to disconnect from the remote computer
BOOL CG1Neighbour::OnBye(CG1Packet* pPacket)
{
	// Setup local variables
	CString strReason;	// The reason the remote computer must disconnect, we'll read this from the byte packet it sent us
	WORD nReason = 0;	// A number code that goes along with the reason

	// If the packet has a payload 3 bytes long or longer
	if ( pPacket->m_nLength >= 3 )
	{
		// Read the reason number, and then the reason text
		nReason   = pPacket->ReadShortLE();			// 2 bytes
		strReason = pPacket->ReadStringASCII(); 	// ASCII byte characters until a null terminating 0 byte
	}

	// Loop the index nChar for each character in the text
	for ( int nChar = 0 ; nChar < strReason.GetLength() ; nChar++ )
	{
		// If this character has a value less than 32, it's a special character, like tab
		if ( strReason[nChar] < 32 )
		{
			// Chop off the text before the weird character
			strReason = strReason.Left( nChar );	// Left clips the given number of characters from the left side of the string
			break;
		}
	}

	// If the text from the packet is blank, or too long, change it to "No Message"
	if ( strReason.IsEmpty() || strReason.GetLength() > 128 ) strReason = _T("No Message");

	// Record the bye message in the program message log
	theApp.Message( MSG_ERROR, IDS_CONNECTION_BYE, (LPCTSTR)m_sAddress, nReason, (LPCTSTR)strReason );

	// Return false to disconnect from the remote computer
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour VENDOR packet handler

// Takes a vendor specific packet
// Sorts it and responds to it
// Always returns true to stay connected to the remote computer
BOOL CG1Neighbour::OnVendor(CG1Packet* pPacket)
{
	// If the packet payload is smaller than 8 bytes, or settings don't allow vendor messages
	if ( pPacket->m_nLength < 8 || ! Settings.Gnutella1.VendorMsg )
	{
		// Don't do anything with this packet
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;	// Stay connected to the remote computer
	}

	// Read the vendor, function, and version numbers from the packet payload
	DWORD nVendor  = pPacket->ReadLongBE(); 	// 4 bytes, vendor code in ASCII characters, like "RAZA" (do)
	WORD nFunction = pPacket->ReadShortLE();	// 2 bytes, function (do)
	WORD nVersion  = pPacket->ReadShortLE();	// 2 bytes, version (do)

	if ( nVendor == 0 && nFunction == 0 )	// If the packet has 0 for the vendor and function (do)
	{
		// Supported vendor messages array (do)
	}
	else if ( nFunction == 0xFFFF )			// The packet has vendor or function numbers, and the 2 bytes of function are all 1s
	{
		// Vendor is 0
		if ( nVendor == 0 )
		{
			// Vendor code query (do)
			CG1Packet* pReply = CG1Packet::New( pPacket->m_nType, 1, pPacket->m_oGUID );	// Create a reply packet
			pReply->WriteLongLE( 0 );
			pReply->WriteShortLE( 0xFFFE );
			pReply->WriteShortLE( 1 );
			pReply->WriteLongBE( 'PEER' );
			pReply->WriteLongBE( 'RAZA' );
			pReply->WriteLongBE( 'BEAR' );
			Send( pReply );		// Send the reply packet to the remote computer
		}
		// Vendor is the ASCII text "RAZA" for Shareaza
		else if ( nVendor == 'RAZA' || nVendor == 'AZAR' )	// It's backwards because of network byte order (Confirm?)
		{
			// Function code query for "RAZA" (do)
			CG1Packet* pReply = CG1Packet::New( pPacket->m_nType, 1, pPacket->m_oGUID );	// Create a reply packet
			pReply->WriteLongBE( 'RAZA' );
			pReply->WriteShortLE( 0xFFFE );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x0001 );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x0002 );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x0003 );
			pReply->WriteShortLE( 1 );
			Send( pReply );		// Send the reply packet to the remote computer
		}
		// Vendor is the ASCII text "PEER" for PeerProject
		else if ( nVendor == 'PEER' || nVendor == 'REEP' )	// It's backwards because of network byte order (Confirm?)
		{
			// Function code query for "PEER" (do)
			CG1Packet* pReply = CG1Packet::New( pPacket->m_nType, 1, pPacket->m_oGUID );	// Create a reply packet
			pReply->WriteLongBE( 'PEER' );
			pReply->WriteShortLE( 0xFFFE );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x0001 );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x0002 );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x0003 );
			pReply->WriteShortLE( 1 );
			Send( pReply ); // Send the reply packet to the remote computer
		}
		// Vendor is the ASCII text "BEAR" for BearShare
		else if ( nVendor == 'BEAR' || nVendor == 'RAEB' )	// It's backwards because of network byte order (Confirm?)
		{
			// Function code query for "BEAR"
			CG1Packet* pReply = CG1Packet::New( pPacket->m_nType, 1, pPacket->m_oGUID );	// Create a reply packet
			pReply->WriteLongBE( 'BEAR' );
			pReply->WriteShortLE( 0xFFFE );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x0004 );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x000B );
			pReply->WriteShortLE( 1 );
			pReply->WriteShortLE( 0x000C );
			pReply->WriteShortLE( 1 );
			Send( pReply );		// Send the reply packet to the remote computer
		}
	}
	else if ( nVendor == 'RAZA' || nVendor == 'PEER')	// The vendor "RAZA" is Shareaza, and the function isn't 0xFFFF
	{
		// Switch on what the function is
		switch ( nFunction )
		{

		// Version Query (do)
		case 0x0001:
			// The version number from the packet is 0 or 1
			if ( nVersion <= 1 )
			{
				// Send a response packet (do)
				CG1Packet* pReply = CG1Packet::New( pPacket->m_nType, 1, pPacket->m_oGUID );
				pReply->WriteLongBE( 'PEER' );
				pReply->WriteShortLE( 0x0002 );
				pReply->WriteShortLE( 1 );
				pReply->WriteShortLE( theApp.m_nVersion[0] );
				pReply->WriteShortLE( theApp.m_nVersion[1] );
				pReply->WriteShortLE( theApp.m_nVersion[2] );
				pReply->WriteShortLE( theApp.m_nVersion[3] );
				Send( pReply );
			}

			break;

		// Version Response (do)
		case 0x0002:
			// The version number we read from the packet is 0 or 1, and there are 8 bytes of payload left to read
			if ( nVersion <= 1 && pPacket->GetRemaining() >= 8 )
			{
				// Read those 8 bytes (do)
				WORD nVersion[4];
				nVersion[0] = pPacket->ReadShortLE();
				nVersion[1] = pPacket->ReadShortLE();
				nVersion[2] = pPacket->ReadShortLE();
				nVersion[3] = pPacket->ReadShortLE();
			}

			break;

		// Cluster Advisor (do)
		case 0x0003:
			// The version number we read from the packet is 0 or 1, and there are 28 bytes of payload left to read
			if ( nVersion <= 1 && pPacket->GetRemaining() >= 28 )
				OnClusterAdvisor( pPacket );	// This is a cluster advisor packet

			break;
		}
	}
	else if ( nVendor == 'BEAR' )	// The vendor is "BEAR" for BearShare
	{
		// Sort by the function number to see what the vendor specific packet from BearShare wants
		switch ( nFunction )
		{

		// Super Pong (do)
		//case 0x0001:
		//	break;

		// Product Identifiers (do)
		//case 0x0003:
		//	break;

		// Hops Flow (do)
		case 0x0004:
			if ( nVersion <= 1 && pPacket->GetRemaining() >= 1 )
				m_nHopsFlow = pPacket->ReadByte();
			break;

		// Horizon Ping (do)
		//case 0x0005:
		//	break;

		// Horizon Pong (do)
		//case 0x0006:
		//	break;

		// Query Status Request (do)
		case 0x000B:
			// If the version is 0 or 1, then we can deal with this
			if ( nVersion <= 1 )
			{
				// Send a response packet (do)
				CG1Packet* pReply = CG1Packet::New( pPacket->m_nType, 1, pPacket->m_oGUID );
				pReply->WriteLongBE( 'BEAR' );
				pReply->WriteShortLE( 0x000C );
				pReply->WriteShortLE( 1 );
				pReply->WriteShortLE( SearchManager.OnQueryStatusRequest( pPacket->m_oGUID ) );
				Send( pReply );
			}
			break;

		// Query Status Response
		//case 0x000C:
		//	break;
		}
	}

	//	ToDo: Other Vendors
	//else if ( nVendor == 'LIME' || nVendor == 'SNOW' )
	//{
	//}
	//else if ( nVendor == 'GTKG' )
	//{
	//}
	//else if ( nVendor == 'GNUC' )
	//{
	//}

	// Always return true to stay connected to the remote computer
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour VENDOR cluster handlers

// Sends a vendor specific cluster advisor packet to the remote computer, which tells it IP addresses to try to connect to
void CG1Neighbour::SendClusterAdvisor()
{
	// Only do this if the remote computer is running PeerProject and the settings here allow custom vendor message packets
	if ( ! m_bClientExtended || ! Settings.Gnutella1.VendorMsg )
		return;

	// Setup local variables
	const DWORD tNow = static_cast< DWORD >( time( NULL ) );	// The time now, when this method was called, which won't change as the loop runs
	CG1Packet* pPacket = NULL;	// A pointer to a Gnutella packet (do)
	WORD nCount = 0;			// Loop up to 20 times

	{
		CQuickLock oLock( HostCache.Gnutella1.m_pSection );

		// Loop through the Gnutella host cache,
		for ( CHostCacheIterator i = HostCache.Gnutella1.Begin() ;
			i != HostCache.Gnutella1.End() && nCount < 20 ; ++i )
		{
			CHostCacheHostPtr pHost = (*i);

			// If host is running PeerProject compatible, was added recently, and we can connect to it (do)
			if ( pHost->m_pVendor && pHost->m_pVendor->m_bExtended &&	// If this host is running PeerProject/Shareaza, and
				pHost->m_tAdded > m_tClusterHost &&						// It was added before m_tClusterHost was sent (do),
				pHost->CanConnect( tNow ) )								// We can connect to it now (do)
			{
				// If there isn't a packet yet, start one
				if ( ! pPacket )
				{
					// Make a new vendor specific packet
					pPacket = CG1Packet::New( G1_PACKET_VENDOR, 1 );
					pPacket->WriteLongBE( 'PEER' ); 	// Back-compatability, vendor code should be "PEER" for PeerProject
					pPacket->WriteShortLE( 0x0003 );	// 3 is the code for a cluster advisor packet
					pPacket->WriteShortLE( 1 );			// Version number is 1
					pPacket->WriteShortLE( 0 ); 		// (do)
				}

				// Add the IP address and port number to the packet
				pPacket->WriteLongLE( pHost->m_pAddress.S_un.S_addr );
				pPacket->WriteShortLE( pHost->m_nPort );

				// Increase the count to make sure we only write 20 IP addresses into the packet
				nCount++;
			}
		}
	}

	// Set m_tClusterHost to now (do)
	m_tClusterHost = GetTickCount();

	// If we prepared a packet with at least 1 IP address in it
	if ( pPacket && nCount )
	{
		// Finish the cluster advisor packet and send it
		m_tClusterSent = m_tClusterHost;				// Record that we sent this remote computer a cluster packet now
		((WORD*)pPacket->m_pBuffer)[4] = nCount;		// Write the number of IP addresses into the packet
		//pPacket->RazaSign();							// Obsolete placeholder does nothing (do)
		Send( pPacket, TRUE, TRUE );					// Send the packet to the remote computer
	}
}

// Takes a vendor specific packet that has been identified as a cluster advisor packet
// Extracts all the IP addresses and port numbers from it, and adds them to the Gnutella host cache
// Always returns true to stay connected to the remote computer
BOOL CG1Neighbour::OnClusterAdvisor(CG1Packet* pPacket)
{
	// RazaVerify placeholder does nothing, and always returns false (do)
	//if ( ! pPacket->RazaVerify() )
	//	return FALSE;

	// Find out how many IP addresses and port numbers are in this packet
	WORD nCount = pPacket->ReadShortLE();				// The first 2 bytes are the number of hosts described
	if ( pPacket->GetRemaining() < nCount * 6u + 20u )	// Make sure the payload is long enough for that many
		return FALSE;

	// Reply to this cluster advisor packet with one of our own
	SendClusterAdvisor();

	// Loop once for each IP address and port number in the packet
	while ( nCount-- )
	{
		// Read the IP address and port number from the packet, and add them to the Gnutella host cache
		DWORD nAddress	= pPacket->ReadLongLE();
		WORD nPort		= pPacket->ReadShortLE();
		HostCache.Gnutella1.Add( (IN_ADDR*)&nAddress, nPort, 0, _T( VENDOR_CODE ) );
	}

	// Record that now was when we last received a cluster advisor packet from the remote computer
	m_tClusterHost = GetTickCount();

	// Always return true to stay connected to the remote computer
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour PUSH packet handler

// Takes a pointer to the bytes of a push packet the remote computer sent us
// If the push is for us, pushes open a new connection, if not, tries to send it to the computer it's for
// Always returns true to stay connected to the remote computer
BOOL CG1Neighbour::OnPush(CG1Packet* pPacket)
{
	if ( pPacket->m_nLength < 26 )
	{
		// Push packets should be 26 bytes, ignore others
		theApp.Message( MSG_NOTICE, IDS_PROTOCOL_SIZE_PACKET, m_sAddress, _T("push") );
		++Statistics.Current.Gnutella1.Dropped;
		++m_nDropCount;
		return TRUE;	// Stay connected
	}

	// The first 16 bytes of the packet payload are the Gnutella client ID GUID, read them into pClientID
	Hashes::Guid oClientID;
	pPacket->Read( oClientID );

	// After that are the file index, IP address, and port number, read them
	DWORD nFileIndex	= pPacket->ReadLongLE();	// 4 bytes, the file index (do)
	DWORD nAddress		= pPacket->ReadLongLE();	// 4 bytes, the IP address of (do)
	WORD  nPort 		= pPacket->ReadShortLE();	// 2 bytes, the port number

	if ( Security.IsDenied( (IN_ADDR*)&nAddress ) )
	{
		++Statistics.Current.Gnutella1.Dropped;
		++m_nDropCount;
		return TRUE;
	}

	bool bGGEP = false;
	//bool bTLS = false;	// ToDo: Gnutella 1 TLS?

	// If longer than a normal push packet, and handshake said it supports GGEP blocks
	if ( pPacket->m_nLength > 26 && m_bGGEP )
	{
		// Read the next byte from the packet and make sure it's 0xC3, the magic code for a GGEP block
		CGGEPBlock pGGEP;
		if ( pGGEP.ReadFromPacket( pPacket ) )
		{
			bGGEP = true;
			//if ( pGGEP.Find( GGEP_HEADER_TLS_SUPPORT ) )
			//	bTLS = true;
		}
	}

	if ( ! nPort || ( pPacket->m_nHops && (
		Network.IsFirewalledAddress( (IN_ADDR*)&nAddress ) ||
		Network.IsReserved( (IN_ADDR*)&nAddress ) ) ) )
	{
		theApp.Message( MSG_NOTICE, IDS_PROTOCOL_ZERO_PUSH, m_sAddress );
		++Statistics.Current.Gnutella1.Dropped;
		++m_nDropCount;
		return TRUE;
	}

	// If the push packet contains our own client ID, this is someone asking us to push open a connection
	if ( validAndEqual( oClientID, Hashes::Guid( MyProfile.oGUID ) ) )
	{
		// Setup push connection and return that packet was handled
		Handshakes.PushTo( (IN_ADDR*)&nAddress, nPort, nFileIndex );
		return TRUE;
	}

	// Otherwise, the push packet is for another computer that we can hopefully can send it to, try to find it
	CNeighbour* pOrigin = NULL;
	Network.NodeRoute->Lookup( oClientID, (CNeighbour**)&pOrigin );

	// If we are connected to a computer with that client ID, and the packet's TTL and hop counts are OK
	if ( pOrigin && pPacket->Hop() )	// Calling Hop moves 1 from TTL to hops
	{
		// If the remote computer the push packet is for is running Gnutella
		if ( pOrigin->m_nProtocol == PROTOCOL_G1 )
		{
			// If this packet has a GGEP block, but the computer its for doesn't support them, cut it off
			if ( bGGEP && ! pOrigin->m_bGGEP )
				pPacket->Shorten( 26 );

			// Send the push packet to the computer that needs to do it
			pOrigin->Send( pPacket, FALSE, TRUE );
		}
		// Otherwise it's running Gnutella2 software like PeerProject
		else if ( pOrigin->m_nProtocol == PROTOCOL_G2 )
		{
			// Create a new Gnutella2 push packet with the same information as this one, and send it
			CG2Packet* pWrap = CG2Packet::New( G2_PACKET_PUSH, TRUE );
			pWrap->WritePacket( G2_PACKET_TO, 16 );
			pWrap->Write( oClientID );
			pWrap->WriteByte( 0 );
			pWrap->WriteLongLE( nAddress );
			pWrap->WriteShortLE( nPort );
			pOrigin->Send( pWrap, TRUE, TRUE );
		}

		// Record that we routed one more packet
		++Statistics.Current.Gnutella1.Routed;
	}

	// Return that packet was handled
	return TRUE;
}

// Called by CNetwork::RoutePacket (do)
// Takes the client ID GUID from a Gnutella push packet, and that Gnutella push packet
// Makes a new Gnutella push packet with the same information, and sends it to the remote computer
void CG1Neighbour::SendG2Push(const Hashes::Guid& oGUID, CPacket* pPacket)
{
	// Make sure there are at least 6 more bytes we haven't read from the packet yet
	if ( pPacket->GetRemaining() < 6 ) return;

	// Read 6 bytes, the IP address and port number
	DWORD nAddress	= pPacket->ReadLongLE();
	WORD nPort		= pPacket->ReadShortLE();

	// Make a new Gnutella push packet, fill it with the same information, and send it to the remote computer
	pPacket = CG1Packet::New( G1_PACKET_PUSH, Settings.Gnutella1.MaximumTTL - 1 );
	pPacket->Write( oGUID );			// 16 bytes, the client ID GUID
	pPacket->WriteLongLE( 0 );			// 4 bytes, all 0 (do)
	pPacket->WriteLongLE( nAddress );	// 4 bytes, the IP address
	pPacket->WriteShortLE( nPort ); 	// 2 bytes, the port number
	Send( pPacket, TRUE, TRUE );
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour QUERY packet handlers

// Takes a Gnutella query packet to see if we have a file like that,
// and forwards the packet to the computers we are connected to.
// Returns false if the remote computer sent a malformed packet and we should disconnect from it, true otherwise
BOOL CG1Neighbour::OnQuery(CG1Packet* pPacket)
{
	Statistics.Current.Gnutella1.Queries++;		// All Incoming

	// If packet payload is too long
	if ( pPacket->m_nLength > Settings.Gnutella1.MaximumQuery )
	{
		// Record it and drop it
		theApp.Message( MSG_ERROR, IDS_PROTOCOL_TOO_LARGE, (LPCTSTR)m_sAddress );
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return FALSE;	// Disconnect from the remote computer
	}

	// If this connection is to a leaf below us, and this packet has traveled across the Internet before
	if ( m_nNodeType == ntLeaf && pPacket->m_nHops > 0 )
	{
		// Drop it (do)
		theApp.Message( MSG_ERROR, IDS_PROTOCOL_LEAF_FORWARD, (LPCTSTR)m_sAddress );
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return FALSE;	// Disconnect from the remote computer
	}

	// Give the query packet to the network's CRouteCache object, if it reports error (do)
	if ( ! Network.QueryRoute->Add( pPacket->m_oGUID, this ) )
	{
		// Drop it
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;	// Stay connected to the remote computer
	}

	// Have the CQuerySearch class turn the query search packet into a CQuerySearch object (do)
	CQuerySearchPtr pSearch = CQuerySearch::FromPacket( pPacket );
	if ( ! pSearch || pSearch->m_bDropMe )
	{
		// The CQuerySearch class rejected the search, drop the packet
		if ( ! pSearch )
		{
			theApp.Message( MSG_INFO, IDS_PROTOCOL_BAD_QUERY, _T("G1"), (LPCTSTR)m_sAddress );
			DEBUG_ONLY( pPacket->Debug( _T("G1 Malformed Query.") ) );
		}
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;	// Stay connected to the remote computer
	}

	// If this connection isn't up to a hub above us, check the TTL and if that's ok, move 1 from TTL to hops
	if ( m_nNodeType != ntHub && pPacket->Hop() )
		Neighbours.RouteQuery( pSearch, pPacket, this, TRUE );	// Have the neighbours object route the query packet (do)

	// Give the CQuerySearch object to the Network object (do)
	Network.OnQuerySearch( new CLocalSearch( pSearch, this ) );

	Statistics.Current.Gnutella1.QueriesProcessed++;

	return TRUE;	// Stay connected to the remote computer
}

// Takes a CQuerySearch object, a Gnutella packet, and (do)
// Makes sure the search makes sense, and then sends the packet to the remote computer
// Returns true if we sent the packet, false if we discovered something wrong with the situation and didn't send it
BOOL CG1Neighbour::SendQuery(const CQuerySearch* pSearch, CPacket* pPacket, BOOL bLocal)
{
	// If the caller didn't give us a packet, or one that isn't for our protocol, leave now
	if ( pPacket == NULL || pPacket->m_nProtocol != PROTOCOL_G1 )
		return FALSE;

	// If the search object isn't for Gnutella
	if ( ! pSearch->m_bAndG1 )
		return FALSE;

	// If the packet's hops count is more than m_nHopsFlow, which is set to 0xFF by the constructor
	if ( static_cast< CG1Packet* >( pPacket )->m_nHops > m_nHopsFlow )
		return FALSE;

	return CNeighbour::SendQuery( pSearch, pPacket, bLocal );
}

//////////////////////////////////////////////////////////////////////
// CG1Neighbour QUERY HIT packet handler

// Takes a Gnutella query hit packet
// Hands it to OnCommonHit (do)
// Returns the result from OnCommonHit (do)
BOOL CG1Neighbour::OnHit(CG1Packet* pPacket)
{
	// If the packet is too short
	if ( pPacket->m_nLength <= 27 )		// 11 + Hashes::Guid::byteCount
	{
		// Drop it
		theApp.Message( MSG_ERROR, IDS_PROTOCOL_BAD_HIT, (LPCTSTR)m_sAddress );
		Statistics.Current.Gnutella1.Dropped++;
		m_nDropCount++;
		return TRUE;	// Stay connected to the remote computer
	}

	// Have OnCommonHit process the query hit packet, and return its result (do)
	return OnCommonHit( pPacket );
}
