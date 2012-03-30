//
// PageFileComments.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "DlgFilePropertiesPage.h"


class CFileCommentsPage : public CFilePropertiesPage
{
// Construction
public:
	CFileCommentsPage();
	virtual ~CFileCommentsPage();

	DECLARE_DYNCREATE(CFileCommentsPage)

// Dialog Data
public:
	//{{AFX_DATA(CFileCommentsPage)
	enum { IDD = IDD_FILE_COMMENTS };
	CEdit	m_wndComments;
	CComboBox	m_wndRating;
	CString	m_sComments;
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