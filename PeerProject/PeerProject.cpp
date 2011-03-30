//
// PeerProject.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "CoolInterface.h"
#include "BTInfo.h"
#include "BTClients.h"
#include "DCClients.h"
#include "EDClients.h"
#include "DDEServer.h"
#include "DiscoveryServices.h"
#include "DlgDeleteFile.h"
#include "DownloadGroups.h"
#include "Downloads.h"
#include "Emoticons.h"
#include "FileExecutor.h"
#include "Firewall.h"
#include "Flags.h"
#include "FontManager.h"
#include "GProfile.h"
#include "HostCache.h"
#include "IEProtocol.h"
#include "ImageServices.h"
#include "ImageFile.h"	// AfxMsgBox Banners
#include "Library.h"
#include "LibraryBuilder.h"
#include "Network.h"
#include "Plugins.h"
#include "PeerProjectURL.h"
#include "QueryHashMaster.h"
#include "Registry.h"
#include "Revision.h"	//.svn
#include "Scheduler.h"
#include "SchemaCache.h"
#include "Security.h"
#include "SharedFile.h"
#include "ShellIcons.h"
#include "Skin.h"
#include "SQLite.h"
#include "ThumbCache.h"
#include "Transfers.h"
#include "UPnPFinder.h"
#include "UploadQueues.h"
#include "Uploads.h"
#include "VendorCache.h"
#include "VersionChecker.h"

#include "DlgHelp.h"
#include "DlgSplash.h"
#include "WndMain.h"
#include "WndMedia.h"
#include "WndSystem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename


/////////////////////////////////////////////////////////////////////////////
// CPeerProjectCommandLineInfo

CPeerProjectCommandLineInfo::CPeerProjectCommandLineInfo()
	: m_bTray		( FALSE )
	, m_bHelp		( FALSE )
//	, m_bWait		( FALSE )
	, m_bNoSplash	( FALSE )
	, m_bNoAlphaWarning ( FALSE )
	, m_nGUIMode	( -1 )
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
		if ( ! lstrcmpi( pszParam, _T("nosplash") ) )
		{
			m_bNoSplash = TRUE;
			return;
		}
		if ( ! lstrcmpi( pszParam, _T("nowarn") ) )
		{
			m_bNoAlphaWarning = TRUE;
			return;
		}
		if ( ! lstrcmpi( pszParam, _T("noskin") ) )
		{
			ClearSkins();
			return;
		}
		if ( ! lstrcmpi( pszParam, _T("basic") ) )
		{
			m_nGUIMode = GUI_BASIC;
			return;
		}
		if ( ! lstrcmpi( pszParam, _T("tabbed") ) )
		{
			m_nGUIMode = GUI_TABBED;
			return;
		}
		if ( ! lstrcmpi( pszParam, _T("windowed") ) )
		{
			m_nGUIMode = GUI_WINDOWED;
			return;
		}
	//	if ( ! lstrcmpi( pszParam, _T("wait") ) )
	//	{
	//		m_bWait = TRUE;
	//		return;
	//	}
		if ( ! lstrcmpi( pszParam, _T("help") ) || ! lstrcmpi( pszParam, _T("?") ) )
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

CPeerProjectApp::CPeerProjectApp()
	: m_pMutex					( NULL )
	, m_pSafeWnd				( NULL )
	, m_bBusy					( 0 )
	, m_bClosing				( false )
	, m_bLive					( false )
	, m_bInteractive			( false )
	, m_bIsServer				( false )
	, m_bIsWin2000				( false )
	, m_bIsVistaOrNewer			( false )
	, m_bLimitedConnections 	( true )
	, m_nWindowsVersion			( 0ul )
	, m_nWindowsVersionMinor	( 0ul )
	, m_nPhysicalMemory			( 0ull )
	, m_bUPnPPortsForwarded 	( TRI_UNKNOWN )
	, m_bUPnPDeviceConnected	( TRI_UNKNOWN )
	, m_bMenuWasVisible			( FALSE )
	, m_nLastInput				( 0ul )
	, m_hHookKbd				( NULL )
	, m_hHookMouse				( NULL )
	, m_nMouseWheel 			( 3 )
	, m_nFontQuality			( DEFAULT_QUALITY )

	, m_hCryptProv				( NULL )
	, m_pRegisterApplicationRestart( NULL )

	, m_dlgSplash				( NULL )
	, m_hTheme					( NULL )
	, m_pfnSetWindowTheme		( NULL )
	, m_pfnIsThemeActive		( NULL )
	, m_pfnOpenThemeData		( NULL )
	, m_pfnCloseThemeData		( NULL )
	, m_pfnDrawThemeBackground	( NULL )

	, m_hShlWapi				( NULL )
	, m_pfnAssocIsDangerous 	( NULL )

	, m_hShell32				( NULL )
	, m_pfnSHGetFolderPathW 	( NULL )
	, m_pfnSHGetKnownFolderPath	( NULL )

	, m_hLibGFL					( NULL )
	, m_hGeoIP					( NULL )
	, m_pGeoIP					( NULL )
	, m_pfnGeoIP_delete			( NULL )
	, m_pfnGeoIP_country_code_by_ipnum( NULL )
	, m_pfnGeoIP_country_name_by_ipnum( NULL )

	, m_SysInfo					( )
{
	ZeroMemory( m_nVersion, sizeof( m_nVersion ) );
	ZeroMemory( m_pBTVersion, sizeof( m_pBTVersion ) );
	m_nUPnPExternalAddress.s_addr = INADDR_NONE;

#if defined(_MSC_VER) && (_MSC_VER >= 1500)	// No VS2005
	AfxSetPerUserRegistration( TRUE );
#endif

// BugTrap (www.intellesoft.net)
#ifdef _DEBUG
	BT_InstallSehFilter();
	BT_SetTerminate();
//	BT_SetAppName( CLIENT_NAME );		// Below
//	BT_SetAppVersion( m_sVersionLong );	// Below
	BT_SetFlags( BTF_INTERCEPTSUEF | BTF_SHOWADVANCEDUI | BTF_DESCRIBEERROR |
		BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_EDITMAIL );
	BT_SetSupportURL( _T("http://peerproject.org") );
	BT_SetSupportEMail( _T("peerprojectreports@lists.sourceforge.net") );
//	BT_SetSupportServer( _T("http://bugtrap.peerproject.org/RequestHandler.aspx"), 80 );
	BT_AddRegFile( _T("Settings.reg"), _T("HKEY_CURRENT_USER\\") REGISTRY_KEY );
#endif
}

CPeerProjectApp::~CPeerProjectApp()
{
	if ( m_pMutex != NULL )
		CloseHandle( m_pMutex );
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp initialization

BOOL CPeerProjectApp::InitInstance()
{
	CWinApp::InitInstance();

	SetRegistryKey( CLIENT_NAME );

	AfxOleInit();				// Initializes OLE support for the application.

	GetVersionNumber();
	Settings.Load();			// Loads settings. Depends on GetVersionNumber()
	InitResources();			// Loads theApp settings. Depends on Settings::Load()
	CoolInterface.Load();		// Loads colors and fonts. Depends on InitResources()

//	m_pFontManager = new CFontManager();
//	AfxEnableControlContainer( m_pFontManager );
	AfxEnableControlContainer();

	LoadStdProfileSettings();
	EnableShellOpen();
//	RegisterShellFileTypes();

	ParseCommandLine( m_cmdInfo );
	if ( m_cmdInfo.m_bHelp )
	{
		// Unskinned Banner Workaround:
		Skin.m_bmBanner.Attach( CImageFile::LoadBitmapFromResource( IDB_BANNER ) );
		Skin.m_nBanner = 50;

		AfxMessageBox( //IDS_COMMANDLINE,	// No translation
			_T("\nPeerProject command-line options:\n\n")
			_T(" -help   -? \tDisplay this help screen\n")
			_T(" -tray\t\tStart application quietly in system tray\n")
			_T(" -nosplash\tDisable startup splash screen\n")
			_T(" -nowarn\t\tSkip debug version warning dialog\n")
			_T(" -noskin\t\tDisable all skins and languages\n")
			_T(" -basic\t\tStart application in Basic mode\n")
			_T(" -tabbed\t\tStart application in Tabbed mode\n")
			_T(" -windowed\tStart application in Windowed mode\n")
			_T(" -regserver\tRegister application internal components\n")
			_T(" -unregserver\tUn-register application internal components\n\n"),
			MB_ICONINFORMATION | MB_OK );
		return FALSE;
	}
	if ( m_cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister )
	{
		// Do not call this ->
		// ProcessShellCommand( m_cmdInfo );
		// ... else all INI settings will be deleted (by design)

		// Do not call this ->
		// AfxOleUnregisterTypeLib( _tlid, _wVerMajor, _wVerMinor );
		// COleTemplateServer::UnregisterAll();
		// COleObjectFactory::UpdateRegistryAll( FALSE );
		// ... else OLE interface settings may be deleted (bug in MFC?)
		return FALSE;
	}
	if ( m_cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister )
		ProcessShellCommand( m_cmdInfo );

	AfxOleRegisterTypeLib( AfxGetInstanceHandle(), _tlid );
	COleTemplateServer::RegisterAll();
	COleObjectFactory::UpdateRegistryAll( TRUE );
	if ( m_cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister )
		return FALSE;

	m_pMutex = CreateMutex( NULL, FALSE, _T("Global\\PeerProject") );
	if ( m_pMutex == NULL )
		return FALSE;		// Mutex probably created in another multi-user session

	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		CloseHandle( m_pMutex );
		m_pMutex = NULL;

		// Show first instance instead
		if ( CWnd* pWnd = CWnd::FindWindow( _T("PeerProjectMainWnd"), NULL ) )
		{
			pWnd->SendMessage( WM_SYSCOMMAND, SC_RESTORE );
			pWnd->ShowWindow( SW_SHOWNORMAL );
			pWnd->BringWindowToTop();
			pWnd->SetForegroundWindow();
		}

		return FALSE;
	}
	// else only app instance, continue.

	if ( m_pRegisterApplicationRestart )
		m_pRegisterApplicationRestart( NULL, 0 );

	ShowStartupText();


	// *****************
	// NO PUBLIC RELEASE
	// Remove this section for final releases and public betas.

#if defined(_DEBUG) || defined(__REVISION__)		// Show for "pre-release release builds."

	// Unskinned Banner Workaround:
	Skin.m_bmBanner.Attach( CImageFile::LoadBitmapFromResource( IDB_BANNER ) );
	Skin.m_nBanner = 50;


	// BETA EXPIRATION.  Remember to re-compile to update the time.

	COleDateTime tCurrent = COleDateTime::GetCurrentTime();
	COleDateTime tCompileTime;
	tCompileTime.ParseDateTime( _T(__DATE__), LOCALE_NOUSEROVERRIDE, 1033 );
#ifdef _DEBUG
	COleDateTimeSpan tTimeOut( 21, 0, 0, 0);		// Daily debug builds
#else
	COleDateTimeSpan tTimeOut( 45, 0, 0, 0);		// Forum Betas (Non-sourceforge release)
#endif

	if ( ( tCompileTime + tTimeOut ) < tCurrent )
	{
		CString strMessage;
		LoadString( strMessage, IDS_BETA_EXPIRED);
		AfxMessageBox( strMessage, MB_ICONQUESTION|MB_OK );
	}


	// ALPHA WARNING.  Remember to remove this section for public betas.

	if ( ! m_cmdInfo.m_bNoAlphaWarning && m_cmdInfo.m_bShowSplash )
	{
		if ( AfxMessageBox(
			L"\nWARNING: This is an ALPHA TEST version of PeerProject p2p"
 #ifdef __REVISION__
			L", r" _T(__REVISION__)
 #endif
			L".\n\nNOT FOR GENERAL USE, it is intended for pre-release testing in controlled environments.  "
			L"It may stop running or display Debug info for testing.\n\n"
			L"If you wish to simply use this software, then download the current\n"
			L"stable release from PeerProject.org.  If you continue past this point,\n"
			L"you could possibly experience system instability or lose files.\n"
			L"Please be aware of recent development before using.\n\n"
			L"Do you wish to continue?", MB_ICONEXCLAMATION|MB_YESNO|MB_SETFOREGROUND ) == IDNO )
				return FALSE;
	}

#endif	// _DEBUG/__REVISION__

	// END NO PUBLIC RELEASE
	// *********************

	m_bInteractive = true;

	// Show Startup Splash Screen

	const int nSplashSteps = 18
		+ ( Settings.Connection.EnableFirewallException ? 1 : 0 )
		+ ( Settings.Connection.EnableUPnP ? 1 : 0 );

	SplashStep( L"Up", ( ( m_cmdInfo.m_bNoSplash || ! m_cmdInfo.m_bShowSplash ) ? 0 : nSplashSteps ), false );
		if ( m_cmdInfo.m_nGUIMode != -1 )
			Settings.General.GUIMode = m_cmdInfo.m_nGUIMode;
		if ( Settings.General.GUIMode != GUI_WINDOWED && Settings.General.GUIMode != GUI_TABBED && Settings.General.GUIMode != GUI_BASIC )
			Settings.General.GUIMode = GUI_BASIC;

		if ( Settings.Live.FirstRun )
			Plugins.Register();

		DDEServer.Create();
		IEProtocol.Create();

		PurgeDeletes();

	SplashStep( L"Network Winsock" );
		WSADATA wsaData;
		for ( int i = 1 ; i <= 2 ; i++ )
		{
			if ( WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) ) return FALSE;
			if ( wsaData.wVersion == MAKEWORD( 1, 1 ) ) break;
			if ( i == 2 ) return FALSE;
			WSACleanup();
		}
		CryptAcquireContext( &m_hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT );

	SplashStep( L"P2P URIs" );
		CPeerProjectURL::Register( TRUE );
	SplashStep( L"Profile" );
		MyProfile.Load();
	SplashStep( L"Vendor Data" );
		VendorCache.Load();
	SplashStep( L"Security Services" );
		Security.Load();
		AdultFilter.Load();
		MessageFilter.Load();
	SplashStep( L"Discovery Services" );
		DiscoveryServices.Load();
	SplashStep( L"Host Cache" );
		HostCache.Load();
	SplashStep( L"Query Manager" );
		QueryHashMaster.Create();
	SplashStep( L"Scheduler" );
		Scheduler.Load();
	SplashStep( L"Shell Icons" );
		ShellIcons.Clear();
		if ( ! Emoticons.Load() )
			Message( MSG_ERROR, _T("Failed to load Emoticons.") );
		if ( ! Flags.Load() )
			Message( MSG_ERROR, _T("Failed to load Flags.") );
	SplashStep( L"Metadata Schemas" );
		if ( SchemaCache.Load() < 48 )	// Presumed number of .xsd files in Schemas folder
		{
			SplashAbort();
			if ( AfxMessageBox( IDS_SCHEMA_LOAD_ERROR, MB_ICONWARNING|MB_OKCANCEL ) != IDOK )
				return FALSE;
			SplashStep( L"Metadata Schemas" );
		}
		if ( ! Settings.MediaPlayer.FileTypes.size() )
		{
			CString sTypeFilter;
			static const LPCTSTR szTypes[] =
			{
				CSchema::uriAudio,
				CSchema::uriVideo,
				NULL
			};
			for ( int i = 0 ; szTypes[ i ] ; ++ i )
			{
				if ( CSchemaPtr pSchema = SchemaCache.Get( szTypes[ i ] ) )
					sTypeFilter += pSchema->m_sTypeFilter;
			}
			sTypeFilter.Replace( _T("|."), _T("|") );
			CSettings::LoadSet( &Settings.MediaPlayer.FileTypes, sTypeFilter );
		}
		if ( ! Settings.Library.SafeExecute.size() )
		{
			CString sTypeFilter;
			static const LPCTSTR szTypes[] =
			{
				CSchema::uriArchive,
				CSchema::uriAudio,
				CSchema::uriVideo,
				CSchema::uriBook,
				CSchema::uriImage,
				CSchema::uriCollection,
				CSchema::uriBitTorrent,
				NULL
			};
			for ( int i = 0 ; szTypes[ i ] ; ++ i )
			{
				if ( CSchemaPtr pSchema = SchemaCache.Get( szTypes[ i ] ) )
					sTypeFilter += pSchema->m_sTypeFilter;
			}
			sTypeFilter.Replace( _T("|."), _T("|") );
			CSettings::LoadSet( &Settings.Library.SafeExecute, sTypeFilter );
		}

	//CWaitCursor pCursor;

	SplashStep( L"Thumb Database" );
		CThumbCache::InitDatabase();	// Lengthy if large
	SplashStep( L"Library" );
		Library.Load();					// Lengthy if very large
	SplashStep( L"Download Manager" );
		Downloads.Load();				// Lengthy if many
	SplashStep( L"Upload Manager" );
		UploadQueues.Load();

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

	if ( Settings.Connection.EnableUPnP )
	{
		SplashStep( L"Plug'n'Play Network Access, Please Wait" );
		// First run will do UPnP discovery in the QuickStart Wizard
		if ( ! Settings.Live.FirstRun )
		{
			m_pUPnPFinder.Attach( new CUPnPFinder );
			if ( m_pUPnPFinder->AreServicesHealthy() )
				m_pUPnPFinder->StartDiscovery();	// Lengthy 30s
		}
	}

	//pCursor.Restore();

	SplashStep( L"GUI" );
		if ( m_cmdInfo.m_bTray )
			WriteProfileInt( _T("Windows"), _T("CMainWnd.ShowCmd"), 0 );
		new CMainWnd();
		CoolMenu.EnableHook();
		if ( m_cmdInfo.m_bTray )
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
		// From this point translations would be available, and LoadString returns correct strings
		Sleep( 60 );	// Allow some splash text visibility

	SplashStep( L"Upgrade Manager" );
		VersionChecker.Start();
		Sleep( 120 );

	SplashStep();

	m_bLive = true;

	ProcessShellCommand( m_cmdInfo );

//	afxMemDF = allocMemDF | delayFreeMemDF | checkAlwaysMemDF;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp termination

int CPeerProjectApp::ExitInstance()
{
	if ( m_bInteractive )
	{
		// Continue Shutdown Splash Screen (from WndMain)

		CWaitCursor pCursor;

		const int nSplashSteps = 5
			+ ( Settings.Connection.DeleteFirewallException ? 1 : 0 )
			+ ( m_pUPnPFinder ? 2 : 0 )
			+ ( m_bLive ? 3 : 0 );

		SplashStep( L"Disconnecting", nSplashSteps, true );
		VersionChecker.Stop();
		DiscoveryServices.Stop();
		Network.Disconnect();

		SplashStep( L"Stopping Library Tasks" );
		LibraryBuilder.CloseThread();
		Library.CloseThread();

		SplashStep( L"Stopping Transfers" );
		Transfers.StopThread();
		Downloads.CloseTransfers();

		SplashStep( L"Clearing Clients" );
		Uploads.Clear( FALSE );
		BTClients.Clear();
		DCClients.Clear();
		EDClients.Clear();

		if ( m_bLive )
		{
			SplashStep( L"Saving Services" );
			Settings.Save( TRUE );
			Security.Save();
			HostCache.Save();
			UploadQueues.Save();
			DiscoveryServices.Save();

			SplashStep( L"Saving Downloads" );
			DownloadGroups.Save();
			Downloads.Save();

			SplashStep( L"Saving Library" );
			Library.Save();
		}

		if ( m_pUPnPFinder )
		{
			SplashStep( L"Closing Plug'n'Play Network Access" );
			m_pUPnPFinder->StopAsyncFind();
			if ( Settings.Connection.DeleteUPnPPorts )
				m_pUPnPFinder->DeletePorts();
			m_pUPnPFinder.Free();
		}

		if ( Settings.Connection.DeleteFirewallException )
		{
			SplashStep( L"Closing Windows Firewall Access" );
			CFirewall firewall;
			if ( firewall.AccessWindowsFirewall() )
				firewall.SetupProgram( m_strBinaryPath, theApp.m_pszAppName, TRUE );
		}

		SplashStep( L"Finalizing" );
		Downloads.Clear( true );
		Library.Clear();
		CoolMenu.Clear();
		Skin.Clear();

		DDEServer.Close();
		IEProtocol.Close();

		SchemaCache.Clear();
		Plugins.Clear();

		FreeCountry();	// Release GeoIP

		Sleep( 80 );
		SplashStep();
	}

	WSACleanup();

	if ( m_hTheme != NULL )
		FreeLibrary( m_hTheme );

	if ( m_hShell32 != NULL )
		FreeLibrary( m_hShell32 );

	if ( m_hShlWapi != NULL )
		FreeLibrary( m_hShlWapi );

	if ( m_hLibGFL != NULL )
		FreeLibrary( m_hLibGFL );

	//delete m_pFontManager;

	UnhookWindowsHookEx( m_hHookKbd );
	UnhookWindowsHookEx( m_hHookMouse );

	if ( m_hCryptProv )
		CryptReleaseContext( m_hCryptProv, 0 );

	// Moved to destructor
	//if ( m_pMutex != NULL )
	//	CloseHandle( m_pMutex );

	{
		CQuickLock pLock( m_csMessage );
		while ( ! m_oMessages.IsEmpty() )
			delete m_oMessages.RemoveHead();
	}

	return CWinApp::ExitInstance();
}

//BOOL CPeerProjectApp::ParseCommandLine()
//{
//	CWinApp::ParseCommandLine( m_cmdInfo );
//
//	AfxSetPerUserRegistration( m_cmdInfo.m_bRegisterPerUser );	// || ! IsRunAsAdmin()
//
//	if ( m_cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister ||
//		 m_cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister )
//	{
//		m_cmdInfo.m_bRunEmbedded = TRUE;	// Suppress dialog
//
//		ProcessShellCommand( m_cmdInfo );
//
//		return FALSE;
//	}
//
//	BOOL bFirst = FALSE;
//	HWND hwndFirst = NULL;
//	for (;;)
//	{
//		m_pMutex = CreateMutex( NULL, FALSE, _T("Global\\") CLIENT_NAME );
//		if ( m_pMutex != NULL )
//		{
//			if ( GetLastError() == ERROR_ALREADY_EXISTS )
//			{
//				CloseHandle( m_pMutex );
//				m_pMutex = NULL;
//				hwndFirst = FindWindow( _T("PeerProjectMainWnd"), NULL );
//			}
//			else
//				bFirst = TRUE;	// We are first!
//		}
//		// else Likely mutex created in another user's session
//
//		if ( ! m_cmdInfo.m_bWait || bFirst )
//			break;
//
//		Sleep( 250 );	// Wait for first instance exit
//	}
//
//	if ( ! m_cmdInfo.m_sTask.IsEmpty() )
//	{
//		// Pass scheduler task to existing instance (Note Windows scheduling not implemented)
//		//CScheduler::Execute( hwndFirst, m_cmdInfo.m_sTask );
//
//		return FALSE;	// Don't start second instance
//	}
//
//	if ( ! bFirst )
//	{
//		if ( hwndFirst )
//		{
//			if ( m_cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen )
//			{
//				// Pass command line to first instance
//				m_cmdInfo.m_strFileName.Trim( _T(" \t\r\n\"") );
//				COPYDATASTRUCT cd =
//				{
//					COPYDATA_OPEN,
//					m_cmdInfo.m_strFileName.GetLength() * sizeof( TCHAR ),
//					(PVOID)(LPCTSTR)m_cmdInfo.m_strFileName
//				};
//				DWORD_PTR dwResult;
//				SendMessageTimeout( hwndFirst, WM_COPYDATA, NULL, (WPARAM)&cd, SMTO_NORMAL, 250, &dwResult );
//			}
//			else
//			{
//				// Popup first instance
//				DWORD_PTR dwResult;
//				SendMessageTimeout( hwndFirst, WM_COMMAND, ID_TRAY_OPEN, 0, SMTO_NORMAL, 250, &dwResult );
//				ShowWindow( hwndFirst, SW_SHOWNA );
//				BringWindowToTop( hwndFirst );
//				SetForegroundWindow( hwndFirst );
//			}
//		}
//		// else Likely window created in another user's session
//
//		return FALSE;	// Don't start second instance
//	}
//
//	if ( m_cmdInfo.m_bWait )
//		Sleep( 1000 );	// Wait for other instance complete exit
//
//	return TRUE;		// Continue PeerProject execution
//}

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
	else if ( m_dlgSplash /*&& ! nMax*/ )	// Reset m_dlgSplash->m_nPos ?
		m_dlgSplash->Step( pszMessage );

	TRACE( _T("Step: %s\n"), pszMessage ? pszMessage : _T("Done") );
}

void CPeerProjectApp::SplashAbort()
{
	if ( m_dlgSplash )
	{
		m_dlgSplash->Hide( TRUE );
		m_dlgSplash = NULL;
	}
}

void CPeerProjectApp::WinHelp(DWORD /*dwData*/, UINT /*nCmd*/)
{
	// Suppress F1
}

CDocument* CPeerProjectApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	if ( lpszFileName )
		Open( lpszFileName );
	return NULL;
}

BOOL CPeerProjectApp::Open(LPCTSTR lpszFileName)		// Note: No BOOL bDoIt needed
{
	CString strExt( PathFindExtension( lpszFileName ) );
	if ( strExt.IsEmpty() ) return FALSE;
	strExt = strExt.MakeLower();

	SwitchMap( Ext )
	{
		Ext[ L".torrent" ] 	= 't';
		Ext[ L".co" ]		= 'c';
		Ext[ L".collection" ] = 'c';
		Ext[ L".emulecollection" ] = 'c';
		Ext[ L".bz2" ]		= 'b';
		Ext[ L".met" ]		= 'i';
		Ext[ L".dat" ]		= 'i';
		Ext[ L".url" ]		= 'u';
		Ext[ L".lnk" ]		= 'l';
	//	Ext[ L".metalink" ]	= 'm';
	//	Ext[ L".meta4" ]	= 'm';
	//	Ext[ L".magma" ]	= 'a';
	}

	switch( Ext[ strExt ] )
	{
	case 't':	// .torrent
		return OpenTorrent( lpszFileName );
	case 'c':	// .co .collection .emulecollection
		return OpenCollection( lpszFileName );
	case 'i':	// .met .dat
		return OpenImport( lpszFileName );
//	case 'm':	// ToDo: .metalink .meta4 .magma (0.2)
//		return OpenMetalink( lpszFileName );
	case 'u':	// .url
		return OpenInternetShortcut( lpszFileName );
	case 'l':	// .lnk
		return OpenShellShortcut( lpszFileName );
	case 'b':	// .xml.bz2 (DC++)
		if ( ! _tcsicmp( lpszFileName + ( _tcslen( lpszFileName ) - 8 ),  _T(".xml.bz2") ) )
		{
			if ( ! _tcsicmp( PathFindFileName( lpszFileName ), _T("hublist.xml.bz2") ) )
				return OpenImport( lpszFileName );
			return OpenCollection( lpszFileName );
		}
		break;
	}

	// Legacy method for reference:
//	if ( nLength > 8  &&  ! _tcsicmp( lpszFileName + nLength - 8,  _T(".torrent") ) )
//		return OpenTorrent( lpszFileName );
//	if (/*nLength > 3 &&*/! _tcsicmp( lpszFileName + nLength - 3,  _T(".co") ) )
//		return OpenCollection( lpszFileName );
//	if ( nLength > 11 &&  ! _tcsicmp( lpszFileName + nLength - 11, _T(".collection") ) )
//		return OpenCollection( lpszFileName );
//	if ( nLength > 16 &&  ! _tcsicmp( lpszFileName + nLength - 16, _T(".emulecollection") ) )
//		return OpenCollection( lpszFileName );
//	if ( nLength > 8  &&  ! _tcsicmp( lpszFileName + nLength - 8,  _T(".xml.bz2") ) )
//		return OpenCollection( lpszFileName );
//	if ( nLength > 14 &&  ! _tcsicmp( lpszFileName + nLength - 15, _T("hublist.xml.bz2") ) )
//		return OpenImport( lpszFileName );
//	if (/*nLength > 4 &&*/! _tcsicmp( lpszFileName + nLength - 4,  _T(".met") ) )
//		return OpenImport( lpszFileName );
//	if (/*nLength > 4 &&*/! _tcsicmp( lpszFileName + nLength - 4,  _T(".dat") ) )
//		return OpenImport( lpszFileName );
//	if (/*nLength > 4 &&*/! _tcsicmp( lpszFileName + nLength - 4,  _T(".url") ) )
//		return OpenInternetShortcut( lpszFileName );
//	if (/*nLength > 4 &&*/! _tcsicmp( lpszFileName + nLength - 4,  _T(".lnk") ) )
//		return OpenShellShortcut( lpszFileName );

	return OpenURL( lpszFileName );
}

BOOL CPeerProjectApp::OpenImport(LPCTSTR lpszFileName)
{
	//return HostCache.Import( lpszFileName );	// Obsolete

	const size_t nLen = _tcslen( lpszFileName ) + 1;
	auto_array< TCHAR > pszPath( new TCHAR[ nLen ] );
	if ( pszPath.get() )
	{
		_tcscpy_s( pszPath.get(), nLen, lpszFileName );
		if ( PostMainWndMessage( WM_IMPORT, (WPARAM)pszPath.release() ) )
			return TRUE;
	}

	return FALSE;
}

BOOL CPeerProjectApp::OpenShellShortcut(LPCTSTR lpszFileName)
{
	CString sPath( ResolveShortcut( lpszFileName ) );
	return ! sPath.IsEmpty() && Open( sPath );
}

BOOL CPeerProjectApp::OpenInternetShortcut(LPCTSTR lpszFileName)
{
	CString sURL;
	BOOL bResult = ( GetPrivateProfileString( _T("InternetShortcut"), _T("URL"),
		_T(""), sURL.GetBuffer( MAX_PATH ), MAX_PATH, lpszFileName ) > 3 );
	sURL.ReleaseBuffer();
	return bResult && ! sURL.IsEmpty() && OpenURL( sURL );
}

BOOL CPeerProjectApp::OpenTorrent(LPCTSTR lpszFileName)
{
	// Test torrent
	//auto_ptr< CBTInfo > pTorrent( new CBTInfo() );
	//if ( ! pTorrent.get() ) return FALSE;
	//if ( ! pTorrent->LoadTorrentFile( lpszFileName ) ) return FALSE;

	// Open torrent
	const size_t nLen = _tcslen( lpszFileName ) + 1;
	auto_array< TCHAR > pszPath( new TCHAR[ nLen ] );
	if ( pszPath.get() )
	{
		_tcscpy_s( pszPath.get(), nLen, lpszFileName );
		if ( PostMainWndMessage( WM_TORRENT, (WPARAM)pszPath.release() ) )
			return TRUE;
	}

	return FALSE;
}

BOOL CPeerProjectApp::OpenCollection(LPCTSTR lpszFileName)
{
	const size_t nLen = _tcslen( lpszFileName ) + 1;
	auto_array< TCHAR > pszPath( new TCHAR[ nLen ] );
	if ( pszPath.get() )
	{
		_tcscpy_s( pszPath.get(), nLen, lpszFileName );
		if ( PostMainWndMessage( WM_COLLECTION, (WPARAM)pszPath.release() ) )
			return TRUE;
	}

	return FALSE;
}

BOOL CPeerProjectApp::OpenURL(LPCTSTR lpszFileName, BOOL bSilent)
{
	if ( ! bSilent )
		theApp.Message( MSG_NOTICE, IDS_URL_RECEIVED, lpszFileName );

	auto_ptr< CPeerProjectURL > pURL( new CPeerProjectURL() );
	if ( pURL.get() && pURL->Parse( lpszFileName ) )
	{
		PostMainWndMessage( WM_URL, (WPARAM)pURL.release() );
		return TRUE;
	}

	if ( ! bSilent )
		theApp.Message( MSG_NOTICE, IDS_URL_PARSE_ERROR );

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp version

void CPeerProjectApp::GetVersionNumber()
{
	// Set Build Date
	COleDateTime tCompileTime;
	tCompileTime.ParseDateTime( _T(__DATE__), LOCALE_NOUSEROVERRIDE, 1033 );
	m_sBuildDate = tCompileTime.Format( _T("%Y%m%d") );

	// Get .exe-file name
	GetModuleFileName( NULL, m_strBinaryPath.GetBuffer( MAX_PATH ), MAX_PATH );
	m_strBinaryPath.ReleaseBuffer( MAX_PATH );

	// Load version from .exe-file properties
	m_nVersion[0] = m_nVersion[1] = m_nVersion[2] = m_nVersion[3] = 0;
	DWORD dwSize = GetFileVersionInfoSize( m_strBinaryPath, &dwSize );
	if ( dwSize )
	{
		if ( BYTE* pBuffer = new BYTE[ dwSize ] )
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

	m_sSmartAgent = CLIENT_NAME _T(" ");
	m_sSmartAgent += m_sVersion;

	m_pBTVersion[ 0 ] = BT_ID1;
	m_pBTVersion[ 1 ] = BT_ID2;
	m_pBTVersion[ 2 ] = (BYTE)m_nVersion[ 0 ];
	m_pBTVersion[ 3 ] = (BYTE)m_nVersion[ 1 ];

	// PeerProject 1.X.X.X  32/64-bit  (date rXXXX)  Debug

	m_sVersionLong = m_sSmartAgent +
#ifdef _WIN64
	_T("  64-bit  ") +
#else
	_T("  32-bit  ") +
#endif
	_T("(") + m_sBuildDate +
#ifdef __REVISION__
	_T(" r") _T(__REVISION__) _T(")") +
#else
	_T(")") +
#endif
#ifdef __MODAUTHOR__
	_T("  ") _T(__MODAUTHOR__);	// YOUR NAME (Edit in Revision.h)
#elif defined(LAN_MODE)
	_T("  LAN Mod");
#elif defined(_DEBUG)
	_T("  Debug");
#else
	_T("");
#endif

#ifdef _DEBUG	// BugTrap
	BT_SetAppName( CLIENT_NAME );
	BT_SetAppVersion( m_sVersionLong );
#endif


	// Determine the version of Windows
	OSVERSIONINFOEX pVersion = { sizeof( OSVERSIONINFOEX ) };
	GetVersionEx( (OSVERSIONINFO*)&pVersion );
	GetSystemInfo( &m_SysInfo );

	// Determine if it's a server
	m_bIsServer = pVersion.wProductType != VER_NT_WORKSTATION;	// VER_NT_SERVER

	// Many supported windows versions have network limiting
	m_bLimitedConnections = ! m_bIsServer;

	// Get Major+Minor version (6.1)
	//	Major ver 5:	Win2000 = 0, WinXP = 1, WinXP64 = 2, Server2003 = 2
	//	Major ver 6:	Vista = 0, Server2008 = 0, Windows7 = 1
	m_nWindowsVersion = pVersion.dwMajorVersion;
	m_nWindowsVersionMinor = pVersion.dwMinorVersion;

	// Get Service Pack version
	TCHAR* sp = _tcsstr( pVersion.szCSDVersion, _T("Service Pack") );

	// Set some variables for different Windows OS
	if ( m_nWindowsVersion == 5 )
	{
#ifndef WIN64
		// Windows 2000
		if ( m_nWindowsVersionMinor == 0 )
		{
			m_bIsWin2000 = true;
			m_bLimitedConnections = false;
		}
		// Windows XP
		else if ( m_nWindowsVersionMinor == 1 )
		{
			// No network limiting for original XP or SP1
			if ( ! sp || sp[ 13 ] == '1' )
				m_bLimitedConnections = false;
		}
		// Windows XP64 or 2003
		else if ( m_nWindowsVersionMinor == 2 )
#endif
		{
			// No network limiting for Vanilla Win2003/XP64
			if ( ! sp )
				m_bLimitedConnections = false;
		}
	}
	else if ( m_nWindowsVersion >= 6 )
	{
		// GUI support
		m_bIsVistaOrNewer = true;

		if ( pVersion.wProductType == VER_NT_SERVER )
		{
			m_bLimitedConnections = false;
			return;
		}

		// Windows 7 or Vista SP2+ have Registry patch
		if ( m_nWindowsVersionMinor >= 1 || ( sp && sp[ 13 ] >= '2' ) ) 	// Vista (no SP) crashfix
		{
			m_bLimitedConnections = false;

			HKEY hKey;
			if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters",
				0, KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS )
			{
				DWORD nSize = sizeof( DWORD ), nResult = 0, nType = REG_NONE;
				if ( ( RegQueryValueEx( hKey, L"EnableConnectionRateLimiting",
					NULL, &nType, (LPBYTE)&nResult, &nSize ) == ERROR_SUCCESS ) &&
					nType == REG_DWORD && nResult == 1 )
				{
					// ToDo: Request user to modify registry value?
					m_bLimitedConnections = true;
				}

				RegCloseKey( hKey );
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp resources

void CPeerProjectApp::InitResources()
{
	// Get .exe-file name
	GetModuleFileName( NULL, m_strBinaryPath.GetBuffer( MAX_PATH ), MAX_PATH );
	m_strBinaryPath.ReleaseBuffer( MAX_PATH );


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
	//	(FARPROC&)m_pfnEnableThemeDialogTexture = GetProcAddress( m_hTheme, "EnableThemeDialogTexture" );
	//	(FARPROC&)m_pfnDrawThemeParentBackground = GetProcAddress( m_hTheme, "DrawThemeParentBackground" );
	//	(FARPROC&)m_pfnGetThemeBackgroundContentRect = GetProcAddress( m_hTheme, "GetThemeBackgroundContentRect" );
	//	(FARPROC&)m_pfnDrawThemeText = GetProcAddress( m_hTheme, "DrawThemeText" );
		(FARPROC&)m_pfnGetThemeSysFont = GetProcAddress( m_hTheme, "GetThemeSysFont" );
	}

	// Get pointers to some functions that require Internet Explorer 6 or greater
	if ( ( m_hShlWapi = LoadLibrary( _T("shlwapi.dll") ) ) != NULL )
	{
		(FARPROC&)m_pfnAssocIsDangerous = GetProcAddress( m_hShlWapi, "AssocIsDangerous" );
	}

	if ( ( m_hShell32 = LoadLibrary( _T("shell32.dll") ) ) != NULL )
	{
		(FARPROC&)m_pfnSHGetFolderPathW = GetProcAddress( m_hShell32, "SHGetFolderPathW" );
		(FARPROC&)m_pfnSHGetKnownFolderPath = GetProcAddress( m_hShell32, "SHGetKnownFolderPath" );
	}

	LoadCountry();	// GeoIP

	// Load LibGFL in a custom way, so PeerProject plugins can use this library too when not in their search path (From Plugins folder, and when running inside Visual Studio)
	m_hLibGFL = CustomLoadLibrary( _T("LibGFL290.dll") );

	// Use GlobalMemoryStatusEx if possible (WinXP)
	MEMORYSTATUSEX pMemory = {};
	pMemory.dwLength = sizeof(pMemory);
	if ( GlobalMemoryStatusEx( &pMemory ) )
		m_nPhysicalMemory = pMemory.ullTotalPhys;

	//
	// Setup default fonts:
	//

	// theApp.m_nFontQuality default ClearType
	UINT nSmoothingType = 0;
	BOOL bFontSmoothing = FALSE;
	if ( SystemParametersInfo( SPI_GETFONTSMOOTHING, 0, &bFontSmoothing, 0 ) && bFontSmoothing &&
		 SystemParametersInfo( SPI_GETFONTSMOOTHINGTYPE, 0, &nSmoothingType, 0 ) )
	{
		m_nFontQuality = ( nSmoothingType == FE_FONTSMOOTHINGSTANDARD ) ?
			ANTIALIASED_QUALITY : ( ( nSmoothingType == FE_FONTSMOOTHINGCLEARTYPE ) ?
			CLEARTYPE_QUALITY : DEFAULT_QUALITY );
	}

	if ( Settings.Fonts.DefaultFont.IsEmpty() )
	{
		// Get font from current theme
		if ( m_pfnGetThemeSysFont )
		{
			LOGFONT pFont = {};
			if ( m_pfnGetThemeSysFont( NULL, TMT_MENUFONT, &pFont ) == S_OK )
				Settings.Fonts.DefaultFont = pFont.lfFaceName;
		}
	}

	if ( Settings.Fonts.DefaultFont.IsEmpty() )
	{
		// Get font by legacy method
		NONCLIENTMETRICS pMetrics = { sizeof( NONCLIENTMETRICS ) };
		SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ), &pMetrics, 0 );
		Settings.Fonts.DefaultFont = pMetrics.lfMenuFont.lfFaceName;
	}

	if ( Settings.Fonts.SystemLogFont.IsEmpty() )
		Settings.Fonts.SystemLogFont = Settings.Fonts.DefaultFont;

	if ( Settings.Fonts.PacketDumpFont.IsEmpty() )
		Settings.Fonts.PacketDumpFont = _T("Lucida Console");

	m_gdiFont.CreateFont( -(int)Settings.Fonts.FontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, Settings.Fonts.DefaultFont );

	m_gdiFontBold.CreateFont( -(int)Settings.Fonts.FontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, Settings.Fonts.DefaultFont );

	m_gdiFontLine.CreateFont( -(int)Settings.Fonts.FontSize, 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, Settings.Fonts.DefaultFont );

	CryptAcquireContext( &m_hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT );

	srand( GetTickCount() );

	m_hHookKbd   = SetWindowsHookEx( WH_KEYBOARD, (HOOKPROC)KbdHook, NULL, AfxGetThread()->m_nThreadID );
	m_hHookMouse = SetWindowsHookEx( WH_MOUSE, (HOOKPROC)MouseHook, NULL, AfxGetThread()->m_nThreadID );
	m_nLastInput = (DWORD)time( NULL );

	if ( SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &m_nMouseWheel, 0 ) )
	{
		if ( m_nMouseWheel > 20 )			// Catch WHEEL_PAGESCROLL (UINT_MAX)
			m_nMouseWheel = 20;				// ToDo: Better handling rare mouse wheel set to scroll by page?
		else if ( m_nMouseWheel < 1 )
			m_nMouseWheel = 3;				// Default lines set at initialization
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp custom library loader

HINSTANCE CPeerProjectApp::CustomLoadLibrary(LPCTSTR pszFileName)
{
	HINSTANCE hLibrary = NULL;

	if ( ( hLibrary = LoadLibrary( pszFileName ) ) != NULL || ( hLibrary = LoadLibrary( Settings.General.Path + _T("\\") + pszFileName ) ) != NULL )
		; // Success
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
	return ( static_cast< DWORD >( nType & MSG_SEVERITY_MASK ) > Settings.General.LogLevel ) ||		// Severity filter
		( ( nType & MSG_FACILITY_MASK ) == MSG_FACILITY_SEARCH && ! Settings.General.SearchLog );	// Facility filter
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

		strLine.Trim();
		if ( strLine.IsEmpty() ) continue;

		if ( strLine == _T(".") ) strLine.Empty();

		if ( _tcsnicmp( strLine, _T("!"), 1 ) == 0 )
			PrintMessage( MSG_NOTICE, (LPCTSTR)strLine + 1 );
		else
			PrintMessage( MSG_INFO, strLine );
	}

	CString strCPU;
	strCPU.Format( _T("\n%u x CPU. Features:"), m_SysInfo.dwNumberOfProcessors );
	if ( Machine::SupportsMMX() )
		strCPU += _T(" MMX");
	if ( Machine::SupportsSSE() )
		strCPU += _T(" SSE");
	if ( Machine::SupportsSSE2() )
		strCPU += _T(" SSE2");
	if ( Machine::SupportsSSE3() )
		strCPU += _T(" SSE3");
	if ( Machine::SupportsSSSE3() )
		strCPU += _T(" SSSE3");
	if ( Machine::SupportsSSE41() )
		strCPU += _T(" SSE4.1");
	if ( Machine::SupportsSSE42() )
		strCPU += _T(" SSE4.2");
	if ( Machine::SupportsSSE4A() )
		strCPU += _T(" SSE4A");
	if ( Machine::SupportsSSE5() )
		strCPU += _T(" SSE5");
	if ( Machine::Supports3DNOW() )
		strCPU += _T(" 3DNow");
	if ( Machine::Supports3DNOWEXT() )
		strCPU += _T(" 3DNowExt");
	PrintMessage( MSG_INFO, strCPU );
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
	if ( ! strTemp.IsEmpty() )
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
	if ( strTemp.GetLength() > 1 )
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
			MoveFileEx( CString( _T("\\\\?\\") ) + Settings.General.UserPath + _T("\\Data\\PeerProject.log"),
				CString( _T("\\\\?\\") ) + Settings.General.UserPath + _T("\\Data\\PeerProject.old.log"),
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			// Start a new log
			if ( ! pFile.Open( Settings.General.UserPath + _T("\\Data\\PeerProject.log"),
				CFile::modeWrite|CFile::modeCreate ) ) return;
			// Unicode marker
			WORD nByteOrder = 0xFEFF;
			pFile.Write( &nByteOrder, 2 );
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
	for ( int i = 0 ; szModules[ i ] ; i++ )
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

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp GeoIP Countries

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

void CPeerProjectApp::LoadCountry()
{
	if ( ( m_hGeoIP = CustomLoadLibrary( _T("GeoIP.dll") ) ) != NULL )
	{
		GeoIP_newFunc pfnGeoIP_new = (GeoIP_newFunc)GetProcAddress( m_hGeoIP, "GeoIP_new" );
		m_pfnGeoIP_delete = (GeoIP_deleteFunc)GetProcAddress( m_hGeoIP, "GeoIP_delete" );
		m_pfnGeoIP_country_code_by_ipnum = (GeoIP_country_code_by_ipnumFunc)GetProcAddress( m_hGeoIP, "GeoIP_country_code_by_ipnum" );
		m_pfnGeoIP_country_name_by_ipnum = (GeoIP_country_name_by_ipnumFunc)GetProcAddress( m_hGeoIP, "GeoIP_country_name_by_ipnum" );
		if ( pfnGeoIP_new )
			m_pGeoIP = pfnGeoIP_new( GEOIP_MEMORY_CACHE );
	}
}

void CPeerProjectApp::FreeCountry()
{
	if ( m_hGeoIP != NULL )
	{
		if ( m_pGeoIP && m_pfnGeoIP_delete )
		{
			__try
			{
				m_pfnGeoIP_delete( m_pGeoIP );
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
			}
			m_pGeoIP = NULL;
		}
		FreeLibrary( m_hGeoIP );
		m_hGeoIP = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPeerProjectApp process an internal URI

BOOL CPeerProjectApp::InternalURI(LPCTSTR pszURI)
{
	CMainWnd* pMainWnd = SafeMainWnd();
	if ( pMainWnd == NULL ) return FALSE;

	CString strURI( pszURI );
//	const int nBreak = strURI.FindOneOf( _T(":") ) + 1;
	strURI = strURI.Left( 20 ).MakeLower();					// Most chars needed to determine protocol or command

	if ( _tcsnicmp( strURI, _T("command:"), 8 ) != 0 )		// Assume external URL if not internal command
	{
		if ( ! _tcsnicmp( strURI, _T("magnet:"), 7 ) ||
			! _tcsnicmp( strURI, _T("http://"), 7 ) ||
			! _tcsnicmp( strURI, _T("https://"), 8 ) ||
			! _tcsnicmp( strURI, _T("ftp://"), 6 ) ||
			! _tcsnicmp( strURI, _T("gnutella:"), 9 ) ||
			! _tcsnicmp( strURI, _T("gnutella1:"), 10 ) ||
			! _tcsnicmp( strURI, _T("gnutella2:"), 10 ) ||
			! _tcsnicmp( strURI, _T("peerproject:"), 12 ) ||
			! _tcsnicmp( strURI, _T("shareaza:"), 9 ) ||
			! _tcsnicmp( strURI, _T("ed2k:"), 5 ) ||
			! _tcsnicmp( strURI, _T("g2:"), 3 ) ||
			! _tcsnicmp( strURI, _T("gwc:"), 4 ) ||
			! _tcsnicmp( strURI, _T("uhc:"), 4 ) ||
			! _tcsnicmp( strURI, _T("ukhl:"), 5 ) ||
			! _tcsnicmp( strURI, _T("gnet:"), 5 ) ||
			! _tcsnicmp( strURI, _T("peer:"), 5 ) ||
			! _tcsnicmp( strURI, _T("p2p:"), 4 ) ||
			! _tcsnicmp( strURI, _T("mp2p:"), 5 ) ||
			! _tcsnicmp( strURI, _T("foxy:"), 5 ) ||
			! _tcsnicmp( strURI, _T("btc:"), 4 ) ||
			! _tcsnicmp( strURI, _T("irc:"), 4 ) ||
			! _tcsnicmp( strURI, _T("aim:"), 4 ) ||
			! _tcsnicmp( strURI, _T("dchub:"), 6 ) ||
			! _tcsnicmp( strURI, _T("dcfile:"), 7 ) ||
			! _tcsnicmp( strURI, _T("mailto:"), 7 ) ||
			! _tcsnicmp( strURI, _T("sig2dat:"), 8 ) )
		{
			ShellExecute( pMainWnd->GetSafeHwnd(), _T("open"),
				pszURI, NULL, NULL, SW_SHOWNORMAL );

			return TRUE;
		}

		theApp.Message( MSG_ERROR, _T("Unknown link URI:  ") + strURI );
		return FALSE;
	}

	// Specific "command:" prefixed internal utilities:

	if ( ! _tcsnicmp( strURI, _T("command:id_"), 11 ) )				// Common "command:ID_"
	{
		if ( UINT nCmdID = CoolInterface.NameToID( pszURI + 8 ) )
		{
			pMainWnd->PostMessage( WM_COMMAND, nCmdID );
			return TRUE;
		}
	}
	else if ( ! _tcsnicmp( strURI, _T("command:shell:"), 14 ) ) 	// Assume "command:shell:downloads"
	{
		ShellExecute( pMainWnd->GetSafeHwnd(), _T("open"),
			Settings.Downloads.CompletePath, NULL, NULL, SW_SHOWNORMAL );
		if ( strURI.Find( _T(":downloads"), 12 ) > 1 )
			return TRUE;
	}
	else if ( ! _tcsnicmp( strURI, _T("command:update"), 14 ) ) 	// Version notice "command:update"
	{
		pMainWnd->PostMessage( WM_VERSIONCHECK, VC_CONFIRM );
		return TRUE;
	}
	else if ( ! _tcsnicmp( strURI, _T("command:copy:"), 13 ) )		// Clipboard "command:copy:<text>"
	{
		if ( ! AfxGetMainWnd()->OpenClipboard() ) return FALSE;

		EmptyClipboard();

		strURI = CString( pszURI ).Mid( 13 );

		CT2CW pszWide( (LPCTSTR)strURI );
		if ( HANDLE hMem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, ( wcslen(pszWide) + 1 ) * sizeof(WCHAR) ) )
		{
			if ( LPVOID pMem = GlobalLock( hMem ) )
			{
				CopyMemory( pMem, pszWide, ( wcslen(pszWide) + 1 ) * sizeof(WCHAR) );
				GlobalUnlock( hMem );
				SetClipboardData( CF_UNICODETEXT, hMem );

				theApp.Message( MSG_TRAY, LoadString( IDS_COPIED_TO_CLIPBOARD ) + _T("\n") + strURI );
			}
		}

		CloseClipboard();
		return TRUE;
	}
	//else if ( ! _tcsnicmp( strURI, _T("command:launch:"), 15 ) )	// Unused but useful? "command:launch:"
	//{
	//	DWORD nIndex = 0;
	//	_stscanf( (LPCTSTR)strURI + 12, _T("%lu"), &nIndex );
	//
	//	CSingleLock oLock( &Library.m_pSection, TRUE );
	//	if ( CLibraryFile* pFile = Library.LookupFile( nIndex ) )
	//	{
	//		if ( pFile->IsAvailable() )
	//		{
	//			CString strPath = pFile->GetPath();
	//			oLock.Unlock();
	//			CFileExecutor::Execute( strPath, FALSE );
	//			return TRUE;
	//		}
	//	}
	//	oLock.Unlock();
	//}
	//else if ( ! _tcsnicmp( strURI, _T("command:windowptr:"), 18 ) ) // Unused but useful? "command:windowptr:"
	//{
	//	CChildWnd* pChild = NULL;
	//	_stscanf( (LPCTSTR)strURI + 15, _T("%lu"), &pChild );
	//	if ( pMainWnd->m_pWindows.Check( pChild ) )
	//	{
	//		pChild->MDIActivate();
	//		return TRUE;
	//	}
	//}

	//else if ( ! _tcsnicmp( strURI, _T("page:"), 5 )				// "page:CSettingsPage" defined locally in PageSettingsRich

	theApp.Message( MSG_ERROR, _T("Unknown internal command:  ") + CString( pszURI ) );
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Runtime class lookup

//void AFXAPI AfxLockGlobals(int nLockType);
//void AFXAPI AfxUnlockGlobals(int nLockType);

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
// String functions  (See Strings.cpp)

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
		return Skin.LoadString( str, IDS_STATUS_SOURCES );

	if ( num == 0 )
		return Skin.LoadString( str, IDS_STATUS_NOSOURCES );

	if ( num == 1 )
		return Skin.LoadString( str, IDS_STATUS_SOURCE );

	if ( ( num % 100 ) > 10 && ( num % 100 ) < 20 )	// 11-19 exempt
		return Skin.LoadString( str, IDS_STATUS_SOURCES );

	switch ( num % 10 )
	{
	case 0:
		return Skin.LoadString( str, IDS_STATUS_SOURCES );
	case 1:
		return Skin.LoadString( str, IDS_STATUS_SOURCES_ONES );
	case 2:
	case 3:
	case 4:
		return Skin.LoadString( str, IDS_STATUS_SOURCES_FEW );
	default:
		return Skin.LoadString( str, IDS_STATUS_SOURCES );
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

	time_t tGMT = mktime( &pTime ), tSub;
	tm pGM = {};
	if ( tGMT == -1 ||
		gmtime_s( &pGM, &tGMT ) != 0 ||
		( tSub = mktime( &pGM ) ) == -1 )
	{
	//	theApp.Message( MSG_ERROR, _T("Invalid Date/Time"), pszTime );
		return 0;
	}

	return DWORD( 2 * tGMT - tSub );
}

CString TimeToString(time_t tVal)
{
	tm time = {};
	CString str;
	if ( gmtime_s( &time, &tVal ) == 0 )
	{
		str.Format( _T("%.4i-%.2i-%.2iT%.2i:%.2iZ"),
			time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
			time.tm_hour, time.tm_min );
	}
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
	for ( int nEntry = 0 ; nEntry < nNumEntries ; nEntry++ )
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

BOOL LoadIcon(LPCTSTR szFilename, HICON* phSmallIcon, HICON* phLargeIcon, HICON* phHugeIcon, int nIcon)
{
	CString strIcon( szFilename );

	if ( phSmallIcon ) *phSmallIcon = NULL;
	if ( phLargeIcon ) *phLargeIcon = NULL;
	if ( phHugeIcon )  *phHugeIcon = NULL;

	int nIndex = strIcon.ReverseFind( _T(',') );
	if ( nIndex != -1 )
	{
		if ( _stscanf( strIcon.Mid( nIndex + 1 ), _T("%i"), &nIcon ) == 1 )
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
		ExtractIconEx( strIcon, nIcon, phLargeIcon, phSmallIcon, 1 );

	if ( phHugeIcon )
	{
		UINT nLoadedID;
		PrivateExtractIcons( strIcon, nIcon, 48, 48,
			phHugeIcon, &nLoadedID, 1, 0 );
	}

	return ( phLargeIcon && *phLargeIcon ) || ( phSmallIcon && *phSmallIcon ) ||
		( phHugeIcon && *phHugeIcon );
}

//HICON LoadCLSIDIcon(LPCTSTR szCLSID)
//{
//	HKEY hKey;
//	CString strPath;
//	strPath.Format( _T("CLSID\\%s\\InProcServer32"), szCLSID );
//	if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, strPath, 0, KEY_READ, &hKey ) != ERROR_SUCCESS )
//	{
//		strPath.Format( _T("CLSID\\%s\\LocalServer32"), szCLSID );
//		if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, strPath, 0, KEY_READ, &hKey ) != ERROR_SUCCESS )
//			return NULL;
//	}
//
//	DWORD dwType = REG_SZ, dwSize = MAX_PATH * sizeof( TCHAR );
//	LONG lResult = RegQueryValueEx( hKey, _T(""), NULL, &dwType,
//		(LPBYTE)strPath.GetBuffer( MAX_PATH ), &dwSize );
//	strPath.ReleaseBuffer( dwSize / sizeof(TCHAR) );
//	RegCloseKey( hKey );
//
//	if ( lResult != ERROR_SUCCESS )
//		return NULL;
//
//	strPath.Trim( _T(" \"") );
//
//	HICON hSmallIcon;
//	if ( ! LoadIcon( strPath, &hSmallIcon, NULL, NULL ) )
//		return NULL;
//
//	return hSmallIcon;
//}

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
		if ( hdcMask )
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
		if ( hdcBitmap && hdcMask && hIconOrig )
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
	if ( ! hbmOrig ) return NULL;
	if ( ! GetObject( hbmOrig, sizeof(BITMAP), &bm ) ) return NULL;

	hdc = GetDC( NULL );
	if ( hdc )
	{
		hdcMem1 = CreateCompatibleDC( hdc );
		if ( ! hdcMem1 )
		{
			ReleaseDC( NULL, hdc );
			return NULL;
		}
		hdcMem2 = CreateCompatibleDC( hdc );
		if ( ! hdcMem2 )
		{
			DeleteDC( hdcMem1 );
			ReleaseDC( NULL, hdc );
			return NULL;
		}
		hbm = CreateCompatibleBitmap( hdc, bm.bmWidth, bm.bmHeight );
		if (! hbm)
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
			else if ( wParam == VK_UP )
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


/////////////////////////////////////////////////////////////////////////////
// Folder Path Methods for Windows Vista/7 or XP/2000/2003

CString CPeerProjectApp::GetWindowsFolder() const
{
	HRESULT hr;
	CString sWindows;

	// Vista+
	if ( m_pfnSHGetKnownFolderPath )
	{
		PWSTR pPath = NULL;
		hr = m_pfnSHGetKnownFolderPath( FOLDERID_Windows,
			KF_FLAG_CREATE | KF_FLAG_INIT, NULL, &pPath );
		if ( pPath )
		{
			sWindows = pPath;
			CoTaskMemFree( pPath );
		}

		if ( SUCCEEDED( hr ) && ! sWindows.IsEmpty() )
			return sWindows;
	}

	// XP
	if ( m_pfnSHGetFolderPathW )
	{
		hr = m_pfnSHGetFolderPathW( NULL, CSIDL_WINDOWS, NULL, NULL,
			sWindows.GetBuffer( MAX_PATH ) );
		sWindows.ReleaseBuffer();
		if ( SUCCEEDED( hr  ) && ! sWindows.IsEmpty() )
			return sWindows;
	}

	// Legacy
	GetWindowsDirectory( sWindows.GetBuffer( MAX_PATH ), MAX_PATH );
	sWindows.ReleaseBuffer();
	return sWindows;
}

CString CPeerProjectApp::GetProgramFilesFolder() const
{
	HRESULT hr;
	CString sProgramFiles;

	// Vista+
	if ( m_pfnSHGetKnownFolderPath )
	{
		PWSTR pPath = NULL;
		hr = m_pfnSHGetKnownFolderPath( FOLDERID_ProgramFiles,
			KF_FLAG_CREATE | KF_FLAG_INIT, NULL, &pPath );
		if ( pPath )
		{
			sProgramFiles = pPath;
			CoTaskMemFree( pPath );
		}

		if ( SUCCEEDED( hr ) && ! sProgramFiles.IsEmpty() )
			return sProgramFiles;
	}

	// XP
	if ( m_pfnSHGetFolderPathW )
	{
		hr = m_pfnSHGetFolderPathW( NULL, CSIDL_PROGRAM_FILES, NULL, NULL,
			sProgramFiles.GetBuffer( MAX_PATH ) );
		sProgramFiles.ReleaseBuffer();
		if ( SUCCEEDED( hr  ) && ! sProgramFiles.IsEmpty() )
			return sProgramFiles;
	}

	// Legacy
	sProgramFiles = GetWindowsFolder().Left( 1 ) + _T(":\\Program Files");

	return sProgramFiles;
}

CString CPeerProjectApp::GetDocumentsFolder() const
{
	HRESULT hr;
	CString sDocuments;

	// Vista+
	if ( m_pfnSHGetKnownFolderPath )
	{
		PWSTR pPath = NULL;
		hr = m_pfnSHGetKnownFolderPath( FOLDERID_Documents,
			KF_FLAG_CREATE | KF_FLAG_INIT, NULL, &pPath );
		if ( pPath )
		{
			sDocuments = pPath;
			CoTaskMemFree( pPath );
		}

		if ( SUCCEEDED( hr ) && ! sDocuments.IsEmpty() )
			return sDocuments;
	}

	// XP
	if ( m_pfnSHGetFolderPathW )
	{
		hr = m_pfnSHGetFolderPathW( NULL, CSIDL_PERSONAL, NULL, NULL,
			sDocuments.GetBuffer( MAX_PATH ) );
		sDocuments.ReleaseBuffer();
		if ( SUCCEEDED( hr ) && ! sDocuments.IsEmpty() )
			return sDocuments;
	}

	// Legacy
	sDocuments = CRegistry::GetString( _T("Shell Folders"), _T("Personal"),
		_T(""), _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer") );

	return sDocuments;
}

CString CPeerProjectApp::GetDownloadsFolder() const
{
	HRESULT hr;
	CString strDownloads;

	// Vista+
	if ( m_pfnSHGetKnownFolderPath )
	{
		PWSTR pPath = NULL;
		hr = m_pfnSHGetKnownFolderPath( FOLDERID_Downloads,
			KF_FLAG_CREATE | KF_FLAG_INIT, NULL, &pPath );
		if ( pPath )
		{
			strDownloads = pPath;
			CoTaskMemFree( pPath );
		}

		if ( SUCCEEDED( hr ) && ! strDownloads.IsEmpty() )
			return strDownloads + _T("\\") + CLIENT_NAME;
	}

	// XP/Legacy
	strDownloads = GetDocumentsFolder() + _T("\\PeerProject Downloads");

	return strDownloads;
}

CString CPeerProjectApp::GetAppDataFolder() const
{
	HRESULT hr;
	CString sAppData;

	// Vista+
	if ( m_pfnSHGetKnownFolderPath )
	{
		PWSTR pPath = NULL;
		hr = m_pfnSHGetKnownFolderPath( FOLDERID_RoamingAppData,
			KF_FLAG_CREATE | KF_FLAG_INIT, NULL, &pPath );
		if ( pPath )
		{
			sAppData = pPath;
			CoTaskMemFree( pPath );
		}

		if ( SUCCEEDED( hr ) && ! sAppData.IsEmpty() )
			return sAppData;
	}

	// XP
	if ( m_pfnSHGetFolderPathW )
	{
		hr = m_pfnSHGetFolderPathW( NULL, CSIDL_APPDATA, NULL, NULL,
			sAppData.GetBuffer( MAX_PATH ) );
		sAppData.ReleaseBuffer();
		if ( SUCCEEDED( hr ) && ! sAppData.IsEmpty() )
			return sAppData;
	}

	// Legacy
	sAppData = CRegistry::GetString( _T("Shell Folders"), _T("AppData"),
		_T(""), _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer") );

	return sAppData;
}

CString CPeerProjectApp::GetLocalAppDataFolder() const
{
	HRESULT hr;
	CString sLocalAppData;

	// Vista+
	if ( m_pfnSHGetKnownFolderPath )
	{
		PWSTR pPath = NULL;
		hr = m_pfnSHGetKnownFolderPath( FOLDERID_LocalAppData,
			KF_FLAG_CREATE | KF_FLAG_INIT, NULL, &pPath );
		if ( pPath )
		{
			sLocalAppData = pPath;
			CoTaskMemFree( pPath );
		}
		if ( SUCCEEDED( hr ) && ! sLocalAppData.IsEmpty() )
			return sLocalAppData;
	}

	// XP
	if ( m_pfnSHGetFolderPathW )
	{
		hr = m_pfnSHGetFolderPathW( NULL, CSIDL_LOCAL_APPDATA, NULL, NULL,
			sLocalAppData.GetBuffer( MAX_PATH ) );
		sLocalAppData.ReleaseBuffer();
		if ( SUCCEEDED( hr ) && ! sLocalAppData.IsEmpty() )
			return sLocalAppData;
	}

	// Legacy
	sLocalAppData = CRegistry::GetString( _T("Shell Folders"), _T("Local AppData"),
		_T(""), _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer") );
	if ( ! sLocalAppData.IsEmpty() )
		return sLocalAppData;

	// Failsafe
	return GetAppDataFolder();
}

void CPeerProjectApp::OnRename(LPCTSTR pszSource, LPCTSTR pszTarget)
{
	LibraryBuilder.Remove( pszSource );

	Uploads.OnRename( pszSource, pszTarget );

	Downloads.OnRename( pszSource, pszTarget );

	// Notify built-in Mediaplayer
	if ( pszTarget == NULL )
	{
		CQuickLock otheAppLock( theApp.m_pSection );

		if ( CMainWnd* pMainWnd = theApp.SafeMainWnd() )
		{
			if ( CMediaWnd* pMediaWnd = (CMediaWnd*)pMainWnd->m_pWindows.Find( RUNTIME_CLASS( CMediaWnd ) ) )
				pMediaWnd->OnFileDelete( pszSource );
		}
	}
}

CDatabase* CPeerProjectApp::GetDatabase(bool bGeneral) const
{
	return new CDatabase( Settings.General.UserPath +
		( bGeneral ? _T("\\Data\\PeerProject.db3") : _T("\\Data\\Thumbnails.db3") ) );
}

CString SafeFilename(CString strName, bool bPath)
{
	// Restore spaces
	strName.Replace( _T("%20"), _T(" ") );

	// Replace incompatible symbols
	for ( ;; )
	{
		const int nChar = strName.FindOneOf(
			bPath ? _T("/:*?\"<>|") : _T("\\/:*?\"<>|") );

		if ( nChar == -1 )
			break;

		strName.SetAt( nChar, _T('_') );
	}

	LPCTSTR szExt = PathFindExtension( strName );
	int nExtLen = lstrlen( szExt );

	// Limit maximum filepath length
	int nMaxFilenameLength = MAX_PATH - 1 - max( max(
		Settings.Downloads.IncompletePath.GetLength(),
		Settings.Downloads.CompletePath.GetLength() ),
		Settings.Downloads.TorrentPath.GetLength() );
	if ( strName.GetLength() > nMaxFilenameLength )
		strName = strName.Left( nMaxFilenameLength - nExtLen ) + strName.Right( nExtLen );

	return strName;
}

BOOL CreateDirectory(LPCTSTR szPath)
{
	CString strDir( szPath );
	BOOL bMax = strDir.GetLength() > MAX_PATH;
	if ( strDir.GetLength() == 2 )	//&& strDir.GetAt( 1 ) == ':'
		strDir.AppendChar( '\\' );	// Root Drive

	DWORD dwAttr = GetFileAttributes( bMax ? _T("\\\\?\\") + strDir : strDir );

	if ( dwAttr != INVALID_FILE_ATTRIBUTES && ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) )
		return TRUE;

	for ( int nStart = 3 ; ; )
	{
		const int nSlash = strDir.Find( _T('\\'), nStart );
		if ( nSlash == -1 || nSlash == strDir.GetLength() - 1 )
			break;
		CString strSubDir = strDir.Left( nSlash + 1 );
		if ( strSubDir.GetLength() > MAX_PATH )
			strSubDir = _T("\\\\?\\") + strSubDir;
		dwAttr = GetFileAttributes( strSubDir );
		if ( ( dwAttr == INVALID_FILE_ATTRIBUTES ) || ! ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			if ( ! CreateDirectory( strSubDir, NULL ) )
				return FALSE;
		}
		nStart = nSlash + 1;
	}
	return CreateDirectory( bMax ? CString( _T("\\\\?\\") ) + szPath : szPath, NULL );
}

void DeleteFolders(CStringList& pList)
{
	// Primarily from WndDownloads torrents
	while ( ! pList.IsEmpty() )
	{
		const CString strPath = pList.RemoveHead();
		if ( PathIsDirectoryEmpty( strPath ) )
			RemoveDirectory( strPath );
	}
}

void DeleteFiles(CStringList& pList)
{
	// From WndDownloads
	while ( ! pList.IsEmpty() )
	{
		const CString strFirstPath = pList.GetHead();

		CDeleteFileDlg dlg;
		dlg.m_bAll = ( pList.GetCount() > 1 );

		{
			CQuickLock pLibraryLock( Library.m_pSection );

			if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( strFirstPath ) )
			{
				dlg.m_sName	= pFile->m_sName;
				dlg.m_sComments = pFile->m_sComments;
				dlg.m_nRateValue = pFile->m_nRating;
			}
			else
			{
				dlg.m_sName = PathFindFileName( strFirstPath );
			}
		}

		if ( dlg.DoModal() != IDOK )
			break;

		for ( INT_PTR nProcess = dlg.m_bAll ? pList.GetCount() : 1 ;
			nProcess > 0 && pList.GetCount() > 0 ; nProcess-- )
		{
			const CString strPath = pList.RemoveHead();

			{
				CQuickLock pTransfersLock( Transfers.m_pSection );	// Can clear uploads and downloads
				CQuickLock pLibraryLock( Library.m_pSection );

				if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( strPath ) )
				{
					// It's a library file
					dlg.Apply( pFile );
					pFile->Delete();
					continue;
				}
			}

			// It's a wild file
			BOOL bToRecycleBin = ( ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) == 0 );
			DeleteFileEx( strPath, TRUE, bToRecycleBin, TRUE );
		}
	}

	// Note: Cleanup empty folders for multifile torrents
}

BOOL DeleteFileEx(LPCTSTR szFileName, BOOL bShared, BOOL bToRecycleBin, BOOL bEnableDelayed)
{
	// Should be double zeroed long path
	DWORD len = GetLongPathName( szFileName, NULL, 0 );
	BOOL bLong = len ? TRUE : FALSE;
	if ( ! bLong )
		len = lstrlen( szFileName );

	auto_array< TCHAR > szPath( new TCHAR[ len + 1 ] );
	if ( bLong )
		GetLongPathName( szFileName, szPath.get(), len );
	else
		lstrcpy( szPath.get(), szFileName );
	szPath[ len ] = 0;

	if ( bShared )	// Stop uploads
		theApp.OnRename( szPath.get(), NULL );

	DWORD dwAttr = GetFileAttributes( szPath.get() );
	if ( ( dwAttr != INVALID_FILE_ATTRIBUTES ) &&		// Filename exist
		( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) == 0 )	// Not a folder
	{
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
		{
			DeleteFile( szPath.get() );
		}

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

	return TRUE;
}

void PurgeDeletes()
{
	HKEY hKey = NULL;
	LSTATUS nResult = RegOpenKeyEx( HKEY_CURRENT_USER,
		REGISTRY_KEY _T("\\Delete"), 0, KEY_ALL_ACCESS, &hKey );
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
				pRemove.AddTail( szPath );
		}

		while ( ! pRemove.IsEmpty() )
		{
			nResult = RegDeleteValue( hKey, pRemove.RemoveHead() );
		}

		nResult = RegCloseKey( hKey );
	}
}

// Direct Web Browsing Page (About.htm/etc.)
CString LoadHTML(HINSTANCE hInstance, UINT nResourceID)
{
	CString strBody;
	BOOL bGZIP = FALSE;

	HRSRC hRes = FindResource( hInstance, MAKEINTRESOURCE( nResourceID ), RT_HTML );

	if ( ! hRes )	// Try *.xml.gz
	{
		hRes = FindResource( hInstance, MAKEINTRESOURCE( nResourceID ), RT_GZIP );
		if ( ! hRes ) return strBody;
		bGZIP = TRUE;
	}

	const DWORD nSize = SizeofResource( hInstance, hRes );
	HGLOBAL hMemory = LoadResource( hInstance, hRes );
	if ( ! hMemory ) return strBody;

	LPCSTR pszInput = (LPCSTR)LockResource( hMemory );
	if ( ! pszInput ) return strBody;

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
	else // .xml
	{
		int nWide = MultiByteToWideChar( 0, 0, pszInput, nSize, NULL, 0 );
		LPTSTR pszOutput = strBody.GetBuffer( nWide + 1 );
		MultiByteToWideChar( 0, 0, pszInput, nSize, pszOutput, nWide );
		pszOutput[ nWide ] = _T('\0');
		strBody.ReleaseBuffer();
	}

	FreeResource( hMemory );

	return strBody;
}

// Direct Web Browsing Style Resources (About.htm/etc.)
const struct
{
	LPCTSTR szPath;
	UINT	nID;
	LPCTSTR	szType;
	LPCTSTR	szContentType;
} WebResources [] =
{
	{ _T("/remote/header.png"),			IDR_HOME_HEADER,		RT_PNG,			_T("image/png") },
	{ _T("/remote/header_repeat.png"),	IDR_HOME_HEADER_REPEAT,	RT_PNG,			_T("image/png") },
	{ _T("/favicon.ico"),				IDI_FAVICON,			RT_GROUP_ICON,	_T("image/x-icon") },
	{ NULL, NULL, NULL, NULL }
};

bool ResourceRequest(const CString& strPath, CBuffer& pResponse, CString& sHeader)
{
	bool ret = false;

	for ( int i = 0 ; WebResources[ i ].szPath ; i++ )
	{
		if ( strPath.Compare( WebResources[ i ].szPath ) != 0 )
			continue;

		HMODULE hModule = AfxGetResourceHandle();
		if ( HRSRC hRes = FindResource( hModule,
			MAKEINTRESOURCE( WebResources[ i ].nID ), WebResources[ i ].szType ) )
		{
			if ( HGLOBAL hMemory = LoadResource( hModule, hRes ) )
			{
				DWORD nSize = SizeofResource( hModule, hRes );
				if ( BYTE* pSource = (BYTE*)LockResource( hMemory ) )
				{
					if ( WebResources[ i ].szType == RT_GROUP_ICON )
					{
						// Save main header
						ICONDIR* piDir = (ICONDIR*)pSource;
						DWORD dwTotalSize = sizeof( ICONDIR ) + sizeof( ICONDIRENTRY ) * piDir->idCount;
						pResponse.EnsureBuffer( dwTotalSize );
						CopyMemory( pResponse.m_pBuffer, piDir, sizeof( ICONDIR ) );

						GRPICONDIRENTRY* piDirEntry = (GRPICONDIRENTRY*)( pSource + sizeof( ICONDIR ) );

						// Find all subicons
						for ( WORD j = 0 ; j < piDir->idCount ; j++ )
						{
							// pResponse.m_pBuffer may be changed
							ICONDIRENTRY* piEntry = (ICONDIRENTRY*)( pResponse.m_pBuffer + sizeof( ICONDIR ) );

							// Load subicon
							if ( HRSRC hResIcon = FindResource( hModule, MAKEINTRESOURCE( piDirEntry[ j ].nID ), RT_ICON ) )
							{
								if ( HGLOBAL hMemoryIcon = LoadResource( hModule, hResIcon ) )
								{
									DWORD nSizeIcon = SizeofResource( hModule, hResIcon );

									BITMAPINFOHEADER* piImage = (BITMAPINFOHEADER*)LockResource( hMemoryIcon );

									// Fill subicon header
									piEntry[ j ].bWidth = piDirEntry[ j ].bWidth;
									piEntry[ j ].bHeight = piDirEntry[ j ].bHeight;
									piEntry[ j ].wPlanes = piDirEntry[ j ].wPlanes;
									piEntry[ j ].bColorCount = piDirEntry[ j ].bColorCount;
									piEntry[ j ].bReserved = 0;
									piEntry[ j ].wBitCount = piDirEntry[ j ].wBitCount;
									piEntry[ j ].dwBytesInRes = nSizeIcon;
									piEntry[ j ].dwImageOffset = dwTotalSize;

									// Save subicon
									pResponse.EnsureBuffer( dwTotalSize + nSizeIcon );
									CopyMemory( pResponse.m_pBuffer + dwTotalSize, piImage, nSizeIcon );
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

					sHeader.Format(	_T("Content-Type: %s\r\n"), WebResources[ i ].szContentType);
					ret = true;
				}
				FreeResource( hMemory );
			}
		}
		break;
	}

	return ret;
}

bool MarkFileAsDownload(const CString& sFilename)
{
	LPCTSTR pszExt = PathFindExtension( (LPCTSTR)sFilename );
	if ( ! pszExt ) return false;

	bool bSuccess = false;

	if ( Settings.Library.MarkFileAsDownload )
	{
		// ToDo: pFile->m_bVerify and IDS_LIBRARY_VERIFY_FIX warning features
		// could be merged with this function, because they resemble the security warning.
		// Shouldn't we unblock files from the application without forcing user to do that manually?
		if ( CFileExecutor::IsSafeExecute( pszExt ) )
			return false;

		// Temporary clear R/O attribute
		BOOL bChanged = FALSE;
		DWORD dwOrigAttr = GetFileAttributes( sFilename );
		if ( dwOrigAttr != INVALID_FILE_ATTRIBUTES && ( dwOrigAttr & FILE_ATTRIBUTE_READONLY ) )
			bChanged = SetFileAttributes( sFilename, dwOrigAttr & ~FILE_ATTRIBUTE_READONLY );

		HANDLE hStream = CreateFile( sFilename + _T(":Zone.Identifier"),
			GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if ( hStream == INVALID_HANDLE_VALUE )
		{
			HANDLE hFile = CreateFile( sFilename,
				GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL );

			if ( hFile != INVALID_HANDLE_VALUE )
			{
				hStream = CreateFile( sFilename + _T(":Zone.Identifier"),
					GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
				CloseHandle( hFile );
			}
		}

		if ( hStream != INVALID_HANDLE_VALUE )
		{
			DWORD dwWritten = 0;
			bSuccess = ( WriteFile( hStream, "[ZoneTransfer]\r\nZoneID=3\r\n", 26,
				&dwWritten, NULL ) && dwWritten == 26 );
			CloseHandle( hStream );
		}
		else
		{
			TRACE( "MarkFileAsDownload() : CreateFile \"%s\" error %d\n", (LPCSTR)CT2A( sFilename ), GetLastError() );
		}

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
			GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			Hashes::Guid oTmpGUID;
			DWORD dwReaded = 0;
			bSuccess = ( ReadFile( hFile, &*oTmpGUID.begin(), oTmpGUID.byteCount,
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

		HANDLE hStream = CreateFile( sFilename + _T(":PeerProject.GUID"),
			GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if ( hStream == INVALID_HANDLE_VALUE )
		{
			HANDLE hFile = CreateFile( sFilename,
				GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL );

			if ( hFile != INVALID_HANDLE_VALUE )
			{
				hStream = CreateFile( sFilename + _T(":PeerProject.GUID"),
					GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
				CloseHandle( hFile );
			}
		}

		if ( hStream != INVALID_HANDLE_VALUE )
		{
			DWORD dwWritten = 0;
			bSuccess = ( WriteFile( hStream, &*oGUID.begin(), oGUID.byteCount,
				&dwWritten, NULL ) && dwWritten == oGUID.byteCount );
			CloseHandle( hStream );
		}
		//else
		//	TRACE( "SaveGUID() : CreateFile \"%s\" error %d\n", sFilename, GetLastError() );

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
			_tcsncpy_s( szDefaultPath, MAX_PATH, (LPCTSTR)theApp.GetDocumentsFolder(), MAX_PATH - 1 );
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
	_tcsncpy_s( szDefaultPath, MAX_PATH, szPath, MAX_PATH - 1 );

	return CString( szPath );
}

BOOL PostMainWndMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if ( CMainWnd* pWnd = theApp.SafeMainWnd() )
		return pWnd->PostMessage( Msg, wParam, lParam );

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

BOOL IsUserFullscreen()
{
	// Detect system availability
	if ( theApp.m_bIsVistaOrNewer )
	{
		QUERY_USER_NOTIFICATION_STATE state;
		SHQueryUserNotificationState( &state );
		if ( state != QUNS_ACCEPTS_NOTIFICATIONS )
			return TRUE;
	}
	else // XP external fullscreen
	{
		const HWND hActive = GetForegroundWindow();
		if ( hActive && hActive != AfxGetMainWnd()->GetSafeHwnd() && ( GetWindowLong( hActive, GWL_EXSTYLE ) & WS_EX_TOPMOST ) )
		{
			RECT rcWindow;
			GetWindowRect( hActive, &rcWindow );
			return rcWindow.top == 0 && rcWindow.left == 0 &&
				rcWindow.right  == GetSystemMetrics(SM_CXSCREEN) &&
				rcWindow.bottom == GetSystemMetrics(SM_CYSCREEN);
		}
	}

	return FALSE;
}

void ClearSkins()
{
	// Commandline "-noskin" reset
	HKEY hRoot;
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGISTRY_KEY, 0, KEY_ALL_ACCESS, &hRoot ) != ERROR_SUCCESS )
		return;
	RegDeleteKey( hRoot, _T("\\Settings\\Language") );
	RegDeleteKey( hRoot, _T("\\Settings\\LanguageRTL") );

	// Prefer native Vista+ way
	OSVERSIONINFOEX pVersion = { sizeof( OSVERSIONINFOEX ) };
	GetVersionEx( (OSVERSIONINFO*)&pVersion );
	if ( pVersion.dwMajorVersion > 5 )
	{
		RegDeleteTree( hRoot, _T("\\Skins") );
		RegDeleteTree( hRoot, _T("\\Toolbars") );
		RegDeleteTree( hRoot, _T("\\Windows") );
		RegDeleteTree( hRoot, _T("\\ListStates") );
		RegDeleteTree( hRoot, _T("\\Interface") );
	}
	else // XP/2000
	{
		SHDeleteKey( hRoot, _T("\\Skins") );
		SHDeleteKey( hRoot, _T("\\Toolbars") );
		SHDeleteKey( hRoot, _T("\\Windows") );
		SHDeleteKey( hRoot, _T("\\ListStates") );
		SHDeleteKey( hRoot, _T("\\Interface") );
	}

	RegCloseKey( hRoot );
}


template <>
__int8 GetRandomNum<__int8>(const __int8& min, const __int8& max)
{
	return (__int8)GetRandomNum<unsigned __int8>( min, max );
}

template <>
__int16 GetRandomNum<__int16>(const __int16& min, const __int16& max)
{
	return (__int16)GetRandomNum<unsigned __int16>( min, max );
}

template <>
__int32 GetRandomNum<__int32>(const __int32& min, const __int32& max)
{
	return (__int32)GetRandomNum<unsigned __int32>( min, max );
}

template <>
__int64 GetRandomNum<__int64>(const __int64& min, const __int64& max)
{
	return (__int64)GetRandomNum<unsigned __int64>( min, max );
}
