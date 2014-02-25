//
// PageSettingsDC.h
//
// This file is part of PeerProject (peerproject.org) © 2011-2014
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

#include "WndSettingsPage.h"


class CDCSettingsPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CDCSettingsPage)

public:
	CDCSettingsPage();
	virtual ~CDCSettingsPage();

	enum { IDD = IDD_SETTINGS_DC };

public:
	CButton	m_wndDiscoveryGo;
	CSpinButtonCtrl	m_wndHubsSpin;
	CEdit	m_wndHubs;
	int 	m_nHubs;
	BOOL	m_bEnabled;
	BOOL	m_bEnableAlways;

public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDiscoveryGo();
	afx_msg void OnEnable();

	DECLARE_MESSAGE_MAP()
};
