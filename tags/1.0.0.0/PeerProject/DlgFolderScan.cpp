//
// DlgFolderScan.cpp
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
#include "DlgFolderScan.h"
#include "Library.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

BEGIN_MESSAGE_MAP(CFolderScanDlg, CSkinDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CFolderScanDlg* CFolderScanDlg::m_pDialog = NULL;


/////////////////////////////////////////////////////////////////////////////
// CFolderScanDlg dialog

CFolderScanDlg::CFolderScanDlg(CWnd* pParent)
	: CSkinDialog(CFolderScanDlg::IDD, pParent)
	, m_nCookie		( 0 )
	, m_nFiles		( 0 )
	, m_nVolume		( 0 )
{
	m_tLastUpdate	= 0;
	m_bActive		= FALSE;

	CSingleLock oLock( &Library.m_pSection );
	if ( oLock.Lock( 800 ) )
	{
		m_pDialog	= this;
		m_nCookie	= Library.GetScanCount();
	}
}

CFolderScanDlg::~CFolderScanDlg()
{
	if ( m_pDialog )
	{
		CSingleLock pLock( &Library.m_pSection );
		if ( pLock.Lock( 3000 ) )
			m_pDialog = NULL;
	}
}

void CFolderScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCAN_VOLUME, m_wndVolume);
	DDX_Control(pDX, IDC_SCAN_FILES, m_wndFiles);
	DDX_Control(pDX, IDC_SCAN_FILE, m_wndFile);
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

	if ( pLock.Lock( 50 ) && m_nCookie != Library.GetScanCount() )
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
		pLock.Lock( 800 );
		m_pDialog = NULL;
	}

	CSkinDialog::OnCancel();
}

void CFolderScanDlg::Update(LPCTSTR pszName, DWORD nVolume)
{
	CSingleLock oLock( &Library.m_pSection );
	if ( m_pDialog != NULL && oLock.Lock( 10 ) )
		m_pDialog->InstanceUpdate( pszName, nVolume );
}

void CFolderScanDlg::InstanceUpdate(LPCTSTR pszName, DWORD nVolume)
{
	const DWORD tNow = GetTickCount();

	m_nFiles++;
	m_nVolume += nVolume;

	if ( m_bActive && tNow > m_tLastUpdate + Settings.Interface.RefreshRateUI )
	{
		m_tLastUpdate = tNow;

		m_wndFile.SetWindowText( pszName );

		CString strItem;
		strItem.Format( _T("%lu"), m_nFiles );
		m_wndFiles.SetWindowText( strItem );

		strItem = Settings.SmartVolume( m_nVolume, KiloBytes );
		m_wndVolume.SetWindowText( strItem );

		RedrawWindow();
	}
}
