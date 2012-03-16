//
// RouteCache.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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

#pragma once

class CNeighbour;


class CRouteCacheItem
{
// Attributes
public:
	CRouteCacheItem();

	CRouteCacheItem*	m_pNext;
	DWORD				m_tAdded;
	Hashes::Guid		m_oGUID;
	const CNeighbour*	m_pNeighbour;
	SOCKADDR_IN			m_pEndpoint;
};


class CRouteCacheTable
{
// Construction
public:
	CRouteCacheTable();
	virtual ~CRouteCacheTable();

// Attributes
protected:
	CRouteCacheItem*	m_pHash[1024];
	CRouteCacheItem*	m_pFree;
	CRouteCacheItem*	m_pBuffer;
	DWORD				m_nBuffer;
	DWORD				m_nUsed;
	DWORD				m_tFirst;
	DWORD				m_tLast;

// Operations
public:
	CRouteCacheItem*	Find(const Hashes::Guid& oGUID);
	CRouteCacheItem*	Add(const Hashes::Guid& oGUID, const CNeighbour* pNeighbour, const SOCKADDR_IN* pEndpoint, DWORD nTime = 0);
	void				Remove(CNeighbour* pNeighbour);
	void				Resize(DWORD nSize);
	DWORD				GetNextSize(DWORD nDesired);
	void				Clear();

	inline BOOL IsFull() const
	{
		return m_nUsed == m_nBuffer;
	}
};


class CRouteCache
{
// Construction
public:
	CRouteCache();
	virtual ~CRouteCache();

// Attributes
protected:
	DWORD				m_nSeconds;
	CRouteCacheTable	m_pTable[2];
	CRouteCacheTable*	m_pRecent;
	CRouteCacheTable*	m_pHistory;

// Operations
public:
	void		SetDuration(DWORD nSeconds);
	BOOL		Add(const Hashes::Guid& oGUID, const CNeighbour* pNeighbour);
	BOOL		Add(const Hashes::Guid& oGUID, const SOCKADDR_IN* pEndpoint);
	void		Remove(CNeighbour* pNeighbour);
	void		Clear();

	CRouteCacheItem*	Add(const Hashes::Guid& oGUID, const CNeighbour* pNeighbour, const SOCKADDR_IN* pEndpoint, DWORD tAdded);
	CRouteCacheItem*	Lookup(const Hashes::Guid& oGUID, CNeighbour** ppNeighbour = NULL, SOCKADDR_IN* pEndpoint = NULL);
};
