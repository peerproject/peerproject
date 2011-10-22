//
// PageSettingsDownloads.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
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
#include "PageSettingsDownloads.h"
#include "Library.h"
#include "LibraryFolders.h"
#include "SharedFolder.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CDownloadsSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CDownloadsSettingsPage, CSettingsPage)
	ON_BN_CLICKED(IDC_DOWNLOADS_BROWSE, OnDownloadsBrowse)
	ON_BN_CLICKED(IDC_INCOMPLETE_BROWSE, OnIncompleteBrowse)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDownloadsSettingsPage property page

CDownloadsSettingsPage::CDownloadsSettingsPage()
	: CSettingsPage(CDownloadsSettingsPage::IDD)
	, m_nMaxDownFiles		( 0 )
	, m_nMaxFileTransfers	( 0 )
	, m_nMaxDownTransfers	( 0 )
	, m_bRequireConnect 	( FALSE )
{
}

CDownloadsSettingsPage::~CDownloadsSettingsPage()
{
}

void CDownloadsSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MAX_TRANSFERS_SPIN, m_wndMaxDownTransfers);
	DDX_Control(pDX, IDC_MAX_TPF_SPIN, m_wndMaxFileTransfers);
	DDX_Control(pDX, IDC_MAX_FILES_SPIN, m_wndMaxDownFiles);
	DDX_Control(pDX, IDC_INCOMPLETE_BROWSE, m_wndIncompletePath);
	DDX_Control(pDX, IDC_DOWNLOADS_BROWSE, m_wndDownloadsPath);
	DDX_Control(pDX, IDC_DOWNLOADS_BANDWIDTH_LIMIT, m_wndBandwidthLimit);
	DDX_Control(pDX, IDC_DOWNLOADS_QUEUE_LIMIT, m_wndQueueLimit);
	DDX_Text(pDX, IDC_DOWNLOADS_FOLDER, m_sDownloadsPath);
	DDX_Text(pDX, IDC_INCOMPLETE_FOLDER, m_sIncompletePath);
	DDX_Text(pDX, IDC_MAX_FILES, m_nMaxDownFiles);
	DDX_Text(pDX, IDC_MAX_TPF, m_nMaxFileTransfers);
	DDX_Text(pDX, IDC_MAX_TRANSFERS, m_nMaxDownTransfers);
	DDX_CBString(pDX, IDC_DOWNLOADS_BANDWIDTH_LIMIT, m_sBandwidthLimit);
	DDX_CBString(pDX, IDC_DOWNLOADS_QUEUE_LIMIT, m_sQueueLimit);
	DDX_Check(pDX, IDC_REQUIRE_CONNECT, m_bRequireConnect);
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadsSettingsPage message handlers

BOOL CDownloadsSettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_sDownloadsPath		= Settings.Downloads.CompletePath;
	m_sIncompletePath		= Settings.Downloads.IncompletePath;
	m_nMaxDownFiles			= Settings.Downloads.MaxFiles;
	m_nMaxDownTransfers		= Settings.Downloads.MaxTransfers;
	m_nMaxFileTransfers		= Settings.Downloads.MaxFileTransfers;
	m_bRequireConnect		= Settings.Connection.RequireForTransfers;

	Settings.SetRange( &Settings.Downloads.MaxFiles, m_wndMaxDownFiles );
	Settings.SetRange( &Settings.Downloads.MaxTransfers, m_wndMaxDownTransfers );
	Settings.SetRange( &Settings.Downloads.MaxFileTransfers, m_wndMaxFileTransfers );

	m_wndDownloadsPath.SetIcon( IDI_BROWSE );
	m_wndIncompletePath.SetIcon( IDI_BROWSE );

	if ( Settings.Downloads.QueueLimit )
		m_sQueueLimit.Format( _T("%d"), Settings.Downloads.QueueLimit );
	else	// 0 = Unlimited
		m_sQueueLimit = _T("MAX");

	m_bDownloadsChanged = FALSE;

	// Update the text in the bandwidth limit combo
	if ( Settings.Bandwidth.Downloads )
		m_sBandwidthLimit = Settings.SmartSpeed( Settings.Bandwidth.Downloads );
	else	// 0 = Unlimited
		m_sBandwidthLimit = _T("MAX");

	UpdateData( FALSE );

	m_wndDownloadsFolder.SubclassDlgItem( IDC_DOWNLOADS_FOLDER, this );
	m_wndIncompleteFolder.SubclassDlgItem( IDC_INCOMPLETE_FOLDER, this );

	return TRUE;
}

void CDownloadsSettingsPage::OnDownloadsBrowse()
{
	CString strPath( BrowseForFolder( _T("Select folder for downloads:"),
		m_sDownloadsPath ) );
	if ( strPath.IsEmpty() )
		return;

	// Warn user about a path that's too long
	if ( _tcslen( strPath ) > MAX_PATH - 33 )
	{
		CString strMessage;
		LoadString( strMessage, IDS_SETTINGS_FILEPATH_TOO_LONG );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		return;
	}

	// Make sure download/incomplete folders aren't the same
	if ( _tcsicmp( strPath, m_sIncompletePath ) == 0 )
	{
		CString strMessage;
		LoadString( strMessage, IDS_SETTINGS_FILEPATH_NOT_SAME );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		return;
	}

	UpdateData( TRUE );
	m_sDownloadsPath = strPath;
	m_bDownloadsChanged = TRUE;
	UpdateData( FALSE );
}

void CDownloadsSettingsPage::OnIncompleteBrowse()
{
	CString strPath( BrowseForFolder( _T("Select folder for incomplete files:"),
		m_sIncompletePath ) );
	if ( strPath.IsEmpty() )
		return;

	// Warn user about a path that's too long
	if ( _tcslen( strPath ) > MAX_PATH - 60 )
	{
		CString strMessage;
		LoadString( strMessage, IDS_SETTINGS_FILEPATH_TOO_LONG );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		return;
	}

	// Make sure download/incomplete folders aren't the same
	if ( _tcsicmp( strPath, m_sDownloadsPath ) == 0 )
	{
		CString strMessage;
		LoadString( strMessage, IDS_SETTINGS_FILEPATH_NOT_SAME );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		return;
	}

	// Warn user about an incomplete folder in the library
	if ( LibraryFolders.IsFolderShared( strPath ) )
	{
		CString strMessage;
		LoadString( strMessage, IDS_SETTINGS_INCOMPLETE_LIBRARY );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		return;
	}

	UpdateData( TRUE );
	m_sIncompletePath = strPath;
	UpdateData( FALSE );
}

BOOL CDownloadsSettingsPage::OnKillActive()
{
	UpdateData();

	if ( IsLimited( m_sBandwidthLimit ) && ! Settings.ParseVolume( m_sBandwidthLimit ) )
	{
		CString strMessage;
		LoadString( strMessage, IDS_SETTINGS_NEED_BANDWIDTH );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		GetDlgItem( IDC_DOWNLOADS_BANDWIDTH_LIMIT )->SetFocus();
		return FALSE;
	}

	return CSettingsPage::OnKillActive();
}

void CDownloadsSettingsPage::OnOK()
{
	DWORD nQueueLimit = 0;
	UpdateData( TRUE );

	// Figure out what the text in the queue limit box means
	if ( IsLimited( m_sQueueLimit ) )
	{
		// Max queue is limited, calculate number
		int nPosition = 1, nCount = m_sQueueLimit.GetLength();
		while ( nCount-- )
		{
			TCHAR cCharacter = m_sQueueLimit.GetAt( nCount );
			if ( ( cCharacter >= '0' ) &&
				 ( cCharacter <= '9' ) )
			{
				nQueueLimit += ( ( cCharacter - '0') * nPosition );
				nPosition *= 10;
			}
		}
	}
	else
	{
		// Max queue is not limited
		nQueueLimit = 0;
	}

	// Check the queue limit value is okay
	if ( ( nQueueLimit > 0 ) && ( nQueueLimit < 2000 ) && ( ! Settings.Live.QueueLimitWarning ) &&
		 ( Settings.eDonkey.Enabled || Settings.eDonkey.EnableAlways ) && ( Settings.Downloads.QueueLimit != (int)nQueueLimit ) )
	{
		// Warn the user about setting the max queue wait limit too low
		CString strMessage;
		LoadString( strMessage, IDS_SETTINGS_WARN_QUEUELIMIT );

		if ( AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) == IDNO )
		{
			nQueueLimit = 0;
		}
		else
		{
			// Don't need to warn the user again.
			Settings.Live.QueueLimitWarning = TRUE;
		}
	}

	// Put new values in the settings.
	Settings.Downloads.CompletePath			= m_sDownloadsPath;
	Settings.Downloads.IncompletePath		= m_sIncompletePath;
	Settings.Downloads.MaxFiles				= m_nMaxDownFiles;
	Settings.Downloads.MaxTransfers			= m_nMaxDownTransfers;
	Settings.Downloads.MaxFileTransfers		= m_nMaxFileTransfers;
	Settings.Downloads.QueueLimit			= nQueueLimit;
	Settings.Bandwidth.Downloads			= static_cast< DWORD >( Settings.ParseVolume( m_sBandwidthLimit ) );
	Settings.Connection.RequireForTransfers	= m_bRequireConnect != FALSE;

	// Normalize data
	Settings.Normalize( &Settings.Downloads.MaxFiles );
	m_nMaxDownFiles		= Settings.Downloads.MaxFiles;
	Settings.Normalize( &Settings.Downloads.MaxTransfers );
	m_nMaxDownTransfers	= Settings.Downloads.MaxTransfers;
	Settings.Normalize( &Settings.Downloads.MaxFileTransfers );
	m_nMaxFileTransfers	= Settings.Downloads.MaxFileTransfers;
	Settings.Normalize( &Settings.Downloads.QueueLimit );

	// Redraw the text in the queue limit box (in case the limit changed)
	if ( Settings.Downloads.QueueLimit > 0 )
		m_sQueueLimit.Format( _T("%d"), Settings.Downloads.QueueLimit );
	else
		m_sQueueLimit = _T("MAX");

	// Display any data changes
	UpdateData( FALSE );

	CreateDirectory( m_sDownloadsPath );
	CreateDirectory( m_sIncompletePath );
	// CreateDirectory( m_sTorrentPath );

	if ( m_bDownloadsChanged )
	{
		if ( LibraryFolders.GetFolderCount() == 0 )
		{
			LibraryFolders.AddFolder( m_sDownloadsPath );
		}
		else if ( ! LibraryFolders.IsFolderShared( m_sDownloadsPath ) )
		{
			CString strFormat, strMessage;

			LoadString( strFormat, IDS_LIBRARY_DOWNLOADS_ADD );
			strMessage.Format( strFormat, (LPCTSTR)m_sDownloadsPath );

			if ( AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) == IDYES )
			{
				CLibraryFolder* pFolder = LibraryFolders.AddFolder( m_sDownloadsPath );

				if ( pFolder )
				{
					LoadString( strMessage, IDS_LIBRARY_DOWNLOADS_SHARE );

					BOOL bShare = AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) == IDYES;

					CQuickLock oLock( Library.m_pSection );
					if ( LibraryFolders.CheckFolder( pFolder, TRUE ) )
						pFolder->SetShared( bShare ? TRI_TRUE : TRI_FALSE );
					Library.Update();
				}
			}
		}
	}

	CSettingsPage::OnOK();
}

void CDownloadsSettingsPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CSettingsPage::OnShowWindow(bShow, nStatus);
	if ( bShow )
	{
		// Update the bandwidth limit combo values

		// Update speed units
		if ( Settings.Bandwidth.Downloads )
			m_sBandwidthLimit = Settings.SmartSpeed( Settings.Bandwidth.Downloads );
		else
			m_sBandwidthLimit = _T("MAX");

		// Remove any existing strings
		m_wndBandwidthLimit.ResetContent();

		// Add the new ones
		const DWORD nSpeeds[] =
		{
			Settings.Connection.InSpeed / 4,			//  25%
			Settings.Connection.InSpeed / 2,			//  50%
			( Settings.Connection.InSpeed * 3 ) / 4,	//  75%
			( Settings.Connection.InSpeed * 17 ) / 20,	//  85%
			Settings.Connection.InSpeed					// 100%
		};
		for ( int nSpeed = 0 ; nSpeed < sizeof( nSpeeds ) / sizeof( DWORD ) ; nSpeed++ )
		{
			CString strSpeed = Settings.SmartSpeed( nSpeeds[ nSpeed ], Kilobits );
			if ( Settings.ParseVolume( strSpeed, Kilobits )
				&& m_wndBandwidthLimit.FindStringExact( -1, strSpeed ) == CB_ERR )
			{
				m_wndBandwidthLimit.AddString( strSpeed );
			}
		}
		m_wndBandwidthLimit.AddString( _T("MAX") );

		// Update the queue limit combo values

		// Remove any existing strings
		while ( m_wndQueueLimit.GetCount() )
			m_wndQueueLimit.DeleteString( 0 );

		// Add the new ones
		if ( Settings.eDonkey.Enabled || Settings.eDonkey.EnableAlways )
		{
			m_wndQueueLimit.AddString( _T("2000") );
			m_wndQueueLimit.AddString( _T("5000") );
			m_wndQueueLimit.AddString( _T("10000") );
			m_wndQueueLimit.AddString( _T("MAX") );
		}
		else
		{
			m_wndQueueLimit.AddString( _T("5") );
			m_wndQueueLimit.AddString( _T("10") );
			m_wndQueueLimit.AddString( _T("20") );
			m_wndQueueLimit.AddString( _T("MAX") );
		}

		UpdateData( FALSE );
	}
}

bool CDownloadsSettingsPage::IsLimited(CString& strText) const
{
	if ( ( _tcslen( strText ) == 0 ) ||
		 ( _tcsistr( strText, _T("MAX") ) != NULL ) ||
		 ( _tcsistr( strText, _T("NONE") ) != NULL ) )
		return false;

	return true;
}
