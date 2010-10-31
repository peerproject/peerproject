//
// DlgShareManager.h
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

#include "DlgSkinDialog.h"


class CShareManagerDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CShareManagerDlg)

// Construction
public:
	CShareManagerDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_SHARE_MANAGER };

// Dialog Data
public:
	//{{AFX_DATA(CShareManagerDlg)
	CListCtrl	m_wndList;
	CButton 	m_wndRemove;
	//}}AFX_DATA

// Overrides
protected:
	//{{AFX_VIRTUAL(CShareManagerDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CShareManagerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnShareAdd();
	afx_msg void OnShareRemove();
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedShareFolders(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
