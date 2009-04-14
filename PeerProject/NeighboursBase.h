//
// NeighboursBase.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

// Keeps a list of CNeighbour objects, with methods to go through them, add and remove them, and count them
// http://pantheraproject.net/wiki/index.php?title=Developers.Code.CNeighboursBase


#pragma once

class CNeighbour;

// Keeps a list of CNeighbour objects, with methods to go through them, add and remove them, and count them
class CNeighboursBase // Begin the inheritance column CNeighbours : CNeighboursWithConnect : Routing : ED2K : G2 : G1 : CNeighboursBase
{
public:
	CNeighboursBase();
	virtual ~CNeighboursBase();

	DWORD m_nBandwidthIn;  // Total number of bytes transferred through all the sockets, in each direction
	DWORD m_nBandwidthOut;

public:
	POSITION    GetIterator()          const;	// Call GetIterator to get the POSITION value
	CNeighbour* GetNext(POSITION& pos) const;	// Give the POSITION to GetNext to get the neighbour beneath it and move to the next one
	CNeighbour* Get(DWORD_PTR nUnique) const;	// Lookup a neighbour by its unique number, like 2, 3, 4, and so on
	CNeighbour* Get(IN_ADDR* pAddress) const;	// Lookup a neighbour by the remote computer's IP address
	CNeighbour* GetNewest(PROTOCOLID nProtocol, int nState, int nNodeType) const;	// Finds the newest neighbour object

	// Count how many computers we are connected to, specifying various filtering characteristics
	// pass -1 to not filter by protocol, state, or node type
	DWORD GetCount(PROTOCOLID nProtocol, int nState, int nNodeType) const;
	BOOL NeighbourExists(PROTOCOLID nProtocol, int nState, int nNodeType) const; // Use this if you just want to know if there are any or not

	// Methods implemented by several classes in the CNeighbours inheritance column
	virtual void Connect(); // Does nothing, but inheriting classes have Connect methods with code in them
	virtual void Close();   // Calls Close on all the neighbours in the list, and resets member variables back to 0
	virtual void OnRun();   // Calls DoRun on each neighbour in the list, making them send and receive data

	// Add and remove neighbour objects from the list
	virtual void Add(CNeighbour* pNeighbour, BOOL bAssignUnique = TRUE);
	virtual void Remove(CNeighbour* pNeighbour);

	inline DWORD GetStableCount() const
	{
		return m_nStableCount;
	}

private:
	CMap< DWORD_PTR, DWORD_PTR, CNeighbour*, CNeighbour* > m_pUniques;	// The list of remote computers we are connected to
	DWORD	m_nUnique;		// The unique key number we'll try to assign a neighbour object for the map next, like 2, 3, 4, and so on
	DWORD	m_nRunCookie;	// OnRun uses this to run each neighbour once even if GetNext returns the same one more than once in the loop
	DWORD	m_nStableCount;	// The number of connections we have older than 1.5 seconds and finished with the handshake
	DWORD	m_nLeafCount;	// The number of connections we have that are down to leaf nodes below us
	DWORD	m_nLeafContent;	// The total size in bytes of all of the files all of these leaves are sharing
};