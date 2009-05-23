//
// PageTorrentGeneral.cpp
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

#include "ShellIcons.h"
#include "DlgDownloadSheet.h"
#include "PageTorrentFiles.h"
#include "Skin.h"
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
	m_wndFiles.EnableTips( pTip );

	CRect rc;
	m_wndFiles.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL );
	m_wndFiles.SetImageList( ShellIcons.GetObject( 16 ), LVSIL_SMALL );
	m_wndFiles.InsertColumn( 0, _T("Filename"), LVCFMT_LEFT, rc.right - 70 - 60 - 60, -1 );
	m_wndFiles.InsertColumn( 1, _T("Size"), LVCFMT_RIGHT, 70, 0 );
	m_wndFiles.InsertColumn( 2, _T("Status"), LVCFMT_RIGHT, 60, 0 );
	m_wndFiles.InsertColumn( 3, _T("Priority"), LVCFMT_RIGHT, 60, 0 );
	Skin.Translate( _T("CTorrentFileList"), m_wndFiles.GetHeaderCtrl() );

	BEGIN_COLUMN_MAP()
		COLUMN_MAP( CFragmentedFile::prNotWanted,	LoadString( IDS_PRIORITY_OFF ) )
		COLUMN_MAP( CFragmentedFile::prLow,			LoadString( IDS_PRIORITY_LOW ) )
		COLUMN_MAP( CFragmentedFile::prNormal,		LoadString( IDS_PRIORITY_NORMAL ) )
		COLUMN_MAP( CFragmentedFile::prHigh,		LoadString( IDS_PRIORITY_HIGH ) )
	END_COLUMN_MAP( m_wndFiles, 3 )

	if ( CComPtr< CFragmentedFile > pFragFile = pDownload->GetFile() )
	{
		for ( DWORD i = 0 ; i < pFragFile->GetCount() ; ++i )
		{
			LV_ITEM pItem = {};
			pItem.mask		= LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
			pItem.iItem		= i;
			pItem.lParam	= (LPARAM)pFragFile->GetAt( i );
			pItem.iImage	= ShellIcons.Get( pFragFile->GetName( i ), 16 );
			pItem.pszText	= (LPTSTR)(LPCTSTR)pFragFile->GetName( i );
			pItem.iItem		= m_wndFiles.InsertItem( &pItem );
			m_wndFiles.SetItemText( pItem.iItem, 1,
				Settings.SmartVolume( pFragFile->GetLength( i ) ) );
			m_wndFiles.SetColumnData( pItem.iItem, 3,
				pFragFile->GetPriority( i ) );
		}
	}

	Update();

	SetTimer( 1, 2000, NULL );

	return TRUE;
}

BOOL CTorrentFilesPage::OnApply()
{
	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return FALSE;

	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
	if ( ! Downloads.Check( pDownload ) || ! pDownload->IsTorrent() )
		return FALSE;

	if ( CComPtr< CFragmentedFile > pFragFile = pDownload->GetFile() )
	{
		for ( DWORD i = 0; i < pFragFile->GetCount(); ++i )
		{
			pFragFile->SetPriority( i, m_wndFiles.GetColumnData( i, 3 ) );
		}
	}

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
		for ( DWORD i = 0 ; i < pFragFile->GetCount() ; ++i )
		{
			CString sCompleted;
			float fProgress = pFragFile->GetProgress( i );
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

void CTorrentFilesPage::OnNMDblclkTorrentFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CSingleLock oLock( &Transfers.m_pSection, TRUE );
	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
	if ( Downloads.Check( pDownload ) )
		pDownload->Launch( pNMItemActivate->iItem, &oLock, FALSE );

	*pResult = 0;
}
