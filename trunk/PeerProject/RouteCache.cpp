//
// RouteCache.cpp
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
#include "RouteCache.h"
#include "Packet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

#undef HASH_SIZE
#undef HASH_MASK
const unsigned HASH_SIZE = 1024u;
const unsigned HASH_MASK = 0x3FF;

const DWORD MIN_BUFFER_SIZE = 1024u;
const DWORD MAX_BUFFER_SIZE = 40960u;
const unsigned BUFFER_BLOCK_SIZE = 1024u;


//////////////////////////////////////////////////////////////////////
// CRouteCache construction

CRouteCache::CRouteCache()
	: m_nSeconds( 60 * 20 )
	, m_pRecent( &m_pTable[0] )
	, m_pHistory ( &m_pTable[1] )
{
}

CRouteCache::~CRouteCache()
{
}

//////////////////////////////////////////////////////////////////////
// CRouteCache operations

void CRouteCache::SetDuration(DWORD nSeconds)
{
	m_nSeconds = nSeconds;
	Clear();
}

BOOL CRouteCache::Add(const Hashes::Guid& oGUID, const CNeighbour* pNeighbour)
{
	if ( CRouteCacheItem* pItem = Lookup( oGUID ) )
	{
		pItem->m_pNeighbour = pNeighbour;
		return FALSE;
	}

	if ( m_pRecent->IsFull() )
	{
		CRouteCacheTable* pTemp = m_pRecent;
		m_pRecent = m_pHistory;
		m_pHistory = pTemp;

		m_pRecent->Resize( m_pHistory->GetNextSize( m_nSeconds ) );
	}

	m_pRecent->Add( oGUID, pNeighbour, NULL );

	return TRUE;
}

BOOL CRouteCache::Add(const Hashes::Guid& oGUID, const SOCKADDR_IN* pEndpoint)
{
	if ( CRouteCacheItem* pItem = Lookup( oGUID ) )
	{
		pItem->m_pNeighbour	= NULL;
		pItem->m_pEndpoint	= *pEndpoint;
		return FALSE;
	}

	if ( m_pRecent->IsFull() )
	{
		CRouteCacheTable* pTemp = m_pRecent;
		m_pRecent = m_pHistory;
		m_pHistory = pTemp;

		m_pRecent->Resize( m_pHistory->GetNextSize( m_nSeconds ) );
	}

	m_pRecent->Add( oGUID, NULL, pEndpoint );

	return TRUE;
}

CRouteCacheItem* CRouteCache::Add(const Hashes::Guid& oGUID,		// GUID of node
								  const CNeighbour* pNeighbour,		// pointer to CNeighbour for Destination of GUID
								  const SOCKADDR_IN* pEndpoint,		// pointer to SOCKADDR_IN structure containing Destination
								  DWORD tAdded)						// Time the node added ( TickCount )
{
	SOCKADDR_IN cEndpoint;
	if ( pEndpoint != NULL ) cEndpoint = *pEndpoint;

	if ( m_pRecent->IsFull() )
	{
		CRouteCacheTable* pTemp = m_pRecent;
		m_pRecent = m_pHistory;
		m_pHistory = pTemp;

		m_pRecent->Resize( m_pHistory->GetNextSize( m_nSeconds ) );
	}

	return m_pRecent->Add( oGUID, pNeighbour, pEndpoint != NULL ? &cEndpoint : NULL, tAdded );
}

CRouteCacheItem* CRouteCache::Lookup(const Hashes::Guid& oGUID, CNeighbour** ppNeighbour, SOCKADDR_IN* pEndpoint)
{
	CRouteCacheItem* pItem = m_pRecent->Find( oGUID );

	if ( pItem == NULL )
	{
		pItem = m_pHistory->Find( oGUID );

		if ( pItem == NULL )
		{
			if ( ppNeighbour ) *ppNeighbour = NULL;
			if ( pEndpoint ) ZeroMemory( pEndpoint, sizeof(SOCKADDR_IN) );

			return NULL;
		}

		ASSERT( oGUID.isValid() );
		ASSERT( pItem->m_oGUID.isValid() );
		ASSERT( validAndEqual( oGUID, pItem->m_oGUID ) );

		// This needs to be done, because CRouteCache::Add() can cause m_pHistory cache table deleted.
		// thus need to copy data member of CRouteCacheItem if it is in m_pHistory, before it gets deleted.
		Hashes::Guid oTempGUID( pItem->m_oGUID);
		CNeighbour* pTempNeighbour = const_cast<CNeighbour*>(pItem->m_pNeighbour);
		SOCKADDR_IN pTempEndPoint = pItem->m_pEndpoint;
		DWORD tTempAddTime = pItem->m_tAdded;

		pItem = Add( oTempGUID, pTempNeighbour, &pTempEndPoint, tTempAddTime );
	}

	if ( ppNeighbour ) *ppNeighbour = (CNeighbour*)pItem->m_pNeighbour;
	if ( pEndpoint ) *pEndpoint = pItem->m_pEndpoint;

	return pItem;
}

void CRouteCache::Remove(CNeighbour* pNeighbour)
{
	m_pTable[0].Remove( pNeighbour );
	m_pTable[1].Remove( pNeighbour );
}

void CRouteCache::Clear()
{
	m_pTable[0].Clear();
	m_pTable[1].Clear();
}


//////////////////////////////////////////////////////////////////////
// CRouteCacheTable construction

CRouteCacheTable::CRouteCacheTable()
	: m_pBuffer( NULL )
	, m_nBuffer( 0 )
	, m_nUsed( 0 )
{
	Clear();
}

CRouteCacheTable::~CRouteCacheTable()
{
	if ( m_pBuffer && m_nBuffer ) delete [] m_pBuffer;
}

//////////////////////////////////////////////////////////////////////
// CRouteCacheTable operations

CRouteCacheItem* CRouteCacheTable::Find(const Hashes::Guid& oGUID)
{
	WORD nGUID = 0, *ppGUID = (WORD*)&oGUID[ 0 ];
	for ( int nIt = 8 ; nIt ; nIt-- ) nGUID = WORD( ( nGUID + *ppGUID++ ) & 0xffff );

	CRouteCacheItem* pItem = *( m_pHash + ( nGUID & HASH_MASK ) );

	for ( ; pItem ; pItem = pItem->m_pNext )
	{
		if ( validAndEqual( oGUID, pItem->m_oGUID ) ) return pItem;
	}

	return NULL;
}

CRouteCacheItem* CRouteCacheTable::Add(const Hashes::Guid& oGUID, const CNeighbour* pNeighbour, const SOCKADDR_IN* pEndpoint, DWORD nTime)
{
	if ( m_nUsed == m_nBuffer || ! m_pFree ) return NULL;

	if ( !oGUID.isValid() ) // There seem to be packets with oGUID == NULL (on heavy load) -> return NULL
		return NULL;

	WORD nGUID = 0;
	WORD *ppGUID = (WORD*)&oGUID[ 0 ];
	for ( int nIt = 8 ; nIt ; nIt-- ) nGUID = WORD( ( nGUID + *ppGUID++ ) & 0xffff );

	CRouteCacheItem** pHash = m_pHash + ( nGUID & HASH_MASK );

	CRouteCacheItem* pItem = m_pFree;
	m_pFree = m_pFree->m_pNext;

	pItem->m_pNext = *pHash;
	*pHash = pItem;

	pItem->m_oGUID			= oGUID;
	pItem->m_tAdded			= nTime ? nTime : GetTickCount();
	pItem->m_pNeighbour		= pNeighbour;
	if ( pEndpoint ) pItem->m_pEndpoint = *pEndpoint;

	if ( ! m_nUsed++ )
		m_tFirst = GetTickCount();
	else if ( m_nUsed == m_nBuffer )
		m_tLast = GetTickCount();

	return pItem;
}

void CRouteCacheTable::Remove(CNeighbour* pNeighbour)
{
	CRouteCacheItem** pHash = m_pHash;

	for ( int nHash = 0 ; nHash < HASH_SIZE ; nHash++, pHash++ )
	{
		CRouteCacheItem** pLast = pHash;

		for ( CRouteCacheItem* pItem = *pLast ; pItem ; )
		{
			CRouteCacheItem* pNext = pItem->m_pNext;

			if ( pItem->m_pNeighbour == pNeighbour )
			{
				*pLast = pNext;
				pItem->m_pNext = m_pFree;
				m_pFree = pItem;
				m_nUsed--;
			}
			else
			{
				pLast = &pItem->m_pNext;
			}

			pItem = pNext;
		}
	}
}

void CRouteCacheTable::Resize(DWORD nSize)
{
	DWORD nPrevSize = m_nBuffer;

	nSize = min( max( nSize, MIN_BUFFER_SIZE ), MAX_BUFFER_SIZE );
	nSize = ( ( nSize + BUFFER_BLOCK_SIZE - 1 ) / BUFFER_BLOCK_SIZE * BUFFER_BLOCK_SIZE );

	if ( nSize != m_nBuffer )
	{
		if ( m_pBuffer && m_nBuffer ) delete [] m_pBuffer;
		m_pBuffer = NULL;

		m_nBuffer = nSize;
		m_pBuffer = nSize ? ( new CRouteCacheItem[ m_nBuffer ] ) : NULL;
		ASSERT ( m_pBuffer != NULL );
		if ( m_pBuffer == NULL )
		{
			m_nBuffer = nPrevSize;
			m_pBuffer = new CRouteCacheItem[ m_nBuffer ];
			ASSERT ( m_pBuffer != NULL );
			if ( m_pBuffer == NULL )
			{
				m_nBuffer = ( ( MIN_BUFFER_SIZE + BUFFER_BLOCK_SIZE - 1 ) / BUFFER_BLOCK_SIZE * BUFFER_BLOCK_SIZE );
				m_pBuffer = new CRouteCacheItem[ m_nBuffer ];
				ASSERT ( m_pBuffer != NULL ); // Buffer memory Allocation error (serious problem should abort running and restart)
			}
		}
	}

	ZeroMemory( m_pHash, sizeof(CRouteCacheItem*) * HASH_SIZE );

	m_pFree		= m_pBuffer;
	m_nUsed		= 0;
	m_tFirst	= 0;
	m_tLast		= 0;

	CRouteCacheItem* pItem = m_pBuffer;

	for ( DWORD nPos = m_nBuffer ; nPos ; nPos--, pItem++ )
	{
		if ( nPos == 1 )
			pItem->m_pNext = NULL;
		else
			pItem->m_pNext = pItem + 1;
	}
}

DWORD CRouteCacheTable::GetNextSize(DWORD nDesired)
{
	DWORD nSeconds = ( m_tLast - m_tFirst ) / 1000;
	if ( ! nSeconds ) nSeconds = 1;

	return m_nBuffer * nDesired / nSeconds;
}

void CRouteCacheTable::Clear()
{
	Resize( 0 );
}
