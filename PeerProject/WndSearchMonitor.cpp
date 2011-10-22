//
// WndSearchMonitor.cpp
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
#include "WndSearchMonitor.h"
#include "WndSearch.h"
#include "WndBrowseHost.h"
#include "QuerySearch.h"
#include "LiveList.h"
#include "Security.h"
#include "Skin.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

// Set Column Order
enum {
	COL_SEARCH,
	COL_URN,
//	COL_SIZE,
	COL_SCHEMA,
//	COL_NETWORK,
	COL_ENDPOINT,
	COL_LAST	// Count
};

IMPLEMENT_SERIAL(CSearchMonitorWnd, CPanelWnd, 0)

BEGIN_MESSAGE_MAP(CSearchMonitorWnd, CPanelWnd)
	//{{AFX_MSG_MAP(CSearchMonitorWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_SEARCHMONITOR_PAUSE, OnUpdateSearchMonitorPause)
	ON_COMMAND(ID_SEARCHMONITOR_PAUSE, OnSearchMonitorPause)
	ON_COMMAND(ID_SEARCHMONITOR_CLEAR, OnSearchMonitorClear)
	ON_UPDATE_COMMAND_UI(ID_HITMONITOR_SEARCH, OnUpdateSearchMonitorSearch)
	ON_COMMAND(ID_HITMONITOR_SEARCH, OnSearchMonitorSearch)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_BAN, OnUpdateSecurityBan)
	ON_COMMAND(ID_SECURITY_BAN, OnSecurityBan)
	ON_UPDATE_COMMAND_UI(ID_BROWSE_LAUNCH, OnUpdateBrowseLaunch)
	ON_COMMAND(ID_BROWSE_LAUNCH, OnBrowseLaunch)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_SEARCHES, OnDblClkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSearchMonitorWnd construction

CSearchMonitorWnd::CSearchMonitorWnd()
{
	Create( IDR_SEARCHMONITORFRAME );
}

CSearchMonitorWnd::~CSearchMonitorWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSearchMonitorWnd message handlers

int CSearchMonitorWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	m_wndList.Create( WS_VISIBLE|LVS_ICON|LVS_AUTOARRANGE|LVS_REPORT|LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_SEARCHES );
	m_wndList.SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_LABELTIP );

	m_pSizer.Attach( &m_wndList );

	VERIFY( m_gdiImageList.Create( 16, 16, ILC_MASK|ILC_COLOR32, 1, 1 ) );
	AddIcon( IDR_SEARCHMONITORFRAME , m_gdiImageList );
	m_wndList.SetImageList( &m_gdiImageList, LVSIL_SMALL );
	m_wndList.SetFont( &theApp.m_gdiFont );

	m_wndList.InsertColumn( COL_SEARCH, _T("Search"), LVCFMT_LEFT, 210, -1 );
	m_wndList.InsertColumn( COL_URN, _T("URN"), LVCFMT_LEFT, 340, 0 );
//	m_wndList.InsertColumn( COL_SIZE, _T("Size"), LVCFMT_LEFT, 100, 1 );
	m_wndList.InsertColumn( COL_SCHEMA, _T("Schema"), LVCFMT_LEFT, 150, 1 );
//	m_wndList.InsertColumn( COL_NETWORK, _T("Network"), LVCFMT_LEFT, 60, 3 );
	m_wndList.InsertColumn( COL_ENDPOINT, _T("Endpoint"), LVCFMT_LEFT, 150, 2 );

	LoadState( _T("CSearchMonitorWnd"), TRUE );

	m_bPaused = FALSE;
	SetTimer( 2, 250, NULL );

	return 0;
}

void CSearchMonitorWnd::OnDestroy()
{
	KillTimer( 2 );

	CSingleLock pLock( &m_pSection, TRUE );
	m_bPaused = TRUE;

	for ( POSITION pos = m_pQueue.GetHeadPosition() ; pos ; )
	{
		delete m_pQueue.GetNext( pos );
	}
	m_pQueue.RemoveAll();

	pLock.Unlock();

	Settings.SaveList( _T("CSearchMonitorWnd"), &m_wndList );
	SaveState( _T("CSearchMonitorWnd") );

	CPanelWnd::OnDestroy();
}

void CSearchMonitorWnd::OnSkinChange()
{
	CPanelWnd::OnSkinChange();
	Settings.LoadList( _T("CSearchMonitorWnd"), &m_wndList );
}

void CSearchMonitorWnd::OnSize(UINT nType, int cx, int cy)
{
	if ( ! m_wndList ) return;

	CPanelWnd::OnSize( nType, cx, cy );
	m_pSizer.Resize( cx );
	m_wndList.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}

void CSearchMonitorWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if ( point.x == -1 && point.y == -1 ) 	// Keyboard fix
		ClientToScreen( &point );

	Skin.TrackPopupMenu( _T("CSearchMonitorWnd"), point, ID_HITMONITOR_SEARCH );
}

void CSearchMonitorWnd::OnUpdateSearchMonitorSearch(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() == 1 );
}

void CSearchMonitorWnd::OnSearchMonitorSearch()
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );

	if ( nItem >= 0 )
	{
		CQuerySearchPtr pSearch = new CQuerySearch();
		pSearch->m_sSearch = m_wndList.GetItemText( nItem, 0 );

		if ( pSearch->m_sSearch.IsEmpty() ||
			 _tcscmp( pSearch->m_sSearch, _T("\\") ) == 0 )
		{
			pSearch->m_sSearch = m_wndList.GetItemText( nItem, 1 );

			if ( _tcsicmp( pSearch->m_sSearch, _T("None") ) != 0 &&
				 _tcsncmp( pSearch->m_sSearch, _T("btih:"), 5 ) != 0 )
				pSearch->m_sSearch = _T("urn:") + m_wndList.GetItemText( nItem, 1 );
			else
				pSearch->m_sSearch.Empty();
		}

		if ( ! pSearch->m_sSearch.IsEmpty() )
			CQuerySearch::OpenWindow( pSearch );
	}
}

void CSearchMonitorWnd::OnUpdateSearchMonitorPause(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_bPaused );
}

void CSearchMonitorWnd::OnSearchMonitorPause()
{
	m_bPaused = ! m_bPaused;
}

void CSearchMonitorWnd::OnSearchMonitorClear()
{
	m_wndList.DeleteAllItems();
}

void CSearchMonitorWnd::OnDblClkList(NMHDR* /*pNotifyStruct*/, LRESULT *pResult)
{
	OnSearchMonitorSearch();
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPanelWnd event handlers

void CSearchMonitorWnd::OnQuerySearch(const CQuerySearch* pSearch)
{
	if ( m_bPaused || m_hWnd == NULL ) return;

	CSingleLock pLock( &m_pSection, TRUE );

	if ( m_bPaused ) return;

	CLiveItem* pItem = new CLiveItem( COL_LAST, NULL );

	CString strSearch = pSearch->m_sSearch;
	CString strSchema, strURN, strNode;

//	LoadString( strSchema, IDS_NEIGHBOUR_COMPRESSION_NONE );	// ToDo: Generic "None" translation ?
//	LoadString( strURN, IDS_NEIGHBOUR_COMPRESSION_NONE );

	CString strSize;
	if ( pSearch->m_nMinSize > 100 )
	{
		strSize = Settings.SmartVolume( pSearch->m_nMinSize );
		if ( pSearch->m_nMaxSize != SIZE_UNKNOWN && ( pSearch->m_nMaxSize - pSearch->m_nMinSize ) < 1024 * 1025 )
			strSize = _T("~ ") + Settings.SmartVolume( pSearch->m_nMaxSize );	// Specific size
		else if ( pSearch->m_nMaxSize != SIZE_UNKNOWN && pSearch->m_nMaxSize > 512 )
			strSize = strSize + _T(" - ") + Settings.SmartVolume( pSearch->m_nMaxSize );
		else
			strSize = _T("> ") + strSize;
	}
	else if ( pSearch->m_nMaxSize != SIZE_UNKNOWN && pSearch->m_nMaxSize > 100 )
	{
		strSize = _T("< ") + Settings.SmartVolume( pSearch->m_nMaxSize );
	}

	CString strNetwork = _T("-");
	if ( pSearch->m_nProtocol > PROTOCOL_NULL )
	{
		if ( pSearch->m_nProtocol == PROTOCOL_G1 )
			strNetwork = _T("  G1");
		else if ( pSearch->m_nProtocol == PROTOCOL_G2 )
			strNetwork = _T("  G2");
		else if ( pSearch->m_nProtocol == PROTOCOL_ED2K )
			strNetwork = _T("  ED2K");
		else
			strNetwork = _T("  ?");	// Others?
	}

	if ( pSearch->m_pEndpoint.sin_addr.s_addr )
		strNode.Format( _T("%hs:%u"),
			inet_ntoa( pSearch->m_pEndpoint.sin_addr ),
			ntohs( pSearch->m_pEndpoint.sin_port ) );

	if ( pSearch->m_oSHA1 && pSearch->m_oTiger )
		strURN	= _T("bitprint:") + pSearch->m_oSHA1.toString() + '.' + pSearch->m_oTiger.toString();
	else if ( pSearch->m_oTiger )
		strURN = pSearch->m_oTiger.toShortUrn();
	else if ( pSearch->m_oSHA1 )
		strURN = pSearch->m_oSHA1.toShortUrn();
	else if ( pSearch->m_oED2K )
		strURN = pSearch->m_oED2K.toShortUrn();
	else if ( pSearch->m_oBTH )
		strURN = pSearch->m_oBTH.toShortUrn();
	else if ( pSearch->m_oMD5 )
		strURN = pSearch->m_oMD5.toShortUrn();
	else
		strURN = _T("-");

	if ( pSearch->m_bWhatsNew )
		strSearch = _T("What's New?");

	if ( pSearch->m_pXML )
	{
		strSearch += _T('«');
		strSearch += pSearch->m_pXML->GetRecursiveWords();
		strSearch += _T('»');
	}

	if ( pSearch->m_pSchema )
		strSchema = pSearch->m_pSchema->m_sTitle;
	else
		strSchema = _T("-");

	// ToDo: Add proper Size and Network columns to HitMonitor instead
	if ( strSize.GetLength() > 1 )
	{
		if ( strSchema.GetLength() > 3 )
			strSchema += _T("  ") + strSize;
		else
			strSchema = strSize;
	}

	if ( strNetwork.GetLength() > 1 )
		strNode += strNetwork;

	pItem->Set( COL_SEARCH, strSearch );
	pItem->Set( COL_URN, strURN );
//	pItem->Set( COL_SIZE, strSize );
	pItem->Set( COL_SCHEMA, strSchema );
//	pItem->Set( COL_NETWORK, strNetwork );
	pItem->Set( COL_ENDPOINT, strNode );

	m_pQueue.AddTail( pItem );
}

void CSearchMonitorWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent != 2 ) return;

	BOOL bScroll = m_wndList.GetTopIndex() + m_wndList.GetCountPerPage() >= m_wndList.GetItemCount();

	CSingleLock pLock( &m_pSection );

	for (;;)
	{
		pLock.Lock();

		if ( m_pQueue.GetCount() == 0 ) break;
		CLiveItem* pItem = m_pQueue.RemoveHead();

		pLock.Unlock();

		if ( (DWORD)m_wndList.GetItemCount() >= Settings.Search.MonitorQueue && Settings.Search.MonitorQueue > 0 )
			m_wndList.DeleteItem( 0 );

		/*int nItem =*/ pItem->Add( &m_wndList, -1, 4 );

		delete pItem;
	}

	if ( bScroll )
		m_wndList.EnsureVisible( m_wndList.GetItemCount() - 1, FALSE );
}

void CSearchMonitorWnd::OnUpdateSecurityBan(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetNextItem( -1, LVNI_SELECTED ) >= 0 );
}

void CSearchMonitorWnd::OnSecurityBan()
{
	CSingleLock pLock( &m_pSection, TRUE );

	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem >= 0 )
	{
		SOCKADDR_IN pHost = { 0 };
		pHost.sin_family = AF_INET;
		CString strNode = m_wndList.GetItemText( nItem, 3 );
		int nPos = strNode.Find( _T(':') );
		pHost.sin_addr.s_addr = inet_addr( CT2CA( (LPCTSTR)strNode.Left( nPos ) ) );
		pHost.sin_port = htons( (WORD)_tstoi( strNode.Mid( nPos + 1 ) ) );
		Security.Ban( &pHost.sin_addr, banSession );
	}
}

void CSearchMonitorWnd::OnUpdateBrowseLaunch(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetNextItem( -1, LVNI_SELECTED ) >= 0 );
}

void CSearchMonitorWnd::OnBrowseLaunch()
{
	CSingleLock pLock( &m_pSection, TRUE );

	const int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem >= 0 )
	{
		SOCKADDR_IN pHost = { 0 };
		pHost.sin_family = AF_INET;
		CString strNode = m_wndList.GetItemText( nItem, 3 );
		int nPos = strNode.Find( _T(':') );
		pHost.sin_addr.s_addr = inet_addr( CT2CA( (LPCTSTR)strNode.Left( nPos ) ) );
		pHost.sin_port = htons( (WORD)_tstoi( strNode.Mid( nPos + 1 ) ) );
		new CBrowseHostWnd( PROTOCOL_ANY, &pHost );
	}
}
