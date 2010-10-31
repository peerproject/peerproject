//
// DlgDiscoveryService.h
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

class CDiscoveryService;


class CDiscoveryServiceDlg : public CSkinDialog
{
// Construction
public:
	CDiscoveryServiceDlg(CWnd* pParent = NULL, CDiscoveryService* pService = NULL);
	virtual ~CDiscoveryServiceDlg();

	enum { IDD = IDD_DISCOVERY_SERVICE };

// Dialog Data
public:
	//{{AFX_DATA(CDiscoveryServiceDlg)
	CButton	m_wndOK;
	CString	m_sAddress;
	int		m_nType;
	//}}AFX_DATA

	CDiscoveryService* m_pService;
	BOOL m_bNew;

// Overrides
public:
	//{{AFX_VIRTUAL(CDiscoveryServiceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDiscoveryServiceDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAddress();
	afx_msg void OnSelChangeServiceType();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
