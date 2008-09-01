//
// G2Packet.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

class CG1Packet;

typedef QWORD G2_PACKET;

#define G2_TYPE_LEN(p)( \
	( ! ( (G2_PACKET)((p)) & 0x00000000000000ffui64 ) ) ? 0 : ( \
	( ! ( (G2_PACKET)((p)) & 0x000000000000ff00ui64 ) ) ? 1 : ( \
	( ! ( (G2_PACKET)((p)) & 0x0000000000ff0000ui64 ) ) ? 2 : ( \
	( ! ( (G2_PACKET)((p)) & 0x00000000ff000000ui64 ) ) ? 3 : ( \
	( ! ( (G2_PACKET)((p)) & 0x000000ff00000000ui64 ) ) ? 4 : ( \
	( ! ( (G2_PACKET)((p)) & 0x0000ff0000000000ui64 ) ) ? 5 : ( \
	( ! ( (G2_PACKET)((p)) & 0x00ff000000000000ui64 ) ) ? 6 : ( \
	( ! ( (G2_PACKET)((p)) & 0xff00000000000000ui64 ) ) ? 7 : ( \
	8 )))))))))

#define	MAKE_G2_PACKET(a,b,c,d,e,f,g,h) \
	MAKEQWORD(	MAKEDWORD(MAKEWORD(((a)),((b))),MAKEWORD(((c)),((d)))), \
				MAKEDWORD(MAKEWORD(((e)),((f))),MAKEWORD(((g)),((h)))) \
	);


#define G2_PACKET_LEN(p,len) \
	(1+(((len)>0xFF)?(((len)>0xFFFF)?3:2):1)+G2_TYPE_LEN(p)+(len))

//
// G2 Packet Flags
//

#define G2_FLAG_COMPOUND	0x04
#define G2_FLAG_BIG_ENDIAN	0x02

//
// G2 Packet Types
//

const G2_PACKET G2_PACKET_NULL				= MAKE_G2_PACKET(  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_BODY				= MAKE_G2_PACKET( 'B', 'O', 'D', 'Y',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_BOGUS				= MAKE_G2_PACKET( 'B', 'O', 'G', 'U', 'S',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_BROWSE_HOST		= MAKE_G2_PACKET( 'B', 'H',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_BROWSE_PROFILE	= MAKE_G2_PACKET( 'B', 'U', 'P',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CACHED_HUB		= MAKE_G2_PACKET( 'C', 'H',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CACHED_SOURCES	= MAKE_G2_PACKET( 'C', 'S', 'C',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CHAT_ACCEPT		= MAKE_G2_PACKET( 'A', 'C', 'C', 'E', 'P', 'T',  0 ,  0  );
const G2_PACKET G2_PACKET_CHAT_ACTION		= MAKE_G2_PACKET( 'A', 'C', 'T',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CHAT_ANSWER		= MAKE_G2_PACKET( 'C', 'H', 'A', 'T', 'A', 'N', 'S',  0  );
const G2_PACKET G2_PACKET_CHAT_AWAY			= MAKE_G2_PACKET( 'A', 'W', 'A', 'Y',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CHAT_DENY			= MAKE_G2_PACKET( 'D', 'E', 'N', 'Y',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CHAT_MESSAGE		= MAKE_G2_PACKET( 'C', 'M', 'S', 'G',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CHAT_REQUEST		= MAKE_G2_PACKET( 'C', 'H', 'A', 'T', 'R', 'E', 'Q',  0  );
const G2_PACKET G2_PACKET_COLLECTION		= MAKE_G2_PACKET( 'C', 'O', 'L', 'L', 'E', 'C', 'T',  0  );
const G2_PACKET G2_PACKET_COMMENT			= MAKE_G2_PACKET( 'C', 'O', 'M',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CRAWL_ANS			= MAKE_G2_PACKET( 'C', 'R', 'A', 'W', 'L', 'A',  0 ,  0  );
const G2_PACKET G2_PACKET_CRAWL_REQ			= MAKE_G2_PACKET( 'C', 'R', 'A', 'W', 'L', 'R',  0 ,  0  );
const G2_PACKET G2_PACKET_CRAWL_REXT		= MAKE_G2_PACKET( 'R', 'E', 'X', 'T',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CRAWL_RGPS		= MAKE_G2_PACKET( 'R', 'G', 'P', 'S',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CRAWL_RLEAF		= MAKE_G2_PACKET( 'R', 'L', 'E', 'A', 'F',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_CRAWL_RNAME		= MAKE_G2_PACKET( 'R', 'N', 'A', 'M', 'E',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_DESCRIPTIVE_NAME	= MAKE_G2_PACKET( 'D', 'N',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_DISCOVERY			= MAKE_G2_PACKET( 'D', 'I', 'S',  0 ,  0 ,  0 ,  0 ,  0  );	// Extension. UDP request for node /KHL (Ryo-oh-ki)
const G2_PACKET G2_PACKET_DISCOVERY_ANS		= MAKE_G2_PACKET( 'D', 'I', 'S', 'C', 'A',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_DISCOVERY_HUB		= MAKE_G2_PACKET( 'D', 'I', 'S', 'C', 'H',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_DISCOVERY_LOG		= MAKE_G2_PACKET( 'D', 'I', 'S', 'C', 'L',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_DISCOVERY_REQ		= MAKE_G2_PACKET( 'D', 'I', 'S', 'C', 'R',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_FILES				= MAKE_G2_PACKET( 'F', 'I', 'L', 'E', 'S',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_FROM_ADDRESS		= MAKE_G2_PACKET( 'F', 'R',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_FW				= MAKE_G2_PACKET( 'F', 'W',  0 ,  0 ,  0 ,  0 ,  0 ,  0  ); // Extension. LNI Firewall Flag. From GnucDNAR3
const G2_PACKET G2_PACKET_G1				= MAKE_G2_PACKET( 'G', '1',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_GPS				= MAKE_G2_PACKET( 'G', 'P', 'S',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_GROUP_ID			= MAKE_G2_PACKET( 'G',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_HAW				= MAKE_G2_PACKET( 'H', 'A', 'W',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_HIT				= MAKE_G2_PACKET( 'Q', 'H', '2',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_HIT_DESCRIPTOR	= MAKE_G2_PACKET( 'H',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_HIT_GROUP			= MAKE_G2_PACKET( 'H', 'G',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_HIT_WRAP			= MAKE_G2_PACKET( 'Q', 'H', '1',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_HORIZON			= MAKE_G2_PACKET( 'S',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_HUB				= MAKE_G2_PACKET( 'H', 'U', 'B',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_HUB_STATUS		= MAKE_G2_PACKET( 'H', 'S',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_INTEREST			= MAKE_G2_PACKET( 'I',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_KHL				= MAKE_G2_PACKET( 'K', 'H', 'L',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_KHL_ANS			= MAKE_G2_PACKET( 'K', 'H', 'L', 'A',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_KHL_REQ			= MAKE_G2_PACKET( 'K', 'H', 'L', 'R',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_LEAF				= MAKE_G2_PACKET( 'L', 'E', 'A', 'F',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_LIBRARY_STATUS	= MAKE_G2_PACKET( 'L', 'S',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_LNI				= MAKE_G2_PACKET( 'L', 'N', 'I',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_METADATA			= MAKE_G2_PACKET( 'M', 'D',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_NAME				= MAKE_G2_PACKET( 'N', 'A', 'M', 'E',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_NEIGHBOUR_HUB		= MAKE_G2_PACKET( 'N', 'H',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_NEIGHBOUR_LEAF	= MAKE_G2_PACKET( 'N', 'L',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );	// Extension. /CRAWL leaf info
const G2_PACKET G2_PACKET_NICK				= MAKE_G2_PACKET( 'N', 'I', 'C', 'K',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_NODE_ADDRESS		= MAKE_G2_PACKET( 'N', 'A',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_NODE_GUID			= MAKE_G2_PACKET( 'G', 'U',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_NODE_INFO			= MAKE_G2_PACKET( 'N', 'I',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );	// Obsolete. Equal to G2_PACKET_NODE_ADDRESS
const G2_PACKET G2_PACKET_OBJECT_ID			= MAKE_G2_PACKET( 'I', 'D',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PARTIAL			= MAKE_G2_PACKET( 'P', 'A', 'R', 'T',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PEER_BUSY			= MAKE_G2_PACKET( 'B', 'U', 'S', 'Y',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PEER_CHAT			= MAKE_G2_PACKET( 'P', 'C', 'H',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PEER_FIREWALLED	= MAKE_G2_PACKET( 'F', 'W',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PEER_STATUS		= MAKE_G2_PACKET( 'S', 'S',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PEER_UNSTABLE		= MAKE_G2_PACKET( 'U', 'N', 'S', 'T', 'A',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PHYSICAL_FOLDER	= MAKE_G2_PACKET( 'P', 'F',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PING				= MAKE_G2_PACKET( 'P', 'I',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PONG				= MAKE_G2_PACKET( 'P', 'O',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PREVIEW_URL		= MAKE_G2_PACKET( 'P', 'V', 'U',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PROFILE			= MAKE_G2_PACKET( 'U', 'P', 'R', 'O',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PROFILE_AVATAR	= MAKE_G2_PACKET( 'U', 'P', 'R', 'O', 'A', 'V', 'T', 'R' );
const G2_PACKET G2_PACKET_PROFILE_CHALLENGE	= MAKE_G2_PACKET( 'U', 'P', 'R', 'O', 'C',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PROFILE_DELIVERY	= MAKE_G2_PACKET( 'U', 'P', 'R', 'O', 'D',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_PUSH				= MAKE_G2_PACKET( 'P', 'U', 'S', 'H',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QHT				= MAKE_G2_PACKET( 'Q', 'H', 'T',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QKY				= MAKE_G2_PACKET( 'Q', 'K', 'Y',  0 ,  0 ,  0 ,  0 ,  0  );	// Extension. /Q2 query key without /Q2/UDP
const G2_PACKET G2_PACKET_QUERY				= MAKE_G2_PACKET( 'Q', '2',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_ACK			= MAKE_G2_PACKET( 'Q', 'A',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_ADDRESS		= MAKE_G2_PACKET( 'Q', 'N', 'A',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_CACHED		= MAKE_G2_PACKET( 'C', 'A', 'C', 'H', 'E', 'D',  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_DONE		= MAKE_G2_PACKET( 'D',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_KEY			= MAKE_G2_PACKET( 'Q', 'K',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_KEY_ANS		= MAKE_G2_PACKET( 'Q', 'K', 'A',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_KEY_REQ		= MAKE_G2_PACKET( 'Q', 'K', 'R',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_REFRESH		= MAKE_G2_PACKET( 'R', 'E', 'F',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_SEARCH		= MAKE_G2_PACKET( 'S',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_QUERY_WRAP		= MAKE_G2_PACKET( 'Q', '1',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );	// Obsolete
const G2_PACKET G2_PACKET_RELAY				= MAKE_G2_PACKET( 'R', 'E', 'L', 'A', 'Y',  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_REQUEST_ADDRESS	= MAKE_G2_PACKET( 'R', 'N', 'A',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_RETRY_AFTER		= MAKE_G2_PACKET( 'R', 'A',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_SELF				= MAKE_G2_PACKET( 'S', 'E', 'L', 'F',  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_SEND_ADDRESS		= MAKE_G2_PACKET( 'S', 'N', 'A',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_SIZE				= MAKE_G2_PACKET( 'S', 'Z',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_SIZE_RESTRICTION	= MAKE_G2_PACKET( 'S', 'Z', 'R',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_TIMESTAMP			= MAKE_G2_PACKET( 'T', 'S',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_TO				= MAKE_G2_PACKET( 'T', 'O',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_UDP				= MAKE_G2_PACKET( 'U', 'D', 'P',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_URL				= MAKE_G2_PACKET( 'U', 'R', 'L',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_URN				= MAKE_G2_PACKET( 'U', 'R', 'N',  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_USER_GUID			= MAKE_G2_PACKET( 'U', 'S', 'E', 'R', 'G', 'U', 'I', 'D' );
const G2_PACKET G2_PACKET_VENDOR			= MAKE_G2_PACKET( 'V',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_VERSION			= MAKE_G2_PACKET( 'C', 'V',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_VIRTUAL_FOLDER	= MAKE_G2_PACKET( 'V', 'F',  0 ,  0 ,  0 ,  0 ,  0 ,  0  );
const G2_PACKET G2_PACKET_XML				= MAKE_G2_PACKET( 'X', 'M', 'L',  0 ,  0 ,  0 ,  0 ,  0  );

//
// G2 SS
//

#define G2_SS_PUSH		0x01
#define G2_SS_BUSY		0x02
#define G2_SS_STABLE	0x04

class CG2Packet : public CPacket
{
// Construction
protected:
	CG2Packet();
	virtual ~CG2Packet();

// Attributes
public:
	G2_PACKET	m_nType;
	BOOL		m_bCompound;

// Operations
public:
	void	WritePacket(CG2Packet* pPacket);
	void	WritePacket(G2_PACKET nType, DWORD nLength, BOOL bCompound = FALSE);
	BOOL	ReadPacket(G2_PACKET& nType, DWORD& nLength, BOOL* pbCompound = NULL);
	BOOL	SkipCompound();
	BOOL	SkipCompound(DWORD& nLength, DWORD nRemaining = 0);
	BOOL	GetTo(Hashes::Guid& oGUID);
	BOOL	SeekToWrapped();
public:
	virtual void	Reset();
	CG2Packet*		Clone() const;
	virtual CString	ReadString(DWORD nMaximum = 0xFFFFFFFF);
	virtual void	WriteString(LPCTSTR pszString, BOOL bNull = TRUE);
	virtual int		GetStringLen(LPCTSTR pszString) const;
	virtual void	ToBuffer(CBuffer* pBuffer) const;
	virtual void	Debug(LPCTSTR pszReason) const;
public:
	static CG2Packet* ReadBuffer(CBuffer* pBuffer);

	virtual void	WriteString(LPCSTR pszString, BOOL bNull = TRUE);

// Inlines
public:
	inline BOOL IsType(G2_PACKET nType) const
	{
		return nType == m_nType;
	}

	virtual CString GetType() const;

// Packet Pool
protected:
	class CG2PacketPool : public CPacketPool
	{
	public:
		virtual ~CG2PacketPool() { Clear(); }
	protected:
		virtual void NewPoolImpl(int nSize, CPacket*& pPool, int& nPitch);
		virtual void FreePoolImpl(CPacket* pPool);
	};

	static CG2PacketPool POOL;

// Construction
public:
	inline static CG2Packet* New(G2_PACKET nType = G2_PACKET_NULL, BOOL bCompound = FALSE)
	{
		CG2Packet* pPacket = (CG2Packet*)POOL.New();

		if ( nType != G2_PACKET_NULL )
		{
			pPacket->m_nType = nType;
		}

		pPacket->m_bCompound = bCompound;

		return pPacket;
	}

	static CG2Packet* New(BYTE* pSource);
	static CG2Packet* New(G2_PACKET nType, CG1Packet* pWrap, int nMinTTL = 255);

	inline virtual void Delete()
	{
		POOL.Delete( this );
	}

	friend class CG2Packet::CG2PacketPool;
};

inline void CG2Packet::CG2PacketPool::NewPoolImpl(int nSize, CPacket*& pPool, int& nPitch)
{
	nPitch	= sizeof(CG2Packet);
	pPool	= new CG2Packet[ nSize ];
	if ( pPool == NULL )
	{
		theApp.Message( MSG_ERROR, _T("Memory allocation error in CG2Packet::CG2PacketPool::NewPoolImpl()") );
	}
}

inline void CG2Packet::CG2PacketPool::FreePoolImpl(CPacket* pPacket)
{
	delete [] (CG2Packet*)pPacket;
}
