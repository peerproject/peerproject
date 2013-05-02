//
// WizardSharePage.h
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

#include "WizardSheet.h"


class CWizardSharePage : public CWizardPage
{
	DECLARE_DYNCREATE(CWizardSharePage)

// Construction
public:
	CWizardSharePage();
	virtual ~CWizardSharePage();

	enum { IDD = IDD_WIZARD_SHARING };

// Dialog Data
public:
	//{{AFX_DATA(CWizardSharePage)
	CListCtrl	m_wndList;
	CButton 	m_wndRemove;
	//}}AFX_DATA

	void	AddPhysicalFolder(LPCTSTR pszFolder);
	void	AddRegistryFolder(HKEY hRoot, LPCTSTR pszKey, LPCTSTR pszValue);

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardSharePage)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardSharePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedShareFolders(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShareAdd();
	afx_msg void OnShareRemove();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
