//
// DlgDownloadGroup.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
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

#include "DlgSkinDialog.h"
#include "CtrlIconButton.h"
#include "CtrlSchemaCombo.h"

class CDownloadGroup;


class CDownloadGroupDlg : public CSkinDialog
{
public:
	CDownloadGroupDlg(CDownloadGroup* pGroup, CWnd* pParent = NULL);

	enum { IDD = IDD_DOWNLOAD_GROUP };

protected:
	CIconButtonCtrl	m_wndBrowse;
	CIconButtonCtrl	m_wndCancel;
	CSchemaCombo	m_wndSchemas;
	CEdit			m_wndFolder;
	CButton			m_wndFilterAdd;
	CButton			m_wndFilterRemove;
	CComboBox		m_wndFilterList;
	CString			m_sName;
	CString			m_sFolder;
	BOOL			m_bTorrent;
	CDownloadGroup*	m_pGroup;
	CString			m_sOldSchemaURI;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnFilterAdd();
	afx_msg void OnFilterRemove();
	afx_msg void OnEditChangeFilterList();
	afx_msg void OnSelChangeFilterList();
	afx_msg void OnBrowse();
	afx_msg void OnCbnCloseupSchemas();
	afx_msg void OnBnClickedDownloadDefault();

	DECLARE_MESSAGE_MAP()
};
