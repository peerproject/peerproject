//
// DCPacket.h
//
// This file is part of PeerProject (peerproject.org) © 2010-2012
// Portions copyright Shareaza Development Team, 2010.
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

#include "Packet.h"


class CDCPacket : public CPacket
{
protected:
	CDCPacket();
	virtual ~CDCPacket();

public:
	virtual CString GetType() const;
	virtual CString ToHex()   const;
	virtual CString ToASCII() const;
	virtual void Reset();
	virtual void ToBuffer(CBuffer* pBuffer, bool bTCP = true) const;
	static	CDCPacket*	ReadBuffer(CBuffer* pBuffer);

#ifdef _DEBUG
	virtual void Debug(LPCTSTR pszReason) const;	// Writes debug info about packet into the PeerProject.log file
#endif

// Packet Pool
protected:
	class CDCPacketPool : public CPacketPool
	{
	public:
		virtual ~CDCPacketPool() { Clear(); }
	protected:
		virtual void NewPoolImpl(int nSize, CPacket*& pPool, int& nPitch);
		virtual void FreePoolImpl(CPacket* pPool);
	};

	static CDCPacketPool POOL;

// Allocation
public:
	inline static CDCPacket* New(const BYTE* pBuffer = NULL, DWORD nLength = 0)
	{
		if ( CDCPacket* pPacket = (CDCPacket*)POOL.New() )
		{
			if ( pBuffer == NULL || pPacket->Write( pBuffer, nLength ) )
				return pPacket;
			pPacket->Release();
		}
		return NULL;
	}

	inline virtual void Delete()
	{
		POOL.Delete( this );
	}

	// Packet handler
	virtual BOOL OnPacket(const SOCKADDR_IN* pHost);

protected:
	BOOL OnCommonHit(const SOCKADDR_IN* pHost);

	friend class CDCPacket::CDCPacketPool;

private:
	CDCPacket(const CDCPacket&);
	CDCPacket& operator=(const CDCPacket&);
};

inline void CDCPacket::CDCPacketPool::NewPoolImpl(int nSize, CPacket*& pPool, int& nPitch)
{
	nPitch	= sizeof(CDCPacket);
	pPool	= new CDCPacket[ nSize ];
}

inline void CDCPacket::CDCPacketPool::FreePoolImpl(CPacket* pPacket)
{
	delete [] (CDCPacket*)pPacket;
}

#define DC_PROTOCOL_MIN_LEN	3
