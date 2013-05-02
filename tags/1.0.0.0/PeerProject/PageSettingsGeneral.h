//
// PageSettingsGeneral.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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


class CGeneralSettingsPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CGeneralSettingsPage)

// Construction
public:
	CGeneralSettingsPage();
	virtual ~CGeneralSettingsPage();

	enum { IDD = IDD_SETTINGS_GENERAL };

// Dialog Data
public:
	//{{AFX_DATA(CGeneralSettingsPage)
	BOOL	m_bAutoConnect;
	BOOL	m_bStartup;
	BOOL	m_bPromptURLs;
	BOOL	m_bUpdateCheck;
	BOOL	m_bExpandDownloads;
	BOOL	m_bSimpleBar;
	BOOL	m_bExpandMatches;
	BOOL	m_bSwitchToTransfers;
	BOOL	m_bHideSearch;
	BOOL	m_bAdultFilter;
	BOOL	m_bTipShadow;
	int		m_nCloseMode;
	int		m_bTrayMinimise;
	int		m_nRatesInBytes;
	CSpinButtonCtrl	m_wndTipSpin;
	CListCtrl	m_wndTips;
	CSliderCtrl	m_wndTipAlpha;
	CComboBox	m_wndCloseMode;
	CComboBox	m_wndTrayMinimise;
	DWORD	m_nTipDelay;
	//}}AFX_DATA

protected:
	void Add(LPCTSTR pszName, BOOL bState);

// Overrides
public:
	//{{AFX_VIRTUAL(CGeneralSettingsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGeneralSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDropdownCloseMode();
	afx_msg void OnDropdownTrayMinimise();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
