//
// DlgProfileManager.h
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

#include "WndSettingsSheet.h"


class CProfileManagerDlg : public CSettingsSheet
{
	DECLARE_DYNAMIC(CProfileManagerDlg)

// Construction
public:
	CProfileManagerDlg(CWnd* pParent = NULL);
	//virtual ~CProfileManagerDlg();

// Operations
public:
	static BOOL	Run(LPCTSTR pszWindow = NULL);
	INT_PTR		DoModal(LPCTSTR pszWindow = NULL);

protected:
	void		AddPage(CSettingsPage* pPage);
	void		AddGroup(CSettingsPage* pPage);

// Overrides
public:
	//{{AFX_VIRTUAL(CProfileManagerDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CProfileManagerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnApply();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
