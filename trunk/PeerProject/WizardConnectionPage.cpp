//
// WizardConnectionPage.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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
#include "Skin.h"
#include "Network.h"
#include "WizardSheet.h"
#include "WizardConnectionPage.h"
#include "UploadQueues.h"
#include "Registry.h"
#include "DlgHelp.h"
#include "HostCache.h"
#include "DiscoveryServices.h"
#include "UPnPFinder.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardConnectionPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardConnectionPage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardConnectionPage)
	ON_CBN_SELCHANGE(IDC_CONNECTION_TYPE, OnSelChangeConnectionType)
	ON_CBN_EDITCHANGE(IDC_WIZARD_DOWNLOAD_SPEED, OnChangeConnectionSpeed)
	ON_CBN_SELCHANGE(IDC_WIZARD_DOWNLOAD_SPEED, OnChangeConnectionSpeed)
	ON_CBN_EDITCHANGE(IDC_WIZARD_UPLOAD_SPEED, OnChangeConnectionSpeed)
	ON_CBN_SELCHANGE(IDC_WIZARD_UPLOAD_SPEED, OnChangeConnectionSpeed)
	ON_CBN_SELCHANGE(IDC_WIZARD_UPNP, OnSelChangeUPnP)
	ON_BN_CLICKED(IDC_WIZARD_RANDOM, OnBnClickedRandom)
	ON_WM_XBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardConnectionPage property page

CWizardConnectionPage::CWizardConnectionPage() : CWizardPage(CWizardConnectionPage::IDD)
, m_bQueryDiscoveries(false)
, m_bUpdateDonkeyServers(false)
, m_bUPnPForward(false)
, m_bRandom(false)
, m_nPort(0)
, m_nProgressSteps(0)
{
}

CWizardConnectionPage::~CWizardConnectionPage()
{
}

void CWizardConnectionPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardConnectionPage)
	DDX_Control(pDX, IDC_CONNECTION_PROGRESS, m_wndProgress);
	DDX_Control(pDX, IDC_CONNECTION_STATUS, m_wndStatus);
	DDX_Control(pDX, IDC_CONNECTION_TYPE, m_wndType);
	DDX_Control(pDX, IDC_WIZARD_DOWNLOAD_SPEED, m_wndDownloadSpeed);
	DDX_Control(pDX, IDC_WIZARD_UPLOAD_SPEED, m_wndUploadSpeed);
	DDX_Control(pDX, IDC_WIZARD_UPNP, m_wndUPnP);
	DDX_Control(pDX, IDC_WIZARD_PORT, m_wndPort);
	DDX_Control(pDX, IDC_WIZARD_RANDOM, m_wndRandom);
	DDX_Check(pDX, IDC_WIZARD_RANDOM, m_bRandom);
	DDX_Text(pDX, IDC_WIZARD_PORT, m_nPort);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardConnectionPage message handlers

BOOL CWizardConnectionPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardConnectionPage"), this );

	CString strTemp;

	m_wndType.SetItemData( 0, 56 );		// Dial-up Modem;
	m_wndType.SetItemData( 1, 128 );	// ISDN (128K)
	m_wndType.SetItemData( 2, 768);		// DSL (768K)
	m_wndType.SetItemData( 3, 1536 );	// DSL (1.5M)
	m_wndType.SetItemData( 4, 4096 );	// DSL (4.0M)
	m_wndType.SetItemData( 5, 8192 );	// DSL2 (8.0M)
	m_wndType.SetItemData( 6, 10240 );	// FIOS (10.0M)
	m_wndType.SetItemData( 7, 12288 );	// DSL2 (12.0M)
	m_wndType.SetItemData( 8, 20480 );	// FIOS (20.0M)
	m_wndType.SetItemData( 9, 24576 );	// DSL2 (24.0M)
	m_wndType.SetItemData(10, 3100 );	// Cable Modem
	m_wndType.SetItemData(11, 1544 );	// T1
	m_wndType.SetItemData(12, 45000 );	// T3
	m_wndType.SetItemData(13, 102400 );	// LAN
	m_wndType.SetItemData(14, 155000 );	// OC3
	m_wndType.SetCurSel( -1 );
	//Dial-up Modem|ISDN (128K)|DSL (768K)|DSL (1.5M)|DSL (4.0M)|DSL2 (8.0M)|FIOS (10.0M)|DSL2 (12.0M)|FIOS (20.0M)|DSL2 (24.0M)|Cable Modem|T1|T3|LAN|OC3

	const double nSpeeds[] = { 28.8, 33.6, 56, 64, 128, 256, 384, 512, 640, 768, 1024, 1536, 1550, 2048, 3072, 4096, 5120, 7200, 8192, 10240, 12288, 16384, 20480, 24576, 45050, 102400, 0 };
	for ( int nSpeed = 0 ; nSpeeds[ nSpeed ] ; nSpeed++ )
	{
		if ( nSpeeds[ nSpeed ] < 100 )
			strTemp.Format( _T("%.1f kbps    (%.1f KB/s)"), nSpeeds[ nSpeed ], nSpeeds[ nSpeed ] / 8 );
		else if ( nSpeeds[ nSpeed ] < 8190 )
			strTemp.Format( _T("%.0f kbps    (%.0f KB/s)"), nSpeeds[ nSpeed ], nSpeeds[ nSpeed ] / 8 );
		else
			strTemp.Format( _T("%.0f kbps    (%.2f MB/s)"), nSpeeds[ nSpeed ], nSpeeds[ nSpeed ] / 8 / 1024 );
		m_wndDownloadSpeed.AddString( strTemp );
		m_wndUploadSpeed.AddString( strTemp );
	}

	strTemp.Format( _T("%lu kbps"), Settings.Connection.InSpeed );
	m_wndDownloadSpeed.SetWindowText( strTemp );
	strTemp.Format( _T("%lu kbps"), Settings.Connection.OutSpeed );
	m_wndUploadSpeed.SetWindowText( strTemp );

	LoadString( strTemp, IDS_GENERAL_YES );
	m_wndUPnP.AddString(strTemp);
	LoadString( strTemp, IDS_GENERAL_NO );
	m_wndUPnP.AddString(strTemp);
	m_wndUPnP.SetCurSel( (Settings.Connection.EnableUPnP) ? 0 : 1 );
	OnSelChangeUPnP();

	m_bRandom = ( Settings.Connection.RandomPort == true );
	m_nPort	= Settings.Connection.InPort;

	if ( m_nPort == GNUTELLA_DEFAULT_PORT )
	{
		CString sRegName = _T("InPort");
		CString sRegPath = _T("Connection");
		DWORD nPort = CRegistry::GetDword( (LPCTSTR)sRegPath, (LPCTSTR)sRegName );
		if ( nPort < 1030 )
		{
			m_nPort	= 6480;		// Substitute Non-standard Port

			// On first run, try using Shareaza's port to accomodate possible UPnP
			sRegPath = _T("Software\\Shareaza\\Shareaza\\Connection");
			DWORD nType = 0, nSize = sizeof( nPort );
			LONG nErrorCode = SHRegGetUSValue( (LPCTSTR)sRegPath, (LPCTSTR)sRegName,
				&nType, (PBYTE)&nPort, &nSize, FALSE, NULL, 0 );

			if ( nErrorCode == ERROR_SUCCESS && nType == REG_DWORD && nSize == sizeof( nPort ) )
				if ( nPort > 1030 && nPort < 65535 )
					m_nPort	= nPort;
		}
	}

	// 3 steps with 30 sub-steps each
	m_wndProgress.SetRange( 0, 90 );
	m_wndProgress.SetPos( 0 );

	m_wndStatus.SetWindowText( L"" );

	UpdateData( FALSE );

	return TRUE;
}

BOOL CWizardConnectionPage::OnSetActive()
{
	//Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	m_wndProgress.SetPos( 0 );
	m_wndStatus.SetWindowText( L"" );
	return CWizardPage::OnSetActive();
}

void CWizardConnectionPage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

void CWizardConnectionPage::OnSelChangeConnectionType()
{
	m_wndDownloadSpeed.SetWindowText( _T("") );
	m_wndUploadSpeed.SetWindowText( _T("") );
}

void CWizardConnectionPage::OnChangeConnectionSpeed()
{
	m_wndType.SetCurSel( -1 );
}

void CWizardConnectionPage::OnSelChangeUPnP()
{
		int nIndex = m_wndUPnP.GetCurSel();

		if ( nIndex == 0 )
		{
			m_bUPnPForward = TRUE;
			m_wndRandom.EnableWindow( TRUE );
			m_wndPort.EnableWindow( ! m_bRandom );
		}
		else
		{
			m_bUPnPForward = FALSE;
			m_wndRandom.EnableWindow( FALSE );
			m_wndPort.EnableWindow( TRUE );
		}
}

void CWizardConnectionPage::OnBnClickedRandom()
{
	UpdateData();
	m_wndPort.EnableWindow( ! m_bRandom );
}

LRESULT CWizardConnectionPage::OnWizardNext()
{
	if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) return 0;

	UpdateData();

	if ( m_nPort > 1022 && m_nPort < 65535 )
		Settings.Connection.InPort = m_nPort;
	else
		AfxMessageBox( L"Port number ignored.  ( Use 1030 - 65530 )" );
	Settings.Connection.RandomPort = ( m_bRandom == TRUE );

	DWORD nSpeed = 0, nDownloadSpeed = 0, nUploadSpeed = 0;
	int nIndex = m_wndType.GetCurSel();

	if ( nIndex >= 0 )
	{
		nSpeed = static_cast< DWORD >( m_wndType.GetItemData( nIndex ) );
		nDownloadSpeed = nSpeed;

		if( nSpeed <= 56 )								// Dial up modem
			nUploadSpeed = 32;
		else if( nSpeed <= 128 )						// ISDN
			nUploadSpeed = nSpeed;
		else if( nSpeed == 384 )						// 384/128 DSL (Europe)
			nUploadSpeed = 128;
		else if( nSpeed <= 700 )						// ADSL (4:1)
			nUploadSpeed = nSpeed / 4;
		else if( nSpeed <  1544 )						// ADSL (6:1)
			nUploadSpeed = nSpeed / 6;
		else if( nSpeed == 1544 )						// T1 (1:1)
			nUploadSpeed = nSpeed;
		else if( nSpeed <= 4000 )						// Cable (2:1)
			nUploadSpeed = nSpeed / 2;
		else if( nSpeed <= 8192 )						// ADSL2 (8:1)
			nUploadSpeed = nSpeed / 8;
		else if( nSpeed <= 12288 )						// ADSL2 (10:1)
			nUploadSpeed = nSpeed / 10;
		else if( nSpeed <= 24576 )						// ADSL2+ (12:1)
			nUploadSpeed = nSpeed / 12;
		else											// High capacity lines. (LAN, etc)
			nUploadSpeed = nSpeed;
	}
	else
	{
		CString strSpeed;
		double nTemp;

		m_wndDownloadSpeed.GetWindowText( strSpeed );
		if ( _stscanf( strSpeed, _T("%lf"), &nTemp ) == 1 )
			nDownloadSpeed = (DWORD)nTemp;

		m_wndUploadSpeed.GetWindowText( strSpeed );
		if ( _stscanf( strSpeed, _T("%lf"), &nTemp ) == 1 )
			nUploadSpeed = (DWORD)nTemp;
	}

	if ( nDownloadSpeed <= 0 || nUploadSpeed <= 0 )
	{
		CString strSpeed;
		LoadString( strSpeed, IDS_WIZARD_NEED_SPEED );
		AfxMessageBox( strSpeed, MB_ICONEXCLAMATION );
		return -1;
	}

	Settings.Connection.InSpeed = nDownloadSpeed;
	Settings.Connection.OutSpeed = nUploadSpeed;

	// Set upload limit to 90% of capacity, trimmed down to nearest KB.
	Settings.Bandwidth.Uploads = ( Settings.Connection.OutSpeed / 8 ) *
		( ( 100 - Settings.Uploads.FreeBandwidthFactor ) / 100 ) * 1024;

	Settings.eDonkey.MaxLinks = nSpeed < 100 ? 100 : 250;
	Settings.OnChangeConnectionSpeed();
	UploadQueues.CreateDefault();

	//if ( theApp.m_bLimitedConnections && !Settings.General.IgnoreXPsp2 )
	//	CHelpDlg::Show( _T("GeneralHelp.XPsp2") );

	m_nProgressSteps = 0;

	// Load default ed2k server list (if necessary)
	m_bUpdateDonkeyServers = true;
	m_nProgressSteps += 30;

	// Update the G1, G2 and eDonkey host cache (if necessary)
	m_bQueryDiscoveries = true;
	m_nProgressSteps += 30;

	CWaitCursor pCursor;
	if ( m_bUPnPForward )
	{
		m_nProgressSteps += 30;	// UPnP device detection

		// Create UPnP finder object if it doesn't exist
		if ( !theApp.m_pUPnPFinder )
			theApp.m_pUPnPFinder.Attach( new CUPnPFinder );
		if ( theApp.m_pUPnPFinder->AreServicesHealthy() )
			theApp.m_pUPnPFinder->StartDiscovery();
	}

	BeginThread( "WizardConnectionPage" );

	// Disable all navigation buttons while the thread is running
	CWizardSheet* pSheet = GetSheet();
	if ( pSheet->GetDlgItem( ID_WIZBACK ) )
		pSheet->GetDlgItem( ID_WIZBACK )->EnableWindow( FALSE );
	if ( pSheet->GetDlgItem( ID_WIZNEXT ) )
		pSheet->GetDlgItem( ID_WIZNEXT )->EnableWindow( FALSE );
	if ( pSheet->GetDlgItem( 2 ) )
		pSheet->GetDlgItem( 2 )->EnableWindow( FALSE );
	return -1; // don't move to the next page; the thread will do this work
}

/////////////////////////////////////////////////////////////////////////////
// CWizardConnectionPage thread

void CWizardConnectionPage::OnRun()
{
	short nCurrentStep = 0;
	CString strMessage;

	m_wndProgress.PostMessage( PBM_SETRANGE32, 0, (LPARAM)m_nProgressSteps );

	if ( m_bUPnPForward )
	{
		LoadString( strMessage, IDS_WIZARD_UPNP_SETUP );
		m_wndStatus.SetWindowText( strMessage );

		while ( theApp.m_pUPnPFinder &&
				theApp.m_pUPnPFinder->IsAsyncFindRunning() )
		{
			Sleep( 1000 );
			if ( nCurrentStep < 30  )
				nCurrentStep++;
			else if ( nCurrentStep == 30 )
				nCurrentStep = 0;
			m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
		}

		nCurrentStep = 30;
		m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
	}

	if ( m_bUpdateDonkeyServers )
	{
		LoadString( strMessage, IDS_WIZARD_ED2K );
		m_wndStatus.SetWindowText( strMessage );

		HostCache.CheckMinimumED2KServers();
		nCurrentStep +=30;
		m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
	}

	if ( m_bQueryDiscoveries )
	{
		LoadString( strMessage, IDS_WIZARD_DISCOVERY );
		m_wndStatus.SetWindowText( strMessage );

		DiscoveryServices.CheckMinimumServices();
		nCurrentStep +=15;
		m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );

		BOOL bConnected = Network.IsConnected();
		if ( bConnected || Network.Connect(TRUE) )
		{
			int i;
			// It will be checked if it is needed inside DiscoveryServices.Execute()
			for ( i = 0; i < 2 && !DiscoveryServices.Execute(TRUE, PROTOCOL_G1, 2); i++ ) Sleep(200);
			nCurrentStep += 5;
			m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
			for ( i = 0; i < 2 && !DiscoveryServices.Execute(TRUE, PROTOCOL_G2, 2); i++ ) Sleep(200);
			nCurrentStep += 5;
			m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
			for ( i = 0; i < 2 && !DiscoveryServices.Execute(TRUE, PROTOCOL_ED2K, 2); i++ ) Sleep(200);
			nCurrentStep += 5;
			m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );

			if ( !bConnected )
				Network.Disconnect();
		}
		else
		{
			nCurrentStep += 15;
			m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
		}
	}

	CWizardSheet* pSheet = GetSheet();
	if ( pSheet->GetDlgItem( ID_WIZBACK ) )
		pSheet->GetDlgItem( ID_WIZBACK )->EnableWindow();
	if ( pSheet->GetDlgItem( ID_WIZNEXT ) )
		pSheet->GetDlgItem( ID_WIZNEXT )->EnableWindow();
	if ( pSheet->GetDlgItem( 2 ) )
		pSheet->GetDlgItem( 2 )->EnableWindow();

	pSheet->SendMessage( PSM_SETCURSEL, 2, 0 );	// Go to the 3rd page
	PostMessage( WM_TIMER, 1 );					// Terminate thread if necessarily
}

BOOL CWizardConnectionPage::OnQueryCancel()
{
	if ( IsThreadAlive() )
		return FALSE;

	return CWizardPage::OnQueryCancel();
}

void CWizardConnectionPage::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent != 1 ) return;

	CloseThread();

	if ( m_bUPnPForward && theApp.m_bUPnPPortsForwarded != TRI_TRUE )
	{
		CString strFormat, strMessage;
		LoadString( strFormat, IDS_WIZARD_PORT_FORWARD );
		strMessage.Format( strFormat, Settings.Connection.InPort );
		AfxMessageBox( strMessage, MB_ICONINFORMATION );
	}
}
