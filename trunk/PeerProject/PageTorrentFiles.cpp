//
// PageTorrentGeneral.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "ShellIcons.h"
#include "DlgDownloadSheet.h"
#include "PageTorrentFiles.h"
#include "Skin.h"
#include "LiveList.h"
#include "Transfers.h"
#include "Downloads.h"
#include "CtrlLibraryTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTorrentFilesPage, CPropertyPageAdv)

BEGIN_MESSAGE_MAP(CTorrentFilesPage, CPropertyPageAdv)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnSortColumn)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TORRENT_FILES, &CTorrentFilesPage::OnCheckbox)
	ON_NOTIFY(NM_DBLCLK, IDC_TORRENT_FILES, &CTorrentFilesPage::OnNMDblclkTorrentFiles)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTorrentFilesPage property page

CTorrentFilesPage::CTorrentFilesPage() :
	CPropertyPageAdv( CTorrentFilesPage::IDD )
{
}

CTorrentFilesPage::~CTorrentFilesPage()
{
}

void CTorrentFilesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageAdv::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TORRENT_FILES, m_wndFiles);
}

/////////////////////////////////////////////////////////////////////////////
// CTorrentFilesPage message handlers

BOOL CTorrentFilesPage::OnInitDialog()
{
	if ( ! CPropertyPageAdv::OnInitDialog() )
		return FALSE;

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return FALSE;

	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
	if ( ! Downloads.Check( pDownload ) || ! pDownload->IsTorrent() )
		return FALSE;

	auto_ptr< CCoolTipCtrl > pTip( new CLibraryTipCtrl );
	pTip->Create( this, &Settings.Interface.TipDownloads );
//	m_wndFiles.EnableTips( pTip );

	CRect rc;
	m_wndFiles.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL );
	m_wndFiles.SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP|LVS_EX_CHECKBOXES );
	m_wndFiles.SetImageList( ShellIcons.GetObject( 16 ), LVSIL_SMALL );
	m_wndFiles.InsertColumn( 0, _T("Filename"), LVCFMT_LEFT, rc.right - 66 - 54, -1 );
	m_wndFiles.InsertColumn( 1, _T("Size"), LVCFMT_RIGHT, 66, 0 );
	m_wndFiles.InsertColumn( 2, _T("Status"), LVCFMT_RIGHT, 54, 0 );
	m_wndFiles.InsertColumn( 3, _T("Index"), LVCFMT_CENTER, 0, 0 );
//	m_wndFiles.InsertColumn( 4, _T("Priority"), LVCFMT_RIGHT, 52, 0 );
	Skin.Translate( _T("CTorrentFileList"), m_wndFiles.GetHeaderCtrl() );

// Priority Column Combobox:
//	BEGIN_COLUMN_MAP()
//		COLUMN_MAP( CFragmentedFile::prHigh,		LoadString( IDS_PRIORITY_HIGH ) )
//		COLUMN_MAP( CFragmentedFile::prNormal,		LoadString( IDS_PRIORITY_NORMAL ) )
//		COLUMN_MAP( CFragmentedFile::prLow,			LoadString( IDS_PRIORITY_LOW ) )
//		COLUMN_MAP( CFragmentedFile::prDiscarded,	LoadString( IDS_PRIORITY_OFF ) )
//	END_COLUMN_MAP( m_wndFiles, 4 )

	if ( CComPtr< CFragmentedFile > pFragFile = pDownload->GetFile() )
	{
		if ( pDownload->IsSeeding() || pFragFile->GetCount() < 2  )
			m_wndFiles.SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP );	// No checkboxes needed
		if ( pFragFile->GetCount() < 2 )
			m_wndFiles.DeleteColumn( 3 );

		for ( DWORD i = 0 ; i < pFragFile->GetCount() ; ++i )
		{
			LV_ITEM pItem = {};
			pItem.mask		= LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
			pItem.iItem		= i;
			CString sText	= pFragFile->GetName( i );
			pItem.lParam	= (LPARAM)pFragFile->GetAt( i );
			pItem.iImage	= ShellIcons.Get( sText, 16 );
			pItem.pszText	= (LPTSTR)(LPCTSTR)sText.Mid( sText.Find( '\\' ) + 1 );
			pItem.iItem		= m_wndFiles.InsertItem( &pItem );
			m_wndFiles.SetItemText( pItem.iItem, 1,
				Settings.SmartVolume( pFragFile->GetLength( i ) ) );
			sText.Format( _T("%i"), i );
			m_wndFiles.SetItemText( pItem.iItem, 3, sText );
			m_wndFiles.SetItemState( i,
				UINT( ( pFragFile->GetPriority( i ) == CFragmentedFile::prDiscarded ? 1 : 2 ) << 12 ), LVIS_STATEIMAGEMASK );
		//Priority Column:
		//	m_wndFiles.SetColumnData( pItem.iItem, 4, pFragFile->GetPriority( i ) );
		}
	}

	Update();

	SetTimer( 1, 100, NULL );	// Rapid Refresh (10 chances per second)

	return TRUE;
}

void CTorrentFilesPage::OnCheckbox(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	BOOL bPrevState = (BOOL)( ( ( pNMListView->uOldState & LVIS_STATEIMAGEMASK) >> 12 ) - 1 );
	if ( bPrevState < 0 )	// No previous state at startup 
		return;

	BOOL bChecked = (BOOL)( ( ( pNMListView->uNewState & LVIS_STATEIMAGEMASK ) >> 12 ) - 1 );
	if ( bChecked < 0 ) 	// Non-checkbox notifications
		return;

	if ( bChecked == bPrevState )	// No change
		return;

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 500 ) )
		return;

	CString strIndex = m_wndFiles.GetItemText( pNMListView->iItem, 3 );

	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;

	CComPtr< CFragmentedFile > pFragFile = pDownload->GetFile();

	pFragFile->SetPriority( /*pNMListView->iItem*/ _wtoi( strIndex ),
		bChecked ? CFragmentedFile::prNormal : CFragmentedFile::prDiscarded );

	oLock.Unlock();

	// Multiple highlighted items group handling
	if ( m_wndFiles.GetItemState( pNMListView->iItem, LVIS_SELECTED ) )
	{
		int nItem = -1;
		while ( ( nItem = m_wndFiles.GetNextItem( nItem, LVNI_SELECTED ) ) > -1 )
		{
			if ( m_wndFiles.GetCheck(nItem) != bChecked )
			{
				strIndex = m_wndFiles.GetItemText( nItem, 3 );
				pFragFile->SetPriority( _wtoi( strIndex ), bChecked ? CFragmentedFile::prNormal : CFragmentedFile::prDiscarded );
				m_wndFiles.SetCheck(nItem, bChecked ? BST_CHECKED : BST_UNCHECKED );
			}
		}
	}
}

void CTorrentFilesPage::OnSortColumn(NMHDR* pNotifyStruct, LRESULT* /*pResult*/)
{
	if ( m_wndFiles.GetItemCount() < 2 ) return;

	HD_NOTIFY *pHDN = (HD_NOTIFY *)pNotifyStruct;

	if( pHDN->iButton != 0 ) return;	// Verify header clicked with left mouse button

	CLiveList::Sort( &m_wndFiles, pHDN->iItem, FALSE );
}

void CTorrentFilesPage::OnNMDblclkTorrentFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CString strIndex = m_wndFiles.GetItemText( pNMItemActivate->iItem, 3 );

	CSingleLock oLock( &Transfers.m_pSection, TRUE );
	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
	if ( Downloads.Check( pDownload ) )
		pDownload->Launch( _wtoi( strIndex ), &oLock, FALSE );

	*pResult = 0;
}

BOOL CTorrentFilesPage::OnApply()
{
// Unused Priority Column:

//	CSingleLock oLock( &Transfers.m_pSection );
//	if ( ! oLock.Lock( 250 ) )
//		return FALSE;

//	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
//	if ( ! Downloads.Check( pDownload ) || ! pDownload->IsTorrent() )
//		return FALSE;

//	if ( CComPtr< CFragmentedFile > pFragFile = pDownload->GetFile() )
//	{
//		for ( DWORD i = 0; i < pFragFile->GetCount(); ++i )
//		{
//			pFragFile->SetPriority( i, m_wndFiles.GetColumnData( i, 3 ) );
//		}
//	}

	return CPropertyPageAdv::OnApply();
}

void CTorrentFilesPage::OnTimer(UINT_PTR nIDEvent)
{
	CPropertyPageAdv::OnTimer( nIDEvent );

	if ( static_cast< CPropertySheet* >( GetParent() )->GetActivePage() == this )
	{
		Update();
	}
}

void CTorrentFilesPage::Update()
{
	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return;

	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
	if ( ! Downloads.Check( pDownload ) || ! pDownload->IsTorrent() )
		return;

	if ( CComPtr< CFragmentedFile > pFragFile = pDownload->GetFile() )
	{
		CString sIndex, sCompleted;
		UINT nIndex;

		for ( DWORD i = 0 ; i < pFragFile->GetCount() ; ++i )
		{
			sIndex = m_wndFiles.GetItemText( i, 3 );
			nIndex = _wtoi( sIndex );

			float fProgress = pFragFile->GetProgress( nIndex );
			if ( fProgress >= 0.0 )
				sCompleted.Format( _T("%.2f%%"), fProgress );

			m_wndFiles.SetItemText( i, 2, sCompleted );
		}
	}
}

void CTorrentFilesPage::OnDestroy()
{
	KillTimer( 1 );

	CPropertyPageAdv::OnDestroy();
}