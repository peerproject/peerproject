//
// WindowManager.h
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

#include "WndChild.h"


class CWindowManager : public CWnd
{
	DECLARE_DYNCREATE(CWindowManager)

public:
	CWindowManager(CMDIFrameWnd* pParent = NULL);
	virtual ~CWindowManager();

public:
	CMDIFrameWnd*		m_pParent;
	CList< CChildWnd* >	m_pWindows;
	CRect				m_rcSize;
	BOOL				m_bIgnoreActivate;

	void		SetOwner(CMDIFrameWnd* pParent);
	CChildWnd*	GetActive() const;
	POSITION	GetIterator() const;
	CChildWnd*	GetNext(POSITION& pos) const;
	CChildWnd*	Open(CRuntimeClass* pClass, BOOL bToggle = FALSE, BOOL bFocus = TRUE);
	CChildWnd*	Find(CRuntimeClass* pClass, CChildWnd* pAfter = NULL, CChildWnd* pExcept = NULL);
	CChildWnd*	FindFromPoint(const CPoint& point) const;
	BOOL		Check(CChildWnd* pChild) const;
	BOOL		IsEmpty() const { return m_pWindows.IsEmpty(); }
	void		Close();
	void		AutoResize();
	void		Cascade(BOOL bActiveOnly = FALSE);
	void		SetGUIMode(int nMode, BOOL bSaveState = TRUE);
	void		LoadWindowStates();
	void		SaveWindowStates() const;
	BOOL		LoadSearchWindows();
	BOOL		SaveSearchWindows() const;
	BOOL		LoadBrowseHostWindows();
	BOOL		SaveBrowseHostWindows() const;
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
