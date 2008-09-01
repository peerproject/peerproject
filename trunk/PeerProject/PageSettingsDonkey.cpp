//
// PageSettingsDonkey.cpp
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
#include "WndSettingsSheet.h"
#include "PageSettingsNetworks.h"
#include "PageSettingsDonkey.h"
#include "DlgDonkeyServers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDonkeySettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CDonkeySettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CDonkeySettingsPage)
	ON_BN_CLICKED(IDC_DISCOVERY_GO, OnDiscoveryGo)
	ON_BN_CLICKED(IDC_SERVER_WALK, OnServerWalk)
	ON_BN_CLICKED(IDC_ENABLE_TODAY, OnEnableToday)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDonkeySettingsPage property page

CDonkeySettingsPage::CDonkeySettingsPage() : CSettingsPage( CDonkeySettingsPage::IDD )
, m_bLearnServers(FALSE)
, m_nResults(0)
, m_bServerWalk(FALSE)
, m_nLinks(0)
, m_bEnableToday(FALSE)
, m_bEnableAlways(FALSE)
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
	DDX_Check(pDX, IDC_SERVER_WALK, m_bServerWalk);
	DDX_Text(pDX, IDC_LINKS, m_nLinks);
	DDX_Check(pDX, IDC_ENABLE_TODAY, m_bEnableToday);
	DDX_Check(pDX, IDC_ENABLE_ALWAYS, m_bEnableAlways);
	DDX_Check(pDX, IDC_LEARN_ED2K_SERVERS, m_bLearnServers);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDonkeySettingsPage message handlers

BOOL CDonkeySettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_bEnableToday	= Settings.eDonkey.EnableToday;
	m_bEnableAlways	= Settings.eDonkey.EnableAlways;
	m_nLinks		= Settings.eDonkey.MaxLinks;
	m_bServerWalk	= Settings.eDonkey.ServerWalk;
	m_nResults		= Settings.eDonkey.MaxResults;
	m_bLearnServers = Settings.eDonkey.LearnNewServers;

	UpdateData( FALSE );

	m_wndResults.EnableWindow( m_bServerWalk );
	m_wndResultsSpin.SetRange( 0, 201 );
	m_wndLinksSpin.SetRange( 0, 2048 );

	return TRUE;
}

BOOL CDonkeySettingsPage::OnSetActive()
{
	CNetworksSettingsPage* ppNetworks =
		(CNetworksSettingsPage*)GetPage( RUNTIME_CLASS(CNetworksSettingsPage) );

	if ( ppNetworks->GetSafeHwnd() != NULL )
	{
		ppNetworks->UpdateData( TRUE );
		m_bEnableToday = ppNetworks->m_bEDEnable;
		UpdateData( FALSE );
	}

	return CSettingsPage::OnSetActive();
}

void CDonkeySettingsPage::OnEnableToday()
{
	UpdateData( TRUE );

	if ( m_bEnableToday && ( Settings.GetOutgoingBandwidth() < 2 ) )
	{
		CString strMessage;
		LoadString( strMessage, IDS_NETWORK_BANDWIDTH_LOW );
		AfxMessageBox( strMessage, MB_OK );
		m_bEnableToday = FALSE;
		UpdateData( FALSE );
	}

	CNetworksSettingsPage* ppNetworks =
		(CNetworksSettingsPage*)GetPage( RUNTIME_CLASS(CNetworksSettingsPage) );

	if ( ppNetworks->GetSafeHwnd() != NULL )
	{
		ppNetworks->UpdateData( TRUE );
		ppNetworks->m_bEDEnable = m_bEnableToday;
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
	CDonkeyServersDlg dlg;
	dlg.DoModal();
}

void CDonkeySettingsPage::OnOK()
{
	UpdateData();

	Settings.eDonkey.EnableAlways	= m_bEnableAlways && ( Settings.GetOutgoingBandwidth() >= 2 );
	Settings.eDonkey.EnableToday	= m_bEnableToday && ( Settings.GetOutgoingBandwidth() >= 2 );
	Settings.eDonkey.MaxLinks		= m_nLinks;
	Settings.eDonkey.ServerWalk		= m_bServerWalk != FALSE;
	Settings.eDonkey.MaxResults		= m_nResults;
	Settings.eDonkey.LearnNewServers = m_bLearnServers != FALSE;

	CSettingsPage::OnOK();
}
