//
// PageFileComments.h
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

#include "DlgFilePropertiesPage.h"


class CFileCommentsPage : public CFilePropertiesPage
{
	DECLARE_DYNCREATE(CFileCommentsPage)

// Construction
public:
	CFileCommentsPage();
	virtual ~CFileCommentsPage();

	enum { IDD = IDD_FILE_COMMENTS };

// Dialog Data
public:
	//{{AFX_DATA(CFileCommentsPage)
	CEdit	m_wndComments;
	CString	m_sComments;
	CComboBox	m_wndRating;
	int		m_nRating;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CFileCommentsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFileCommentsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
