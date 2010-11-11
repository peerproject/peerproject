//
// ChatSession.h
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

#include "Connection.h"

class CEDPacket;
class CEDClient;
class CG2Packet;
class CGProfile;
class CChatFrame;
class CPrivateChatFrame;


class CChatSession : public CConnection
{
public:
	CChatSession(CChatFrame* pFrame = NULL);
	virtual ~CChatSession();

public:
	Hashes::Guid	m_oGUID;

	int				m_nState;
	BOOL			m_bMustPush;
	CString			m_sUserNick;
	BOOL			m_bUnicode;		// ED2K Client in UTF-8 format
	DWORD			m_nClientID;	// ED2K Client ID (if appropriate)
	SOCKADDR_IN		m_pServer;		// ED2K server (If appropriate)

public:
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close(UINT nError = 0);

	void			Setup(const Hashes::Guid& oGUID, SOCKADDR_IN* pHost, BOOL bMustPush);
	TRISTATE		GetConnectedState() const;
	BOOL			Connect();
	BOOL			SendPush(BOOL bAutomatic);
	BOOL			OnPush(const Hashes::Guid& oGUID, CConnection* pConnection);
	void			OnED2KMessage(CEDPacket* pPacket);

	bool			SendPrivateMessage(bool bAction, const CString& strText);
//	bool			SendAwayMessage(const CString& strText);
	void			StatusMessage(int nFlags, UINT nID, ...);
	void			OnOpenWindow();
	void			OnCloseWindow();

protected:
	BOOL				m_bOld;
	DWORD				m_tPushed;
	CGProfile*			m_pProfile;
	CPrivateChatFrame*	m_pWndPrivate;
	CWnd*				m_pWndPublic;


	BOOL	ReadHandshake();
	BOOL	ReadPacketsED2K();
	BOOL	SendPacketsED2K();
	BOOL	ReadText();
	BOOL	ReadPackets();
	void	MakeActive();
	void	PostOpenWindow();
	void	Print(LPCTSTR pszString, size_t nLength);
	void	Send(CG2Packet* pPacket, BOOL bRelease = TRUE);
	BOOL	SendChatMessage(CEDPacket* pPacket);

	BOOL	OnChatMessage(CEDPacket* pPacket);
	BOOL	OnEstablished();
	BOOL	OnText(const CString& str);
	BOOL	OnPacket(CG2Packet* pPacket);
	BOOL	OnProfileChallenge(CG2Packet* pPacket);
	BOOL	OnProfileDelivery(CG2Packet* pPacket);
	BOOL	OnChatRequest(CG2Packet* pPacket);
	BOOL	OnChatAnswer(CG2Packet* pPacket);
	BOOL	OnChatMessage(CG2Packet* pPacket);

	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual BOOL	OnRead();
	virtual void	OnDropped();
	virtual BOOL	OnHeaderLine(CString& strHeader, CString& strValue);
	virtual BOOL	OnHeadersComplete();
};

enum
{
	cssNull, cssConnecting, cssRequest1, cssHeaders1, cssRequest2, cssHeaders2,
	cssRequest3, cssHeaders3, cssHandshake, cssActive, cssAway
};
