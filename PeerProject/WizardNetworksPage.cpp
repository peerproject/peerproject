//
// WizardNetworksPage.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "WizardNetworksPage.h"
#include "Network.h"
#include "HostCache.h"
#include "PeerProjectURL.h"
#include "DlgDonkeyServers.h"
#include "DlgHelp.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CWizardNetworksPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardNetworksPage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardNetworksPage)
	ON_WM_XBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardNetworksPage property page

CWizardNetworksPage::CWizardNetworksPage()
	: CWizardPage(CWizardNetworksPage::IDD)
	, m_bG2Enable	( TRUE )
	, m_bG1Enable	( TRUE )
	, m_bEDEnable	( TRUE )
	, m_bHandleTorrents	( TRUE )
{
}

CWizardNetworksPage::~CWizardNetworksPage()
{
}

void CWizardNetworksPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardNetworksPage)
	DDX_Check(pDX, IDC_G2_ENABLE, m_bG2Enable);
	DDX_Check(pDX, IDC_G1_ENABLE, m_bG1Enable);
	DDX_Check(pDX, IDC_ED2K_ENABLE, m_bEDEnable);
	DDX_Check(pDX, IDC_URI_TORRENT, m_bHandleTorrents);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardNetworksPage message handlers

BOOL CWizardNetworksPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardNetworksPage"), this );

	m_bG2Enable = Settings.Gnutella2.EnableAlways;
	m_bG1Enable = Settings.Gnutella1.EnableAlways;
	m_bEDEnable = Settings.eDonkey.EnableAlways;
	m_bHandleTorrents = Settings.Web.Torrent;

#ifdef LAN_MODE
	GetDlgItem( IDC_G1_ENABLE )->EnableWindow( FALSE );
	GetDlgItem( IDC_ED2K_ENABLE )->EnableWindow( FALSE );
#endif // LAN_MOD

	UpdateData( FALSE );

	return TRUE;
}

BOOL CWizardNetworksPage::OnSetActive()
{
	//Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

void CWizardNetworksPage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

LRESULT CWizardNetworksPage::OnWizardNext()
{
	UpdateData();

	if ( ! m_bG2Enable )
	{
		CString strMessage;
		LoadString( strMessage, IDS_NETWORK_DISABLE_G2 );

		if ( AfxMessageBox( strMessage, MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2 ) != IDYES )
		{
			m_bG2Enable = TRUE;
			UpdateData( FALSE );
		}
	}

	if ( Settings.Web.Torrent != ( m_bHandleTorrents != FALSE ) )
	{
		Settings.Web.Torrent = m_bHandleTorrents != FALSE;
		CPeerProjectURL::Register();
	}

	Settings.Gnutella2.EnableAlways	= m_bG2Enable != FALSE;
	Settings.Gnutella2.EnableToday	= m_bG2Enable != FALSE;
	Settings.Gnutella1.EnableAlways	= m_bG1Enable != FALSE;
	Settings.Gnutella1.EnableToday	= m_bG1Enable != FALSE;
	Settings.eDonkey.EnableAlways	= m_bEDEnable != FALSE;
	Settings.eDonkey.EnableToday	= m_bEDEnable != FALSE;

	if ( m_bEDEnable && HostCache.eDonkey.GetCount() < 3 )
	{
		CDonkeyServersDlg dlg;
		dlg.DoModal();
	}

	if ( ! Network.IsConnected() && ( m_bG2Enable || m_bG1Enable || m_bEDEnable ) )
		Network.Connect( TRUE );

	return 0;
}
