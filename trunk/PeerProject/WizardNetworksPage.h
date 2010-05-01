//
// WizardNetworksPage.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "WizardSheet.h"


class CWizardNetworksPage : public CWizardPage
{
	DECLARE_DYNCREATE(CWizardNetworksPage)

// Construction
public:
	CWizardNetworksPage();
	virtual ~CWizardNetworksPage();

	enum { IDD = IDD_WIZARD_NETWORKS };

// Dialog Data
public:
	//{{AFX_DATA(CWizardNetworksPage)
	BOOL	m_bG2Enable;
	BOOL	m_bG1Enable;
	BOOL	m_bEDEnable;
	BOOL	m_bHandleTorrents;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardNetworksPage)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
protected:
	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardNetworksPage)
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
