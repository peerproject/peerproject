//
// PageSettingsRich.h
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

#include "WndSettingsPage.h"
#include "RichViewCtrl.h"


class CRichSettingsPage : public CSettingsPage
{
// Construction
public:
	CRichSettingsPage(LPCTSTR pszName = NULL);
	virtual ~CRichSettingsPage();

	DECLARE_DYNCREATE(CRichSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CRichSettingsPage)
	enum { IDD = IDD_SETTINGS_RICH };
	//}}AFX_DATA

// Attributes
public:
	CString			m_sName;
	CString			m_sCaption;
	CRichViewCtrl	m_wndView;
	CRichDocument*	m_pDocument;

// Overrides
public:
	//{{AFX_VIRTUAL(CRichSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CRichSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnClickView(NMHDR* pNotify, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

#define IDC_RICH_VIEW	100
