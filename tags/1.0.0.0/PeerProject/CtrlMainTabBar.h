//
// CtrlMainTabBar.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "PeerProjectDataSource.h"

class CSkinWindow;


class CMainTabBarCtrl : public CControlBar
{
	DECLARE_DYNAMIC(CMainTabBarCtrl)

public:
	CMainTabBarCtrl();
	virtual ~CMainTabBarCtrl();

// Item Class
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

		TabItem(CMainTabBarCtrl* pCtrl, LPCTSTR pszName);
		virtual ~TabItem() {}

		BOOL	Update(CFrameWnd* pTarget);
		BOOL	HitTest(const CPoint& point) const;
		void	Paint(CDC* pDstDC, CDC* pSrcDC, const CPoint& ptOffset, BOOL bHover, BOOL bDown);
		void	OnSkinChange(CSkinWindow* pSkin, CDC* pdcCache, CBitmap* pbmCache);

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
	virtual void	DoPaint(CDC* pDC);
	virtual void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual CSize	CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual INT_PTR	OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	TabItem*		HitTest(const CPoint& point) const;

	inline void		RemoveSkin()
	{
		m_pSkin = NULL;
	}

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
