//
// PageSettingsDC.cpp
//
// This file is part of PeerProject (peerproject.org) © 2011
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
#include "Settings.h"
#include "PeerProject.h"
#include "WndSettingsSheet.h"
#include "PageSettingsNetworks.h"
#include "PageSettingsDC.h"
#include "DlgUpdateServers.h"
#include "Downloads.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CDCSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CDCSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CDCSettingsPage)
	ON_BN_CLICKED(IDC_ENABLE_TODAY, OnEnable)
	ON_BN_CLICKED(IDC_DISCOVERY_GO, OnDiscoveryGo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDonkeySettingsPage property page

CDCSettingsPage::CDCSettingsPage() : CSettingsPage( CDCSettingsPage::IDD )
	, m_bEnabled		( FALSE )
	, m_bEnableAlways	( FALSE )
	, m_nHubs			( 0 )
{
}

CDCSettingsPage::~CDCSettingsPage()
{
}

void CDCSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDCSettingsPage)
	DDX_Check(pDX, IDC_ENABLE_TODAY, m_bEnabled);
	DDX_Check(pDX, IDC_ENABLE_ALWAYS, m_bEnableAlways);
	DDX_Control(pDX, IDC_LINKS, m_wndLinks);
	DDX_Control(pDX, IDC_LINKS_SPIN, m_wndLinksSpin);
	DDX_Control(pDX, IDC_DISCOVERY_GO, m_wndDiscoveryGo);
	DDX_Text(pDX, IDC_LINKS, m_nLinks);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDonkeySettingsPage message handlers

BOOL CDCSettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_bEnabled		= Settings.DC.Enabled;
	m_bEnableAlways	= Settings.DC.EnableAlways;
	m_nHubs		= Settings.DC.NumServers;

	UpdateData( FALSE );

//	m_wndResults.EnableWindow( m_bServerWalk );
//	Settings.SetRange( &Settings.DC.NumServers, m_wndLinksSpin );

	return TRUE;
}

BOOL CDCSettingsPage::OnSetActive()
{
	CNetworksSettingsPage* ppNetworks =
		(CNetworksSettingsPage*)GetPage( RUNTIME_CLASS(CNetworksSettingsPage) );

	if ( ppNetworks->GetSafeHwnd() != NULL )
	{
		ppNetworks->UpdateData( TRUE );
	//	m_bEnabled = ppNetworks->m_bDCEnable;
		UpdateData( FALSE );
	}

	return CSettingsPage::OnSetActive();
}

void CDCSettingsPage::OnEnable()
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
	//	ppNetworks->m_bDCEnable = m_bEnabled;
		ppNetworks->UpdateData( FALSE );
	}
}

void CDCSettingsPage::OnDiscoveryGo()
{
	// Load hublist.xml.bz2 from web various ways

	//if ( PathFileExists( Settings.General.UserPath + _T("\\Data\\hublist.xml.bz2") ) &&
	//	AfxMessageBox( L"Load local file?", MB_ICONQUESTION | MB_YESNO ) == IDYES ) )
	//{
	//	theApp.OpenImport( Settings.General.UserPath + _T("\\Data\\hublist.xml.bz2") );
	//	return;
	//}

	CUpdateServersDlg dlg;
	dlg.m_sURL = Settings.DC.HubListURL;
	if ( dlg.DoModal() != IDOK &&
		AfxMessageBox( IDS_DOWNLOAD_DC_HUBLIST, MB_ICONQUESTION | MB_YESNO ) == IDYES )
	{
		CPeerProjectURL pURL;
	//	pURL.Parse( Settings.DC.HubListURL );
		pURL.m_sURL = Settings.DC.HubListURL;
		pURL.m_sName = L"hublist.xml.bz2";
		pURL.m_nAction = CPeerProjectURL::uriDownload;
		Downloads.Add( pURL );
	}
}

void CDCSettingsPage::OnOK()
{
	UpdateData();

	Settings.DC.Enabled		= m_bEnabled && ( Settings.GetOutgoingBandwidth() >= 2 );
	Settings.DC.EnableAlways	= m_bEnableAlways && ( Settings.GetOutgoingBandwidth() >= 2 );
//	Settings.DC.NumServers		= m_nHubs;

//	Settings.Normalize( &Settings.DC.NumServers );

	// Update display in case settings were changed
//	m_nHubs = Settings.DC.NumServers;
	UpdateData( FALSE );

	CSettingsPage::OnOK();
}
