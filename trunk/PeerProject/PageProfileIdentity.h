//
// PageProfileIdentity.h
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

#if !defined(AFX_PAGEPROFILEIDENTITY_H__C5556B79_CE0F_4636_93B4_2F4D0ECD6A11__INCLUDED_)
#define AFX_PAGEPROFILEIDENTITY_H__C5556B79_CE0F_4636_93B4_2F4D0ECD6A11__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CIdentityProfilePage : public CSettingsPage
{
// Construction
public:
	CIdentityProfilePage();
	virtual ~CIdentityProfilePage();

	DECLARE_DYNCREATE(CIdentityProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CIdentityProfilePage)
	enum { IDD = IDD_PROFILE_IDENTITY };
	CComboBox	m_wndAge;
	CString	m_sAge;
	CString	m_sGender;
	CString	m_sNick;
	CString	m_sFirst;
	CString	m_sLast;
	BOOL	m_bBrowseUser;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CIdentityProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CIdentityProfilePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	void GetGenderTranslations(CString& pMale, CString& pFemale);

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEPROFILEIDENTITY_H__C5556B79_CE0F_4636_93B4_2F4D0ECD6A11__INCLUDED_)
