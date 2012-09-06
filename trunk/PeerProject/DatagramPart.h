//
// DatagramPart.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2006.
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

class CBuffer;
class CG2Packet;


class CDatagramOut
{
// Construction
public:
	CDatagramOut();
	virtual ~CDatagramOut();

// Attributes
public:
	CDatagramOut*	m_pNextHash;
	CDatagramOut**	m_pPrevHash;
	CDatagramOut*	m_pNextTime;
	CDatagramOut*	m_pPrevTime;

	SOCKADDR_IN		m_pHost;
	WORD			m_nSequence;
	CBuffer*		m_pBuffer;
	LPVOID			m_pToken;
	BOOL			m_bCompressed;
	DWORD			m_nPacket;
	BYTE			m_nCount;
	BYTE			m_nAcked;
	DWORD*			m_pLocked;
	BYTE			m_nLocked;
	DWORD			m_tSent;
	BOOL			m_bAck;

// Operations
public:
	void	Create(const SOCKADDR_IN* pHost, CG2Packet* pPacket, WORD nSequence, CBuffer* pBuffer, BOOL bAck);
	BOOL	GetPacket(DWORD tNow, BYTE** ppPacket, DWORD* pnPacket, BOOL bResend);
	BOOL	Acknowledge(BYTE nPart);
};
