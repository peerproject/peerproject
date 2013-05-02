//
// PageFileSharing.h
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

#include "DlgFilePropertiesPage.h"


class CFileSharingPage : public CFilePropertiesPage
{
	DECLARE_DYNCREATE(CFileSharingPage)

// Construction
public:
	CFileSharingPage();
	virtual ~CFileSharingPage();

	enum { IDD = IDD_FILE_SHARING };

// Dialog Data
public:
	//{{AFX_DATA(CFileSharingPage)
	//CListCtrl	m_wndNetworks;
	CComboBox	m_wndTags;
	CButton	m_wndShare;
	int		m_bOverride;
	BOOL	m_bShare;
	CString	m_sTags;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CFileSharingPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFileSharingPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShareOverride0();
	afx_msg void OnShareOverride1();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
