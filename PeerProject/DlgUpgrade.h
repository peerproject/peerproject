//
// DlgUpgrade.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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


class CUpgradeDlg : public CSkinDialog
{
// Construction
public:
	CUpgradeDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_UPGRADE };

// Dialog Data
public:
	//{{AFX_DATA(CUpgradeDlg)
	BOOL	m_bCheck;
	CString	m_sMessage;
	//}}AFX_DATA

	void ParseCheckAgain();

// Overrides
protected:
	//{{AFX_VIRTUAL(CUpgradeDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUpgradeDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	//DECLARE_MESSAGE_MAP()
};
