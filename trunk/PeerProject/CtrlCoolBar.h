//
// CtrlCoolBar.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#if !defined(CTRLCOOLBAR_H)
#define CTRLCOOLBAR_H

#pragma once

#include "PeerProjectDataSource.h"

class CCoolBarItem;


class CCoolBarCtrl : public CControlBar
{
// Construction
public:
	CCoolBarCtrl();
	virtual ~CCoolBarCtrl();

	DECLARE_DYNAMIC(CCoolBarCtrl)

// Attributes
protected:
	CList< CCoolBarItem* >	m_pItems;
	int				m_nHeight;
	BOOL			m_bStretch;
	BOOL			m_bGripper;
	BOOL			m_bBold;
	BOOL			m_bDragForward;
	CBitmap			m_bmImage;
	BOOL			m_bBuffered;
	BOOL			m_bMenuGray;
	CSyncObject*	m_pSyncObject;
	DWORD			m_dwHoverTime;
	CCoolBarItem*	m_pDown;
	CCoolBarItem*	m_pHot;
	BOOL			m_bTimer;
	CSize			m_czLast;
	CBrush			m_brBack;
	COLORREF		m_crBack;
	BOOL			m_bRecalc;
	BOOL			m_bDropEnabled;

// Operations
public:
	inline void		EnableDrop() { m_bDropEnabled = TRUE; }
	void			SetSize(int nHeight, BOOL bStretch);
	void			SetGripper(BOOL bGripper);
	void			SetBold(BOOL bBold);
	void			SetDragForward(BOOL bForward);
	void			SetWatermark(HBITMAP hBitmap, BOOL bDetach = FALSE);
	void			SetSyncObject(CSyncObject* pSyncObject);
	CCoolBarItem*	Add(UINT nID, LPCTSTR pszText = NULL, int nPosition = -1);
	CCoolBarItem*	Add(UINT nCtrlID, int nWidth, int nHeight = 0);
	CCoolBarItem*	GetIndex(int nIndex) const;
	CCoolBarItem*	GetID(UINT nID) const;
	int				GetIndexForID(UINT nID) const;
	INT_PTR			GetCount() const { return m_pItems.GetCount(); }
	BOOL			LoadToolBar(UINT nIDToolBar);
	void			Clear();
	void			Copy(CCoolBarCtrl* pOther);
	BOOL			GetItemRect(CCoolBarItem* pFind, CRect* pRect) const;
	UINT			ThrowMenu(UINT nID, CMenu* pMenu, CWnd* pParent = NULL, BOOL bCommand = FALSE, BOOL bRight = FALSE);
	void			OnUpdated();
protected:
	virtual void	PrepareRect(CRect* pRect) const;
	virtual void	DoPaint(CDC* pDC, CRect& rc, BOOL bTransparent);
	CCoolBarItem*	HitTest(const CPoint& point, CRect* pItemRect = NULL, BOOL bSeparators = FALSE) const;
	void			SmartMove(CWnd* pCtrl, CRect* pRect);

// Overrides
public:
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_TOP, UINT nID = AFX_IDW_TOOLBAR);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual void DoPaint(CDC* pDC);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

// Implementation
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	DECLARE_DROP()

	friend class CCoolBarItem;
};


class CCoolBarItem : public CCmdUI
{
// Construction
public:
	CCoolBarItem(CCoolBarCtrl* pBar, UINT nID = ID_SEPARATOR, int nImage = -1);
	CCoolBarItem(CCoolBarCtrl* pBar, CCoolBarItem* pCopy);
	virtual ~CCoolBarItem();

// Attributes
public:
	BOOL		m_bVisible;
	BOOL		m_bEnabled;
	BOOL		m_bChecked;
public:
	int			m_nImage;
	CString		m_sText;
	CString		m_sTip;
	COLORREF	m_crText;
	UINT		m_nCtrlID;
	int			m_nCtrlHeight;
	BOOL		m_bRegularButton;	// Button with image and text
	BOOL		m_bCheckButton;		// Button with check mark and text
	CBitmap		m_bmButtonmark;
protected:
	CCoolBarCtrl*	m_pBar;
	BOOL			m_bDirty;
	int				m_nWidth;

// Operations
public:
	virtual void Show(BOOL bOn);
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetImage(UINT nCommandID);
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetTip(LPCTSTR pszTip);
	virtual void SetTextColor(COLORREF crText = 0xFFFFFFFF);
protected:
	void	Paint(CDC* pDC, CRect& rc, BOOL bDown, BOOL bHot, BOOL bMenuGray, BOOL bTransparent);
	void	DrawText(CDC* pDC, CRect& rc, BOOL bDown, BOOL bHot, BOOL bMenuGray, BOOL bTransparent);
public:
	void	SetButtonmark(HBITMAP hBitmap);
	static CCoolBarItem* FromCmdUI(CCmdUI* pUI);

	friend class CCoolBarCtrl;
};

#ifndef ID_SEPARATOR
#define ID_SEPARATOR 0
#endif

#ifndef ID_RIGHTALIGN
#define ID_RIGHTALIGN -1
#endif


#endif // !defined(CTRLCOOLBAR_H)
