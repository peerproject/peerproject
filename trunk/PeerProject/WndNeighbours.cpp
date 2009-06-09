//
// WndNeighbours.cpp
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
#include "Network.h"
#include "Neighbours.h"
#include "G1Neighbour.h"
#include "G2Neighbour.h"
#include "EDNeighbour.h"
#include "EDPacket.h"
#include "HostCache.h"
#include "Security.h"
#include "LiveList.h"
#include "GProfile.h"
#include "ChatWindows.h"
#include "Skin.h"

#include "WndMain.h"
#include "WndNeighbours.h"
#include "WndPacket.h"
#include "WndBrowseHost.h"
#include "DlgURLCopy.h"
#include "DlgSettingsManager.h"
#include "CoolInterface.h"
#include "WindowManager.h"
#include "WndSystem.h"

#include "Flags.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CNeighboursWnd, CPanelWnd, 0)

BEGIN_MESSAGE_MAP(CNeighboursWnd, CPanelWnd)
	//{{AFX_MSG_MAP(CNeighboursWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_QUERYNEWPALETTE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_NEIGHBOURS, OnSortList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_NEIGHBOURS, OnCustomDrawList)
	ON_UPDATE_COMMAND_UI(ID_NEIGHBOURS_DISCONNECT, OnUpdateNeighboursDisconnect)
	ON_COMMAND(ID_NEIGHBOURS_DISCONNECT, OnNeighboursDisconnect)
	ON_UPDATE_COMMAND_UI(ID_NEIGHBOURS_VIEW_ALL, OnUpdateNeighboursViewAll)
	ON_COMMAND(ID_NEIGHBOURS_VIEW_ALL, OnNeighboursViewAll)
	ON_UPDATE_COMMAND_UI(ID_NEIGHBOURS_VIEW_INCOMING, OnUpdateNeighboursViewIncoming)
	ON_COMMAND(ID_NEIGHBOURS_VIEW_INCOMING, OnNeighboursViewIncoming)
	ON_UPDATE_COMMAND_UI(ID_NEIGHBOURS_VIEW_OUTGOING, OnUpdateNeighboursViewOutgoing)
	ON_COMMAND(ID_NEIGHBOURS_VIEW_OUTGOING, OnNeighboursViewOutgoing)
	ON_UPDATE_COMMAND_UI(ID_NEIGHBOURS_CHAT, OnUpdateNeighboursChat)
	ON_COMMAND(ID_NEIGHBOURS_CHAT, OnNeighboursChat)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_BAN, OnUpdateSecurityBan)
	ON_COMMAND(ID_SECURITY_BAN, OnSecurityBan)
	ON_UPDATE_COMMAND_UI(ID_BROWSE_LAUNCH, OnUpdateBrowseLaunch)
	ON_COMMAND(ID_BROWSE_LAUNCH, OnBrowseLaunch)
	ON_UPDATE_COMMAND_UI(ID_NEIGHBOURS_URI, OnUpdateNeighboursCopy)
	ON_COMMAND(ID_NEIGHBOURS_URI, OnNeighboursCopy)
	ON_COMMAND(ID_NEIGHBOURS_SETTINGS, OnNeighboursSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNeighboursWnd construction

CNeighboursWnd::CNeighboursWnd() : CPanelWnd( TRUE, TRUE ), m_nProtocolRev( 0 )
{
	Create( IDR_NEIGHBOURSFRAME );
}

/////////////////////////////////////////////////////////////////////////////
// CNeighboursWnd system message handlers

int CNeighboursWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_BORDER_TOP );

	m_wndList.Create( WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE |
		LVS_AUTOARRANGE | LVS_REPORT | LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_NEIGHBOURS );
	m_pSizer.Attach( &m_wndList );

	m_wndTip.Create( &m_wndList, &Settings.Interface.TipNeighbours );
	m_wndList.SetTip( &m_wndTip );

	m_wndList.SetExtendedStyle(
		LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_LABELTIP|LVS_EX_SUBITEMIMAGES );

	CBitmap bmImages;
	bmImages.LoadBitmap( IDB_PROTOCOLS );
	if ( Settings.General.LanguageRTL )
		bmImages.m_hObject = CreateMirroredBitmap( (HBITMAP)bmImages.m_hObject );

	m_gdiImageList.Create( 16, 16, ILC_COLOR32|ILC_MASK, 7, 1 ) ||
	m_gdiImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 7, 1 ) ||
	m_gdiImageList.Create( 16, 16, ILC_COLOR16|ILC_MASK, 7, 1 );
	m_gdiImageList.Add( &bmImages, RGB( 0, 255, 0 ) );

	// Merge protocols and flags in one image list
	int nImages = m_gdiImageList.GetImageCount();
	int nFlags = Flags.m_pImage.GetImageCount();
	m_nProtocolRev = nImages - 1; // save the max index

	m_gdiImageList.SetImageCount( nImages + nFlags );
	for ( int nFlag = 0 ; nFlag < nFlags ; nFlag++ )
	{
		HICON hIcon = Flags.m_pImage.ExtractIcon( nFlag );
		if ( hIcon )
		{
			m_gdiImageList.Replace( nImages + nFlag, hIcon );
			VERIFY( DestroyIcon( hIcon ) );
		}
	}

	m_wndList.SetImageList( &m_gdiImageList, LVSIL_SMALL );
	bmImages.DeleteObject();

	m_wndList.InsertColumn( 0, _T("Address"), LVCFMT_LEFT, 110, -1 );
	m_wndList.InsertColumn( 1, _T("Port"), LVCFMT_CENTER, 44, 0 );
	m_wndList.InsertColumn( 2, _T("Time"), LVCFMT_CENTER, 56, 1 );
	m_wndList.InsertColumn( 3, _T("Packets"), LVCFMT_CENTER, 70, 2 );
	m_wndList.InsertColumn( 4, _T("Bandwidth"), LVCFMT_CENTER, 84, 3 );
	m_wndList.InsertColumn( 5, _T("Total"), LVCFMT_CENTER, 96, 4 );
	m_wndList.InsertColumn( 6, _T("Flow"), LVCFMT_CENTER, 0, 5 );
	m_wndList.InsertColumn( 7, _T("Leaves"), LVCFMT_CENTER, 52, 6 );
	m_wndList.InsertColumn( 8, _T("Mode"), LVCFMT_CENTER, 60, 7 );
	m_wndList.InsertColumn( 9, _T("Client"), LVCFMT_LEFT, 100, 8 );
	m_wndList.InsertColumn( 10, _T("Name"), LVCFMT_LEFT, 100, 9 );
	m_wndList.InsertColumn( 11, _T("Country"), LVCFMT_LEFT, 54, 10 );

	m_wndList.SetFont( &theApp.m_gdiFont );

	LoadState( _T("CNeighboursWnd"), FALSE );

	m_tLastUpdate = 0;

	PostMessage( WM_TIMER, 1 );

	return 0;
}

void CNeighboursWnd::OnDestroy()
{
	Settings.SaveList( _T("CNeighboursWnd"), &m_wndList );
	SaveState( _T("CNeighboursWnd") );
	CPanelWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CNeighboursWnd operations

void CNeighboursWnd::Update()
{
	CSingleLock pLock( &Network.m_pSection );
	if ( ! pLock.Lock( 50 ) ) return;

	CLiveList pLiveList( 12 );

	m_tLastUpdate = GetTickCount();

	for ( POSITION pos = Neighbours.GetIterator() ; pos ; )
	{
		CString str;
		CNeighbour* pNeighbour = Neighbours.GetNext( pos );
		CLiveItem* pItem = pLiveList.Add( pNeighbour->m_nUnique );

		pItem->Set( 0, pNeighbour->m_sAddress );
		pItem->Format( 1, _T("%hu"), htons( pNeighbour->m_pHost.sin_port ) );

		DWORD nTime = ( m_tLastUpdate - pNeighbour->m_tConnected ) / 1000;

		switch ( pNeighbour->m_nState )
		{
		case nrsConnecting:
			LoadString ( str,IDS_NEIGHBOUR_CONNECTING );
			break;
		case nrsHandshake1:
		case nrsHandshake2:
		case nrsHandshake3:
			LoadString ( str,IDS_NEIGHBOUR_HANDSHAKING );
			break;
		case nrsRejected:
			LoadString ( str,IDS_NEIGHBOUR_REJECTED );
			break;
		case nrsClosing:
			LoadString ( str,IDS_NEIGHBOUR_CLOSING );
			break;
		case nrsConnected:
			if ( nTime > 86400 )
				str.Format( _T("%i:%.2i:%.2i:%.2i"), nTime / 86400, ( nTime / 3600 ) % 24, ( nTime / 60 ) % 60, nTime % 60 );
			else
				str.Format( _T("%i:%.2i:%.2i"), nTime / 3600, ( nTime / 60 ) % 60, nTime % 60 );
			break;
		case nrsNull:
		default:
			LoadString ( str,IDS_NEIGHBOUR_UNKNOWN );
			break;
		}

		pItem->Set( 2, str );

		pNeighbour->Measure();

		pItem->Format( 3, _T("%u - %u"), pNeighbour->m_nInputCount, pNeighbour->m_nOutputCount );
		pItem->Format( 4, _T("%s - %s"),
			Settings.SmartSpeed( pNeighbour->m_mInput.nMeasure ),
			Settings.SmartSpeed( pNeighbour->m_mOutput.nMeasure ) );
		pItem->Format( 5, _T("%s - %s"),
			Settings.SmartVolume( pNeighbour->m_mInput.nTotal ),
			Settings.SmartVolume( pNeighbour->m_mOutput.nTotal ) );
		pItem->Format( 6, _T("%u (%u)"), pNeighbour->m_nOutbound, pNeighbour->m_nLostCount );

		pItem->Set( 9, pNeighbour->m_sUserAgent );

		if ( pNeighbour->m_nState >= nrsConnected )
		{
			if ( pNeighbour->m_nProtocol == PROTOCOL_G1 )
			{
//				CG1Neighbour* pG1 = reinterpret_cast<CG1Neighbour*>(pNeighbour);

				switch ( pNeighbour->m_nNodeType )
				{
				case ntNode:
					LoadString ( str,IDS_NEIGHBOUR_G1PEER );
					break;
				case ntHub:
					LoadString ( str,IDS_NEIGHBOUR_G1ULTRA );
					break;
				case ntLeaf:
					LoadString ( str,IDS_NEIGHBOUR_G1LEAF );
					break;
				}

				pItem->Set( 8, str );
				pItem->m_nImage = Settings.General.LanguageRTL ? m_nProtocolRev - PROTOCOL_G1 : PROTOCOL_G1;
			}
			else if ( pNeighbour->m_nProtocol == PROTOCOL_G2 )
			{
				CG2Neighbour* pG2 = static_cast<CG2Neighbour*>(pNeighbour);

				switch ( pNeighbour->m_nNodeType )
				{
				case ntNode:
					LoadString ( str,IDS_NEIGHBOUR_G2PEER );
					break;
				case ntHub:
					LoadString ( str,IDS_NEIGHBOUR_G2HUB );
					break;
				case ntLeaf:
					LoadString ( str,IDS_NEIGHBOUR_G2LEAF );
					break;
				}
				pItem->Set( 8, str );

				if ( pG2->m_nLeafCount > 0 )
				{
					if ( pG2->m_nLeafLimit > 0 )
					{
						pItem->Format( 7, _T("%u/%u"), pG2->m_nLeafCount, pG2->m_nLeafLimit );
					}
					else
					{
						pItem->Format( 7, _T("%u"), pG2->m_nLeafCount );
					}
				}
				else if ( pG2->m_nNodeType != ntLeaf )
				{
					pItem->Set( 7, _T("?") );
				}

				pItem->m_nImage = Settings.General.LanguageRTL ? m_nProtocolRev - PROTOCOL_G2 : PROTOCOL_G2;
			}
			else if ( pNeighbour->m_nProtocol == PROTOCOL_ED2K )
			{
				CEDNeighbour* pED2K = static_cast<CEDNeighbour*>(pNeighbour);

				pItem->m_nImage = Settings.General.LanguageRTL ? m_nProtocolRev - PROTOCOL_ED2K : PROTOCOL_ED2K;
				pItem->Set( 8, _T("eDonkey") );
				pItem->Set( 10, pED2K->m_sServerName );

				if ( pED2K->m_nClientID > 0 )
				{
					if ( pED2K->m_nUserLimit > 0 )
					{
						pItem->Format( 7, _T("%u/%u"), pED2K->m_nUserCount, pED2K->m_nUserLimit );
					}
					else
					{
						pItem->Format( 7, _T("%u"), pED2K->m_nUserCount );
					}

					LoadString( str, CEDPacket::IsLowID( pED2K->m_nClientID ) ? IDS_NEIGHBOUR_ED2K_LOWID : IDS_NEIGHBOUR_ED2K_HIGHID );
					pItem->Set( 9, str );
				}
				else
				{
					LoadString ( str,IDS_NEIGHBOUR_ED2K_SERVER );
					pItem->Set( 9, str );
				}
			}
			else
			{
				pItem->m_nImage = Settings.General.LanguageRTL ? m_nProtocolRev : PROTOCOL_NULL;
			}
		}
		else
		{
			pItem->m_nImage = Settings.General.LanguageRTL ? m_nProtocolRev : PROTOCOL_NULL;
		}

		if ( pNeighbour->m_pProfile != NULL )
		{
			pItem->Set( 10, pNeighbour->m_pProfile->GetNick() );
		}

		pItem->Set( 11, pNeighbour->m_sCountry );
		int nFlag = Flags.GetFlagIndex( pNeighbour->m_sCountry );
		if ( nFlag >= 0 )
			pItem->SetImage( &m_wndList, (int)pNeighbour->m_nUnique, 11, m_nProtocolRev + nFlag + 1 );
	}

	pLiveList.Apply( &m_wndList, TRUE );
}

CNeighbour* CNeighboursWnd::GetItem(int nItem)
{
	if ( m_wndList.GetItemState( nItem, LVIS_SELECTED ) )
	{
		return Neighbours.Get( m_wndList.GetItemData( nItem ) );
	}

	return NULL;
}

void CNeighboursWnd::OnSkinChange()
{
	CPanelWnd::OnSkinChange();
	Settings.LoadList( _T("CNeighboursWnd"), &m_wndList );
	Skin.CreateToolBar( _T("CNeighboursWnd"), &m_wndToolBar );

	for ( int nImage = 0 ; nImage < 4 ; nImage++ )
	{
		HICON hIcon = CoolInterface.ExtractIcon( (UINT)protocolCmdMap[ nImage ].commandID, FALSE );
		if ( hIcon )
		{
			m_gdiImageList.Replace( Settings.General.LanguageRTL ? m_nProtocolRev - nImage : nImage, hIcon );
			DestroyIcon( hIcon );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNeighboursWnd message handlers

BOOL CNeighboursWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_wndToolBar.m_hWnd )
	{
		if ( m_wndToolBar.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}

	return CPanelWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CNeighboursWnd::OnSize(UINT nType, int cx, int cy)
{
	CPanelWnd::OnSize( nType, cx, cy );

	BOOL bSized = m_pSizer.Resize( cx );

	SizeListAndBar( &m_wndList, &m_wndToolBar );

	if ( bSized && m_wndList.GetItemCount() == 0 ) m_wndList.Invalidate();
}

void CNeighboursWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1 )
	{
		if ( ( IsPartiallyVisible() ) || ( GetTickCount() - m_tLastUpdate > 30000 ) )
			 Update();
	}
}

void CNeighboursWnd::OnSortList(NMHDR* pNotifyStruct, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNotifyStruct;
	CLiveList::Sort( &m_wndList, pNMListView->iSubItem );
	*pResult = 0;
}

void CNeighboursWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	Skin.TrackPopupMenu( _T("CNeighboursWnd"), point );
}

void CNeighboursWnd::OnUpdateNeighboursDisconnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CNeighboursWnd::OnNeighboursDisconnect()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVNI_SELECTED ) ) >= 0 ; )
	{
		if ( CNeighbour* pNeighbour = GetItem( nItem ) )
		{
			pNeighbour->Close( IDS_CONNECTION_CLOSED );
		}
	}
}

void CNeighboursWnd::OnUpdateNeighboursCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() == 1 );

}

void CNeighboursWnd::OnNeighboursCopy()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	CNeighbour* pNeighbour = GetItem( m_wndList.GetNextItem( -1, LVNI_SELECTED ) );
	if ( ! pNeighbour ) return;

	CString strURL;

	if ( pNeighbour->m_nProtocol == PROTOCOL_G1 || pNeighbour->m_nProtocol == PROTOCOL_G2 )
	{
		strURL.Format( _T("gnutella:host:%s:%lu"),
			(LPCTSTR)pNeighbour->m_sAddress, htons( pNeighbour->m_pHost.sin_port ) );
	}
	else if ( pNeighbour->m_nProtocol == PROTOCOL_ED2K )
	{
		strURL.Format( _T("ed2k://|server|%s|%lu|/"),
			(LPCTSTR)pNeighbour->m_sAddress, htons( pNeighbour->m_pHost.sin_port ) );
	}

	CURLCopyDlg::SetClipboardText( strURL );
}

void CNeighboursWnd::OnUpdateNeighboursChat(CCmdUI* pCmdUI)
{
	if ( m_wndList.GetSelectedCount() == 1 && Settings.Community.ChatEnable )
	{
		CSingleLock pNetworkLock( &Network.m_pSection );
		if ( pNetworkLock.Lock( 500 ) )
		{
			CNeighbour* pNeighbour = GetItem( m_wndList.GetNextItem( -1, LVNI_SELECTED ) );
			if ( pNeighbour && pNeighbour->m_nProtocol != PROTOCOL_ED2K )
			{
				pCmdUI->Enable( TRUE );
				return;
			}
		}
	}
	pCmdUI->Enable( FALSE );
}

void CNeighboursWnd::OnNeighboursChat()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVNI_SELECTED ) ) >= 0 ; )
	{
		if ( CNeighbour* pNeighbour = GetItem( nItem ) )
		{
			if ( pNeighbour->m_nProtocol != PROTOCOL_ED2K )
			{
				ChatWindows.OpenPrivate( pNeighbour->m_oGUID,
					&pNeighbour->m_pHost, FALSE, pNeighbour->m_nProtocol );
			}
		}
	}
}

void CNeighboursWnd::OnUpdateSecurityBan(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CNeighboursWnd::OnSecurityBan()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVNI_SELECTED ) ) >= 0 ; )
	{
		if ( CNeighbour* pNeighbour = GetItem( nItem ) )
		{
			IN_ADDR pAddress = pNeighbour->m_pHost.sin_addr;
			pNeighbour->Close();
			pLock.Unlock();
			Security.Ban( &pAddress, banSession );
			pLock.Lock();
		}
	}
}

void CNeighboursWnd::OnUpdateBrowseLaunch(CCmdUI* pCmdUI)
{
	if ( m_wndList.GetSelectedCount() == 1 )
	{
		CSingleLock pNetworkLock( &Network.m_pSection );
		if ( pNetworkLock.Lock( 500 ) )
		{
			CNeighbour* pNeighbour = GetItem( m_wndList.GetNextItem( -1, LVNI_SELECTED ) );
			if ( pNeighbour && pNeighbour->m_nProtocol != PROTOCOL_ED2K )
			{
				pCmdUI->Enable( TRUE );
				return;
			}
		}
	}
	pCmdUI->Enable( FALSE );
}

void CNeighboursWnd::OnBrowseLaunch()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	if ( CNeighbour* pNeighbour = GetItem( m_wndList.GetNextItem( -1, LVNI_SELECTED ) ) )
	{
		if ( pNeighbour->m_nProtocol != PROTOCOL_ED2K )
		{
			PROTOCOLID nProtocol = pNeighbour->m_nProtocol;
			SOCKADDR_IN pAddress = pNeighbour->m_pHost;
			Hashes::Guid oGUID = pNeighbour->m_oGUID;

			pLock.Unlock();

			new CBrowseHostWnd( nProtocol, &pAddress, oGUID );
		}
	}
}

void CNeighboursWnd::OnUpdateNeighboursViewAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() == 1 );
}

void CNeighboursWnd::OnNeighboursViewAll()
{
	OpenPacketWnd( TRUE, TRUE );
}

void CNeighboursWnd::OnUpdateNeighboursViewIncoming(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() == 1 );
}

void CNeighboursWnd::OnNeighboursViewIncoming()
{
	OpenPacketWnd( TRUE, FALSE );
}

void CNeighboursWnd::OnUpdateNeighboursViewOutgoing(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() == 1 );
}

void CNeighboursWnd::OnNeighboursViewOutgoing()
{
	OpenPacketWnd( FALSE, TRUE );
}

void CNeighboursWnd::OnNeighboursSettings()
{
	CSettingsManagerDlg::Run( _T("CNetworksSettingsPage") );
}

void CNeighboursWnd::OpenPacketWnd(BOOL bIncoming, BOOL bOutgoing)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );

	CWindowManager* pManager = GetManager();
	CPacketWnd* pWnd = NULL;

	while ( ( pWnd = (CPacketWnd*)pManager->Find( RUNTIME_CLASS(CPacketWnd), pWnd ) ) != NULL )
	{
		if ( pWnd->m_pOwner == this ) break;
	}

	if ( ! pWnd ) pWnd = new CPacketWnd( this );

	for ( int nItem = 0 ; nItem < m_wndList.GetItemCount() ; nItem++ )
	{
		if ( CNeighbour* pNeighbour = GetItem( nItem ) )
		{
			pWnd->m_nInputFilter	= bIncoming ? pNeighbour->m_nUnique : 1;
			pWnd->m_nOutputFilter	= bOutgoing ? pNeighbour->m_nUnique : 1;
		}
	}

	pWnd->m_bPaused = FALSE;
	pWnd->BringWindowToTop();
}

void CNeighboursWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;

	if ( ! ::IsWindow( m_wndList.GetSafeHwnd() ) ) return;

	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		if ( m_wndList.GetItemCount() == 0 && ! Network.IsConnected() )
		{
			DrawEmptyMessage( CDC::FromHandle( pDraw->nmcd.hdc ) );
		}

		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		LV_ITEM pItem = { LVIF_IMAGE, static_cast< int >( pDraw->nmcd.dwItemSpec ) };
		m_wndList.GetItem( &pItem );

		int nImage = Settings.General.LanguageRTL ? m_nProtocolRev - pItem.iImage : pItem.iImage;
		switch ( nImage )
		{
		case PROTOCOL_NULL:
			pDraw->clrText = CoolInterface.m_crNetworkNull ;
			break;
		case PROTOCOL_G1:
			pDraw->clrText = CoolInterface.m_crNetworkG1 ;
			break;
		case PROTOCOL_G2:
			pDraw->clrText = CoolInterface.m_crNetworkG2 ;
			break;
		case PROTOCOL_ED2K:
			pDraw->clrText = CoolInterface.m_crNetworkED2K ;
			break;
		}

		*pResult = CDRF_DODEFAULT;
	}
}

void CNeighboursWnd::DrawEmptyMessage(CDC* pDC)
{
	CRect rcClient, rcText;
	CString strText;

	m_wndList.GetClientRect( &rcClient );

	if ( CWnd* pHeader = m_wndList.GetWindow( GW_CHILD ) )
	{
		pHeader->GetWindowRect( &rcText );
		rcClient.top += rcText.Height();
	}

	rcText.SetRect( rcClient.left, 16, rcClient.right, 0 );
	rcText.bottom = ( rcClient.top + rcClient.bottom ) / 2;
	rcText.top = rcText.bottom - rcText.top;

	pDC->SetBkMode( TRANSPARENT );
	CFont* pOldFont = (CFont*)pDC->SelectObject( &theApp.m_gdiFont );
	pDC->SetTextColor( CoolInterface.m_crText );
	LoadString( strText, IDS_NEIGHBOURS_NOT_CONNECTED );
	pDC->DrawText( strText, &rcText, DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX );

	rcText.OffsetRect( 0, rcText.Height() );

	LoadString( strText, IDS_NEIGHBOURS_CONNECT );
	pDC->DrawText( strText, &rcText, DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX );

	pDC->SelectObject( pOldFont );
}

void CNeighboursWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CPanelWnd::OnActivate(nState, pWndOther, bMinimized);
	Update();
}

BOOL CNeighboursWnd::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
	{
		GetManager()->Open( RUNTIME_CLASS(CSystemWnd) );
		return TRUE;
	}

	return CPanelWnd::PreTranslateMessage(pMsg);
}
