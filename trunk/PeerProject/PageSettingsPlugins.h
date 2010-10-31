//
// PageSettingsPlugins.h
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


class CPluginsSettingsPage : public CSettingsPage
{
// Construction
public:
	CPluginsSettingsPage();
	virtual ~CPluginsSettingsPage();

	DECLARE_DYNCREATE(CPluginsSettingsPage)

	enum { IDD = IDD_SETTINGS_PLUGINS };

// Dialog Data
public:
	//{{AFX_DATA(CPluginsSettingsPage)
	CButton	m_wndSetup;
	CEdit	m_wndDesc;
	CStatic	m_wndName;
	CListCtrl	m_wndList;
	//}}AFX_DATA

protected:
	CImageList	m_gdiImageList;
	BOOL		m_bRunning;
protected:
	void		InsertPlugin(LPCTSTR pszCLSID, LPCTSTR pszName, int nImage, TRISTATE bEnabled,
							 LPVOID pPlugin = NULL, LPCTSTR pszExtension = NULL);
	void		EnumerateGenericPlugins();
	void		EnumerateMiscPlugins();
	void		EnumerateMiscPlugins(LPCTSTR pszType, HKEY hRoot);
	void		AddMiscPlugin(LPCTSTR pszType, LPCTSTR pszCLSID, LPCTSTR pszExtension = NULL);
	CString		GetPluginComments(LPCTSTR pszCLSID) const;

public:
	void		UpdateList();

// Overrides
public:
	//{{AFX_VIRTUAL(CPluginsSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPluginsSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemChangingPlugins(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemChangedPlugins(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkPlugins(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomDrawPlugins(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPluginsSetup();
	afx_msg void OnPluginsWeb();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
