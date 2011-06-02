//
// WizardInterfacePage.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
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
#include "WizardInterfacePage.h"
#include "WndMain.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CWizardInterfacePage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardInterfacePage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardInterfacePage)
	ON_WM_LBUTTONDOWN()
	ON_WM_XBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardInterfacePage property page

CWizardInterfacePage::CWizardInterfacePage() : CWizardPage(CWizardInterfacePage::IDD)
{
	//{{AFX_DATA_INIT(CWizardInterfacePage)
	m_bExpert = Settings.General.GUIMode != GUI_BASIC;
	m_bSimpleDownloadBars = Settings.Downloads.SimpleBar;
	//}}AFX_DATA_INIT
}

CWizardInterfacePage::~CWizardInterfacePage()
{
}

void CWizardInterfacePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardInterfacePage)
	DDX_Control(pDX, IDC_DESCRIPTION_EXPERT, m_wndDescriptionExpert);
	DDX_Control(pDX, IDC_DESCRIPTION_BASIC, m_wndDescriptionBasic);
	DDX_Control(pDX, IDC_INTERFACE_EXPERT, m_wndInterfaceExpert);
	DDX_Control(pDX, IDC_INTERFACE_BASIC, m_wndInterfaceBasic);
	DDX_Radio(pDX, IDC_INTERFACE_BASIC, m_bExpert);
	DDX_Check(pDX, IDC_DOWNLOADS_SIMPLEBAR, m_bSimpleDownloadBars);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardInterfacePage message handlers

BOOL CWizardInterfacePage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardInterfacePage"), this );

	m_bExpert = Settings.General.GUIMode != GUI_BASIC;
	m_bSimpleDownloadBars = Settings.Downloads.SimpleBar;

	UpdateData( FALSE );

	m_wndInterfaceBasic.SetFont( &theApp.m_gdiFontBold );
	m_wndInterfaceExpert.SetFont( &theApp.m_gdiFontBold );

	return TRUE;
}

BOOL CWizardInterfacePage::OnSetActive()
{
	// Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

void CWizardInterfacePage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

void CWizardInterfacePage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rc;
	ClientToScreen( &point );

	// Select mode by clicking any text
	m_wndDescriptionBasic.GetWindowRect( &rc );
	if ( rc.PtInRect( point ) )
	{
		m_wndInterfaceBasic.SetCheck( TRUE );
		m_wndInterfaceExpert.SetCheck( FALSE );
	}
	else
	{
		m_wndDescriptionExpert.GetWindowRect( &rc );
		if ( rc.PtInRect( point ) )
		{
			m_wndInterfaceExpert.SetCheck( TRUE );
			m_wndInterfaceBasic.SetCheck( FALSE );
		}
	}

	CWizardPage::OnLButtonDown(nFlags, point);
}

LRESULT CWizardInterfacePage::OnWizardNext()
{
	UpdateData( TRUE );

	Settings.Downloads.SimpleBar = m_bSimpleDownloadBars != FALSE;

	if ( m_bExpert && Settings.General.GUIMode == GUI_BASIC )
	{
		CWaitCursor pCursor;
		CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();

		Settings.General.GUIMode = GUI_TABBED;
		Settings.Interface.RowSize = 17;
		pMainWnd->SetGUIMode( Settings.General.GUIMode, FALSE );
	}
	else if ( ! m_bExpert && Settings.General.GUIMode != GUI_BASIC )
	{
		CWaitCursor pCursor;
		CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();

		Settings.General.GUIMode = GUI_BASIC;
		Settings.Interface.RowSize = 18;
		pMainWnd->SetGUIMode( Settings.General.GUIMode, FALSE );
	}

	Settings.Save();

	return 0;
}