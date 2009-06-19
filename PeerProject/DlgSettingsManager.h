//
// DlgSettingsManager.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#if !defined(AFX_DLGSETTINGSMANAGER_H__7E76D5DE_A0EA_4CA6_B05B_8F4470E2CF03__INCLUDED_)
#define AFX_DLGSETTINGSMANAGER_H__7E76D5DE_A0EA_4CA6_B05B_8F4470E2CF03__INCLUDED_

#pragma once

#include "WndSettingsSheet.h"


class CSettingsManagerDlg : public CSettingsSheet
{
// Construction
public:
	CSettingsManagerDlg(CWnd* pParent = NULL);

// Dialog Data
public:
	//{{AFX_DATA(CSettingsManagerDlg)
	//}}AFX_DATA
	//{{AFX_VIRTUAL(CSettingsManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	CBitmap	m_bmHeader;

// Static Run
public:
	static BOOL Run(LPCTSTR pszWindow = NULL);
	static void OnSkinChange(BOOL bSet);
	static CSettingsManagerDlg* m_pThis;

// Operations
public:
	INT_PTR	DoModal(LPCTSTR pszWindow = NULL);
protected:
	void			AddPage(CSettingsPage* pPage);
	void			AddGroup(CSettingsPage* pPage);
	virtual void	DoPaint(CDC& dc);

// Implementation
protected:
	//{{AFX_MSG(CSettingsManagerDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	virtual void OnOK();
	virtual void OnApply();

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGSETTINGSMANAGER_H__7E76D5DE_A0EA_4CA6_B05B_8F4470E2CF03__INCLUDED_)