//
// Datagram.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

class CBuffer;
class CG2Packet;


class CDatagramIn
{
// Construction
public:
	CDatagramIn();
	virtual ~CDatagramIn();

// Attributes
public:
	CDatagramIn*	m_pNextHash;
	CDatagramIn**	m_pPrevHash;
	CDatagramIn*	m_pNextTime;
	CDatagramIn*	m_pPrevTime;

	SOCKADDR_IN		m_pHost;
	BOOL			m_bCompressed;
	WORD			m_nSequence;
	BYTE			m_nCount;
	BYTE			m_nLeft;
	DWORD			m_tStarted;

	CBuffer**		m_pBuffer;
	BOOL*			m_pLocked;
	BYTE			m_nBuffer;

// Operations
public:
	void		Create(const SOCKADDR_IN* pHost, BYTE nFlags, WORD nSequence, BYTE nCount);
	BOOL		Add(BYTE nPart, LPCVOID pData, DWORD nLength);
	CG2Packet*	ToG2Packet();
};
