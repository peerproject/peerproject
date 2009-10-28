//
// WndMedia.h
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

#include "WndPanel.h"
#include "CtrlMediaFrame.h"
#include "PeerProjectDataSource.h"

class CMediaWnd : public CPanelWnd
{
// Construction
public:
	CMediaWnd();
	virtual ~CMediaWnd();

	DECLARE_SERIAL(CMediaWnd)

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
