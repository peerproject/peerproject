//
// WizardWelcomePage.h
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

#if !defined(AFX_WIZARDWELCOMEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
#define AFX_WIZARDWELCOMEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_

#pragma once

#include "WizardSheet.h"


class CWizardWelcomePage : public CWizardPage
{
// Construction
public:
	CWizardWelcomePage();
	virtual ~CWizardWelcomePage();

	DECLARE_DYNCREATE(CWizardWelcomePage)

// Dialog Data
public:
	enum { IDD = IDD_WIZARD_WELCOME };

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetActive();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

};

#endif // !defined(AFX_WIZARDWELCOMEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
