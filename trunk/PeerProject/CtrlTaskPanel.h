//
// CtrlTaskPanel.h
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

#if !defined(AFX_CTRLTASKPANEL_H__509DBC3E_C56B_4A11_993A_3F227C6D9621__INCLUDED_)
#define AFX_CTRLTASKPANEL_H__509DBC3E_C56B_4A11_993A_3F227C6D9621__INCLUDED_

#pragma once

class CTaskBox;


class CTaskPanel : public CWnd
{
// Construction
public:
	CTaskPanel();

	DECLARE_DYNAMIC(CTaskPanel)

// Attributes
protected:
	CList< CTaskBox* >	m_pBoxes;
	CTaskBox*	m_pStretch;
	int			m_nMargin;
	int			m_nCurve;
	CBitmap		m_bmWatermark;
	CBitmap		m_bmFooter;
	BOOL		m_bLayout;

// Operations
public:
	CTaskBox*	AddBox(CTaskBox* pBox, POSITION posBefore = NULL);
	POSITION	GetBoxIterator() const;
	CTaskBox*	GetNextBox(POSITION& pos) const;
	INT_PTR		GetBoxCount() const;
	void		RemoveBox(CTaskBox* pBox);
	void		ClearBoxes(BOOL bDelete);
public:
	void		SetStretchBox(CTaskBox* pBox);
	void		SetMargin(int nMargin, int nCurve = 2);
	void		SetWatermark(HBITMAP hBitmap);
	void		SetFooter(HBITMAP hBitmap, BOOL bDefault = TRUE);
	void		OnChanged();
protected:
	void		Layout(CRect& rcClient);

// Overrides
public:
	//{{AFX_VIRTUAL(CTaskPanel)
	public:
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTaskPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CTaskBox;
};


class CTaskBox : public CButton
{
public:
	CTaskBox();
	virtual ~CTaskBox();

	DECLARE_DYNAMIC(CTaskBox)
	
	CTaskPanel*	GetPanel() const;
	void		SetCaption(LPCTSTR pszCaption);
	void		SetIcon(HICON hIcon);
	void		SetSize(int nHeight);
	void		SetPrimary(BOOL bPrimary = TRUE);
	void		SetWatermark(HBITMAP hBitmap);
	void		SetCaptionmark(HBITMAP hBitmap, BOOL bDefault = TRUE);
	void		Expand(BOOL bOpen = TRUE);

protected:
	CTaskPanel*	m_pPanel;
	int			m_nHeight;
	BOOL		m_bVisible;
	BOOL		m_bOpen;
	BOOL		m_bHover;
	BOOL		m_bPrimary;
	HICON		m_hIcon;
	BOOL		m_bIconDel;
	CBitmap		m_bmWatermark;
	CBitmap		m_bmCaptionmark;
	BOOL		m_bCaptionCurve;

	int			GetOuterHeight() const;
	void		PaintBorders();
	void		InvalidateNonclient();
	virtual void OnExpanded(BOOL bOpen);

public:
	virtual BOOL Create(CTaskPanel* pPanel, int nHeight = 0, LPCTSTR pszCaption = NULL, UINT nIDIcon = 0, UINT nID = 0);
	virtual void DrawItem(LPDRAWITEMSTRUCT) {}

protected:
	//{{AFX_MSG(CTaskBox)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg ONNCHITTESTRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CTaskPanel;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CTRLTASKPANEL_H__509DBC3E_C56B_4A11_993A_3F227C6D9621__INCLUDED_)
