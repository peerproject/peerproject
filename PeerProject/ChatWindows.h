//
// ChatWindows.h
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

class CChatFrame;
class CPrivateChatFrame;


class CChatWindows
{
public:
	CChatWindows();
	virtual ~CChatWindows();

// Attributes
protected:
	CList< CChatFrame* > m_pList;

// Operations
public:
	POSITION	GetIterator() const;
	CChatFrame*	GetNext(POSITION& pos) const;
	INT_PTR		GetCount() const { return m_pList.GetCount(); }
	void		Close();
public:
	CPrivateChatFrame*	OpenPrivate(const Hashes::Guid& oGUID, SOCKADDR_IN* pHost, BOOL bMustPush = FALSE, PROTOCOLID nProtocol = PROTOCOL_NULL, SOCKADDR_IN* pServer = NULL );
	CPrivateChatFrame*	OpenPrivate(const Hashes::Guid& oGUID, IN_ADDR* pAddress, WORD nPort = 6346, BOOL bMustPush = FALSE, PROTOCOLID nProtocol = PROTOCOL_NULL, IN_ADDR* pServerAddress = NULL, WORD nServerPort = 0 );
	CPrivateChatFrame*	FindPrivate(const Hashes::Guid& oGUID);
	CPrivateChatFrame*	FindPrivate(IN_ADDR* pAddress);
	CPrivateChatFrame*  FindED2KFrame(SOCKADDR_IN* pAddress);
	CPrivateChatFrame*  FindED2KFrame(DWORD nClientID, SOCKADDR_IN* pServerAddress);

	void	Add(CChatFrame* pFrame);
	void	Remove(CChatFrame* pFrame);

protected:
	friend class CChatFrame;
};

extern CChatWindows ChatWindows;
