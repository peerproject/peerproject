//
// PageFileSources.h
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

#if !defined(AFX_PAGEFILESOURCES_H__DD454E2A_A0C8_41F2_9A15_B5DA558667CF__INCLUDED_)
#define AFX_PAGEFILESOURCES_H__DD454E2A_A0C8_41F2_9A15_B5DA558667CF__INCLUDED_

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

// Dialog Data
public:
	//{{AFX_DATA(CFileSourcesPage)
	enum { IDD = IDD_FILE_SOURCES };
	CButton	m_wndRemove;
	CButton	m_wndNew;
	CListCtrl	m_wndList;
	CString	m_sSource;
	//}}AFX_DATA

private:
	CImageList	m_gdiImageList;
private:
	void AddSource(CSharedSource* pSource);

// Overrides
public:
	//{{AFX_VIRTUAL(CFileSourcesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEFILESOURCES_H__DD454E2A_A0C8_41F2_9A15_B5DA558667CF__INCLUDED_)
