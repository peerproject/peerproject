//
// TorrentWizard.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008-2012
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#include "StdAfx.h"
#include "TorrentWizard.h"
#include "WizardSheet.h"
#include "CmdLine.h"

#include "PageWelcome.h"
#include "PageExpert.h"
#include "PageSingle.h"
#include "PagePackage.h"
#include "PageTracker.h"
#include "PageComment.h"
#include "PageOutput.h"
#include "PageFinished.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTorrentWizardApp, CWinApp)
	ON_COMMAND(ID_HELP, CTorrentWizardApp::OnHelp)
END_MESSAGE_MAP()

CTorrentWizardApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CTorrentWizardApp construction

CTorrentWizardApp::CTorrentWizardApp()
{
}

void CTorrentWizardApp::OnHelp()
{
	CWinApp::OnHelp();
//	ShellExecute( NULL, NULL, _T("http://peerproject.org/torrentwizard/") +
//		static_cast< CWizardPage* >( m_pSheet->GetActivePage() )->m_sHelp,
//		NULL, NULL, SW_SHOWNORMAL );
}

/////////////////////////////////////////////////////////////////////////////
// CTorrentWizardApp initialization

BOOL CTorrentWizardApp::InitInstance()
{
	CCommandLineInfoEx cmdInfo;
	ParseCommandLine(cmdInfo);

	cmdInfo.GetOption( _T("sourcefile"), m_sCommandLineSourceFile );
	cmdInfo.GetOption( _T("destination"), m_sCommandLineDestination );
	cmdInfo.GetOption( _T("tracker"), m_sCommandLineTracker );
	cmdInfo.GetOption( _T("comment"), m_sCommandLineComment );

	if( ! m_sCommandLineSourceFile.IsEmpty() &&
		! m_sCommandLineDestination.IsEmpty() &&
		! m_sCommandLineTracker.IsEmpty() )
	{
		if ( m_sCommandLineComment.IsEmpty() )
			m_sCommandLineComment = _T("http://peerproject.org/");
	}
	else
	{
		// Test prior app instance for non-commandline
		HANDLE pMutex = CreateMutex( NULL, FALSE, _T("Global\\TorrentWizard") );
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			// Show first instance
			//if ( CWnd* pWnd = CWnd::FindWindow( _T("TorrentWizard"), NULL ) )
			//{
			//	pWnd->SendMessage( WM_SYSCOMMAND, SC_RESTORE );
			//	pWnd->ShowWindow( SW_SHOWNORMAL );
			//	pWnd->BringWindowToTop();
			//	pWnd->SetForegroundWindow();
			//}

			if ( MessageBox( NULL,
				(LPCWSTR)L"TorrentWizard is currently running.\nDo you wish to open a new window?",
				(LPCWSTR)L"PeerProject TorrentWizard",
				MB_ICONQUESTION | MB_OKCANCEL | MB_SETFOREGROUND ) == IDCANCEL )
			{
				CloseHandle( pMutex );
				return FALSE;
			}
		}
		// else Continue...
	}

	SetRegistryKey( _T("PeerProject") );

	InitEnvironment();
	InitResources();

	CWizardSheet	pSheet;
	CWelcomePage	pWelcome;
	CExpertPage		pExpert;
	CSinglePage		pSingle;
	CPackagePage	pPackage;
	CTrackerPage	pTracker;
	CCommentPage	pComment;
	COutputPage		pOutput;
	CFinishedPage	pFinished;

	m_pSheet = &pSheet;

	pSheet.AddPage( &pWelcome );
	pSheet.AddPage( &pExpert );
	pSheet.AddPage( &pSingle );
	pSheet.AddPage( &pPackage );
	pSheet.AddPage( &pTracker );
	pSheet.AddPage( &pComment );
	pSheet.AddPage( &pOutput );
	pSheet.AddPage( &pFinished );

	pSheet.DoModal();

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CTorrentWizardApp environment

void CTorrentWizardApp::InitEnvironment()
{
	TCHAR szPath[260];
	DWORD dwSize = 0;

	m_nVersion[0] = m_nVersion[1] = m_nVersion[2] = m_nVersion[3] = 0;

	if ( GetModuleFileName( NULL, szPath, 260 ) )
	{
		m_sPath	= szPath;
		dwSize	= GetFileVersionInfoSize( szPath, &dwSize );
	}

	if ( dwSize > 0 )
	{
		BYTE* pBuffer = new BYTE[ dwSize ];

		if ( GetFileVersionInfo( szPath, NULL, dwSize, pBuffer ) )
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

	m_sVersion.Format( _T("%i.%i.%i.%i"),
		m_nVersion[0], m_nVersion[1], m_nVersion[2], m_nVersion[3] );

	// Obsolete:
	//OSVERSIONINFO pVersion;
	//pVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	//GetVersionEx( &pVersion );
	//
	//m_bNT = ( pVersion.dwPlatformId == VER_PLATFORM_WIN32_NT );
}

/////////////////////////////////////////////////////////////////////////////
// CTorrentWizardApp resources

void CTorrentWizardApp::InitResources()
{
	OSVERSIONINFO pVersion;
	pVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &pVersion );
	const BOOL bVista = pVersion.dwMajorVersion > 5;

	m_fntNormal.CreateFont( -11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, ( bVista ? _T("Segoe UI") : _T("Tahoma") ) );

	m_fntBold.CreateFont( -11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, ( bVista ? _T("Segoe UI") : _T("Tahoma") ) );

	m_fntLine.CreateFont( -11, 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, ( bVista ? _T("Segoe UI") : _T("Tahoma") ) );

	m_fntTiny.CreateFont( -8, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, ( bVista ? _T("Segoe UI") : _T("Tahoma") ) );

	srand( GetTickCount() );
}

/////////////////////////////////////////////////////////////////////////////
// Utilities

CString SmartSize(QWORD nVolume)
{
	LPCTSTR pszUnit = _T("B");
	CString strVolume;

	if ( nVolume < 1024 )
	{
		strVolume.Format( _T("%lu %s"), (DWORD)nVolume, pszUnit );
		return strVolume;
	}

	nVolume /= 1024;

	if ( nVolume < 1024 )
		strVolume.Format( _T("%lu K%s"), (DWORD)nVolume, pszUnit );
	else if ( nVolume < 1024*1024 )
		strVolume.Format( _T("%.2lf M%s"), (double)(__int64)nVolume / 1024, pszUnit );
	else if ( nVolume < 1024*1024*1024 )
		strVolume.Format( _T("%.3lf G%s"), (double)(__int64)nVolume / (1024*1024), pszUnit );
	else
		strVolume.Format( _T("%.3lf T%s"), (double)(__int64)nVolume / (1024*1024*1024), pszUnit );

	return strVolume;
}
