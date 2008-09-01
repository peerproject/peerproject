//
// HubHorizon.h
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

#if !defined(AFX_HUBHORIZON_H__CDEFE0F8_036F_4647_9A85_CE2E41984B01__INCLUDED_)
#define AFX_HUBHORIZON_H__CDEFE0F8_036F_4647_9A85_CE2E41984B01__INCLUDED_

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
// Construction
public:
	CHubHorizonGroup();
	virtual ~CHubHorizonGroup();

// Attributes
protected:
	CHubHorizonHub**	m_pList;
	DWORD				m_nCount;
	DWORD				m_nBuffer;

// Operations
public:
	void		Add(IN_ADDR* pAddress, WORD nPort);
	void		Clear();

};


class CHubHorizonPool
{
// Construction
public:
	CHubHorizonPool();
	virtual ~CHubHorizonPool();

// Attributes
protected:
	CHubHorizonHub*		m_pBuffer;
	DWORD				m_nBuffer;
	CHubHorizonHub*		m_pFree;
	CHubHorizonHub*		m_pActive;
	DWORD				m_nActive;

// Operations
public:
	void				Setup();
	void				Clear();
	CHubHorizonHub*		Add(IN_ADDR* pAddress, WORD nPort);
	void				Remove(CHubHorizonHub* pHub);
	CHubHorizonHub*		Find(IN_ADDR* pAddress);
	int					AddHorizonHubs(CG2Packet* pPacket);

};

extern CHubHorizonPool	HubHorizonPool;

#endif // !defined(AFX_HUBHORIZON_H__CDEFE0F8_036F_4647_9A85_CE2E41984B01__INCLUDED_)
