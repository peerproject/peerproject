//
// PageFileComments.h
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

#if !defined(AFX_PAGEFILECOMMENTS_H__F8A2DD9D_0866_4C6D_A2BE_D439E33F19D9__INCLUDED_)
#define AFX_PAGEFILECOMMENTS_H__F8A2DD9D_0866_4C6D_A2BE_D439E33F19D9__INCLUDED_

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEFILECOMMENTS_H__F8A2DD9D_0866_4C6D_A2BE_D439E33F19D9__INCLUDED_)
