//
// WizardFoldersPage.h
//
// This file is part of PeerProject (peerproject.org) © 2010
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

#include "WizardSheet.h"
#include "WndSettingsPage.h"
#include "CtrlIconButton.h"


class CWizardFoldersPage : public CWizardPage
{
	DECLARE_DYNCREATE(CWizardFoldersPage)

public:
	CWizardFoldersPage();
	virtual ~CWizardFoldersPage();

	enum { IDD = IDD_WIZARD_FOLDERS };

public:
	CIconButtonCtrl	m_wndDownloadsBrowse;
	CIconButtonCtrl	m_wndIncompleteBrowse;
	CIconButtonCtrl	m_wndTorrentsBrowse;
	CEditPath		m_wndDownloadsFolder;
	CEditPath		m_wndIncompleteFolder;
	CEditPath		m_wndTorrentsFolder;
	CString			m_sDownloadsPath;
	CString			m_sIncompletePath;
	CString			m_sTorrentsPath;

	void DoDonkeyImport();

	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();
	afx_msg void OnDownloadsBrowse();
	afx_msg void OnIncompleteBrowse();
	afx_msg void OnTorrentsBrowse();
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);

	DECLARE_MESSAGE_MAP()
};
