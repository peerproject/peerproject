//
// PageSettingsBitTorrent.cpp
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
#include "WndMain.h"
#include "PageSettingsBitTorrent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CBitTorrentSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CBitTorrentSettingsPage, CSettingsPage)
	ON_BN_CLICKED(IDC_TORRENT_AUTOCLEAR, OnTorrentsAutoClear)
	ON_BN_CLICKED(IDC_TORRENTS_BROWSE, OnTorrentsBrowse)
	ON_BN_CLICKED(IDC_TORRENTS_TORRENTMAKERBROWSE, OnMakerBrowse)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBitTorrentSettingsPage property page

CBitTorrentSettingsPage::CBitTorrentSettingsPage()
	: CSettingsPage(CBitTorrentSettingsPage::IDD)
	, m_nDownloads		( 0 )
	, m_nLinks			( 0 )
	, m_bEndGame		( FALSE )
	, m_bAutoClear		( FALSE )
	, m_nClearPercentage ( 0 )
	, m_bPrefBTSources	( TRUE )
	, m_sTracker		( _T("") )
	, m_sTorrentPath	( _T("") )
	, m_sMakerPath		( _T("") )
{
	//{{AFX_DATA_INIT(CBitTorrentSettingsPage)
	//}}AFX_DATA_INIT
}

CBitTorrentSettingsPage::~CBitTorrentSettingsPage()
{
}

void CBitTorrentSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBitTorrentSettingsPage)
	DDX_Check(pDX, IDC_TORRENT_ENDGAME, m_bEndGame);
	DDX_Text(pDX, IDC_TORRENT_CLIENTLINKS, m_nLinks);
	DDX_Control(pDX, IDC_TORRENT_LINKS_SPIN, m_wndLinksSpin);
	DDX_Text(pDX, IDC_TORRENT_DOWNLOADS, m_nDownloads);
	DDX_Control(pDX, IDC_TORRENT_DOWNLOADS_SPIN, m_wndDownloadsSpin);
	DDX_Check(pDX, IDC_TORRENT_AUTOCLEAR, m_bAutoClear);
	DDX_Control(pDX, IDC_TORRENT_CLEAR_PERCENTAGE, m_wndClearPercentage);
	DDX_Control(pDX, IDC_TORRENT_CLEAR_SPIN, m_wndClearPercentageSpin);
	DDX_Text(pDX, IDC_TORRENT_CLEAR_PERCENTAGE, m_nClearPercentage);
	DDX_Check(pDX, IDC_TORRENT_PREFERENCE, m_bPrefBTSources);
	DDX_Text(pDX, IDC_TORRENT_DEFAULTTRACKER, m_sTracker);
	DDX_Control(pDX, IDC_TORRENTS_BROWSE, m_wndTorrentPath);
	DDX_Text(pDX, IDC_TORRENTS_FOLDER, m_sTorrentPath);
	DDX_Control(pDX, IDC_TORRENTS_TORRENTMAKERBROWSE, m_wndMakerPath);
	DDX_Text(pDX, IDC_TORRENTS_TORRENTMAKER, m_sMakerPath);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CBitTorrentSettingsPage message handlers

BOOL CBitTorrentSettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();
	m_bEndGame			= Settings.BitTorrent.Endgame;
	m_nLinks			= Settings.BitTorrent.DownloadConnections;
	m_sTracker			= Settings.BitTorrent.DefaultTracker;
	m_sTorrentPath		= Settings.Downloads.TorrentPath;
	m_nDownloads		= Settings.BitTorrent.DownloadTorrents;
	m_sMakerPath		= Settings.BitTorrent.TorrentCreatorPath;
	m_bAutoClear		= Settings.BitTorrent.AutoClear;
	m_nClearPercentage	= Settings.BitTorrent.ClearRatio;
	m_bPrefBTSources	= Settings.BitTorrent.PreferenceBTSources;

	m_wndTorrentPath.SetIcon( IDI_BROWSE );
	m_wndMakerPath.SetIcon( IDI_BROWSE );

	m_wndClearPercentage.EnableWindow( m_bAutoClear );

	DWORD nMaxTorrents = ( Settings.GetOutgoingBandwidth() / 2 ) + 2;
	nMaxTorrents = min ( 10ul, nMaxTorrents);

	m_wndClearPercentageSpin.SetRange( 100, 999 );

	m_wndLinksSpin.SetRange( 0, 200 );
	m_wndDownloadsSpin.SetRange( 0, (WORD)nMaxTorrents );
	UpdateData( FALSE );

	m_wndTorrentFolder.SubclassDlgItem( IDC_TORRENTS_FOLDER, this );

	return TRUE;
}

BOOL CBitTorrentSettingsPage::OnSetActive()
{
	DWORD nMaxTorrents = ( Settings.GetOutgoingBandwidth() / 2 ) + 2;
	nMaxTorrents = min( 10ul, nMaxTorrents );

	m_nDownloads	= min( m_nDownloads, (int)nMaxTorrents );
	m_wndDownloadsSpin.SetRange( 0, (WORD)nMaxTorrents );

	UpdateData( FALSE );

	return CSettingsPage::OnSetActive();
}

void CBitTorrentSettingsPage::OnTorrentsAutoClear()
{
	UpdateData();
	m_wndClearPercentage.EnableWindow( m_bAutoClear );
	m_wndClearPercentageSpin.EnableWindow( m_bAutoClear );
}

void CBitTorrentSettingsPage::OnTorrentsBrowse()
{
	CString strPath( BrowseForFolder( _T("Select folder for torrents:"),
		m_sTorrentPath ) );
	if ( strPath.IsEmpty() )
		return;

	UpdateData( TRUE );
	m_sTorrentPath = strPath;
	UpdateData( FALSE );
}

void CBitTorrentSettingsPage::OnMakerBrowse()
{
	CFileDialog dlg( TRUE, _T("exe"), _T("TorrentWizard.exe") , OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		_T("Executable Files|*.exe|") + LoadString( IDS_FILES_ALL ) + _T("|*.*||"), this );

	if ( dlg.DoModal() != IDOK ) return;

	UpdateData( TRUE );
	m_sMakerPath = dlg.GetPathName();
	UpdateData( FALSE );
}

void CBitTorrentSettingsPage::OnOK()
{
	UpdateData( TRUE );

	m_nClearPercentage = min (m_nClearPercentage, 999);
	m_nClearPercentage = max (m_nClearPercentage, 100);

	// Guestimate a good value based on available bandwidth
	if ( Settings.GetOutgoingBandwidth() < 16 )
		m_nLinks = min ( m_nLinks, 200 );
	else if ( Settings.GetOutgoingBandwidth() < 32 )
		m_nLinks = min ( m_nLinks, 300 );
	else if ( Settings.GetOutgoingBandwidth() < 64 )
		m_nLinks = min ( m_nLinks, 500 );
	else
		m_nLinks = min ( m_nLinks, 800 );

	m_nDownloads = min( m_nDownloads, (int)( ( Settings.GetOutgoingBandwidth() / 2 ) + 2 ) );

	UpdateData( FALSE );

	Settings.BitTorrent.Endgame				= m_bEndGame != FALSE;
	Settings.BitTorrent.DownloadConnections	= m_nLinks;
	Settings.BitTorrent.DownloadTorrents	= m_nDownloads;
	Settings.BitTorrent.AutoClear			= m_bAutoClear != FALSE;
	Settings.BitTorrent.ClearRatio			= m_nClearPercentage;
	Settings.BitTorrent.PreferenceBTSources	= m_bPrefBTSources != FALSE;
	Settings.BitTorrent.DefaultTracker		= m_sTracker;
	Settings.Downloads.TorrentPath			= m_sTorrentPath;
	Settings.BitTorrent.TorrentCreatorPath	= m_sMakerPath;

	// Redraw the GUI if we need to (show/hide obsolete torrents box)
	//if ( bRedraw )
	//{
	//	CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();
	//	pMainWnd->SetGUIMode( Settings.General.GUIMode, FALSE );
	//}

	CSettingsPage::OnOK();
}