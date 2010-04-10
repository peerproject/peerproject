//
// PongCache.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

class CPongItem;
class CNeighbour;
class CG1Packet;


class CPongCache
{
// Construction
public:
	CPongCache();
	virtual ~CPongCache();


// Operations
public:
	void		Clear();
	void		ClearNeighbour(CNeighbour* pNeighbour);
	BOOL		ClearIfOld();
	CPongItem*	Add(CNeighbour* pNeighbour, IN_ADDR* pAddress, WORD nPort, BYTE nHops, DWORD nFiles, DWORD nVolume);
	CPongItem*	Lookup(CNeighbour* pNotFrom, BYTE nHops, CList< CPongItem* >* pIgnore);
	CPongItem*	Lookup(CNeighbour* pFrom);

//	POSITION	GetIterator() const;
//	CPongItem*	GetNext(POSITION& pos) const;

// Attributes
protected:
	CList< CPongItem* >	m_pCache;
	DWORD		m_nTime;
};


class CPongItem
{
// Construction
public:
	CPongItem(CNeighbour* pNeighbour, IN_ADDR* pAddress, WORD nPort, BYTE nHops, DWORD nFiles, DWORD nVolume);
	virtual ~CPongItem();

// Attributes
public:
	CNeighbour*	m_pNeighbour;
	IN_ADDR		m_pAddress;
	WORD		m_nPort;
	BYTE		m_nHops;
	DWORD		m_nFiles;
	DWORD		m_nVolume;

	CG1Packet*	ToPacket(int nTTL, const Hashes::Guid& oGUID);
};
