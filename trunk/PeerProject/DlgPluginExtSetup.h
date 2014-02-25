//
// DlgPluginExtSetup.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
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

class CPluginExtSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CPluginExtSetupDlg)

public:
	CPluginExtSetupDlg(CWnd* pParent = NULL, LPCTSTR pszExt = NULL );
	virtual ~CPluginExtSetupDlg();

private:
	enum { IDD = IDD_PLUGIN_EXT_SETUP };

	CListCtrl	m_wndList;
	CListCtrl*	m_pParent;
	CString		m_sExtensions;
	TRISTATE	m_bParentState;
	BOOL		m_bRunning;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnChangingAssociations(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOK();

	DECLARE_MESSAGE_MAP()
};
