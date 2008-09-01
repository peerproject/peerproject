//
// DatagramPart.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
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

#if !defined(AFX_DATAGRAMPART_H__293C0502_E9B1_4C85_9E8C_9C641318B939__INCLUDED_)
#define AFX_DATAGRAMPART_H__293C0502_E9B1_4C85_9E8C_9C641318B939__INCLUDED_

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
public:
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
	void	Create(SOCKADDR_IN* pHost, CG2Packet* pPacket, WORD nSequence, CBuffer* pBuffer, BOOL bAck);
	BOOL	GetPacket(DWORD tNow, BYTE** ppPacket, DWORD* pnPacket, BOOL bResend);
	BOOL	Acknowledge(BYTE nPart);

};

#endif // !defined(AFX_DATAGRAMPART_H__293C0502_E9B1_4C85_9E8C_9C641318B939__INCLUDED_)
