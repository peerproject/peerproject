//
// PageFileSharing.h
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

#if !defined(AFX_PAGEFILESHARING_H__483C6B26_E29E_4CCD_9524_2704C3EA0A03__INCLUDED_)
#define AFX_PAGEFILESHARING_H__483C6B26_E29E_4CCD_9524_2704C3EA0A03__INCLUDED_

#pragma once

#include "DlgFilePropertiesPage.h"


class CFileSharingPage : public CFilePropertiesPage
{
// Construction
public:
	CFileSharingPage();
	virtual ~CFileSharingPage();

	DECLARE_DYNCREATE(CFileSharingPage)

// Dialog Data
public:
	//{{AFX_DATA(CFileSharingPage)
	enum { IDD = IDD_FILE_SHARING };
	CComboBox	m_wndTags;
	CButton	m_wndShare;
	CListCtrl	m_wndNetworks;
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEFILESHARING_H__483C6B26_E29E_4CCD_9524_2704C3EA0A03__INCLUDED_)
