//
// CtrlWndTabBar.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "PeerProjectDataSource.h"

class CChildWnd;


class CWndTabBar : public CControlBar
{
// Construction
public:
	CWndTabBar();
	virtual ~CWndTabBar();

	friend class CWindowManager;

// Item Class
public:
	class TabItem
	{
	// Construction
	public:
		TabItem(CChildWnd* pWnd, DWORD nCookie, LPCTSTR pszCaption);
		virtual ~TabItem();

	// Attributes
	public:
		HWND			m_hWnd;
		CRuntimeClass*	m_pClass;
		int				m_nImage;

		CString			m_sCaption;
		BOOL			m_bVisible;
		BOOL			m_bAlert;
		DWORD			m_nCookie;

	// Operations
	public:
		void		Paint(CWndTabBar* pBar, CDC* pDC, CRect* pRect, BOOL bSelected, BOOL bHot, BOOL bTransparent);
	};

// Attributes
protected:
	CList< TabItem* > m_pItems;
	TabItem*		m_pSelected;
	TabItem*		m_pHot;
	DWORD			m_dwHoverTime;
	DWORD			m_nCookie;
	BOOL			m_bTimer;
	BOOL			m_bMenuGray;
	BOOL			m_bCloseButton;
	CImageList		m_pImages;
	CMap< void*, void*, int, int > m_pIcons;
	int				m_nCloseImage;
	int				m_nMaximumWidth;
	UINT			m_nMessage;
	CString			m_sMessage;
	CRect			m_rcMessage;
	CBitmap			m_bmImage;

// Operations
public:
	void			MoveLeft( TabItem* pItem );
	void			MoveRight( TabItem* pItem );
	void			MoveFirst( TabItem* pItem );
	void			MoveLast( TabItem* pItem );
	void			SetMaximumWidth(int nWidth);
	void			SetMessage(UINT nMessageID);
	void			SetMessage(LPCTSTR pszText);
	void			SetWatermark(HBITMAP hBitmap);
	void			OnSkinChange();
protected:
	TabItem*		HitTest(const CPoint& point, CRect* pItemRect = NULL) const;
	int				ImageIndexForWindow(CWnd* pChild);

// Overrides
public:
	//{{AFX_VIRTUAL(CWndTabBar)
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_BOTTOM, UINT nID = AFX_IDW_STATUS_BAR);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void DoPaint(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWndTabBar)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DROP()

	friend class TabItem;
};
