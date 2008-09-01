//
// PageSettingsPlugins.h
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

#if !defined(AFX_PAGESETTINGSPLUGINS_H__F89745A8_9CC2_497C_B9E2_6F8C90C206D4__INCLUDED_)
#define AFX_PAGESETTINGSPLUGINS_H__F89745A8_9CC2_497C_B9E2_6F8C90C206D4__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CPluginsSettingsPage : public CSettingsPage
{
// Construction
public:
	CPluginsSettingsPage();
	virtual ~CPluginsSettingsPage();

	DECLARE_DYNCREATE(CPluginsSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CPluginsSettingsPage)
	enum { IDD = IDD_SETTINGS_PLUGINS };
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGESETTINGSPLUGINS_H__F89745A8_9CC2_497C_B9E2_6F8C90C206D4__INCLUDED_)
