//
// RichViewCtrl.h
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

#if !defined(AFX_RICHVIEWCTRL_H__EB7BD9A6_8F32_48F1_ACAA_3503B8A61769__INCLUDED_)
#define AFX_RICHVIEWCTRL_H__EB7BD9A6_8F32_48F1_ACAA_3503B8A61769__INCLUDED_

#pragma once

class CRichDocument;
class CRichElement;
class CRichFragment;

typedef struct
{
	int		nFragment;
	int		nOffset;
} RICHPOSITION;


class CRichViewCtrl : public CWnd
{
// Construction
public:
	CRichViewCtrl();
	virtual ~CRichViewCtrl();

// Attributes
public:
	TCHAR*			m_szSign;
protected:
	CSyncObject*	m_pSyncRoot;
	BOOL			m_bSelectable;
	BOOL			m_bFollowBottom;
	BOOL			m_bDefaultLink;
protected:
	CRichDocument*	m_pDocument;
	DWORD			m_nCookie;
	CArray< CRichFragment* > m_pFragments;
	int				m_nLength;
	int				m_nScrollWheelLines;
protected:
	CRichElement*	m_pHover;
	BOOL			m_bSelecting;
	RICHPOSITION	m_pSelStart;
	RICHPOSITION	m_pSelEnd;
	RICHPOSITION	m_pSelAbsStart;
	RICHPOSITION	m_pSelAbsEnd;
protected:
	HCURSOR			m_hcHand;
	HCURSOR			m_hcText;
	CBrush			m_pBrush;

// Operations
public:
	void			SetSyncObject(CSyncObject* pSyncRoot);
	void			SetSelectable(BOOL bSelectable);
	void			SetFollowBottom(BOOL bFollowBottom);
	void			SetDefaultLink(BOOL bDefaultLink);
	void			SetDocument(CRichDocument* pDocument);
	BOOL			IsModified() const;
	void			InvalidateIfModified();
	int				FullHeightMove(int nX, int nY, int nWidth, BOOL bShow = FALSE);
	BOOL			GetElementRect(CRichElement* pElement, RECT* prc);
protected:
	void			ClearFragments();
	void			Layout(CDC* pDC, CRect* pRect);
	void			WrapLineHelper(CList< CRichFragment* >& pLine, CPoint& pt, int& nLineHeight, int nWidth, int nAlign);
	CRichFragment*	PointToFrag(CPoint& pt);
	RICHPOSITION	PointToPosition(CPoint& pt);
	CPoint			PositionToPoint(RICHPOSITION& pt);
	void			UpdateSelection();
	void			CopySelection();
protected:
	virtual void	OnLayoutComplete() {};
	virtual void	OnPaintBegin(CDC* /*pDC*/) {};
	virtual void	OnPaintComplete(CDC* /*pDC*/) {};
	virtual void	OnVScrolled() {};

// Overrides
public:
	//{{AFX_VIRTUAL(CRichViewCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CRichViewCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CRichFragment;
	friend class CIRCFrame;
};

typedef struct
{
	NMHDR			hdr;
	CRichElement*	pElement;
} RVN_ELEMENTEVENT;

#define RVN_CLICK		100
#define RVN_DBLCLICK	101
#define RVN_SETCURSOR	102

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_RICHVIEWCTRL_H__EB7BD9A6_8F32_48F1_ACAA_3503B8A61769__INCLUDED_)
