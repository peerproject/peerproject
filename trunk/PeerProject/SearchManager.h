//
// SearchManager.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#if !defined(AFX_SEARCHMANAGER_H__FE71D8A9_8260_4548_B331_C1BF2D0DFEF0__INCLUDED_)
#define AFX_SEARCHMANAGER_H__FE71D8A9_8260_4548_B331_C1BF2D0DFEF0__INCLUDED_

#pragma once

class CManagedSearch;
class CG2Packet;
class CQueryHit;


class CSearchManager
{
// Construction
public:
	CSearchManager();
	virtual ~CSearchManager();

// Attributes
public:
	CMutex			m_pSection;
	Hashes::Guid	m_oLastED2KSearch;
protected:
	CList< CManagedSearch* > m_pList;
	DWORD			m_tLastTick;
	int				m_nPriorityClass;
	int				m_nPriorityCount;

// Operations
public:
	POSITION		GetIterator() const;
	CManagedSearch*	GetNext(POSITION& pos) const;
	INT_PTR			GetCount() const;
	CManagedSearch*	Find(const Hashes::Guid& oGUID);
	void			OnRun();
	BOOL			OnQueryAck(CG2Packet* pPacket, SOCKADDR_IN* pHost, Hashes::Guid& oGUID);
	BOOL			OnQueryHits(CQueryHit* pHits);
	WORD			OnQueryStatusRequest(const Hashes::Guid& oGUID);
protected:
	void			Add(CManagedSearch* pSearch);
	void			Remove(CManagedSearch* pSearch);

	friend class CManagedSearch;
	friend class CSearchWnd;
};

extern CSearchManager SearchManager;

#endif // !defined(AFX_SEARCHMANAGER_H__FE71D8A9_8260_4548_B331_C1BF2D0DFEF0__INCLUDED_)
