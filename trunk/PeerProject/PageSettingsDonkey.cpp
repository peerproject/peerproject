//
// PageSettingsDonkey.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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
#include "WndSettingsSheet.h"
#include "PageSettingsNetworks.h"
#include "PageSettingsDonkey.h"
#include "DlgUpdateServers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CDonkeySettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CDonkeySettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CDonkeySettingsPage)
	ON_BN_CLICKED(IDC_DISCOVERY_GO, OnDiscoveryGo)
	ON_BN_CLICKED(IDC_SERVER_WALK, OnServerWalk)
	ON_BN_CLICKED(IDC_ENABLE_TODAY, OnEnable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDonkeySettingsPage property page

CDonkeySettingsPage::CDonkeySettingsPage() : CSettingsPage( CDonkeySettingsPage::IDD )
	, m_bLearnServers	( FALSE )
	, m_nResults		( 0 )
	, m_nLinks			( 0 )
	, m_bServerWalk 	( FALSE )
	, m_bEnabled		( FALSE )
	, m_bEnableAlways	( FALSE )
{
}

CDonkeySettingsPage::~CDonkeySettingsPage()
{
}

void CDonkeySettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDonkeySettingsPage)
	DDX_Control(pDX, IDC_LINKS_SPIN, m_wndLinksSpin);
	DDX_Control(pDX, IDC_RESULTS, m_wndResults);
	DDX_Control(pDX, IDC_RESULTS_SPIN, m_wndResultsSpin);
	DDX_Control(pDX, IDC_DISCOVERY_GO, m_wndDiscoveryGo);
	DDX_Text(pDX, IDC_RESULTS, m_nResults);
	DDX_Text(pDX, IDC_LINKS, m_nLinks);
	DDX_Check(pDX, IDC_ENABLE_TODAY, m_bEnabled);
	DDX_Check(pDX, IDC_ENABLE_ALWAYS, m_bEnableAlways);
	DDX_Check(pDX, IDC_LEARN_ED2K_SERVERS, m_bLearnServers);
	DDX_Check(pDX, IDC_SERVER_WALK, m_bServerWalk);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDonkeySettingsPage message handlers

BOOL CDonkeySettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_bEnabled		= Settings.eDonkey.Enabled;
	m_bEnableAlways	= Settings.eDonkey.EnableAlways;
	m_nLinks		= Settings.eDonkey.MaxLinks;
	m_nResults		= Settings.eDonkey.MaxResults;
	m_bServerWalk	= Settings.eDonkey.ServerWalk;
	m_bLearnServers = Settings.eDonkey.LearnNewServers;

	UpdateData( FALSE );

	m_wndResults.EnableWindow( m_bServerWalk );
	Settings.SetRange( &Settings.eDonkey.MaxResults, m_wndResultsSpin );
	Settings.SetRange( &Settings.eDonkey.MaxLinks, m_wndLinksSpin );

	return TRUE;
}

BOOL CDonkeySettingsPage::OnSetActive()
{
	CNetworksSettingsPage* ppNetworks =
		(CNetworksSettingsPage*)GetPage( RUNTIME_CLASS(CNetworksSettingsPage) );

	if ( ppNetworks->GetSafeHwnd() != NULL )
	{
		ppNetworks->UpdateData( TRUE );
		m_bEnabled = ppNetworks->m_bEDEnable;
		UpdateData( FALSE );
	}

	return CSettingsPage::OnSetActive();
}

void CDonkeySettingsPage::OnEnable()
{
	UpdateData( TRUE );

	if ( m_bEnabled && ( Settings.GetOutgoingBandwidth() < 2 ) )
	{
		CString strMessage;
		LoadString( strMessage, IDS_NETWORK_BANDWIDTH_LOW );
		AfxMessageBox( strMessage, MB_OK );
		m_bEnabled = FALSE;
		UpdateData( FALSE );
	}

	CNetworksSettingsPage* ppNetworks =
		(CNetworksSettingsPage*)GetPage( RUNTIME_CLASS(CNetworksSettingsPage) );

	if ( ppNetworks->GetSafeHwnd() != NULL )
	{
		ppNetworks->UpdateData( TRUE );
		ppNetworks->m_bEDEnable = m_bEnabled;
		ppNetworks->UpdateData( FALSE );
	}
}

void CDonkeySettingsPage::OnServerWalk()
{
	UpdateData();
	m_wndResultsSpin.EnableWindow( m_bServerWalk );
	m_wndResults.EnableWindow( m_bServerWalk );
}

void CDonkeySettingsPage::OnDiscoveryGo()
{
	CUpdateServersDlg dlg;
	//dlg.m_sURL = Settings.eDonkey.ServerListURL;
	dlg.DoModal();
}

void CDonkeySettingsPage::OnOK()
{
	UpdateData();

	Settings.eDonkey.EnableAlways	= m_bEnableAlways && ( Settings.GetOutgoingBandwidth() >= 2 );
	Settings.eDonkey.Enabled		= m_bEnabled && ( Settings.GetOutgoingBandwidth() >= 2 );
	Settings.eDonkey.MaxLinks		= m_nLinks;
	Settings.eDonkey.MaxResults		= m_nResults;
	Settings.eDonkey.ServerWalk		= m_bServerWalk != FALSE;
	Settings.eDonkey.LearnNewServers = m_bLearnServers != FALSE;

	Settings.Normalize( &Settings.eDonkey.MaxResults );
	Settings.Normalize( &Settings.eDonkey.MaxLinks );

	// Update display in case settings were changed
	m_nResults = Settings.eDonkey.MaxResults;
	m_nLinks = Settings.eDonkey.MaxLinks;
	UpdateData( FALSE );

	CSettingsPage::OnOK();
}
