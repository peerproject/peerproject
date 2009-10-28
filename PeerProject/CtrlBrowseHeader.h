//
// CtrlBrowseHeader.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "CtrlCoolBar.h"

class CHostBrowser;


class CBrowseHeaderCtrl : public CCoolBarCtrl
{
// Construction
public:
	CBrowseHeaderCtrl();
	virtual ~CBrowseHeaderCtrl();

// Attributes
protected:
	CString	m_sTitle;
	CString	m_sIntro;
	int		m_nIcon32;
	int		m_nIcon48;

// Operations
public:
	void	Update(CHostBrowser* pBrowser);
	void	OnSkinChange();
protected:
	virtual void PrepareRect(CRect* pRect) const;
	virtual void DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent);

// Overrides
public:
	//{{AFX_VIRTUAL(CBrowseHeaderCtrl)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CBrowseHeaderCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#define IDC_BROWSE_HEADER	123
