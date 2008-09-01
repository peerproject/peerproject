//
// PageProfileFiles.h
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

#if !defined(AFX_PAGEPROFILEFILES_H__0AA47105_BB43_453D_9A80_8A27255593F7__INCLUDED_)
#define AFX_PAGEPROFILEFILES_H__0AA47105_BB43_453D_9A80_8A27255593F7__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CFilesProfilePage : public CSettingsPage
{
// Construction
public:
	CFilesProfilePage();
	virtual ~CFilesProfilePage();

	DECLARE_DYNCREATE(CFilesProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CFilesProfilePage)
	enum { IDD = IDD_PROFILE_FILES };
	CListCtrl	m_wndList;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CFilesProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFilesProfilePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEPROFILEFILES_H__0AA47105_BB43_453D_9A80_8A27255593F7__INCLUDED_)
