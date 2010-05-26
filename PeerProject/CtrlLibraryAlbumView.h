//
// CtrlLibraryAlbumView.h
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

#include "CtrlLibraryFileView.h"

class CLibraryFile;
class CLibraryAlbumTrack;


class CLibraryAlbumView : public CLibraryFileView
{
	DECLARE_DYNCREATE(CLibraryAlbumView)

// Construction
public:
	CLibraryAlbumView();

// Attributes
public:
	static COLORREF			m_crRows[2];

protected:
	CSize					m_szTrack;
	int						m_nRows;
	LPCTSTR					m_pStyle;
	static LPCTSTR			m_pStaticStyle;

	CLibraryAlbumTrack**	m_pList;
	CImageList				m_pStars;
	int						m_nCount;
	int						m_nBuffer;
	int						m_nScroll;

	int						m_nSelected;
	CList< CLibraryAlbumTrack* > m_pSelTrack;
	CLibraryAlbumTrack*		m_pFocus;
	CLibraryAlbumTrack*		m_pFirst;
	CLibraryAlbumTrack*		m_pRating;
	BOOL					m_bDrag;
	CPoint					m_ptDrag;

// Operations
public:
	virtual void		Update();
	virtual void		SelectAll();
	virtual BOOL		Select(DWORD nObject);
	virtual DWORD_PTR 	HitTestIndex(const CPoint& point) const;
	virtual HBITMAP		CreateDragImage(const CPoint& ptMouse, CPoint& ptMiddle);

protected:
	void				Clear();
	BOOL				Select(CLibraryAlbumTrack* pItem, TRISTATE bSelect = TRI_TRUE);
	BOOL				DeselectAll(CLibraryAlbumTrack* pItem = NULL);
	BOOL				SelectTo(CLibraryAlbumTrack* pItem);
	void				SelectTo(int nDelta);

	void				UpdateScroll();
	void				ScrollBy(int nDelta);
	void				ScrollTo(int nDelta);
	CLibraryAlbumTrack*	HitTest(const CPoint& point, CRect* pRect = NULL) const;
	int					GetTrackIndex(CLibraryAlbumTrack* pTrack) const;
	BOOL				GetItemRect(CLibraryAlbumTrack* pTrack, CRect* pRect);

	static int			SortList(LPCVOID pA, LPCVOID pB);

// Overrides
protected:
	//{{AFX_VIRTUAL(CLibraryAlbumView)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLibraryAlbumView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar = NULL);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CLibraryAlbumTrack;
};

class CLibraryAlbumTrack
{
// Construction
public:
	CLibraryAlbumTrack(CLibraryFile* pFile);
	virtual ~CLibraryAlbumTrack();

// Attributes
public:
	DWORD	m_nIndex;
	DWORD	m_nCookie;
	BOOL	m_bShared;
	BOOL	m_bSelected;

	int		m_nShell;
	int		m_nTrack;
	CString	m_sTrack;
	CString	m_sTitle;
	CString	m_sArtist;
	CString	m_sAlbum;
	CString	m_sLength;
	int		m_nLength;
	CString	m_sBitrate;
	int		m_nBitrate;
	int		m_nRating;
	BOOL	m_bComments;
	int		m_nSetRating;

// Operations
public:
	BOOL	Update(CLibraryFile* pFile);
	void	Paint(CLibraryAlbumView* pView, CDC* pDC, const CRect& rcBlock, int nCount);
	BOOL	HitTestRating(const CRect& rcBlock, const CPoint& point);
	BOOL	LockRating();
	static void	PaintText(CDC* pDC, const CRect& rcTrack, int nFrom, int nTo, const CString* pstr, BOOL bCenter = FALSE);
	static void	PaintText(CDC* pDC, const CRect& rcTrack, int nFrom, int nTo, int nID, BOOL bCenter = FALSE);
};
