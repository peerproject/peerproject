//
// BTClient.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "Transfer.h"

class CBTPacket;
class CDownload;
class CDownloadSource;
class CDownloadTransferBT;
class CUploadTransferBT;


class CBTClient : public CTransfer
{
// Construction
public:
	CBTClient();
	virtual ~CBTClient();

// Attributes
public:
	Hashes::BtGuid		m_oGUID;
	CUploadTransferBT*	m_pUploadTransfer;
	CDownloadTransferBT* m_pDownloadTransfer;
	CDownload*			m_pDownload;
	BOOL				m_bExtended;		// Extension Protocol support
	BOOL				m_bDHTPort;			// DHT Port support
	BOOL				m_bSeeder;
	BOOL				m_bPrefersEncryption;

protected:
	BOOL				m_bShake;
	BOOL				m_bOnline;
	BOOL				m_bClosing;
	DWORD				m_tLastKeepAlive;
	DWORD				m_tLastUtPex;
	DWORD				m_nUtMetadataID;
	DWORD				m_nUtMetadataSize;
	QWORD				m_nUtPexID;
	QWORD				m_nLtTexID;
	CString				m_sLtTexTrackers;
	QWORD				m_nSrcExchangeID;

// Operations
public:
	static CString	GetUserAgentAzureusStyle(LPBYTE pVendor, size_t nVendor = 6);
	CString			GetUserAgentOtherStyle(LPBYTE pVendor, CString* strNick);
	void			DetermineUserAgent();					// Figure out the other client name/version from the peer ID

	void			SendHandshake(BOOL bPart1, BOOL bPart2);
	void			SendExtendedHandshake();
	void			SendMetadataRequest(QWORD nPiece);
	void			SendInfoRequest(QWORD nPiece);
	void			SendUtPex(DWORD tConnectedAfter = 0);
	void			SendLtTex();
	void			SendDHTPort();
	void			SendSourceRequest();					// Send extended client source (PeerProject/Shareaza) BT_PACKET_SOURCE_REQUEST
	void			SendBeHandshake();						// Send extended client handshake (PeerProject/Shareaza)
	BOOL			OnHandshake1();							// First part of handshake
	BOOL			OnHandshake2();							// Second part- Peer ID
	BOOL			OnPacket(CBTPacket* pPacket);
	BOOL			OnUtPex(CBTPacket* pPacket);
	BOOL			OnLtTex(CBTPacket* pPacket);
	BOOL			OnDHTPort(CBTPacket* pPacket);
	BOOL			OnBeHandshake(CBTPacket* pPacket);		// Process extended client handshake (PeerProject/Shareaza)
	BOOL			OnSourceRequest(CBTPacket* pPacket);	// Process extended client source (PeerProject/Shareaza)
	BOOL			OnMetadataRequest(CBTPacket* pPacket);
	BOOL			OnExtendedHandshake(CBTPacket* pPacket);

	void			Choke();								// Send BT_PACKET_CHOKE
	void			UnChoke();								// Send BT_PACKET_UNCHOKE
	void			Interested();							// Send BT_PACKET_INTERESTED
	void			NotInterested();						// Send BT_PACKET_NOT_INTERESTED
	void			Have(DWORD nBlock);						// Send BT_PACKET_HAVE
	void			Piece(DWORD nBlock, DWORD nOffset, DWORD nLength, LPCVOID pBuffer);
	void			Request(DWORD nBlock, DWORD nOffset, DWORD nLength);
	void			Cancel(DWORD nBlock, DWORD nOffset, DWORD nLength);

	CDownloadSource* GetSource() const;						// Get download transfer source

	inline BOOL		IsOnline() const
	{
		return m_bOnline;
	}

	virtual BOOL	Connect(CDownloadTransferBT* pDownloadTransfer);
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close(UINT nError = 0);

protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnWrite();
	virtual BOOL	OnRead();

	void			Send(CBTPacket* pPacket, BOOL bRelease = TRUE);
};
