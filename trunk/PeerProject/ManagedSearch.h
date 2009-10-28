//
// ManagedSearch.h
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

#pragma once

#include "QuerySearch.h"

class CPacket;
class CNeighbour;


class CManagedSearch : private boost::noncopyable
{
// Construction
public:
	CManagedSearch(auto_ptr< CQuerySearch > pSearch = auto_ptr< CQuerySearch >( new CQuerySearch() ),
			int nPriority = 0);
	~CManagedSearch() { Stop(); }

	enum { spHighest, spMedium, spLowest, spMax };

// Attributes
public:
	auto_ptr< CQuerySearch > m_pSearch;
	int				m_nPriority;
	BOOL			m_bAllowG2;
	BOOL			m_bAllowG1;
	BOOL			m_bAllowED2K;
public:
	BOOL			m_bActive;
	BOOL			m_bReceive;
public:
	DWORD			m_tStarted;					// Time search was started
	DWORD			m_nHits;					// Total hits
	DWORD			m_nG1Hits;					// G1 hits
	DWORD			m_nG2Hits;					// G2 hits
	DWORD			m_nEDHits;					// ED2k hits
	DWORD			m_nHubs;					// Number of G2 hubs searched
	DWORD			m_nLeaves;					// Number of G2 leaves searched
	DWORD			m_nQueryCount;				// Total Gnutella2 queries sent
	DWORD			m_tLastG2;					// Time a G2 hub was last searched
	DWORD			m_tLastED2K;				// Time an ed2k server was last searched
	DWORD			m_tMoreResults;				// Time more results were requested from an ed2k server
	DWORD			m_nEDServers;				// Number of EDonkey servers searched
	DWORD			m_nEDClients;				// Number of ED2K clients searched (Guess)
protected:
	DWORD			m_tExecute;
	CMap< DWORD, DWORD, DWORD, DWORD > m_pNodes;	// Pair of IP and query time (s)
	CMap< DWORD, DWORD, DWORD, DWORD > m_pG1Nodes;	// Pair of IP and last sent packet TTL

// Operations
public:
	void	Serialize(CArchive& ar);
	void	Start();
	void	Stop();
	BOOL	Execute();
	void	OnHostAcknowledge(DWORD nAddress);
	BOOL	IsLastED2KSearch();
protected:
	BOOL	ExecuteNeighbours(DWORD tTicks, DWORD tSecs);
	BOOL	ExecuteG2Mesh(DWORD tTicks, DWORD tSecs);
	BOOL	ExecuteDonkeyMesh(DWORD tTicks, DWORD tSecs);

// Inlines
public:

	inline CQuerySearch* GetSearch() const { return m_pSearch.get(); }
	inline BOOL IsActive() const { return m_bActive; }
	inline int GetPriority() const { return m_nPriority; }
	inline void SetPriority(int nPriority) { m_nPriority = nPriority; }
};
