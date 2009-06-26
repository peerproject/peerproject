//
// WndUploads.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#if !defined(AFX_WNDUPLOADS_H__1067A4F9_E488_4037_BA97_04B1C6EB46B4__INCLUDED_)
#define AFX_WNDUPLOADS_H__1067A4F9_E488_4037_BA97_04B1C6EB46B4__INCLUDED_

#pragma once

#include "WndPanel.h"
#include "CtrlUploads.h"


class CUploadsWnd : public CPanelWnd
{
// Construction
public:
	CUploadsWnd();
	virtual ~CUploadsWnd();

	DECLARE_SERIAL(CUploadsWnd)

// Operations
public:
	virtual void	OnSkinChange();
protected:
	inline BOOL		IsSelected(CUploadFile* pFile);
	void			Prepare();

// Attributes
public:
	CUploadsCtrl	m_wndUploads;
	CCoolBarCtrl	m_wndToolBar;
protected:
	DWORD			m_tLastUpdate;
protected:
	DWORD			m_tSel;
	BOOL			m_bSelFile;
	BOOL			m_bSelUpload;
	BOOL			m_bSelActive;
	BOOL			m_bSelQueued;
	BOOL			m_bSelChat;
	BOOL			m_bSelBrowse;
	BOOL			m_bSelSourceAcceptConnections;

// Overrides
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnUpdateUploadsDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnUploadsDisconnect();
	afx_msg void OnUpdateUploadsLaunch(CCmdUI* pCmdUI);
	afx_msg void OnUploadsLaunch();
	afx_msg void OnUpdateUploadsFolder(CCmdUI* pCmdUI);
	afx_msg void OnUploadsFolder();
	afx_msg void OnUpdateUploadsClear(CCmdUI* pCmdUI);
	afx_msg void OnUploadsClear();
	afx_msg void OnUploadsClearCompleted();
	afx_msg void OnUpdateUploadsChat(CCmdUI* pCmdUI);
	afx_msg void OnUploadsChat();
	afx_msg void OnUpdateUploadsAutoClear(CCmdUI* pCmdUI);
	afx_msg void OnUploadsAutoClear();
	afx_msg void OnUpdateSecurityBan(CCmdUI* pCmdUI);
	afx_msg void OnSecurityBan();
	afx_msg void OnUpdateBrowseLaunch(CCmdUI* pCmdUI);
	afx_msg void OnBrowseLaunch();
	afx_msg void OnUpdateUploadsStart(CCmdUI* pCmdUI);
	afx_msg void OnUploadsStart();
	afx_msg void OnUpdateEditQueue(CCmdUI* pCmdUI);
	afx_msg void OnEditQueue();
	afx_msg void OnUploadsHelp();
	afx_msg void OnUploadsSettings();
	afx_msg void OnUpdateUploadsFilterAll(CCmdUI* pCmdUI);
	afx_msg void OnUploadsFilterAll();
	afx_msg void OnUpdateUploadsFilterActive(CCmdUI* pCmdUI);
	afx_msg void OnUploadsFilterActive();
	afx_msg void OnUpdateUploadsFilterQueued(CCmdUI* pCmdUI);
	afx_msg void OnUploadsFilterQueued();
	afx_msg void OnUpdateUploadsFilterHistory(CCmdUI* pCmdUI);
	afx_msg void OnUploadsFilterHistory();
	afx_msg void OnUploadsFilterMenu();
public:
	afx_msg void OnUpdateUploadsFilterTorrent(CCmdUI *pCmdUI);
	afx_msg void OnUploadsFilterTorrent();
};

#define IDC_UPLOADS		100

#endif // !defined(AFX_WNDUPLOADS_H__1067A4F9_E488_4037_BA97_04B1C6EB46B4__INCLUDED_)
