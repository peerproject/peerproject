//
// DlgDonkeyImport.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "DlgSkinDialog.h"
#include "EDPartImporter.h"


class CDonkeyImportDlg : public CSkinDialog
{
// Construction
public:
	CDonkeyImportDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_DONKEY_IMPORT };

// Dialog Data
public:
	//{{AFX_DATA(CDonkeyImportDlg)
	CButton	m_wndClose;
	CButton	m_wndCancel;
	CButton	m_wndImport;
	CEdit	m_wndLog;

	CString	m_sCancel;
	CEDPartImporter	m_pImporter;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CDonkeyImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDonkeyImportDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnImport();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
