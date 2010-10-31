//
// CtrlPrivateChatFrame.h"
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

#include "CtrlChatFrame.h"
#include "GProfile.h"

class CChatSession;


class CPrivateChatFrame : public CChatFrame
{
// Construction
public:
	CPrivateChatFrame();
	virtual ~CPrivateChatFrame();

	DECLARE_DYNAMIC(CPrivateChatFrame)

// Attributes
public:
	CString		m_sNick;

// Operations
public:
	void	Initiate(const Hashes::Guid& oGUID, SOCKADDR_IN* pHost, BOOL bMustPush);
	BOOL	Accept(CChatSession* pSession);
public:
	virtual void	OnLocalMessage(bool bAction, LPCTSTR pszText);
	virtual void	OnLocalCommand(LPCTSTR pszCommand, LPCTSTR pszArgs);
	virtual void	OnProfileReceived();
	virtual void	OnRemoteMessage(BOOL bAction, LPCTSTR pszText);

// Overrides
public:
	virtual void OnSkinChange();

// Implementation
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateChatBrowse(CCmdUI* pCmdUI);
	afx_msg void OnChatBrowse();
	afx_msg void OnUpdateChatPriority(CCmdUI* pCmdUI);
	afx_msg void OnChatPriority();

	DECLARE_MESSAGE_MAP()
};
