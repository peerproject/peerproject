//
// PageSettingsLibrary.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2006.
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
#include "CtrlIconButton.h"


class CLibrarySettingsPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CLibrarySettingsPage)

public:
	CLibrarySettingsPage();
	virtual ~CLibrarySettingsPage();

	enum { IDD = IDD_SETTINGS_LIBRARY };

	virtual void OnOK();

protected:
	CSpinButtonCtrl	m_wndRecentTotal;
	CSpinButtonCtrl	m_wndRecentDays;
	CButton			m_wndSafeRemove;
	CButton			m_wndSafeAdd;
	CComboBox		m_wndSafeList;
	CButton			m_wndPrivateRemove;
	CButton			m_wndPrivateAdd;
	CComboBox		m_wndPrivateList;
	BOOL			m_bWatchFolders;
	DWORD			m_nRecentDays;
	int 			m_nRecentTotal;
	BOOL			m_bStoreViews;
	BOOL			m_bHighPriorityHash;
	BOOL			m_bBrowseFiles;
	BOOL			m_bMakeGhosts;
	BOOL			m_bSmartSeries;
	CString			m_sCollectionPath;
	CIconButtonCtrl	m_wndCollectionPath;
	CEditPath		m_wndCollectionFolder;
	CButton			m_wndRecentClear;
	CButton			m_wndRecentClearGhosts;

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg void OnSelChangeSafeTypes();
	afx_msg void OnEditChangeSafeTypes();
	afx_msg void OnSafeAdd();
	afx_msg void OnSafeRemove();
	afx_msg void OnSelChangePrivateTypes();
	afx_msg void OnEditChangePrivateTypes();
	afx_msg void OnPrivateAdd();
	afx_msg void OnPrivateRemove();
	afx_msg void OnRecentClear();
	afx_msg void OnRecentClearGhosts();
	afx_msg void OnCollectionsBrowse();

	DECLARE_MESSAGE_MAP()
};
