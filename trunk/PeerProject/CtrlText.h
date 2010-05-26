//
// CtrlText.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

class CTextLine;

class CTextCtrl : public CWnd
{
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
	CSize				m_cCharacter;
	CFont				m_pFont;
	COLORREF			m_crBackground[4];
	COLORREF			m_crText[5];
	BOOL				m_bProcess;
	int					m_nLastClicked;			// Index of last clicked item

// Operations
public:
	void	Add(WORD nType, const CString& strText);
	void	AddLine(WORD nType, const CString& strLine);
	void	Clear(BOOL bInvalidate = TRUE);
	void	CopyText() const;
	CFont*	GetFont();

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
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CTextLine
{
// Construction
public:
	CTextLine(WORD nType, const CString& strText);
	virtual ~CTextLine();

// Attributes
public:
	CString	m_sText;
	int*	m_pLine;
	int		m_nLine;
	WORD	m_nType;
	BOOL	m_bSelected;

// Operations
public:
	int		Process(int nWidth);
	void	Paint(CDC* pDC, CRect* pRect, BOOL bSkinned = FALSE);
protected:
	void	AddLine(int nLength);
};
