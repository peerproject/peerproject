//
// HubHorizon.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2014
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

class CG2Packet;


class CHubHorizonHub
{
public:
	IN_ADDR			m_pAddress;
	WORD			m_nPort;
	DWORD			m_nReference;
	CHubHorizonHub*	m_pNext;
};


class CHubHorizonGroup
{
public:
	CHubHorizonGroup();
	virtual ~CHubHorizonGroup();

protected:
	CHubHorizonHub**	m_pList;
	DWORD				m_nCount;
	DWORD				m_nBuffer;

public:
	void		Add(IN_ADDR* pAddress, WORD nPort);
	void		Clear();
};


class CHubHorizonPool
{
public:
	CHubHorizonPool();
	virtual ~CHubHorizonPool();

protected:
	CHubHorizonHub*		m_pBuffer;
	DWORD				m_nBuffer;
	CHubHorizonHub*		m_pFree;
	CHubHorizonHub*		m_pActive;
	DWORD				m_nActive;

public:
	void				Setup();
	void				Clear();
	CHubHorizonHub*		Add(IN_ADDR* pAddress, WORD nPort);
	void				Remove(CHubHorizonHub* pHub);
	CHubHorizonHub*		Find(IN_ADDR* pAddress);
	int					AddHorizonHubs(CG2Packet* pPacket);
};

extern CHubHorizonPool	HubHorizonPool;
