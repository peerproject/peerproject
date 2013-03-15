//
// PageProfileFiles.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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


class CFilesProfilePage : public CSettingsPage
{
	DECLARE_DYNCREATE(CFilesProfilePage)

// Construction
public:
	CFilesProfilePage();
	virtual ~CFilesProfilePage();

	enum { IDD = IDD_PROFILE_FILES };

// Dialog Data
public:
	//{{AFX_DATA(CFilesProfilePage)
	CListCtrl	m_wndList;
	//}}AFX_DATA

// Overrides
protected:
	//{{AFX_VIRTUAL(CFilesProfilePage)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFilesProfilePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	//DECLARE_MESSAGE_MAP()
};
