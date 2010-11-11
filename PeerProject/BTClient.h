//
// BTClient.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
	BOOL				m_bExchange;		// Exchange sources/other info (with extended client)
	BOOL				m_bExtended;		// Extension Protocol support
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

// Operations
public:
	virtual BOOL	Connect(CDownloadTransferBT* pDownloadTransfer);
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close(UINT nError = 0);

	void			Send(CBTPacket* pPacket, BOOL bRelease = TRUE);
	inline BOOL		IsOnline() const throw() { return m_bOnline; }
	static CString	GetUserAgentAzureusStyle(LPBYTE pVendor, size_t nVendor = 6);
	CString			GetUserAgentOtherStyle(LPBYTE pVendor, CString* strNick);

protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnWrite();
	virtual BOOL	OnRead();

	void			SendHandshake(BOOL bPart1, BOOL bPart2);
	void			SendExtendedHandshake();
	void			SendMetadataRequest(QWORD nPiece);
	void			SendInfoRequest(QWORD nPiece);
	void			SendUtPex(DWORD tConnectedAfter = 0);
	BOOL			OnHandshake1();						// First part of handshake
	BOOL			OnHandshake2();						// Second part- Peer ID
	//BOOL			OnNoHandshake2();					// If no peer ID is received
	BOOL			OnOnline();
	BOOL			OnPacket(CBTPacket* pPacket);
	void			SendBeHandshake();					// Send extended client handshake
	BOOL			OnBeHandshake(CBTPacket* pPacket);	// Process extended client handshake
	BOOL			OnSourceRequest(CBTPacket* pPacket);
	BOOL			OnDHTPort(CBTPacket* pPacket);
	BOOL			OnExtended(CBTPacket* pPacket);
	void			DetermineUserAgent();				// Figure out the other client name/version from the peer ID

	CDownloadSource* GetSource() const;					// Get download transfer source
};
