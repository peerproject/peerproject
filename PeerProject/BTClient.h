//
// BTClient.h
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

#include "Transfer.h"

class CBTPacket;
class CDownload;
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
    Hashes::BtGuid          m_oGUID;
	BOOL					m_bExtended;		// Send extended details (User name, exact version, etc. For G2 capable clients)
	BOOL					m_bExchange;		// Exchange sources/other info (with extended client)
	CUploadTransferBT*		m_pUpload;
	CDownload*				m_pDownload;
	CDownloadTransferBT*	m_pDownloadTransfer;

protected:
	BOOL					m_bShake;
	BOOL					m_bOnline;
	BOOL					m_bClosing;
	DWORD					m_tLastKeepAlive;

// Operations
public:
	virtual BOOL	Connect(CDownloadTransferBT* pDownloadTransfer);
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close();
	void			Send(CBTPacket* pPacket, BOOL bRelease = TRUE);
	inline BOOL		IsOnline() const throw() { return m_bOnline; }
	static CString	GetAzureusStyleUserAgent(LPBYTE pVendor, size_t nVendor);

protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnWrite();
	virtual BOOL	OnRead();

	void			SendHandshake(BOOL bPart1, BOOL bPart2);
	BOOL			OnHandshake1();								// First part of handshake
	BOOL			OnHandshake2();								// Second part- Peer ID
	//BOOL			OnNoHandshake2();							// If no peer ID is received
	BOOL			OnOnline();
	BOOL			OnPacket(CBTPacket* pPacket);
	void			SendBeHandshake();							// Send extended client handshake
	BOOL			OnBeHandshake(CBTPacket* pPacket);			// Process extended client handshake
	BOOL			OnSourceRequest(CBTPacket* pPacket);
	BOOL			OnDHTPort(CBTPacket* pPacket);
	void			DetermineUserAgent();						// Figure out the other client name/version from the peer ID
};
