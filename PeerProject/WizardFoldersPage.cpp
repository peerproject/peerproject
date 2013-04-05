//
// WizardFoldersPage.cpp
//
// This file is part of PeerProject (peerproject.org) © 2011-2012
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
#include "WizardFoldersPage.h"

#include "DlgFolderScan.h"
#include "DlgDonkeyImport.h"
#include "Library.h"
#include "LibraryFolders.h"
#include "SharedFolder.h"
#include "ShellIcons.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CWizardFoldersPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardFoldersPage, CWizardPage)
	ON_BN_CLICKED(IDC_DOWNLOADS_BROWSE, OnDownloadsBrowse)
	ON_BN_CLICKED(IDC_INCOMPLETE_BROWSE, OnIncompleteBrowse)
	ON_BN_CLICKED(IDC_TORRENTS_BROWSE, OnTorrentsBrowse)
	ON_WM_XBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardFoldersPage property page

CWizardFoldersPage::CWizardFoldersPage() : CWizardPage(CWizardFoldersPage::IDD)
{
}

CWizardFoldersPage::~CWizardFoldersPage()
{
}

void CWizardFoldersPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DOWNLOADS_BROWSE, m_wndDownloadsBrowse);
	DDX_Control(pDX, IDC_INCOMPLETE_BROWSE, m_wndIncompleteBrowse);
	DDX_Control(pDX, IDC_TORRENTS_BROWSE, m_wndTorrentsBrowse);
	DDX_Text(pDX, IDC_DOWNLOADS_FOLDER, m_sDownloadsPath);
	DDX_Text(pDX, IDC_INCOMPLETE_FOLDER, m_sIncompletePath);
	DDX_Text(pDX, IDC_TORRENTS_FOLDER, m_sTorrentsPath);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardFoldersPage message handlers

BOOL CWizardFoldersPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardFoldersPage"), this );

	m_wndDownloadsBrowse.SetIcon( IDI_BROWSE );
	m_wndIncompleteBrowse.SetIcon( IDI_BROWSE );
	m_wndTorrentsBrowse.SetIcon( IDI_BROWSE );

	m_wndDownloadsBrowse.SetHandCursor( TRUE );
	m_wndIncompleteBrowse.SetHandCursor( TRUE );
	m_wndTorrentsBrowse.SetHandCursor( TRUE );

	m_sDownloadsPath	= Settings.Downloads.CompletePath;
	m_sIncompletePath	= Settings.Downloads.IncompletePath;
	m_sTorrentsPath 	= Settings.Downloads.TorrentPath;

	UpdateData( FALSE );

	m_wndDownloadsFolder.SubclassDlgItem( IDC_DOWNLOADS_FOLDER, this );
	m_wndIncompleteFolder.SubclassDlgItem( IDC_INCOMPLETE_FOLDER, this );
	m_wndTorrentsFolder.SubclassDlgItem( IDC_TORRENTS_FOLDER, this );

	return TRUE;
}

void CWizardFoldersPage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

BOOL CWizardFoldersPage::OnSetActive()
{
	// Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	CoolInterface.FixTheme( this );		// Checkbox/Groupbox text colors (Remove theme if needed)

	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

void CWizardFoldersPage::OnDownloadsBrowse()
{
	CString strPath( BrowseForFolder( _T("Select folder for downloads:"), m_sDownloadsPath ) );
	if ( strPath.IsEmpty() )
		return;

	// Warn user about a path that's too long
	if ( _tcslen( strPath ) > MAX_PATH - 33 )
	{
		MsgBox( IDS_SETTINGS_FILEPATH_TOO_LONG, MB_ICONEXCLAMATION );
		return;
	}

	// Make sure download/incomplete folders aren't the same
	if ( _tcsicmp( strPath, m_sIncompletePath ) == 0 )
	{
		MsgBox( IDS_SETTINGS_FILEPATH_NOT_SAME, MB_ICONEXCLAMATION );
		return;
	}

	UpdateData( TRUE );
	m_sDownloadsPath = strPath;
	m_sTorrentsPath  = strPath + _T("\\Torrents");
	UpdateData( FALSE );
}

void CWizardFoldersPage::OnIncompleteBrowse()
{
	CString strPath( BrowseForFolder( _T("Select folder for incomplete files:"), m_sIncompletePath ) );
	if ( strPath.IsEmpty() )
		return;

	// Warn user about a path that's too long
	if ( _tcslen( strPath ) > MAX_PATH - 52 )
	{
		MsgBox( IDS_SETTINGS_FILEPATH_TOO_LONG, MB_ICONEXCLAMATION );
		return;
	}

	// Make sure download/incomplete folders aren't the same
	if ( _tcsicmp( strPath, m_sDownloadsPath ) == 0 )
	{
		MsgBox( IDS_SETTINGS_FILEPATH_NOT_SAME, MB_ICONEXCLAMATION );
		return;
	}

	// Warn user about an incomplete folder in the library
	if ( LibraryFolders.IsFolderShared( strPath ) )
	{
		MsgBox( IDS_SETTINGS_INCOMPLETE_LIBRARY, MB_ICONEXCLAMATION );
		return;
	}

	UpdateData( TRUE );
	m_sIncompletePath = strPath;
	UpdateData( FALSE );
}

void CWizardFoldersPage::OnTorrentsBrowse()
{
	CString strPath( BrowseForFolder( _T("Select folder for torrents:"), m_sTorrentsPath ) );
	if ( strPath.IsEmpty() )
		return;

	UpdateData( TRUE );
	m_sTorrentsPath = strPath;
	UpdateData( FALSE );
}

LRESULT CWizardFoldersPage::OnWizardNext()
{
	CWaitCursor pCursor;

	CreateDirectory( m_sDownloadsPath );
	CreateDirectory( m_sIncompletePath );
	CreateDirectory( m_sTorrentsPath );

	if ( m_sIncompletePath != Settings.Downloads.IncompletePath && Settings.Library.UseCustomFolders )
	{
		// Set desktop.ini
		CLibraryFolder*	pFolderNew = new CLibraryFolder( NULL, m_sIncompletePath );
		pFolderNew->Maintain( TRUE );
		CLibraryFolder*	pFolderOld = new CLibraryFolder( NULL, Settings.Downloads.IncompletePath );
		pFolderOld->Maintain( FALSE );
	}

	Settings.Downloads.CompletePath		= m_sDownloadsPath;
	Settings.Downloads.IncompletePath	= m_sIncompletePath;
	Settings.Downloads.TorrentPath		= m_sTorrentsPath;

	UpdateData( FALSE );

	//LibraryFolders.AddFolder( m_sDownloadsPath );
	//LibraryFolders.AddFolder( m_sTorrentsPath );

	DoDonkeyImport();

	return 0;
}

void CWizardFoldersPage::DoDonkeyImport()
{
	CString strPrograms( theApp.GetProgramFilesFolder() ), strFolder;
	CDonkeyImportDlg dlg( this );

	LPCTSTR pszFolders[] =
	{
		_T("<%PROGRAMFILES%>\\eMule\\temp"),
		_T("<%PROGRAMFILES%>\\aMule\\temp"),
		_T("<%PROGRAMFILES%>\\Neo Mule\\temp"),
		//_T("<%PROGRAMFILES%>\\eDonkey2000\\temp"),
		NULL
	};

	int nCount = 0;
	for ( int nFolder = 0 ; pszFolders[ nFolder ] ; nFolder++ )
	{
		strFolder = pszFolders[ nFolder ];
		strFolder.Replace( _T("<%PROGRAMFILES%>"), strPrograms );

		if ( GetFileAttributes( strFolder ) != 0xFFFFFFFF )
		{
			dlg.m_pImporter.AddFolder( strFolder );
			nCount++;
		}
	}

	if ( nCount > 0 )
		dlg.DoModal();
}
