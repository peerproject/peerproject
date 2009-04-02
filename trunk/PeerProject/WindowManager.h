//
// WindowManager.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
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

#include "WndChild.h"


class CWindowManager : public CWnd
{
	DECLARE_DYNCREATE(CWindowManager)

public:
	CWindowManager(CMDIFrameWnd* pParent = NULL);
	virtual ~CWindowManager();

	CMDIFrameWnd*		m_pParent;
	CList< CChildWnd* >	m_pWindows;
	CRect				m_rcSize;
	BOOL				m_bIgnoreActivate;

	void		SetOwner(CMDIFrameWnd* pParent);
	CChildWnd*	GetActive() const;
	POSITION	GetIterator() const;
	CChildWnd*	GetNext(POSITION& pos) const;
	BOOL		Check(CChildWnd* pChild) const;
	CChildWnd*	Find(CRuntimeClass* pClass, CChildWnd* pAfter = NULL, CChildWnd* pExcept = NULL);
	CChildWnd*	Open(CRuntimeClass* pClass, BOOL bToggle = FALSE, BOOL bFocus = TRUE);
	CChildWnd*	FindFromPoint(const CPoint& point) const;
	void		Close();
	void		AutoResize();
	void		Cascade(BOOL bActiveOnly = FALSE);
	void		SetGUIMode(int nMode, BOOL bSaveState = TRUE);
	void		LoadWindowStates();
	void		SaveWindowStates();
	BOOL		LoadSearchWindows();
	void		SaveSearchWindows();
	BOOL		LoadBrowseHostWindows();
	void		SaveBrowseHostWindows();
	void		OpenNewSearchWindow();
	void		PostSkinChange();
	void		PostSkinRemove();

protected:
	void		Add(CChildWnd* pChild);
	void		Remove(CChildWnd* pChild);
	void		ActivateGrouped(CChildWnd* pExcept);
	void		CreateTabbedWindows();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

	friend class CChildWnd;
	friend class CPluginWnd;
};
