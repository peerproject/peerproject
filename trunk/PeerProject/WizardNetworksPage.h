//
// WizardNetworksPage.h
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

#if !defined(AFX_WIZARDNETWORKSPAGE_H__7EB199BE_1FB8_421B_B80C_5214FED8C9DA__INCLUDED_)
#define AFX_WIZARDNETWORKSPAGE_H__7EB199BE_1FB8_421B_B80C_5214FED8C9DA__INCLUDED_

#pragma once

#include "WizardSheet.h"


class CWizardNetworksPage : public CWizardPage
{
// Construction
public:
	CWizardNetworksPage();
	virtual ~CWizardNetworksPage();

	DECLARE_DYNCREATE(CWizardNetworksPage)

// Dialog Data
public:
	//{{AFX_DATA(CWizardNetworksPage)
	enum { IDD = IDD_WIZARD_NETWORKS };
	CButton	m_wndG2Enable;
	BOOL	m_bG2Enable;
	BOOL	m_bG1Enable;
	BOOL	m_bEDEnable;
	//}}AFX_DATA

	void DoDonkeyImport();

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardNetworksPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardNetworksPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_WIZARDNETWORKSPAGE_H__7EB199BE_1FB8_421B_B80C_5214FED8C9DA__INCLUDED_)
