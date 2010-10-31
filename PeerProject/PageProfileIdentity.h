//
// PageProfileIdentity.h
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


class CIdentityProfilePage : public CSettingsPage
{
// Construction
public:
	CIdentityProfilePage();
	virtual ~CIdentityProfilePage();

	DECLARE_DYNCREATE(CIdentityProfilePage)

	enum { IDD = IDD_PROFILE_IDENTITY };

// Dialog Data
public:
	//{{AFX_DATA(CIdentityProfilePage)
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
	virtual void DoDataExchange(CDataExchange* pDX);
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
