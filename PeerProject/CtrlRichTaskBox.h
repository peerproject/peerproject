//
// CtrlRichTaskBox.h
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

#include "CtrlTaskPanel.h"
#include "RichViewCtrl.h"

class CRichTaskBox : public CTaskBox
{
// Construction
public:
	CRichTaskBox();
	virtual ~CRichTaskBox();

	DECLARE_DYNAMIC(CRichTaskBox)

// Attributes
protected:
	CRichViewCtrl	m_wndView;
	int				m_nWidth;
	CRichDocument*	m_pDocument;

// Operations
public:
	inline CRichViewCtrl& GetView() const { return (CRichViewCtrl&)m_wndView; }
public:
	void	SetDocument(CRichDocument* pDocument);
	void	Update();

// Overrides
public:
	//{{AFX_VIRTUAL(CRichTaskBox)
	public:
	virtual BOOL Create(CTaskPanel* pPanel, LPCTSTR pszCaption = NULL, UINT nIDIcon = 0);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CRichTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
