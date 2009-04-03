//
// PeerProject.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2009
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
#include "BTClients.h"
#include "BTInfo.h"
#include "CoolInterface.h"
#include "DDEServer.h"
#include "DiscoveryServices.h"
#include "DlgHelp.h"
#include "DlgSplash.h"
#include "DownloadGroups.h"
#include "Downloads.h"
#include "EDClients.h"
#include "Emoticons.h"
#include "FileExecutor.h"
#include "Firewall.h"
#include "Flags.h"
#include "FontManager.h"
#include "GProfile.h"
#include "HostCache.h"
#include "IEProtocol.h"
#include "ImageServices.h"
#include "Library.h"
#include "LibraryBuilder.h"
#include "Network.h"
#include "Plugins.h"
#include "PeerProjectURL.h"
#include "QueryHashMaster.h"
#include "Registry.h"
#include "Scheduler.h"
#include "SchemaCache.h"
#include "Security.h"
#include "Settings.h"
#include "SharedFile.h"
#include "ShellIcons.h"
#include "Skin.h"
#include "ThumbCache.h"
#include "Transfers.h"
#include "UPnPFinder.h"
#include "UploadQueues.h"
#include "Uploads.h"
#include "VendorCache.h"
#include "VersionChecker.h"
#include "WndMain.h"
#include "WndSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const LPCTSTR RT_BMP = _T("BMP");
const LPCTSTR RT_JPEG = _T("JPEG");
const LPCTSTR RT_PNG = _T("PNG");
const LPCTSTR RT_GZIP = _T("GZIP");
// double scaleX = 1;
// double scaleY = 1;

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectCommandLineInfo

CPeerProjectCommandLineInfo::CPeerProjectCommandLineInfo() :
	m_bTray( FALSE ),
	m_bNoSplash( FALSE ),
	m_bNoAlphaWarning( FALSE ),
	m_nGUIMode( -1 ),
	m_bHelp( FALSE )
{
}

void CPeerProjectCommandLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if ( bFlag )
	{
		if ( ! lstrcmpi( pszParam, _T("tray") ) )
		{
			m_bTray = TRUE;
			m_bNoSplash = TRUE;
			return;
		}
		else if ( ! lstrcmpi( pszParam, _T("nosplash") ) )
		{
			m_bNoSplash = TRUE;
			return;
		}
		else if ( ! lstrcmpi( pszParam, _T("nowarn") ) )
		{
			m_bNoAlphaWarning = TRUE;
			return;
		}
		else if ( ! lstrcmpi( pszParam, _T("basic") ) )
		{
			m_nGUIMode = GUI_BASIC;
			return;
		}
		else if ( ! lstrcmpi( pszParam, _T("tabbed") ) )
		{
			m_nGUIMode = GUI_TABBED;
			return;
		}
		else if ( ! lstrcmpi( pszParam, _T("windowed") ) )
		{
			m_nGUIMode = GUI_WINDOWED;
			return;
		}
		else if ( ! lstrcmpi( pszParam, _T("?") ) )
		{
			m_bHelp = TRUE;
			return;
		}
	}
	CCommandLineInfo::ParseParam( pszParam, bFlag, bLast );
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp

BEGIN_MESSAGE_MAP(CPeerProjectApp, CWinApp)
	//{{AFX_MSG_MAP(CPeerProjectApp)
	//}}AFX_MSG
END_MESSAGE_MAP()

// {E3481FE3-E062-4E1C-A23A-62A6D13CBFB8}
const GUID CDECL BASED_CODE _tlid =
	{ 0xE3481FE3, 0xE062, 0x4E1C, { 0xA2, 0x3A, 0x62, 0xA6, 0xD1, 0x3C, 0xBF, 0xB8 } };

CPeerProjectApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp construction

CPeerProjectApp::CPeerProjectApp() :
	m_pMutex				( NULL )
,	m_pSafeWnd				( NULL )
,	m_bBusy					( 0 )
,	m_bClosing				( false )
,	m_bLive					( false )
,	m_bInteractive			( false )
,	m_bIsServer				( false )
,	m_bIsWin2000			( false )
,	m_bIsVistaOrNewer		( false )
,	m_bLimitedConnections	( true )
,	m_nWindowsVersion		( 0ul )
,	m_nWindowsVersionMinor	( 0ul )
,	m_nPhysicalMemory		( 0ull )
,	m_nLogicalProcessors	( -1 )
,	m_bMenuWasVisible		( FALSE )
,	m_nDefaultFontSize		( 0 )
,	m_bUPnPPortsForwarded	( TRI_UNKNOWN )
,	m_bUPnPDeviceConnected	( TRI_UNKNOWN )
,	m_nUPnPExternalAddress	( 0ul )
,	m_nLastInput			( 0ul )
,	m_hHookKbd				( NULL )
,	m_hHookMouse			( NULL )

,	m_hCryptProv			( NULL )
,	m_pRegisterApplicationRestart( NULL )

,	m_dlgSplash				( NULL )
,	m_hTheme				( NULL )
,	m_pfnSetWindowTheme		( NULL )
,	m_pfnIsThemeActive		( NULL )
,	m_pfnOpenThemeData		( NULL )
,	m_pfnCloseThemeData		( NULL )
,	m_pfnDrawThemeBackground( NULL )

,	m_hShlWapi				( NULL )
,	m_pfnAssocIsDangerous	( NULL )

,	m_hLibGFL				( NULL )
,	m_hGeoIP				( NULL )
,	m_pGeoIP				( NULL )
,	m_pfnGeoIP_country_code_by_ipnum( NULL )
,	m_pfnGeoIP_country_name_by_ipnum( NULL )

{
	ZeroMemory( m_nVersion, sizeof( m_nVersion ) );
	ZeroMemory( m_pBTVersion, sizeof( m_pBTVersion ) );

// BugTrap (www.intellesoft.net)
#ifdef _DEBUG
	BT_InstallSehFilter();
	BT_SetTerminate();
	BT_SetFlags( BTF_INTERCEPTSUEF | BTF_SHOWADVANCEDUI | BTF_DESCRIBEERROR | BTF_DETAILEDMODE );
	BT_SetSupportURL( _T("http://support.peerproject.org") );
	//BT_SetSupportEMail( _T("support@peerproject.org") );
	//BT_SetSupportServer( _T("http://peerproject.org/BugTrapServer/RequestHandler.aspx"), 80 );
	BT_AddRegFile( _T("settings.reg"), _T("HKEY_CURRENT_USER\\Software\\PeerProject\\PeerProject") );
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp initialization

BOOL CPeerProjectApp::InitInstance()
{
	// Set Build Date
	COleDateTime tCompileTime;
	tCompileTime.ParseDateTime( _T(__DATE__), LOCALE_NOUSEROVERRIDE, 1033 );
	m_sBuildDate = tCompileTime.Format( _T("%Y%m%d") );

	CWinApp::InitInstance();

	SetRegistryKey( _T("PeerProject") );
	GetVersionNumber();
	Settings.Load();			// Loads settings. Depends on GetVersionNumber()
	InitResources();			// Loads theApp settings. Depends on Settings::Load()
	CoolInterface.Load();		// Loads colors and fonts. Depends on InitResources()

	AfxOleInit();
//	m_pFontManager = new CFontManager();
//	AfxEnableControlContainer( m_pFontManager );
	AfxEnableControlContainer();

	LoadStdProfileSettings();
	EnableShellOpen();
//	RegisterShellFileTypes();

	ParseCommandLine( m_ocmdInfo );
	if ( m_ocmdInfo.m_bHelp )
	{
		AfxMessageBox( _T("Command-line options:\n")
			_T("-tray\t\tDisable startup splash screen and send application to tray\n")
			_T("-nosplash\t\tDisable startup splash screen\n")
			_T("-nowarn\t\tDisable alpha (daily builds) version warning dialog\n")
			_T("-basic\t\tStart application in Basic interface mode\n")
			_T("-tabbed\t\tStart application in Tabbed interface mode\n")
			_T("-windowed\tStart application in Windowed interface mode\n")
			_T("-regserver\tRegister application internal components\n")
			_T("-unregserver\tUn-register application internal components\n"),
			MB_ICONINFORMATION | MB_OK );
		return FALSE;
	}
	if ( m_ocmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister )
	{
		// Do not call this ->
		// ProcessShellCommand( m_ocmdInfo );
		// ... else all INI settings will be deleted (by design)

		// Do not call this ->
		// AfxOleUnregisterTypeLib( _tlid, _wVerMajor, _wVerMinor );
		// COleTemplateServer::UnregisterAll();
		// COleObjectFactory::UpdateRegistryAll( FALSE );
		// ... else OLE interface settings may be deleted (bug in MFC?)
		return FALSE;
	}
	if ( m_ocmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister )
	{
		ProcessShellCommand( m_ocmdInfo );
	}
	AfxOleRegisterTypeLib( AfxGetInstanceHandle(), _tlid );
	COleTemplateServer::RegisterAll();
	COleObjectFactory::UpdateRegistryAll( TRUE );
	if ( m_ocmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister )
	{
		return FALSE;
	}

	m_pMutex = CreateMutex( NULL, FALSE, _T("Global\\PeerProject") );
	if ( m_pMutex != NULL )
	{
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			CloseHandle( m_pMutex );
			m_pMutex = NULL;

			// Popup first instance
			if ( CWnd* pWnd = CWnd::FindWindow( _T("PeerProjectMainWnd"), NULL ) )
			{
				pWnd->SendMessage( WM_SYSCOMMAND, SC_RESTORE );
				pWnd->ShowWindow( SW_SHOWNORMAL );
				pWnd->BringWindowToTop();
				pWnd->SetForegroundWindow();
			}
			else
			{
				// Probably window created in another user's session
			}
			return FALSE;
		}
		// We are first!
	}
	else
	{
		// Probably mutex created in another user's session
		return FALSE;
	}

	m_bInteractive = true;

	if ( m_pRegisterApplicationRestart )
		m_pRegisterApplicationRestart( NULL, 0 );

	ShowStartupText();

	DDEServer.Create();
	IEProtocol.Create();

	// ***********
	//*
	// BETA EXPIRATION. Remember to re-compile to update the time, 
	// and remove this section for final releases and public betas.
	COleDateTime tCurrent = COleDateTime::GetCurrentTime();
	//COleDateTimeSpan tTimeOut( 31 * 2, 0, 0, 0);	// Betas that aren't on sourceforge
	COleDateTimeSpan tTimeOut( 14, 0, 0, 0);		// Daily debug builds
	if ( ( tCompileTime + tTimeOut )  < tCurrent )
	{
		CString strMessage;
		LoadString( strMessage, IDS_BETA_EXPIRED);
		AfxMessageBox( strMessage, MB_ICONQUESTION|MB_OK );
		//return FALSE;
	}
	//*/

	//*
	// ALPHA WARNING. Remember to remove this section for final releases and public betas.
	if ( ! m_ocmdInfo.m_bNoAlphaWarning && m_ocmdInfo.m_bShowSplash )
	if ( AfxMessageBox(
		L"\nWARNING: This is an ALPHA TEST version of PeerProject p2p.\n\n"
		L"NOT FOR GENERAL USE, it is intended for pre-release testing in controlled environments.  "
		L"It may stop running or display debug info to assist testing.\n\n"
		L"If you wish to simply use this software, then download the current\n"
		L"stable release from PeerProject.org.  If you continue past this point,\n"
		L"you may experience system instability or lose files.  Be aware of\n"
		L"recent development before using, effects might not be recoverable.\n\n"
		L"Do you wish to continue?", MB_ICONEXCLAMATION|MB_YESNO ) == IDNO )
		return FALSE;
	//*/
	// ***********

	int nSplashSteps = 18
		+ ( Settings.Connection.EnableFirewallException ? 1 : 0 )
		+ ((Settings.Connection.EnableUPnP && !Settings.Live.FirstRun) ? 1 : 0 );

	SplashStep( L"Winsock", ( ( m_ocmdInfo.m_bNoSplash || ! m_ocmdInfo.m_bShowSplash ) ? 0 : nSplashSteps ), false );
		WSADATA wsaData;
		for ( int i = 1; i <= 2; i++ )
		{
			if ( WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) ) return FALSE;
			if ( wsaData.wVersion == MAKEWORD( 1, 1 ) ) break;
			if ( i == 2 ) return FALSE;
			WSACleanup();
		}

	CryptAcquireContext( &m_hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT );

	if ( m_ocmdInfo.m_nGUIMode != -1 )
		Settings.General.GUIMode = m_ocmdInfo.m_nGUIMode;

	if ( Settings.General.GUIMode != GUI_WINDOWED && Settings.General.GUIMode != GUI_TABBED && Settings.General.GUIMode != GUI_BASIC )
		Settings.General.GUIMode = GUI_BASIC;

	SplashStep( L"PeerProject Database" );
		PurgeDeletes();
		CThumbCache::InitDatabase();
	SplashStep( L"P2P URIs" );
		CPeerProjectURL::Register( TRUE );
	SplashStep( L"Shell Icons" );
		ShellIcons.Clear();
	SplashStep( L"Metadata Schemas" );
		SchemaCache.Load();
	SplashStep( L"Vendor Data" );
		VendorCache.Load();
	SplashStep( L"Profile" );
		MyProfile.Load();
	SplashStep( L"Security Services" );
		Security.Load();
		AdultFilter.Load();
		MessageFilter.Load();
	SplashStep( L"Query Manager" );
		QueryHashMaster.Create();
	SplashStep( L"Host Cache" );
		HostCache.Load();
	SplashStep( L"Discovery Services" );
		DiscoveryServices.Load();
	SplashStep( L"Scheduler" );
		Schedule.Load();
	SplashStep( L"Rich Documents" );
		Emoticons.Load();
		Flags.Load();

	if ( Settings.Connection.EnableFirewallException )
	{
		SplashStep( L"Windows Firewall Setup" );
		CFirewall firewall;
		if ( firewall.AccessWindowsFirewall() && firewall.AreExceptionsAllowed() )
		{
			// Add to firewall exception list if necessary
			// and enable UPnP Framework if disabled
			firewall.SetupService( NET_FW_SERVICE_UPNP );
			firewall.SetupProgram( m_strBinaryPath, theApp.m_pszAppName );
		}
	}

	// If it is the first run we will run the UPnP discovery only in the QuickStart Wizard
	if ( Settings.Connection.EnableUPnP && ! Settings.Live.FirstRun )
	{
		SplashStep( L"Firewall/Router Setup" );
		try
		{
			m_pUPnPFinder.Attach( new CUPnPFinder );
			if ( m_pUPnPFinder->AreServicesHealthy() )
				m_pUPnPFinder->StartDiscovery();
		}
		catch ( CUPnPFinder::UPnPError& ) {}
		catch ( CException* e ) { e->Delete(); }
	}

	SplashStep( L"GUI" );
		if ( m_ocmdInfo.m_bTray ) WriteProfileInt( _T("Windows"), _T("CMainWnd.ShowCmd"), 0 );
		new CMainWnd();
		CoolMenu.EnableHook();
		if ( m_ocmdInfo.m_bTray )
		{
			((CMainWnd*)m_pMainWnd)->CloseToTray();
		}
		else
		{
			m_pMainWnd->ShowWindow( SW_SHOW );
			if ( m_dlgSplash )
				m_dlgSplash->Topmost();
			m_pMainWnd->UpdateWindow();
		}

	// From this point translations are available and LoadString returns correct strings
	SplashStep( L"Download Manager" );
		Downloads.Load();
	SplashStep( L"Upload Manager" );
		UploadQueues.Load();
	SplashStep( L"Library" );
		Library.Load();
	SplashStep( L"Upgrade Manager" );
		VersionChecker.Start();

	SplashStep();

	m_bLive = true;

	ProcessShellCommand( m_ocmdInfo );

//	afxMemDF = allocMemDF | delayFreeMemDF | checkAlwaysMemDF;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp termination

int CPeerProjectApp::ExitInstance()
{
	if ( m_bInteractive )
	{
		CWaitCursor pCursor;

		DDEServer.Close();
		IEProtocol.Close();

		SplashStep( L"Disconnecting" );
		VersionChecker.Stop();
		DiscoveryServices.Stop();
		Network.Disconnect();

		SplashStep( L"Stopping Library Tasks" );
		Library.StopThread();

		SplashStep( L"Stopping Transfers" );
		Transfers.StopThread();
		Downloads.CloseTransfers();

		SplashStep( L"Clearing Clients" );
		Uploads.Clear( FALSE );
		EDClients.Clear();

		if ( Settings.Connection.DeleteFirewallException )
		{
			SplashStep( L"Closing Windows Firewall Access" );
			CFirewall firewall;
			if ( firewall.AccessWindowsFirewall() )
			{
				// Remove application from the firewall exception list
				firewall.SetupProgram( m_strBinaryPath, theApp.m_pszAppName, TRUE );
			}
		}

		if ( m_pUPnPFinder )
		{
			SplashStep( L"Closing Firewall/Router Access" );
			m_pUPnPFinder->StopAsyncFind();
			if ( Settings.Connection.DeleteUPnPPorts )
				m_pUPnPFinder->DeletePorts();
			m_pUPnPFinder.Free();
		}

		if ( m_bLive )
		{
			SplashStep( L"Saving" );
			Downloads.Save();
			DownloadGroups.Save();
			Library.Save();
			Security.Save();
			HostCache.Save();
			UploadQueues.Save();
			DiscoveryServices.Save();
			Settings.Save( TRUE );
		}

		SplashStep( L"Finalizing" );
		BTClients.Clear();
		Downloads.Clear( TRUE );
		Library.Clear();
		CoolMenu.Clear();
		Skin.Clear();

		SplashStep();

		LibraryBuilder.CleanupPlugins();
		ImageServices.Clear();
		Plugins.Clear();
	}

	WSACleanup();

	if ( m_hTheme != NULL )
		FreeLibrary( m_hTheme );

	if ( m_hShlWapi != NULL )
		FreeLibrary( m_hShlWapi );

	if ( m_hGeoIP != NULL )
		FreeLibrary( m_hGeoIP );

	if ( m_hLibGFL != NULL )
		FreeLibrary( m_hLibGFL );

//	delete m_pFontManager;

	UnhookWindowsHookEx( m_hHookKbd );
	UnhookWindowsHookEx( m_hHookMouse );

	if ( theApp.m_hCryptProv != 0 )
		CryptReleaseContext( theApp.m_hCryptProv, 0 );

	if ( m_pMutex != NULL )
		CloseHandle( m_pMutex );

	{
		CQuickLock pLock( m_csMessage );
		while ( ! m_oMessages.IsEmpty() )
			delete m_oMessages.RemoveHead();
	}

	return CWinApp::ExitInstance();
}

void CPeerProjectApp::SplashStep(LPCTSTR pszMessage, int nMax, bool bClosing)
{
	if ( ! pszMessage )
	{
		if ( m_dlgSplash )
		{
			m_dlgSplash->Hide();
			m_dlgSplash = NULL;
		}
	}
	else if ( ! m_dlgSplash && nMax )
	{
		m_dlgSplash = new CSplashDlg( nMax, bClosing );
		m_dlgSplash->Step( pszMessage );
	}
	else if ( m_dlgSplash && ! nMax )
		m_dlgSplash->Step( pszMessage );

	TRACE( _T("Step: %s\n"), pszMessage ? pszMessage : _T("Done") );
}

void CPeerProjectApp::WinHelp(DWORD /*dwData*/, UINT /*nCmd*/)
{
	// Suppress F1
}

CDocument* CPeerProjectApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	if ( lpszFileName )
		Open( lpszFileName, TRUE );
	return NULL;
}

BOOL CPeerProjectApp::Open(LPCTSTR lpszFileName, BOOL bDoIt)
{
	int nLength = lstrlen( lpszFileName );
	if ( nLength > 8 && ! lstrcmpi( lpszFileName + nLength - 8, _T(".torrent") ) )
		return OpenTorrent( lpszFileName, bDoIt );
	else if ( nLength > 3 && ! lstrcmpi( lpszFileName + nLength - 3, _T(".co") ) )
		return OpenCollection( lpszFileName, bDoIt );
	else if ( nLength > 11 && ! lstrcmpi( lpszFileName + nLength - 11, _T(".collection") ) )
		return OpenCollection( lpszFileName, bDoIt );
	else if ( nLength > 16 && ! lstrcmpi( lpszFileName + nLength - 16, _T(".emulecollection") ) )
		return OpenCollection( lpszFileName, bDoIt );
	else if ( nLength > 4 && ! lstrcmpi( lpszFileName + nLength - 4, _T(".url") ) )
		return OpenInternetShortcut( lpszFileName, bDoIt );
	else if ( nLength > 4 && ! lstrcmpi( lpszFileName + nLength - 4, _T(".met") ) )
		return ! bDoIt || HostCache.Import( lpszFileName );
	else if ( nLength > 4 && ! lstrcmpi( lpszFileName + nLength - 4, _T(".lnk") ) )
		return OpenShellShortcut( lpszFileName, bDoIt );
	else
		return OpenURL( lpszFileName, bDoIt );
}

BOOL CPeerProjectApp::OpenShellShortcut(LPCTSTR lpszFileName, BOOL bDoIt)
{
	CString sPath( ResolveShortcut( lpszFileName ) );
	return sPath.GetLength() && Open( sPath, bDoIt );
}

BOOL CPeerProjectApp::OpenInternetShortcut(LPCTSTR lpszFileName, BOOL bDoIt)
{
	CString sURL;
	BOOL bResult = ( GetPrivateProfileString( _T("InternetShortcut"), _T("URL"),
		_T(""), sURL.GetBuffer( MAX_PATH ), MAX_PATH, lpszFileName ) > 3 );
	sURL.ReleaseBuffer();
	return bResult && sURL.GetLength() && OpenURL( sURL, bDoIt );
}

BOOL CPeerProjectApp::OpenTorrent(LPCTSTR lpszFileName, BOOL bDoIt)
{
	if ( bDoIt )
		theApp.Message( MSG_NOTICE, IDS_BT_PREFETCH_FILE, lpszFileName );

	BOOL bResult = FALSE;
	auto_ptr< CBTInfo > pTorrent( new CBTInfo() );
	if ( pTorrent.get() )
	{
		if ( pTorrent->LoadTorrentFile( lpszFileName ) )
		{
			if ( bDoIt && pTorrent->HasEncodingError() )
				theApp.Message( MSG_NOTICE, IDS_BT_ENCODING );
			auto_ptr< CPeerProjectURL > pURL( new CPeerProjectURL( pTorrent.release() ) );
			if ( pURL.get() )
			{
				bResult = TRUE;
				if ( bDoIt )
					return PostMainWndMessage( WM_URL, (WPARAM)pURL.release() );
			}
		}
	}

	if ( bDoIt )
		theApp.Message( MSG_ERROR, IDS_BT_PREFETCH_ERROR, lpszFileName );

	return bResult;
}

BOOL CPeerProjectApp::OpenCollection(LPCTSTR lpszFileName, BOOL bDoIt)
{
	if ( ! bDoIt )
		return TRUE;

	auto_array< TCHAR > pszPath( new TCHAR[ lstrlen( lpszFileName ) + 1 ] );
	if ( pszPath.get() )
	{
		lstrcpy( pszPath.get(), lpszFileName );
		if ( PostMainWndMessage( WM_COLLECTION, (WPARAM)pszPath.release() ) )
			return TRUE;
	}

	return FALSE;
}

BOOL CPeerProjectApp::OpenURL(LPCTSTR lpszFileName, BOOL bDoIt, BOOL bSilent)
{
	if ( bDoIt && ! bSilent )
		theApp.Message( MSG_NOTICE, IDS_URL_RECEIVED, lpszFileName );

	auto_ptr< CPeerProjectURL > pURL( new CPeerProjectURL() );
	if ( pURL.get() )
	{
		if ( pURL->Parse( lpszFileName ) )
		{
			if ( bDoIt )
				PostMainWndMessage( WM_URL, (WPARAM)pURL.release() );
			return TRUE;
		}
	}

	if ( bDoIt && ! bSilent )
		theApp.Message( MSG_NOTICE, IDS_URL_PARSE_ERROR );

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp version

void CPeerProjectApp::GetVersionNumber()
{
	DWORD dwSize;

	m_nVersion[0] = m_nVersion[1] = m_nVersion[2] = m_nVersion[3] = 0;

	GetModuleFileName( NULL, m_strBinaryPath.GetBuffer( MAX_PATH ), MAX_PATH );
	m_strBinaryPath.ReleaseBuffer( MAX_PATH );
	dwSize = GetFileVersionInfoSize( m_strBinaryPath, &dwSize );

	if ( dwSize )
	{
		BYTE* pBuffer = new BYTE[ dwSize ];

		if ( pBuffer )
		{
			if ( GetFileVersionInfo( m_strBinaryPath, NULL, dwSize, pBuffer ) )
			{
				VS_FIXEDFILEINFO* pTable;

				if ( VerQueryValue( pBuffer, _T("\\"), (VOID**)&pTable, (UINT*)&dwSize ) )
				{
					m_nVersion[0] = (WORD)( pTable->dwFileVersionMS >> 16 );
					m_nVersion[1] = (WORD)( pTable->dwFileVersionMS & 0xFFFF );
					m_nVersion[2] = (WORD)( pTable->dwFileVersionLS >> 16 );
					m_nVersion[3] = (WORD)( pTable->dwFileVersionLS & 0xFFFF );
				}
			}

			delete [] pBuffer;
		}
	}

	m_sVersion.Format( _T("%i.%i.%i.%i"),
		m_nVersion[0], m_nVersion[1],
		m_nVersion[2], m_nVersion[3] );

	m_sSmartAgent = _T( CLIENT_NAME );
	m_sSmartAgent += _T(" ");
	m_sSmartAgent += m_sVersion;

	m_pBTVersion[ 0 ] = BT_ID1;
	m_pBTVersion[ 1 ] = BT_ID2;
	m_pBTVersion[ 2 ] = (BYTE)m_nVersion[ 0 ];
	m_pBTVersion[ 3 ] = (BYTE)m_nVersion[ 1 ];

	//Determine the version of Windows
	OSVERSIONINFOEX pVersion;
	pVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx( (OSVERSIONINFO*)&pVersion );

	// Determine if it's a server
	m_bIsServer = pVersion.wProductType != VER_NT_WORKSTATION;

	// Get Major version
	// Windows 2000 (Major Version == 5) does not support some functions
	m_nWindowsVersion = pVersion.dwMajorVersion;

	// Get Minor version
	//	Major version == 5
	//		Win2000 = 0, WinXP = 1, WinXP64 = 2, Server2003 = 2
	//	Major version == 6
	//		Vista = 0, Server2008 = 0
	m_nWindowsVersionMinor = pVersion.dwMinorVersion;

	// Most supported windows versions have network limiting
	m_bLimitedConnections = true;

	// Set some variables for different Windows OSes
	if ( m_nWindowsVersion == 5 )
	{
		TCHAR* sp = _tcsstr( pVersion.szCSDVersion, _T("Service Pack") );

		// Windows 2000
		if ( m_nWindowsVersionMinor == 0 )
			m_bIsWin2000 = true;

		// Windows XP
		else if ( m_nWindowsVersionMinor == 1 )
		{
			// No network limiting for Vanilla XP or SP1
			if ( !sp || sp[ 13 ] == '1' )
				m_bLimitedConnections = false;
		}

		// Windows 2003 or Windows XP64
		else if ( m_nWindowsVersionMinor == 2 )
		{
			// No network limiting for Vanilla Win2003/XP64
			if ( !sp )
				m_bLimitedConnections = false;
		}
	}
	else if ( m_nWindowsVersion >= 6 )
		m_bIsVistaOrNewer = true;
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp resources

void CPeerProjectApp::InitResources()
{
	// Get pointers to some functions that require Windows Vista or greater
	if ( HMODULE hKernel32 = GetModuleHandle( _T("kernel32.dll") ) )
	{
		(FARPROC&)m_pRegisterApplicationRestart = GetProcAddress( hKernel32, "RegisterApplicationRestart" );
	}

	// Get pointers to some functions that require Windows XP or greater
	if ( ( m_hTheme = LoadLibrary( _T("UxTheme.dll") ) ) != NULL )
	{
		(FARPROC&)m_pfnSetWindowTheme = GetProcAddress( m_hTheme, "SetWindowTheme" );
		(FARPROC&)m_pfnIsThemeActive = GetProcAddress( m_hTheme, "IsThemeActive" );
		(FARPROC&)m_pfnOpenThemeData = GetProcAddress( m_hTheme, "OpenThemeData" );
		(FARPROC&)m_pfnCloseThemeData = GetProcAddress( m_hTheme, "CloseThemeData" );
		(FARPROC&)m_pfnDrawThemeBackground = GetProcAddress( m_hTheme, "DrawThemeBackground" );
		(FARPROC&)m_pfnEnableThemeDialogTexture = GetProcAddress( m_hTheme, "EnableThemeDialogTexture" );
	//	(FARPROC&)m_pfnDrawThemeParentBackground = GetProcAddress( m_hTheme, "DrawThemeParentBackground" );
	//	(FARPROC&)m_pfnGetThemeBackgroundContentRect = GetProcAddress( m_hTheme, "GetThemeBackgroundContentRect" );
	//	(FARPROC&)m_pfnGetThemeSysFont = GetProcAddress( m_hTheme, "GetThemeSysFont" );
	//	(FARPROC&)m_pfnDrawThemeText = GetProcAddress( m_hTheme, "DrawThemeText" );
	}

	// Get pointers to some functions that require Internet Explorer 6 or greater
	if ( ( m_hShlWapi = LoadLibrary( _T("shlwapi.dll") ) ) != NULL )
	{
		(FARPROC&)m_pfnAssocIsDangerous = GetProcAddress( m_hShlWapi, "AssocIsDangerous" );
	}

	// Load the GeoIP library for mapping IPs to countries
	if ( ( m_hGeoIP = CustomLoadLibrary( _T("GeoIP.dll") ) ) != NULL )
	{
		GeoIP_newFunc pfnGeoIP_new = (GeoIP_newFunc)GetProcAddress( m_hGeoIP, "GeoIP_new" );
		m_pfnGeoIP_country_code_by_ipnum = (GeoIP_country_code_by_ipnumFunc)GetProcAddress( m_hGeoIP, "GeoIP_country_code_by_ipnum" );
		m_pfnGeoIP_country_name_by_ipnum = (GeoIP_country_name_by_ipnumFunc)GetProcAddress( m_hGeoIP, "GeoIP_country_name_by_ipnum" );
		if ( pfnGeoIP_new ) 
			m_pGeoIP = pfnGeoIP_new( GEOIP_MEMORY_CACHE );
	}

	// We load it in a custom way, so PeerProject plugins can use this library also when it isn't in its search path but loaded by CustomLoadLibrary (very useful when running PeerProject inside Visual Studio)
	m_hLibGFL = CustomLoadLibrary( _T("LibGFL290.dll") );

	// Use GlobalMemoryStatusEx if possible (WinXP)
	MEMORYSTATUSEX pMemory = {};
	pMemory.dwLength = sizeof(pMemory);
	if ( GlobalMemoryStatusEx( &pMemory ) )
		m_nPhysicalMemory = pMemory.ullTotalPhys;

	long nResult = ERROR_SUCCESS;
	CString strProcKey;
	while ( nResult == ERROR_SUCCESS )
	{
		HKEY hKey;
		// These keyes are populated during startup, so it's safe to check them
		strProcKey.Format( L"Hardware\\Description\\System\\CentralProcessor\\%i", ++m_nLogicalProcessors );
		if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, strProcKey, 0, KEY_QUERY_VALUE, &hKey ) != ERROR_SUCCESS )
			break;
		RegCloseKey( hKey );
	}

//	For Disabled Font Manager-
//	HDC screen = GetDC( 0 );
//	scaleX = GetDeviceCaps( screen, LOGPIXELSX ) / 96.0;
//	scaleY = GetDeviceCaps( screen, LOGPIXELSY ) / 96.0;
//	ReleaseDC( 0, screen );

	// Get the fonts from the registry
	CString strFont = m_bIsVistaOrNewer ? _T( "Segoe UI" ) : _T( "Tahoma" ) ;
	theApp.m_sDefaultFont		= theApp.GetProfileString( _T("Fonts"), _T("DefaultFont"), strFont );
	theApp.m_sPacketDumpFont	= theApp.GetProfileString( _T("Fonts"), _T("PacketDumpFont"), _T("Lucida Console") );
	theApp.m_sSystemLogFont		= theApp.GetProfileString( _T("Fonts"), _T("SystemLogFont"), strFont );
	theApp.m_nDefaultFontSize	= theApp.GetProfileInt( _T("Fonts"), _T("FontSize"), 11 );

	// Set up the default font
	m_gdiFont.CreateFontW( -theApp.m_nDefaultFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, theApp.m_sDefaultFont );

	m_gdiFontBold.CreateFontW( -theApp.m_nDefaultFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, theApp.m_sDefaultFont );

	m_gdiFontLine.CreateFontW( -theApp.m_nDefaultFontSize, 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, theApp.m_sDefaultFont );

	srand( GetTickCount() );

	m_hHookKbd   = SetWindowsHookEx( WH_KEYBOARD, (HOOKPROC)KbdHook, NULL, AfxGetThread()->m_nThreadID );
	m_hHookMouse = SetWindowsHookEx( WH_MOUSE, (HOOKPROC)MouseHook, NULL, AfxGetThread()->m_nThreadID );
	m_nLastInput = (DWORD)time( NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp custom library loader

HINSTANCE CPeerProjectApp::CustomLoadLibrary(LPCTSTR pszFileName)
{
	HINSTANCE hLibrary = NULL;

	if ( ( hLibrary = LoadLibrary( pszFileName ) ) != NULL || ( hLibrary = LoadLibrary( Settings.General.Path + _T("\\") + pszFileName ) ) != NULL );
	else
		TRACE( _T("DLL not found: %s\r\n"), pszFileName );

	return hLibrary;
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp safe main window

CMainWnd* CPeerProjectApp::SafeMainWnd() const
{
	if ( m_pSafeWnd == NULL ) return NULL;
	ASSERT_KINDOF( CMainWnd, m_pSafeWnd );
	return static_cast< CMainWnd* >( IsWindow( m_pSafeWnd->m_hWnd ) ? m_pSafeWnd : NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp message

bool CPeerProjectApp::IsLogDisabled(WORD nType) const
{
	return
		// Severity filter
		( static_cast< DWORD >( nType & MSG_SEVERITY_MASK ) > Settings.General.LogLevel ) ||
		// Facility filter
		( ( nType & MSG_FACILITY_MASK ) == MSG_FACILITY_SEARCH && ! Settings.General.SearchLog );
}

void CPeerProjectApp::ShowStartupText()
{
	CString strBody;
	LoadString( strBody, IDS_SYSTEM_MESSAGE );

	strBody.Replace( _T("(version)"), (LPCTSTR)(theApp.m_sVersion + _T(" (") + theApp.m_sBuildDate + _T(")")) );

	for ( strBody += '\n' ; strBody.GetLength() ; )
	{
		CString strLine = strBody.SpanExcluding( _T("\r\n") );
		strBody = strBody.Mid( strLine.GetLength() + 1 );

		strLine.TrimLeft();
		strLine.TrimRight();
		if ( strLine.IsEmpty() ) continue;

		if ( strLine == _T(".") ) strLine.Empty();

		if ( _tcsnicmp( strLine, _T("!"), 1 ) == 0 )
			PrintMessage( MSG_NOTICE, (LPCTSTR)strLine + 1 );
		else
			PrintMessage( MSG_INFO, strLine );
	}
}

void CPeerProjectApp::Message(WORD nType, UINT nID, ...)
{
	// Check if logging this type of message is enabled
	if ( IsLogDisabled( nType ) )
		return;

	// Load the format string from the resource file
	CString strFormat;
	LoadString( strFormat, nID );

	// Initialize variable arguments list
	va_list pArgs;
	va_start( pArgs, nID );

	// Work out the type of format string and call the appropriate function
	CString strTemp;
	if ( strFormat.Find( _T("%1") ) >= 0 )
		strTemp.FormatMessageV( strFormat, &pArgs );
	else
		strTemp.FormatV( strFormat, pArgs );

	// Print the message if there still is one
	if ( strTemp.GetLength() )
		PrintMessage( nType, strTemp );

	// Null the argument list pointer
	va_end( pArgs );

	return;
}

void CPeerProjectApp::Message(WORD nType, LPCTSTR pszFormat, ...)
{
	// Check if logging this type of message is enabled
	if ( IsLogDisabled( nType ) )
		return;

	// Initialize variable arguments list
	va_list pArgs;
	va_start( pArgs, pszFormat );

	// Format the message
	CString strTemp;
	strTemp.FormatV( pszFormat, pArgs );

	// Print the message if there still is one
	if ( strTemp.GetLength() )
		PrintMessage( nType, strTemp );

	// Null the argument list pointer
	va_end( pArgs );

	return;
}

void CPeerProjectApp::PrintMessage(WORD nType, const CString& strLog)
{
	if ( Settings.General.DebugLog )
		LogMessage( strLog );

	CQuickLock pLock( m_csMessage );

	// Max 1000 lines
	if ( m_oMessages.GetCount() >= 1000 )
		delete m_oMessages.RemoveHead();

	m_oMessages.AddTail( new CLogMessage( nType, strLog ) );
}

void CPeerProjectApp::LogMessage(const CString& strLog)
{
	CQuickLock pLock( m_csMessage );

	CFile pFile;
	if ( pFile.Open( Settings.General.UserPath + _T("\\Data\\PeerProject.log"), CFile::modeReadWrite ) )
	{
		pFile.Seek( 0, CFile::end ); // go to the end of the file to add entires.

		if ( ( Settings.General.MaxDebugLogSize ) &&					// If log rotation is on
			( pFile.GetLength() > Settings.General.MaxDebugLogSize ) )	// and file is too long...
		{
			// Close the file
			pFile.Close();
			
			// Rotate the logs
			if ( DeleteFileEx( Settings.General.UserPath + _T("\\Data\\PeerProject.old.log"), FALSE, FALSE, FALSE ) )
			{
				MoveFile( Settings.General.UserPath + _T("\\Data\\PeerProject.log"),
					Settings.General.UserPath + _T("\\Data\\PeerProject.old.log") );
				// Start a new log
				if ( ! pFile.Open( Settings.General.UserPath + _T("\\Data\\PeerProject.log"),
					CFile::modeWrite|CFile::modeCreate ) ) return;
				// Unicode marker
				WORD nByteOrder = 0xFEFF;
				pFile.Write( &nByteOrder, 2 );
			}
		}
	}
	else
	{
		if ( ! pFile.Open( Settings.General.UserPath + _T("\\Data\\PeerProject.log"),
			CFile::modeWrite|CFile::modeCreate ) ) return;
		// Unicode marker
		WORD nByteOrder = 0xFEFF;
		pFile.Write( &nByteOrder, 2 );
	}

	if ( Settings.General.ShowTimestamp )
	{
		CTime pNow = CTime::GetCurrentTime();
		CString strLine;
		strLine.Format( _T("[%.2i:%.2i:%.2i] "),
			pNow.GetHour(), pNow.GetMinute(), pNow.GetSecond() );
		pFile.Write( (LPCTSTR)strLine, sizeof(TCHAR) * strLine.GetLength() );
	}

	pFile.Write( (LPCTSTR)strLog, static_cast< UINT >( sizeof(TCHAR) * strLog.GetLength() ) );
	pFile.Write( _T("\r\n"), sizeof(TCHAR) * 2 );

	pFile.Close();
}

CString GetErrorString(DWORD dwError)
{
	LPTSTR MessageBuffer = NULL;
	CString strMessage;
	if ( FormatMessage (
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwError, 0, (LPTSTR)&MessageBuffer, 0, NULL ) )
	{
		strMessage = MessageBuffer;
		strMessage.Trim( _T(" \t\r\n") );
		LocalFree( MessageBuffer );
		return strMessage;
	}

	static LPCTSTR const szModules [] =
	{
		_T("netapi32.dll"),
		_T("netmsg.dll"),
		_T("wininet.dll"),
		_T("ntdll.dll"),
		_T("ntdsbmsg.dll"),
		NULL
	};
	for ( int i = 0; szModules[ i ]; i++ )
	{
		HMODULE hModule = LoadLibraryEx( szModules[ i ], NULL, LOAD_LIBRARY_AS_DATAFILE );
		if ( hModule )
		{
			DWORD bResult = FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_FROM_HMODULE,
				hModule, dwError, 0, (LPTSTR)&MessageBuffer, 0, NULL );
			FreeLibrary( hModule );
			if ( bResult )
			{
				strMessage = MessageBuffer;
				strMessage.Trim( _T(" \t\r\n") );
				LocalFree( MessageBuffer );
				return strMessage;
			}
		}
	}
	return CString();
}

CString CPeerProjectApp::GetCountryCode(IN_ADDR pAddress) const
{
	if ( m_pfnGeoIP_country_code_by_ipnum && m_pGeoIP )
		return CString( m_pfnGeoIP_country_code_by_ipnum( m_pGeoIP, htonl( pAddress.s_addr ) ) );
	return _T("");
}

CString CPeerProjectApp::GetCountryName(IN_ADDR pAddress) const
{
	if ( m_pfnGeoIP_country_name_by_ipnum && m_pGeoIP )
		return CString( m_pfnGeoIP_country_name_by_ipnum( m_pGeoIP, htonl( pAddress.s_addr ) ) );
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp process an internal URI

BOOL CPeerProjectApp::InternalURI(LPCTSTR pszURI)
{
	CMainWnd* pMainWnd = SafeMainWnd();
	if ( pMainWnd == NULL ) return FALSE;

	CString strURI( pszURI );

	if ( strURI.Find( _T("peer:command:") ) == 0 )
	{
		if ( UINT nCmdID = CoolInterface.NameToID( pszURI + 13 ) )
		{
			pMainWnd->PostMessage( WM_COMMAND, nCmdID );
		}
	}
	else if ( strURI.Find( _T("peer:windowptr:") ) == 0 )
	{
		CChildWnd* pChild = NULL;
		_stscanf( (LPCTSTR)strURI + 15, _T("%lu"), &pChild );
		if ( pMainWnd->m_pWindows.Check( pChild ) ) pChild->MDIActivate();
	}
	else if ( strURI.Find( _T("peer:launch:") ) == 0 )
	{
		DWORD nIndex = 0;
		_stscanf( (LPCTSTR)strURI + 12, _T("%lu"), &nIndex );

		CSingleLock oLock( &Library.m_pSection, TRUE );
		if ( CLibraryFile* pFile = Library.LookupFile( nIndex ) )
		{
			if ( pFile->m_pFolder )
			{
				CString strPath = pFile->GetPath();
				oLock.Unlock();
				CFileExecutor::Execute( strPath, FALSE );
			}
		}
	}
	else if (	strURI.Find( _T("http://") ) == 0 ||
				strURI.Find( _T("https://") ) == 0 ||
				strURI.Find( _T("ftp://") ) == 0 ||
				strURI.Find( _T("mailto:") ) == 0 ||
				strURI.Find( _T("aim:") ) == 0 ||
				strURI.Find( _T("magnet:") ) == 0 ||
				strURI.Find( _T("foxy:") ) == 0 ||
				strURI.Find( _T("gnutella:") ) == 0 ||
				strURI.Find( _T("shareaza:") ) == 0 ||
				strURI.Find( _T("peerproject:") ) == 0 ||
				strURI.Find( _T("gwc:") ) == 0 ||
				strURI.Find( _T("uhc:") ) == 0 ||
				strURI.Find( _T("ukhl:") ) == 0 ||
				strURI.Find( _T("gnet:") ) == 0 ||
				strURI.Find( _T("gnutella1:") ) == 0 ||
				strURI.Find( _T("gnutella2:") ) == 0 ||
				strURI.Find( _T("mp2p:") ) == 0 ||
				strURI.Find( _T("ed2k:") ) == 0 ||
				strURI.Find( _T("sig2dat:") ) == 0 )
	{
		ShellExecute( pMainWnd->GetSafeHwnd(), _T("open"), strURI,
			NULL, NULL, SW_SHOWNORMAL );
	}
	else if ( strURI == _T("peer:connect") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_NETWORK_CONNECT );
	}
	else if ( strURI == _T("peer:disconnect") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_NETWORK_DISCONNECT );
	}
	else if ( strURI == _T("peer:search") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_TAB_SEARCH );
	}
	else if ( strURI == _T("peer:neighbours") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_VIEW_NEIGHBOURS );
	}
	else if ( strURI == _T("peer:downloads") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_VIEW_DOWNLOADS );
	}
	else if ( strURI == _T("peer:uploads") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_VIEW_UPLOADS );
	}
	else if ( strURI == _T("peer:shell:downloads") )
	{
		ShellExecute( pMainWnd->GetSafeHwnd(), _T("open"),
			Settings.Downloads.CompletePath, NULL, NULL, SW_SHOWNORMAL );
	}
	else if ( strURI == _T("peer:upgrade") )
	{
		pMainWnd->PostMessage( WM_VERSIONCHECK, VC_CONFIRM );
	}
	else if ( strURI == _T("peer:options") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_TOOLS_SETTINGS );
	}
	else if ( strURI == _T("peer:options:skins") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_TOOLS_SKIN );
	}
	else if ( strURI == _T("peer:wizard") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_TOOLS_WIZARD );
	}
	else if ( strURI == _T("peer:library") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_VIEW_LIBRARY );
	}
	else if ( strURI == _T("peer:library:downloads") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_VIEW_LIBRARY );
	}
	else if ( strURI == _T("peer:library:history") )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_VIEW_LIBRARY );
		pMainWnd->PostMessage( WM_COMMAND, ID_LIBRARY_TREE_VIRTUAL );
	}
	else if ( strURI.Find( _T("peer:library:/") ) == 0 )
	{
		pMainWnd->PostMessage( WM_COMMAND, ID_VIEW_LIBRARY );
		pMainWnd->PostMessage( WM_COMMAND, ID_LIBRARY_TREE_VIRTUAL );
	}
	else
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Runtime class lookup

void AFXAPI AfxLockGlobals(int nLockType);
void AFXAPI AfxUnlockGlobals(int nLockType);

CRuntimeClass* AfxClassForName(LPCTSTR pszClass)
{
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();

	AfxLockGlobals( 0 );

	for ( CRuntimeClass* pClass = pModuleState->m_classList ; pClass != NULL ; pClass = pClass->m_pNextClass )
	{
		if ( CString( pClass->m_lpszClassName ).CompareNoCase( pszClass ) == 0 )
		{
			AfxUnlockGlobals( 0 );
			return pClass;
		}
	}

	AfxUnlockGlobals( 0 );

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// String functions

void Split(const CString& strSource, TCHAR cDelimiter, CStringArray& pAddIt, BOOL bAddFirstEmpty)
{
	for( LPCTSTR start = strSource; *start; start++ )
	{
		LPCTSTR c = _tcschr( start, cDelimiter );
		int len = c ? (int) ( c - start ) : (int) _tcslen( start );
		if ( len > 0 )
			pAddIt.Add( CString( start, len ) );
		else
			if ( bAddFirstEmpty && ( start == strSource ) )
				pAddIt.Add( CString() );
		if ( ! c )
			break;
		start = c;
	}
}

BOOL LoadString(CString& str, UINT nID)
{
	return Skin.LoadString( str, nID );
}

CString LoadString(UINT nID)
{
	CString str;
	LoadString( str, nID );
	return str;
}

BOOL LoadSourcesString(CString& str, DWORD num, bool bFraction)
{
	if ( bFraction )
	{
		return Skin.LoadString( str, IDS_STATUS_SOURCESOF );
	}
	else if ( num == 0 )
	{
		return Skin.LoadString( str, IDS_STATUS_NOSOURCES );
	}
	else if ( num == 1 )
	{
		return Skin.LoadString( str, IDS_STATUS_SOURCE );
	}
	else if ( ( ( num % 100 ) > 10) && ( ( num % 100 ) < 20 ) )
	{
		return Skin.LoadString( str, IDS_STATUS_SOURCES11TO19 );
	}
	else
	{
		switch ( num % 10 )
		{
			case 0:
				return Skin.LoadString( str, IDS_STATUS_SOURCESTENS );
			case 1:
				return Skin.LoadString( str, IDS_STATUS_SOURCES );
			case 2:
			case 3:
			case 4:
				return Skin.LoadString( str, IDS_STATUS_SOURCES2TO4 );
			default:
				return Skin.LoadString( str, IDS_STATUS_SOURCES5TO9 );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Case independent string search

LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszSubString)
{
	// Return null if string or substring is empty
	if ( !*pszString || !*pszSubString )
		return NULL;

	// Return if string is too small to hold the substring
	size_t nString( _tcslen( pszString ) );
	size_t nSubString( _tcslen( pszSubString ) );
	if ( nString < nSubString )
		return NULL;

	// Get the first character from the substring and lowercase it
	const TCHAR cFirstPatternChar = ToLower( *pszSubString );

	// Loop over the part of the string that the substring could fit into
	LPCTSTR pszCutOff = pszString + nString - nSubString;
	while ( pszString <= pszCutOff )
	{
		// Search for the start of the substring
		while ( pszString <= pszCutOff
			&& ToLower( *pszString ) != cFirstPatternChar )
		{
			++pszString;
		}

		// Exit loop if no match found
		if ( pszString > pszCutOff )
			break;

		// Check the rest of the substring
		size_t nChar( 1 );
		while ( pszSubString[nChar]
			&& ToLower( pszString[nChar] ) == ToLower( pszSubString[nChar] ) )
		{
			++nChar;
		}

		// If substring matched return pointer to the start of the match
		if ( !pszSubString[nChar] )
			return pszString;

		// Move on to next character and continue search
		++pszString;
	}

	// No match found, return null pointer
	return NULL;
}

LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszPattern, size_t plen)
{
	if ( !*pszString || !*pszPattern || !plen ) return NULL;

	const TCHAR cFirstPatternChar = ToLower( *pszPattern );

	for ( ; ; ++pszString )
	{
		while ( *pszString && ToLower( *pszString ) != cFirstPatternChar ) ++pszString;

		if ( !*pszString ) return NULL;

		DWORD i = 0;
		while ( ++i < plen )
		{
			if ( const TCHAR cStringChar = ToLower( pszString[ i ] ) )
			{
				if ( cStringChar != ToLower( pszPattern[ i ] ) ) break;
			}
			else
			{
				return NULL;
			}
		}

		if ( i == plen ) return pszString;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Time Management Functions (C-runtime)

DWORD TimeFromString(LPCTSTR pszTime)
{
	// 2002-04-30T08:30Z

	if ( _tcslen( pszTime ) != 17 ) return 0;
	if ( pszTime[4] != '-' || pszTime[7] != '-' ) return 0;
	if ( pszTime[10] != 'T' || pszTime[13] != ':' || pszTime[16] != 'Z' ) return 0;

	LPCTSTR psz;
	int nTemp;

	tm pTime = {};

	if ( _stscanf( pszTime, _T("%i"), &nTemp ) != 1 ) return 0;
	pTime.tm_year = nTemp - 1900;
	for ( psz = pszTime + 5 ; *psz == '0' ; psz++ );
	if ( _stscanf( psz, _T("%i"), &nTemp ) != 1 ) return 0;
	pTime.tm_mon = nTemp - 1;
	for ( psz = pszTime + 8 ; *psz == '0' ; psz++ );
	if ( _stscanf( psz, _T("%i"), &nTemp ) != 1 ) return 0;
	pTime.tm_mday = nTemp;
	for ( psz = pszTime + 11 ; *psz == '0' ; psz++ );
	if ( _stscanf( psz, _T("%i"), &nTemp ) != 1 ) return 0;
	pTime.tm_hour = nTemp;
	for ( psz = pszTime + 14 ; *psz == '0' ; psz++ );
	if ( _stscanf( psz, _T("%i"), &nTemp ) != 1 ) return 0;
	pTime.tm_min = nTemp;

	time_t tGMT = mktime( &pTime );
	// check for invalid dates
	if (tGMT == -1)
	{
		theApp.Message( MSG_ERROR, _T("Invalid Date/Time"), pszTime );
		return 0;
	}
	struct tm* pGM = gmtime( &tGMT );
	time_t tSub = mktime( pGM );

	if (tSub == -1)
	{
		theApp.Message( MSG_ERROR, _T("Invalid Date/Time"), pszTime );
		return 0;
	}

	return DWORD( 2 * tGMT - tSub );
}

CString TimeToString(time_t tVal)
{
	tm* pTime = gmtime( &tVal );
	CString str;

	str.Format( _T("%.4i-%.2i-%.2iT%.2i:%.2iZ"),
		pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday,
		pTime->tm_hour, pTime->tm_min );

	return str;
}

/////////////////////////////////////////////////////////////////////////////
// Time Management Functions (FILETIME)

BOOL TimeFromString(LPCTSTR pszTime, FILETIME* pTime)
{
	// 2002-04-30T08:30Z

	if ( _tcslen( pszTime ) != 17 ) return FALSE;
	if ( pszTime[4] != '-' || pszTime[7] != '-' ) return FALSE;
	if ( pszTime[10] != 'T' || pszTime[13] != ':' || pszTime[16] != 'Z' ) return FALSE;

	LPCTSTR psz;
	int nTemp;

	SYSTEMTIME pOut = {};

	if ( _stscanf( pszTime, _T("%i"), &nTemp ) != 1 ) return FALSE;
	pOut.wYear = WORD( nTemp );
	for ( psz = pszTime + 5 ; *psz == '0' ; psz++ );
	if ( _stscanf( psz, _T("%i"), &nTemp ) != 1 ) return FALSE;
	pOut.wMonth = WORD( nTemp );
	for ( psz = pszTime + 8 ; *psz == '0' ; psz++ );
	if ( _stscanf( psz, _T("%i"), &nTemp ) != 1 ) return FALSE;
	pOut.wDay = WORD( nTemp );
	for ( psz = pszTime + 11 ; *psz == '0' ; psz++ );
	if ( _stscanf( psz, _T("%i"), &nTemp ) != 1 ) return FALSE;
	pOut.wHour = WORD( nTemp );
	for ( psz = pszTime + 14 ; *psz == '0' ; psz++ );
	if ( _stscanf( psz, _T("%i"), &nTemp ) != 1 ) return FALSE;
	pOut.wMinute = WORD( nTemp );

	return SystemTimeToFileTime( &pOut, pTime );
}

CString	TimeToString(FILETIME* pTime)
{
	SYSTEMTIME pOut;
	CString str;

	FileTimeToSystemTime( pTime, &pOut );

	str.Format( _T("%.4i-%.2i-%.2iT%.2i:%.2iZ"),
		pOut.wYear, pOut.wMonth, pOut.wDay,
		pOut.wHour, pOut.wMinute );

	return str;
}

/////////////////////////////////////////////////////////////////////////////
// Automatic dropdown list width adjustment (to fit translations)
// Use in ON_CBN_DROPDOWN events

void RecalcDropWidth(CComboBox* pWnd)
{
	// Reset the dropped width
	int nNumEntries = pWnd->GetCount();
	int nWidth = 0;
	CString str;

	CClientDC dc( pWnd );
	int nSave = dc.SaveDC();
	dc.SelectObject( pWnd->GetFont() );

	int nScrollWidth = GetSystemMetrics( SM_CXVSCROLL );
	for ( int nEntry = 0; nEntry < nNumEntries; nEntry++ )
	{
		pWnd->GetLBText( nEntry, str );
		int nLength = dc.GetTextExtent( str ).cx + nScrollWidth;
		nWidth = max( nWidth, nLength );
	}

	// Add margin space to the calculations
	nWidth += dc.GetTextExtent( _T("0") ).cx;

	dc.RestoreDC( nSave );
	pWnd->SetDroppedWidth( nWidth );
}

BOOL LoadIcon(LPCTSTR szFilename, HICON* phSmallIcon, HICON* phLargeIcon, HICON* phHugeIcon)
{
	CString strIcon( szFilename );

	if ( phSmallIcon )
		*phSmallIcon = NULL;
	if ( phLargeIcon )
		*phLargeIcon = NULL;
	if ( phHugeIcon )
		*phHugeIcon = NULL;

	int nIndex = strIcon.ReverseFind( _T(',') );
	int nIcon = 0;
	if ( nIndex != -1 )
	{
		if ( _stscanf( strIcon.Mid( nIndex + 1 ), _T("%i"), &nIcon ) != 1 )
			return FALSE;
		strIcon = strIcon.Left( nIndex );
	}
	else
		nIndex = 0;

	if ( strIcon.GetLength() < 3 )
		return FALSE;

	if ( strIcon.GetAt( 0 ) == _T('\"') &&
		 strIcon.GetAt( strIcon.GetLength() - 1 ) == _T('\"') )
		strIcon = strIcon.Mid( 1, strIcon.GetLength() - 2 );

	if ( phLargeIcon || phSmallIcon )
	{
		ExtractIconEx( strIcon, nIcon, phLargeIcon, phSmallIcon, 1 );
	}

	if ( phHugeIcon )
	{
		UINT nLoadedID;
		PrivateExtractIcons( strIcon, nIcon, 48, 48,
			phHugeIcon, &nLoadedID, 1, 0 );
	}

	return ( phLargeIcon && *phLargeIcon ) || ( phSmallIcon && *phSmallIcon ) ||
		( phHugeIcon && *phHugeIcon );
}

int AddIcon(UINT nIcon, CImageList& gdiImageList)
{
	return AddIcon( theApp.LoadIcon( nIcon ), gdiImageList );
}

int AddIcon(HICON hIcon, CImageList& gdiImageList)
{
	int num = -1;
	if ( hIcon )
	{
		if ( Settings.General.LanguageRTL )
			hIcon = CreateMirroredIcon( hIcon );
		num = gdiImageList.Add( hIcon );
		VERIFY( DestroyIcon( hIcon ) );
	}
	return num;
}

HICON CreateMirroredIcon(HICON hIconOrig, BOOL bDestroyOriginal)
{
	HDC hdcScreen, hdcBitmap, hdcMask = NULL;
	HBITMAP hbm, hbmMask, hbmOld,hbmOldMask;
	BITMAP bm;
	ICONINFO ii;
	HICON hIcon = NULL;
	hdcBitmap = CreateCompatibleDC( NULL );
	if ( hdcBitmap )
	{
		hdcMask = CreateCompatibleDC( NULL );
		if( hdcMask )
		{
			SetLayout( hdcBitmap, LAYOUT_RTL );
			SetLayout( hdcMask, LAYOUT_RTL );
		}
		else
		{
			DeleteDC( hdcBitmap );
			hdcBitmap = NULL;
		}
	}
	hdcScreen = GetDC( NULL );
	if ( hdcScreen )
	{
		if ( hdcBitmap && hdcMask )
		{
			if ( hIconOrig )
			{
				if ( GetIconInfo( hIconOrig, &ii ) && GetObject( ii.hbmColor, sizeof(BITMAP), &bm ) )
				{
					// Do the cleanup for the bitmaps.
					DeleteObject( ii.hbmMask );
					DeleteObject( ii.hbmColor );
					ii.hbmMask = ii.hbmColor = NULL;
					hbm = CreateCompatibleBitmap( hdcScreen, bm.bmWidth, bm.bmHeight );
					if ( hbm != NULL )
					{
						hbmMask = CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL );
						if ( hbmMask != NULL )
						{
							hbmOld = (HBITMAP)SelectObject( hdcBitmap, hbm );
							hbmOldMask = (HBITMAP)SelectObject( hdcMask,hbmMask );
							DrawIconEx( hdcBitmap, 0, 0, hIconOrig, bm.bmWidth, bm.bmHeight, 0, NULL, DI_IMAGE );
							DrawIconEx( hdcMask, 0, 0, hIconOrig, bm.bmWidth, bm.bmHeight, 0, NULL, DI_MASK );
							SelectObject( hdcBitmap, hbmOld );
							SelectObject( hdcMask, hbmOldMask );
							// Create the new mirrored icon and delete bitmaps

							ii.hbmMask = hbmMask;
							ii.hbmColor = hbm;
							hIcon = CreateIconIndirect( &ii );
							DeleteObject( hbmMask );
						}
						DeleteObject( hbm );
					}
				}
			}
		}
		ReleaseDC( NULL, hdcScreen );
	}

	if ( hdcBitmap ) DeleteDC( hdcBitmap );
	if ( hdcMask ) DeleteDC( hdcMask );
	if ( hIcon && hIconOrig && bDestroyOriginal ) VERIFY( DestroyIcon( hIconOrig ) );
	if ( ! hIcon ) hIcon = hIconOrig;
	return hIcon;
}

HBITMAP CreateMirroredBitmap(HBITMAP hbmOrig)
{
	HDC hdc, hdcMem1, hdcMem2;
	HBITMAP hbm = NULL, hOld_bm1, hOld_bm2;
	BITMAP bm;
	if ( !hbmOrig ) return NULL;
	if ( !GetObject( hbmOrig, sizeof(BITMAP), &bm ) ) return NULL;

	hdc = GetDC( NULL );
	if ( hdc )
	{
		hdcMem1 = CreateCompatibleDC( hdc );
		if ( !hdcMem1 )
		{
			ReleaseDC( NULL, hdc );
			return NULL;
		}
		hdcMem2 = CreateCompatibleDC( hdc );
		if ( !hdcMem2 )
		{
			DeleteDC( hdcMem1 );
			ReleaseDC( NULL, hdc );
			return NULL;
		}
		hbm = CreateCompatibleBitmap( hdc, bm.bmWidth, bm.bmHeight );
		if (!hbm)
		{
			DeleteDC( hdcMem1 );
			DeleteDC( hdcMem2 );
			ReleaseDC( NULL, hdc );
			return NULL;
		}
		// Flip the bitmap.
		hOld_bm1 = (HBITMAP)SelectObject( hdcMem1, hbmOrig );
		hOld_bm2 = (HBITMAP)SelectObject( hdcMem2, hbm );
		SetLayout( hdcMem2, LAYOUT_RTL );
		BitBlt( hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem1, 0, 0, SRCCOPY );
		SelectObject( hdcMem1, hOld_bm1 );
		SelectObject( hdcMem2, hOld_bm2 );
		DeleteDC( hdcMem1 );
		DeleteDC( hdcMem2 );
		ReleaseDC( NULL, hdc );
	}
	return hbm;
}

/////////////////////////////////////////////////////////////////////////////
// Keyboard hook: record tick count

LRESULT CALLBACK KbdHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if ( nCode == HC_ACTION )
	{
		theApp.m_nLastInput = (DWORD)time( NULL );

		BOOL bAlt = (WORD)( lParam >> 16 ) & KF_ALTDOWN;
		// BOOL bCtrl = GetAsyncKeyState( VK_CONTROL ) & 0x80000000;
		if ( bAlt )
		{
			if ( wParam == VK_DOWN )
				SendMessage( AfxGetMainWnd()->GetSafeHwnd(), WM_SETALPHA, (WPARAM)0, 0 );
			if ( wParam == VK_UP )
				SendMessage( AfxGetMainWnd()->GetSafeHwnd(), WM_SETALPHA, (WPARAM)1, 0 );
		}
	}

	return ::CallNextHookEx( theApp.m_hHookKbd, nCode, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// Mouse hook: record tick count

LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if ( nCode == HC_ACTION )
		theApp.m_nLastInput = (DWORD)time( NULL );

	return ::CallNextHookEx( theApp.m_hHookMouse, nCode, wParam, lParam );
}

CString GetFolderPath( int nFolder )
{
	TCHAR pszFolderPath[ MAX_PATH ] = { 0 };

	if ( SUCCEEDED( SHGetFolderPath( NULL, nFolder, NULL, NULL, pszFolderPath ) ) )
		return CString( pszFolderPath );

	return _T("");
}

CString GetWindowsFolder()
{
	TCHAR pszWindowsPath[ MAX_PATH ] = { 0 };
	UINT nReturnValue = GetWindowsDirectory( pszWindowsPath, MAX_PATH );
	if ( nReturnValue == 0 || nReturnValue > MAX_PATH ) return CString( _T("c:\\windows") );

	CharLower( pszWindowsPath );
	return CString( pszWindowsPath );
}

CString GetProgramFilesFolder()
{
	TCHAR pszProgramsPath[ MAX_PATH ] = { 0 };
	BOOL bOK = FALSE;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_PROGRAM_FILES, NULL, NULL, pszProgramsPath ) ) )
		bOK = TRUE;

	if ( !bOK || ! *pszProgramsPath )
	{
		// Get drive letter
		UINT nReturnValue = GetWindowsDirectory( pszProgramsPath, MAX_PATH );
		if ( nReturnValue == 0 || nReturnValue > MAX_PATH ) return CString( _T("c:\\program files") );

		_tcscpy( pszProgramsPath + 1, _T(":\\program files") );
	}

	CharLower( pszProgramsPath );
	return CString( pszProgramsPath );
}

CString GetDocumentsFolder()
{
	CString strDocumentsPath( GetFolderPath( CSIDL_PERSONAL ) );

	if ( !strDocumentsPath.GetLength() )
	{
		strDocumentsPath = CRegistry::GetString( _T("Shell Folders"), _T("Personal"), _T(""), _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer") );
	}
	ASSERT( strDocumentsPath.GetLength() );

	CharLower( strDocumentsPath.GetBuffer() );
	strDocumentsPath.ReleaseBuffer();

	return strDocumentsPath;
}

CString GetAppDataFolder()
{
	CString strAppDataPath( GetFolderPath( CSIDL_APPDATA ) );

	if ( !strAppDataPath.GetLength() )
	{
		strAppDataPath = CRegistry::GetString( _T("Shell Folders"), _T("AppData"), _T(""), _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer") );
	}
	ASSERT( strAppDataPath.GetLength() );

	CharLower( strAppDataPath.GetBuffer() );
	strAppDataPath.ReleaseBuffer();

	return strAppDataPath;
}

CString GetLocalAppDataFolder()
{
	CString strLocalAppDataPath( GetFolderPath( CSIDL_LOCAL_APPDATA ) );

	if ( !strLocalAppDataPath.GetLength() )
	{
		strLocalAppDataPath = CRegistry::GetString( _T("Shell Folders"), _T("Local AppData"), _T(""), _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer") );

		if ( !strLocalAppDataPath.GetLength() )
			return GetAppDataFolder();
	}
	ASSERT( strLocalAppDataPath.GetLength() );

	CharLower( strLocalAppDataPath.GetBuffer() );
	strLocalAppDataPath.ReleaseBuffer();

	return strLocalAppDataPath;
}

BOOL CreateDirectory(LPCTSTR szPath)
{
	DWORD dwAttr = GetFileAttributes( CString( _T("\\\\?\\") ) + szPath );
	if ( ( dwAttr != INVALID_FILE_ATTRIBUTES ) &&
		( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) )
		return TRUE;

	CString strDir( szPath );
	for ( int nStart = 3; ; )
	{
		int nSlash = strDir.Find( _T('\\'), nStart );
		if ( ( nSlash == -1 ) || ( nSlash == strDir.GetLength() - 1 ) )
			break;
		CString strSubDir( strDir.Left( nSlash + 1 ) );
		dwAttr = GetFileAttributes( CString( _T("\\\\?\\") ) + strSubDir );
		if ( ( dwAttr == INVALID_FILE_ATTRIBUTES ) ||
			! ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) )
			if ( ! CreateDirectory( CString( _T("\\\\?\\") ) + strSubDir, NULL ) )
				return FALSE;
		nStart = nSlash + 1;
	}
	return CreateDirectory( CString( _T("\\\\?\\") ) + szPath, NULL );
}

BOOL DeleteFileEx(LPCTSTR szFileName, BOOL bShared, BOOL bToRecycleBin, BOOL bEnableDelayed)
{
	// Should be double zeroed long path
	DWORD len = GetLongPathName( szFileName, NULL, 0 );
	if ( len )
	{
		auto_array< TCHAR > szPath( new TCHAR[ len + 1 ] );
		GetLongPathName( szFileName, szPath.get(), len );
		szPath[ len ] = 0;

		DWORD dwAttr = GetFileAttributes( szPath.get() );
		if ( ( dwAttr != INVALID_FILE_ATTRIBUTES ) &&		// Filename exists
			( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) == 0 )	// Not a folder
		{
			if ( bShared )
			{
				// Stop builder
				LibraryBuilder.Remove( szPath.get() );

				// Stop uploads
				while( ! Uploads.OnRename( szPath.get(), NULL, true ) );
			}

			if ( bToRecycleBin )
			{
				SHFILEOPSTRUCT sfo = {};
				sfo.hwnd = GetDesktopWindow();
				sfo.wFunc = FO_DELETE;
				sfo.pFrom = szPath.get();
				sfo.fFlags = FOF_ALLOWUNDO | FOF_FILESONLY | FOF_NORECURSION | FOF_NO_UI;
				SHFileOperation( &sfo );
			}
			else
				DeleteFile( szPath.get() );

			dwAttr = GetFileAttributes( szPath.get() );
			if ( dwAttr != INVALID_FILE_ATTRIBUTES )
			{
				// File still exists
				if ( bEnableDelayed )
				{
					// Set delayed deletion
					CString sJob;
					sJob.Format( _T("%d%d"), bShared, bToRecycleBin );
					theApp.WriteProfileString( _T("Delete"), szPath.get(), sJob );
				}
				return FALSE;
			}
		}

		// Cancel delayed deletion (if any)
		theApp.WriteProfileString( _T("Delete"), szPath.get(), NULL );
	}

	return TRUE;
}

void PurgeDeletes()
{
	HKEY hKey = NULL;
	LSTATUS nResult = RegOpenKeyEx( HKEY_CURRENT_USER,
		_T("Software\\PeerProject\\PeerProject\\Delete"), 0, KEY_ALL_ACCESS, &hKey );
	if ( ERROR_SUCCESS == nResult )
	{
		CList< CString > pRemove;
		for ( DWORD nIndex = 0 ; ; ++nIndex )
		{
			DWORD nPath = MAX_PATH * 2;
			TCHAR szPath[ MAX_PATH * 2 ] = {};
			DWORD nType;
			DWORD nMode = 8;
			TCHAR szMode[ 8 ] = {};
			nResult = RegEnumValue( hKey, nIndex, szPath, &nPath, 0,
				&nType, (LPBYTE)szMode, &nMode );
			if ( ERROR_SUCCESS != nResult )
				break;

			BOOL bShared = ( nType == REG_SZ ) && ( szMode[ 0 ] == '1' );
			BOOL bToRecycleBin = ( nType == REG_SZ ) && ( szMode[ 1 ] == '1' );
			if ( DeleteFileEx( szPath, bShared, bToRecycleBin, TRUE ) )
			{
				pRemove.AddTail( szPath );
			}
		}

		while ( ! pRemove.IsEmpty() )
		{
			nResult = RegDeleteValue( hKey, pRemove.RemoveHead() );
		}

		nResult = RegCloseKey( hKey );
	}
}

CString LoadHTML(HINSTANCE hInstance, UINT nResourceID)
{
	CString strBody;
	BOOL bGZIP = FALSE;
	HRSRC hRes = FindResource( hInstance, MAKEINTRESOURCE( nResourceID ), RT_HTML );
	if ( ! hRes )
	{
		hRes = FindResource( hInstance, MAKEINTRESOURCE( nResourceID ), RT_GZIP );
		bGZIP = ( hRes != NULL );
	}
	if ( hRes )
	{
		DWORD nSize			= SizeofResource( hInstance, hRes );
		HGLOBAL hMemory		= LoadResource( hInstance, hRes );
		if ( hMemory )
		{
			LPCSTR pszInput	= (LPCSTR)LockResource( hMemory );
			if ( pszInput )
			{
				if ( bGZIP )
				{
					CBuffer buf;
					buf.Add( pszInput, nSize );
					if ( buf.Ungzip() )
					{
						int nWide = MultiByteToWideChar( 0, 0, (LPCSTR)buf.m_pBuffer, buf.m_nLength, NULL, 0 );
						LPTSTR pszOutput = strBody.GetBuffer( nWide + 1 );
						MultiByteToWideChar( 0, 0, (LPCSTR)buf.m_pBuffer, buf.m_nLength, pszOutput, nWide );
						pszOutput[ nWide ] = _T('\0');
						strBody.ReleaseBuffer();
					}
				}
				else
				{
					int nWide = MultiByteToWideChar( 0, 0, pszInput, nSize, NULL, 0 );
					LPTSTR pszOutput = strBody.GetBuffer( nWide + 1 );
					MultiByteToWideChar( 0, 0, pszInput, nSize, pszOutput, nWide );
					pszOutput[ nWide ] = _T('\0');
					strBody.ReleaseBuffer();
				}
			}
			FreeResource( hMemory );
		}
	}
	return strBody;
}

const struct
{
	LPCTSTR szPath;
	UINT	nID;
	LPCTSTR	szType;
	LPCTSTR	szContentType;
} WebResources [] =
{
	{ _T("/remote/header_1.png"),	IDR_HOME_HEADER_1,	RT_PNG,			_T("image/png") },
	{ _T("/remote/header_2.png"),	IDR_HOME_HEADER_2,	RT_PNG,			_T("image/png") },
	{ _T("/favicon.ico"),			IDR_MAINFRAME,		RT_GROUP_ICON,	_T("image/x-icon") },
	{ NULL, NULL, NULL, NULL }
};

bool ResourceRequest(const CString& strPath, CBuffer& pResponse, CString& sHeader)
{
	bool ret = false;
	for ( int i = 0; WebResources[ i ].szPath; i++ )
	{
		if ( strPath.Compare( WebResources[ i ].szPath ) == 0 )
		{
			HMODULE hModule = AfxGetResourceHandle();
			if ( HRSRC hRes = FindResource( hModule,
				MAKEINTRESOURCE( WebResources[ i ].nID ), WebResources[ i ].szType ) )
			{
				DWORD nSize			= SizeofResource( hModule, hRes );
				HGLOBAL hMemory		= LoadResource( hModule, hRes );
				if ( hMemory )
				{
					BYTE* pSource	= (BYTE*)LockResource( hMemory );
					if ( pSource )
					{
						if ( WebResources[ i ].szType == RT_GROUP_ICON )
						{
							// Save main header
							ICONDIR* piDir = (ICONDIR*)pSource;
							DWORD dwTotalSize = sizeof( ICONDIR ) +
								sizeof( ICONDIRENTRY ) * piDir->idCount;
							pResponse.EnsureBuffer( dwTotalSize );
							CopyMemory( pResponse.m_pBuffer, piDir, sizeof( ICONDIR ) );

							GRPICONDIRENTRY* piDirEntry = (GRPICONDIRENTRY*)
								( pSource + sizeof( ICONDIR ) );

							// Find all subicons
							for ( WORD i = 0; i < piDir->idCount; i++ )
							{
								// pResponse.m_pBuffer may be changed
								ICONDIRENTRY* piEntry = (ICONDIRENTRY*)
									( pResponse.m_pBuffer + sizeof( ICONDIR ) );

								// Load subicon
								HRSRC hResIcon = FindResource( hModule, MAKEINTRESOURCE(
									piDirEntry[ i ].nID ), RT_ICON );
								if ( hResIcon )
								{
									DWORD nSizeIcon = SizeofResource( hModule, hResIcon );
									HGLOBAL hMemoryIcon = LoadResource( hModule, hResIcon );
									if ( hMemoryIcon )
									{
										BITMAPINFOHEADER* piImage = (BITMAPINFOHEADER*)LockResource( hMemoryIcon );

										// Fill subicon header
										piEntry[ i ].bWidth = piDirEntry[ i ].bWidth;
										piEntry[ i ].bHeight = piDirEntry[ i ].bHeight;
										piEntry[ i ].wPlanes = piDirEntry[ i ].wPlanes;
										piEntry[ i ].bColorCount = piDirEntry[ i ].bColorCount;
										piEntry[ i ].bReserved = 0;
										piEntry[ i ].wBitCount = piDirEntry[ i ].wBitCount;
										piEntry[ i ].dwBytesInRes = nSizeIcon;
										piEntry[ i ].dwImageOffset = dwTotalSize;

										// Save subicon
										pResponse.EnsureBuffer( dwTotalSize + nSizeIcon );
										CopyMemory( pResponse.m_pBuffer + dwTotalSize,
											piImage, nSizeIcon );
										dwTotalSize += nSizeIcon;

										FreeResource( hMemoryIcon );
									}
								}
							}
							pResponse.m_nLength = dwTotalSize;
						}
						else
						{
							pResponse.EnsureBuffer( nSize );
							CopyMemory( pResponse.m_pBuffer, pSource, nSize );
							pResponse.m_nLength = nSize;
						}
						sHeader.Format(
							_T("Content-Type: %s\r\n"),
							WebResources[ i ].szContentType);
						ret = true;
					}
					FreeResource( hMemory );
				}
			}
			break;
		}
	}
	return ret;
}

bool MarkFileAsDownload(const CString& sFilename)
{
	LPCTSTR pszExt = PathFindExtension( (LPCTSTR)sFilename );
	if ( pszExt == NULL ) return false;

	bool bSuccess = false;

	if ( Settings.Library.MarkFileAsDownload )
	{
		// TODO: pFile->m_bVerify and IDS_LIBRARY_VERIFY_FIX warning features could be merged
		// with this function, because they resemble the security warning.
		// Should raza unblock files from the application without forcing user to do that manually?
		if ( CFileExecutor::IsSafeExecute( pszExt ) )
			return false;

		// Temporary clear R/O attribute
		BOOL bChanged = FALSE;
		DWORD dwOrigAttr = GetFileAttributes( sFilename );
		if ( dwOrigAttr != INVALID_FILE_ATTRIBUTES && ( dwOrigAttr & FILE_ATTRIBUTE_READONLY ) )
			bChanged = SetFileAttributes( sFilename, dwOrigAttr & ~FILE_ATTRIBUTE_READONLY );

		HANDLE hFile = CreateFile( sFilename + _T(":Zone.Identifier"),
			GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			DWORD dwWritten = 0;
			bSuccess = ( WriteFile( hFile, "[ZoneTransfer]\r\nZoneID=3\r\n", 26,
				&dwWritten, NULL ) && dwWritten == 26 );
			CloseHandle( hFile );
		}
		else
			TRACE( "MarkFileAsDownload() : CreateFile \"%s\" error %d\n", sFilename, GetLastError() );

		if ( bChanged )
			SetFileAttributes( sFilename, dwOrigAttr );
	}
	return bSuccess;
}

bool LoadGUID(const CString& sFilename, Hashes::Guid& oGUID)
{
	bool bSuccess = false;
	if ( Settings.Library.UseFolderGUID )
	{
		HANDLE hFile = CreateFile( sFilename + _T(":PeerProject.GUID"),
			GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			Hashes::Guid oTmpGUID;
			DWORD dwReaded = 0;
			bSuccess = ( ReadFile( hFile, oTmpGUID.begin(), oTmpGUID.byteCount,
				&dwReaded, NULL ) && dwReaded == oTmpGUID.byteCount );
			if ( bSuccess )
			{
				oTmpGUID.validate();
				oGUID = oTmpGUID;
			}
			CloseHandle( hFile );
		}
	}
	return bSuccess;
}

bool SaveGUID(const CString& sFilename, const Hashes::Guid& oGUID)
{
	bool bSuccess = false;
	if ( Settings.Library.UseFolderGUID )
	{
		// Temporary clear R/O attribute
		BOOL bChanged = FALSE;
		DWORD dwOrigAttr = GetFileAttributes( sFilename );
		if ( dwOrigAttr != 0xffffffff && ( dwOrigAttr & FILE_ATTRIBUTE_READONLY ) )
			bChanged = SetFileAttributes( sFilename, dwOrigAttr & ~FILE_ATTRIBUTE_READONLY );

		HANDLE hFile = CreateFile( sFilename + _T(":PeerProject.GUID"),
			GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			DWORD dwWritten = 0;
			bSuccess = ( WriteFile( hFile, oGUID.begin(), oGUID.byteCount,
				&dwWritten, NULL ) && dwWritten == oGUID.byteCount );
			CloseHandle( hFile );
		}
		else
			TRACE( "SaveGUID() : CreateFile \"%s\" error %d\n", sFilename, GetLastError() );

		if ( bChanged )
			SetFileAttributes( sFilename, dwOrigAttr );
	}
	return bSuccess;
}

CString ResolveShortcut(LPCTSTR lpszFileName)
{
	CComPtr< IShellLink > pIShellLink;
	if ( SUCCEEDED( pIShellLink.CoCreateInstance( CLSID_ShellLink ) ) )
	{
		CComPtr< IPersistFile > pIPersistFile;
		pIPersistFile = pIShellLink;
		if ( pIPersistFile &&
			SUCCEEDED( pIPersistFile->Load( CComBSTR( lpszFileName ), STGM_READ ) ) &&
			SUCCEEDED( pIShellLink->Resolve( AfxGetMainWnd()->GetSafeHwnd(), SLR_NO_UI |
			SLR_NOUPDATE | SLR_NOSEARCH | SLR_NOTRACK | SLR_NOLINKINFO ) ) )
		{
			CString sPath;
			BOOL bResult = SUCCEEDED( pIShellLink->GetPath( sPath.GetBuffer( MAX_PATH ),
				MAX_PATH, NULL, 0 ) );
			sPath.ReleaseBuffer();
			if ( bResult )
				return sPath;
		}
	}
	return CString();
}

// BrowseCallbackProc - BrowseForFolder callback function
static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch ( uMsg )
	{
	case BFFM_INITIALIZED:
		{
			// Remove context help button from dialog caption
			SetWindowLongPtr( hWnd, GWL_STYLE,
				GetWindowLongPtr( hWnd, GWL_STYLE ) & ~DS_CONTEXTHELP );
			SetWindowLongPtr( hWnd, GWL_EXSTYLE,
				GetWindowLongPtr( hWnd, GWL_EXSTYLE ) & ~WS_EX_CONTEXTHELP );

			// Set initial directory
			SendMessage( hWnd, BFFM_SETSELECTION, TRUE, lpData );
		}
		break;

	case BFFM_SELCHANGED:
		{
			// Fail if non-filesystem
			TCHAR szDir[ MAX_PATH ] = {};
			BOOL bResult = SHGetPathFromIDList( (LPITEMIDLIST)lParam, szDir );
			if ( bResult )
			{
				// Fail if folder not accessible
				bResult = ( _taccess( szDir, 4 ) == 0 );
				if ( bResult )
				{
					// Fail if pidl is a link
					SHFILEINFO sfi = {};
					bResult = ( SHGetFileInfo( (LPCTSTR)lParam, 0, &sfi, sizeof( sfi ),
						SHGFI_PIDL | SHGFI_ATTRIBUTES ) &&
						( sfi.dwAttributes & SFGAO_LINK ) == 0 );
				}
			}
			SendMessage( hWnd, BFFM_ENABLEOK, 0, bResult );
		}
		break;
	}
	return 0;
}

// Displays a dialog box enabling the user to select a Shell folder
CString BrowseForFolder(UINT nTitle, LPCTSTR szInitialPath, HWND hWnd)
{
	CString strTitle;
	LoadString( strTitle, nTitle );
	return BrowseForFolder( strTitle, szInitialPath, hWnd );
}

// Displays a dialog box enabling the user to select a Shell folder
CString BrowseForFolder(LPCTSTR szTitle, LPCTSTR szInitialPath, HWND hWnd)
{
	// Get last used folder
	static TCHAR szDefaultPath[ MAX_PATH ] = {};
	if ( ! szInitialPath || ! *szInitialPath )
	{
		if ( ! *szDefaultPath )
			lstrcpyn( szDefaultPath, (LPCTSTR)GetDocumentsFolder(), MAX_PATH );
		szInitialPath = szDefaultPath;
	}

	TCHAR szDisplayName[ MAX_PATH ] = {};
	BROWSEINFO pBI = {};
	pBI.hwndOwner = hWnd ? hWnd : AfxGetMainWnd()->GetSafeHwnd();
	pBI.pszDisplayName = szDisplayName;
	pBI.lpszTitle = szTitle;
	pBI.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
	pBI.lpfn = BrowseCallbackProc;
	pBI.lParam = (LPARAM)szInitialPath;
	LPITEMIDLIST pPath = SHBrowseForFolder( &pBI );
	if ( pPath == NULL )
		return CString();

	TCHAR szPath[ MAX_PATH ] = {};
	BOOL bResult = SHGetPathFromIDList( pPath, szPath );

	CComPtr< IMalloc > pMalloc;
	if ( SUCCEEDED( SHGetMalloc( &pMalloc ) ) )
		pMalloc->Free( pPath );

	if ( ! bResult )
		return CString();

	// Save last used folder
	lstrcpyn( szDefaultPath, szPath, MAX_PATH );

	return CString( szPath );
}

BOOL PostMainWndMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if ( CMainWnd* pWnd = theApp.SafeMainWnd() )
		return pWnd->PostMessage( Msg, wParam, lParam );
	else
		return FALSE;
}

void SafeMessageLoop()
{
	InterlockedIncrement( &theApp.m_bBusy );
	__try
	{
		MSG msg;
		while ( PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if ( GetWindowThreadProcessId( AfxGetMainWnd()->GetSafeHwnd(), NULL ) ==
			GetCurrentThreadId() )
		{
			LONG lIdle = 0;
			while ( AfxGetApp()->OnIdle( lIdle++ ) );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	InterlockedDecrement( &theApp.m_bBusy );
}
