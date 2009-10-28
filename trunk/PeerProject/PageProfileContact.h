//
// PageProfileContact.h
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


class CContactProfilePage : public CSettingsPage
{
// Construction
public:
	CContactProfilePage();
	virtual ~CContactProfilePage();

	DECLARE_DYNCREATE(CContactProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CContactProfilePage)
	enum { IDD = IDD_PROFILE_CONTACT };
	CString	m_sEmail;
	CString	m_sAOL;
	CString	m_sICQ;
	CString	m_sYahoo;
	CString	m_sMSN;
	CString m_sJabber;
	//}}AFX_DATA

	void	AddAddress(LPCTSTR pszClass, LPCTSTR pszName, LPCTSTR pszAddress);

// Overrides
public:
	//{{AFX_VIRTUAL(CContactProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CContactProfilePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
