//
// WndMedia.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "WndPanel.h"
#include "CtrlMediaFrame.h"
#include "PeerProjectDataSource.h"

class CMediaWnd : public CPanelWnd
{
	DECLARE_SERIAL(CMediaWnd)

// Construction
public:
	CMediaWnd();
	virtual ~CMediaWnd();

// Attributes
protected:
	CMediaFrame	m_wndFrame;

// Operations
public:
	virtual void OnSkinChange();
	virtual BOOL PlayFile(LPCTSTR pszFile);
	virtual BOOL EnqueueFile(LPCTSTR pszFile);
	virtual BOOL IsPlaying();
	virtual void OnFileDelete(LPCTSTR pszFile);

// Overrides
public:
	//{{AFX_VIRTUAL(CMediaWnd)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMediaWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	//}}AFX_MSG

	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMediaKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDevModeChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnqueueFile(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlayFile(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	DECLARE_DROP()
};
