//
// WndPacket.cpp
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
#include "PeerProject.h"
#include "Settings.h"
#include "Network.h"
#include "Neighbours.h"
#include "Neighbour.h"
#include "WndPacket.h"
#include "LiveList.h"
#include "CoolMenu.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

// Set Column Order
enum {
	COL_TIME,
	COL_ADDRESS,
	COL_PROTOCOL,
	COL_TYPE,
	COL_HOPS,
	COL_GUID,
	COL_HEX,
	COL_ASCII,
	COL_LAST  // Column Count
};


IMPLEMENT_SERIAL(CPacketWnd, CPanelWnd, 0)

BEGIN_MESSAGE_MAP(CPacketWnd, CPanelWnd)
	//{{AFX_MSG_MAP(CPacketWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PACKETS, OnCustomDrawList)
	ON_UPDATE_COMMAND_UI(ID_SYSTEM_CLEAR, OnUpdateSystemClear)
	ON_UPDATE_COMMAND_UI_RANGE(1, 3500, OnUpdateBlocker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

G2_PACKET CPacketWnd::m_nG2[nTypeG2Size] = {
	G2_PACKET_CACHED_HUB,
	G2_PACKET_CRAWL_ANS,
	G2_PACKET_CRAWL_REQ,
	G2_PACKET_DISCOVERY,
	G2_PACKET_HAW,
	G2_PACKET_HIT,
	G2_PACKET_HIT_WRAP,
	G2_PACKET_KHL,
	G2_PACKET_KHL_ANS,
	G2_PACKET_KHL_REQ,
	G2_PACKET_LNI,
	G2_PACKET_PING,
	G2_PACKET_PONG,
	G2_PACKET_PROFILE_CHALLENGE,
	G2_PACKET_PROFILE_DELIVERY,
	G2_PACKET_PUSH,
	G2_PACKET_QHT,
	G2_PACKET_QUERY,
	G2_PACKET_QUERY_ACK,
	G2_PACKET_QUERY_KEY_ANS,
	G2_PACKET_QUERY_KEY_REQ,
	G2_PACKET_QUERY_WRAP
};

/////////////////////////////////////////////////////////////////////////////
// CPacketWnd construction

CPacketWnd::CPacketWnd(CChildWnd* pOwner)
{
	m_pOwner  = pOwner;
	m_bPaused = TRUE;
	Create( IDR_PACKETFRAME );
}

CPacketWnd::~CPacketWnd()
{
//	theApp.m_pPacketWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CPacketWnd create

int CPacketWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	m_wndList.Create( WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE |
		LVS_AUTOARRANGE | LVS_REPORT | LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_PACKETS );
	m_pSizer.Attach( &m_wndList );

	m_wndList.SetExtendedStyle(
		LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_LABELTIP|LVS_EX_SUBITEMIMAGES );

	CRect rcList;
	m_wndList.GetClientRect( &rcList );

	m_wndList.InsertColumn( COL_TIME,	_T("Time"), 	LVCFMT_CENTER,	80, -1 );
	m_wndList.InsertColumn( COL_ADDRESS, _T("Address"), LVCFMT_LEFT,	110, -1 );
	m_wndList.InsertColumn( COL_PROTOCOL, _T("Protocol"), LVCFMT_CENTER, 80, 0 );
	m_wndList.InsertColumn( COL_TYPE,	_T("Type"), 	LVCFMT_CENTER,	80, 1 );
	m_wndList.InsertColumn( COL_HOPS,	_T("TTL/Hops"), LVCFMT_CENTER,	50, 2 );
	m_wndList.InsertColumn( COL_GUID,	_T("GUID"), 	LVCFMT_LEFT,	50, 5 );
	m_wndList.InsertColumn( COL_HEX,	_T("Hex"),		LVCFMT_LEFT,	50, 3 );
	m_wndList.InsertColumn( COL_ASCII,	_T("ASCII"),	LVCFMT_LEFT,	rcList.Width() - 540, 4 );

	LoadState();

	m_pCoolMenu		= NULL;
	m_nInputFilter	= 0;
	m_nOutputFilter	= 0;
	m_bPaused		= FALSE;

	for ( int nType = 0 ; nType < nTypeG1Size ; nType++ ) m_bTypeG1[ nType ] = TRUE;
	for ( int nType = 0 ; nType < nTypeG2Size ; nType++ ) m_bTypeG2[ nType ] = TRUE;
	m_bTypeED = TRUE;
	m_bTypeDC = TRUE;
	m_bTypeBT = TRUE;

	SetTimer( 2, 500, NULL );

	return 0;
}

void CPacketWnd::OnDestroy()
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

	Settings.SaveList( _T("CPacketWnd"), &m_wndList );
	SaveState( _T("CPacketWnd") );

	CPanelWnd::OnDestroy();
}

void CPacketWnd::OnSkinChange()
{
	CPanelWnd::OnSkinChange();

	// Columns
	Settings.LoadList( _T("CPacketWnd"), &m_wndList );

	// Fonts
	if ( m_pFont.m_hObject ) m_pFont.DeleteObject();
	m_pFont.CreateFont( -(int)(Settings.Fonts.FontSize /*- 1*/), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, Settings.Fonts.PacketDumpFont );
	m_wndList.SetFont( &m_pFont );
	m_wndList.GetHeaderCtrl()->SetFont( &theApp.m_gdiFont );

	// Icons
	CoolInterface.LoadIconsTo( m_gdiImageList, protocolIDs );
	VERIFY( m_gdiImageList.SetImageCount( m_gdiImageList.GetImageCount() + 2 ) );
	VERIFY( m_gdiImageList.Replace( PROTOCOL_LAST + 0, CoolInterface.ExtractIcon( IDI_INCOMING ) ) != -1 );
	VERIFY( m_gdiImageList.Replace( PROTOCOL_LAST + 1, CoolInterface.ExtractIcon( IDI_OUTGOING ) ) != -1 );
	m_wndList.SetImageList( &m_gdiImageList, LVSIL_SMALL );
}


/////////////////////////////////////////////////////////////////////////////
// CPacketWnd operations

void CPacketWnd::SmartDump(const CPacket* pPacket, const SOCKADDR_IN* pAddress, BOOL bUDP, BOOL bOutgoing, DWORD_PTR nNeighbourUnique)
{
	if ( m_bPaused || m_hWnd == NULL )
		return;

	if ( nNeighbourUnique )
	{
		if ( bOutgoing )
		{
			if ( m_nOutputFilter && m_nOutputFilter != nNeighbourUnique )
				return;
		}
		else
		{
			if ( m_nInputFilter && m_nInputFilter != nNeighbourUnique )
				return;
		}
	}
	else
	{
		if ( bOutgoing )
		{
			if ( m_nOutputFilter )
				return;
		}
		else
		{
			if ( m_nInputFilter )
				return;
		}
	}

	switch ( pPacket->m_nProtocol )
	{
	case PROTOCOL_G1:
		if ( ! m_bTypeG1[ ((CG1Packet*)pPacket)->m_nTypeIndex ] )
			return;
		break;

	case PROTOCOL_G2:
		for ( int nType = 0 ; nType < nTypeG2Size ; nType++ )
		{
			if ( ((CG2Packet*)pPacket)->IsType( m_nG2[ nType ] ) )
			{
				if ( ! m_bTypeG2[ nType ] )
					return;
				break;
			}
		}
		break;

	case PROTOCOL_ED2K:
		// Filter ED2K packets?
		if ( ! m_bTypeED )
			return;
		break;

	case PROTOCOL_DC:
		// Filter DC++ packets?
		if ( ! m_bTypeDC )
			return;
		break;

	case PROTOCOL_BT:
		// Filter BitTorrent packets?
		if ( ! m_bTypeBT )
			return;
		break;
	}

	CLiveItem* pItem = new CLiveItem( COL_LAST, bOutgoing );

	CTime pNow( CTime::GetCurrentTime() );
	CString strNow;
	strNow.Format( _T("%0.2i:%0.2i:%0.2i"),
		pNow.GetHour(), pNow.GetMinute(), pNow.GetSecond() );
	const CString sAddress( inet_ntoa( pAddress->sin_addr ) );
	const CString sProtocol( protocolAbbr[ pPacket->m_nProtocol ] );

	pItem->Set( COL_TIME,	strNow );
	pItem->Set( COL_ADDRESS, bUDP ? _T("(") + sAddress + _T(")") : sAddress );
	pItem->Set( COL_PROTOCOL, sProtocol + ( bUDP ? _T(" UDP") : _T(" TCP") ) );
	pItem->Set( COL_TYPE,	pPacket->GetType() );
	pItem->Set( COL_HEX,	pPacket->ToHex() );
	pItem->Set( COL_ASCII,	pPacket->ToASCII() );

	pItem->SetImage( COL_TIME, PROTOCOL_LAST + ( bOutgoing ? 1 : 0 ) );
	pItem->SetImage( COL_PROTOCOL, pPacket->m_nProtocol );

	if ( pPacket->m_nProtocol == PROTOCOL_G1 )
	{
		pItem->Format( COL_HOPS, _T("%u/%u"), ((CG1Packet*)pPacket)->m_nTTL, ((CG1Packet*)pPacket)->m_nHops );
		pItem->Set( COL_GUID, ((CG1Packet*)pPacket)->GetGUID() );
	}
	else if ( pPacket->m_nLength )
	{
		pItem->Format( COL_GUID, _T("(%u)"), pPacket->m_nLength );
	}

	CSingleLock pLock( &m_pSection, TRUE );

	m_pQueue.AddTail( pItem );
}

void CPacketWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent != 2 ) return;

	BOOL bScroll = m_wndList.GetTopIndex() + m_wndList.GetCountPerPage() >= m_wndList.GetItemCount();

	CSingleLock pLock( &m_pSection );
	BOOL bAny = FALSE;

	for (;;)
	{
		pLock.Lock();

		if ( m_pQueue.GetCount() == 0 ) break;
		CLiveItem* pItem = m_pQueue.RemoveHead();

		pLock.Unlock();

		if ( ! bAny )
			bAny = TRUE;

		if ( (DWORD)m_wndList.GetItemCount() >= Settings.Search.MonitorQueue && Settings.Search.MonitorQueue > 0 )
			m_wndList.DeleteItem( 0 );

		pItem->Add( &m_wndList, -1, 8 );

		delete pItem;
	}

	if ( bAny && bScroll )
		m_wndList.EnsureVisible( m_wndList.GetItemCount() - 1, FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CPacketWnd message handlers

void CPacketWnd::OnSize(UINT nType, int cx, int cy)
{
	if ( ! m_wndList ) return;

	CPanelWnd::OnSize( nType, cx, cy );
	m_pSizer.Resize( cx );
	m_wndList.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}

void CPacketWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;

	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		if ( m_nInputFilter != 1 && m_nOutputFilter != 1 )
		{
			if ( pDraw->nmcd.lItemlParam )
				pDraw->clrText = Colors.m_crNetworkUp;
			else
				pDraw->clrText = Colors.m_crNetworkDown;
		}
		*pResult = CDRF_DODEFAULT;
	}
}

void CPacketWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	CMenu pMenu, pHosts[2], pTypesG1, pTypesG2, pTypesED, pTypesDC, pTypesBT;

	for ( int nGroup = 0 ; nGroup < 2 ; nGroup++ )
	{
		UINT nID = nGroup ? 2000 : 1000;

		pHosts[nGroup].CreatePopupMenu();

		AddNeighbour( pHosts, nGroup, nID++, 1, _T("Disable") );
		AddNeighbour( pHosts, nGroup, nID++, 0, _T("Any Neighbour") );
		pHosts[nGroup].AppendMenu( MF_SEPARATOR, ID_SEPARATOR );

		for ( POSITION pos = Neighbours.GetIterator() ; pos ; nID++ )
		{
			CNeighbour* pNeighbour = Neighbours.GetNext( pos );
			if ( pNeighbour->m_nState < nrsConnected ) continue;
			AddNeighbour( pHosts, nGroup, nID, (DWORD_PTR)pNeighbour, pNeighbour->m_sAddress );
		}

		if ( ( nID % 1000 ) == 2 )
			pHosts[nGroup].AppendMenu( MF_STRING|MF_GRAYED, 999, _T("No Neighbours") );
	}

	pTypesG1.CreatePopupMenu();
	for ( int nType = 0 ; nType < nTypeG1Size ; nType++ )
	{
		pTypesG1.AppendMenu( MF_STRING|( m_bTypeG1[ nType ] ? MF_CHECKED : 0 ), 3000 + nType, CG1Packet::m_pszPackets[ nType ] );
	}

	pTypesG2.CreatePopupMenu();
	for ( int nType = 0 ; nType < nTypeG2Size ; nType++ )
	{
		CStringA tmp;
		tmp.Append( (LPCSTR)&m_nG2[ nType ], G2_TYPE_LEN( m_nG2[ nType ] ) );
		pTypesG2.AppendMenu( MF_STRING|( m_bTypeG2[ nType ] ? MF_CHECKED : 0 ), 3100 + nType, CString( tmp ) );
	}

// ToDo: Filter packet types for other networks
//	pTypesED.CreatePopupMenu();
//	pTypesED.AppendMenu( MF_STRING|( m_bTypeED ? MF_CHECKED : 0 ), 3300, LoadString( IDS_GENERAL_ALL ) );
//
//	pTypesDC.CreatePopupMenu();
//	pTypesDC.AppendMenu( MF_STRING|( m_bTypeDC ? MF_CHECKED : 0 ), 3400, LoadString( IDS_GENERAL_ALL ) );
//
//	pTypesBT.CreatePopupMenu();
//	pTypesBT.AppendMenu( MF_STRING|( m_bTypeBT ? MF_CHECKED : 0 ), 3500, LoadString( IDS_GENERAL_ALL ) );

	const CString strType = Settings.General.Language.Left(2) == _T("en") ? L"Types" : LoadString( IDS_TIP_TYPE );

	pMenu.CreatePopupMenu();
	pMenu.AppendMenu( MF_STRING|MF_POPUP, (UINT_PTR)pHosts[0].GetSafeHmenu(), _T("&Incoming") );
	pMenu.AppendMenu( MF_STRING|MF_POPUP, (UINT_PTR)pHosts[1].GetSafeHmenu(), _T("&Outgoing") );
	pMenu.AppendMenu( MF_SEPARATOR, ID_SEPARATOR );
	pMenu.AppendMenu( MF_STRING|MF_POPUP, (UINT_PTR)pTypesG1.GetSafeHmenu(), _T("&G1 ") + strType );
	pMenu.AppendMenu( MF_STRING|MF_POPUP, (UINT_PTR)pTypesG2.GetSafeHmenu(), _T("G&2 ") + strType );
//	pMenu.AppendMenu( MF_STRING|MF_POPUP, (UINT_PTR)pTypesED.GetSafeHmenu(), _T("&ED2K ") + strType );
//	pMenu.AppendMenu( MF_STRING|MF_POPUP, (UINT_PTR)pTypesDC.GetSafeHmenu(), _T("&DC ") + strType );
//	pMenu.AppendMenu( MF_STRING|MF_POPUP, (UINT_PTR)pTypesBT.GetSafeHmenu(), _T("&BT ") + strType );
	pMenu.AppendMenu( MF_STRING|( m_bTypeED ? MF_CHECKED : 0 ), 3300, _T("&ED2K ") + strType );
	pMenu.AppendMenu( MF_STRING|( m_bTypeDC ? MF_CHECKED : 0 ), 3400, _T("&DC ") + strType );
	pMenu.AppendMenu( MF_STRING|( m_bTypeBT ? MF_CHECKED : 0 ), 3500, _T("&BT ") + strType );
	pMenu.AppendMenu( MF_SEPARATOR, ID_SEPARATOR );
	pMenu.AppendMenu( MF_STRING | ( m_bPaused ? MF_CHECKED : 0 ), 1, _T("&Pause Display") );
	pMenu.AppendMenu( MF_STRING, ID_SYSTEM_CLEAR, _T("&Clear Buffer") );

	m_pCoolMenu = new CCoolMenu();
	m_pCoolMenu->AddMenu( &pMenu, TRUE );
	m_pCoolMenu->SetWatermark( Skin.GetWatermark( _T("CCoolMenu") ) );

	pLock.Unlock();

	if ( point.x == -1 && point.y == -1 ) 	// Keyboard fix
		ClientToScreen( &point );

	UINT nCmd = pMenu.TrackPopupMenu( TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
		point.x, point.y, this );

	delete m_pCoolMenu;
	m_pCoolMenu = NULL;

	DWORD_PTR* pModify = NULL;

	if ( nCmd == 1 )
	{
		m_bPaused = ! m_bPaused;
		return;
	}

	if ( nCmd == ID_SYSTEM_CLEAR )
	{
		m_wndList.DeleteAllItems();
		return;
	}

	if ( nCmd >= 3000 && nCmd < 3000 + nTypeG1Size )
	{
		nCmd -= 3000;

		if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
		{
			for ( int nType = 0 ; nType < nTypeG1Size ; nType++ )
			{
				m_bTypeG1[ nType ] = ( nCmd == (UINT)nType ) ? TRUE : FALSE;
			}
		}
		else
		{
			m_bTypeG1[ nCmd ] = ! m_bTypeG1[ nCmd ];
		}

		return;
	}

	if ( nCmd >= 3100 && nCmd < 3100 + nTypeG2Size )
	{
		nCmd -= 3100;

		if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
		{
			for ( int nType = 0 ; nType < nTypeG2Size ; nType++ )
			{
				m_bTypeG2[ nType ] = ( nCmd == (UINT)nType ) ? TRUE : FALSE;
			}
		}
		else
		{
			m_bTypeG2[ nCmd ] = ! m_bTypeG2[ nCmd ];
		}

		return;
	}

	if ( nCmd == 3300 )
	{
	//	nCmd -= 3300;
	//	if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
	//		m_bTypeED = TRUE;	//( nCmd == nCmd ) ? TRUE : FALSE;
	//	else
			m_bTypeED = ! m_bTypeED;
		return;
	}

	if ( nCmd == 3400 )
	{
	//	nCmd -= 3400;
	//	if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
	//		m_bTypeDC = TRUE;	//( nCmd == nCmd ) ? TRUE : FALSE;
	//	else
			m_bTypeDC = ! m_bTypeDC;
		return;
	}

	if ( nCmd == 3500 )
	{
	//	nCmd -= 3500;
	//	if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
	//		m_bTypeBT = TRUE;	//( nCmd == nCmd ) ? TRUE : FALSE;
	//	else
			m_bTypeBT = ! m_bTypeBT;
		return;
	}

	if ( nCmd >= 1000 && nCmd < 2000 )
	{
		pModify = &m_nInputFilter;
		nCmd -= 1000;
	}
	else if ( nCmd >= 2000 && nCmd < 3000 )
	{
		pModify = &m_nOutputFilter;
		nCmd -= 2000;
	}
	else
	{
		return;
	}

	if ( nCmd == 0 )
	{
		*pModify = 1;
	}
	else if ( nCmd == 1 )
	{
		*pModify = 0;
	}
	else
	{
		pLock.Lock();
		nCmd -= 2;

		for ( POSITION pos = Neighbours.GetIterator() ; pos ; nCmd-- )
		{
			CNeighbour* pNeighbour = Neighbours.GetNext( pos );
			if ( ! nCmd )
			{
				*pModify = (DWORD_PTR)pNeighbour;
				break;
			}
		}
	}

	Invalidate();
}

void CPacketWnd::AddNeighbour(CMenu* pMenus, int nGroup, UINT nID, DWORD_PTR nTarget, LPCTSTR pszText)
{
	UINT nChecked = ( ( nGroup == 1 && m_nOutputFilter == nTarget ) ||
		 ( nGroup == 0 && m_nInputFilter == nTarget ) ) ? MF_CHECKED : 0;

	pMenus[nGroup].AppendMenu( MF_STRING|nChecked, nID, pszText );
}

void CPacketWnd::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if ( m_pCoolMenu )
		m_pCoolMenu->OnMeasureItem( lpMeasureItemStruct );
}

void CPacketWnd::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ( m_pCoolMenu )
		m_pCoolMenu->OnDrawItem( lpDrawItemStruct );
}

void CPacketWnd::OnUpdateBlocker(CCmdUI* pCmdUI)
{
	if ( m_pCoolMenu )
		pCmdUI->Enable( pCmdUI->m_nID != 999 );
	else
		pCmdUI->ContinueRouting();
}

void CPacketWnd::OnUpdateSystemClear(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( TRUE );
}
