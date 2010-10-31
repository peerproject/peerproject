//
// ManagedSearch.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "SearchManager.h"
#include "ManagedSearch.h"
#include "QuerySearch.h"
#include "Network.h"
#include "HostCache.h"
#include "Neighbours.h"
#include "Datagrams.h"
#include "G1Neighbour.h"
#include "G2Neighbour.h"
#include "G1Packet.h"
#include "G2Packet.h"
#include "EDPacket.h"
#include "EDNeighbour.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename


//////////////////////////////////////////////////////////////////////
// CManagedSearch construction

CManagedSearch::CManagedSearch(CQuerySearch* pSearch, int nPriority)
	: m_pSearch		( pSearch ? pSearch : new CQuerySearch() )
	, m_nPriority	( nPriority )
	, m_bAllowG2	( TRUE )
	, m_bAllowG1	( TRUE )
	, m_bAllowED2K	( TRUE )
	, m_bActive		( FALSE )
	, m_bReceive	( TRUE )
	, m_tStarted	( 0 )
	, m_nHits		( 0 )
	, m_nG1Hits		( 0 )
	, m_nG2Hits		( 0 )
	, m_nEDHits		( 0 )
	, m_nHubs		( 0 )
	, m_nLeaves		( 0 )
	, m_nQueryCount	( 0 )
	, m_tLastG2		( 0 )
	, m_tLastED2K	( 0 )
	, m_tMoreResults( 0 )
	, m_nEDServers	( 0 )
	, m_nEDClients	( 0 )
	, m_tExecute	( 0 )
{
	m_dwRef = 0;
}

CManagedSearch::~CManagedSearch()
{
	DEBUG_ONLY( CQuickLock( SearchManager.m_pSection ) );
	ASSERT( SearchManager.m_pList.Find( this ) == NULL );
}

//////////////////////////////////////////////////////////////////////
// CManagedSearch serialize

void CManagedSearch::Serialize(CArchive& ar)
{
	CQuickLock oLock( SearchManager.m_pSection );

	int nVersion = 3;

	if ( ar.IsStoring() )
	{
		ar << nVersion;

		m_pSearch->Serialize( ar );
		ar << m_nPriority;
		ar << m_bActive;
		ar << m_bReceive;
		ar << m_bAllowG2;
		ar << m_bAllowG1;
		ar << m_bAllowED2K;
	}
	else // Loading
	{
		ar >> nVersion;
		if ( nVersion < 2 ) AfxThrowUserException();

		m_pSearch->Serialize( ar );

		ar >> m_nPriority;
		ar >> m_bActive;
		ar >> m_bReceive;

		// Auto-start search ability turned off
		m_bActive = m_bReceive = FALSE;

		//if ( nVersion > 2 )
		//{
			ar >> m_bAllowG2;
			ar >> m_bAllowG1;
			ar >> m_bAllowED2K;
		//}
	}
}

//////////////////////////////////////////////////////////////////////
// CManagedSearch start and stop

void CManagedSearch::Start()
{
	if ( InterlockedCompareExchange( (LONG*)&m_bActive, TRUE, FALSE ) )
		return;

	CQuickLock oLock( SearchManager.m_pSection );

	if ( SearchManager.Add( this ) )
	{
		m_tStarted		= static_cast< DWORD >( time( NULL ) );
		m_tExecute		= 0;
		m_tLastED2K		= 0;
		m_tMoreResults	= 0;
		m_nQueryCount	= 0;
		m_pNodes.RemoveAll();
	}
}

void CManagedSearch::Stop()
{
	if ( InterlockedCompareExchange( (LONG*)&m_bActive, FALSE, TRUE ) )
		Datagrams.PurgeToken( this );

	CQuickLock oLock( SearchManager.m_pSection );

	SearchManager.Remove( this );
}

void CManagedSearch::CreateGUID()
{
	if ( m_pSearch )
		Network.CreateID( m_pSearch->m_oGUID );
}

//////////////////////////////////////////////////////////////////////
// CManagedSearch execute

BOOL CManagedSearch::Execute(int nPriorityClass)
{
	ASSUME_LOCK( SearchManager.m_pSection );

	if ( m_nPriority != nPriorityClass || ! m_bActive || ! m_pSearch )
		return FALSE;

	// Throttle this individual search (so it doesn't take up too many resources)
	DWORD nThrottle = Settings.Search.GeneralThrottle;
	if ( m_nPriority == spLowest )
		nThrottle += 30000;
	else if ( m_nPriority == spMedium )
		nThrottle += 800;

	const DWORD tTicks = GetTickCount();
	const DWORD tSecs = static_cast< DWORD >( time( NULL ) );

	if ( tTicks - m_tExecute < nThrottle )
		return FALSE;
	m_tExecute = tTicks;

	// Search local neighbours: hubs, servers and ultrapeers. (TCP)
	BOOL bSuccess = ExecuteNeighbours( tTicks, tSecs );

	// G2 global search. (UDP)
	if ( Settings.Gnutella2.EnableToday && m_bAllowG2 )
	{
		if ( tTicks >= m_tLastG2 + Settings.Gnutella2.QueryGlobalThrottle )
		{
			bSuccess |= ExecuteG2Mesh( tTicks, tSecs );
			m_tLastG2 = tTicks;
		}
	}

	// ED2K global search. (UDP)
	if ( Settings.eDonkey.EnableToday && Settings.eDonkey.ServerWalk && m_bAllowED2K &&
		 tTicks >= m_tLastED2K + Settings.eDonkey.QueryGlobalThrottle &&
		 Network.IsListening() && ( m_pSearch->m_oED2K || IsLastED2KSearch() ) )
	{
		bSuccess |= ExecuteDonkeyMesh( tTicks, tSecs );
		m_tLastED2K = tTicks;
	}

	if ( bSuccess ) m_nQueryCount++;

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CManagedSearch execute the search on G1 / G2 / ED2K neighbours

BOOL CManagedSearch::ExecuteNeighbours(const DWORD tTicks, const DWORD tSecs)
{
	ASSUME_LOCK( SearchManager.m_pSection );

	int nCount = 0;
	for ( POSITION pos = Neighbours.GetIterator() ; pos ; )
	{
		CNeighbour* pNeighbour = Neighbours.GetNext( pos );

		// Must be connected
		if ( pNeighbour->m_nState != nrsConnected ) continue;

		// Check network flags
		switch ( pNeighbour->m_nProtocol )
		{
		case PROTOCOL_G1:
			if ( ! m_bAllowG1 ) continue;
			break;
		case PROTOCOL_G2:
			if ( ! m_bAllowG2 ) continue;
			break;
		case PROTOCOL_ED2K:
			if ( ! m_bAllowED2K ) continue;
			break;
		default:
			continue;
		}

		// Must be stable for 15 seconds
		if ( tTicks - pNeighbour->m_tConnected < 15000 )
			continue;

		// Do not hammer neighbours for search results
		if ( pNeighbour->m_nProtocol == PROTOCOL_G1 )
		{
			if ( tSecs < pNeighbour->m_tLastQuery + Settings.Gnutella1.QueryThrottle )
				continue;
		}
		else if ( pNeighbour->m_nProtocol == PROTOCOL_G2 )
		{
			if ( tSecs < pNeighbour->m_tLastQuery + Settings.Gnutella2.QueryHostThrottle )
				continue;
		}

		// Lookup the host
		DWORD nLastQuery;
		DWORD nAddress = pNeighbour->m_pHost.sin_addr.S_un.S_addr;
		if ( m_pNodes.Lookup( nAddress, nLastQuery ) )
		{
			DWORD nFrequency = 0;
			if ( pNeighbour->m_nProtocol == PROTOCOL_G1 )
				nFrequency = Settings.Gnutella1.RequeryDelay * ( m_nPriority + 1 );
			else if ( pNeighbour->m_nProtocol == PROTOCOL_G2 )
				nFrequency = Settings.Gnutella2.RequeryDelay * ( m_nPriority + 1 );
			else if ( pNeighbour->m_nProtocol == PROTOCOL_ED2K )
				nFrequency = 86400; // 1 day

			if ( tSecs - nLastQuery < nFrequency )	// If we've queried this neighbour 'recently'
			{
				// Request more ed2k results (if appropriate)
				if ( ( pNeighbour->m_nProtocol == PROTOCOL_ED2K ) && pNeighbour->m_oMoreResultsGUID )	// It's an ed2k server and has more results
				{
					if ( IsEqualGUID( pNeighbour->m_oMoreResultsGUID ) &&	// This search is the one with results waiting
						( m_tMoreResults + 10000 < tTicks ) )				// and we've waited a while (to ensure the search is still active)
					{
						// Request more results
						pNeighbour->Send( CEDPacket::New(  ED2K_C2S_MORERESULTS ) );
						((CEDNeighbour*)pNeighbour)->m_pQueries.AddTail( pNeighbour->m_oMoreResultsGUID );
						// Reset "more results" indicator
						pNeighbour->m_oMoreResultsGUID.clear();
						// Set timer
						m_tMoreResults = tTicks;
						// Display message in system window
						theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH,
							_T("Asking ed2k neighbour for additional search results") );
					}
				}

				continue;	// Don't search this neighbour again.
			}
		}

		// Create the appropriate packet type
		CPacket* pPacket = NULL;
		if ( pNeighbour->m_nProtocol == PROTOCOL_G1 )
		{
			DWORD nTTL;
			BOOL bKnownHost = m_pG1Nodes.Lookup( nAddress, nTTL );
			if ( pNeighbour->m_bExtProbes )
			{
				// Neighbour supports X-Ext-Probes
				if ( bKnownHost )
				{
					if ( nTTL >= pNeighbour->GetMaxTTL() )
						continue;	// X-Max-TTL reached
					if ( m_nG1Hits >= Settings.Gnutella.MaxResults )
						continue;	// Maximum hits reached
					nTTL++;
				}
				else	// It's the first step
				{
					nTTL = 1;
				}
			}
			else
			{
				if ( bKnownHost )
					continue;	// We under pledge of X-Requeries
				else
					nTTL = pNeighbour->GetMaxTTL();	// Single query with max available TTL
			}

			m_pG1Nodes.SetAt( nAddress, nTTL );
			pPacket = m_pSearch->ToG1Packet( nTTL );
		}
		else if ( pNeighbour->m_nProtocol == PROTOCOL_G2 )
		{
			m_pSearch->m_bAndG1 = ( Settings.Gnutella1.EnableToday && m_bAllowG1 );
			pPacket = m_pSearch->ToG2Packet( !Network.IsFirewalled(CHECK_UDP) ? &Network.m_pHost : NULL, 0 );
		}
		else if ( pNeighbour->m_nProtocol == PROTOCOL_ED2K )
		{
			pPacket = m_pSearch->ToEDPacket( FALSE, ((CEDNeighbour*)pNeighbour)->m_nTCPFlags );
		}

		// Try to send the search
		if ( pPacket != NULL )
		{
			// Set the last query time for this host for this search
			m_pNodes.SetAt( nAddress, tSecs );

			if ( pNeighbour->SendQuery( m_pSearch, pPacket, TRUE ) )
			{
				// Reset the last "search more" sent to this neighbour (if applicable)
				pNeighbour->m_oMoreResultsGUID.clear();
				m_tMoreResults = 0;

				//Display message in system window
				theApp.Message( MSG_INFO, IDS_NETWORK_SEARCH_SENT,
					m_pSearch->m_sSearch.GetLength() ? (LPCTSTR)m_pSearch->m_sSearch : _T("URN"),
					(LPCTSTR)CString( inet_ntoa( pNeighbour->m_pHost.sin_addr ) ) );

				if ( pNeighbour->m_nProtocol == PROTOCOL_ED2K )
				{
					// Add to ED2K search counts
					m_nEDServers++;
					m_nEDClients += ((CEDNeighbour*)pNeighbour)->m_nUserCount;

					// Set the "last ED2K search" value if we sent a text search (to find the search later).
					if ( ! m_pSearch->m_oED2K )
						SearchManager.m_oLastED2KSearch = m_pSearch->m_oGUID;
				}
			}
			pPacket->Release();
		}

		nCount++;
	}

	return ( nCount > 0 );
}

//////////////////////////////////////////////////////////////////////
// CManagedSearch execute the search on the G2 mesh

BOOL CManagedSearch::ExecuteG2Mesh(const DWORD /*tTicks*/, const DWORD tSecs)
{
	ASSUME_LOCK( SearchManager.m_pSection );

	// Look at all known Gnutella2 hubs, newest first

	CQuickLock oLock( HostCache.Gnutella2.m_pSection );

	for ( CHostCacheIterator i = HostCache.Gnutella2.Begin() ;
		i != HostCache.Gnutella2.End();	++i )
	{
		CHostCacheHostPtr pHost = (*i);

		// Must be Gnutella2
		ASSERT( pHost->m_nProtocol == PROTOCOL_G2 );

		// If this host is a neighbour, don't UDP to it
		if ( Neighbours.Get( pHost->m_pAddress ) )
			continue;

		// If this host can't be queried now, don't query it
		if ( ! pHost->CanQuery( tSecs ) )
			continue;

		// Check if we have an appropriate query key for this host,
		// and if so, record the receiver address
		SOCKADDR_IN* pReceiver = NULL;

		if ( pHost->m_nKeyValue == 0 )
		{
			// We already know we don't have a key... pretty simple
		}
		else if ( ! Network.IsFirewalled(CHECK_UDP) )
		{
			// If we are "stable", we have to TX/RX our own UDP traffic,
			// so we must have a query key for the local addess
			if ( pHost->m_nKeyHost == Network.m_pHost.sin_addr.S_un.S_addr )
				pReceiver = &Network.m_pHost;
			else
				pHost->m_nKeyValue = 0;
		}
		else
		{
			// Make sure we have a query key via one of our neighbours,
			// and ensure we have queried this neighbour
			if ( CNeighbour* pNeighbour = Neighbours.Get( *(IN_ADDR*)&pHost->m_nKeyHost ) )
			{
				DWORD nTemp;
				if ( m_pNodes.Lookup( pHost->m_nKeyHost, nTemp ) )
					pReceiver = &pNeighbour->m_pHost;
				else
					continue;
			}
			else
			{
				pHost->m_nKeyValue = 0;
			}
		}

		// Now, if we still have a query key, send the query
		if ( pHost->m_nKeyValue != 0 )
		{
			DWORD tLastQuery;
			ASSERT( pReceiver != NULL );

			// Lookup the host
			if ( m_pNodes.Lookup( pHost->m_pAddress.s_addr, tLastQuery ) )
			{
				// Check per-hub re-query time
				DWORD nFrequency;

				if ( m_nPriority >=  spLowest )
				{
					// Low priority "auto find" sources
					if ( m_pSearch->m_oSHA1 )		// Has SHA1- probably exists on G2
						nFrequency = 16 * 60 * 60;
					else							// Reduce frequency if no SHA1.
						nFrequency = 32 * 60 * 60;
				}
				else
					nFrequency = Settings.Gnutella2.RequeryDelay * ( m_nPriority + 1 );

				if ( tSecs - tLastQuery < nFrequency )
					continue;
			}

			// Set the last query time for this host for this search
			m_pNodes.SetAt( pHost->m_pAddress.s_addr, tSecs );

			// Record the query time on the host, for all searches
			pHost->m_tQuery = tSecs;
			if ( pHost->m_tAck == 0 )
				pHost->m_tAck = tSecs;

			// Try to create a packet
			m_pSearch->m_bAndG1 = ( Settings.Gnutella1.EnableToday && m_bAllowG1 );

			if ( CPacket* pPacket = m_pSearch->ToG2Packet( pReceiver, pHost->m_nKeyValue ) )
			{
				if ( Datagrams.Send( &pHost->m_pAddress, pHost->m_nPort, pPacket, TRUE, this, TRUE ) )
				{
					theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH,
						_T("Querying %s"), (LPCTSTR)CString( inet_ntoa( pHost->m_pAddress ) ) );
					return TRUE;
				}
			}
		}
		else if ( tSecs - pHost->m_tKeyTime >= max( Settings.Gnutella2.QueryHostThrottle * 5ul, 5ul * 60ul ) )
		{
			// Timing wise, we can request a query key now --
			// but first we must figure out who should be the receiver

			CNeighbour* pCacheHub = NULL;
			pReceiver = NULL;

			if ( ! Network.IsFirewalled( CHECK_UDP ) )
			{
				// If we are stable, we must be the receiver
				pReceiver = &Network.m_pHost;
			}
			else
			{
				// Otherwise, we need to find a neighbour G2 hub who has acked this query already
				for ( POSITION pos = Neighbours.GetIterator() ; pos ; pCacheHub = NULL )
				{
					pCacheHub = Neighbours.GetNext( pos );
					DWORD nTemp;

					if ( m_pNodes.Lookup( pCacheHub->m_pHost.sin_addr.s_addr, nTemp ) )
					{
						if ( pCacheHub->m_nProtocol == PROTOCOL_G2 &&
							 pCacheHub->m_nNodeType == ntHub )
						{
							pReceiver = &pCacheHub->m_pHost;
							if ( ! ((CG2Neighbour*)pCacheHub)->m_bCachedKeys )
								pCacheHub = NULL;
							break;
						}
					}
				}
			}

			// If we found a receiver, we can ask for the query key
			if ( pCacheHub != NULL )
			{
				// The receiver is a cache-capable hub, so we ask it to return
				// a cached key, or fetch a fresh one
				if ( CG2Packet* pPacket = CG2Packet::New( G2_PACKET_QUERY_KEY_REQ, TRUE ) )
				{
					pPacket->WritePacket( G2_PACKET_QUERY_ADDRESS, 6 );
					pPacket->WriteLongLE( pHost->m_pAddress.S_un.S_addr );
					pPacket->WriteShortBE( pHost->m_nPort );

					if ( pCacheHub->Send( pPacket ) )
					{
						if ( pHost->m_tAck == 0 )
							pHost->m_tAck = tSecs;
						pHost->m_tKeyTime = tSecs;
						pHost->m_nKeyValue = 0;

						theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH,
							_T("Requesting query key from %s through %s"),
							(LPCTSTR)CString( inet_ntoa( pHost->m_pAddress ) ),
							(LPCTSTR)CString( inet_ntoa( pReceiver->sin_addr ) ) );
						return TRUE;
					}
				}
			}
			else if ( pReceiver != NULL )
			{
				// We need to transmit directly to the remote query host
				if ( CG2Packet* pPacket = CG2Packet::New( G2_PACKET_QUERY_KEY_REQ, TRUE ) )
				{
					if ( pReceiver != &Network.m_pHost )
					{
						// We are not the receiver, so include receiver address
						pPacket->WritePacket( G2_PACKET_REQUEST_ADDRESS, 6 );
						pPacket->WriteLongLE( pReceiver->sin_addr.S_un.S_addr );
						pPacket->WriteShortBE( ntohs( pReceiver->sin_port ) );
					}

					if ( Datagrams.Send( &pHost->m_pAddress, pHost->m_nPort, pPacket, TRUE, NULL, FALSE ) )
					{
						if ( pHost->m_tAck == 0 )
							pHost->m_tAck = tSecs;
						pHost->m_tKeyTime = tSecs;
						pHost->m_nKeyValue = 0;

						if ( pReceiver == &Network.m_pHost )
						{
							theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH,
								_T("Requesting query key from %s"),
								(LPCTSTR)CString( inet_ntoa( pHost->m_pAddress ) ) );
						}
						else
						{
							theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH,
								_T("Requesting query key from %s for %s"),
								(LPCTSTR)CString( inet_ntoa( pHost->m_pAddress ) ),
								(LPCTSTR)CString( inet_ntoa( pReceiver->sin_addr ) ) );
						}
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CManagedSearch execute the search on eDonkey2000 servers

BOOL CManagedSearch::ExecuteDonkeyMesh(const DWORD /*tTicks*/, const DWORD tSecs)
{
	ASSUME_LOCK( SearchManager.m_pSection );

	CQuickLock oLock( HostCache.eDonkey.m_pSection );

	for ( CHostCacheIterator i = HostCache.eDonkey.Begin() ; i != HostCache.eDonkey.End();	++i )
	{
		CHostCacheHostPtr pHost = (*i);

		ASSERT( pHost->m_nProtocol == PROTOCOL_ED2K );

		// If this host is a neighbour, don't UDP to it
		if ( Neighbours.Get( pHost->m_pAddress ) )
			continue;

		// Make sure this host can be queried (now)
		if ( ! pHost->CanQuery( tSecs ) )
			continue;

		// Never requery eDonkey2000 servers
		DWORD tLastQuery;
		if ( m_pNodes.Lookup( pHost->m_pAddress.s_addr, tLastQuery ) )
			continue;

		// Set the last query time for this host for this search
		m_pNodes.SetAt( pHost->m_pAddress.s_addr, tSecs );

		// Record the query time on the host, for all searches
		pHost->m_tQuery = tSecs;

		// Create a packet in the appropriate format
		if ( CPacket* pPacket = m_pSearch->ToEDPacket( TRUE, pHost->m_nUDPFlags ) )
		{
			// Send the datagram if possible
			if ( Datagrams.Send( &pHost->m_pAddress, pHost->m_nPort + 4, pPacket, TRUE ) )
			{
				// Add to ED2K search counts
				m_nEDServers ++;
				m_nEDClients += pHost->m_nUserCount;

				theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH,
					_T("Sending UDP query to %s"),
					(LPCTSTR)CString( inet_ntoa( pHost->m_pAddress ) ) );

				return TRUE;
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CManagedSearch host acknowledgement

void CManagedSearch::OnHostAcknowledge(DWORD nAddress)
{
	ASSUME_LOCK( SearchManager.m_pSection );

	m_pNodes.SetAt( nAddress, static_cast< DWORD >( time( NULL ) ) );
}

//////////////////////////////////////////////////////////////////////
// CManagedSearch check if we were the most recent ed2k text search. (Not find more sources)

BOOL CManagedSearch::IsLastED2KSearch()
{
	return IsEqualGUID( SearchManager.m_oLastED2KSearch );
}
