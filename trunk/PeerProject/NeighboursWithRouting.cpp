//
// NeighboursWithRouting.cpp
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

// Adds methods that send packets and Gnutella queries to all the computers we're connected to
// http://shareazasecurity.be/wiki/index.php?title=Developers.Code.CNeighboursWithRouting

// Copy in the contents of these files here before compiling
#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "Statistics.h"
#include "Network.h"
#include "Datagrams.h"
#include "Neighbour.h"
#include "NeighboursWithRouting.h"
#include "QuerySearch.h"
#include "G1Packet.h"
#include "G2Packet.h"

// If we are compiling in debug mode, replace the text "THIS_FILE" in the code with the name of this file
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CNeighboursWithRouting construction

// Nothing that CNeighboursWithRouting adds to CNeighbours needs to be setup
CNeighboursWithRouting::CNeighboursWithRouting()
{
}

// Nothing that CNeighboursWithRouting adds to CNeighbours needs to be put away
CNeighboursWithRouting::~CNeighboursWithRouting()
{
}

//////////////////////////////////////////////////////////////////////
// CNeighboursWithRouting packet broadcasting

// Takes a packet, and neighbour to ignore (do)
// Sends the packet to all the neighbours
// Returns the number of neighbours that got the packet
int CNeighboursWithRouting::Broadcast(CPacket* pPacket, CNeighbour* pExcept, BOOL bGGEP)
{
	// Have this thread get exclusive access to the network object (do)
	CSingleLock pLock( &Network.m_pSection, TRUE ); // When this method returns, pLock will go out of scope and release access

	// Count how many neighbours we will send this packet to
	int nCount = 0;
	bool bSend = true;

	if ( ! Settings.Gnutella1.EnableGGEP && bGGEP ) 
		return 0;

	// Loop through each neighbour in the list
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		// Get the neighbour under pos, and move pos to the next one in the list
		CNeighbour* pNeighbour = GetNext( pos );

		if ( Settings.Gnutella1.EnableGGEP )
		{
			// Don't send GGEP packets if neighbour doesn't support them
			bSend = ( bGGEP && pNeighbour->m_bGGEP );;
		}

		// If this isn't the neighbour the caller told us to avoid, and we've finished the handshake with it
		if ( pNeighbour != pExcept && pNeighbour->m_nState == nrsConnected && bSend )
		{
			// Send it the packet, and count one more packet was sent
			if ( pNeighbour->Send( pPacket, FALSE, TRUE ) ) nCount++;
		}
	}

	// Returns the count of how many neighbours got the packet
	return nCount;
}

//////////////////////////////////////////////////////////////////////
// CNeighboursWithRouting query dispatch

// Takes a CQuerySearch object (do), the packet that goes along with it, the neighbour it's from, and true to forward it to hubs (do)
// Forwards the query to connected computers, converting it into a Gnutella or Gnutella2 query if necessary
// Returns the number of computers we sent the packet to
int CNeighboursWithRouting::RouteQuery(CQuerySearch* pSearch, CPacket* pPacket, CNeighbour* pFrom, BOOL bToHubs)
{
	// Local variables
	BOOL bHubLoop = FALSE; // We'll set this to true if this is a Gnutella Q2 packet and we found at least one Gnutella2 computer
	int nCount = 0;        // We'll count how many neighbours we send the packet to
	POSITION pos;          // Position used looping down the list of neighbours

	// If the given packet is a Gnutella packet, point pG1 at it, otherwise make pG1 NULL, and do the same for pG2
	CG1Packet* pG1 = ( pPacket->m_nProtocol == PROTOCOL_G1 ) ? (CG1Packet*)pPacket : NULL;
	CG2Packet* pG2 = ( pPacket->m_nProtocol == PROTOCOL_G2 ) ? (CG2Packet*)pPacket : NULL;

	// These pointers will point at the packet if it is a special kind of Gnutella2 packet
	CG2Packet* pG2Q1 = NULL;
	CG2Packet* pG2Q2 = NULL;

	// Make sure that the packet is either for Gnutella or Gnutella2, and one of the pointers points to it
	ASSERT( pG1 || pG2 );

	// It's a Gnutella2 packet
	if ( pG2 )
	{
		// If it's a Q2 packet, point pG2Q2 at it, otherwise point pG2Q1 at it
		if ( pG2->IsType( G2_PACKET_QUERY ) ) pG2Q2 = pG2; else pG2Q1 = pG2;
	}

	// Loop for each connected neighbour
	for ( pos = GetIterator() ; pos ; )
	{
		// Get the neighbour at pos, and move pos forward to the next one
		CNeighbour* pNeighbour = (CNeighbour*)GetNext( pos );

		// If this is the neighbour we got the packet from, or if we're still doing the handshake with this neighbour
		if ( pNeighbour == pFrom )                 continue; // Skip the rest of the code in this loop and go back to the top
		if ( pNeighbour->m_nState < nrsConnected ) continue;

		// This neighbour is running Gnutella software
		if ( pNeighbour->m_nProtocol == PROTOCOL_G1 )
		{
			// The caller wants to include hubs, or it doesn't but our connection to this one is down to a leaf anyway
			if ( bToHubs || pNeighbour->m_nNodeType == ntLeaf )
			{
				// This isn't a Gnutella packet
				if ( pG1 == NULL )
				{
					// This is a Gnutella2 packet, but not a Q2 one
					if ( pG2Q1 != NULL )
					{
						// Determine if we can turn it into a Gnutella packet, and do it if possible (do)
						if ( ! pG2Q1->SeekToWrapped() ) break;
						pG1 = CG1Packet::New( (GNUTELLAPACKET*)( pG2Q1->m_pBuffer + pG2Q1->m_nPosition ) );

					} // This is a Gnutella2 Q2 packet
					else
					{
						// Turn it into a Gnutella packet (do)
						pG1 = pSearch->ToG1Packet();
					}
				}

				// Send the packet to this connected Gnutella computer
				if ( pNeighbour->SendQuery( pSearch, pG1, pG2Q2 != NULL ) ) nCount++;
			}

		} // This neighbour is running Gnutella2 software
		else if ( pNeighbour->m_nProtocol == PROTOCOL_G2 )
		{
			// Our connection to this computer is down to a leaf
			if ( pNeighbour->m_nNodeType == ntLeaf )
			{
				// This isn't a Gnutella2 packet
				if ( pG2 == NULL )
				{
					// Turn it into one
					//pG2 = pG2Q1 = CG2Packet::New( G2_PACKET_QUERY_WRAP, pG1, Settings.Gnutella1.TranslateTTL );
					theApp.Message( MSG_ERROR, _T("CNeighboursWithRouting::RouteQuery not relaying wrapped packet to leaf") );
				}

				// Send the packet to this remote computer
				if ( pNeighbour->SendQuery( pSearch, pG2, FALSE ) ) nCount++;

			} // This remote computer is a hub, and the caller said we can send packets to hubs
			else if ( bToHubs )
			{
				// This isn't a Gnutella2 Q2 packet
				if ( pG2Q2 == NULL )
				{
					// In fact, it's not a Gnutella2 packet at all
					if ( pG2 == NULL )
					{
						// Turn it into one
						//pG2 = pG2Q1 = CG2Packet::New( G2_PACKET_QUERY_WRAP, pG1, Settings.Gnutella1.TranslateTTL );
						theApp.Message( MSG_ERROR, _T("CNeighboursWithRouting::RouteQuery not relaying wrapped packet to hub") );
					}

					// Send the packet to this remote computer
					if ( pNeighbour->SendQuery( pSearch, pG2, FALSE ) ) nCount++;

				} // This is a Gnutella2 Q2 packet
				else
				{
					// Set the flag to enter the next if statement
					bHubLoop = TRUE;
				}
			}
		}
	}

	// We found at least one Gnutella2 computer, and this is a Gnutella2 Q2 packet
	if ( bHubLoop )
	{
		// (do)
		if ( pSearch->m_bUDP == FALSE && !Network.IsFirewalled(CHECK_UDP) )
		{
			pG2 = pG2->Clone();
			if ( pG2Q2 != pPacket ) pG2Q2->Release();
			pG2Q2 = pG2;

			BYTE* pPtr = pG2->WriteGetPointer( 5 + 6, 0 );

			if ( pPtr == NULL )
			{
				theApp.Message( MSG_ERROR, _T("Memory allocation error in CNeighboursWithRouting::RouteQuery()") );
				return 0;
			}

			*pPtr++ = 0x50;
			*pPtr++ = 6;
			*pPtr++ = 'U';
			*pPtr++ = 'D';
			*pPtr++ = 'P';
			*pPtr++ = Network.m_pHost.sin_addr.S_un.S_un_b.s_b1;
			*pPtr++ = Network.m_pHost.sin_addr.S_un.S_un_b.s_b2;
			*pPtr++ = Network.m_pHost.sin_addr.S_un.S_un_b.s_b3;
			*pPtr++ = Network.m_pHost.sin_addr.S_un.S_un_b.s_b4;
			if ( pPacket->m_bBigEndian )
			{
				*pPtr++ = (BYTE)( Network.m_pHost.sin_port & 0xFF );
				*pPtr++ = (BYTE)( ( Network.m_pHost.sin_port >> 8 ) & 0xFF );
			}
			else
			{
				*pPtr++ = (BYTE)( ( Network.m_pHost.sin_port >> 8 ) & 0xFF );
				*pPtr++ = (BYTE)( Network.m_pHost.sin_port & 0xFF );
			}
		}

		// Loop through all the computers we're connected to
		for ( pos = GetIterator() ; pos ; )
		{
			// Get the neighbouring computer at this position, and move position to the next one
			CNeighbour* pNeighbour = (CNeighbour*)GetNext( pos );

			// If this computer should get the packet
			if ( pNeighbour != pFrom                    && // This isn't the computer we got the packet from, and
				 pNeighbour->m_nState >= nrsConnected   && // We're done with the handshake with this computer, and
				 pNeighbour->m_nProtocol == PROTOCOL_G2 && // This computer is running Gnutella2 software, and
				 pNeighbour->m_nNodeType != ntLeaf )       // This computer isn't a leaf below us
			{
				// Send the packet to this computer
				if ( pNeighbour->SendQuery( pSearch, pG2, FALSE ) ) nCount++;
			}
		}
	}

	// If we made a Gnutella packet from a Gnutella2 one, or the other way around, release the packet we made
	if ( pG1 && pG1 != pPacket ) pG1->Release();
	if ( pG2 && pG2 != pPacket ) pG2->Release();

	// If we sent the packet to at least one computer
	if ( nCount )
	{
		// Record it in statistics
		if      ( pPacket->m_nProtocol == PROTOCOL_G1 ) Statistics.Current.Gnutella1.Routed++;
		else if ( pPacket->m_nProtocol == PROTOCOL_G2 ) Statistics.Current.Gnutella2.Routed++;
	}

	// Return the number of packets we sent
	return nCount;
}
