//
// DlgURLAction.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "PeerProjectURL.h"
#include "DlgURLAction.h"
#include "DlgExistingFile.h"
#include "Download.h"
#include "Downloads.h"
#include "Transfers.h"
#include "Network.h"
#include "Library.h"
#include "SharedFile.h"
#include "HostCache.h"
#include "WndMain.h"
#include "WndSearch.h"
#include "WndDownloads.h"
#include "WndBrowseHost.h"
#include "DiscoveryServices.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CURLActionDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CURLActionDlg, CSkinDialog)
	ON_BN_CLICKED(IDC_URL_DOWNLOAD, OnUrlDownload)
	ON_BN_CLICKED(IDC_URL_SEARCH, OnUrlSearch)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CURLActionDlg construction

CURLActionDlg::CURLActionDlg(CPeerProjectURL* pURL)
	: CSkinDialog( CURLActionDlg::IDD )
	, m_bNewWindow	( FALSE )
	, m_bAlwaysOpen	( FALSE )
{
	if ( pURL )
	{
		m_pURL = pURL;
		Create( CURLActionDlg::IDD );
		ShowWindow( SW_SHOW );
	}
}

CURLActionDlg::~CURLActionDlg()
{
	delete m_pURL;
}

void CURLActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_wndCancel);
	DDX_Control(pDX, IDC_NEW_WINDOW, m_wndNewWindow);
	DDX_Control(pDX, IDC_MESSAGE_1, m_wndMessage1);
	DDX_Control(pDX, IDC_MESSAGE_2, m_wndMessage2);
	DDX_Control(pDX, IDC_MESSAGE_3, m_wndMessage3);
	DDX_Control(pDX, IDC_MESSAGE_4, m_wndMessage4);
	DDX_Control(pDX, IDC_URL_SEARCH, m_wndSearch);
	DDX_Control(pDX, IDC_URL_DOWNLOAD, m_wndDownload);
	DDX_Text(pDX, IDC_URL_NAME_TITLE, m_sNameTitle);
	DDX_Text(pDX, IDC_URL_NAME_VALUE, m_sNameValue);
	DDX_Text(pDX, IDC_URL_URN_TITLE, m_sHashTitle);
	DDX_Text(pDX, IDC_URL_URN_VALUE, m_sHashValue);
	DDX_Check(pDX, IDC_NEW_WINDOW, m_bNewWindow);
	DDX_Check(pDX, IDC_ALWAYS_OPEN, m_bAlwaysOpen);
}

/////////////////////////////////////////////////////////////////////////////
// CURLActionDlg message handlers

BOOL CURLActionDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CURLActionDlg"), IDR_MAINFRAME );

	m_bAlwaysOpen = Settings.General.AlwaysOpenURLs;
	m_bNewWindow  = Settings.Downloads.ShowMonitorURLs;

	CString strMessage;
	const BOOL bGoodURL =
		( m_pURL->m_sName.GetLength() > 10 && m_pURL->m_sName.Find( '.', 5 ) > 5 && m_pURL->m_sName[ m_pURL->m_sName.GetLength() - 1 ] > '.' ) ||
		( m_pURL->m_sURL.GetLength() > 8 && m_pURL->m_sURL.Find( '.' ) > 1 && m_pURL->m_sURL[ m_pURL->m_sURL.GetLength() - 1 ] > '.' ) ||
		( m_pURL->m_oSHA1 || m_pURL->m_oTiger || m_pURL->m_oBTH || m_pURL->m_oED2K || m_pURL->m_oMD5 ) ||
		( m_pURL->m_nAction == CPeerProjectURL::uriSearch );

	if ( ! bGoodURL )
	{
		// Malformed?
		ShowWindow( SW_HIDE );
		DestroyWindow();
		return TRUE;
	}

	if ( m_pURL->m_nAction == CPeerProjectURL::uriHost )
	{
		LoadString( m_sNameTitle, IDS_URL_HOST );
		LoadString( m_sHashTitle, IDS_URL_PORT );

		m_sNameValue = m_pURL->m_sName;
		m_sHashValue.Format( _T("%lu"), m_pURL->m_nPort );

		m_wndMessage2.ShowWindow( SW_SHOW );
		m_wndNewWindow.ShowWindow( SW_HIDE );

		LoadString( strMessage, IDS_URL_CONNECT );
		m_wndDownload.SetWindowText( strMessage );
		m_wndDownload.SetFocus();

		if ( m_pURL->m_nProtocol != PROTOCOL_ED2K &&
			 m_pURL->m_nProtocol != PROTOCOL_BT &&
			 m_pURL->m_nProtocol != PROTOCOL_KAD )
		{
			LoadString( strMessage, IDS_URL_BROWSE );
			m_wndSearch.SetWindowText( strMessage );
		}
		else
		{
			m_wndSearch.ShowWindow( SW_HIDE );
		}
	}
	else if ( m_pURL->m_nAction == CPeerProjectURL::uriBrowse )
	{
		LoadString( m_sNameTitle, IDS_URL_HOST );
		LoadString( m_sHashTitle, IDS_URL_PORT );

		m_sNameValue = m_pURL->m_sName;
		m_sHashValue.Format( _T("%lu"), m_pURL->m_nPort );

		m_wndMessage3.ShowWindow( SW_SHOW );
		m_wndNewWindow.ShowWindow( SW_HIDE );

		LoadString( strMessage, IDS_URL_BROWSE );
		m_wndDownload.SetWindowText( strMessage );
		m_wndDownload.SetFocus();
		LoadString( strMessage, IDS_URL_CONNECT );
		m_wndSearch.SetWindowText( strMessage );
	}
	else if ( m_pURL->m_nAction == CPeerProjectURL::uriDiscovery )
	{
		LoadString( m_sNameTitle, IDS_URL_URL );
		LoadString( m_sHashTitle, IDS_URL_TYPE );

		m_sNameValue = m_pURL->m_sURL;

		switch ( m_pURL->m_nSize )
		{
		case CDiscoveryService::dsWebCache:
			m_sHashValue = _T("GWebCache");
			break;
		case CDiscoveryService::dsServerList:
			m_sHashValue = m_pURL->m_nProtocol == PROTOCOL_DC ?
				_T("Hublist URL") :_T("Server.met URL");
			break;
		}

		m_wndMessage4.ShowWindow( SW_SHOW );
		LoadString( strMessage, IDS_URL_ADD );
		m_wndDownload.SetWindowText( strMessage );
		m_wndSearch.ShowWindow( SW_HIDE );
		m_wndNewWindow.ShowWindow( SW_HIDE );
	}
	else if ( m_pURL->m_nAction == CPeerProjectURL::uriSource )
	{
		LoadString( m_sNameTitle, IDS_URL_URL );

		m_sNameValue = m_pURL->m_sURL;

		m_wndMessage1.ShowWindow( SW_SHOW );
		m_wndSearch.ShowWindow( SW_HIDE );
	}
	else
	{
		LoadString( m_sNameTitle, IDS_URL_FILENAME );
		m_sHashTitle = _T("URN:");

		if ( ! m_pURL->m_sName.IsEmpty() )
		{
			m_sNameValue = m_pURL->m_sName;

			if ( m_pURL->m_bSize )
				m_sNameValue += _T(" (") + Settings.SmartVolume( m_pURL->m_nSize ) + _T(")");
		}
		else
		{
			LoadString( m_sNameValue, IDS_URL_UNSPECIFIED );
		}

		if ( m_pURL->m_oTiger && m_pURL->m_oSHA1 )
			m_sHashValue = _T("bitprint:") + m_pURL->m_oSHA1.toString() + _T(".") + m_pURL->m_oTiger.toString();
		else if ( m_pURL->m_oTiger )
			m_sHashValue = m_pURL->m_oTiger.toShortUrn();
		else if ( m_pURL->m_oSHA1 )
			m_sHashValue = m_pURL->m_oSHA1.toShortUrn();
		else if ( m_pURL->m_oBTH )
			m_sHashValue = m_pURL->m_oBTH.toShortUrn();
		else if ( m_pURL->m_oED2K )
			m_sHashValue = m_pURL->m_oED2K.toShortUrn();
		else if ( m_pURL->m_oMD5 )
			m_sHashValue = m_pURL->m_oMD5.toShortUrn();
		else
		{
			LoadString( m_sHashValue, IDS_URL_UNSPECIFIED );

			if ( StartsWith( m_pURL->m_sURL, _T("http"), 4 ) )
				m_sHashValue += _T("  (HTTP)");
			else if ( StartsWith( m_pURL->m_sURL, _T("ftp"), 4 ) )
				m_sHashValue += _T("  (FTP)");
		}

		m_wndMessage1.ShowWindow( SW_SHOW );

		if ( m_pURL->m_nAction == CPeerProjectURL::uriDownload )
		{
			m_wndDownload.SetFocus();
		}
		else if ( m_pURL->m_nAction == CPeerProjectURL::uriSearch )
		{
			m_wndDownload.EnableWindow( FALSE );
			m_wndDownload.ModifyStyle( BS_DEFPUSHBUTTON, 0 );
			m_wndSearch.ModifyStyle( 0, BS_DEFPUSHBUTTON );
			m_wndSearch.SetFocus();
			m_wndNewWindow.ShowWindow( SW_HIDE );
		}

		//if ( m_pURL->m_oBTH )
		//	PostMessage( WM_COMMAND, IDC_URL_DOWNLOAD );
	}

	UpdateData( FALSE );

	if ( m_bAlwaysOpen )
	{
		if ( m_wndDownload.IsWindowEnabled() )
			PostMessage( WM_COMMAND, IDC_URL_DOWNLOAD );
		else
			PostMessage( WM_COMMAND, IDC_URL_SEARCH );
	}

	return FALSE;
}

BOOL CURLActionDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		if ( GetFocus() == &m_wndCancel )
			PostMessage( WM_COMMAND, IDCANCEL );
		else if ( m_wndDownload.IsWindowEnabled() )
			PostMessage( WM_COMMAND, IDC_URL_DOWNLOAD );
		else if ( m_wndSearch.IsWindowEnabled() )
			PostMessage( WM_COMMAND, IDC_URL_SEARCH );
		return TRUE;
	}

	return CSkinDialog::PreTranslateMessage( pMsg );
}

void CURLActionDlg::OnUrlDownload()
{
	UpdateData();

	Settings.General.AlwaysOpenURLs		= m_bAlwaysOpen != FALSE;
	Settings.Downloads.ShowMonitorURLs	= m_bNewWindow != FALSE;

	switch ( m_pURL->m_nAction )
	{
	case CPeerProjectURL::uriDownload:
	case CPeerProjectURL::uriSource:
		{
			CExistingFileDlg::Action action = CExistingFileDlg::CheckExisting( m_pURL );
			if ( action == CExistingFileDlg::Cancel )
				return;

			if ( action != CExistingFileDlg::Download )
			{
				DestroyWindow();
				return;
			}

			CDownload* pDownload = Downloads.Add( *m_pURL );
			if ( ! pDownload )
			{
				DestroyWindow();
				return;
			}

			if ( ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) == 0 &&
				! Network.IsWellConnected() )
			{
				Network.Connect( TRUE );
			}

			CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();
			pMainWnd->m_pWindows.Open( RUNTIME_CLASS(CDownloadsWnd) );

			if ( Settings.Downloads.ShowMonitorURLs )
			{
				CSingleLock pLock( &Transfers.m_pSection, TRUE );
				if ( Downloads.Check( pDownload ) )
					pDownload->ShowMonitor( &pLock );
			}
		}
		break;
	case CPeerProjectURL::uriBrowse:
		{
			SOCKADDR_IN pAddress;
			if ( Network.Resolve( m_pURL->m_sName, m_pURL->m_nPort, &pAddress ) )
				new CBrowseHostWnd( m_pURL->m_nProtocol, &pAddress );
		}
		break;
	case CPeerProjectURL::uriHost:
		Network.ConnectTo( m_pURL->m_sName, m_pURL->m_nPort, m_pURL->m_nProtocol );
		break;
	case CPeerProjectURL::uriDiscovery:
		DiscoveryServices.Add( m_pURL->m_sURL, (int)m_pURL->m_nSize );
		break;
	//case CPeerProjectURL::uriSearch:
	//default:
	//	ASSERT( ! m_pURL->m_nAction );
	}

	DestroyWindow();
}

void CURLActionDlg::OnUrlSearch()
{
	Settings.General.AlwaysOpenURLs = m_bAlwaysOpen != FALSE;

	if ( m_pURL->m_nAction == CPeerProjectURL::uriHost )
	{
		SOCKADDR_IN pAddress;
		if ( Network.Resolve( m_pURL->m_sName, m_pURL->m_nPort, &pAddress ) )
			new CBrowseHostWnd( m_pURL->m_nProtocol, &pAddress );
	}
	else if ( m_pURL->m_nAction == CPeerProjectURL::uriBrowse )
	{
		Network.ConnectTo( m_pURL->m_sName, m_pURL->m_nPort );
	}
	else if ( m_pURL->m_nAction == CPeerProjectURL::uriDownload ||
			  m_pURL->m_nAction == CPeerProjectURL::uriSearch )
	{
		if ( ! Network.IsWellConnected() )
			Network.Connect( TRUE );

		new CSearchWnd( m_pURL->ToQuery() );
	}

	DestroyWindow();
}

void CURLActionDlg::OnCancel()
{
	DestroyWindow();
}

void CURLActionDlg::PostNcDestroy()
{
	delete this;
}
