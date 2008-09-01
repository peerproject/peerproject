//
// PageWelcome.h
//
// Portions Copyright Shareaza Development Team, 2007.
// This file is part of PeerProject Torrent Wizard (peerproject.org).
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#if !defined(AFX_PAGEWELCOME_H__8EF056B1_B633_4CA7_AC46_A99B4A7C9A4F__INCLUDED_)
#define AFX_PAGEWELCOME_H__8EF056B1_B633_4CA7_AC46_A99B4A7C9A4F__INCLUDED_

#pragma once

#include "WizardSheet.h"

class CWelcomePage : public CWizardPage
{
// Construction
public:
	CWelcomePage();
	virtual ~CWelcomePage();

	DECLARE_DYNCREATE(CWelcomePage)

// Dialog Data
public:
	//{{AFX_DATA(CWelcomePage)
	enum { IDD = IDD_WELCOME_PAGE };
	int		m_nType;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CWelcomePage)
	public:
	virtual void OnReset();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWelcomePage)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEWELCOME_H__8EF056B1_B633_4CA7_AC46_A99B4A7C9A4F__INCLUDED_)
