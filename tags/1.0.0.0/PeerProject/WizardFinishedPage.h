//
// WizardFinishedPage.h
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

#include "WizardSheet.h"


class CWizardFinishedPage : public CWizardPage
{
	DECLARE_DYNCREATE(CWizardFinishedPage)

// Construction
public:
	CWizardFinishedPage();
	virtual ~CWizardFinishedPage();

	enum { IDD = IDD_WIZARD_FINISHED };

// Dialog Data
public:
	//{{AFX_DATA(CWizardFinishedPage)
	BOOL	m_bAutoConnect;
	BOOL	m_bStartup;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardFinishedPage)
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
protected:
	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardFinishedPage)
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
