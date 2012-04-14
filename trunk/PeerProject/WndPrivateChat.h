//
// WndPrivateChat.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2011.
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

#include "WndChat.h"

class CChatSession;


class CPrivateChatWnd : public CChatWnd
{
	DECLARE_DYNAMIC(CPrivateChatWnd)

public:
	CPrivateChatWnd();
	virtual ~CPrivateChatWnd();

public:
	void	Setup(LPCTSTR szNick);
	void	Setup(const Hashes::Guid& oGUID, const SOCKADDR_IN* pHost, BOOL bMustPush, PROTOCOLID nProtocol);
	BOOL	Accept(CChatSession* pSession);
	BOOL	Find(const SOCKADDR_IN* pAddress) const;
	BOOL	Find(const Hashes::Guid& oGUID, bool bLive) const;
	BOOL	Find(const CString& sNick) const;

protected:
	CChatSession*	m_pSession;
	CString			m_sNick;

	virtual CString GetChatID() const;
	virtual CString GetCaption() const;
	virtual BOOL OnLocalMessage(bool bAction, const CString& sText);
	virtual BOOL OnLocalCommand(const CString& sCommand, const CString& sArgs);

	afx_msg void OnDestroy();
	afx_msg void OnUpdateChatConnect(CCmdUI* pCmdUI);
	afx_msg void OnChatConnect();
	afx_msg void OnUpdateChatDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnChatDisconnect();
	afx_msg void OnUpdateChatBrowse(CCmdUI* pCmdUI);
	afx_msg void OnChatBrowse();
	afx_msg void OnUpdateChatPriority(CCmdUI* pCmdUI);
	afx_msg void OnChatPriority();

	DECLARE_MESSAGE_MAP()
};
