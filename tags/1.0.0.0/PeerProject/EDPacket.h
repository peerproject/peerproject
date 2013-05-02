//
// EDPacket.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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

// See eMule opcodes.h http://www.koders.com/c/fidA35361F7667536FD1EE7EE0DABCCDF8D9BF01082.aspx

#pragma once

#include "Packet.h"

#pragma pack(1)

typedef struct
{
	BYTE	nProtocol;
	DWORD	nLength;
	BYTE	nType;
} ED2K_TCP_HEADER;

typedef struct
{
	BYTE	nProtocol;
	BYTE	nType;
} ED2K_UDP_HEADER;

typedef struct
{
	BYTE	nProtocol;
	DWORD	nLength;
	BYTE	nType;
	Hashes::Ed2kHash::RawStorage pMD4;
	DWORD	nOffset1;
	DWORD	nOffset2;
} ED2K_PART_HEADER;

typedef struct
{
	BYTE	nProtocol;
	DWORD	nLength;
	BYTE	nType;
	Hashes::Ed2kHash::RawStorage pMD4;
	QWORD	nOffset1;
	QWORD	nOffset2;
} ED2K_PART_HEADER_I64;

#pragma pack()

typedef struct
{
	DWORD	nType;
	LPCTSTR	pszName;
} ED2K_PACKET_DESC;

#define	ED2K_VERSION					0x3D

// Protocols
#define ED2K_PROTOCOL_EDONKEY			0xE3	// eDonkey, Overnet, MLDonkey and other (OP_EDONKEYHEADER,OP_EDONKEYPROT)
#define ED2K_PROTOCOL_KAD				0xE4	// eMule KAD (OP_KADEMLIAHEADER)
#define ED2K_PROTOCOL_KAD_PACKED		0xE5	// eMule KAD compressed (OP_KADEMLIAPACKEDPROT)
#define ED2K_PROTOCOL_REVCONNECT		0xD0	// RevConnect KAD
#define ED2K_PROTOCOL_REVCONNECT_PACKED	0xD1	// RevConnect KAD compressed
#define ED2K_PROTOCOL_EMULE_PACKED		0xD4	// eMule compressed (OP_PACKEDPROT)
#define ED2K_PROTOCOL_EMULE				0xC5	// eMule (OP_EMULEPROT)
//#define ED2K_PROTOCOL_MLDONKEY		0x00	// MLDonkey (OP_MLDONKEYPROT)
//#define ED2K_PROTOCOL_LANCAST			0xC6	// eMule Plus LANCast
//#define ...							0xA3	// eMule reserved for later UDP headers (OP_UDPRESERVEDPROT1)
//#define ...							0xB2	// eMule reserved for later UDP headers (OP_UDPRESERVEDPROT2)

#define ED2K_MET						0x0E	// First byte of .met-file
#define ED2K_MET_I64TAGS				0x0F	// First byte of .met-file with "Large File" support

#define ED2K_FILE_VERSION1_INITIAL		0x01	// First 4 bytes of .emulecollection-file
#define ED2K_FILE_VERSION2_LARGEFILES	0x02	// First 4 bytes of .emulecollection-file with "Large File" support

class CBuffer;
class CPeerProjectFile;
class CEDClient;
class CEDNeighbour;


class CEDPacket : public CPacket
{
// Construction
protected:
	CEDPacket();
	virtual ~CEDPacket();

// Attributes
public:
	BYTE	m_nEdProtocol;
	BYTE	m_nType;

// Operations
public:
	CString				ReadEDString(BOOL bUnicode);
	void				WriteEDString(LPCTSTR psz, BOOL bUnicode);
	CString				ReadLongEDString(BOOL bUnicode);
	void				WriteLongEDString(LPCTSTR psz, BOOL bUnicode);
	void				WriteFile(const CPeerProjectFile* pFile, QWORD nSize,
							const CEDClient* pClient, const CEDNeighbour* pServer = NULL, bool bPartial = false);
	BOOL				Deflate();
	BOOL				Inflate();	// Unzip packet if any

	virtual void		Reset();
	virtual	void		ToBuffer(CBuffer* pBuffer, bool bTCP = true) const;
	static	CEDPacket*	ReadBuffer(CBuffer* pBuffer);
	virtual CString		GetType() const;

#ifdef _DEBUG
	virtual void		Debug(LPCTSTR pszReason) const;
#endif

	inline static bool IsLowID(DWORD nID) { return nID > 0 && nID < 16777216; }

// Packet Types
protected:
	static ED2K_PACKET_DESC m_pszTypes[];

// Packet Pool
protected:
	class CEDPacketPool : public CPacketPool
	{
	public:
		virtual ~CEDPacketPool() { Clear(); }
	protected:
		virtual void NewPoolImpl(int nSize, CPacket*& pPool, int& nPitch);
		virtual void FreePoolImpl(CPacket* pPool);
	};

	static CEDPacketPool POOL;

// Construction
public:
	static CEDPacket* New(BYTE nType, BYTE nProtocol = ED2K_PROTOCOL_EDONKEY)
	{
		if ( CEDPacket* pPacket = (CEDPacket*)POOL.New() )
		{
			pPacket->m_nEdProtocol = nProtocol;
			pPacket->m_nType = nType;
			return pPacket;
		}
		return NULL;
	}

	static CEDPacket* New(const ED2K_TCP_HEADER* pHeader)
	{
		if ( CEDPacket* pPacket = (CEDPacket*)POOL.New() )
		{
			pPacket->m_nEdProtocol	= pHeader->nProtocol;
			pPacket->m_nType		= pHeader->nType;
			if ( pPacket->Write( &pHeader[1], pHeader->nLength - 1 ) )
				return pPacket;
			pPacket->Release();
		}
		return NULL;
	}

	static CEDPacket* New(const ED2K_UDP_HEADER* pHeader, DWORD nLength)
	{
		if ( CEDPacket* pPacket = (CEDPacket*)POOL.New() )
		{
			pPacket->m_nEdProtocol	= pHeader->nProtocol;
			pPacket->m_nType		= pHeader->nType;
			if ( pPacket->Write( &pHeader[1], nLength - sizeof( *pHeader ) ) && pPacket->Inflate() )
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

	friend class CEDPacket::CEDPacketPool;

private:
	CEDPacket(const CEDPacket&);
	CEDPacket& operator=(const CEDPacket&);
};

inline void CEDPacket::CEDPacketPool::NewPoolImpl(int nSize, CPacket*& pPool, int& nPitch)
{
	nPitch	= sizeof( CEDPacket );
	pPool	= new CEDPacket[ nSize ];
}

inline void CEDPacket::CEDPacketPool::FreePoolImpl(CPacket* pPacket)
{
	delete [] (CEDPacket*)pPacket;
}

// Client - Server, Local (TCP)
#define ED2K_C2S_LOGINREQUEST			0x01
#define ED2K_C2S_GETSERVERLIST			0x14
#define	ED2K_C2S_OFFERFILES				0x15
#define ED2K_C2S_SEARCHREQUEST			0x16
#define ED2K_C2S_SEARCHUSER				0x1a
#define ED2K_C2S_GETSOURCES				0x19
#define ED2K_C2S_CALLBACKREQUEST		0x1C
#define ED2K_C2S_MORERESULTS			0x21
#define ED2K_S2C_REJECTED				0x05
#define ED2K_S2C_SERVERMESSAGE			0x38
#define ED2K_S2C_IDCHANGE				0x40
#define ED2K_S2C_SERVERLIST				0x32
#define ED2K_S2C_SEARCHRESULTS			0x33
#define ED2K_S2C_FOUNDSOURCES			0x42
#define	ED2K_S2C_SERVERSTATUS			0x34
#define ED2K_S2C_SERVERIDENT			0x41
#define ED2K_S2C_CALLBACKREQUESTED		0x35

// Client - Server, Global (UDP)
#define ED2K_C2SG_SEARCHREQUEST3		0x90
#define ED2K_C2SG_SEARCHREQUEST2		0x92
#define ED2K_C2SG_GETSOURCES2			0x94
#define ED2K_C2SG_SERVERSTATUSREQUEST	0x96
#define	ED2K_S2CG_SERVERSTATUS			0x97
#define ED2K_C2SG_SEARCHREQUEST			0x98
#define ED2K_S2CG_SEARCHRESULT			0x99
#define ED2K_C2SG_GETSOURCES			0x9A
#define ED2K_S2CG_FOUNDSOURCES			0x9B
#define ED2K_C2SG_CALLBACKREQUEST		0x9C
#define ED2K_S2CG_CALLBACKFAIL			0x9E

// Client - Client, TCP
#define ED2K_C2C_HELLO					0x01	// 0x10<HASH 16><ID 4><PORT 2><1 Tag_set>
#define ED2K_C2C_HELLOANSWER			0x4C
#define ED2K_C2C_FILEREQUEST			0x58
#define ED2K_C2C_FILEREQANSWER			0x59
#define ED2K_C2C_FILENOTFOUND			0x48
#define ED2K_C2C_FILESTATUS				0x50
#define ED2K_C2C_QUEUEREQUEST			0x54
#define ED2K_C2C_QUEUERELEASE			0x56
#define ED2K_C2C_QUEUERANK				0x5C
#define ED2K_C2C_STARTUPLOAD			0x55
#define ED2K_C2C_FINISHUPLOAD			0x57
#define ED2K_C2C_REQUESTPARTS			0x47
#define ED2K_C2C_SENDINGPART			0x46
#define ED2K_C2C_FILESTATUSREQUEST		0x4F
#define ED2K_C2C_HASHSETREQUEST			0x51
#define ED2K_C2C_HASHSETANSWER			0x52
#define ED2K_C2C_ASKSHAREDFILES			0x4A
#define ED2K_C2C_ASKSHAREDFILESANSWER	0x4B
#define ED2K_C2C_MESSAGE				0x4E
#define ED2K_C2C_CHANGECLIENTID			0x4D	// <New client ID 4><New server IP 4>
#define ED2K_C2C_ASKSHAREDDIRS			0x5D	// (null)
#define ED2K_C2C_ASKSHAREDDIRSANSWER	0x5F	// <count 4>(<len 2><Directory len>)[count]
#define ED2K_C2C_VIEWSHAREDDIR			0x5E	// <len 2><Directory len>
#define ED2K_C2C_VIEWSHAREDDIRANSWER	0x60	// <len 2><Directory len><count 4>(<HASH 16><ID 4><PORT 2><1 Tag_set>)[count]
#define ED2K_C2C_ASKSHAREDDIRSDENIED	0x61	// (null)

// eMule Client - Client TCP
#define	ED2K_C2C_EMULEINFO				0x01	//
#define	ED2K_C2C_EMULEINFOANSWER		0x02	//
#define ED2K_C2C_COMPRESSEDPART			0x40	// <HASH 16><von 4><size 4><Daten len:size>
#define ED2K_C2C_QUEUERANKING			0x60	// <RANG 2>
#define ED2K_C2C_FILEDESC				0x61	// <len 2><NAME len>
#define ED2K_C2C_REQUESTSOURCES			0x81	// *DEPRECATED* <HASH 16>
#define ED2K_C2C_ANSWERSOURCES			0x82	// *DEPRECATED*
#define ED2K_C2C_REQUESTSOURCES2		0x83	// <HASH 16><Version 1><Options 2>
#define ED2K_C2C_ANSWERSOURCES2			0x84	// <Version 1>[content]
#define ED2K_C2C_PUBLICKEY				0x85	// <len 1><pubkey len>
#define ED2K_C2C_SIGNATURE				0x86	// v1: <len 1><signature len>  v2:<len 1><signature len><sigIPused 1>
#define ED2K_C2C_SECIDENTSTATE			0x87	// <state 1><rndchallenge 4>
#define ED2K_C2C_REQUESTPREVIEW			0x90	// <HASH 16>
#define ED2K_C2C_PREVIEWANWSER			0x91	// <HASH 16><frames 1>{frames * <len 4><frame len>}
#define ED2K_C2C_MULTIPACKET			0x92	// *DEPRECATED*
#define ED2K_C2C_MULTIPACKETANSWER		0x93	// *DEPRECATED*
#define ED2K_C2C_PEERCACHE_QUERY		0x94	// *DEPRECATED*
#define ED2K_C2C_PEERCACHE_ANSWER		0x95	// *DEPRECATED*
#define ED2K_C2C_PEERCACHE_ACK			0x96	// *DEPRECATED*
#define ED2K_C2C_PUBLICIP_REQ			0x97
#define ED2K_C2C_PUBLICIP_ANSWER		0x98
#define ED2K_C2C_CALLBACK				0x99	// <HASH 16><HASH 16><uint 16>
#define ED2K_C2C_REASKCALLBACKTCP		0x9A
#define ED2K_C2C_AICHREQUEST			0x9B	// *DEPRECATED* <HASH 16><uint16><HASH aichhashlen>
#define ED2K_C2C_AICHANSWER				0x9C	// *DEPRECATED* <HASH 16><uint16><HASH aichhashlen> <data>
#define ED2K_C2C_AICHFILEHASHANS		0x9D
#define ED2K_C2C_AICHFILEHASHREQ		0x9E
#define ED2K_C2C_BUDDYPING				0x9F
#define ED2K_C2C_BUDDYPONG				0xA0
#define ED2K_C2C_COMPRESSEDPART_I64		0xA1	// 64Bit LargeFile support: <HASH 16><von 8><size 4><Daten len:size>
#define ED2K_C2C_SENDINGPART_I64		0xA2	// 64Bit LargeFile support: <HASH 16><von 8><bis 8><Daten len:(von-bis)>
#define ED2K_C2C_REQUESTPARTS_I64		0xA3	// 64Bit LargeFile support: <HASH 16><von[3] 8*3><bis[3] 8*3>
#define ED2K_C2C_MULTIPACKET_EXT		0xA4	// *DEPRECATED*
#define ED2K_C2C_CHATCAPTCHAREQ			0xA5	// <tags 1>[tags]<Captcha BITMAP>
#define ED2K_C2C_CHATCAPTCHARES			0xA6	// <status 1>
#define ED2K_C2C_FWCHECKUDPREQ			0xA7	// <Inter_Port 2><Extern_Port 2><KadUDPKey 4> *Support required for Kadversion >= 6
#define ED2K_C2C_KAD_FWTCPCHECK_ACK		0xA8	// (null/reserved), replaces KADEMLIA_FIREWALLED_ACK_RES, *Support required for Kadversion >= 7
#define ED2K_C2C_MULTIPACKET_EXT2		0xA9	// <FileIdentifier> ...
#define ED2K_C2C_MULTIPACKETANSWER_EXT2 0xB0	// <FileIdentifier> ...
#define ED2K_C2C_HASHSETREQUEST2		0xB1	// <FileIdentifier><Options 1>
#define ED2K_C2C_HASHSETANSWER2			0xB2	// <FileIdentifier><Options 1>[<HashSets> Options]

// Client - Client, UDP
#define ED2K_C2C_UDP_REASKFILEPING		0x90	// <HASH 16>
#define ED2K_C2C_UDP_REASKACK			0x91	// <RANG 2>
#define ED2K_C2C_UDP_FILENOTFOUND		0x92	// (null)
#define ED2K_C2C_UDP_QUEUEFULL			0x93	// (null)
#define ED2K_C2C_UDP_REASKCALLBACKUDP	0x94	//
#define ED2K_C2C_UDP_DIRECTCALLBACKREQ	0x95	// <TCPPort 2><Userhash 16><ConnectionOptions 1>
#define ED2K_C2C_UDP_PORTTEST			0xFE	// Connection Test

// Values for ED2K_CT_SERVER_FLAGS (server capabilities)
#define ED2K_SRVCAP_ZLIB				0x0001
#define ED2K_SRVCAP_IP_IN_LOGIN			0x0002
#define ED2K_SRVCAP_AUXPORT				0x0004
#define ED2K_SRVCAP_NEWTAGS				0x0008
#define	ED2K_SRVCAP_UNICODE				0x0010
#define	ED2K_SRVCAP_LARGEFILES			0x0100
#define ED2K_SRVCAP_SUPPORTCRYPT		0x0200
#define ED2K_SRVCAP_REQUESTCRYPT		0x0400
#define ED2K_SRVCAP_REQUIRECRYPT		0x0800

// Server TCP flags for ED2K_S2C_IDCHANGE (server capabilities)
#define	ED2K_SERVER_TCP_DEFLATE			0x00000001
#define	ED2K_SERVER_TCP_SMALLTAGS		0x00000008
#define	ED2K_SERVER_TCP_UNICODE			0x00000010
#define	ED2K_SERVER_TCP_GETSOURCES2		0x00000020
#define	ED2K_SERVER_TCP_RELATEDSEARCH	0x00000040
#define ED2K_SERVER_TCP_TYPETAGINTEGER	0x00000080
#define ED2K_SERVER_TCP_64BITSIZE		0x00000100
#define ED2K_SERVER_TCP_TCPOBFUSCATION	0x00000400

// Server UDP flags for ED2K_S2CG_SERVERSTATUS (server capabilities)
#define	ED2K_SERVER_UDP_GETSOURCES		0x00000001
#define	ED2K_SERVER_UDP_GETFILES		0x00000002
#define	ED2K_SERVER_UDP_NEWTAGS			0x00000008
#define	ED2K_SERVER_UDP_UNICODE			0x00000010
#define	ED2K_SERVER_UDP_GETSOURCES2		0x00000020
#define	ED2K_SERVER_UDP_64BITSIZE		0x00000100
#define ED2K_SERVER_UDP_UDPOBFUSCATION	0x00000200
#define ED2K_SERVER_UDP_TCPOBFUSCATION	0x00000400


class CEDTag
{
// Construction
public:
	explicit CEDTag();
	explicit CEDTag(const CEDTag&);
	explicit CEDTag(BYTE nKey, const Hashes::Ed2kHash& oHash);
	explicit CEDTag(BYTE nKey, QWORD nValue);
	explicit CEDTag(BYTE nKey, LPCTSTR pszValue);
	explicit CEDTag(LPCTSTR pszKey, QWORD nValue);
	explicit CEDTag(LPCTSTR pszKey, LPCTSTR pszValue);
	CEDTag& operator=(const CEDTag& t);

// Attributes
public:
	BYTE				m_nType;
	BYTE				m_nKey;
	CString				m_sKey;
	CString				m_sValue;	// ED2K_TAG_STRING
	QWORD				m_nValue;	// ED2K_TAG_INT
	Hashes::Ed2kHash	m_oValue;	// ED2K_TAG_HASH

// Operations
public:
	void	Clear();
	void	Write(CEDPacket* pPacket, BOOL bUnicode = FALSE, BOOL bSmallTags = FALSE);
	BOOL	Read(CEDPacket* pPacket, BOOL bUnicode = FALSE);
	BOOL	Read(CFile* pFile);

// Inlines
public:
	inline BOOL Check(BYTE nKey, BYTE nType) const
	{
		return m_nKey == nKey && m_nType == nType;
	}
};

// Standard tags
#define ED2K_TAG_NULL				0x00
#define ED2K_TAG_HASH				0x01	// 16 bytes MD4 hash
#define ED2K_TAG_STRING				0x02	// Length prefixed string
#define ED2K_TAG_INT				0x03	// 32 bit integer
#define ED2K_TAG_FLOAT				0x04	// 32 bit float
#define ED2K_TAG_BOOL				0x05
#define ED2K_TAG_BOOL_ARRAY			0x06
#define ED2K_TAG_BLOB				0x07	// 32 bit size, then value (eMule versions prior to 0.42e.29 uses 16 bit size)
#define ED2K_TAG_UINT16				0x08	// 16 bit integer
#define ED2K_TAG_UINT8				0x09	// 8 bit integer
#define ED2K_TAG_BSOB				0x0A	// 8 bit size, then value
#define ED2K_TAG_UINT64				0x0B	// 64 bit integer
#define ED2K_TAG_SHORTSTRING		0x11	// String <=16 bytes, using tag ID for length
// 0x10 to 0x20 are reserved for short strings

// Server tags / met files
#define ED2K_ST_SERVERNAME			0x01
#define ED2K_ST_DESCRIPTION			0x0B
#define ED2K_ST_PING				0x0C
#define ED2K_ST_PREFERENCE			0x0E
#define ED2K_ST_FAIL				0x0D
#define	ED2K_ST_DYNIP				0x85
#define ED2K_ST_LASTPING			0x86
#define ED2K_ST_MAXUSERS			0x87
#define ED2K_ST_MAXFILES			0x88
#define ED2K_ST_UDPFLAGS			0x92

// Client tags
#define ED2K_CT_NAME				0x01
#define	ED2K_CT_PORT				0x0F
#define ED2K_CT_VERSION				0x11
#define ED2K_CT_SERVER_FLAGS		0x20	// Tell server about compression, new tags, unicode
#define ED2K_CT_MODVERSION			0x55	// MOD version
#define	ED2K_CT_UDPPORTS			0xF9	// Ports used for UDP (hi word - KAD port, low word - UDP port)
#define	ED2K_CT_FEATUREVERSIONS		0xFA	// <uint32> Features 1:
											//  3 AICH Version (0 = not supported)
											//  1 Unicode
											//  4 UDP version
											//  4 Data compression version
											//  4 Secure Ident
											//  4 Source Exchange - deprecated
											//  4 Ext. Requests
											//  4 Comments
											//  1 PeerCache supported
											//  1 Browse disabled
											//  1 MultiPacket - deprecated with FileIdentifiers/MultipacketExt2
											//  1 Preview
#define	ED2K_CT_SOFTWAREVERSION		0xFB	// Version of the program.
#define	ED2K_CT_BUDDYIP 			0xFC	// BUDDY IP, Was ED2K_CT_UNKNOWN1
#define	ED2K_CT_BUDDYUDP			0xFD	// BUDDY Port, Was ED2K_CT_UNKNOWN2 (hi word not used, low word is UDP port)
#define	ED2K_CT_UNKNOWN3			0xFF
#define	ED2K_CT_MOREFEATUREVERSIONS	0xFE	// <uint32> Features 2:
											// 19 Reserved
											//  1 Direct UDP Callback. Direct callback is only possible if connected to kad, tcp firewalled and verified UDP open (for example on a full cone NAT)
											//  1 Supports Captcha
											//  1 Supports SourceExachnge2 Packets, ignores SX1 Packet Version
											//  1 Requires CryptLayer
											//  1 Requests CryptLayer
											//  1 Supports CryptLayer
											//  1 Reserved (ModBit)
											//  1 Ext Multipacket (Hash+Size instead of Hash)
											//  1 Large Files (includes support for 64bit tags)
											//  4 Kad Version - can only go up to version 15


// File tags
#define ED2K_FT_FILENAME			0x01	// <string>
#define ED2K_FT_FILESIZE			0x02	// <uint32> (or <uint64> when supported)
#define ED2K_FT_FILETYPE			0x03	// <string>
#define ED2K_FT_FILEFORMAT			0x04	// <string>
#define ED2K_FT_LASTSEENCOMPLETE	0x05	// <uint32> (0 - currently available)
#define ED2K_FT_TRANSFERED			0x08	// <uint32>
#define ED2K_FT_GAPSTART			0x09	// <uint32>
#define ED2K_FT_GAPEND				0x0A	// <uint32>
#define ED2K_FT_DESCRIPTION			0x0B	// <string>
#define ED2K_FT_PARTFILENAME		0x12	// <string>
//#define ED2K_FT_PRIORITY			0x13	// <uint32> (Not used anymore)
#define ED2K_FT_STATUS				0x14	// <uint32>
#define ED2K_FT_SOURCES				0x15	// <uint32>
#define ED2K_FT_PERMISSIONS			0x16	// <uint32>
//#define ED2K_FT_ULPRIORITY		0x17	// <uint32> (Not used anymore)
#define ED2K_FT_DLPRIORITY			0x18	// (Was ED2K_FT_PRIORITY)
#define ED2K_FT_ULPRIORITY			0x19
#define ED2K_FT_COMPRESSION			0x1A
#define ED2K_FT_CORRUPTED			0x1B
#define ED2K_FT_KADLASTPUBLISHKEY	0x20	// <uint32>
#define ED2K_FT_KADLASTPUBLISHSRC	0x21	// <uint32>
#define ED2K_FT_FLAGS				0x22	// <uint32>
#define ED2K_FT_DL_ACTIVE_TIME		0x23	// <uint32>
#define ED2K_FT_CORRUPTEDPARTS		0x24	// <string>
#define ED2K_FT_DL_PREVIEW			0x25
#define ED2K_FT_KADLASTPUBLISHNOTES	0x26	// <uint32>
#define ED2K_FT_AICH_HASH			0x27
#define ED2K_FT_FILEHASH			0x28
#define ED2K_FT_COMPLETE_SOURCES	0x30	// <uint32>
#define ED2K_FT_COLLECTIONAUTHOR	0x31	// <string>
#define ED2K_FT_COLLECTIONAUTHORKEY	0x32	// <blob>
#define ED2K_FT_FILESIZE_HI			0x3A	// <uint32>
// Statistics
#define ED2K_FT_ATTRANSFERED		0x50	// <uint32>
#define ED2K_FT_ATREQUESTED			0x51	// <uint32>
#define ED2K_FT_ATACCEPTED			0x52	// <uint32>
#define ED2K_FT_CATEGORY			0x53	// <uint32>
#define ED2K_FT_ATTRANSFERREDHI		0x54	// <uint32>
#define ED2K_FT_MAXSOURCES			0x55	// <uint32>
// Metadata
#define	ED2K_FT_ARTIST				0xD0	// <string>
#define	ED2K_FT_ALBUM				0xD1	// <string>
#define	ED2K_FT_TITLE				0xD2	// <string>
#define ED2K_FT_LENGTH				0xD3	// <uint32>
#define ED2K_FT_BITRATE				0xD4	// <uint32>
#define ED2K_FT_CODEC				0xD5	// <string>
#define ED2K_FT_FILECOMMENT			0xF6	// <string>
#define ED2K_FT_FILERATING			0xF7	// <uint8>


// eMuleinfo tags. Note this is now obsolete, due to ED2K_CT_FEATUREVERSIONS
#define ED2K_ET_COMPRESSION			0x20
#define ED2K_ET_UDPPORT				0x21
#define ED2K_ET_UDPVER				0x22
#define ED2K_ET_SOURCEEXCHANGE		0x23
#define ED2K_ET_COMMENTS			0x24
#define ED2K_ET_EXTENDEDREQUEST		0x25
#define ED2K_ET_COMPATIBLECLIENT	0x26	// Client ID.
#define ED2K_ET_FEATURES			0x27	// Preview and sec ID
#define ED2K_ET_INCOMPLETEPARTS		0x3D	// ICS
#define ED2K_ET_L2HAC				0x3E	// L2HAC
#define ED2K_ET_MOD_FEATURESET		0x54	// [Bloodymad Featureset]
#define ED2K_ET_MOD_VERSION			0x55	// Mod ver Generic String
#define ED2K_ET_MOD_PLUS			0x99	// To avoid conflicts with ET_TAROD_VERSION recognized by lugdunum srvers

// Max files (Hash + Size) in a getsources packet
#define ED2K_MAXFILESINPACKET		0x20

// Max message (chat) length
#define ED2K_MESSAGE_MAX			500
// Max file comment length
#define ED2K_COMMENT_MAX			250
// Max file size in 32 bits
#define MAX_SIZE_32BIT				0xFFFFFFFF
// Name of incomplete virtual folder
#define OP_INCOMPLETE_SHARED_FILES	"!Incomplete Files"

// Client ID
// ED2K_CLIENT_ID predefined as 80 (0x50)

// "Unknown" and "Unknown mod" client ID for compatible client variable
#define ED2K_CLIENT_UNKNOWN			0xFF
#define ED2K_CLIENT_MOD				0xFE

// PeerProject's advertised capabilities / feature versions
#define ED2K_VERSION_COMPRESSION	0x01
#define ED2K_VERSION_UDP			0x02
#define ED2K_VERSION_SOURCEEXCHANGE	0x02
#define ED2K_VERSION_COMMENTS		0x01
#define ED2K_VERSION_EXTENDEDREQUEST 0x02	// Note: Defined at run time. 0, 1, or 2

// Things that aren't supported
#define ED2K_VERSION_AICH			0x00
#define ED2K_VERSION_SECUREID		0x00
