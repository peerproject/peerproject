//
// CtrlRichTaskBox.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "CtrlTaskPanel.h"
#include "RichViewCtrl.h"

class CRichTaskBox : public CTaskBox
{
	DECLARE_DYNAMIC(CRichTaskBox)

// Construction
public:
	CRichTaskBox();
	virtual ~CRichTaskBox();

// Attributes
protected:
	CRichViewCtrl	m_wndView;
	CRichDocument*	m_pDocument;
	int				m_nWidth;

// Operations
public:
	inline CRichViewCtrl& GetView() const { return (CRichViewCtrl&)m_wndView; }
public:
	void	SetDocument(CRichDocument* pDocument);
	void	Update();

// Overrides
public:
	//{{AFX_VIRTUAL(CRichTaskBox)
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
