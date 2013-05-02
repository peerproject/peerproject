//
// CtrlCoolMenuBar.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#include "CtrlCoolBar.h"


class CCoolMenuBarCtrl : public CCoolBarCtrl
{
// Construction
public:
	CCoolMenuBarCtrl();
	virtual ~CCoolMenuBarCtrl();

// Attributes
protected:
	CPoint			m_pMouse;
	HMENU			m_hMenu;
	CCoolBarItem*	m_pSelect;

// Operations
public:
	void	SetMenu(HMENU hMenu);
	void	OpenMenuBar();
	BOOL	OpenMenuChar(UINT nChar);
	void	OnSkinChange();
protected:
	void	ShowMenu();
	void	UpdateWindowMenu(CMenu* pMenu);
	void	ShiftMenu(int nOffset);
	BOOL	OnMenuMessage(MSG* pMsg);

// Overrides
public:
	//{{AFX_VIRTUAL(CCoolMenuBarCtrl)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL

// Statics
protected:
	static LRESULT CALLBACK MenuFilter(int nCode, WPARAM wParam, LPARAM lParam);
	static CCoolMenuBarCtrl*	m_pMenuBar;
	static HHOOK				m_hMsgHook;

// Implementation
protected:
	//{{AFX_MSG(CCoolMenuBarCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
