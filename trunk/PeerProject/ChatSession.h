//
// ChatSession.h
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

#include "Connection.h"

class CEDPacket;
class CEDClient;
class CG2Packet;
class CGProfile;
class CChatWnd;
class CPrivateChatWnd;


class CChatSession : public CConnection
{
public:
	CChatSession(PROTOCOLID nProtocol = PROTOCOL_ANY, CPrivateChatWnd* pFrame = NULL);
	virtual ~CChatSession();

public:
	Hashes::Guid	m_oGUID;

	int				m_nState;
	BOOL			m_bMustPush;
	CString			m_sUserNick;
	BOOL			m_bUnicode;		// ED2K Client in UTF-8 format
	DWORD			m_nClientID;	// ED2K Client ID (if appropriate)
	SOCKADDR_IN		m_pServer;		// ED2K server (if appropriate)

public:
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close(UINT nError = 0);
	virtual void	OnDropped();

	TRISTATE		GetConnectedState() const;
	BOOL			Connect();
	BOOL			SendPush(BOOL bAutomatic);
	BOOL			OnPush(const Hashes::Guid& oGUID, CConnection* pConnection);
	void			OnED2KMessage(CEDPacket* pPacket);

	BOOL			SendPrivateMessage(bool bAction, const CString& strText);
//	BOOL			SendAwayMessage(const CString& strText);
	void			StatusMessage(int nFlags, UINT nID, ...);
//	void			NotifyMessage(MessageType bType, const CString& sFrom, const CString& sMessage = CString(), HBITMAP hBitmap = NULL);
	void			OnOpenWindow();
	void			OnCloseWindow();

protected:
	TRISTATE			m_bOld; 		// Chat version: TRI_TRUE = CHAT/0.1
	DWORD				m_tPushed;
	CGProfile*			m_pProfile;
	CPrivateChatWnd*	m_pWndPrivate;
//	CList< MSG >		m_pMessages;	// Undelivered messages queue

	BOOL	ReadHandshake();
	BOOL	ReadText();
	BOOL	ReadPackets();
	void	MakeActive();
	void	PostOpenWindow();
	BOOL	OnText(const CString& str);

	// G1/G2
	void	Send(CG2Packet* pPacket);
	BOOL	OnChatMessage(CG2Packet* pPacket);
	BOOL	OnPacket(CG2Packet* pPacket);
	BOOL	OnProfileChallenge(CG2Packet* pPacket);
	BOOL	OnProfileDelivery(CG2Packet* pPacket);
	BOOL	OnChatRequest(CG2Packet* pPacket);
	BOOL	OnChatAnswer(CG2Packet* pPacket);
	BOOL	OnEstablished();

	// ED2K
	BOOL	ReadPacketsED2K();
	BOOL	SendPacketsED2K();
	BOOL	SendChatMessage(CEDPacket* pPacket);
	BOOL	OnChatMessage(CEDPacket* pPacket);
	BOOL	OnCaptchaRequest(CEDPacket* pPacket);
	BOOL	OnCaptchaResult(CEDPacket* pPacket);

	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual BOOL	OnRead();
//	virtual void	OnDropped();	// Public
	virtual BOOL	OnHeaderLine(CString& strHeader, CString& strValue);
	virtual BOOL	OnHeadersComplete();
};

enum
{
	cssNull, cssConnecting, cssRequest1, cssHeaders1, cssRequest2, cssHeaders2,
	cssRequest3, cssHeaders3, cssHandshake, cssActive, cssAway
};
