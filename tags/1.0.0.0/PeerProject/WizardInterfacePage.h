//
// WizardInterfacePage.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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


class CWizardInterfacePage : public CWizardPage
{
	DECLARE_DYNCREATE(CWizardInterfacePage)

// Construction
public:
	CWizardInterfacePage();
	virtual ~CWizardInterfacePage();

	enum { IDD = IDD_WIZARD_INTERFACE };

// Dialog Data
public:
	//{{AFX_DATA(CWizardInterfacePage)
	CStatic	m_wndDescriptionExpert;
	CStatic	m_wndDescriptionBasic;
	CButton	m_wndInterfaceExpert;
	CButton	m_wndInterfaceBasic;
	BOOL	m_bSimpleDownloadBars;
	int		m_bExpert;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardInterfacePage)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardInterfacePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
