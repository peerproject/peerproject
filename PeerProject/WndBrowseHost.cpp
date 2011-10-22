//
// WndBrowseHost.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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
#include "WndBrowseHost.h"
#include "HostBrowser.h"
#include "MatchObjects.h"
#include "GProfile.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "RichDocument.h"
#include "DlgHitColumns.h"
#include "ChatWindows.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CBrowseHostWnd, CBaseMatchWnd)

BEGIN_MESSAGE_MAP(CBrowseHostWnd, CBaseMatchWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_NCLBUTTONUP()
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_MATCHES, OnSelChangeMatches)
	ON_UPDATE_COMMAND_UI(ID_BROWSE_STOP, OnUpdateBrowseHostStop)
	ON_COMMAND(ID_BROWSE_STOP, OnBrowseHostStop)
	ON_COMMAND(ID_BROWSE_REFRESH, OnBrowseHostRefresh)
	ON_UPDATE_COMMAND_UI(ID_BROWSE_PROFILE, OnUpdateBrowseProfile)
	ON_COMMAND(ID_BROWSE_PROFILE, OnBrowseProfile)
	ON_UPDATE_COMMAND_UI(ID_BROWSE_FILES, OnUpdateBrowseFiles)
	ON_COMMAND(ID_BROWSE_FILES, OnBrowseFiles)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_CHAT, OnUpdateSearchChat)
	ON_COMMAND(ID_SEARCH_CHAT, OnSearchChat)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBrowseHostWnd construction

CBrowseHostWnd::CBrowseHostWnd(PROTOCOLID nProtocol, SOCKADDR_IN* pAddress, const Hashes::Guid& oClientID)
	: m_pBrowser	( NULL )
	, m_bOnFiles	( FALSE )
	, m_bAutoBrowse ( TRUE )
{
	m_pBrowser = new CHostBrowser( this, nProtocol, &pAddress->sin_addr, htons( pAddress->sin_port ), FALSE, oClientID );
	Create( IDR_BROWSEHOSTFRAME );
}

CBrowseHostWnd::CBrowseHostWnd(PROTOCOLID nProtocol, IN_ADDR* pAddress, WORD nPort, BOOL bMustPush, const Hashes::Guid& oClientID)
	: m_pBrowser	( NULL )
	, m_bOnFiles	( FALSE )
	, m_bAutoBrowse ( pAddress != NULL )
{
	m_pBrowser = new CHostBrowser( this, nProtocol, pAddress, nPort, bMustPush, oClientID );
	Create( IDR_BROWSEHOSTFRAME );
}

CBrowseHostWnd::~CBrowseHostWnd()
{
	if ( m_pBrowser ) delete m_pBrowser;
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseHostWnd message handlers

int CBrowseHostWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CBaseMatchWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	if ( CSchemaPtr pSchema = SchemaCache.Get( Settings.Search.BlankSchemaURI ) )
	{
		CList< CSchemaMember* > pColumns;
		CSchemaColumnsDlg::LoadColumns( pSchema, &pColumns );
		m_wndList.SelectSchema( pSchema, &pColumns );
	}

	m_wndHeader.Create( this );
	m_wndProfile.Create( this );
	m_wndFrame.Create( this, &m_wndList );

	LoadState( _T("CBrowseHostWnd"), TRUE );

	if ( m_bAutoBrowse )
		m_bPaused = ! m_pBrowser->Browse();

	OnSkinChange();

	return 0;
}

void CBrowseHostWnd::OnDestroy()
{
	m_pBrowser->Stop();

	delete m_pBrowser;
	m_pBrowser = NULL;

	if ( m_wndList.m_pSchema != NULL )
		Settings.Search.BlankSchemaURI = m_wndList.m_pSchema->GetURI();
	else
		Settings.Search.BlankSchemaURI.Empty();

	SaveState( _T("CBrowseHostWnd") );

	CBaseMatchWnd::OnDestroy();
}

void CBrowseHostWnd::OnSkinChange()
{
	OnSize( 0, 0, 0 );
	CBaseMatchWnd::OnSkinChange();

	Skin.CreateToolBar( _T("CBrowseHostWnd"), &m_wndToolBar );
	m_wndFrame.OnSkinChange();
	m_wndHeader.OnSkinChange();
	m_wndProfile.OnSkinChange();

	UpdateMessages();
}

BOOL CBrowseHostWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_wndHeader.m_hWnd != NULL )
	{
		if ( m_wndHeader.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}

	if ( m_wndProfile.m_hWnd != NULL )
	{
		if ( m_wndProfile.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}

	if ( m_wndFrame.m_hWnd != NULL )
	{
		if ( m_wndFrame.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}

	return CBaseMatchWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CBrowseHostWnd::OnSize(UINT nType, int cx, int cy)
{
	CPanelWnd::OnSize( nType, cx, cy );

	if ( ! m_wndHeader ) return;	// Initial Load Debug Assert Workaround

	CRect rc;
	GetClientRect( &rc );

	rc.top += Skin.m_nHeaderbarHeight;
	rc.bottom -= Skin.m_nToolbarHeight;

	m_wndHeader.SetWindowPos( NULL, rc.left, 0, rc.Width(), rc.top, SWP_NOZORDER );
	m_wndToolBar.SetWindowPos( NULL, rc.left, rc.bottom, rc.Width(), Skin.m_nToolbarHeight, SWP_NOZORDER );
	m_wndProfile.SetWindowPos( NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );
	m_wndFrame.SetWindowPos( NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );
}

void CBrowseHostWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if ( point.x == -1 && point.y == -1 ) 	// Keyboard fix
		ClientToScreen( &point );

	if ( m_bContextMenu )
		Skin.TrackPopupMenu( _T("CBrowseHostWnd"), point, ID_SEARCH_DOWNLOAD );
	else
		CBaseMatchWnd::OnContextMenu( pWnd, point );
}

void CBrowseHostWnd::OnUpdateBrowseProfile(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_wndProfile.IsWindowVisible() );
}

void CBrowseHostWnd::OnBrowseProfile()
{
	m_wndProfile.SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
	m_wndFrame.ShowWindow( SW_HIDE );
	m_wndProfile.Update( m_pBrowser );
}

void CBrowseHostWnd::OnUpdateBrowseFiles(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_wndList.IsWindowVisible() );
}

void CBrowseHostWnd::OnBrowseFiles()
{
	m_wndFrame.SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
	m_wndProfile.ShowWindow( SW_HIDE );
	m_bOnFiles = TRUE;
}

void CBrowseHostWnd::OnUpdateBrowseHostStop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_pBrowser->IsBrowsing() );
}

void CBrowseHostWnd::OnBrowseHostStop()
{
	m_pBrowser->Stop();
	m_bPaused = TRUE;
}

void CBrowseHostWnd::OnBrowseHostRefresh()
{
	m_pBrowser->Stop();
	m_bPaused = TRUE;

	if ( m_pBrowser->Browse() )
	{
		m_wndList.DestructiveUpdate();
		m_pMatches->Clear();

		m_bPaused = FALSE;
		m_bUpdate = TRUE;
		PostMessage( WM_TIMER, 2 );
	}
}

void CBrowseHostWnd::OnUpdateSearchChat(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_pBrowser && m_pBrowser->m_bCanChat );
}

void CBrowseHostWnd::OnSearchChat()
{
	if ( m_pBrowser != NULL )
	{
		ChatWindows.OpenPrivate( m_pBrowser->m_oClientID,
			&m_pBrowser->m_pAddress, m_pBrowser->m_nPort, m_pBrowser->m_bMustPush, m_pBrowser->m_nProtocol );
	}
}

void CBrowseHostWnd::OnSelChangeMatches()
{
	m_wndFrame.OnSelChangeMatches();
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseHostWnd event handlers

void CBrowseHostWnd::UpdateMessages(BOOL /*bActive*/)
{
	CString strCaption, strOld;

	m_wndHeader.Update( m_pBrowser );
	m_wndProfile.Update( m_pBrowser );

	LoadString( strCaption, IDR_BROWSEHOSTFRAME );
	if ( Settings.General.LanguageRTL )
		strCaption = _T("\x200F") + strCaption + _T("\x202E : \x202B");
	else
		strCaption += _T(" : ");

	if ( m_pBrowser->m_pProfile != NULL && m_pBrowser->m_pProfile->IsValid() )
	{
		strCaption += m_pBrowser->m_pProfile->GetNick();
		strOld.Format( _T(" (%s:%lu)"),
			(LPCTSTR)CString( inet_ntoa( m_pBrowser->m_pAddress ) ),
			m_pBrowser->m_nPort );
		if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
		strCaption += strOld;
	}
	else
	{
		strOld.Format( _T("%s:%lu"),
			(LPCTSTR)CString( inet_ntoa( m_pBrowser->m_pAddress ) ),
			m_pBrowser->m_nPort );
		strCaption += strOld;
	}

	if ( m_pMatches->m_nFilteredFiles || m_pMatches->m_nFilteredHits )
	{
		strOld.Format( _T(" [%lu/%lu]"),
			m_pMatches->m_nFilteredFiles, m_pMatches->m_nFilteredHits );
		if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
		strCaption += strOld;
	}

	if ( ! m_pBrowser->m_sServer.IsEmpty() )
	{
		if ( Settings.General.LanguageRTL ) strCaption += _T("\x200F");
		strCaption = strCaption + _T(" - ") + m_pBrowser->m_sServer;
	}

	GetWindowText( strOld );

	if ( strOld != strCaption ) SetWindowText( strCaption );

	if ( m_pMatches->m_nFilteredFiles == 0 )
	{
		if ( m_pMatches->m_nFiles > 0 )
		{
			m_wndList.SetMessage( IDS_SEARCH_FILTERED, FALSE );
			return;
		}

		switch ( m_pBrowser->m_nState )
		{
		case CHostBrowser::hbsNull:
			m_wndList.SetMessage( m_pBrowser->m_bConnect ? IDS_BROWSE_NOT_SUPPORTED : IDS_BROWSE_CANT_CONNECT );
			break;
		case CHostBrowser::hbsConnecting:
			m_wndList.SetMessage( m_pBrowser->m_tPushed ? IDS_BROWSE_PUSHED : IDS_BROWSE_CONNECTING );
			break;
		case CHostBrowser::hbsRequesting:
		case CHostBrowser::hbsHeaders:
			m_wndList.SetMessage( IDS_BROWSE_REQUESTING );
			break;
		case CHostBrowser::hbsContent:
			LoadString( strOld, IDS_BROWSE_DOWNLOADING );
			strCaption.Format( strOld, m_pBrowser->GetProgress() * 100.0f );
			m_wndList.SetMessage( strCaption );
			break;
		}
	}
}

void CBrowseHostWnd::OnProfileReceived()
{
	if ( ! m_bPaused && m_hWnd != NULL && m_wndProfile.m_hWnd != NULL)
	{
		if ( m_bOnFiles == FALSE || m_wndProfile.IsWindowVisible() )
			PostMessage( WM_COMMAND, ID_BROWSE_PROFILE );

		m_bUpdate = TRUE;
	}
}

BOOL CBrowseHostWnd::OnQueryHits(const CQueryHit* pHits)
{
	if ( m_bPaused || m_hWnd == NULL ) return FALSE;

	CSingleLock pLock( &m_pMatches->m_pSection );

	if ( pLock.Lock( 100 ) && ! m_bPaused )
	{
		m_pMatches->AddHits( pHits );
		m_bUpdate = TRUE;

		SetModified();
		return TRUE;
	}

	return FALSE;
}

void CBrowseHostWnd::OnHeadPacket(CG2Packet* pPacket)
{
	if ( m_bPaused || m_hWnd == NULL ) return;

	m_wndProfile.OnHeadPacket( pPacket );

	SetModified();
}

void CBrowseHostWnd::OnPhysicalTree(CG2Packet* pPacket)
{
	if ( m_bPaused || m_hWnd == NULL ) return;

	m_wndFrame.OnPhysicalTree( pPacket );

	SetModified();
}

void CBrowseHostWnd::OnVirtualTree(CG2Packet* pPacket)
{
	if ( m_bPaused || m_hWnd == NULL ) return;

	m_wndFrame.OnVirtualTree( pPacket );

	SetModified();
}

BOOL CBrowseHostWnd::OnPush(const Hashes::Guid& oClientID, CConnection* pConnection)
{
	return m_pBrowser != NULL && m_pBrowser->OnPush( oClientID, pConnection );
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseHostWnd serialize

void CBrowseHostWnd::Serialize(CArchive& ar)
{
	int nVersion = 1;

	if ( ar.IsStoring() )
	{
		ar << nVersion;

		ar << m_bOnFiles;
	}
	else // Loading
	{
		ar >> nVersion;
		if ( nVersion != 1 ) AfxThrowUserException();

		ar >> m_bOnFiles;
	}

	CBaseMatchWnd::Serialize(ar);

	m_pBrowser->Serialize( ar );
	m_wndProfile.Serialize( ar );
	m_wndFrame.Serialize( ar );

	if ( ar.IsLoading() )
	{
		m_wndProfile.Update( m_pBrowser );

		PostMessage( WM_TIMER, 1 );
		SendMessage( WM_TIMER, 2 );
		SetAlert( FALSE );
	}
}
