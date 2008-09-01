//
// PageProfileBio.h
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

#if !defined(AFX_PAGEPROFILEBIO_H__98D89C38_9799_42D2_969E_F78FCDC7A167__INCLUDED_)
#define AFX_PAGEPROFILEBIO_H__98D89C38_9799_42D2_969E_F78FCDC7A167__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CBioProfilePage : public CSettingsPage
{
// Construction
public:
	CBioProfilePage();
	virtual ~CBioProfilePage();

	DECLARE_DYNCREATE(CBioProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CBioProfilePage)
	enum { IDD = IDD_PROFILE_BIO };
	CEdit	m_wndText;
	//}}AFX_DATA

// Overrides
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBioProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CBioProfilePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEPROFILEBIO_H__98D89C38_9799_42D2_969E_F78FCDC7A167__INCLUDED_)
