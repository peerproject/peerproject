//
// WizardConnectionPage.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "WizardConnectionPage.h"
//#include "WizardSheet.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Skin.h"
#include "Network.h"
#include "Registry.h"
#include "HostCache.h"
#include "UploadQueues.h"
#include "DiscoveryServices.h"
#include "DlgHelp.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CWizardConnectionPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardConnectionPage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardConnectionPage)
	ON_CBN_SELCHANGE(IDC_CONNECTION_TYPE, &CWizardConnectionPage::OnSelChangeConnectionType)
	ON_CBN_EDITCHANGE(IDC_WIZARD_DOWNLOAD_SPEED, &CWizardConnectionPage::OnChangeConnectionSpeed)
	ON_CBN_SELCHANGE(IDC_WIZARD_DOWNLOAD_SPEED, &CWizardConnectionPage::OnChangeConnectionSpeed)
	ON_CBN_EDITCHANGE(IDC_WIZARD_UPLOAD_SPEED, &CWizardConnectionPage::OnChangeConnectionSpeed)
	ON_CBN_SELCHANGE(IDC_WIZARD_UPLOAD_SPEED, &CWizardConnectionPage::OnChangeConnectionSpeed)
	ON_BN_CLICKED(IDC_WIZARD_RANDOM, &CWizardConnectionPage::OnBnClickedRandom)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_XBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardConnectionPage property page

CWizardConnectionPage::CWizardConnectionPage()
	: CWizardPage(CWizardConnectionPage::IDD)
	, m_bQueryDiscoveries	( false )
	, m_bUpdateServers		( false )
	, m_bRandom 			( false )
	, m_nPort				( 0 )
	, m_nProgressSteps		( 0 )
{
}

CWizardConnectionPage::~CWizardConnectionPage()
{
}

void CWizardConnectionPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardConnectionPage)
	DDX_Control(pDX, IDC_WEB, m_wndTest);
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

	// Set dropdown download values:  (Text via resource editor)
	m_wndType.SetItemData( 0, 56 );		// Dial-up Modem
	m_wndType.SetItemData( 1, 128 );	// ISDN 128K
	m_wndType.SetItemData( 2, 768 );	// DSL  768K
	m_wndType.SetItemData( 3, 1536 );	// DSL  1.5
	m_wndType.SetItemData( 4, 3100 );	// Cable 3
	m_wndType.SetItemData( 5, 4096 );	// DSL  4
	m_wndType.SetItemData( 6, 8192 );	// DSL2 8
	m_wndType.SetItemData( 7, 10240 );	// FIOS 10
	m_wndType.SetItemData( 8, 12288 );	// DSL2 12
	m_wndType.SetItemData( 9, 16384 );	// FIOS 15
	m_wndType.SetItemData(10, 20480 );	// FIOS 20
	m_wndType.SetItemData(11, 25400 );	// FIOS 25
	m_wndType.SetItemData(12, 30720 );	// FIOS 30
	m_wndType.SetItemData(13, 50800 );	// FIOS 50
	m_wndType.SetItemData(14, 1544 );	// T1
	m_wndType.SetItemData(15, 44800 );	// T3
	m_wndType.SetItemData(16, 102400 );	// LAN
	m_wndType.SetItemData(17, 155000 );	// OC3
	m_wndType.SetCurSel( -1 );

	// Set corresponding uploads:  (Must match above)
	m_mapSpeed[ 56 ]		= 36;		// Dial-up Modem
	m_mapSpeed[ 128 ]		= 128;		// ISDN 128K
	m_mapSpeed[ 768 ]		= 128;		// DSL  768K
	m_mapSpeed[ 1536 ]		= 256;		// DSL  1.5
	m_mapSpeed[ 4096 ]		= 768;		// DSL  4
	m_mapSpeed[ 3100 ]		= 2040;		// Cable 3
	m_mapSpeed[ 8192 ]		= 1024;		// DSL2 8
	m_mapSpeed[ 10240 ]		= 2048;		// FIOS 10
	m_mapSpeed[ 12288 ]		= 2048;		// DSL2 12
	m_mapSpeed[ 16384 ]		= 5120;		// FIOS 15
	m_mapSpeed[ 20480 ]		= 5120;		// FIOS 20
	m_mapSpeed[ 25400 ]		= 10240;	// FIOS 25
	m_mapSpeed[ 30720 ]		= 10240;	// FIOS 30
	m_mapSpeed[ 50800 ]		= 20480;	// FIOS 50
	m_mapSpeed[ 1544 ]		= 1544; 	// T1
	m_mapSpeed[ 44800 ]		= 44800;	// T3
	m_mapSpeed[ 102400 ]	= 102400;	// LAN
	m_mapSpeed[ 155000 ]	= 155000;	// OC3

	// Translation: |Dial-up Modem|ISDN 128K|DSL 768K|DSL 1.5|Cable 3|DSL 4|DSL2 8|FIOS 10|DSL2 12|FIOS 15|FIOS 20|FIOS 25|FIOS 30|FIOS 50|T1|T3|LAN|OC3

	const double nSpeeds[] = { 28.8, 33.6, 56, 64, 128, 256, 384, 512, 640, 768, 1024, 1536, 1550, 1760, 2048, 3072, 4096, 5120, 7200, 8192, 10240, 12288, 16384, 20480, 24576, 30720, 45050, 50800, 102400, 0 };
	for ( int nSpeed = 0 ; nSpeeds[ nSpeed ] ; nSpeed++ )
	{
		// Populate "0000 kbps  (0.00 MB/s)"
		strTemp = SpeedFormat( nSpeeds[ nSpeed ] );
		m_wndDownloadSpeed.AddString( strTemp );
		m_wndUploadSpeed.AddString( strTemp );
	}

	strTemp.Format( _T("%lu kbps"), Settings.Connection.InSpeed );
	m_wndDownloadSpeed.SetWindowText( strTemp );
	strTemp.Format( _T("%lu kbps"), Settings.Connection.OutSpeed );
	m_wndUploadSpeed.SetWindowText( strTemp );

	m_wndUPnP.AddString( LoadString( IDS_GENERAL_YES ) );
	m_wndUPnP.AddString( LoadString( IDS_GENERAL_NO ) );
	m_wndUPnP.SetCurSel( Settings.Connection.EnableUPnP ? 0 : 1 );

	m_bRandom = ( Settings.Connection.RandomPort == true );
	m_nPort	= Settings.Connection.InPort;

	if ( Settings.Live.FirstRun && m_nPort == protocolPorts[ PROTOCOL_G2 ] )
	{
		m_nPort	= protocolPorts[ PROTOCOL_NULL ];		// Substitute Non-standard Port (6480)

		// Obsolete check:
		//CString strRegName = _T("InPort");
		//CString strRegPath = _T("Connection");
		//DWORD nPort = CRegistry::GetDword( (LPCTSTR)strRegPath, (LPCTSTR)strRegName );

		// Initially try Shareaza's port to accomodate possible existing port-forwarding (with conflict)
		const CString strRegPath = _T("Software\\Shareaza\\Shareaza\\Connection");
		DWORD nType = 0, nEnabled = 1, nPort, nSize = sizeof( m_nPort );

		CString strRegName = _T("EnableUPnP");
		LONG nErrorCode = SHRegGetUSValue( (LPCTSTR)strRegPath, (LPCTSTR)strRegName,
			&nType, (PBYTE)&nEnabled, &nSize, FALSE, NULL, 0 );

		if ( nErrorCode == ERROR_SUCCESS && nEnabled == 0 )	// Plug'n'Play disabled, assume deliberately
		{
			strRegName = _T("InPort");
			nErrorCode = SHRegGetUSValue( (LPCTSTR)strRegPath, (LPCTSTR)strRegName,
				&nType, (PBYTE)&nPort, &nSize, FALSE, NULL, 0 );

			if ( nErrorCode == ERROR_SUCCESS && nPort > 1030 && nPort < 65535 ) 	//&& nType == REG_DWORD && nSize == sizeof( nPort ) )
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
	// Wizard Window Caption Workaround
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
//	m_wndDownloadSpeed.SetWindowText( _T("") );
//	m_wndUploadSpeed.SetWindowText( _T("") );

	const int nIndex = m_wndType.GetCurSel();
	if ( nIndex < 0 )
		return;

	const DWORD nSpeed = static_cast< DWORD >( m_wndType.GetItemData( nIndex ) );

	m_wndDownloadSpeed.SetWindowText( SpeedFormat( (double)nSpeed ) );
	m_wndUploadSpeed.SetWindowText( SpeedFormat( (double)m_mapSpeed[ nSpeed ] ) );
}

void CWizardConnectionPage::OnChangeConnectionSpeed()
{
	m_wndType.SetCurSel( -1 );
}

// Obsolete for reference & deletion
//void CWizardConnectionPage::OnSelChangeUPnP()
//{
//	if ( m_wndUPnP.GetCurSel() == 0 )	// Index
//	{
//		m_bUPnPForward = TRUE;
//		m_wndRandom.EnableWindow( TRUE );
//		m_wndPort.EnableWindow( ! m_bRandom );
//	}
//	else
//	{
//		m_bUPnPForward = FALSE;
//		m_wndRandom.EnableWindow( FALSE );
//		m_wndPort.EnableWindow( TRUE );
//	}
//}

void CWizardConnectionPage::OnBnClickedRandom()
{
	UpdateData();
	m_wndPort.EnableWindow( ! m_bRandom );
}

LRESULT CWizardConnectionPage::OnWizardNext()
{
	if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) return 0;

	CWaitCursor pCursor;

	UpdateData();

	if ( m_nPort > 1022 && m_nPort < 65536 )
		Settings.Connection.InPort = m_nPort;
	else
		MsgBox( L"Port number ignored.  (Use 1030-65530)" );
	Settings.Connection.RandomPort = ( m_bRandom == TRUE );
	Settings.Connection.EnableUPnP = ( m_wndUPnP.GetCurSel() == 0 );

	DWORD nDownloadSpeed = 0, nUploadSpeed = 0;
	const int nIndex = m_wndType.GetCurSel();

	if ( nIndex >= 0 )
	{
		nDownloadSpeed = static_cast< DWORD >( m_wndType.GetItemData( nIndex ) );
		nUploadSpeed = m_mapSpeed[ nDownloadSpeed ];
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

	if ( nDownloadSpeed < 2 || nUploadSpeed < 2 )
	{
		MsgBox( IDS_WIZARD_NEED_SPEED, MB_ICONEXCLAMATION );
		return -1;
	}

	Settings.Connection.InSpeed = nDownloadSpeed;
	Settings.Connection.OutSpeed = nUploadSpeed;

	//if ( Settings.Experimental.LAN_Mode )
	//{
	//	Settings.Connection.InSpeed = 40960;
	//	Settings.Connection.OutSpeed = 40960;
	//}

	// Set upload limit to 90% of capacity, trimmed down to nearest KB.
	Settings.Bandwidth.Uploads = ( Settings.Connection.OutSpeed / 8 ) *
		( ( 100 - Settings.Uploads.FreeBandwidthFactor ) / 100 ) * 1024;

	Settings.eDonkey.MaxLinks = nUploadSpeed < 130 ? 100 : 250;
	Settings.OnChangeConnectionSpeed();
	UploadQueues.CreateDefault();

	//if ( theApp.m_bLimitedConnections && ! Settings.General.IgnoreXPsp2 )
	//	CHelpDlg::Show( _T("GeneralHelp.XPsp2") );

	m_nProgressSteps = 0;

	// Load default ed2k server list (if necessary)
	m_bUpdateServers = true;
	m_nProgressSteps += 30;

	// Update the G1, G2 and eDonkey host cache (if necessary)
	m_bQueryDiscoveries = true;
	m_nProgressSteps += 30;

	// Obsolete for reference & deletion
	//CWaitCursor pCursor;
	//if ( m_bUPnPForward )
	//{
	//	Settings.Connection.EnableUPnP = true;
	//
	//	m_nProgressSteps += 30;		// UPnP device detection
	//
	//	//Network.MapPorts();
	//
	//	// Create UPnP finder object if it doesn't exist
	//	//if ( ! Network.UPnPFinder )
	//	//	Network.UPnPFinder.Attach( new CUPnPFinder );
	//	//if ( Network.UPnPFinder->AreServicesHealthy() )
	//	//	Network.UPnPFinder->StartDiscovery();
	//}
	//else if ( m_wndUPnP.GetCurSel() == 1 )
	//{
	//	Settings.Connection.EnableUPnP = false;
	//}

	BeginThread( "WizardConnectionPage" );

	// Disable all navigation buttons while the thread is running
	CWizardSheet* pSheet = GetSheet();
	if ( pSheet->GetDlgItem( ID_WIZBACK ) )
		pSheet->GetDlgItem( ID_WIZBACK )->EnableWindow( FALSE );
	if ( pSheet->GetDlgItem( ID_WIZNEXT ) )
		pSheet->GetDlgItem( ID_WIZNEXT )->EnableWindow( FALSE );
	if ( pSheet->GetDlgItem( 2 ) )
		pSheet->GetDlgItem( 2 )->EnableWindow( FALSE );

	return -1;	// Don't move to the next page; the thread will do this work
}

/////////////////////////////////////////////////////////////////////////////
// CWizardConnectionPage thread

void CWizardConnectionPage::OnRun()
{
	short nCurrentStep = 0;

	m_wndProgress.PostMessage( PBM_SETRANGE32, 0, (LPARAM)m_nProgressSteps );

	// Obsolete for reference & deletion
	//if ( m_bUPnPForward )
	//{
	//	m_wndStatus.SetWindowText( LoadString( IDS_WIZARD_UPNP_SETUP ) );
	//
	//	while ( Network.UPnPFinder &&
	//			Network.UPnPFinder->IsAsyncFindRunning() )
	//	{
	//		Sleep( 1000 );
	//		if ( nCurrentStep < 30 )
	//			nCurrentStep++;
	//		else if ( nCurrentStep == 30 )
	//			nCurrentStep = 0;
	//		m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
	//	}
	//
	//	nCurrentStep = 30;
	//	m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
	//}

	if ( m_bUpdateServers )
	{
		m_wndStatus.SetWindowText( LoadString( IDS_WIZARD_ED2K ) );

		HostCache.CheckMinimumServers( PROTOCOL_ED2K );
		nCurrentStep += 30;
		m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
		Sleep( 10 );	// Mixed text bugfix?
		m_wndStatus.SetWindowText( _T("") );
	}

	if ( m_bQueryDiscoveries )
	{
		m_wndStatus.SetWindowText( LoadString( IDS_WIZARD_DISCOVERY ) );

		DiscoveryServices.CheckMinimumServices();
		nCurrentStep += 15;
		m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );

		BOOL bConnected = Network.IsConnected();
		if ( bConnected || Network.Connect(TRUE) )
		{
			int i;
			// It will be checked if it is needed inside DiscoveryServices.Execute()
			for ( i = 0 ; i < 2 && ! DiscoveryServices.Execute(TRUE, PROTOCOL_G1, 2) ; i++ ) Sleep(200);
			nCurrentStep += 5;
			m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
			for ( i = 0 ; i < 2 && ! DiscoveryServices.Execute(TRUE, PROTOCOL_G2, 2) ; i++ ) Sleep(200);
			nCurrentStep += 5;
			m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );
			for ( i = 0 ; i < 2 && ! DiscoveryServices.Execute(TRUE, PROTOCOL_ED2K, 2) ; i++ ) Sleep(200);
			nCurrentStep += 5;
			m_wndProgress.PostMessage( PBM_SETPOS, nCurrentStep );

		//	if ( ! bConnected ) Network.Disconnect();		// Caused UPnP false fail on first run?
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
	PostMessage( WM_TIMER, 1 );					// Terminate thread if necessary
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

	// Obsolete for reference & deletion
	//if ( m_bUPnPForward && Network.m_bUPnPPortsForwarded != TRI_TRUE )
	//{
	//	CString strMessage;
	//	strMessage.Format( LoadString( IDS_WIZARD_PORT_FORWARD ), Settings.Connection.InPort );
	//	MsgBox( strMessage, MB_ICONINFORMATION );
	//}
}

CString CWizardConnectionPage::SpeedFormat(const double nSpeed) const
{
	CString strSpeed;

	if ( nSpeed < 100 )
		strSpeed.Format( _T("%.1f kbps    (%.1f KB/s)"), nSpeed, nSpeed / 8 );
	else if ( nSpeed < 8190 )
		strSpeed.Format( _T("%.0f kbps    (%.0f KB/s)"), nSpeed, nSpeed / 8 );
	else
		strSpeed.Format( _T("%.0f kbps    (%.2f MB/s)"), nSpeed, nSpeed / 8 / 1024 );

	return strSpeed;
}

HBRUSH CWizardConnectionPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = (HBRUSH)CWizardPage::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( pWnd == &m_wndTest )
	{
		pDC->SetTextColor( Colors.m_crTextLink );

		//CPoint point;
		//GetCursorPos( &point );
		//
		//CRect rc;
		//m_wndTest.GetWindowRect( &rc );
		//
		//if ( rc.PtInRect( point ) )
			pDC->SelectObject( &CoolInterface.m_fntUnder );
	}

	return hbr;
}

BOOL CWizardConnectionPage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint point;
	GetCursorPos( &point );

	CRect rc;
	m_wndTest.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}

	return CWizardPage::OnSetCursor( pWnd, nHitTest, message );
}

void CWizardConnectionPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWizardPage::OnLButtonUp( nFlags, point );

	CRect rc;
	m_wndTest.GetWindowRect( &rc );
	ScreenToClient( &rc );
	if ( ! rc.PtInRect( point ) )
		return;

	const CString strTestSite = _T("http://www.speedtest.net/switch_language.php?lang=") + Settings.General.Language.Left(2);

	ShellExecute( GetSafeHwnd(), _T("open"), strTestSite, NULL, NULL, SW_SHOWNORMAL );
}

void CWizardConnectionPage::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWizardPage::OnRButtonDown( nFlags, point );

	CRect rc;
	m_wndTest.GetWindowRect( &rc );
	ScreenToClient( &rc );
	if ( ! rc.PtInRect( point ) )
		return;

	const CString strTestSite = _T("http://www.speedtest.net/switch_language.php?lang=") + Settings.General.Language.Left(2);

	theApp.SetClipboard( strTestSite, TRUE );
}