//
// CtrlText.h
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

class CTextLine;


class CTextCtrl : public CWnd
{
	DECLARE_DYNCREATE(CTextCtrl)

// Construction
public:
	CTextCtrl();
	virtual ~CTextCtrl();

// Attributes
protected:
	mutable CCriticalSection	m_pSection;

	CArray< CTextLine* > m_pLines;
	int					m_nPosition;
	int					m_nTotal;
	int					m_nHeight;
	COLORREF			m_crBackground[4];
	COLORREF			m_crText[5];
	CFont				m_pFont;
	BOOL				m_bProcess;
	int					m_nLastClicked;			// Index of last clicked item

// Operations
public:
	void	Add(const CLogMessage* pMsg);
	void	AddLine(WORD nType, const CString& strLine);
	void	Clear(BOOL bInvalidate = TRUE);
	void	CopyText() const;

protected:
	void	UpdateScroll(BOOL bFull = FALSE);
	int		HitTest(const CPoint& pt) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CTextCtrl)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTextCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar = NULL);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CTextLine
{
// Construction
public:
	CTextLine(WORD nType, const CString& strText);
	~CTextLine();

// Attributes
public:
	CString	m_sText;
	int*	m_pLine;
	int		m_nLine;
	WORD	m_nType;
	BOOL	m_bSelected;

// Operations
public:
	int		Process(CDC* pDC, int nWidth);
	void	Paint(CDC* pDC, CRect* pRect, BOOL bSkinned = FALSE);
protected:
	void	AddLine(int nLength);
};
