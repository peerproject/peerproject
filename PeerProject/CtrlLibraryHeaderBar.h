//
// CtrlLibraryHeaderBar.h
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

#include "CtrlCoolBar.h"

class CLibraryView;
class CCoolMenu;


class CLibraryHeaderBar : public CCoolBarCtrl
{
// Construction
public:
	CLibraryHeaderBar();
	virtual ~CLibraryHeaderBar();

	DECLARE_DYNAMIC(CLibraryHeaderBar)

// Attributes
protected:
	CLibraryView*	m_pLastView;
	int				m_nImage;
	CString			m_sTitle;

	CCoolMenu*		m_pCoolMenu;

// Operations
public:
	void	Update(CLibraryView* pView);
protected:
	void	PaintHeader(CDC* pDC, CRect& rcBar, BOOL bTransparent);

// Overrides
protected:
	//{{AFX_VIRTUAL(CLibraryHeaderBar)
	virtual void PrepareRect(CRect* pRect) const;
	virtual void DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLibraryHeaderBar)
	afx_msg void OnLibraryView();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
