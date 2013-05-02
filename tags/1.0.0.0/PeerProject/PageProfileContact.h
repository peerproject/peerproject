//
// PageProfileContact.h
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

#include "WndSettingsPage.h"


class CContactProfilePage : public CSettingsPage
{
	DECLARE_DYNCREATE(CContactProfilePage)

// Construction
public:
	CContactProfilePage();
	virtual ~CContactProfilePage();

	enum { IDD = IDD_PROFILE_CONTACT };

// Dialog Data
public:
	//{{AFX_DATA(CContactProfilePage)
	CString	m_sEmail;
	CString	m_sMSN;
	CString	m_sYahoo;
	CString	m_sAOL;
	CString	m_sICQ;
	CString m_sJabber;
	CString m_sTwitter;
	CString m_sFacebook;
	CString m_sPeerProjectOrg;
	//}}AFX_DATA

	void	AddAddress(LPCTSTR pszClass, LPCTSTR pszName, LPCTSTR pszAddress);

// Overrides
protected:
	//{{AFX_VIRTUAL(CContactProfilePage)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CContactProfilePage)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
