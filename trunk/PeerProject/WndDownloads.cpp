//
// WndDownloads.cpp
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
#include "Transfers.h"
#include "Downloads.h"
#include "Download.h"
#include "DownloadTask.h"
#include "DownloadSource.h"
#include "DownloadTransfer.h"
#include "DownloadTransferED2K.h"
#include "DownloadGroups.h"
#include "EDClient.h"
#include "EDClients.h"

#include "Library.h"
#include "LibraryMaps.h"
#include "SharedFile.h"
#include "FileExecutor.h"
#include "PeerProjectURL.h"
#include "ChatWindows.h"

#include "Skin.h"
#include "WindowManager.h"
#include "WndDownloads.h"
#include "WndUploads.h"
#include "WndBrowseHost.h"
#include "DlgDownload.h"
#include "DlgDownloadReviews.h"
#include "DlgSettingsManager.h"
#include "DlgDeleteFile.h"
#include "DlgFilePropertiesSheet.h"
#include "DlgDownloadSheet.h"
#include "DlgURLCopy.h"
#include "DlgURLExport.h"
#include "DlgHelp.h"
#include "Network.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CDownloadsWnd, CPanelWnd, 0)

BEGIN_MESSAGE_MAP(CDownloadsWnd, CPanelWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_MDIACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SETCURSOR()
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_RESUME, OnUpdateDownloadsResume)
	ON_COMMAND(ID_DOWNLOADS_RESUME, OnDownloadsResume)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_PAUSE, OnUpdateDownloadsPause)
	ON_COMMAND(ID_DOWNLOADS_PAUSE, OnDownloadsPause)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_CLEAR, OnUpdateDownloadsClear)
	ON_COMMAND(ID_DOWNLOADS_CLEAR, OnDownloadsClear)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_LAUNCH, OnUpdateDownloadsLaunch)
	ON_COMMAND(ID_DOWNLOADS_LAUNCH, OnDownloadsLaunch)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_VIEW_REVIEWS, OnUpdateDownloadsViewReviews)
	ON_COMMAND(ID_DOWNLOADS_VIEW_REVIEWS, OnDownloadsViewReviews)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_REMOTE_PREVIEW, OnUpdateDownloadsRemotePreview)
	ON_COMMAND(ID_DOWNLOADS_REMOTE_PREVIEW, OnDownloadsRemotePreview)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_SOURCES, OnUpdateDownloadsSources)
	ON_COMMAND(ID_DOWNLOADS_SOURCES, OnDownloadsSources)
	ON_COMMAND(ID_DOWNLOADS_CLEAR_COMPLETED, OnDownloadsClearCompleted)
	ON_COMMAND(ID_DOWNLOADS_CLEAR_PAUSED, OnDownloadsClearPaused)
	ON_UPDATE_COMMAND_UI(ID_TRANSFERS_DISCONNECT, OnUpdateTransfersDisconnect)
	ON_COMMAND(ID_TRANSFERS_DISCONNECT, OnTransfersDisconnect)
	ON_UPDATE_COMMAND_UI(ID_TRANSFERS_FORGET, OnUpdateTransfersForget)
	ON_COMMAND(ID_TRANSFERS_FORGET, OnTransfersForget)
	ON_UPDATE_COMMAND_UI(ID_TRANSFERS_CHAT, OnUpdateTransfersChat)
	ON_COMMAND(ID_TRANSFERS_CHAT, OnTransfersChat)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_ADD_SOURCE, OnUpdateDownloadsAddSource)
	ON_COMMAND(ID_DOWNLOADS_ADD_SOURCE, OnDownloadsAddSource)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_ENQUEUE, OnUpdateDownloadsEnqueue)
	ON_COMMAND(ID_DOWNLOADS_ENQUEUE, OnDownloadsEnqueue)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_AUTO_CLEAR, OnUpdateDownloadsAutoClear)
	ON_COMMAND(ID_DOWNLOADS_AUTO_CLEAR, OnDownloadsAutoClear)
	ON_UPDATE_COMMAND_UI(ID_TRANSFERS_CONNECT, OnUpdateTransfersConnect)
	ON_COMMAND(ID_TRANSFERS_CONNECT, OnTransfersConnect)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_SHOW_SOURCES, OnUpdateDownloadsShowSources)
	ON_COMMAND(ID_DOWNLOADS_SHOW_SOURCES, OnDownloadsShowSources)
	ON_UPDATE_COMMAND_UI(ID_BROWSE_LAUNCH, OnUpdateBrowseLaunch)
	ON_COMMAND(ID_BROWSE_LAUNCH, OnBrowseLaunch)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_BOOST, OnUpdateDownloadsBoost)
	ON_COMMAND(ID_DOWNLOADS_BOOST, OnDownloadsBoost)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_MONITOR, OnUpdateDownloadsMonitor)
	ON_COMMAND(ID_DOWNLOADS_MONITOR, OnDownloadsMonitor)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_FOLDER, OnUpdateDownloadsFolder)
	ON_COMMAND(ID_DOWNLOADS_FOLDER, OnDownloadsFolder)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_LAUNCH_COPY, OnUpdateDownloadsLaunchCopy)
	ON_COMMAND(ID_DOWNLOADS_LAUNCH_COPY, OnDownloadsLaunchCopy)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_FILE_DELETE, OnUpdateDownloadsFileDelete)
	ON_COMMAND(ID_DOWNLOADS_FILE_DELETE, OnDownloadsFileDelete)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_RATE, OnUpdateDownloadsRate)
	ON_COMMAND(ID_DOWNLOADS_RATE, OnDownloadsRate)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_MOVE_UP, OnUpdateDownloadsMove)
	ON_COMMAND(ID_DOWNLOADS_MOVE_UP, OnDownloadsMoveUp)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_MOVE_DOWN, OnUpdateDownloadsMove)
	ON_COMMAND(ID_DOWNLOADS_MOVE_DOWN, OnDownloadsMoveDown)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_MOVE_TOP, OnUpdateDownloadsMove)
	ON_COMMAND(ID_DOWNLOADS_MOVE_TOP, OnDownloadsMoveTop)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_MOVE_BOTTOM, OnUpdateDownloadsMove)
	ON_COMMAND(ID_DOWNLOADS_MOVE_BOTTOM, OnDownloadsMoveBottom)
	ON_COMMAND(ID_DOWNLOADS_SETTINGS, OnDownloadsSettings)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_FILTER_ALL, OnUpdateDownloadsFilterAll)
	ON_COMMAND(ID_DOWNLOADS_FILTER_ALL, OnDownloadsFilterAll)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_FILTER_ACTIVE, OnUpdateDownloadsFilterActive)
	ON_COMMAND(ID_DOWNLOADS_FILTER_ACTIVE, OnDownloadsFilterActive)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_FILTER_PAUSED, OnUpdateDownloadsFilterPaused)
	ON_COMMAND(ID_DOWNLOADS_FILTER_PAUSED, OnDownloadsFilterPaused)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_FILTER_QUEUED, OnUpdateDownloadsFilterQueued)
	ON_COMMAND(ID_DOWNLOADS_FILTER_QUEUED, OnDownloadsFilterQueued)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_FILTER_SOURCES, OnUpdateDownloadsFilterSources)
	ON_COMMAND(ID_DOWNLOADS_FILTER_SOURCES, OnDownloadsFilterSources)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_FILTER_SEEDS, OnUpdateDownloadsFilterSeeds)
	ON_COMMAND(ID_DOWNLOADS_FILTER_SEEDS, OnDownloadsFilterSeeds)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_LAUNCH_COMPLETE, OnUpdateDownloadsLaunchComplete)
	ON_COMMAND(ID_DOWNLOADS_LAUNCH_COMPLETE, OnDownloadsLaunchComplete)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_SHARE, OnUpdateDownloadsShare)
	ON_COMMAND(ID_DOWNLOADS_SHARE, OnDownloadsShare)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_URI, OnUpdateDownloadsURI)
	ON_COMMAND(ID_DOWNLOADS_URI, OnDownloadsURI)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_GROUP_SHOW, OnUpdateDownloadGroupShow)
	ON_COMMAND(ID_DOWNLOAD_GROUP_SHOW, OnDownloadGroupShow)
	ON_COMMAND(ID_DOWNLOADS_HELP, OnDownloadsHelp)
	ON_COMMAND(ID_DOWNLOADS_FILTER_MENU, OnDownloadsFilterMenu)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_CLEAR_INCOMPLETE, OnUpdateDownloadsClearIncomplete)
	ON_COMMAND(ID_DOWNLOADS_CLEAR_INCOMPLETE, OnDownloadsClearIncomplete)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_CLEAR_COMPLETE, OnUpdateDownloadsClearComplete)
	ON_COMMAND(ID_DOWNLOADS_CLEAR_COMPLETE, OnDownloadsClearComplete)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOADS_EDIT, OnUpdateDownloadsEdit)
	ON_COMMAND(ID_DOWNLOADS_EDIT, OnDownloadsEdit)
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDownloadsWnd construction

CDownloadsWnd::CDownloadsWnd()
: CPanelWnd( TRUE, TRUE )
, m_bMouseCaptured(false)
{
	Create( IDR_DOWNLOADSFRAME );
}

CDownloadsWnd::~CDownloadsWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadsWnd system message handlers

int CDownloadsWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	if ( ! m_wndTabBar.Create( this, WS_CHILD|CBRS_TOP, AFX_IDW_TOOLBAR+1 ) ) return -1;
	m_wndTabBar.SetBarStyle( m_wndTabBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_BORDER_BOTTOM );
	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_BORDER_TOP );
	m_wndToolBar.SetSyncObject( &Transfers.m_pSection );

	m_wndDownloads.Create( this, IDC_DOWNLOADS );

	LoadState( NULL, TRUE );

	SetTimer( 2, Settings.General.RefreshRate, NULL );
	PostMessage( WM_TIMER, 2 );

	SetTimer( 4, 10000, NULL );
	PostMessage( WM_TIMER, 4 );

	SetTimer( 6, 2 * 3600 * 1000, NULL );

	m_pDragList		= NULL;
	m_pDragImage	= NULL;
	m_hCursMove		= AfxGetApp()->LoadCursor( theApp.m_bIsVistaOrNewer ? IDC_MOVE_VISTA : IDC_MOVE );
	m_hCursCopy		= AfxGetApp()->LoadCursor( theApp.m_bIsVistaOrNewer ? IDC_COPY_VISTA : IDC_COPY );
	m_tSel			= 0;

	m_nMoreSourcesLimiter	= 8;
	m_tMoreSourcesTimer		= 0;
	m_tLastUpdate			= 0;

	return 0;
}

void CDownloadsWnd::OnDestroy()
{
	CancelDrag();
	KillTimer( 6 );
	KillTimer( 4 );
	KillTimer( 2 );
	SaveState();
	CPanelWnd::OnDestroy();
}

void CDownloadsWnd::OnSkinChange()
{
	CRect rc;
	GetClientRect( &rc );
	OnSize( 0, rc.Width(), rc.Height() );

	CPanelWnd::OnSkinChange();
	Skin.Translate( _T("CDownloadCtrl"), &m_wndDownloads.m_wndHeader);
	Skin.CreateToolBar( _T("CDownloadsWnd"), &m_wndToolBar );
	m_wndDownloads.OnSkinChange();
	m_wndTabBar.OnSkinChange();
}

void CDownloadsWnd::Update()
{
	if ( !IsWindow( m_hWnd ) ) return;

	if ( m_bMouseCaptured )
	{
		ClipCursor( NULL );
		ReleaseCapture();
		m_bMouseCaptured = false;
	}

	int nCookie = 0;
	DWORD tNow = GetTickCount();

	m_tLastUpdate = tNow;

	if ( Settings.General.GUIMode != GUI_BASIC && Settings.Downloads.ShowGroups )
	{
		nCookie = (int)tNow;
		m_wndTabBar.Update( nCookie );
	}

	m_wndDownloads.Update( nCookie );
}

BOOL CDownloadsWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_wndToolBar.m_hWnd )
	{
		if ( m_wndToolBar.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
			return TRUE;
	}

	if ( m_wndTabBar.m_hWnd )
	{
		if ( m_wndTabBar.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
			return TRUE;
	}

	return CPanelWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CDownloadsWnd::OnSize(UINT nType, int cx, int cy)
{
	CPanelWnd::OnSize( nType, cx, cy );

	CRect rc( 0, 0, cx, cy - Skin.m_nToolbarHeight );

	BOOL bTabs = ( Settings.General.GUIMode != GUI_BASIC ) && Settings.Downloads.ShowGroups;

	if ( bTabs )
		rc.top += 24;
	else
		m_wndTabBar.ShowWindow( SW_HIDE );

	HDWP hPos = BeginDeferWindowPos( bTabs ? 3 : 2 );
	DeferWindowPos( hPos, m_wndDownloads, NULL,
		rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_SHOWWINDOW );
	if ( bTabs ) DeferWindowPos( hPos, m_wndTabBar, NULL,
		rc.left, 0, rc.Width(), 24, SWP_NOZORDER|SWP_SHOWWINDOW );
	DeferWindowPos( hPos, m_wndToolBar, NULL,
		rc.left, rc.bottom, rc.Width(), Skin.m_nToolbarHeight, SWP_NOZORDER );
	EndDeferWindowPos( hPos );
}

void CDownloadsWnd::OnTimer(UINT_PTR nIDEvent)
{
	 // Purge old failed sources ( X-NAlt mesh ) every 2 hours
	if ( nIDEvent == 6 )
	{
		for ( POSITION pos = Downloads.GetIterator() ; pos ; )
		{
			CDownload* pDownload = Downloads.GetNext( pos );
			pDownload->ExpireFailedSources();
		}
	}

	// Reset Selection Timer event (posted by ctrldownloads)
	if ( nIDEvent == 5 ) m_tSel = 0;

	// Clear Completed event (10 second timer)
	if ( nIDEvent == 4 )
	{
		DWORD tNow = GetTickCount();

		if ( ( tNow - m_tMoreSourcesTimer ) > 8*60*1000 )
		{
			if ( m_nMoreSourcesLimiter < 15 ) m_nMoreSourcesLimiter ++;

			m_tMoreSourcesTimer = tNow;
		}

		// If some kind of auto-clear is active
		if ( Settings.Downloads.AutoClear || Settings.BitTorrent.AutoClear )
		{
			// Lock transfers section
			CSingleLock pLock( &Transfers.m_pSection );
			if ( ! pLock.Lock( 10 ) ) return;

			// Loop through all downloads
			for ( POSITION pos = Downloads.GetIterator() ; pos ; )
			{
				CDownload* pDownload = Downloads.GetNext( pos );

				if ( pDownload->IsCompleted() &&				// If the download has completed
					pDownload->IsPreviewVisible() == FALSE &&	// And isn't previewing
					tNow - pDownload->m_tCompleted > Settings.Downloads.ClearDelay )
				{
					// We might want to clear this download
					if ( pDownload->IsTorrent() == TRUE )		// It's a torrent
					{
						// Check the torrent clear settings
						if ( Settings.BitTorrent.AutoClear )
						{
							// If we're seeding and have reached the required ratio
							if ( pDownload->IsSeeding() && ( Settings.BitTorrent.ClearRatio < pDownload->GetRatio() ) )
							{
								pDownload->Remove();
							}
						}
					}
					else										// Or it's a normal download
					{
						// Check the general auto clear setting
						if ( Settings.Downloads.AutoClear )
						{
							pDownload->Remove();
						}
					}
				}
			}
		}
	}

	// Window Update event (2 second timer)
	if ( ( nIDEvent == 2 ) && ( m_pDragList == NULL ) )
	{
		for ( POSITION pos = Downloads.GetIterator() ; pos ; )
		{
			CDownload* pDownload = Downloads.GetNext( pos );
			if ( pDownload->m_bGotPreview && pDownload->m_bWaitingPreview )
			{
				pDownload->m_bWaitingPreview = FALSE;
				CFile pFile;
				CFileExecutor::Execute( pDownload->m_sPath + L".png", TRUE );
			}
		}

		// If the window is visible or hasn't been updated in 10 seconds
		if ( ( IsWindowVisible() && IsActive( FALSE ) ) || ( ( GetTickCount() - m_tLastUpdate ) > 10*1000 ) )
		{
			// Update the window
			Update();
		}
	}
}

void CDownloadsWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CPanelWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );

	if ( bActivate )
	{
		Update();
		m_wndDownloads.SetFocus();
	}
}

void CDownloadsWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	AfxGetMainWnd()->SendMessage( WM_MEASUREITEM, nIDCtl, (LPARAM)lpMeasureItemStruct );
}

void CDownloadsWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	AfxGetMainWnd()->SendMessage( WM_DRAWITEM, nIDCtl, (LPARAM)lpDrawItemStruct );
}

void CDownloadsWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CDownloadSource* pSource;
	CDownload* pDownload;

	CPoint ptLocal( point );
	m_wndDownloads.ScreenToClient( &ptLocal );
	m_tSel = 0;

	if ( m_wndDownloads.HitTest( ptLocal, &pDownload, &pSource, NULL, NULL ) )
	{
		if ( pSource != NULL )
		{
			pLock.Unlock();
			Skin.TrackPopupMenu( _T("CDownloadsWnd.Source"), point, ID_TRANSFERS_CONNECT );
			return;
		}
		else if ( pDownload->IsSeeding() )
		{
			pLock.Unlock();
			Skin.TrackPopupMenu( _T("CDownloadsWnd.Seeding"), point, ID_DOWNLOADS_LAUNCH_COMPLETE );
			return;
		}
		else if ( pDownload->IsCompleted() )
		{
			pLock.Unlock();
			Skin.TrackPopupMenu( _T("CDownloadsWnd.Completed"), point, ID_DOWNLOADS_LAUNCH_COMPLETE );
			return;
		}
	}

	if ( pDownload != NULL )
		Skin.TrackPopupMenu( _T("CDownloadsWnd.Download"), point, ID_DOWNLOADS_LAUNCH_COPY );
	else
		Skin.TrackPopupMenu( _T("CDownloadsWnd.Default"), point, ID_DOWNLOADS_HELP );

	pLock.Unlock();
}

BOOL CDownloadsWnd::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_TAB )
		{
			GetManager()->Open( RUNTIME_CLASS(CUploadsWnd) );
			return TRUE;
		}
	}

	return CPanelWnd::PreTranslateMessage( pMsg );
}

BOOL CDownloadsWnd::Select(CDownload* pSelect)
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	BOOL bFound = FALSE;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload == pSelect )
		{
			pDownload->m_bSelected = TRUE;
			bFound = TRUE;
		}
		else
		{
			pDownload->m_bSelected = FALSE;
		}

		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			pSource->m_bSelected = FALSE;
		}
	}

	Invalidate();
	m_tSel = 0;

	return bFound;
}

void CDownloadsWnd::Prepare()
{
	DWORD tNow = GetTickCount();
	if ( tNow - m_tSel < 250 ) return;

	m_nSelectedDownloads = 0;
	m_bSelAny = m_bSelDownload = m_bSelSource = m_bSelTrying = m_bSelPaused = FALSE;
	m_bSelNotPausedOrMoving = m_bSelNoPreview = m_bSelNotCompleteAndNoPreview = FALSE;
	m_bSelCompletedAndNoPreview = m_bSelStartedAndNotMoving = m_bSelCompleted = FALSE;
	m_bSelNotMoving = m_bSelBoostable = m_bSelSHA1orTTHorED2KorName = FALSE;
	m_bSelTorrent = m_bSelIdleSource = m_bSelActiveSource = FALSE;
	m_bSelBrowse = m_bSelChat = m_bSelShareState = FALSE;
	m_bSelShareConsistent = TRUE;
	m_bSelMoreSourcesOK = FALSE;
	m_bSelSourceAcceptConnections = m_bSelSourceExtended = m_bSelHasReviews = FALSE;
	m_bSelRemotePreviewCapable = FALSE;

	m_bConnectOkay = FALSE;

	CSingleLock pLock( &Transfers.m_pSection, FALSE );
	if ( ! pLock.Lock( 250 ) )
		return;

	BOOL bFirstShare = TRUE;
	BOOL bPreviewDone = FALSE;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload->m_bSelected )
		{
			m_nSelectedDownloads ++;
			m_bSelAny = TRUE;
			m_bSelDownload = TRUE;
			if ( pDownload->IsCompleted() )
				m_bSelCompleted = TRUE;
			if ( ! pDownload->IsMoving() )
				m_bSelNotMoving = TRUE;
			if ( ! pDownload->IsBoosted() )
				m_bSelBoostable = TRUE;
			if ( pDownload->m_oSHA1 || pDownload->m_oTiger || pDownload->m_oED2K || pDownload->m_oBTH || pDownload->m_oMD5 || pDownload->m_sName.GetLength() )
				m_bSelSHA1orTTHorED2KorName = TRUE;
			if ( pDownload->IsTorrent() )
				m_bSelTorrent = TRUE;
			if ( pDownload->IsTrying() )
				m_bSelTrying = TRUE;
			if ( pDownload->IsPaused() )
				m_bSelPaused = TRUE;
			else if ( ! pDownload->IsMoving() )
			{
				m_bSelNotPausedOrMoving = TRUE;
				if ( pDownload->IsTrying() && pDownload->FindSourcesAllowed( tNow ) )
					m_bSelMoreSourcesOK = TRUE;
			}

			if ( ! pDownload->IsPreviewVisible() )
			{
				m_bSelNoPreview = TRUE;
				if ( pDownload->IsCompleted() )
					m_bSelCompletedAndNoPreview = TRUE;
				else
					m_bSelNotCompleteAndNoPreview = TRUE;
			}
			if ( pDownload->IsStarted() && ! pDownload->IsMoving() )
				m_bSelStartedAndNotMoving = TRUE;
			if ( bFirstShare )
			{
				m_bSelShareState = pDownload->IsShared();
				bFirstShare = FALSE;
			}
			else if ( ( m_bSelShareState != 0 ) != pDownload->IsShared() )
			{
				m_bSelShareState = FALSE;
				m_bSelShareConsistent = FALSE;
			}
			if ( pDownload->GetReviewCount() > 0 )
				m_bSelHasReviews = TRUE;
		}

		pDownload->m_bRemotePreviewCapable = FALSE;

		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			if ( pSource->m_bSelected )
			{
				m_bSelAny = TRUE;
				m_bSelSource = TRUE;
				m_bSelSourceExtended = pSource->m_bClientExtended;
				if ( pSource->m_pTransfer == NULL )
					m_bSelIdleSource = TRUE;
				else
					m_bSelActiveSource = TRUE;
				if ( pSource->m_bClientExtended || pSource->m_nProtocol == PROTOCOL_ED2K  )
				{
					m_bSelBrowse = TRUE;
					m_bSelChat = TRUE;
				}
				if ( ! pSource->m_bPushOnly ) m_bSelSourceAcceptConnections = TRUE;
			}

			// Check if we could get remote previews (only from the connected sources for the efficiency)
			if ( !bPreviewDone && pDownload->m_bSelected && !pDownload->m_bRemotePreviewCapable &&
				 !pSource->m_bPreviewRequestSent && pSource->IsOnline() )
			{
				if ( pSource->m_nProtocol == PROTOCOL_ED2K )
				{
					// m_pTransfer is checked for validity by pSource->IsOnline()
					if ( static_cast< CDownloadTransferED2K* >( pSource->m_pTransfer )->m_pClient->m_bEmPreview )
					{
						pDownload->m_bRemotePreviewCapable = TRUE;
					}
				}
				else if ( pSource->m_nProtocol == PROTOCOL_HTTP && pSource->m_bPreview )
				{
					pDownload->m_bRemotePreviewCapable = TRUE;
				}
			}
		}

		if ( pDownload->m_bSelected )
		{
			m_bSelRemotePreviewCapable = pDownload->m_bRemotePreviewCapable || pDownload->m_bGotPreview;
			bPreviewDone = TRUE;
		}
	}

	if ( ( ! Settings.Connection.RequireForTransfers ) || ( Network.IsConnected() ) )
		m_bConnectOkay = TRUE;

	m_tSel = GetTickCount();
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadsWnd command handlers

void CDownloadsWnd::OnUpdateDownloadsResume(CCmdUI* pCmdUI)
{
	Prepare();
	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( m_bSelPaused || ( m_bSelDownload && ! m_bSelTrying && ! m_bSelCompleted ) );
	pCmdUI->Enable( m_bSelPaused || ( m_bSelDownload && ! m_bSelTrying && ! m_bSelCompleted ) );
}

void CDownloadsWnd::OnDownloadsResume()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload->m_bSelected )
		{
			pDownload->Resume();
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsPause(CCmdUI* pCmdUI)
{
	Prepare();
	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( ! m_bSelCompletedAndNoPreview && ( m_bSelNotPausedOrMoving || ! m_bSelDownload ) );
	pCmdUI->Enable( ! m_bSelCompletedAndNoPreview && m_bSelNotPausedOrMoving );
}

void CDownloadsWnd::OnDownloadsPause()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload->m_bSelected )
		{
			if ( ! pDownload->IsPaused() && ! pDownload->IsMoving() ) pDownload->Pause( TRUE );
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsClear(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelNoPreview || m_bSelSource );
}

void CDownloadsWnd::OnDownloadsClear()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CDownload*> pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected ) pList.AddTail( pDownload );
	}

	// If no downloads selected then process selected sources
	if ( pList.IsEmpty() )
		OnTransfersForget();

	while ( ! pList.IsEmpty() )
	{
		CDownload* pDownload = pList.RemoveHead();

		if ( Downloads.Check( pDownload ) )
		{
			if ( pDownload->IsPreviewVisible() )
			{
				// Can't
			}
			else if ( pDownload->IsCompleted() )
			{
				pDownload->Remove();
			}
			else if ( pDownload->IsStarted() )
			{
				CDeleteFileDlg dlg;
				dlg.m_sName = pDownload->m_sName;
				bool bShared = pDownload->IsShared();

				pLock.Unlock();
				if ( dlg.DoModal() != IDOK )
					break;
				pLock.Lock();

				if ( Downloads.Check( pDownload ) )
				{
					dlg.Create( pDownload, bShared );
					pDownload->Remove();
				}
			}
			else
			{
				pDownload->Remove();
			}
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsClearIncomplete(CCmdUI *pCmdUI)
{
	Prepare();

	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( m_bSelNotCompleteAndNoPreview || ! m_bSelDownload );
	pCmdUI->Enable( m_bSelNotCompleteAndNoPreview );
}

void CDownloadsWnd::OnDownloadsClearIncomplete()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CDownload*> pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected ) pList.AddTail( pDownload );
	}

	while ( ! pList.IsEmpty() )
	{
		CDownload* pDownload = pList.RemoveHead();

		if ( Downloads.Check( pDownload ) )
		{
			if ( ! pDownload->IsCompleted() && ! pDownload->IsPreviewVisible() )
			{
				if ( pDownload->IsStarted() )
				{
					CDeleteFileDlg dlg;
					dlg.m_sName = pDownload->m_sName;
					bool bShared = pDownload->IsShared();

					pLock.Unlock();
					if ( dlg.DoModal() != IDOK )
						break;
					pLock.Lock();

					if ( Downloads.Check( pDownload ) )
						dlg.Create( pDownload, bShared );
				}

				if ( Downloads.Check( pDownload ) )
					pDownload->Remove();
			}
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsClearComplete(CCmdUI *pCmdUI)
{
	Prepare();
	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( m_bSelCompletedAndNoPreview );
	pCmdUI->Enable( m_bSelCompletedAndNoPreview );
}

void CDownloadsWnd::OnDownloadsClearComplete()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload->m_bSelected )
		{
			if ( pDownload->IsCompleted() && ! pDownload->IsPreviewVisible() )
			{
				pDownload->Remove();
			}
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsViewReviews(CCmdUI* pCmdUI)
{
	Prepare();

	pCmdUI->Enable( m_bSelHasReviews );
}

void CDownloadsWnd::OnDownloadsViewReviews()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected && ( pDownload->GetReviewCount() >= 0 ) )
		{
			// Make sure data is locked while initializing the dialog
			CDownloadReviewDlg dlg( NULL, pDownload );
			pLock.Unlock();

			dlg.DoModal();
			return;
		}
	}
}

void CDownloadsWnd::OnUpdateDownloadsRemotePreview(CCmdUI* pCmdUI)
{
	int nSelected = 0;
	CDownload* pDownload = NULL;

	for ( POSITION pos = Downloads.GetIterator() ; pos && nSelected < 3 ; )
	{
		pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected )
			nSelected++;
	}

	if ( nSelected == 1 && !pDownload->IsTasking() )
	{
		Prepare();
	}
	else
		m_bSelRemotePreviewCapable = FALSE;

	pCmdUI->Enable( m_bSelRemotePreviewCapable );
}

void CDownloadsWnd::OnDownloadsRemotePreview()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( !pDownload->m_bSelected )
			continue;

		// Check if the saved preview file is available first
		if ( pDownload->m_bGotPreview )
		{
			// OnTimer event will launch it
			pDownload->m_bWaitingPreview = TRUE;
			break;
		}

		// Don't do anything if we are already working on the file
		if ( pDownload->IsTasking() )
			break;

		// Find first client which supports previews
		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			if ( !pSource->m_bPreviewRequestSent && pSource->IsOnline() )
			{
				if ( pSource->m_nProtocol == PROTOCOL_ED2K )
				{
					// m_pTransfer is checked for validity by pSource->IsOnline()
					CDownloadTransferED2K* pEDTransfer = static_cast< CDownloadTransferED2K* >( pSource->m_pTransfer );
					if ( pEDTransfer->m_pClient->m_bEmPreview )
					{
						pEDTransfer->m_pClient->SendPreviewRequest( pDownload );
						// Don't block sending requests for ed2k sources
						// They are not run in threads, so no harm.
						// pDownload->m_bWaitingPreview = TRUE;
						pSource->m_bPreviewRequestSent = TRUE;
						break;
					}
				}
				else if ( pSource->m_nProtocol == PROTOCOL_HTTP && pSource->m_bPreview && pDownload->m_oSHA1 )
				{
					if (  pSource->m_sPreview.IsEmpty() )
					{
						pSource->m_sPreview.Format( _T("http://%s:%i/gnutella/preview/v1?%s"),
							(LPCTSTR)CString( inet_ntoa( pSource->m_pAddress ) ), pSource->m_nPort,
							(LPCTSTR)pDownload->m_oSHA1.toUrn() );
					}
					pDownload->SetTask( new CDownloadTask( pDownload,
						CDownloadTask::dtaskPreviewRequest, pSource->m_sPreview ) );
					pDownload->m_bWaitingPreview = TRUE;
					pSource->m_bPreviewRequestSent = TRUE;
					break;
				}
			}
		}
		// Only one download can get selected at a time for preview
		// so there's no need to check the rest
		break;
	}
}

void CDownloadsWnd::OnUpdateDownloadsLaunch(CCmdUI* pCmdUI)
{
	Prepare();

	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( ! m_bSelCompletedAndNoPreview && ( m_bSelStartedAndNotMoving || ! m_bSelCompleted ) );
	pCmdUI->Enable( ! m_bSelCompletedAndNoPreview && ( m_bSelCompleted || m_bSelStartedAndNotMoving ) );
}

void CDownloadsWnd::OnDownloadsLaunch()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CDownload*> pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected ) pList.AddTail( pDownload );
	}

	while ( ! pList.IsEmpty() )
	{
		CDownload* pDownload = pList.RemoveHead();

		if ( Downloads.Check( pDownload ) )
			if ( ! pDownload->Launch( -1, &pLock, TRUE ) )
				break;
	}
}

void CDownloadsWnd::OnUpdateDownloadsLaunchComplete(CCmdUI* pCmdUI)
{
	Prepare();
	if ( CCoolBarItem* pItem = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pItem->Show( m_bSelCompleted || ! m_bSelDownload || ( m_bSelDownload && ! m_bSelNotMoving ) );
	pCmdUI->Enable( m_bSelCompleted );
}

void CDownloadsWnd::OnDownloadsLaunchComplete()
{
	OnDownloadsLaunchCopy();
}

void CDownloadsWnd::OnUpdateDownloadsLaunchCopy(CCmdUI* pCmdUI)
{
	Prepare();
	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( ! m_bSelCompleted && m_bSelNotMoving );
	pCmdUI->Enable( m_bSelStartedAndNotMoving );
}

void CDownloadsWnd::OnDownloadsLaunchCopy()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CDownload*> pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected ) pList.AddTail( pDownload );
	}

	while ( ! pList.IsEmpty() )
	{
		CDownload* pDownload = pList.RemoveHead();

		if ( Downloads.Check( pDownload ) )
			if ( ! pDownload->Launch( -1, &pLock, FALSE ) )
				break;
	}
}

void CDownloadsWnd::OnUpdateDownloadsEnqueue(CCmdUI* pCmdUI)
{
	Prepare();
	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( m_bSelStartedAndNotMoving || ! m_bSelCompleted );
	pCmdUI->Enable( m_bSelCompleted || m_bSelStartedAndNotMoving );
}

void CDownloadsWnd::OnDownloadsEnqueue()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CDownload*> pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected ) pList.AddTail( pDownload );
	}

	while ( ! pList.IsEmpty() )
	{
		CDownload* pDownload = pList.RemoveHead();

		if ( Downloads.Check( pDownload ) )
		{
			if ( ! pDownload->Enqueue( -1, &pLock ) )
				break;
		}
	}
}

void CDownloadsWnd::OnUpdateDownloadsSources(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelMoreSourcesOK );
}

void CDownloadsWnd::OnDownloadsSources()
{
	int nCount = 0;
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload->m_bSelected )
		{
			if ( ! pDownload->IsMoving() )
			{
				nCount++;						// Simultaneous FMS operations count
				m_nMoreSourcesLimiter--;		// Overall (Network use) check.
				if ( m_nMoreSourcesLimiter >= 0 )
				{
					pDownload->FindMoreSources();
				}
				else
				{
					// Warn user
					theApp.Message( MSG_DEBUG, _T("Find more sources unable to start due to excessive network traffic") );
					// Prevent ed2k bans, client drops, etc.
					m_tMoreSourcesTimer = GetTickCount();
					if ( m_nMoreSourcesLimiter < -30 ) m_nMoreSourcesLimiter = -30;
				}
			}
		}

		// Also only allow 3 FMS operations at once to avoid being blacklisted
		if ( nCount >=3 ) break;
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsAddSource(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelNotMoving );
}

void CDownloadsWnd::OnDownloadsAddSource()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CDownload*> pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected ) pList.AddTail( pDownload );
	}

	while ( ! pList.IsEmpty() )
	{
		CDownload* pDownload = pList.RemoveHead();

		if ( Downloads.Check( pDownload ) )
		{
			if ( ! pDownload->IsMoving() )
			{
				CDownloadDlg dlg( NULL, pDownload );

				pLock.Unlock();
				if ( dlg.DoModal() != IDOK ) break;
				pLock.Lock();

				if ( ! Downloads.Check( pDownload ) || pDownload->IsMoving() ) continue;

				for ( POSITION pos = dlg.m_pURLs.GetHeadPosition(); pos; )
				{
					pDownload->AddSourceURL( dlg.m_pURLs.GetNext( pos ), FALSE );
				}
			}
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsBoost(CCmdUI* pCmdUI)
{
	if ( Settings.Bandwidth.Downloads == 0 )
	{
		pCmdUI->Enable( FALSE );
		return;
	}

	Prepare();
	pCmdUI->Enable( m_bSelBoostable );
}

void CDownloadsWnd::OnDownloadsBoost()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload->m_bSelected )
		{
			if ( pDownload->HasActiveTransfers() && ! pDownload->IsBoosted() )
			{
				pDownload->Boost();
			}
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsURI(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelSHA1orTTHorED2KorName || m_bSelSource );
}

void CDownloadsWnd::OnDownloadsURI()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CPeerProjectFile*> pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected && Downloads.Check( pDownload ) && ( pDownload->m_oSHA1 || pDownload->m_oTiger || pDownload->m_oED2K || pDownload->m_oBTH || pDownload->m_oMD5 || pDownload->m_sName.GetLength() ) )
		{
			CPeerProjectFile* pFile = new CPeerProjectFile();
			pFile->m_sName		= pDownload->m_sName;
			pFile->m_oSHA1		= pDownload->m_oSHA1;
			pFile->m_oTiger		= pDownload->m_oTiger;
			pFile->m_oED2K		= pDownload->m_oED2K;
			pFile->m_oBTH		= pDownload->m_oBTH;
			pFile->m_oMD5		= pDownload->m_oMD5;
			pFile->m_nSize		= pDownload->m_nSize;

			pList.AddTail( pFile );
		}

		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			if ( pSource->m_bSelected )
			{
				CPeerProjectFile* pFile = new CPeerProjectFile();
				pFile->m_sName		= pDownload->m_sName;
				pFile->m_oSHA1		= pDownload->m_oSHA1;
				pFile->m_oTiger		= pDownload->m_oTiger;
				pFile->m_oED2K		= pDownload->m_oED2K;
				pFile->m_oBTH		= pDownload->m_oBTH;
				pFile->m_oMD5		= pDownload->m_oMD5;
				pFile->m_nSize		= pDownload->m_nSize;
				pFile->m_sURL		= pSource->m_sURL;

				pList.AddTail( pFile );
			}
		}
	}

	if ( pList.GetCount() == 1 )
	{
		CURLCopyDlg dlg;
		POSITION pos = pList.GetHeadPosition();
		CPeerProjectFile* pFile = pList.GetNext( pos );
		dlg.Add( pFile );
		dlg.DoModal();
	}
	else if ( pList.GetCount() > 1 )
	{
		CURLExportDlg dlg;

		POSITION pos = pList.GetHeadPosition();
		while ( pos )
		{
			CPeerProjectFile* pFile = pList.GetNext( pos );
			dlg.Add( pFile );
		}
		dlg.DoModal();
	}

	for ( POSITION pos = pList.GetHeadPosition() ; pos ; )
	{
		CPeerProjectFile* pFile = pList.GetNext( pos );
		delete pFile;
	}
	pList.RemoveAll();
}

void CDownloadsWnd::OnUpdateDownloadsShare(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelDownload && m_bSelShareConsistent );
	pCmdUI->SetCheck( m_bSelShareState );
}

void CDownloadsWnd::OnDownloadsShare()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload->m_bSelected )
		{
			pDownload->Share( ! pDownload->IsShared() );
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsMonitor(CCmdUI* pCmdUI)
{
	Prepare();
	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( ! m_bSelCompleted );
	pCmdUI->Enable( m_bSelDownload && ! m_bSelCompleted );
}

void CDownloadsWnd::OnDownloadsMonitor()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CDownload*> pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected ) pList.AddTail( pDownload );
	}

	while ( ! pList.IsEmpty() )
	{
		CDownload* pDownload = pList.RemoveHead();

		if ( Downloads.Check( pDownload ) && ! pDownload->IsMoving() )
		{
			pDownload->ShowMonitor( &pLock );
		}
	}
}

void CDownloadsWnd::OnUpdateDownloadsEdit(CCmdUI *pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelNotMoving && m_nSelectedDownloads == 1 );
}

void CDownloadsWnd::OnDownloadsEdit()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		if ( pDownload->m_bSelected && ! pDownload->IsMoving() )
		{
			if ( ! pDownload->IsComplete() || pDownload->IsSeeding() )
			{
				CDownloadSheet dlg( pDownload );
				pLock.Unlock();
				dlg.DoModal();
				break;
			}
			else if ( m_bSelCompleted )
			{
				CFilePropertiesSheet dlg;

				for ( DWORD i = 0; i < pDownload->GetFileCount(); ++i )
				{
					CString strPath	= pDownload->GetPath( i );
					CQuickLock oLibraryLock( Library.m_pSection );
					if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( strPath ) )
						dlg.Add( pFile );
				}

				pLock.Unlock();
				dlg.DoModal( 0 );
				break;
			}
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsMove(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelDownload );
}

void CDownloadsWnd::OnDownloadsMoveUp()
{
	m_wndDownloads.MoveSelected( -1 );
}

void CDownloadsWnd::OnDownloadsMoveDown()
{
	m_wndDownloads.MoveSelected( 1 );
}

void CDownloadsWnd::OnDownloadsMoveTop()
{
	m_wndDownloads.MoveToTop();
}

void CDownloadsWnd::OnDownloadsMoveBottom()
{
	m_wndDownloads.MoveToEnd();
}


void CDownloadsWnd::OnUpdateTransfersConnect(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelIdleSource && m_bConnectOkay );
}

void CDownloadsWnd::OnTransfersConnect()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			if ( pSource->m_bSelected && pSource->m_pTransfer == NULL )
			{
				if ( pSource->m_nProtocol != PROTOCOL_ED2K )
				{
					pSource->m_pDownload->Resume();

					if ( pSource->m_bPushOnly )
						pSource->PushRequest();
					else if ( CDownloadTransfer* pTransfer = pSource->CreateTransfer() )
						pTransfer->Initiate();
				}
			}
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateTransfersDisconnect(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelActiveSource );
}

void CDownloadsWnd::OnTransfersDisconnect()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			if ( pSource->m_bSelected && pSource->m_pTransfer != NULL )
				pSource->m_pTransfer->Close( TRI_TRUE );
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateTransfersForget(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelSource );
}

void CDownloadsWnd::OnTransfersForget()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			if ( pSource->m_bSelected )
				pSource->Remove( TRUE, TRUE );
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateTransfersChat(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelChat && Settings.Community.ChatEnable );
}

void CDownloadsWnd::OnTransfersChat()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			if ( pSource->m_bSelected )
			{
				if ( pSource->m_nProtocol == PROTOCOL_HTTP )						// HTTP chat
					ChatWindows.OpenPrivate( Hashes::Guid(), &pSource->m_pAddress, pSource->m_nPort, pSource->m_bPushOnly, pSource->m_nProtocol, &pSource->m_pServerAddress, pSource->m_nServerPort );
				else if ( pSource->m_bClientExtended && ! pSource->m_bPushOnly )	// Client accepts G2 chat connections
					ChatWindows.OpenPrivate( Hashes::Guid(), &pSource->m_pAddress, pSource->m_nPort, FALSE, PROTOCOL_G2 );
				else if ( pSource->m_nProtocol == PROTOCOL_ED2K )					// ED2K chat
					ChatWindows.OpenPrivate( Hashes::Guid(), &pSource->m_pAddress, pSource->m_nPort, pSource->m_bPushOnly, pSource->m_nProtocol, &pSource->m_pServerAddress, pSource->m_nServerPort );
				else		// Should never be called
					theApp.Message( MSG_ERROR, _T("Error while initiating chat- Unable to select protocol") );
			}
		}
	}
}

void CDownloadsWnd::OnUpdateBrowseLaunch(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelBrowse );
}

void CDownloadsWnd::OnBrowseLaunch()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		for ( POSITION posSource = pDownload->GetIterator(); posSource ; )
		{
			CDownloadSource* pSource = pDownload->GetNext( posSource );

			if ( pSource->m_bSelected )
			{
				if ( pSource->m_nProtocol == PROTOCOL_HTTP || pSource->m_nProtocol == PROTOCOL_ED2K )	// Many HTTP clients support this
					new CBrowseHostWnd( pSource->m_nProtocol, &pSource->m_pAddress, pSource->m_nPort, pSource->m_bPushOnly, pSource->m_oGUID );
				else if ( pSource->m_bClientExtended )			// Over other protocols, you can only contact non-push G2 clients
					new CBrowseHostWnd( pSource->m_nProtocol, &pSource->m_pAddress, pSource->m_nPort, FALSE, Hashes::Guid() );
			}
		}
	}
}

void CDownloadsWnd::OnUpdateDownloadsFolder(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelDownload );
}

void CDownloadsWnd::OnDownloadsFolder()
{
//	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected && ( pDownload->IsCompleted() || pDownload->IsSeeding() ) )
		{
			CString strPath = pDownload->GetPath( 0 );

			if ( pDownload->GetFileCount() == 1 )
			{
				ShellExecute( GetSafeHwnd(), NULL, _T("Explorer.exe"), "/select, " + strPath, NULL, SW_SHOWNORMAL );
			}
			else
			{
				CString strName = pDownload->m_sName;
				if ( strPath.Find( strName ) > 1 )
					strPath = strPath.Left( strPath.Find( strName ) + strName.GetLength() );
				else
					strPath = strPath.Left( strPath.ReverseFind( '\\' ) + 1 );

				if ( PathIsDirectory( strPath ) )
					ShellExecute( GetSafeHwnd(), _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL );
			}
		}
	}
}

void CDownloadsWnd::OnUpdateDownloadsFileDelete(CCmdUI* pCmdUI)
{
	Prepare();
	if ( CCoolBarItem* pcCmdUI = CCoolBarItem::FromCmdUI( pCmdUI ) )
		pcCmdUI->Show( m_bSelCompleted );
	pCmdUI->Enable( m_bSelCompleted );
}

void CDownloadsWnd::OnDownloadsFileDelete()
{
	// Create common library file list of all selected and completed downloads
	CSingleLock pTransfersLock( &Transfers.m_pSection, TRUE );
	CSingleLock pLibraryLock( &Library.m_pSection, TRUE );
	CLibraryList pList;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected && pDownload->IsCompleted() )
		{
			for ( DWORD i = 0; i < pDownload->GetFileCount(); ++i )
			{
				CString strPath	= pDownload->GetPath( i );
				if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( strPath ) )
				{
					pList.AddTail( pFile );
				}
			}
		}
	}

	while ( ! pList.IsEmpty() )
	{
		CLibraryFile* pFile = Library.LookupFile( pList.GetHead() );
		if ( pFile == NULL )
		{
			pList.RemoveHead();
			continue;
		}

		CDeleteFileDlg dlg( this );
		dlg.m_sName	= pFile->m_sName;
		dlg.m_sComments = pFile->m_sComments;
		dlg.m_nRateValue = pFile->m_nRating;
		dlg.m_bAll = pList.GetCount() > 1;

		pLibraryLock.Unlock();
		pTransfersLock.Unlock();

		if ( dlg.DoModal() != IDOK ) break;

		pTransfersLock.Lock();
		pLibraryLock.Lock();

		for ( INT_PTR nProcess = dlg.m_bAll ? pList.GetCount() : 1 ; nProcess > 0 && pList.GetCount() > 0 ; nProcess-- )
		{
			if ( ( pFile = Library.LookupFile( pList.RemoveHead(), FALSE, TRUE ) ) != NULL )
			{
				dlg.Apply( pFile );

				pFile->Delete();	// Also deletes owner download
			}
		}
	}

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsRate(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelCompleted );
}

void CDownloadsWnd::OnDownloadsRate()
{
	CFilePropertiesSheet dlg;

	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected && pDownload->IsCompleted() )
		{
			for ( DWORD i = 0; i < pDownload->GetFileCount(); ++i )
			{
				CString strPath	= pDownload->GetPath( i );
				CQuickLock oLibraryLock( Library.m_pSection );
				if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( strPath ) )
				{
					dlg.Add( pFile );
				}
			}
		}
	}
	pLock.Unlock();

	dlg.DoModal( 2 );

	Update();
}

void CDownloadsWnd::OnUpdateDownloadsShowSources(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( Settings.Downloads.ShowSources );
}

void CDownloadsWnd::OnDownloadsShowSources()
{
	Settings.Downloads.ShowSources = ! Settings.Downloads.ShowSources;
	Update();
}

void CDownloadsWnd::OnDownloadsClearCompleted()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	Downloads.ClearCompleted();
	Update();
}

void CDownloadsWnd::OnDownloadsClearPaused()
{
	CString strMessage;
	LoadString( strMessage, IDS_DOWNLOAD_CONFIRM_CLEAR_PAUSED );

	if ( AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 ) == IDYES )
	{
		Downloads.ClearPaused();
		Update();
	}
}

void CDownloadsWnd::OnUpdateDownloadsAutoClear(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( Settings.Downloads.AutoClear );
}

void CDownloadsWnd::OnDownloadsAutoClear()
{
	Settings.Downloads.AutoClear = ! Settings.Downloads.AutoClear;
	if ( Settings.Downloads.AutoClear ) OnTimer( 4 );
}

void CDownloadsWnd::OnDownloadsFilterMenu()
{
	CMenu* pMenu = Skin.GetMenu( _T("CDownloadsWnd.Filter") );
	m_wndToolBar.ThrowMenu( ID_DOWNLOADS_FILTER_MENU, pMenu, NULL, FALSE, TRUE );
}

void CDownloadsWnd::OnUpdateDownloadsFilterAll(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Downloads.FilterMask & DLF_ALL ) == DLF_ALL );
}

void CDownloadsWnd::OnDownloadsFilterAll()
{
	Settings.Downloads.FilterMask |= DLF_ALL;
	Update();
}

void CDownloadsWnd::OnUpdateDownloadsFilterActive(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Downloads.FilterMask & DLF_ACTIVE ) > 0 );
}

void CDownloadsWnd::OnDownloadsFilterActive()
{
	Settings.Downloads.FilterMask ^= DLF_ACTIVE;
	Update();
}

void CDownloadsWnd::OnUpdateDownloadsFilterPaused(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Downloads.FilterMask & DLF_PAUSED ) > 0 );
}

void CDownloadsWnd::OnDownloadsFilterPaused()
{
	Settings.Downloads.FilterMask ^= DLF_PAUSED;
	Update();
}

void CDownloadsWnd::OnUpdateDownloadsFilterQueued(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Downloads.FilterMask & DLF_QUEUED ) > 0 );
}

void CDownloadsWnd::OnDownloadsFilterQueued()
{
	Settings.Downloads.FilterMask ^= DLF_QUEUED;
	Update();
}

void CDownloadsWnd::OnUpdateDownloadsFilterSources(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Downloads.FilterMask & DLF_SOURCES ) > 0 );
}

void CDownloadsWnd::OnDownloadsFilterSources()
{
	Settings.Downloads.FilterMask ^= DLF_SOURCES;
	Update();
}

void CDownloadsWnd::OnUpdateDownloadsFilterSeeds(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Downloads.FilterMask & DLF_SEED ) > 0 );
}

void CDownloadsWnd::OnDownloadsFilterSeeds()
{
	Settings.Downloads.FilterMask ^= DLF_SEED;
	Update();
}

void CDownloadsWnd::OnDownloadsSettings()
{
	CSettingsManagerDlg::Run( _T("CDownloadsSettingsPage") );
}

void CDownloadsWnd::OnUpdateDownloadGroupShow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( Settings.General.GUIMode != GUI_BASIC );
	pCmdUI->SetCheck( Settings.General.GUIMode != GUI_BASIC && Settings.Downloads.ShowGroups );
}

void CDownloadsWnd::OnDownloadGroupShow()
{
	Settings.Downloads.ShowGroups = ! Settings.Downloads.ShowGroups;

	CRect rc;
	GetClientRect( &rc );
	OnSize( SIZE_RESTORED, rc.right, rc.bottom );

	Update();
}

void CDownloadsWnd::OnDownloadsHelp()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CDownload* pDownload = NULL;

	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		pDownload = Downloads.GetNext( pos );
		if ( pDownload->m_bSelected ) break;
		pDownload = NULL;
	}

	CString strHelp;
	if ( pDownload == NULL )
		strHelp = L"DownloadHelp.Select";
	else if ( pDownload->IsSeeding() )
		strHelp = L"DownloadHelp.Seeding";
	else if ( pDownload->IsMoving() )
		strHelp = pDownload->IsCompleted() ?
			L"DownloadHelp.Completed" : L"DownloadHelp.Moving";
	else if ( pDownload->IsPaused() )
		strHelp = ( pDownload->GetFileError() != ERROR_SUCCESS ) ?
			L"DownloadHelp.DiskFull" : L"DownloadHelp.Paused";
	else if ( pDownload->IsStarted() && pDownload->GetProgress() == 100.0f )
		strHelp = L"DownloadHelp.Verifying";
	else if ( pDownload->IsDownloading() )
		strHelp = L"DownloadHelp.Downloading";
	else if ( ! pDownload->IsTrying() )
		strHelp = L"DownloadHelp.Queued";
	else if ( pDownload->GetSourceCount() > 0 )
		strHelp = L"DownloadHelp.Pending";
	else if ( pDownload->m_nSize == SIZE_UNKNOWN )
		strHelp = L"DownloadHelp.Searching";
	else if ( pDownload->IsTorrent() && pDownload->IsTasking() )
		strHelp = L"DownloadHelp.Creating";
	else if ( pDownload->m_bTorrentTrackerError )
		strHelp = L"DownloadHelp.Tracker";
	else
		strHelp = L"DownloadHelp.Searching";

	pLock.Unlock();
	CHelpDlg::Show( strHelp );
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadsWnd drag and drop

void CDownloadsWnd::DragDownloads(CList< CDownload* >* pList, CImageList* pImage, const CPoint& ptScreen)
{
	if ( m_pDragList )
	{
		CancelDrag();
		return;
	}

	m_pDragList		= pList;
	m_pDragImage	= pImage;

	CRect rcClient;
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );

	ClipCursor( &rcClient );
	SetCapture();
	SetFocus();
	UpdateWindow();

	OnSetCursor( NULL, 0, 0 );

	CPoint ptStart( ptScreen );
	ScreenToClient( &ptStart );

	CRect rcList;
	m_wndDownloads.GetWindowRect( &rcList );
	ScreenToClient( &rcList );
	m_pDragOffs = rcList.TopLeft();
	m_pDragOffs.y -= 4;

	m_pDragImage->DragEnter( this, ptStart + m_pDragOffs );
}

void CDownloadsWnd::CancelDrag()
{
	if ( m_pDragList == NULL ) return;

	m_pDragImage->DragLeave( this );
	m_pDragImage->EndDrag();
	delete m_pDragImage;
	m_pDragImage = NULL;

	delete m_pDragList;
	m_pDragList = NULL;

	CPoint point( 0, 0 );
	m_wndTabBar.DropObjects( NULL, point );
	m_wndDownloads.DropObjects( NULL, point );
}

void CDownloadsWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_pDragList != NULL )
	{
		m_pDragImage->DragMove( point + m_pDragOffs );
		ClientToScreen( &point );
		m_wndTabBar.DropShowTarget( m_pDragList, point );
		m_wndDownloads.DropShowTarget( m_pDragList, point );
	}

	CPanelWnd::OnMouseMove( nFlags, point );
}

void CDownloadsWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_pDragList != NULL )
	{
		ClipCursor( NULL );
		ReleaseCapture();

		m_pDragImage->DragLeave( this );
		m_pDragImage->EndDrag();
		delete m_pDragImage;
		m_pDragImage = NULL;

		ClientToScreen( &point );
		m_wndTabBar.DropObjects( m_pDragList, point );
		m_wndDownloads.DropObjects( m_pDragList, point );

		delete m_pDragList;
		m_pDragList = NULL;

		Update();
	}

	CPanelWnd::OnLButtonUp( nFlags, point );
}

void CDownloadsWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	if ( m_pDragList != NULL )
	{
		CancelDrag();
		return;
	}

	CPanelWnd::OnRButtonDown( nFlags, point );
}

void CDownloadsWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( nChar == VK_ESCAPE )
		CancelDrag();

	if ( m_pDragList != NULL )
		OnSetCursor( NULL, 0, 0 );

	CPanelWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}

void CDownloadsWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( m_pDragList )
		OnSetCursor( NULL, 0, 0 );

	CPanelWnd::OnKeyUp( nChar, nRepCnt, nFlags );
}

BOOL CDownloadsWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( m_pDragList != NULL )
	{
		if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			SetCursor( m_hCursCopy );
		else
			SetCursor( m_hCursMove );

		return TRUE;
	}

	return CPanelWnd::OnSetCursor( pWnd, nHitTest, message );
}

void CDownloadsWnd::OnCaptureChanged(CWnd *pWnd)
{
	CPanelWnd::OnCaptureChanged(pWnd);
	m_bMouseCaptured = true;
	Update();
}
