//
// WndUploads.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "Transfers.h"
#include "Uploads.h"
#include "UploadQueues.h"
#include "UploadQueue.h"
#include "UploadFiles.h"
#include "UploadFile.h"
#include "UploadTransfer.h"
#include "UploadTransferED2K.h"
#include "EDClient.h"
#include "Library.h"
#include "FileExecutor.h"
#include "Security.h"

#include "Skin.h"
#include "ChatWindows.h"
#include "WindowManager.h"
#include "WndDownloads.h"
#include "WndUploads.h"
#include "WndMain.h"
#include "WndLibrary.h"
#include "WndBrowseHost.h"
#include "DlgSettingsManager.h"
#include "DlgQueueProperties.h"

#include "DlgHelp.h"
#include "LibraryDictionary.h"

//#include <IO.h>	// For torrent filepath validaton

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_SERIAL(CUploadsWnd, CPanelWnd, 0)

BEGIN_MESSAGE_MAP(CUploadsWnd, CPanelWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_MDIACTIVATE()
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_DISCONNECT, OnUpdateUploadsDisconnect)
	ON_COMMAND(ID_UPLOADS_DISCONNECT, OnUploadsDisconnect)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_LAUNCH, OnUpdateUploadsLaunch)
	ON_COMMAND(ID_UPLOADS_LAUNCH, OnUploadsLaunch)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_FOLDER, OnUpdateUploadsFolder)
	ON_COMMAND(ID_UPLOADS_FOLDER, OnUploadsFolder)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_CLEAR, OnUpdateUploadsClear)
	ON_COMMAND(ID_UPLOADS_CLEAR, OnUploadsClear)
	ON_COMMAND(ID_UPLOADS_CLEAR_COMPLETED, OnUploadsClearCompleted)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_CHAT, OnUpdateUploadsChat)
	ON_COMMAND(ID_UPLOADS_CHAT, OnUploadsChat)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_AUTO_CLEAR, OnUpdateUploadsAutoClear)
	ON_COMMAND(ID_UPLOADS_AUTO_CLEAR, OnUploadsAutoClear)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_BAN, OnUpdateSecurityBan)
	ON_COMMAND(ID_SECURITY_BAN, OnSecurityBan)
	ON_UPDATE_COMMAND_UI(ID_BROWSE_LAUNCH, OnUpdateBrowseLaunch)
	ON_COMMAND(ID_BROWSE_LAUNCH, OnBrowseLaunch)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_START, OnUpdateUploadsStart)
	ON_COMMAND(ID_UPLOADS_START, OnUploadsStart)
	ON_COMMAND(ID_UPLOADS_HELP, OnUploadsHelp)
	ON_COMMAND(ID_UPLOADS_SETTINGS, OnUploadsSettings)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_FILTER_ALL, OnUpdateUploadsFilterAll)
	ON_COMMAND(ID_UPLOADS_FILTER_ALL, OnUploadsFilterAll)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_FILTER_ACTIVE, OnUpdateUploadsFilterActive)
	ON_COMMAND(ID_UPLOADS_FILTER_ACTIVE, OnUploadsFilterActive)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_FILTER_QUEUED, OnUpdateUploadsFilterQueued)
	ON_COMMAND(ID_UPLOADS_FILTER_QUEUED, OnUploadsFilterQueued)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_FILTER_HISTORY, OnUpdateUploadsFilterHistory)
	ON_COMMAND(ID_UPLOADS_FILTER_HISTORY, OnUploadsFilterHistory)
	ON_COMMAND(ID_UPLOADS_FILTER_MENU, OnUploadsFilterMenu)
	ON_UPDATE_COMMAND_UI(ID_UPLOADS_FILTER_TORRENT, OnUpdateUploadsFilterTorrent)
	ON_COMMAND(ID_UPLOADS_FILTER_TORRENT, OnUploadsFilterTorrent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUploadsWnd construction

CUploadsWnd::CUploadsWnd() : CPanelWnd( Settings.General.GUIMode == GUI_TABBED, TRUE )
{
	Create( IDR_UPLOADSFRAME );
}

CUploadsWnd::~CUploadsWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUploadsWnd message handlers

int CUploadsWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	m_wndUploads.Create( this, IDC_UPLOADS );

	if ( ! theApp.m_bIsWin2000 )
		m_wndUploads.ModifyStyleEx( 0, WS_EX_COMPOSITED );	// Stop flicker XP+, CPU intensive!

	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_BORDER_TOP );
	m_wndToolBar.SetSyncObject( &Transfers.m_pSection );

	LoadState( NULL, TRUE );

	SetTimer( 2, Settings.Interface.RefreshRateText, NULL );
	PostMessage( WM_TIMER, 2 );

	SetTimer( 4, 5000, NULL );
	PostMessage( WM_TIMER, 4 );

	m_tSel			= 0;
	m_tLastUpdate	= 0;

	return 0;
}

void CUploadsWnd::OnDestroy()
{
	KillTimer( 4 );
	SaveState();
	CPanelWnd::OnDestroy();
}

BOOL CUploadsWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_wndToolBar.m_hWnd )
	{
		if ( m_wndToolBar.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}

	return CPanelWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CUploadsWnd::OnSize(UINT nType, int cx, int cy)
{
	SizeListAndBar( &m_wndUploads, &m_wndToolBar );
	CPanelWnd::OnSize( nType, cx, cy );
}

void CUploadsWnd::OnSkinChange()
{
	OnSize( 0, 0, 0 );
	CPanelWnd::OnSkinChange();
	Skin.Translate( _T("CUploadCtrl"), &m_wndUploads.m_wndHeader);
	Skin.CreateToolBar( _T("CUploadsWnd"), &m_wndToolBar );
	m_wndUploads.OnSkinChange();
}

void CUploadsWnd::OnTimer(UINT_PTR nIDEvent)
{
	// Reset Selection Timer event (posted by ctrluploads)
	if ( nIDEvent == 5 ) m_tSel	= 0;

	// Clear event (5 second timer)
	if ( nIDEvent == 4 )
	{
		CSingleLock pLock( &Transfers.m_pSection );
		if ( ! pLock.Lock( 10 ) ) return;

		DWORD tNow = GetTickCount();
		BOOL bCull = Uploads.GetCount( NULL ) > 75;

		for ( POSITION pos = Uploads.GetIterator() ; pos ; )
		{
			CUploadTransfer* pUpload = Uploads.GetNext( pos );

			if ( pUpload->m_nState == upsNull &&
				 tNow - pUpload->m_tConnected > Settings.Uploads.ClearDelay )
			{
				if ( Settings.Uploads.AutoClear || pUpload->m_nUploaded == 0 || bCull )
					pUpload->Remove( FALSE );
			}
		}
	}

	// Update event (2 second timer)
	if ( nIDEvent == 2 )
	{
		DWORD tNow = GetTickCount();

		// If the window is visible or hasn't been updated in 10 seconds
		if ( ( IsWindowVisible() && IsActive( FALSE ) ) || ( ( tNow - m_tLastUpdate ) > 10*1000 ) )
		{
			m_wndUploads.Update();
			m_tLastUpdate = tNow;
		}
	}
}

void CUploadsWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if ( point.x == -1 && point.y == -1 ) 	// Keyboard fix
	{
		m_wndUploads.ClientToScreen( &point );
		Skin.TrackPopupMenu( _T("CUploadsWnd.Default"), point, ID_UPLOADS_HELP );
		return;
	}

	CPoint ptLocal( point );
	m_wndUploads.ScreenToClient( &ptLocal );
	m_tSel = 0;

	CUploadFile* pUpload;

	if ( m_wndUploads.HitTest( ptLocal, NULL , &pUpload, NULL, NULL ) && pUpload != NULL )
		Skin.TrackPopupMenu( _T("CUploadsWnd.Upload"), point, ID_UPLOADS_LAUNCH );
	else
		Skin.TrackPopupMenu( _T("CUploadsWnd.Default"), point, ID_UPLOADS_HELP );
}

void CUploadsWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CPanelWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
	if ( bActivate )
		m_wndUploads.SetFocus();
}

BOOL CUploadsWnd::IsSelected(CUploadFile* pFile)
{
	if ( ! pFile->m_bSelected ) return FALSE;

	if ( CUploadTransfer* pTransfer = pFile->GetActive() )
	{
		if ( pTransfer->m_nProtocol == PROTOCOL_BT )
		{
			if ( 0 == ( Settings.Uploads.FilterMask & ULF_TORRENT ) ) return FALSE;
		}
		else if ( pTransfer->m_pQueue != NULL )
		{
			if ( pTransfer->m_pQueue->m_bExpanded == FALSE ) return FALSE;

			if ( pTransfer->m_pQueue->IsActive( pTransfer ) )
			{
				if ( 0 == ( Settings.Uploads.FilterMask & ULF_ACTIVE ) ) return FALSE;
			}
			else
			{
				if ( 0 == ( Settings.Uploads.FilterMask & ULF_QUEUED ) ) return FALSE;
			}
		}
		else
		{
			if ( 0 == ( Settings.Uploads.FilterMask & ULF_HISTORY ) ) return FALSE;
		}
	}
	else
	{
		if ( 0 == ( Settings.Uploads.FilterMask & ULF_HISTORY ) ) return FALSE;
	}

	return TRUE;
}

void CUploadsWnd::Prepare()
{
	if ( GetTickCount() - m_tSel < 250 )
		return;

	CSingleLock pLock( &Transfers.m_pSection, FALSE );
	if ( ! pLock.Lock( 250 ) )
		return;

	m_tSel = GetTickCount();
	m_bSelFile = m_bSelUpload = FALSE;
	m_bSelActive = m_bSelQueued = FALSE;
	m_bSelChat = m_bSelBrowse = FALSE;
	m_bSelSourceAcceptConnections = FALSE;
	m_bSelPartial = TRUE;

	for ( POSITION posFile = UploadFiles.GetIterator() ; posFile ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( posFile );

		if ( pFile->m_bSelected && IsSelected( pFile ) )
		{
			m_bSelFile = TRUE;

			if ( CUploadTransfer* pTransfer = pFile->GetActive() )
			{
				m_bSelUpload = TRUE;

				if ( pTransfer->m_bClientExtended )
				{
					m_bSelChat = TRUE;
					m_bSelBrowse = TRUE;
				}
				else if ( pTransfer->m_nProtocol == PROTOCOL_ED2K )
				{
					m_bSelChat = TRUE;
					CUploadTransferED2K* pTransferED2K = static_cast< CUploadTransferED2K* >( pTransfer );
					if ( pTransferED2K->m_pClient && pTransferED2K->m_pClient->m_bEmBrowse )
						m_bSelBrowse = TRUE;
				}

				if ( pTransfer->m_pQueue != NULL )
				{
					if ( pTransfer->m_pQueue->IsActive( pTransfer ) )
						m_bSelActive = TRUE;
					else
						m_bSelQueued = TRUE;
				}
				else if ( pTransfer->m_nState != upsNull )
				{
					m_bSelActive = TRUE;
				}

				if ( m_bSelPartial == TRUE )
				{
					CPeerProjectFile oFile = *pFile;
					CSingleLock pLibraryLock( &Library.m_pSection, TRUE );
					if ( CLibraryFile* pLibFile = LibraryMaps.LookupFileByHash( &oFile, FALSE, TRUE ) )
						m_bSelPartial = FALSE;
					else if ( PathIsDirectory( Settings.Downloads.TorrentPath + _T("\\") + pFile->m_sName ) )	// Try multifile torrent
						m_bSelPartial = FALSE;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CUploadsWnd command handlers

void CUploadsWnd::OnUpdateUploadsDisconnect(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelActive );
}

void CUploadsWnd::OnUploadsDisconnect()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CUploadFile*> pList;

	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( pos );
		if ( IsSelected( pFile ) ) pList.AddTail( pFile );
	}

	while ( ! pList.IsEmpty() )
	{
		CUploadFile* pFile = pList.RemoveHead();

		if ( UploadFiles.Check( pFile ) && pFile->GetActive() != NULL )
		{
			CUploadTransfer* pUpload = pFile->GetActive();

			if ( pUpload->m_nProtocol == PROTOCOL_ED2K && pUpload->m_nState != upsNull )
			{
				CString strFormat, strMessage;
				LoadString( strFormat, IDS_UPLOAD_CANCEL_ED2K );
				strMessage.Format( strFormat, (LPCTSTR)pUpload->m_sName );
				pLock.Unlock();
				INT_PTR nResp = AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2 );
				pLock.Lock();
				if ( nResp == IDCANCEL )
					break;
				if ( nResp != IDYES || ! Uploads.Check( pUpload ) )
					continue;
			}

			pUpload->Close( TRUE );
		}
	}
}

void CUploadsWnd::OnUpdateUploadsStart(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelQueued );
}

void CUploadsWnd::OnUploadsStart()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( pos );

		if ( IsSelected( pFile ) && pFile->GetActive() != NULL )
			pFile->GetActive()->Promote();
	}
}

void CUploadsWnd::OnUpdateUploadsClear(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelFile );
}

void CUploadsWnd::OnUploadsClear()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CUploadFile*> pList;

	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( pos );
		if ( IsSelected( pFile ) )
			pList.AddTail( pFile );
	}

	while ( ! pList.IsEmpty() )
	{
		CUploadFile* pFile = pList.RemoveHead();

		if ( UploadFiles.Check( pFile ) )
		{
			CUploadTransfer* pUpload = pFile->GetActive();

			if ( pUpload != NULL && pUpload->m_nProtocol == PROTOCOL_ED2K && pUpload->m_nState != upsNull )
			{
				CString strFormat, strMessage;
				LoadString( strFormat, IDS_UPLOAD_CANCEL_ED2K );
				strMessage.Format( strFormat, (LPCTSTR)pUpload->m_sName );
				pLock.Unlock();
				INT_PTR nResp = AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2 );
				pLock.Lock();
				if ( nResp == IDCANCEL )
					break;
				if ( nResp != IDYES || ! UploadFiles.Check( pFile ) )
					continue;
			}

			pFile->Remove();
		}
	}
}

void CUploadsWnd::OnUpdateUploadsLaunch(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelFile && ! m_bSelPartial );
}

void CUploadsWnd::OnUploadsLaunch()
{
	CSingleLock pTransfersLock( &Transfers.m_pSection, TRUE );

	// Shift key opens files directly
	//if ( ! ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0 )
	//{
		// Show file in Libray:
		for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
		{
			CUploadFile* pFile = UploadFiles.GetNext( pos );

			if ( IsSelected( pFile ) )
			{
				CPeerProjectFile oFile = *pFile;
				pTransfersLock.Unlock();

				CSingleLock pLibraryLock( &Library.m_pSection, TRUE );
				if ( CLibraryFile* pLibFile = LibraryMaps.LookupFileByHash( &oFile ) )
				{
					if ( CMainWnd* pMainWnd = theApp.SafeMainWnd() )
					{
						if ( CLibraryWnd* pLibrary = (CLibraryWnd*)( pMainWnd->m_pWindows.Open( RUNTIME_CLASS(CLibraryWnd) ) ) )
							pLibrary->Display( pLibFile );
					}
				}
				else if ( PathIsDirectory( Settings.Downloads.TorrentPath + _T("\\") + pFile->m_sName ) )	// Try default multifile torrent folder
				{
					ShellExecute( GetSafeHwnd(), _T("open"),
						Settings.Downloads.TorrentPath + _T("\\") + pFile->m_sName, NULL, NULL, SW_SHOWNORMAL );
				}
			}
		}
	//}
	//else // Alternately launch files
	//{
	//	CList<CUploadFile*> pList;
	//
	//	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	//	{
	//		CUploadFile* pFile = UploadFiles.GetNext( pos );
	//		if ( IsSelected( pFile ) ) pList.AddTail( pFile );
	//	}
	//
	//	while ( ! pList.IsEmpty() )
	//	{
	//		CUploadFile* pFile = pList.RemoveHead();
	//
	//		if ( UploadFiles.Check( pFile ) )
	//		{
	//			CString strPath = pFile->m_sPath;
	//
	//			if ( strPath.Find( pFile->m_sName ) > 3 )	// Not Torrent
	//			{
	//				pTransfersLock.Unlock();
	//				if ( CFileExecutor::Execute( strPath ) )
	//					pTransfersLock.Lock();
	//			}
	//		}
	//	}
	//}
}

void CUploadsWnd::OnUpdateUploadsFolder(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelFile && ! m_bSelPartial );
}

void CUploadsWnd::OnUploadsFolder()
{
	CQuickLock oLock( UploadQueues.m_pSection );
	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( pos );
		if ( IsSelected( pFile ) )
		{
			CString strPath;	// = *pFile->m_sPath;
			CPeerProjectFile oFile = *pFile;
			if ( CLibraryFile* pLibFile = LibraryMaps.LookupFileByHash( &oFile, FALSE, TRUE ) )
				strPath = pLibFile->GetPath();

			if ( strPath.Find( pFile->m_sName ) > 3 )
			{
			//	char charPath[255];
			//	sprintf( charPath, "%S", strPath );	// CString to Char for <IO> filepath validation
			//	if (_access (charPath, 0) == 0)
					ShellExecute( GetSafeHwnd(), NULL, _T("Explorer.exe"), _T("/select, ") + strPath, NULL, SW_SHOWNORMAL );
			}
			else if ( PathIsDirectory( Settings.Downloads.TorrentPath + _T("\\") + pFile->m_sName ) )
			{
				// ToDo: Fix non-default multifile torrents
				ShellExecute( GetSafeHwnd(), _T("open"),
					Settings.Downloads.TorrentPath + _T("\\") + pFile->m_sName, NULL, NULL, SW_SHOWNORMAL );
			}

			//AfxMessageBox( _T("Path: ") + strPath +  _T("\nName: ") + pFile->m_sName );	// TEST
		}
	}
}

void CUploadsWnd::OnUpdateUploadsChat(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelChat && Settings.Community.ChatEnable );
}

void CUploadsWnd::OnUploadsChat()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( pos );

		if ( IsSelected( pFile ) && pFile->GetActive() != NULL )
		{
			if ( pFile->GetActive()->m_nProtocol == PROTOCOL_HTTP )		// HTTP chat. (G2, G1)
				ChatWindows.OpenPrivate( Hashes::Guid(), &pFile->GetActive()->m_pHost, FALSE, PROTOCOL_HTTP );
			else if ( pFile->GetActive()->m_bClientExtended )			// Client accepts G2 chat
				ChatWindows.OpenPrivate( Hashes::Guid(), &pFile->GetActive()->m_pHost, FALSE, PROTOCOL_G2 );
			else if ( pFile->GetActive()->m_nProtocol == PROTOCOL_ED2K )// ED2K chat.
				ChatWindows.OpenPrivate( Hashes::Guid(), &pFile->GetActive()->m_pHost, FALSE, PROTOCOL_ED2K );
			else		// Should never be called
				theApp.Message( MSG_ERROR, _T("Error while initiating chat- Unable to select protocol") );
		}
	}
}

void CUploadsWnd::OnUpdateSecurityBan(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelUpload );
}

void CUploadsWnd::OnSecurityBan()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CUploadFile*> pList;

	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( pos );
		if ( IsSelected( pFile ) ) pList.AddTail( pFile );
	}

	while ( ! pList.IsEmpty() )
	{
		CUploadFile* pFile = pList.RemoveHead();

		if ( UploadFiles.Check( pFile ) && pFile->GetActive() != NULL )
		{
			CUploadTransfer* pUpload = pFile->GetActive();

			IN_ADDR pAddress = pUpload->m_pHost.sin_addr;
			pUpload->Remove( FALSE );
			pLock.Unlock();
			Security.Ban( &pAddress, banSession );
			pLock.Lock();
		}
	}
}

void CUploadsWnd::OnUpdateBrowseLaunch(CCmdUI* pCmdUI)
{
	Prepare();
	pCmdUI->Enable( m_bSelBrowse );
}

void CUploadsWnd::OnBrowseLaunch()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CList<CUploadFile*> pList;

	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( pos );
		if ( IsSelected( pFile ) ) pList.AddTail( pFile );
	}

	while ( ! pList.IsEmpty() )
	{
		CUploadFile* pFile = pList.RemoveHead();

		if ( UploadFiles.Check( pFile ) && pFile->GetActive() != NULL )
		{
			CUploadTransfer* pTransfer = pFile->GetActive();
			PROTOCOLID nProtocol = pTransfer->m_nProtocol;
			SOCKADDR_IN pAddress = pTransfer->m_pHost;
			pLock.Unlock();
			new CBrowseHostWnd( nProtocol, &pAddress );
			pLock.Lock();
		}
	}
}

void CUploadsWnd::OnUploadsClearCompleted()
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	for ( POSITION pos = Uploads.GetIterator() ; pos ; )
	{
		CUploadTransfer* pUpload = Uploads.GetNext( pos );
		if ( pUpload->m_nState == upsNull ) pUpload->Remove( FALSE );
	}

	m_wndUploads.Update();
}

void CUploadsWnd::OnUpdateUploadsAutoClear(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( Settings.Uploads.AutoClear );
}

void CUploadsWnd::OnUploadsAutoClear()
{
	Settings.Uploads.AutoClear = ! Settings.Uploads.AutoClear;
	if ( Settings.Uploads.AutoClear ) OnTimer( 4 );
}

// Removed redundant function:
//void CUploadsWnd::OnEditQueue()

void CUploadsWnd::OnUploadsSettings()
{
	CSettingsManagerDlg::Run( _T("CUploadsSettingsPage") );
}

void CUploadsWnd::OnUploadsHelp()
{
	CHelpDlg::Show( _T("UploadHelp") );
}

BOOL CUploadsWnd::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_TAB )			// Toggle window focus to Downloads
		{
			GetManager()->Open( RUNTIME_CLASS(CDownloadsWnd) );
			return TRUE;
		}
		else if ( pMsg->wParam == VK_DELETE )	// Cancel uploads
		{
			OnUploadsClear();
			return TRUE;
		}
		else if ( pMsg->wParam == VK_ESCAPE )	// Clear selections
		{
			for ( POSITION posFile = UploadFiles.GetIterator() ; posFile ; )
			{
				CUploadFile* pFile = UploadFiles.GetNext( posFile );
				pFile->m_bSelected = FALSE;
			}

			m_wndUploads.Update();
		}
	}

	return CPanelWnd::PreTranslateMessage( pMsg );
}

void CUploadsWnd::OnUploadsFilterMenu()
{
	CMenu* pMenu = Skin.GetMenu( _T("CUploadsWnd.Filter") );
	m_wndToolBar.ThrowMenu( ID_UPLOADS_FILTER_MENU, pMenu, NULL, FALSE, TRUE );
}

void CUploadsWnd::OnUpdateUploadsFilterAll(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Uploads.FilterMask & ULF_ALL ) == ULF_ALL );
}

void CUploadsWnd::OnUploadsFilterAll()
{
	Settings.Uploads.FilterMask |= ULF_ALL;
	m_wndUploads.Update();
}

void CUploadsWnd::OnUpdateUploadsFilterActive(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Uploads.FilterMask & ULF_ACTIVE ) > 0 );
}

void CUploadsWnd::OnUploadsFilterActive()
{
	Settings.Uploads.FilterMask ^= ULF_ACTIVE;
	m_wndUploads.Update();
}

void CUploadsWnd::OnUpdateUploadsFilterQueued(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Uploads.FilterMask & ULF_QUEUED ) > 0 );
}

void CUploadsWnd::OnUploadsFilterQueued()
{
	Settings.Uploads.FilterMask ^= ULF_QUEUED;
	m_wndUploads.Update();
}

void CUploadsWnd::OnUpdateUploadsFilterHistory(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Uploads.FilterMask & ULF_HISTORY ) > 0 );
}

void CUploadsWnd::OnUploadsFilterHistory()
{
	Settings.Uploads.FilterMask ^= ULF_HISTORY;
	m_wndUploads.Update();
}

void CUploadsWnd::OnUpdateUploadsFilterTorrent(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( ( Settings.Uploads.FilterMask & ULF_TORRENT ) > 0 );
}

void CUploadsWnd::OnUploadsFilterTorrent()
{
	Settings.Uploads.FilterMask ^= ULF_TORRENT;
	m_wndUploads.Update();
}
