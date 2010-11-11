//
// WizardFinishedPage.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "WizardSheet.h"
#include "WizardFinishedPage.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CWizardFinishedPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardFinishedPage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardFinishedPage)
	ON_WM_XBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardFinishedPage property page

CWizardFinishedPage::CWizardFinishedPage() : CWizardPage(CWizardFinishedPage::IDD)
	, m_bAutoConnect( FALSE )
	, m_bStartup	( FALSE )
{
	//{{AFX_DATA_INIT(CWizardFinishedPage)
	//}}AFX_DATA_INIT
}

CWizardFinishedPage::~CWizardFinishedPage()
{
}

void CWizardFinishedPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardFinishedPage)
	DDX_Check(pDX, IDC_WIZARD_AUTO, m_bAutoConnect);
	DDX_Check(pDX, IDC_WIZARD_STARTUP, m_bStartup);
	//}}AFX_DATA_MAP
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
	//Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

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