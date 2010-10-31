//
// SearchManager.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "ManagedSearch.h"

class CG2Packet;
class CQueryHit;


class CSearchManager
{
// Construction
public:
	CSearchManager();
	~CSearchManager();

	void			OnRun();
	BOOL			OnQueryAck(CG2Packet* pPacket, const SOCKADDR_IN* pAddress, Hashes::Guid& oGUID);
	BOOL			OnQueryHits(const CQueryHit* pHits);
	WORD			OnQueryStatusRequest(const Hashes::Guid& oGUID);

protected:
	typedef CList< CManagedSearch* > CSearchList;

	CMutexEx		m_pSection;
	CSearchList		m_pList;
	DWORD			m_tLastTick;
	int				m_nPriorityClass;
	int				m_nPriorityCount;
	Hashes::Guid	m_oLastED2KSearch;

	bool			Add(CManagedSearch* pSearch);
	bool			Remove(CManagedSearch* pSearch);
	CSearchPtr		Find(const Hashes::Guid& oGUID) const;

	friend class CManagedSearch;	// m_pSection, m_oLastED2KSearch, Add(), Remove()
};

extern CSearchManager SearchManager;
