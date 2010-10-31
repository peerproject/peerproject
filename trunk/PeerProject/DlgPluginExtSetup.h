//
// DlgPluginExtSetup.h
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

class CPluginExtSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CPluginExtSetupDlg)

// Construction
public:
	CPluginExtSetupDlg(CWnd* pParent = NULL, LPCTSTR pszExt = NULL );
	virtual ~CPluginExtSetupDlg();

// Members
private:
	enum { IDD = IDD_PLUGIN_EXT_SETUP };

	CListCtrl	m_wndList;
	CListCtrl*	m_pParent;
	CString		m_sExtensions;
	TRISTATE	m_bParentState;
	BOOL		m_bRunning;

// Overrides
protected:
	//{{AFX_VIRTUAL(CPluginExtSetupDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPluginExtSetupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangingAssociations(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOK();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
