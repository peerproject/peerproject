//
// DCNeighbour.h
//
// This file is part of PeerProject (peerproject.org) � 2010-2012
// Portions copyright Shareaza Development Team, 2010.
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

#include "Neighbour.h"
#include "WndChat.h"

class CDCPacket;
class CChatSession;


class CDCNeighbour : public CNeighbour
{
public:
	CDCNeighbour();
	virtual ~CDCNeighbour();

public:
	CString			m_sNick;		// User nick on this hub
	BOOL			m_bNickValid;	// User nick was accepted
	BOOL			m_bExtended;	// Using extended protocol
	CStringList		m_oFeatures;	// Remote client supported features

	BOOL			ConnectToMe(const CString& sNick);		// Send $ConnectToMe command
	void			OnChatOpen(CChatSession* pSession); 	// Chat window was (re)opened
	CChatUser*		GetUser(const CString& sNick) const;	// Find user

	virtual BOOL	ConnectTo(const IN_ADDR* pAddress, WORD nPort, BOOL bAutomatic);
	virtual BOOL	Send(CPacket* pPacket, BOOL bRelease = TRUE, BOOL bBuffered = FALSE);
	virtual DWORD	GetUserCount() const { return m_oUsers.GetCount(); }

protected:
	CChatUser::Map	m_oUsers;		// Hub user list

	void			RemoveAllUsers();

	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnRead();

	BOOL			OnPacket(CDCPacket* pPacket);			// Got DC++ command
	BOOL			OnPing();								// Got ping
	BOOL			OnHello(LPSTR szNick);					// Got $Hello command
	BOOL			OnLock(LPSTR szParams);					// Got $Lock command
	BOOL			OnQuery(CDCPacket* pPacket);			// Got search request
	BOOL			OnChat(CDCPacket* pPacket);				// Got chat message
	BOOL			OnChatPrivate(CDCPacket* pPacket);		// Got private chat message
	BOOL			OnSupports(LPSTR szParams);				// Got $Supports command
	BOOL			OnHubName(CDCPacket* pPacket);			// Got $HubName command
	BOOL			OnHubTopic(CDCPacket* pPacket);			// Got $HubTopic command
	BOOL			OnOpList(LPSTR szParams);				// Got $OpList command
	BOOL			OnUserInfo(LPSTR szInfo);				// Got $MyINFO command
	BOOL			OnUserIP(LPSTR szIP);					// Got $UserIP command
	BOOL			OnQuit(LPSTR szNick);					// Got $Quit command
	BOOL			OnConnectToMe(LPSTR szParams);			// Got $ConnectToMe command
	BOOL			OnRevConnectToMe(LPSTR szParams);		// Got $RevConnectToMe command
	BOOL			OnForceMove(LPSTR szParams);			// Got $ForceMove command
	BOOL			OnValidateDenide();						// Got $ValidateDenide command (not 'denied')
	BOOL			OnGetPass();							// Got $GetPass command
	BOOL			OnZOn();								// Got $ZOn command
	BOOL			OnUnknown(CDCPacket* pPacket);			// Got unknown message

	BOOL			SendUserInfo();							// Send $MyINFO command
};
