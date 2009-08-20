//
// PageSettingsProtocols.h
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

#if !defined(AFX_PAGESETTINGSPROTOCOLS_H__A8D2F964_2179_4E0D_BAF2_04ABE55BFA5A__INCLUDED_)
#define AFX_PAGESETTINGSPROTOCOLS_H__A8D2F964_2179_4E0D_BAF2_04ABE55BFA5A__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CProtocolsSettingsPage : public CSettingsPage
{
// Construction
public:
	CProtocolsSettingsPage();
	virtual ~CProtocolsSettingsPage();

	DECLARE_DYNCREATE(CProtocolsSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CProtocolsSettingsPage)
	enum { IDD = IDD_SETTINGS_PROTOCOLS };
	CTreeCtrl	m_wndTree;
	//}}AFX_DATA

	HTREEITEM	AddItem(HTREEITEM hParent, LPCTSTR pszText, LPCTSTR pszValue = NULL);

// Overrides
public:
	//{{AFX_VIRTUAL(CProtocolsSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CProtocolsSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGESETTINGSPROTOCOLS_H__A8D2F964_2179_4E0D_BAF2_04ABE55BFA5A__INCLUDED_)