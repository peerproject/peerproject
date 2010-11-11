//
// Datagrams.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008
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

#pragma pack(1)

typedef struct
{
	CHAR	szTag[3];
	BYTE	nFlags;
	WORD	nSequence;
	BYTE	nPart;
	BYTE	nCount;
} SGP_HEADER;

#pragma pack()

// #define SGP_TAG_1	"SGP"
#define SGP_TAG_2		"GND"
#define SGP_DEFLATE		0x01
#define SGP_ACKNOWLEDGE	0x02

typedef struct
{
	DWORD		nTotal;
	DWORD		tLast;
	DWORD		nMeasure;
	DWORD		pHistory[24];
	DWORD		pTimes[24];
	DWORD		nPosition;
	DWORD		tLastAdd;
	DWORD		tLastSlot;
} UDPBandwidthMeter;

class CBuffer;
class CPacket;
class CG1Packet;
class CG2Packet;
class CDatagramIn;
class CDatagramOut;
class CBENode;

class CDatagrams
{
// Construction
public:
	CDatagrams();
	virtual ~CDatagrams();

// Attributes
public:
	UDPBandwidthMeter	m_mInput;
	DWORD				m_nInBandwidth;
	DWORD				m_nInFrags;
	DWORD				m_nInPackets;
	UDPBandwidthMeter	m_mOutput;
	DWORD				m_nOutBandwidth;
	DWORD				m_nOutFrags;
	DWORD				m_nOutPackets;

protected:
	SOCKET			m_hSocket;
	WORD			m_nSequence;
	BOOL			m_bStable;
	DWORD			m_tLastWrite;

	DWORD			m_nBufferBuffer;	// Number of output buffers (Settings.Gnutella2.UdpBuffers)
	CBuffer*		m_pBufferBuffer;	// Output buffers
	CBuffer*		m_pBufferFree;		// List of free output buffers
	DWORD			m_nBufferFree;		// Number of free output buffer items in list

	DWORD			m_nInputBuffer;
	CDatagramIn*	m_pInputBuffer;
	CDatagramIn*	m_pInputFree;
	CDatagramIn*	m_pInputFirst;
	CDatagramIn*	m_pInputLast;
	CDatagramIn*	m_pInputHash[32];

	DWORD			m_nOutputBuffer;
	CDatagramOut*	m_pOutputBuffer;
	CDatagramOut*	m_pOutputFree;
	CDatagramOut*	m_pOutputFirst;
	CDatagramOut*	m_pOutputLast;
	CDatagramOut*	m_pOutputHash[32];

private:
	// Buffer for current incoming UDP packet. It's global since
	// CDatagrams processes one packet at once only. Maximum UDP size 64KB.
	BYTE		m_pReadBuffer[ 65536 ];

// Operations
public:
	BOOL	Listen();
	void	Disconnect();

	// True if the socket is valid, false if closed
	inline BOOL IsValid() const throw()
	{
		return ( m_hSocket != INVALID_SOCKET );
	}

	// Avoid using this function directly, use !Network.IsFirewalled(CHECK_UDP) instead
	inline BOOL IsStable() const throw()
	{
		return IsValid() && m_bStable;
	}

	inline void SetStable(BOOL bStable = TRUE)
	{
		m_bStable = bStable;
	}

	BOOL	Send(const IN_ADDR* pAddress, WORD nPort, CPacket* pPacket, BOOL bRelease = TRUE, LPVOID pToken = NULL, BOOL bAck = TRUE);
	BOOL	Send(const SOCKADDR_IN* pHost, CPacket* pPacket, BOOL bRelease = TRUE, LPVOID pToken = NULL, BOOL bAck = TRUE);
	void	PurgeToken(LPVOID pToken);
	void	OnRun();

protected:
	BOOL	TryRead();
	BOOL	TryWrite();
	void	Measure();
	void	ManageOutput();
	void	Remove(CDatagramOut* pDG);
	void	Remove(CDatagramIn* pDG, BOOL bReclaimOnly = FALSE);
	void	ManagePartials();

	BOOL	OnDatagram(const SOCKADDR_IN* pHost, const BYTE* pBuffer, DWORD nLength);
	BOOL	OnReceiveSGP(const SOCKADDR_IN* pHost, const SGP_HEADER* pHeader, DWORD nLength);
	BOOL	OnAcknowledgeSGP(const SOCKADDR_IN* pHost, const SGP_HEADER* pHeader, DWORD nLength);
};

extern CDatagrams Datagrams;
