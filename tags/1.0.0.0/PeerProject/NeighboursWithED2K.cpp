//
// NeighboursWithED2K.cpp
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

// Add methods helpful for eDonkey2000 that look at the list of neighbours
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CNeighboursWithED2K
// http://peerproject.org/shareazawiki/Developers.Code.CNeighboursWithED2K.html

#include "StdAfx.h"
#include "PeerProject.h"
#include "NeighboursWithED2K.h"
#include "EDNeighbour.h"
#include "EDPacket.h"
#include "Datagrams.h"
#include "Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

//////////////////////////////////////////////////////////////////////
// CNeighboursWithED2K construction

// CNeighboursWithED2K adds two arrays that need to be
// filled with 0s when the program creates its CNeighbours object
CNeighboursWithED2K::CNeighboursWithED2K()
	: m_tEDSources()
	, m_oEDSources()
{
}

// CNeighboursWithED2K doesn't add anything to the
// CNeighbours inheritance column that needs to be cleaned up
CNeighboursWithED2K::~CNeighboursWithED2K()
{
}

//////////////////////////////////////////////////////////////////////
// CNeighboursWithED2K server lookup

// Looks in the neighbours list for an eDonkey2000 computer that we've finished the handshake with and that has a client ID
// Returns a pointer to the first one found, or null if none found in the list
CEDNeighbour* CNeighboursWithED2K::GetDonkeyServer() const
{
	ASSUME_SINGLE_LOCK( Network.m_pSection );

	// Loop through the list of neighbours
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		// Get the neighbour under the current position, and move to the next position
		CEDNeighbour* pNeighbour = (CEDNeighbour*)GetNext( pos );

		// This neighbour really is running eDonkey2000
		if ( pNeighbour->m_nProtocol == PROTOCOL_ED2K )
		{
			// And, we've finished the handshake with it, and it has a client ID
			if ( pNeighbour->m_nState == nrsConnected && pNeighbour->m_nClientID != 0 )
				return pNeighbour;	// Return a pointer to it
		}
	}

	// We couldn't find a connected eDonkey2000 computer that we've finished the handshake with and that has a client ID
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CNeighboursWithED2K server lookup

// Calls Close() on all the eDonkey2000 computers in the list of neighbours we're connected to
void CNeighboursWithED2K::CloseDonkeys()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	// Loop through the list of neighbours
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		// Get the neighbour under the current position, and move to the next position
		CEDNeighbour* pNeighbour = (CEDNeighbour*)GetNext( pos );

		// If this neighbour really is running eDonkey2000, close our connection to it
		if ( pNeighbour->m_nProtocol == PROTOCOL_ED2K )
			pNeighbour->Close();
	}
}

//////////////////////////////////////////////////////////////////////
// CNeighboursWithED2K advertise a new download

// Takes a pointer to a CDownload object (do)
// Tells all the eDonkey2000 computers we're connected to about it
void CNeighboursWithED2K::SendDonkeyDownload(CDownload* pDownload)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	// Loop through the list of neighbours
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		// Get the neighbour under the current position, and move to the next position
		CEDNeighbour* pNeighbour = (CEDNeighbour*)GetNext( pos );

		// If this neighbour is running eDonkey2000 software, Tell it about this download
		if ( pNeighbour->m_nProtocol == PROTOCOL_ED2K )
			pNeighbour->SendSharedDownload( pDownload );
	}
}

//////////////////////////////////////////////////////////////////////
// CNeighboursWithED2K server push

// Takes a client ID (do), the IP address of an eDonkey2000 computer we're connected to, nServerPort unused (do)
// Finds the computer we're connected to with that IP address, and sends it a call back request with the client ID
// Returns true if we sent the packet, false if we couldn't find the computer
BOOL CNeighboursWithED2K::PushDonkey(DWORD nClientID, const IN_ADDR& pServerAddress, WORD)	// Was named nServerPort (do)
{
	CSingleLock oNetworkLock( &Network.m_pSection );
	if ( ! oNetworkLock.Lock( 300 ) )
		return FALSE;

	// If we don't have a socket listening for incoming connections, leave now
	if ( ! Network.IsListening() )
		return FALSE;

	// Get the neighbour with the given IP address, and look at it as an eDonkey2000 computer
	CEDNeighbour* pNeighbour = (CEDNeighbour*)Get( pServerAddress );

	// If we found it, and it really is running eDonkey2000
	if ( pNeighbour != NULL && pNeighbour->m_nProtocol == PROTOCOL_ED2K && ! CEDPacket::IsLowID( pNeighbour->m_nClientID ) )
	{
		// Make a new eDonkey2000 call back request packet, write in the client ID, and send it to the eDonkey2000 computer
		CEDPacket* pPacket = CEDPacket::New( ED2K_C2S_CALLBACKREQUEST );
		pPacket->WriteLongLE( nClientID );
		pNeighbour->Send( pPacket );

		// Report we found the computer and sent the packet
		return TRUE;
	}

	// lugdunum (ed2k server) requests no more of this
	//CEDPacket* pPacket = CEDPacket::New( ED2K_C2SG_CALLBACKREQUEST );
	//pPacket->WriteLongLE( Network.m_pHost.sin_addr.S_un.S_addr );
	//pPacket->WriteShortLE( htons( Network.m_pHost.sin_port ) );
	//pPacket->WriteLongLE( nClientID );
	//Datagrams.Send( pServerAddress, nServerPort + 4, pPacket );
	//return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CNeighboursWithED2K quick source lookup

// Takes the MD4 hash, IP address, and port number from a query hit (do), nServerPort unused (do)
// Updates the hash in the hash and time arrays, and sends an eDonkey2000 get sources packet to the given IP address
// Returns true if we sent a packet, false if we didn't because of the arrays (do)
BOOL CNeighboursWithED2K::FindDonkeySources(const Hashes::Ed2kHash& oED2K, IN_ADDR* pServerAddress, WORD nServerPort)
{
	// If we don't have a socket listening for incoming connections, leave now
	if ( ! Network.IsListening() ) return FALSE;

	// Start out nHash as the lowest byte 0xff of the IP address
	int nHash = (int)pServerAddress->S_un.S_un_b.s_b4 & 255;

	// Make sure nHash is between 0 and 255
	if ( nHash < 0 )
		nHash = 0;
	else if ( nHash > 255 )
		nHash = 255;

	// Number of milliseconds since the user turned the computer on
	const DWORD tNow = GetTickCount();

	// Lookup the MD4 hash at nHash in the m_pEDSources array of them, if it's equal to the given hash
	if ( validAndEqual( m_oEDSources[ nHash ], oED2K ) )
	{
		// If the hash in the array is less than an hour old, don't do anything and return false
		if ( tNow < m_tEDSources[ nHash ] + 3600000 )
			return FALSE;	// 1 hour
	}
	else	// The m_pEDSources array doesn't have pED2K at position nHash
	{
		// If that spot in the array was added in the last 15 seconds, don't do anything and return false
		if ( tNow < m_tEDSources[ nHash ] + 15000 )
			return FALSE;	// 15 seconds

		// That spot in the array is more than 15 seconds old, put the hash there
		m_oEDSources[ nHash ] = oED2K;
	}

	// Record that we visited this spot in the array now
	m_tEDSources[ nHash ] = tNow;

	// Make a eDonkey2000 get sources packet, write in the MD4 hash, and send it to the given address
	CEDPacket* pPacket = CEDPacket::New( ED2K_C2SG_GETSOURCES );
	pPacket->Write( oED2K );
	Datagrams.Send( pServerAddress, nServerPort + 4, pPacket );

	// Report that we sent a packet
	return TRUE;
}
