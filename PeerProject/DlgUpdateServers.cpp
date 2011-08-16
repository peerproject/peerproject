//
// DlgUpdateServers.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2008.
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

// Fetch .met and other hostcache lists from the web
// Was DlgDonkeyServers.cpp, CDonkeyServersDlg

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "DlgUpdateServers.h"
#include "HostCache.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

BEGIN_MESSAGE_MAP(CUpdateServersDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CUpdateServersDlg)
	ON_EN_CHANGE(IDC_URL, OnChangeURL)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUpdateServersDlg dialog

CUpdateServersDlg::CUpdateServersDlg(CWnd* pParent)
	: CSkinDialog(CUpdateServersDlg::IDD, pParent)
{
}

void CUpdateServersDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateServersDlg)
	DDX_Control(pDX, IDC_URL, m_wndURL);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	DDX_Text(pDX, IDC_URL, m_sURL);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CUpdateServersDlg message handlers

BOOL CUpdateServersDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CUpdateServersDlg"), IDR_MAINFRAME );

	// Define dlg.m_URL = Settings.DC.HubListURL etc. before dlg.DoModal()
	if ( m_sURL.GetLength() < 12 )
		m_sURL = Settings.eDonkey.ServerListURL;

	m_wndOK.EnableWindow( IsValidURL() );
	m_wndProgress.SetRange( 0, 100 );
	m_wndProgress.SetPos( 0 );

	UpdateData( FALSE );

	return TRUE;
}

BOOL CUpdateServersDlg::IsValidURL()
{
	return
		m_sURL.GetLength() > 12 &&
		StartsWith( m_sURL, _T("http://"), 7 ) &&
	//	m_sURL.Find( _T('/'), 7 ) > 12 &&
		m_sURL.Find( _T('.'), 7 ) > 8;
}

void CUpdateServersDlg::OnChangeURL()
{
	UpdateData();

	m_wndOK.EnableWindow( IsValidURL() );
}

void CUpdateServersDlg::OnOK()
{
	UpdateData();

	if ( ! IsValidURL() ) return;
	if ( ! m_pRequest.SetURL( m_sURL ) ) return;
	if ( ! m_pRequest.Execute( true ) ) return;

	m_wndOK.EnableWindow( FALSE );
	m_wndURL.EnableWindow( FALSE );

	SetTimer( 1, 100, NULL );
}

void CUpdateServersDlg::OnCancel()
{
	KillTimer( 1 );

	m_pRequest.Cancel();

	CSkinDialog::OnCancel();
}

void CUpdateServersDlg::OnTimer(UINT_PTR nIDEvent)
{
	CSkinDialog::OnTimer( nIDEvent );

	if ( m_pRequest.IsPending() )
	{
		int n = m_wndProgress.GetPos();
		if ( n < 5 )
			n = 5;
		else if ( n < 100 )
			n++;
		m_wndProgress.SetPos( n );
	}
	else
	{
		KillTimer( 1 );

		if ( m_pRequest.GetStatusSuccess() )
		{
			const CString strExt = CString( PathFindExtension( m_sURL ) ).MakeLower();
			if ( strExt == L".met" || m_sURL.Find( _T("//server"), 8 ) > 8 )		// || strExt == L".php"
				Settings.eDonkey.ServerListURL = m_sURL;
			else if ( strExt == L".bz2" || m_sURL.Find( _T("hublist"), 8 ) > 8 )
				Settings.DC.HubListURL = m_sURL;
		//	else if ( strExt == L".xml" )
		//		Settings.Gnutella.CacheURL = m_sURL;
		//	else if ( strExt == L".dat" )
		//		Settings.KAD.NodesListURL = m_sURL;

			const CBuffer* pBuffer = m_pRequest.GetResponseBuffer();

			CMemFile pFile;
			pFile.Write( pBuffer->m_pBuffer, pBuffer->m_nLength );
			pFile.Seek( 0, CFile::begin );

			if ( ( strExt == L".bz2" && HostCache.ImportHubList( &pFile ) ) ||
				 HostCache.ImportMET( &pFile ) )
			//	 HostCache.ImportCache( &pFile ) || 	// ToDo: G2/Gnutella loading
			//	 HostCache.ImportNodes( &pFile ) )		// ToDo: KAD
			{
				HostCache.Save();

				m_sURL.Empty();
				EndDialog( IDOK );
				return;
			}
		}

		CString strError;
		strError.Format( LoadString( IDS_DOWNLOAD_DROPPED ), m_sURL );
		AfxMessageBox( strError, MB_OK | MB_ICONEXCLAMATION );

		m_sURL.Empty();
		EndDialog( IDCANCEL );
	}

	UpdateWindow();
}
