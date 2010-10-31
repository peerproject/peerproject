//
// PageSettingsMedia.h
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

#include "WndSettingsPage.h"


class CMediaSettingsPage : public CSettingsPage
{
// Construction
public:
	CMediaSettingsPage();
	virtual ~CMediaSettingsPage();

	DECLARE_DYNCREATE(CMediaSettingsPage)

	enum { IDD = IDD_SETTINGS_MEDIA };

// Dialog Data
public:
	CButton		m_wndRemove;
	CButton		m_wndAdd;
	CComboBox	m_wndList;
	CComboBox	m_wndServices;
	CString		m_sServicePath[3];
	int			m_nSelected;
	CString		m_sType;
	BOOL		m_bEnablePlay;
	BOOL		m_bEnableEnqueue;

// Overrides
protected:
	void Update();

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnMediaPlay();
	afx_msg void OnMediaEnqueue();
	afx_msg void OnSelChangeMediaTypes();
	afx_msg void OnEditChangeMediaTypes();
	afx_msg void OnSelChangeMediaService();
	afx_msg void OnMediaAdd();
	afx_msg void OnMediaRemove();
	afx_msg void OnMediaVis();

	DECLARE_MESSAGE_MAP()
};
