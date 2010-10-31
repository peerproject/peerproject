//
// PageFileSources.h
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

#include "DlgFilePropertiesPage.h"

class CSharedSource;


class CFileSourcesPage : public CFilePropertiesPage
{
// Construction
public:
	CFileSourcesPage();
	virtual ~CFileSourcesPage();

	DECLARE_DYNCREATE(CFileSourcesPage)

	enum { IDD = IDD_FILE_SOURCES };

// Dialog Data
public:
	//{{AFX_DATA(CFileSourcesPage)
	CButton	m_wndRemove;
	CButton	m_wndNew;
	CListCtrl	m_wndList;
	CString	m_sSource;
	//}}AFX_DATA

private:
	CImageList	m_gdiImageList;

	void AddSource(CSharedSource* pSource);

// Overrides
public:
	//{{AFX_VIRTUAL(CFileSourcesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFileSourcesPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemChangedFileSources(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFileSource();
	afx_msg void OnSourceRemove();
	afx_msg void OnSourceNew();
	afx_msg void OnDblClk(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
