//
// PageSettingsMedia.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "WndSettingsPage.h"
#include "WndSettingsSheet.h"
#include "PageSettingsMedia.h"
#include "PageSettingsPlugins.h"
#include "DlgMediaVis.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CMediaSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CMediaSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CMediaSettingsPage)
	ON_BN_CLICKED(IDC_MEDIA_PLAY, OnMediaPlay)
	ON_BN_CLICKED(IDC_MEDIA_ENQUEUE, OnMediaEnqueue)
	ON_BN_CLICKED(IDC_MEDIA_ADD, OnMediaAdd)
	ON_BN_CLICKED(IDC_MEDIA_REMOVE, OnMediaRemove)
	ON_BN_CLICKED(IDC_MEDIA_VIS, OnMediaVis)
	ON_CBN_EDITCHANGE(IDC_MEDIA_TYPES, OnEditChangeMediaTypes)
	ON_CBN_SELCHANGE(IDC_MEDIA_TYPES, OnSelChangeMediaTypes)
	ON_CBN_SELCHANGE(IDC_MEDIA_SERVICE, OnSelChangeMediaService)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMediaSettingsPage property page

CMediaSettingsPage::CMediaSettingsPage()
	: CSettingsPage(CMediaSettingsPage::IDD)
	, m_sType		( _T("") )
	, m_bEnablePlay ( FALSE )
	, m_bEnableEnqueue ( FALSE )
{
	//{{AFX_DATA_INIT(CMediaSettingsPage)
	//}}AFX_DATA_INIT
}

CMediaSettingsPage::~CMediaSettingsPage()
{
}

void CMediaSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMediaSettingsPage)
	DDX_Control(pDX, IDC_MEDIA_SERVICE, m_wndServices);
	DDX_Control(pDX, IDC_MEDIA_REMOVE, m_wndRemove);
	DDX_Control(pDX, IDC_MEDIA_ADD, m_wndAdd);
	DDX_Control(pDX, IDC_MEDIA_TYPES, m_wndList);
	DDX_CBString(pDX, IDC_MEDIA_TYPES, m_sType);
	DDX_Check(pDX, IDC_MEDIA_PLAY, m_bEnablePlay);
	DDX_Check(pDX, IDC_MEDIA_ENQUEUE, m_bEnableEnqueue);
	//}}AFX_DATA_MAP

}

void CMediaSettingsPage::Update()
{
	UpdateData();

	m_wndList.EnableWindow( m_bEnablePlay || m_bEnableEnqueue );
	m_wndAdd.EnableWindow( ( m_bEnablePlay || m_bEnableEnqueue ) && m_wndList.GetWindowTextLength() > 0 );
	m_wndRemove.EnableWindow( ( m_bEnablePlay || m_bEnableEnqueue ) && m_wndList.GetCurSel() >= 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CMediaSettingsPage message handlers

BOOL CMediaSettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_bEnablePlay		= Settings.MediaPlayer.EnablePlay;
	m_bEnableEnqueue	= Settings.MediaPlayer.EnableEnqueue;
	m_nSelected			= 3;	// Default PeerProject Media Player

	for ( string_set::const_iterator i = Settings.MediaPlayer.FileTypes.begin() ;
		i != Settings.MediaPlayer.FileTypes.end(); i++ )
	{
		m_wndList.AddString( *i );
	}

	CString str;
	LoadString( str, IDS_GENERAL_CUSTOM );
	str.Insert( 0, '(' );
	str.Append( _T("\x2026)") );
	m_wndServices.AddString( str );
	LoadString( str, IDS_MEDIA_SMPLAYER );
	m_wndServices.AddString( str );

	int nCount = 0;
	for( string_set::const_reverse_iterator i = Settings.MediaPlayer.ServicePath.rbegin() ; i != Settings.MediaPlayer.ServicePath.rend() ; ++i )
	{
		m_sServicePath[nCount] = *i;
		int nBackSlash = m_sServicePath[nCount].ReverseFind( '\\' );
		str = m_sServicePath[nCount].Mid( nBackSlash + 1 );
		int nAstrix= str.ReverseFind( '*' );

		if( nAstrix != -1 )	// Selected player
		{
			m_nSelected	= nCount;
			str.Remove('*');
		}

		m_wndServices.InsertString( 2+nCount, str );
		m_sServicePath[nCount].Remove('*');
		nCount++;
	}

	if ( m_nSelected == 3 ) // Default
	{
		m_wndServices.SetCurSel( 1 );
	}
	else // Custom player
	{
		m_wndServices.SetCurSel( m_nSelected + 2 );
		GetDlgItem( IDC_MEDIA_PLAY )->EnableWindow( FALSE );
		GetDlgItem( IDC_MEDIA_ENQUEUE )->EnableWindow( FALSE );
		GetDlgItem( IDC_MEDIA_VIS )->EnableWindow( FALSE );
	}

	UpdateData( FALSE );

	Update();

	return TRUE;
}

void CMediaSettingsPage::OnMediaPlay()
{
	Update();
}

void CMediaSettingsPage::OnMediaEnqueue()
{
	Update();
}

void CMediaSettingsPage::OnSelChangeMediaTypes()
{
	Update();
}

void CMediaSettingsPage::OnEditChangeMediaTypes()
{
	Update();
}

void CMediaSettingsPage::OnMediaAdd()
{
	UpdateData();

	ToLower( m_sType );
	m_sType.Trim();
	if ( m_sType.IsEmpty() ) return;

	if ( m_wndList.FindStringExact( -1, m_sType ) >= 0 ) return;

	m_wndList.AddString( m_sType );
	m_sType.Empty();
	UpdateData( FALSE );
}

void CMediaSettingsPage::OnMediaRemove()
{
	int nItem = m_wndList.GetCurSel();
	if ( nItem >= 0 )
		m_wndList.DeleteString( nItem );

	m_wndRemove.EnableWindow( FALSE );
}

void CMediaSettingsPage::OnMediaVis()
{
	CMediaVisDlg dlg( NULL );
	dlg.DoModal();
}

void CMediaSettingsPage::OnOK()
{
	UpdateData();

	Settings.MediaPlayer.EnablePlay		= m_bEnablePlay != FALSE;
	Settings.MediaPlayer.EnableEnqueue	= m_bEnableEnqueue != FALSE;
	Settings.MediaPlayer.ServicePath.clear();

	CString str = _T("*");
	for( int i = 0 ; i < 3 && ! m_sServicePath[i].IsEmpty() ; ++i )
	{
		if( i == m_nSelected )
			m_sServicePath[i] += _T("*");
		Settings.MediaPlayer.ServicePath.insert( str + m_sServicePath[i] );
		str += _T("*");
	}

	CString strRegData;

	if ( m_nSelected == 3 )		// PeerProject Media Player is selected
		Settings.MediaPlayer.ShortPaths = FALSE;
	else
		strRegData = _T("-");

		// Starting from v.0.8.5 VLC player reads unicode paths
		//CString strExecutable;
		//m_wndServices.GetWindowText( strExecutable );
		//Settings.MediaPlayer.ShortPaths = ToLower( strExecutable ) == _T("vlc.exe");

	theApp.WriteProfileString( _T("Plugins"), Settings.MediaPlayer.AviPreviewCLSID, strRegData );
	theApp.WriteProfileString( _T("Plugins"), Settings.MediaPlayer.MediaServicesCLSID, strRegData );
	theApp.WriteProfileString( _T("Plugins"), Settings.MediaPlayer.Mp3PreviewCLSID, strRegData );
	theApp.WriteProfileString( _T("Plugins"), Settings.MediaPlayer.Mpeg1PreviewCLSID, strRegData );
	theApp.WriteProfileString( _T("Plugins"), Settings.MediaPlayer.VisCLSID, strRegData );
	theApp.WriteProfileString( _T("Plugins"), Settings.MediaPlayer.VisWrapperCLSID, strRegData );
//	theApp.WriteProfileString( _T("Plugins"), Settings.MediaPlayer.VisSoniqueCLSID, strRegData );

	CSettingsSheet* pSheet = GetSheet();
	for ( INT_PTR nPage = 0 ; nPage < pSheet->GetPageCount() ; nPage++ )
	{
		CSettingsPage* pPage = pSheet->GetPage( nPage );
		if ( pPage )
		{
			CString strClass( pPage->GetRuntimeClass()->m_lpszClassName );
			if ( strClass == _T("CPluginsSettingsPage") )
			{
				CPluginsSettingsPage* pPluginPage = static_cast< CPluginsSettingsPage* >( pPage );
				pPluginPage->UpdateList();
				break;
			}
		}
	}

	Settings.MediaPlayer.FileTypes.clear();

	for ( int nItem = 0 ; nItem < m_wndList.GetCount() ; nItem++ )
	{
		CString str;
		m_wndList.GetLBText( nItem, str );
		if ( ! str.IsEmpty() )
			Settings.MediaPlayer.FileTypes.insert( str );
	}

	CSettingsPage::OnOK();
}

void CMediaSettingsPage::OnSelChangeMediaService()
{
	const int nCustomIndex = 0;
	int nSelected = m_wndServices.GetCurSel();

	if ( nSelected == nCustomIndex ) // Not default (1)
	{
		CFileDialog dlg( TRUE, _T("exe"), _T("") , OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("Executable Files|*.exe;*.com|All Files|*.*||"), this );

		if ( dlg.DoModal() != IDOK )
		{
			m_wndServices.SetCurSel( 0 );
			return;
		}

		// List only keeps 5 items
		if ( m_wndServices.GetCount() == 5 )
		{
			m_wndServices.DeleteString( 4 );	//FIFO
			m_wndServices.InsertString( 2, dlg.GetFileName() );
			m_wndServices.SetCurSel( 2 );

			m_sServicePath[2] = m_sServicePath[1];
			m_sServicePath[1] = m_sServicePath[0];
			m_sServicePath[0] = dlg.GetPathName();
			m_nSelected = 0;
		}
		else
		{
			m_wndServices.InsertString( 2, dlg.GetFileName() );
			m_wndServices.SetCurSel( 2 );

			int i = 0 ;
			//while( ! m_sServicePath[i].IsEmpty() ) ++i;
			if ( m_sServicePath[0].IsEmpty() )
			{
				m_sServicePath[0] = dlg.GetPathName();
			}
			else if ( m_sServicePath[1].IsEmpty() )
			{
				m_sServicePath[1] = m_sServicePath[0];
				m_sServicePath[0] = dlg.GetPathName();
			}
			else
			{
				m_sServicePath[2] = m_sServicePath[1];
				m_sServicePath[1] = m_sServicePath[0];
				m_sServicePath[0] = dlg.GetPathName();
			}

			m_nSelected = i;
		}

		m_bEnablePlay = m_bEnableEnqueue = FALSE;
		UpdateData( FALSE );

		GetDlgItem( IDC_MEDIA_PLAY )->EnableWindow( FALSE );
		GetDlgItem( IDC_MEDIA_ENQUEUE )->EnableWindow( FALSE );
		GetDlgItem( IDC_MEDIA_VIS )->EnableWindow( FALSE );
	}
	else if ( nSelected == 1 ) // PeerProject Media Player selected
	{
		m_nSelected = 3;
		m_bEnablePlay = m_bEnableEnqueue = TRUE;
		UpdateData( FALSE );

		GetDlgItem( IDC_MEDIA_PLAY )->EnableWindow( TRUE );
		GetDlgItem( IDC_MEDIA_ENQUEUE )->EnableWindow( TRUE );
		GetDlgItem( IDC_MEDIA_VIS )->EnableWindow( TRUE );
	}
	else // Not PeerProject default, not custom
	{
		m_nSelected = nSelected - 2;
		m_bEnablePlay = m_bEnableEnqueue = FALSE;
		UpdateData( FALSE );

		GetDlgItem( IDC_MEDIA_PLAY )->EnableWindow( FALSE );
		GetDlgItem( IDC_MEDIA_ENQUEUE )->EnableWindow( FALSE );
		GetDlgItem( IDC_MEDIA_VIS )->EnableWindow( FALSE );
	}
}
