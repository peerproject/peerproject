//
// DlgDownloadGroup.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#if !defined(DLGDOWNLOADGROUP_H)
#define DLGDOWNLOADGROUP_H

#pragma once

#include "DlgSkinDialog.h"
#include "CtrlIconButton.h"

class CDownloadGroup;


class CDownloadGroupDlg : public CSkinDialog
{
// Construction
public:
	CDownloadGroupDlg(CDownloadGroup* pGroup, CWnd* pParent = NULL);

// Dialog Data
public:
	enum { IDD = IDD_DOWNLOAD_GROUP };
	CIconButtonCtrl	m_wndBrowse;
	CListCtrl		m_wndImages;
	CEdit			m_wndFolder;
	CButton			m_wndFilterAdd;
	CButton			m_wndFilterRemove;
	CComboBox		m_wndFilterList;
	CString			m_sName;
	CString			m_sFolder;

	CDownloadGroup*	m_pGroup;

protected:
	bool			m_bInitializing;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnFilterAdd();
	afx_msg void OnFilterRemove();
	afx_msg void OnEditChangeFilterList();
	afx_msg void OnSelChangeFilterList();
	virtual void OnOK();
	afx_msg void OnBrowse();
	afx_msg void OnLvnItemchangingIconList(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(DLGDOWNLOADGROUP_H)
