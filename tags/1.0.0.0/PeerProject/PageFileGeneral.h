//
// PageFileGeneral.h
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

#include "DlgFilePropertiesPage.h"


class CFileGeneralPage : public CFilePropertiesPage
{
	DECLARE_DYNCREATE(CFileGeneralPage)

// Construction
public:
	CFileGeneralPage();
	virtual ~CFileGeneralPage();

	enum { IDD = IDD_FILE_GENERAL };

// Dialog Data
public:
	//{{AFX_DATA(CFileGeneralPage)
	CString	m_sSHA1;
	CString	m_sTiger;
	CString	m_sType;
	CString	m_sSize;
	CString	m_sPath;
	CString	m_sModified;
	CString	m_sIndex;
	CString	m_sMD5;
	CString	m_sED2K;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CFileGeneralPage)
	virtual void OnOK();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFileGeneralPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
