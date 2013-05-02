//
// WizardWelcomePage.cpp
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "WizardSheet.h"
#include "WizardWelcomePage.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CWizardWelcomePage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardWelcomePage, CWizardPage)
	ON_WM_XBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardWelcomePage property page

CWizardWelcomePage::CWizardWelcomePage() : CWizardPage(CWizardWelcomePage::IDD)
{
}

CWizardWelcomePage::~CWizardWelcomePage()
{
}

void CWizardWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardWelcomePage message handlers

BOOL CWizardWelcomePage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardWelcomePage"), this );

	SetWizardButtons( PSWIZB_NEXT );

	return TRUE;
}

BOOL CWizardWelcomePage::OnSetActive()
{
	//Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	SetWizardButtons( PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

void CWizardWelcomePage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}
