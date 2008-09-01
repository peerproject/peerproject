//
// PageSettingsDonkey.h
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

#if !defined(AFX_PAGESETTINGSDONKEY_H__BAAD25E2_5409_43A7_ADA2_2B4409B166AD__INCLUDED_)
#define AFX_PAGESETTINGSDONKEY_H__BAAD25E2_5409_43A7_ADA2_2B4409B166AD__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CDonkeySettingsPage : public CSettingsPage
{
// Construction
public:
	CDonkeySettingsPage();
	virtual ~CDonkeySettingsPage();

	DECLARE_DYNCREATE(CDonkeySettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CDonkeySettingsPage)
	enum { IDD = IDD_SETTINGS_DONKEY };
	CSpinButtonCtrl	m_wndLinksSpin;
	CEdit	m_wndResults;
	CSpinButtonCtrl	m_wndResultsSpin;
	CButton	m_wndDiscoveryGo;
	int		m_nResults;
	BOOL	m_bServerWalk;
	int		m_nLinks;
	BOOL	m_bEnableToday;
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDonkeySettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDiscoveryGo();
	afx_msg void OnServerWalk();
	afx_msg void OnEnableToday();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGESETTINGSDONKEY_H__BAAD25E2_5409_43A7_ADA2_2B4409B166AD__INCLUDED_)
