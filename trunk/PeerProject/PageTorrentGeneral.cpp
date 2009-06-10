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

#include "DlgDownloadSheet.h"
#include "PageTorrentGeneral.h"
#include "Transfers.h"
#include "Downloads.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTorrentGeneralPage, CPropertyPageAdv)

BEGIN_MESSAGE_MAP(CTorrentGeneralPage, CPropertyPageAdv)
	ON_WM_PAINT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTorrentGeneralPage property page

CTorrentGeneralPage::CTorrentGeneralPage() :
	CPropertyPageAdv( CTorrentGeneralPage::IDD )
{
}

CTorrentGeneralPage::~CTorrentGeneralPage()
{
}

void CTorrentGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageAdv::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTorrentGeneralPage)
	DDX_Text(pDX, IDC_TORRENT_NAME, m_sName);
	DDX_Text(pDX, IDC_TORRENT_COMMENTS, m_sComment);
	DDX_Text(pDX, IDC_TORRENT_CREATEDBY, m_sCreatedBy );
	DDX_Text(pDX, IDC_TORRENT_CREATIONDATE, m_sCreationDate );
	DDX_Text(pDX, IDC_TORRENT_OTHER, m_sTorrentOther );
	DDX_Control(pDX, IDC_TORRENT_STARTDOWNLOADS, m_wndStartDownloads);
	DDX_Text(pDX, IDC_TORRENT_UPLOADTOTAL, m_sUploadTotal );
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTorrentGeneralPage message handlers

BOOL CTorrentGeneralPage::OnInitDialog()
{
	if ( ! CPropertyPageAdv::OnInitDialog() )
		return FALSE;

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return FALSE;

	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
	if ( ! Downloads.Check( pDownload ) || ! pDownload->IsTorrent() )
		return FALSE;

	CBTInfo& oInfo = pDownload->m_pTorrent;

	m_sName			= oInfo.m_sName;
	m_sComment		= oInfo.m_sComment;
	m_sCreatedBy	= oInfo.m_sCreatedBy;
	if ( oInfo.m_tCreationDate > 0 )
	{
		CTime pTime( (time_t)oInfo.m_tCreationDate );
		m_sCreationDate = pTime.Format( _T("%Y-%m-%d  %H:%M") );
	}

	// Assembler 'other' string
	if ( oInfo.m_bPrivate )
	{
		CString str;
		LoadString( str, IDS_BT_PRIVATE );
		m_sTorrentOther += str;
		m_sTorrentOther += _T(", ");
	}
	if ( oInfo.HasEncodingError() )
	{
		CString str;
		LoadString( str, IDS_BT_ENCODING );
		m_sTorrentOther += str;
		m_sTorrentOther += _T(", ");
	}

	// Cut off last comma
	if ( m_sTorrentOther.GetLength() )
		m_sTorrentOther = m_sTorrentOther.Left( m_sTorrentOther.GetLength() - 2 );

	m_wndStartDownloads.SetCurSel( oInfo.m_nStartDownloads );

	m_sUploadTotal.Format( _T(" %s"),
		(LPCTSTR)Settings.SmartVolume( oInfo.m_nTotalUpload ) );

	UpdateData( FALSE );

	return TRUE;
}

BOOL CTorrentGeneralPage::OnApply()
{
	if ( ! UpdateData() )
		return FALSE;

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return FALSE;

	CDownload* pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
	if ( ! Downloads.Check( pDownload ) || ! pDownload->IsTorrent() )
		return FALSE;

	CBTInfo& oInfo = pDownload->m_pTorrent;

	// Update the starting of torrent transfers
	oInfo.m_nStartDownloads = m_wndStartDownloads.GetCurSel();

	return CPropertyPageAdv::OnApply();
}
