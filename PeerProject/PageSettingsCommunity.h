//
// PageSettingsCommunity.h
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


class CCommunitySettingsPage : public CSettingsPage
{
// Construction
public:
	CCommunitySettingsPage();
	virtual ~CCommunitySettingsPage();

	DECLARE_DYNCREATE(CCommunitySettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CCommunitySettingsPage)
	enum { IDD = IDD_SETTINGS_COMMUNITY };
	BOOL	m_bChatEnable;
	BOOL	m_bChatAllNetworks;
	BOOL	m_bChatFilter;
	BOOL	m_bChatCensor;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CCommunitySettingsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCommunitySettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditProfile();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
