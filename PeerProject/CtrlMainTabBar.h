//
// CtrlMainTabBar.h
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

#include "PeerProjectDataSource.h"

class CSkinWindow;


class CMainTabBarCtrl : public CControlBar
{
// Construction
public:
	CMainTabBarCtrl();
	virtual ~CMainTabBarCtrl();

	DECLARE_DYNAMIC(CMainTabBarCtrl)

// Item Class
public:
	class TabItem : public CCmdUI
	{
	public:
		CMainTabBarCtrl* m_pCtrl;
		CString	m_sName;
		CString	m_sTitle;
		CRect	m_rc;
		CRect	m_rcSrc[5];
		BOOL	m_bEnabled;
		BOOL	m_bSelected;

	public:
		TabItem(CMainTabBarCtrl* pCtrl, LPCTSTR pszName);
		virtual ~TabItem() {}
		void	OnSkinChange(CSkinWindow* pSkin, CDC* pdcCache, CBitmap* pbmCache);
		BOOL	Update(CFrameWnd* pTarget);
		BOOL	HitTest(const CPoint& point) const;
		void	Paint(CDC* pDstDC, CDC* pSrcDC, const CPoint& ptOffset, BOOL bHover, BOOL bDown);
	public:
		virtual void	Enable(BOOL bEnable);
		virtual void	SetCheck(BOOL bCheck);
	};

// Attributes
protected:
	CList< TabItem* > m_pItems;
	CSkinWindow*	m_pSkin;
	TabItem*		m_pHover;
	TabItem*		m_pDown;
	DWORD			m_dwHoverTime;
	CDC				m_dcSkin;
	CBitmap			m_bmSkin;
	HBITMAP			m_hOldSkin;

// Operations
public:
	BOOL			Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID);
	BOOL			HasLocalVersion();
	void			OnSkinChange();
	virtual void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual CSize	CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	TabItem*		HitTest(const CPoint& point) const;
	virtual INT_PTR	OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual void	DoPaint(CDC* pDC);

// Implementation
protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
	DECLARE_DROP()
};
