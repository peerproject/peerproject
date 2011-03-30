//
// PageSettingsDonkey.h
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


class CDonkeySettingsPage : public CSettingsPage
{
// Construction
public:
	CDonkeySettingsPage();
	virtual ~CDonkeySettingsPage();

	DECLARE_DYNCREATE(CDonkeySettingsPage)

	enum { IDD = IDD_SETTINGS_DONKEY };

// Dialog Data
public:
	//{{AFX_DATA(CDonkeySettingsPage)
	CSpinButtonCtrl	m_wndLinksSpin;
	CSpinButtonCtrl	m_wndResultsSpin;
	CEdit	m_wndResults;
	CButton	m_wndDiscoveryGo;
	int		m_nResults;
	BOOL	m_bServerWalk;
	int		m_nLinks;
	BOOL	m_bEnabled;
	BOOL	m_bEnableAlways;
	BOOL	m_bLearnServers;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CDonkeySettingsPage)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDonkeySettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDiscoveryGo();
	afx_msg void OnServerWalk();
	afx_msg void OnEnable();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
