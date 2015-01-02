//
// DlgDiscoveryService.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
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

class CDiscoveryService;


class CDiscoveryServiceDlg : public CSkinDialog
{
public:
	CDiscoveryServiceDlg(CWnd* pParent = NULL, CDiscoveryService* pService = NULL);
	virtual ~CDiscoveryServiceDlg();

	enum { IDD = IDD_DISCOVERY_SERVICE };

public:
	CButton	m_wndOK;
	CString	m_sAddress;
	int		m_nType;

	CDiscoveryService* m_pService;
	BOOL m_bNew;

public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAddress();
	afx_msg void OnSelChangeServiceType();

	DECLARE_MESSAGE_MAP()
};
