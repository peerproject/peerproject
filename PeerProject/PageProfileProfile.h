//
// PageProfileProfile.h
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

#include "WndSettingsPage.h"

class CWorldGPS;


class CProfileProfilePage : public CSettingsPage
{
// Construction
public:
	CProfileProfilePage();
	virtual ~CProfileProfilePage();

	DECLARE_DYNCREATE(CProfileProfilePage)

	enum { IDD = IDD_PROFILE_PROFILE };

// Dialog Data
public:
	//{{AFX_DATA(CProfileProfilePage)
	CButton 	m_wndInterestAdd;
	CButton 	m_wndInterestRemove;
	CListBox	m_wndInterestList;
	CComboBox	m_wndInterestAll;
	CComboBox	m_wndAge;
	CComboBox	m_wndCity;
	CComboBox	m_wndCountry;
	CString 	m_sLocCity;
	CString 	m_sLocCountry;
	CString 	m_sLocLatitude;
	CString 	m_sLocLongitude;
	CString 	m_sAge;
	CString 	m_sGender;
	//}}AFX_DATA

	CWorldGPS*	m_pWorld;

private:
	int 		LoadDefaultInterests();

// Overrides
public:
	//{{AFX_VIRTUAL(CProfileProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CProfileProfilePage)
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
