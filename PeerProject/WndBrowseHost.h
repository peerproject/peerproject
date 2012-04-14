//
// WndBrowseHost.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2006.
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

#include "WndBaseMatch.h"
#include "CtrlBrowseHeader.h"
#include "CtrlBrowseProfile.h"
#include "CtrlBrowseFrame.h"

class CHostBrowser;
class CG2Packet;

#define BROWSER_SER_VERSION 2	// 1000		// ToDo: INTERNAL_VERSION ?
// History:
// 2 - Added CHostBrowser::m_sNick (ryo-oh-ki) (Shareaza 2.5.4.0)

class CBrowseHostWnd : public CBaseMatchWnd
{
	DECLARE_DYNCREATE(CBrowseHostWnd)

// Construction
public:
	CBrowseHostWnd(PROTOCOLID nProtocol = PROTOCOL_ANY, SOCKADDR_IN* pHost = NULL, BOOL bMustPush = FALSE, const Hashes::Guid& pClientID = Hashes::Guid(), const CString& sNick = CString());
	virtual ~CBrowseHostWnd();

public:
	inline CHostBrowser* GetBrowser() const { return m_pBrowser; }

// Attributes
protected:
	CHostBrowser*		m_pBrowser;
	CBrowseHeaderCtrl	m_wndHeader;
	CBrowseProfileCtrl	m_wndProfile;
	CBrowseFrameCtrl	m_wndFrame;
	BOOL				m_bOnFiles;
	BOOL				m_bAutoBrowse;

// Operations
public:
	void			Serialize(CArchive& ar, int nVersion = BROWSER_SER_VERSION);
	virtual void	OnSkinChange();
	virtual void	OnProfileReceived();
	virtual BOOL	OnQueryHits(const CQueryHit* pHits);
	virtual void	OnHeadPacket(CG2Packet* pPacket);
	virtual void	OnPhysicalTree(CG2Packet* pPacket);
	virtual void	OnVirtualTree(CG2Packet* pPacket);
	virtual BOOL	OnPush(const Hashes::Guid& pClientID, CConnection* pConnection);
	virtual BOOL	OnNewFile(CLibraryFile* pFile);
	virtual void	UpdateMessages(BOOL bActive = TRUE);

	virtual BOOL	OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateBrowseHostStop(CCmdUI* pCmdUI);
	afx_msg void OnBrowseHostStop();
	afx_msg void OnBrowseHostRefresh();
	afx_msg void OnUpdateBrowseProfile(CCmdUI* pCmdUI);
	afx_msg void OnBrowseProfile();
	afx_msg void OnUpdateBrowseFiles(CCmdUI* pCmdUI);
	afx_msg void OnBrowseFiles();
	afx_msg void OnUpdateSearchChat(CCmdUI* pCmdUI);
	afx_msg void OnSearchChat();
	afx_msg void OnSelChangeMatches();

	DECLARE_MESSAGE_MAP()
};
