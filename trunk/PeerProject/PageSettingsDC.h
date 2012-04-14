//
// PageSettingsDC.h
//
// This file is part of PeerProject (peerproject.org) © 2011-2012
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


class CDCSettingsPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CDCSettingsPage)

// Construction
public:
	CDCSettingsPage();
	virtual ~CDCSettingsPage();

	enum { IDD = IDD_SETTINGS_DC };

// Dialog Data
public:
	//{{AFX_DATA(CDCSettingsPage)
	CButton	m_wndDiscoveryGo;
	CSpinButtonCtrl	m_wndHubsSpin;
	CEdit	m_wndHubs;
	int 	m_nHubs;
	BOOL	m_bEnabled;
	BOOL	m_bEnableAlways;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CDCSettingsPage)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDCSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDiscoveryGo();
	afx_msg void OnEnable();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
