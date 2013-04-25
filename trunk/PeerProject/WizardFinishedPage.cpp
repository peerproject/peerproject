//
// WizardFinishedPage.cpp
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
#include "Settings.h"
#include "PeerProject.h"
#include "WizardFinishedPage.h"
#include "WizardSheet.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CWizardFinishedPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardFinishedPage, CWizardPage)
	ON_WM_XBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardFinishedPage property page

CWizardFinishedPage::CWizardFinishedPage() : CWizardPage(CWizardFinishedPage::IDD)
	, m_bAutoConnect( FALSE )
	, m_bStartup	( FALSE )
{
}

CWizardFinishedPage::~CWizardFinishedPage()
{
}

void CWizardFinishedPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_WIZARD_AUTO, m_bAutoConnect);
	DDX_Check(pDX, IDC_WIZARD_STARTUP, m_bStartup);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardFinishedPage message handlers

BOOL CWizardFinishedPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardFinishedPage"), this );

	m_bAutoConnect = Settings.Connection.AutoConnect;
	m_bStartup = ( Settings.Live.FirstRun ? FALSE : Settings.CheckStartup() );

	UpdateData( FALSE );

	return TRUE;
}

BOOL CWizardFinishedPage::OnSetActive()
{
	// Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	CoolInterface.FixThemeControls( this );		// Checkbox/Groupbox text colors (Remove theme if needed)

	SetWizardButtons( PSWIZB_BACK | PSWIZB_FINISH );
	return CWizardPage::OnSetActive();
}

void CWizardFinishedPage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
}

LRESULT CWizardFinishedPage::OnWizardBack()
{
	return 0;
}

BOOL CWizardFinishedPage::OnWizardFinish()
{
	UpdateData();

	Settings.Connection.AutoConnect = m_bAutoConnect != FALSE;
	Settings.SetStartup( m_bStartup );

	return CWizardPage::OnWizardFinish();
}
