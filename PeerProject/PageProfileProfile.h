//
// PageProfileProfile.h
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

#include "WndSettingsPage.h"

class CWorldGPS;


class CProfileProfilePage : public CSettingsPage
{
// Construction
public:
	CProfileProfilePage();
	virtual ~CProfileProfilePage();

	DECLARE_DYNCREATE(CProfileProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CProfileProfilePage)
	enum { IDD = IDD_PROFILE_PROFILE };
	CButton	m_wndInterestRemove;
	CButton	m_wndInterestAdd;
	CComboBox	m_wndInterestAll;
	CListBox	m_wndInterestList;
	CComboBox	m_wndAge;
	CComboBox	m_wndCity;
	CComboBox	m_wndCountry;
	CString	m_sLocCity;
	CString	m_sLocCountry;
	CString	m_sLocLatitude;
	CString	m_sLocLongitude;
	CString	m_sAge;
	CString	m_sGender;
	//}}AFX_DATA

	CWorldGPS*	m_pWorld;

// Overrides
public:
	//{{AFX_VIRTUAL(CProfileProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CProfileProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeCountry();
	afx_msg void OnSelChangeCity();
	afx_msg void OnSelChangeInterestList();
	afx_msg void OnSelChangeInterestAll();
	afx_msg void OnEditChangeInterestAll();
	afx_msg void OnInterestAdd();
	afx_msg void OnInterestRemove();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
