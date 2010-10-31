//
// CtrlLibraryHeaderPanel.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "MetaList.h"

class CAlbumFolder;


class CLibraryHeaderPanel : public CWnd
{
// Construction
public:
	CLibraryHeaderPanel();
	virtual ~CLibraryHeaderPanel();

// Attributes
protected:
	int			m_nIcon32;
	int			m_nIcon48;
	CString		m_sTitle;
	CString		m_sSubtitle;
	CMetaList	m_pMetadata;
	int			m_nMetaWidth;
	int			m_nKeyWidth;

	CSize		m_szBuffer;
	CDC			m_dcBuffer;
	CBitmap		m_bmBuffer;
	HBITMAP		m_hBuffer;
	CBitmap		m_bmWatermark;

// Operations
public:
	int				Update();
	void			OnSkinChange();
protected:
	CAlbumFolder*	GetSelectedAlbum() const;
	void			DoPaint(CDC* pDC, CRect& rcClient);
	void			DrawText(CDC* pDC, int nX, int nY, LPCTSTR pszText);

// Overrides
public:
	//{{AFX_VIRTUAL(CLibraryHeaderPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLibraryHeaderPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define ID_LIBRARY_HEADER	134
