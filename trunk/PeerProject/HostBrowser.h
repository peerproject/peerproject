//
// HostBrowser.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "zlib.h"

class CG1Packet;
class CG2Packet;
class CEDPacket;
class CQueryHit;
class CGProfile;
class CBuffer;
class CVendor;
class CBrowseHostWnd;

class CHostBrowser : public CTransfer
{
public:
	CHostBrowser(CBrowseHostWnd* pNotify = NULL, PROTOCOLID nProtocol = PROTOCOL_ANY, IN_ADDR* pAddress = NULL, WORD nPort = 0,
		BOOL bMustPush = FALSE, const Hashes::Guid& pClientID = Hashes::Guid());
	virtual ~CHostBrowser();

public:
	enum { hbsNull, hbsConnecting, hbsRequesting, hbsHeaders, hbsContent };

	int				m_nState;
	CGProfile*		m_pProfile;
	BOOL			m_bNewBrowse;
	IN_ADDR			m_pAddress;
	WORD			m_nPort;
	Hashes::Guid	m_oClientID;
	Hashes::Guid	m_oPushID;
	BOOL			m_bMustPush;
	BOOL			m_bCanPush;
	DWORD			m_tPushed;
	BOOL			m_bConnect;
	int				m_nHits;
	CVendor*		m_pVendor;
	BOOL			m_bCanChat;
	CString			m_sServer;
	BOOL			m_bDeflate;
	DWORD			m_nLength;
	DWORD			m_nReceived;
	CBuffer*		m_pBuffer;
	z_streamp		m_pInflate;

// Operations
public:
	void		Serialize(CArchive& ar);
	BOOL		Browse();
	void		Stop(BOOL bCompleted = FALSE);
	BOOL		IsBrowsing() const;
	float		GetProgress() const;
	void		OnQueryHits(CQueryHit* pHits);

	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnHeadersComplete();
	virtual BOOL	OnPush(const Hashes::Guid& oClientID, CConnection* pConnection);

protected:
	CBrowseHostWnd*	m_pNotify;

	BOOL		SendPush(BOOL bMessage);
	void		SendRequest();
	BOOL		ReadResponseLine();
	BOOL		ReadContent();
	BOOL		StreamContent();
	BOOL		StreamPacketsG1();
	BOOL		StreamPacketsG2();
	BOOL		StreamHTML();
	BOOL		OnPacket(CG1Packet* pPacket);
	BOOL		OnPacket(CG2Packet* pPacket);
	void		OnProfilePacket(CG2Packet* pPacket);

	virtual BOOL	OnRead();
	virtual BOOL	OnHeaderLine(CString& strHeader, CString& strValue);
	virtual BOOL	OnRun();
};
