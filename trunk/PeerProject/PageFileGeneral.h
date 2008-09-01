//
// PageFileGeneral.h
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

#if !defined(AFX_PAGEFILEGENERAL_H__3FE3563E_A574_48F0_88EB_4AD6B592BE64__INCLUDED_)
#define AFX_PAGEFILEGENERAL_H__3FE3563E_A574_48F0_88EB_4AD6B592BE64__INCLUDED_

#pragma once

#include "DlgFilePropertiesPage.h"


class CFileGeneralPage : public CFilePropertiesPage
{
// Construction
public:
	CFileGeneralPage();
	virtual ~CFileGeneralPage();

	DECLARE_DYNCREATE(CFileGeneralPage)

// Dialog Data
public:
	//{{AFX_DATA(CFileGeneralPage)
	enum { IDD = IDD_FILE_GENERAL };
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
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFileGeneralPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEFILEGENERAL_H__3FE3563E_A574_48F0_88EB_4AD6B592BE64__INCLUDED_)
