//
// WndChild.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
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

#include "CtrlCoolBar.h"
#include "LiveListSizer.h"

class CMainWnd;
class CWindowManager;
class CQuerySearch;
class CQueryHit;
class CBuffer;
class CConnection;
class CSkinWindow;


class CChildWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildWnd)

public:
	CChildWnd();

	UINT			m_nResID;
	BOOL			m_bTabMode;
	BOOL			m_bGroupMode;
	CChildWnd*		m_pGroupParent;
	float			m_nGroupSize;
	BOOL			m_bPanelMode;
	BOOL			m_bAlert;
	CSkinWindow*	m_pSkin;

private:
	CMainWnd*		m_pMainWndCache;
	static CChildWnd*	m_pCmdMsg;

// Operations
public:
	CMainWnd*		GetMainWnd();
	CWindowManager*	GetManager();
	BOOL			IsActive(BOOL bFocused = FALSE);
	BOOL			IsPartiallyVisible();
	BOOL			TestPoint(const CPoint& ptScreen);
	BOOL			LoadState(LPCTSTR pszName = NULL, BOOL bDefaultMaximise = TRUE);
	BOOL			SaveState(LPCTSTR pszName = NULL);
	BOOL			SetAlert(BOOL bAlert = TRUE);
	void			SizeListAndBar(CWnd* pList, CWnd* pBar);
	void			RemoveSkin();

	virtual void	OnSkinChange();
	virtual void	OnQuerySearch(const CQuerySearch* pSearch);
	virtual BOOL	OnQueryHits(const CQueryHit* pHits);
	virtual void	SanityCheck();
	virtual BOOL	OnPush(const Hashes::Guid& pClientID, CConnection* pConnection);
	virtual HRESULT	GetGenericView(IGenericView** ppView);
	virtual BOOL	PreTranslateMessage(MSG* pMsg);


protected:
	virtual BOOL	Create(UINT nID, BOOL bVisible = TRUE);
	virtual BOOL	OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	//{{AFX_MSG(CChildWnd)
	afx_msg int 	OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void	OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void	OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void	OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void	OnNcPaint();
	afx_msg LRESULT	OnNcHitTest(CPoint point);
	afx_msg BOOL	OnNcActivate(BOOL bActive);
	afx_msg void	OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void	OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void	OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void	OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg BOOL	OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT	OnSetText(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
