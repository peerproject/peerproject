//
// CtrlLibraryThumbView.h
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

#if !defined(AFX_CTRLLIBRARYTHUMBVIEW_H__700A82CF_4E9C_4BE0_9EBA_DF2E49AC378F__INCLUDED_)
#define AFX_CTRLLIBRARYTHUMBVIEW_H__700A82CF_4E9C_4BE0_9EBA_DF2E49AC378F__INCLUDED_

#pragma once

#include "CtrlLibraryFileView.h"

class CLibraryThumbItem;
class CLibraryFile;


class CLibraryThumbView :
	public CLibraryFileView,
	public CThreadImpl
{
// Construction
public:
	CLibraryThumbView();

	DECLARE_DYNCREATE(CLibraryThumbView)

// Attributes
protected:
	CCriticalSection	m_pSection;
	DWORD				m_nInvalidate;
	int					m_nColumns;
	int					m_nRows;
	CLibraryThumbItem**	m_pList;
	int					m_nCount;
	int					m_nBuffer;
	int					m_nScroll;
	int					m_nSelected;
	CLibraryThumbItem*	m_pFocus;
	CLibraryThumbItem*	m_pFirst;
	CList< CLibraryThumbItem* > m_pSelThumb;
	BOOL				m_bDrag;
	CPoint				m_ptDrag;

// Operations
public:
	virtual void		Update();
	virtual BOOL		Select(DWORD nObject);
	virtual DWORD_PTR	HitTestIndex(const CPoint& point) const;
	virtual HBITMAP		CreateDragImage(const CPoint& ptMouse, CPoint& ptMiddle);
protected:
	void				Clear();
	int					GetThumbIndex(CLibraryThumbItem* pThumb) const;
	BOOL				Select(CLibraryThumbItem* pThumb, TRISTATE bSelect = TRI_TRUE);
	BOOL				DeselectAll(CLibraryThumbItem* pThumb = NULL);
	BOOL				SelectTo(CLibraryThumbItem* pThumb);
	void				SelectTo(int nDelta);
protected:
	void				UpdateScroll();
	void				ScrollBy(int nDelta);
	void				ScrollTo(int nDelta);
	CLibraryThumbItem*	HitTest(const CPoint& point) const;
	BOOL				GetItemRect(CLibraryThumbItem* pThumb, CRect* pRect);
protected:
	void		StartThread();
	void		StopThread();
	void		OnRun();

protected:
	static int	SortList(LPCVOID pA, LPCVOID pB);

// Overrides
public:
	//{{AFX_VIRTUAL(CLibraryThumbView)
	protected:
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLibraryThumbView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CLibraryThumbItem
{
// Construction
public:
	CLibraryThumbItem(CLibraryFile* pFile);
	virtual ~CLibraryThumbItem();

// Attributes
public:
	DWORD	m_nIndex;
	DWORD	m_nCookie;
	CString	m_sText;
	BOOL	m_bShared;
public:
	BOOL	m_bSelected;
	int		m_nThumb;
	CBitmap	m_bmThumb;
	int		m_nShell;

	enum { thumbWaiting, thumbValid, thumbError };

// Operations
public:
	BOOL	Update(CLibraryFile* pFile);
	void	Paint(CDC* pDC, const CRect& rcBlock);

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CTRLLIBRARYTHUMBVIEW_H__700A82CF_4E9C_4BE0_9EBA_DF2E49AC378F__INCLUDED_)
