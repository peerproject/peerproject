//
// DlgProfileManager.h
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

#pragma once

#include "WndSettingsSheet.h"


class CProfileManagerDlg : public CSettingsSheet
{
// Construction
public:
	CProfileManagerDlg(CWnd* pParent = NULL);
	virtual ~CProfileManagerDlg();

	DECLARE_DYNAMIC(CProfileManagerDlg)

// Attributes
protected:
	CBitmap	m_bmHeader;

// Operations
public:
	static BOOL		Run(LPCTSTR pszWindow = NULL);
	INT_PTR			DoModal(LPCTSTR pszWindow = NULL);
protected:
	void			AddPage(CSettingsPage* pPage);
	void			AddGroup(CSettingsPage* pPage);
	virtual void	DoPaint(CDC& dc);

// Overrides
public:
	//{{AFX_VIRTUAL(CProfileManagerDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CProfileManagerDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	virtual void OnOK();
	virtual void OnApply();

	DECLARE_MESSAGE_MAP()
};
