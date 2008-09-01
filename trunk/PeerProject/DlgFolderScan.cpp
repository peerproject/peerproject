//
// DlgFolderScan.cpp
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
#include "Library.h"
#include "DlgFolderScan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CFolderScanDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CFolderScanDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFolderScanDlg* CFolderScanDlg::m_pDialog = NULL;


/////////////////////////////////////////////////////////////////////////////
// CFolderScanDlg dialog

CFolderScanDlg::CFolderScanDlg(CWnd* pParent) : CSkinDialog(CFolderScanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFolderScanDlg)
	//}}AFX_DATA_INIT

	m_nCookie		= 0;
	m_nFiles		= 0;
	m_nVolume		= 0;
	m_tLastUpdate	= 0;
	m_bActive		= FALSE;

	CSingleLock oLock( &Library.m_pSection );
	if ( oLock.Lock( 500 ) )
	{
		m_pDialog	= this;
		m_nCookie	= Library.m_nScanCount;
	}
}

CFolderScanDlg::~CFolderScanDlg()
{
	if ( m_pDialog )
	{
		CSingleLock pLock( &Library.m_pSection );
		pLock.Lock( 500 );
		m_pDialog = NULL;
	}
}

void CFolderScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFolderScanDlg)
	DDX_Control(pDX, IDC_SCAN_VOLUME, m_wndVolume);
	DDX_Control(pDX, IDC_SCAN_FILES, m_wndFiles);
	DDX_Control(pDX, IDC_SCAN_FILE, m_wndFile);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFolderScanDlg operations

BOOL CFolderScanDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CFolderScanDlg"), IDR_LIBRARYFRAME );

	SetTimer( 1, 500, NULL );
	m_tLastUpdate	= 0;
	m_bActive		= TRUE;

	return TRUE;
}

void CFolderScanDlg::OnTimer(UINT_PTR /*nIDEvent*/)
{
	CSingleLock pLock( &Library.m_pSection );

	if ( pLock.Lock( 50 ) && m_nCookie != Library.m_nScanCount )
	{
		pLock.Unlock();
		CSkinDialog::OnCancel();
	}
}

void CFolderScanDlg::OnCancel()
{
	m_bActive = FALSE;

	if ( m_pDialog )
	{
		CSingleLock pLock( &Library.m_pSection );
		pLock.Lock( 500 );
		m_pDialog = NULL;
	}

	CSkinDialog::OnCancel();
}

void CFolderScanDlg::Update(LPCTSTR pszName, DWORD nVolume)
{
	CSingleLock oLock( &Library.m_pSection );
	if ( m_pDialog != NULL && oLock.Lock( 10 ) )
	{
		m_pDialog->InstanceUpdate( pszName, nVolume );
	}
}

void CFolderScanDlg::InstanceUpdate(LPCTSTR pszName, DWORD nVolume)
{
	DWORD dwNow = GetTickCount();
	CString strItem;

	m_nFiles ++;
	m_nVolume += nVolume;

	if ( m_bActive && dwNow - m_tLastUpdate > 250 )
	{
		m_tLastUpdate = dwNow;

		m_wndFile.SetWindowText( pszName );

		strItem.Format( _T("%lu"), m_nFiles );
		m_wndFiles.SetWindowText( strItem );

		strItem = Settings.SmartVolume( m_nVolume, KiloBytes );
		m_wndVolume.SetWindowText( strItem );

		RedrawWindow();
	}
}

