//
// DlgDownloadSheet.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Shareaza is distributed in the hope that it will be useful,
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
#include "DlgDownloadSheet.h"

#include "PageDownloadEdit.h"
#include "PageDownloadActions.h"
#include "PageTorrentGeneral.h"
#include "PageTorrentFiles.h"
#include "PageTorrentTrackers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDownloadSheet, CPropertySheetAdv)

BEGIN_MESSAGE_MAP(CDownloadSheet, CPropertySheetAdv)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDownloadSheet

CDownloadSheet::CDownloadSheet(CDownload* pDownload) :
	m_pDownload( pDownload ),
	m_sFilesTitle( L"Files" ),
	m_sTrackersTitle( L"Trackers" ),
	m_sGeneralTitle( L"Torrent" ),
	m_sDownloadTitle( L"General" ),
	m_sActionsTitle( L"Actions" )
{
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadSheet operations

INT_PTR CDownloadSheet::DoModal(int nPage)
{
	CTorrentFilesPage		pFiles;
	CTorrentTrackersPage	pTrackers;
	CTorrentGeneralPage		pGeneral;
	CDownloadEditPage		pDownload;
	CDownloadActionsPage	pActions;

	if ( m_pDownload->IsTorrent() )
	{
		SetTabTitle( &pFiles, m_sFilesTitle );
		AddPage( &pFiles );
		SetTabTitle( &pTrackers, m_sTrackersTitle );
		AddPage( &pTrackers );
		SetTabTitle( &pGeneral, m_sGeneralTitle );
		AddPage( &pGeneral );
	}

	if ( ! m_pDownload->IsMoving() && ! m_pDownload->IsCompleted() )
	{
		SetTabTitle( &pDownload, m_sDownloadTitle );
		AddPage( &pDownload );
		SetTabTitle( &pActions, m_sActionsTitle );
		AddPage( &pActions );
	}

	m_psh.nStartPage = nPage;

	return CPropertySheetAdv::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadSheet message handlers

BOOL CDownloadSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheetAdv::OnInitDialog();

	SetFont( &theApp.m_gdiFont );
	SetIcon( theApp.LoadIcon( IDI_PROPERTIES ), TRUE );

	CString strCaption;
	LoadString( strCaption, IDS_DOWNLOAD_PROPERTIES );
	SetWindowText( strCaption );

	if ( GetDlgItem( IDOK ) )
	{
		CRect rc;
		GetDlgItem( IDOK )->GetWindowRect( &rc );
		ScreenToClient( &rc );
		GetDlgItem( IDOK )->SetWindowPos( NULL, 6, rc.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE );
		GetDlgItem( IDCANCEL )->SetWindowPos( NULL, 11 + rc.Width(), rc.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE );
	}

	if ( GetDlgItem( 0x3021 ) ) GetDlgItem( 0x3021 )->ShowWindow( SW_HIDE );
	if ( GetDlgItem( 0x0009 ) ) GetDlgItem( 0x0009 )->ShowWindow( SW_HIDE );

	return bResult;
}
