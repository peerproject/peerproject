//
// DlgTorrentSeed.cpp
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
#include "Network.h"
#include "Library.h"
#include "SharedFile.h"
#include "Transfers.h"
#include "Downloads.h"
#include "Download.h"
#include "PeerProjectURL.h"
#include "HttpRequest.h"
#include "DlgTorrentSeed.h"
#include "WndMain.h"
#include "WndDownloads.h"
#include "DlgExistingFile.h"
#include "DlgHelp.h"
#include "DownloadTask.h"
#include "FragmentedFile.h"
#include "LibraryHistory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CTorrentSeedDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CTorrentSeedDlg, CSkinDialog)
	ON_BN_CLICKED(IDC_DOWNLOAD, OnDownload)
	ON_BN_CLICKED(IDC_SEED, OnSeed)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

const DWORD BUFFER_SIZE = 2 * 1024 * 1024u;


/////////////////////////////////////////////////////////////////////////////
// CTorrentSeedDlg construction

CTorrentSeedDlg::CTorrentSeedDlg(LPCTSTR pszTorrent, BOOL bForceSeed, CWnd* pParent)
	: CSkinDialog	( CTorrentSeedDlg::IDD, pParent )
	, m_sTorrent	( pszTorrent )
	, m_bForceSeed	( bForceSeed )
{
}

void CTorrentSeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_DOWNLOAD, m_wndDownload );
	DDX_Control( pDX, IDC_SEED, m_wndSeed );
	DDX_Control( pDX, IDC_PROGRESS, m_wndProgress );
}

/////////////////////////////////////////////////////////////////////////////
// CTorrentSeedDlg message handlers

BOOL CTorrentSeedDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( L"CTorrentSeedDlg", IDR_MAINFRAME );

	if ( Settings.General.LanguageRTL )
		m_wndProgress.ModifyStyleEx( WS_EX_LAYOUTRTL, 0, 0 );
	m_wndProgress.SetRange( 0, 1000 );
	m_wndProgress.SetPos( 0 );

	if ( m_bForceSeed )
	{
		m_wndDownload.EnableWindow( FALSE );
		if ( Settings.BitTorrent.AutoSeed )
			PostMessage( WM_TIMER, 4 );
	}
	// if ( m_bForceSeed ) m_wndDownload.ShowWindow( SW_HIDE );

	return TRUE;
}

void CTorrentSeedDlg::OnDownload()
{
	/*CWnd* pWnd =*/ AfxGetMainWnd();
	CBTInfo* pTorrent = new CBTInfo();

	if ( ! pTorrent->LoadTorrentFile( m_sTorrent ) )
	{
		delete pTorrent;
		theApp.Message( MSG_ERROR, IDS_BT_PREFETCH_ERROR, (LPCTSTR)m_sTorrent );
		EndDialog( IDOK );
		return;
	}

	if ( pTorrent->HasEncodingError() )		// Check the torrent is valid
		CHelpDlg::Show( _T("GeneralHelp.BadTorrentEncoding") );

	CPeerProjectURL oURL( pTorrent );

	CSingleLock oLibraryLock( &Library.m_pSection, TRUE );

	CExistingFileDlg::Action action = CExistingFileDlg::CheckExisting( &oURL );
	if ( action == CExistingFileDlg::Cancel )
		return;
	if ( action != CExistingFileDlg::Download )
	{
		EndDialog( IDOK );
		return;
	}

	oLibraryLock.Unlock();

	CDownload* pDownload = Downloads.Add( oURL );

	if ( pDownload == NULL )
	{
		EndDialog( IDOK );
		return;
	}

	if ( ! Network.IsWellConnected() &&
		( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) == 0 )
	{
		Network.Connect( TRUE );
	}

	CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();
	pMainWnd->m_pWindows.Open( RUNTIME_CLASS(CDownloadsWnd) );

	if ( Settings.Downloads.ShowMonitorURLs )
	{
		CSingleLock pTransfersLock( &Transfers.m_pSection, TRUE );
		if ( Downloads.Check( pDownload ) ) pDownload->ShowMonitor( &pTransfersLock );
	}

	EndDialog( IDOK );
}

void CTorrentSeedDlg::OnSeed()
{
	m_wndDownload.EnableWindow( FALSE );
	m_wndSeed.EnableWindow( FALSE );
	m_bCancel = FALSE;

	if ( m_pInfo.LoadTorrentFile( m_sTorrent ) )
	{
		if ( m_pInfo.HasEncodingError() )		// Check the torrent is valid
			CHelpDlg::Show( _T("GeneralHelp.BadTorrentEncoding") );

		if ( Downloads.FindByBTH( m_pInfo.m_oBTH ) == NULL || m_pInfo.GetCount() == 1 )
		{
			// Connect if (we aren't)
			if ( ! Network.IsConnected() ) Network.Connect();

			// Update the last seeded torrent
			CSingleLock pLock( &Library.m_pSection );
			if ( pLock.Lock( 250 ) )
			{
				LibraryHistory.LastSeededTorrent.m_sName		= m_pInfo.m_sName.Left( 40 );
				LibraryHistory.LastSeededTorrent.m_sPath		= m_sTorrent;
				LibraryHistory.LastSeededTorrent.m_tLastSeeded	= static_cast< DWORD >( time( NULL ) );

				// If it's a 'new' torrent, reset the counters
				if ( !validAndEqual( LibraryHistory.LastSeededTorrent.m_oBTH, m_pInfo.m_oBTH ) )
				{
					LibraryHistory.LastSeededTorrent.m_nUploaded	= 0;
					LibraryHistory.LastSeededTorrent.m_nDownloaded	= 0;
					LibraryHistory.LastSeededTorrent.m_oBTH 		= m_pInfo.m_oBTH;
				}
			}

			// Start the torrent seed process
			BeginThread( "DlgTorrentSeed" );
		}
		else	// We are already seeding the torrent
		{
			CString strFormat, strMessage;
			LoadString( strFormat, IDS_BT_SEED_ALREADY );
			strMessage.Format( strFormat, (LPCTSTR)m_pInfo.m_sName );
			AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
			EndDialog( IDOK );
		}
	}
	else
	{
		// We couldn't load the .torrent file
		CString strFormat, strMessage;
		LoadString( strFormat, IDS_BT_SEED_PARSE_ERROR );
		strMessage.Format( strFormat, (LPCTSTR)m_sTorrent );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		EndDialog( IDOK );
	}
}

void CTorrentSeedDlg::OnCancel()
{
	if ( m_wndDownload.IsWindowEnabled() || m_bCancel )
		CSkinDialog::OnCancel();
	else
		m_bCancel = TRUE;
}

void CTorrentSeedDlg::OnDestroy()
{
	m_bCancel = TRUE;
	CloseThread();

	CSkinDialog::OnDestroy();
}

void CTorrentSeedDlg::OnTimer(UINT_PTR nIDEvent)
{
	CSkinDialog::OnTimer( nIDEvent );

	if ( nIDEvent == 1 )
	{
		EndDialog( IDOK );
	}
	else if ( nIDEvent == 2 )
	{
		if ( m_bCancel == FALSE ) AfxMessageBox( m_sMessage, MB_ICONEXCLAMATION );
		EndDialog( IDCANCEL );
	}
	else if ( nIDEvent == 3 )
	{
		if ( m_nScaled != m_nOldScaled )
		{
			m_nOldScaled = m_nScaled;
			m_wndProgress.SetPos( m_nScaled );
		}
	}
	else if ( nIDEvent == 4 )
	{
		OnSeed();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTorrentSeedDlg Torrent Handling

BOOL CTorrentSeedDlg::LoadTorrent(CString strPath)
{
	if ( ! m_pInfo.LoadTorrentFile( strPath ) )
		return FALSE;	// Try again with manual Dialog

	if ( Downloads.FindByBTH( m_pInfo.m_oBTH ) == NULL )		// Not already listed
	{
		if ( ! Network.IsConnected() )
			Network.Connect();

		CSingleLock pTransfersLock( &Transfers.m_pSection );	// Rare new .torrent crashfix elsewhere?
		if ( ! pTransfersLock.Lock( 2000 ) ) return FALSE;

		CPeerProjectURL oURL( new CBTInfo( m_pInfo ) );
		CDownload* pDownload = Downloads.Add( oURL );

		if ( pDownload == NULL )
			return FALSE;

		// ToDo: Fix existing fragment detection instead.

		// Try Seeding First.
		pDownload->SeedTorrent( m_sMessage );

		if ( pDownload->GetVolumeRemaining() == 0 )
		{
			pTransfersLock.Unlock();
			CSingleLock pLock( &Library.m_pSection );
			if ( pLock.Lock( 250 ) )
			{
				// Update last seeded torrent
				LibraryHistory.LastSeededTorrent.m_sName = m_pInfo.m_sName.Left( 40 );
				LibraryHistory.LastSeededTorrent.m_sPath = strPath;
				LibraryHistory.LastSeededTorrent.m_tLastSeeded = static_cast< DWORD >( time( NULL ) );

				// If 'new' torrent, reset the counters
				if ( ! validAndEqual( LibraryHistory.LastSeededTorrent.m_oBTH, m_pInfo.m_oBTH ) )
				{
					LibraryHistory.LastSeededTorrent.m_nUploaded	= 0;
					LibraryHistory.LastSeededTorrent.m_nDownloaded	= 0;
					LibraryHistory.LastSeededTorrent.m_oBTH 		= m_pInfo.m_oBTH;
				}
			}
		}
		else // Cannot fully seed, so just download
		{
			pDownload->Remove();
			pDownload = Downloads.Add( oURL );

			if ( Settings.Downloads.ShowMonitorURLs )
			{
			//	CSingleLock pTransfersLock( &Transfers.m_pSection, TRUE );
				if ( Downloads.Check( pDownload ) )
					pDownload->ShowMonitor( &pTransfersLock );
			}

			pTransfersLock.Unlock();
		}
	}

	CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();
	pMainWnd->m_pWindows.Open( RUNTIME_CLASS(CDownloadsWnd) );

	if ( m_pInfo.HasEncodingError() )	// Check torrent is valid
		CHelpDlg::Show( _T("GeneralHelp.BadTorrentEncoding") );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CTorrentSeedDlg thread run

void CTorrentSeedDlg::OnRun()
{
	if ( CreateDownload() )
		PostMessage( WM_TIMER, 1 );
	else
		PostMessage( WM_TIMER, 2 );
}

BOOL CTorrentSeedDlg::CreateDownload()
{
	CSingleLock pTransfersLock( &Transfers.m_pSection );
	if ( pTransfersLock.Lock( 2000 ) )
	{
		if ( Downloads.FindByBTH( m_pInfo.m_oBTH ) )
		{
			// Already seeding
			CString strFormat;
			LoadString( strFormat, IDS_BT_SEED_ALREADY );
			m_sMessage.Format( strFormat, (LPCTSTR)m_pInfo.m_sName );
		}
		else if ( CDownload* pDownload = Downloads.Add( CPeerProjectURL( new CBTInfo( m_pInfo ) ) ) )
		{
			if ( pDownload->SeedTorrent( m_sMessage ) )
				return TRUE;

			pDownload->Remove();
		}
	}

	if ( m_sMessage.IsEmpty() )
	{
		CString strFormat;
		LoadString( strFormat, IDS_BT_SEED_ERROR );
		m_sMessage.Format( strFormat, (LPCTSTR)m_pInfo.m_sName );
	}

	return FALSE;
}
