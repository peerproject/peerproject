//
// RouteCache.h
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

#if !defined(AFX_ROUTECACHE_H__7FDD7D02_ABC8_4718_A985_C411BCE0D660__INCLUDED_)
#define AFX_ROUTECACHE_H__7FDD7D02_ABC8_4718_A985_C411BCE0D660__INCLUDED_

#pragma once

class CNeighbour;


class CRouteCacheItem
{
// Attributes
public:
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
public:
	CRouteCacheItem*	Add(const Hashes::Guid& oGUID, const CNeighbour* pNeighbour, const SOCKADDR_IN* pEndpoint, DWORD tAdded);
	CRouteCacheItem*	Lookup(const Hashes::Guid& oGUID, CNeighbour** ppNeighbour = NULL, SOCKADDR_IN* pEndpoint = NULL);

};

#endif // !defined(AFX_ROUTECACHE_H__7FDD7D02_ABC8_4718_A985_C411BCE0D660__INCLUDED_)
