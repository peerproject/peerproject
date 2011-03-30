//
// PageSettingsWeb.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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

#include "WndSettingsPage.h"

// ToDo: Add Settings.Web.Foxy option ?

class CWebSettingsPage : public CSettingsPage
{
// Construction
public:
	CWebSettingsPage();
	virtual ~CWebSettingsPage();

	DECLARE_DYNCREATE(CWebSettingsPage)

	enum { IDD = IDD_SETTINGS_WEB };

// Dialog Data
public:
	//{{AFX_DATA(CWebSettingsPage)
	CButton	m_wndExtRemove;
	CButton	m_wndExtAdd;
	CComboBox	m_wndExtensions;
	BOOL	m_bWebHook;
	BOOL	m_bUriMagnet;
	BOOL	m_bUriGnutella;
	BOOL	m_bUriED2K;
	BOOL	m_bUriDC;
	BOOL	m_bUriPiolet;
	BOOL	m_bUriTorrent;
//	BOOL	m_bUriMetalink;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CWebSettingsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWebSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditChangeExtList();
	afx_msg void OnSelChangeExtList();
	afx_msg void OnExtAdd();
	afx_msg void OnExtRemove();
	afx_msg void OnWebHook();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
