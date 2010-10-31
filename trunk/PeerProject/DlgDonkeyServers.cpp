//
// DlgDonkeyServers.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "HostCache.h"
#include "DlgDonkeyServers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

BEGIN_MESSAGE_MAP(CDonkeyServersDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CDonkeyServersDlg)
	ON_EN_CHANGE(IDC_URL, OnChangeURL)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDonkeyServersDlg dialog

CDonkeyServersDlg::CDonkeyServersDlg(CWnd* pParent)
	: CSkinDialog(CDonkeyServersDlg::IDD, pParent)
{
}

void CDonkeyServersDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDonkeyServersDlg)
	DDX_Control(pDX, IDC_URL, m_wndURL);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	DDX_Text(pDX, IDC_URL, m_sURL);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDonkeyServersDlg message handlers

BOOL CDonkeyServersDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CDonkeyServersDlg"), IDR_MAINFRAME );

	m_sURL = Settings.eDonkey.ServerListURL;

	m_wndOK.EnableWindow( m_sURL.Find( _T("http://") ) == 0 );
	m_wndProgress.SetRange( 0, 100 );
	m_wndProgress.SetPos( 0 );

	UpdateData( FALSE );

	return TRUE;
}

void CDonkeyServersDlg::OnChangeURL()
{
	UpdateData();

	m_wndOK.EnableWindow( m_sURL.Find( _T("http://") ) == 0 );
}

void CDonkeyServersDlg::OnOK()
{
	UpdateData();

	if ( m_sURL.Find( _T("http://") ) != 0 ) return;
	if ( ! m_pRequest.SetURL( m_sURL ) ) return;
	if ( ! m_pRequest.Execute( true ) ) return;

	Settings.eDonkey.ServerListURL = m_sURL;

	m_wndOK.EnableWindow( FALSE );
	m_wndURL.EnableWindow( FALSE );

	SetTimer( 1, 250, NULL );
}

void CDonkeyServersDlg::OnCancel()
{
	KillTimer( 1 );

	m_pRequest.Cancel();

	CSkinDialog::OnCancel();
}

void CDonkeyServersDlg::OnTimer(UINT_PTR nIDEvent)
{
	CSkinDialog::OnTimer( nIDEvent );

	if ( m_pRequest.IsPending() )
	{
		int n = m_wndProgress.GetPos();
		if ( ++n >= 100 )
			n = 0;
		m_wndProgress.SetPos( n );
	}
	else
	{
		KillTimer( 1 );

		if ( m_pRequest.GetStatusSuccess() )
		{
			const CBuffer* pBuffer = m_pRequest.GetResponseBuffer();

			CMemFile pFile;
			pFile.Write( pBuffer->m_pBuffer, pBuffer->m_nLength );
			pFile.Seek( 0, CFile::begin );

			if ( HostCache.ImportMET( &pFile ) )
				HostCache.Save();
		}
		else
		{
			CString strError;
			strError.Format( LoadString( IDS_DOWNLOAD_DROPPED ), m_sURL );
			AfxMessageBox( strError, MB_OK | MB_ICONEXCLAMATION );
		}

		EndDialog( IDOK );
	}

	UpdateWindow();
}
