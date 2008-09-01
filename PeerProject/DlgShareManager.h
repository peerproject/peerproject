//
// DlgShareManager.h
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

#if !defined(AFX_DLGSHAREMANAGER_H__70B35897_B7D9_4CD0_A2CA_6BA31B717156__INCLUDED_)
#define AFX_DLGSHAREMANAGER_H__70B35897_B7D9_4CD0_A2CA_6BA31B717156__INCLUDED_

#pragma once

#include "DlgSkinDialog.h"


class CShareManagerDlg : public CSkinDialog
{
// Construction
public:
	CShareManagerDlg(CWnd* pParent = NULL);

	DECLARE_DYNAMIC(CShareManagerDlg)

// Dialog Data
public:
	//{{AFX_DATA(CShareManagerDlg)
	enum { IDD = IDD_SHARE_MANAGER };
	CButton	m_wndRemove;
	CListCtrl	m_wndList;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CShareManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CShareManagerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnShareAdd();
	afx_msg void OnShareRemove();
	afx_msg void OnItemChangedShareFolders(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGSHAREMANAGER_H__70B35897_B7D9_4CD0_A2CA_6BA31B717156__INCLUDED_)
