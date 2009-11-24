//
// BTPacket.h
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

#pragma once

#include "Packet.h"

//
// Packet
//

class CBTPacket : public CPacket
{
// Construction
protected:
	CBTPacket();
	virtual ~CBTPacket();

// Attributes
public:
	BYTE	m_nType;

// Operations
public:
	virtual	void		ToBuffer(CBuffer* pBuffer) const;
	static	CBTPacket*	ReadBuffer(CBuffer* pBuffer);
public:
	virtual CString		GetType() const;

// Packet Pool
protected:
	class CBTPacketPool : public CPacketPool
	{
	public:
		virtual ~CBTPacketPool() { Clear(); }
	protected:
		virtual void NewPoolImpl(int nSize, CPacket*& pPool, int& nPitch);
		virtual void FreePoolImpl(CPacket* pPool);
	};

	static CBTPacketPool POOL;

// Allocation
public:
	inline static CBTPacket* New(BYTE nType)
	{
		CBTPacket* pPacket = (CBTPacket*)POOL.New();
		pPacket->m_nType = nType;
		return pPacket;
	}

	inline virtual void Delete()
	{
		POOL.Delete( this );
	}

	friend class CBTPacket::CBTPacketPool;
};

inline void CBTPacket::CBTPacketPool::NewPoolImpl(int nSize, CPacket*& pPool, int& nPitch)
{
	nPitch	= sizeof(CBTPacket);
	pPool	= new CBTPacket[ nSize ];
}

inline void CBTPacket::CBTPacketPool::FreePoolImpl(CPacket* pPacket)
{
	delete [] (CBTPacket*)pPacket;
}

//
// Packet Types
//

#define BT_PACKET_CHOKE				0
#define BT_PACKET_UNCHOKE			1
#define BT_PACKET_INTERESTED		2
#define BT_PACKET_NOT_INTERESTED	3
#define BT_PACKET_HAVE				4
#define BT_PACKET_BITFIELD			5
#define BT_PACKET_REQUEST			6
#define BT_PACKET_PIECE				7
#define BT_PACKET_CANCEL			8
#define BT_PACKET_DHT_PORT			9
#define BT_PACKET_EXTENSION 		20	// http://www.bittorrent.org/beps/bep_0010.html

#define BT_PACKET_HANDSHAKE			128
#define BT_PACKET_SOURCE_REQUEST	129
#define BT_PACKET_SOURCE_RESPONSE	130

#define BT_PACKET_KEEPALIVE			255

#pragma pack(1)

typedef struct
{
	DWORD	nLength;
	BYTE	nType;
	DWORD	nPiece;
	DWORD	nOffset;
} BT_PIECE_HEADER;

#pragma pack()
