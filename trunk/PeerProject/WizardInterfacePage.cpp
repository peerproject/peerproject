//
// WizardInterfacePage.cpp
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "WizardInterfacePage.h"
#include "WndMain.h"
#include "Skin.h"
#include "PeerProjectURL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardInterfacePage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardInterfacePage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardInterfacePage)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardInterfacePage property page

CWizardInterfacePage::CWizardInterfacePage() : CWizardPage(CWizardInterfacePage::IDD)
{
	//{{AFX_DATA_INIT(CWizardInterfacePage)
	m_bExpert				= Settings.General.GUIMode != GUI_BASIC;
	m_bSimpleDownloadBars	= Settings.Downloads.SimpleBar;
	m_bHandleTorrents		= Settings.Web.Torrent;
	//}}AFX_DATA_INIT
}

CWizardInterfacePage::~CWizardInterfacePage()
{
}

void CWizardInterfacePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardInterfacePage)
	DDX_Control(pDX, IDC_DESCRIPTION_1, m_wndDescription1);
	DDX_Control(pDX, IDC_DESCRIPTION_0, m_wndDescription0);
	DDX_Control(pDX, IDC_INTERFACE_1, m_wndInterface1);
	DDX_Control(pDX, IDC_INTERFACE_0, m_wndInterface0);
	DDX_Radio(pDX, IDC_INTERFACE_0, m_bExpert);
	DDX_Check(pDX, IDC_DOWNLOADS_SIMPLEBAR, m_bSimpleDownloadBars);
	DDX_Check(pDX, IDC_URI_TORRENT, m_bHandleTorrents);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardInterfacePage message handlers

BOOL CWizardInterfacePage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardInterfacePage"), this );

	m_bExpert = Settings.General.GUIMode != GUI_BASIC;
	m_bSimpleDownloadBars	= Settings.Downloads.SimpleBar;
	m_bHandleTorrents		= Settings.Web.Torrent;

	UpdateData( FALSE );

	m_wndInterface0.SetFont( &theApp.m_gdiFontBold );
	m_wndInterface1.SetFont( &theApp.m_gdiFontBold );

	return TRUE;
}

BOOL CWizardInterfacePage::OnSetActive()
{
	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

void CWizardInterfacePage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rc;

	ClientToScreen( &point );

	m_wndDescription0.GetWindowRect( &rc );
	if ( rc.PtInRect( point ) )
	{
		m_wndInterface0.SetCheck( TRUE );
		m_wndInterface0.SetCheck( FALSE );
	}

	m_wndDescription1.GetWindowRect( &rc );
	if ( rc.PtInRect( point ) )
	{
		m_wndInterface0.SetCheck( FALSE );
		m_wndInterface1.SetCheck( TRUE );
	}

	CWizardPage::OnLButtonDown(nFlags, point);
}

LRESULT CWizardInterfacePage::OnWizardNext()
{
	UpdateData( TRUE );

	Settings.Downloads.SimpleBar = m_bSimpleDownloadBars != FALSE;

	if ( Settings.Web.Torrent != ( m_bHandleTorrents != FALSE ) )
	{
		Settings.Web.Torrent = m_bHandleTorrents != FALSE;
		CPeerProjectURL::Register();
	}

	CWaitCursor pCursor;
	CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();

	if ( m_bExpert && Settings.General.GUIMode != GUI_TABBED )
	{
#ifndef LAN_MODE
		Settings.BitTorrent.AdvancedInterface = TRUE;
#endif
		Settings.General.GUIMode = GUI_TABBED;
		pMainWnd->SetGUIMode( Settings.General.GUIMode, FALSE );
	}
	else if ( !m_bExpert && Settings.General.GUIMode != GUI_BASIC )
	{
		Settings.General.GUIMode = GUI_BASIC;
		pMainWnd->SetGUIMode( Settings.General.GUIMode, FALSE );
	}

	Settings.Save();

	return 0;
}
