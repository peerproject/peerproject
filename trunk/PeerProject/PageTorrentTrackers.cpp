//
// PageTorrentTrackers.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2006.
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
#include "PageTorrentTrackers.h"

#include "DlgDownloadSheet.h"
#include "BENode.h"
#include "Network.h"
#include "Transfers.h"
#include "Downloads.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CTorrentTrackersPage, CPropertyPageAdv)

BEGIN_MESSAGE_MAP(CTorrentTrackersPage, CPropertyPageAdv)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TORRENT_TRACKERS, &CTorrentTrackersPage::OnCustomDrawList)
	ON_NOTIFY(NM_CLICK, IDC_TORRENT_TRACKERS, &CTorrentTrackersPage::OnNMClickTorrentTrackers)
	ON_BN_CLICKED(IDC_TORRENT_REFRESH, &CTorrentTrackersPage::OnTorrentRefresh)
	ON_EN_CHANGE(IDC_TORRENT_TRACKER, &CTorrentTrackersPage::OnEnChangeTorrentTracker)
	ON_CBN_SELCHANGE(IDC_TORRENT_TRACKERMODE, &CTorrentTrackersPage::OnCbnSelchangeTorrentTrackermode)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTorrentTrackersPage property page

CTorrentTrackersPage::CTorrentTrackersPage()
	: CPropertyPageAdv	( CTorrentTrackersPage::IDD )
	, m_nComplete		( 0 )
	, m_nIncomplete		( 0 )
	, m_pRequest		( NULL )
{
}

CTorrentTrackersPage::~CTorrentTrackersPage()
{
}

void CTorrentTrackersPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageAdv::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_TORRENT_TRACKER, m_sTracker);
	DDX_Control(pDX, IDC_TORRENT_COMPLETED, m_wndComplete);
	DDX_Control(pDX, IDC_TORRENT_INCOMPLETE, m_wndIncomplete);
	DDX_Control(pDX, IDC_TORRENT_REFRESH, m_wndRefresh);
	DDX_Control(pDX, IDC_TORRENT_TRACKERS, m_wndTrackers);
	DDX_Control(pDX, IDC_TORRENT_TRACKERMODE, m_wndTrackerMode);
}

/////////////////////////////////////////////////////////////////////////////
// CTorrentTrackersPage message handlers

BOOL CTorrentTrackersPage::OnInitDialog()
{
	if ( ! CPropertyPageAdv::OnInitDialog() )
		return FALSE;

	ASSUME_LOCK( Transfers.m_pSection );

	CDownload* pDownload = ((CDownloadSheet*)GetParent())->GetDownload();
	ASSERT( pDownload && pDownload->IsTorrent() );

	CBTInfo& oInfo = pDownload->m_pTorrent;

	m_sTracker = oInfo.GetTrackerAddress();
	int nCount = oInfo.GetTrackerCount();
	int nTrackerMode = oInfo.GetTrackerMode();

	// Remove invalid modes
	if ( nCount < 2 )
	{
		ASSERT( nTrackerMode!= CBTInfo::tMultiFound );
		m_wndTrackerMode.DeleteString( CBTInfo::tMultiFound );
		ASSERT( nTrackerMode != CBTInfo::tMultiFinding );
		m_wndTrackerMode.DeleteString( CBTInfo::tMultiFinding );
	}
	m_wndTrackerMode.SetCurSel( nTrackerMode );

	CRect rc;
	m_wndTrackers.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL );
	CoolInterface.SetImageListTo( m_wndTrackers, LVSIL_SMALL );
	m_wndTrackers.SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP );
	m_wndTrackers.InsertColumn( 0, _T("Tracker"), LVCFMT_LEFT, rc.right - 82, -1 );
	m_wndTrackers.InsertColumn( 1, _T("Status"), LVCFMT_CENTER, 82, 0 );
	m_wndTrackers.InsertColumn( 2, _T("Type"), LVCFMT_CENTER, 0, 0 );
	Skin.Translate( _T("CTorrentTrackerList"), m_wndTrackers.GetHeaderCtrl() );

	if ( m_wndTrackers.SetBkImage( Skin.GetWatermark( _T("CListCtrl") ) ) )
		m_wndTrackers.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_LABELTIP );	// No LVS_EX_DOUBLEBUFFER
	else
		m_wndTrackers.SetBkColor( Colors.m_crWindow );

	for ( int nTracker = 0 ; nTracker < nCount ; nTracker++ )
	{
		LV_ITEM pItem = {};
		pItem.mask		= LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
		pItem.iItem		= m_wndTrackers.GetItemCount();
		pItem.lParam	= (LPARAM)nTracker;

		if ( oInfo.GetTrackerIndex() == nTracker )
			pItem.iImage = CoolInterface.ImageForID( ID_MEDIA_SELECT );
		else if ( oInfo.GetTrackerAddress( nTracker ).GetAt( 0 ) == BAD_TRACKER_TOKEN )
			pItem.iImage = CoolInterface.ImageForID( ID_DISCOVERY_BLOCKED );
		else
			pItem.iImage = CoolInterface.ImageForID( ID_DOWNLOADS_URI );

		pItem.pszText	= (LPTSTR)(LPCTSTR)oInfo.GetTrackerAddress( nTracker );
		pItem.iItem		= m_wndTrackers.InsertItem( &pItem );

		// Display status
		CString sStatus, sType = _T("Announce");
		if ( oInfo.GetTrackerAddress( nTracker ).GetAt( 0 ) == BAD_TRACKER_TOKEN )	// Tagged for display only (*https:// etc.)
		{
			LoadString( sStatus, IDS_STATUS_UNSUPPORTED );
		}
		else
		{
			switch ( oInfo.GetTrackerStatus( nTracker ) )
			{
			case TRI_UNKNOWN:
				LoadString( sStatus, IDS_STATUS_UNKNOWN );
				break;
			case TRI_FALSE:
				LoadString( sStatus, IDS_STATUS_TRACKERDOWN );
				break;
			case TRI_TRUE:
				LoadString( sStatus, IDS_STATUS_ACTIVE );
				break;
			}
		}

		m_wndTrackers.SetItemText( pItem.iItem, 1, sStatus );

		// Display type
		if ( oInfo.IsMultiTracker() )
			sType.Format( _T("Tier %i"), oInfo.GetTrackerTier( nTracker ) );

		m_wndTrackers.SetItemText( pItem.iItem, 2, sType );
	}

	GetDlgItem( IDC_TORRENT_TRACKER )->EnableWindow( nTrackerMode != CBTInfo::tNull );

	UpdateData( FALSE );

	if ( Network.IsConnected() )
		PostMessage( WM_COMMAND, MAKELONG( IDC_TORRENT_REFRESH, BN_CLICKED ), (LPARAM)m_wndRefresh.GetSafeHwnd() );

	return TRUE;
}

void CTorrentTrackersPage::OnDestroy()
{
	KillTimer( 1 );

	// Cancel unfinished yet request
	if ( m_pRequest )
	{
		CDownloadSheet* pSheet = (CDownloadSheet*)GetParent();

		CQuickLock oLock( Transfers.m_pSection );

		if ( CDownload* pDownload = pSheet->GetDownload() )
		{
			pDownload->CancelRequest( m_pRequest );
		}
	}

	CPropertyPageAdv::OnDestroy();
}

void CTorrentTrackersPage::OnTrackerEvent(bool bSuccess, LPCTSTR /*pszReason*/, LPCTSTR /*pszTip*/, CBTTrackerRequest* pEvent)
{
	ASSUME_LOCK( Transfers.m_pSection );

	m_pRequest = NULL;	// Need no cancel

	if ( bSuccess )
	{
		m_nComplete = pEvent->m_nSeeders;
		m_nIncomplete = pEvent->m_nLeechers;
	}

	PostMessage( WM_TIMER, bSuccess ? 3 : 2 );
}

void CTorrentTrackersPage::OnTorrentRefresh()
{
	if ( m_wndRefresh.IsWindowEnabled() == FALSE )
		return;

	CDownloadSheet* pSheet = (CDownloadSheet*)GetParent();

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return;

	CDownload* pDownload = pSheet->GetDownload();
	if ( ! pDownload )
		return;

	m_wndRefresh.EnableWindow( FALSE );

	m_pRequest = new CBTTrackerRequest( pDownload, BTE_TRACKER_SCRAPE, 0, this );

	//BeginThread( "PageTorrentTrackers" );	// Obsolete
}

void CTorrentTrackersPage::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1 )
	{
		// Re-enable the refresh button
		m_wndRefresh.EnableWindow( TRUE );
		KillTimer( 1 );
	}
	else
	{
		// Close the scrape thread (Obsolete)
		//CloseThread();

		// Re-enable the refresh button
		SetTimer( 1, 3300, NULL );

		// Update the display
		if ( nIDEvent == 3 )
		{
			CString str;
			str.Format( _T("%u"), m_nComplete );
			m_wndComplete.SetWindowText( str );
			str.Format( _T("%u"), m_nIncomplete );
			m_wndIncomplete.SetWindowText( str );
		}
		else
		{
			m_wndComplete.SetWindowText( _T("") );
			m_wndIncomplete.SetWindowText( _T("") );
		}
	}
}

BOOL CTorrentTrackersPage::OnApply()
{
	if ( ! UpdateData() )
		return FALSE;

	CDownloadSheet* pSheet = (CDownloadSheet*)GetParent();

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 500 ) )
		return FALSE;

	CDownload* pDownload = pSheet->GetDownload();
	if ( ! pDownload )
		return CPropertyPageAdv::OnApply();

	CBTInfo& oInfo = pDownload->m_pTorrent;

	const int nTrackerMode = m_wndTrackerMode.GetCurSel();
	const BOOL bModeChanged = oInfo.GetTrackerMode() != nTrackerMode;
	const BOOL bAddressChanged = oInfo.GetTrackerAddress() != m_sTracker;
	if ( bAddressChanged || bModeChanged )
	{
		oLock.Unlock();

		GetDlgItem( IDC_TORRENT_TRACKER )->SetFocus();

		// Display warning
		if ( AfxMessageBox( IDS_BT_TRACK_CHANGE, MB_ICONQUESTION | MB_YESNO ) != IDYES )
			return FALSE;

		if ( ! oLock.Lock( 250 ) )
			return FALSE;

		pDownload = pSheet->GetDownload();
		if ( ! pDownload )
			return CPropertyPageAdv::OnApply();

		if ( bAddressChanged )
			oInfo.SetTracker( m_sTracker );
		if ( bModeChanged )
			oInfo.SetTrackerMode( nTrackerMode );
	}

	return CPropertyPageAdv::OnApply();
}

// Obsolete thread scrape method for reference or deletion
//void CTorrentTrackersPage::OnRun()
//{
//	CSingleLock oLock( &Transfers.m_pSection );
//	if ( oLock.Lock( 250 ) )
//	{
//		CDownloadSheet* pSheet = (CDownloadSheet*)GetParent();
//		if ( CDownload* pDownload = pSheet->GetDownload() )
//		{
//			m_pRequest.Clear();
//
//			CString strURL = m_sTracker;
//			if ( strURL.Left( 4 ) == _T("http") &&	// ToDo: Support UDP Tracker Scrape!
//				strURL.Replace( _T("/announce"), _T("/scrape") ) == 1 )
//			{
//				// Fetch scrape only for the given info hash
//				strURL = strURL.TrimRight( _T('&') ) +
//					( ( strURL.Find( _T('?') ) != -1 ) ? _T('&') : _T('?') ) +
//					_T("info_hash=") + CBTTrackerRequest::Escape( pDownload->m_pTorrent.m_oBTH ) +
//					_T("&peer_id=")  + CBTTrackerRequest::Escape( pDownload->m_pPeerID );
//
//				oLock.Unlock();
//
//				m_pRequest.SetURL( strURL );
//				m_pRequest.AddHeader( _T("Accept-Encoding"), _T("deflate, gzip") );
//				m_pRequest.EnableCookie( false );
//				m_pRequest.SetUserAgent( Settings.SmartAgent() );
//
//				theApp.Message( MSG_DEBUG | MSG_FACILITY_OUTGOING,
//					_T("[BT] Sending BitTorrent tracker scrape: %s"), strURL );
//
//				if ( m_pRequest.Execute( FALSE ) && m_pRequest.InflateResponse() )
//				{
//					CBuffer* pResponse = m_pRequest.GetResponseBuffer();
//					if ( pResponse != NULL && pResponse->m_pBuffer != NULL )
//					{
//						if ( CBENode* pNode = CBENode::Decode( pResponse ) )
//						{
//							theApp.Message( MSG_DEBUG | MSG_FACILITY_INCOMING,
//								_T("[BT] Recieved BitTorrent tracker response: %s"), pNode->Encode() );
//
//							if ( oLock.Lock( 250 ) )
//							{
//								pDownload = pSheet->GetDownload();
//								if ( pDownload )
//								{
//									if ( OnTree( pDownload, pNode ) )
//									{
//										delete pNode;
//										PostMessage( WM_TIMER, 3 );
//										return;
//									}
//								}
//							}
//							delete pNode;
//						}
//					}
//				}
//			}
//		}
//	}
//
//	m_pRequest.Clear();
//	PostMessage( WM_TIMER, 2 );
//}

//BOOL CTorrentTrackersPage::OnTree(CDownload* pDownload, CBENode* pNode)
//{
//	ASSUME_LOCK( Transfers.m_pSection );
//
//	if ( ! pNode->IsType( CBENode::beDict ) ) return FALSE;
//
//	CBENode* pFiles = pNode->GetNode( "files" );
//	if ( ! pFiles->IsType( CBENode::beDict ) ) return FALSE;
//
//	LPBYTE nKey = &pDownload->m_pTorrent.m_oBTH[ 0 ];
//
//	CBENode* pFile = pFiles->GetNode( nKey, Hashes::BtHash::byteCount );
//	if ( ! pFile->IsType( CBENode::beDict ) ) return FALSE;
//
//	m_nComplete = 0;
//	m_nIncomplete = 0;
//
//	if ( CBENode* pComplete = pFile->GetNode( "complete" ) )
//	{
//		if ( ! pComplete->IsType( CBENode::beInt ) ) return FALSE;
//		// Since we read QWORDs, make sure we won't get negative values;
//		// Some buggy trackers send very huge numbers, so leave them as the max int.
//		m_nComplete = (int)( pComplete->GetInt() & ~0xFFFF0000 );
//	}
//
//	if ( CBENode* pIncomplete = pFile->GetNode( "incomplete" ) )
//	{
//		if ( ! pIncomplete->IsType( CBENode::beInt ) ) return FALSE;
//		m_nIncomplete = (int)( pIncomplete->GetInt() & ~0xFFFF0000 );
//	}
//
//	return TRUE;
//}

void CTorrentTrackersPage::OnEnChangeTorrentTracker()
{
	if ( ! UpdateData() )
		return;

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return;

	CDownload* pDownload = ((CDownloadSheet*)GetParent())->GetDownload();
	if ( ! pDownload )
		return;

	CBTInfo& oInfo = pDownload->m_pTorrent;

	if ( m_sTracker != oInfo.GetTrackerAddress() )
		m_wndTrackerMode.SetCurSel( CBTInfo::tSingle );
	else if ( oInfo.GetTrackerMode() != CBTInfo::tNull )
		m_wndTrackerMode.SetCurSel( oInfo.GetTrackerMode() );
}

void CTorrentTrackersPage::OnNMClickTorrentTrackers(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast< LPNMITEMACTIVATE >( pNMHDR );
	*pResult = 0;

	if ( ! UpdateData() )
		return;

	if ( pNMItemActivate->iItem != -1 )
	{
		m_sTracker = m_wndTrackers.GetItemText( pNMItemActivate->iItem, 0 );
		m_wndTrackerMode.SetCurSel( CBTInfo::tSingle );

		GetDlgItem( IDC_TORRENT_TRACKER )->EnableWindow( TRUE );

		UpdateData( FALSE );
	}
}

void CTorrentTrackersPage::OnCbnSelchangeTorrentTrackermode()
{
	const int nMode = m_wndTrackerMode.GetCurSel();

	GetDlgItem( IDC_TORRENT_TRACKER )->EnableWindow( nMode != CBTInfo::tNull );
}

void CTorrentTrackersPage::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if ( ! ::IsWindow( m_wndTrackers.GetSafeHwnd() ) ) return;

	if ( m_wndTrackers.GetBkColor() != Colors.m_crWindow ) return;	// Rarely needed (Remove this line when useful)

	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;

	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		if ( m_wndTrackers.GetBkColor() == Colors.m_crWindow )
			pDraw->clrTextBk = Colors.m_crWindow;

		*pResult = CDRF_DODEFAULT;
	}
}
