//
// WizardProfilePage.h
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

class CWorldGPS;


class CWizardProfilePage : public CWizardPage
{
	DECLARE_DYNCREATE(CWizardProfilePage)

// Construction
public:
	CWizardProfilePage();
	virtual ~CWizardProfilePage();

	enum { IDD = IDD_WIZARD_PROFILE };

// Dialog Data
public:
	//{{AFX_DATA(CWizardProfilePage)
	CString 	m_sNick;
	CWorldGPS*	m_pWorld;
	CString		m_sLocCity;
	CString		m_sLocCountry;
	CComboBox	m_wndCity;
	CComboBox	m_wndCountry;
	CComboBox	m_wndAge;
	int 		m_nAge;
	int			m_nGender;
	CEdit		m_wndComments;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardProfilePage)
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardProfilePage)
	afx_msg void OnSelChangeCountry();
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
