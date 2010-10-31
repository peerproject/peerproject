//
// PageSettingsDownloads.h
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
#include "CtrlIconButton.h"


class CDownloadsSettingsPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CDownloadsSettingsPage)

public:
	CDownloadsSettingsPage();
	virtual ~CDownloadsSettingsPage();

	enum { IDD = IDD_SETTINGS_DOWNLOADS };

	virtual void OnOK();
	virtual BOOL OnKillActive();

protected:
	CSpinButtonCtrl	m_wndMaxDownTransfers;
	CSpinButtonCtrl	m_wndMaxFileTransfers;
	CSpinButtonCtrl	m_wndMaxDownFiles;
	CIconButtonCtrl	m_wndIncompletePath;
	CIconButtonCtrl	m_wndDownloadsPath;
	CComboBox		m_wndBandwidthLimit;
	CComboBox		m_wndQueueLimit;
	CString			m_sDownloadsPath;
	CString			m_sIncompletePath;
	int				m_nMaxDownFiles;
	int				m_nMaxFileTransfers;
	int				m_nMaxDownTransfers;
	CString			m_sBandwidthLimit;
	CString			m_sQueueLimit;
	BOOL			m_bRequireConnect;
	BOOL			m_bDownloadsChanged;
	CEditPath		m_wndDownloadsFolder;
	CEditPath		m_wndIncompleteFolder;

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	bool	IsLimited(CString& strText) const;

	afx_msg void OnDownloadsBrowse();
	afx_msg void OnIncompleteBrowse();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()
};
