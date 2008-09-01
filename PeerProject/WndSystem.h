//
// WndSystem.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#if !defined(AFX_WNDSYSTEM_H__15622BB7_9266_465C_8902_0F494E1DCAA0__INCLUDED_)
#define AFX_WNDSYSTEM_H__15622BB7_9266_465C_8902_0F494E1DCAA0__INCLUDED_

#pragma once

#include "WndPanel.h"
#include "CtrlText.h"


class CSystemWnd : public CPanelWnd
{
// Construction
public:
	CSystemWnd();

	DECLARE_SERIAL(CSystemWnd)

// Attributes
protected:
	CTextCtrl	m_wndText;

// Operations
public:
	void		Add(int nType, const CString& strText);
	void		Clear();
	void		ShowStartupText();
	void		OnSkinChange();

// Overrides
public:
	//{{AFX_VIRTUAL(CSystemWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSystemWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSystemClear();
	afx_msg void OnSystemCopy();
	afx_msg void OnDestroy();
	afx_msg void OnUpdateSystemVerboseError(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseError();
	afx_msg void OnUpdateSystemVerboseWarning(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseWarning();
	afx_msg void OnUpdateSystemVerboseNotice(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseNotice();
	afx_msg void OnUpdateSystemVerboseInfo(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseInfo();
	afx_msg void OnUpdateSystemVerboseDebug(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseDebug();
	afx_msg void OnUpdateSystemTimestamp(CCmdUI* pCmdUI);
	afx_msg void OnSystemTimestamp();
	afx_msg void OnSystemTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_WNDSYSTEM_H__15622BB7_9266_465C_8902_0F494E1DCAA0__INCLUDED_)
