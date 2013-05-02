//
// PongCache.h
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

#pragma once

class CPongItem;
class CNeighbour;
class CG1Packet;


class CPongCache
{
// Construction
public:
	CPongCache();
	~CPongCache();

// Operations
public:
	void		Clear();
	void		ClearNeighbour(CNeighbour* pNeighbour);
	BOOL		ClearIfOld();
	CPongItem*	Add(CNeighbour* pNeighbour, IN_ADDR* pAddress, WORD nPort, BYTE nHops, DWORD nFiles, DWORD nVolume);
	CPongItem*	Lookup(CNeighbour* pNotFrom, BYTE nHops, CList< CPongItem* >* pIgnore) const;
	CPongItem*	Lookup(CNeighbour* pFrom) const;

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
//	virtual ~CPongItem();

// Attributes
public:
	CNeighbour*	m_pNeighbour;
	IN_ADDR		m_pAddress;
	WORD		m_nPort;
	BYTE		m_nHops;
	DWORD		m_nFiles;
	DWORD		m_nVolume;

	CG1Packet*	ToPacket(int nTTL, const Hashes::Guid& oGUID) const;
};
