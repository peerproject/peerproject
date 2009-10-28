//
// PageProfileCertificate.h
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


class CCertificateProfilePage : public CSettingsPage
{
// Construction
public:
	CCertificateProfilePage();
	virtual ~CCertificateProfilePage();

	DECLARE_DYNCREATE(CCertificateProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CCertificateProfilePage)
	enum { IDD = IDD_PROFILE_CERTIFICATE };
	CString	m_sGUID;
	CString	m_sTime;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CCertificateProfilePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCertificateProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnGuidCreate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
