//
// DlgUpgrade.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "VersionChecker.h"
#include "Network.h"
#include "Downloads.h"
#include "PeerProjectURL.h"
#include "DlgUpgrade.h"
#include "WndMain.h"
#include "WndDownloads.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

BEGIN_MESSAGE_MAP(CUpgradeDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CUpgradeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUpgradeDlg dialog

CUpgradeDlg::CUpgradeDlg(CWnd* pParent) : CSkinDialog(CUpgradeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpgradeDlg)
	m_bCheck = FALSE;
	m_sMessage = _T("");
	//}}AFX_DATA_INIT
}

void CUpgradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpgradeDlg)
	DDX_Check(pDX, IDC_DONT_CHECK, m_bCheck);
	DDX_Text(pDX, IDC_MESSAGE, m_sMessage);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CUpgradeDlg message handlers

BOOL CUpgradeDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CUpgradeDlg"), IDR_MAINFRAME );

	m_sMessage	= Settings.VersionCheck.UpgradePrompt;
	m_bCheck	= FALSE;

	UpdateData( FALSE );

	return TRUE;
}

void CUpgradeDlg::OnOK()
{
	ParseCheckAgain();

	CPeerProjectURL pURL;

	pURL.m_nAction		= CPeerProjectURL::uriDownload;
	pURL.m_sName		= Settings.VersionCheck.UpgradeFile;
	pURL.m_sURL			= Settings.VersionCheck.UpgradeSources;
	pURL.m_oSHA1.fromString( Settings.VersionCheck.UpgradeSHA1 );
	pURL.m_oTiger.fromString( Settings.VersionCheck.UpgradeTiger );

	if ( Settings.VersionCheck.UpgradeSize.GetLength() )
	{
		QWORD nSize;
		if ( ( _stscanf( Settings.VersionCheck.UpgradeSize.GetString(), _T("%I64i"), &nSize ) == 1 ) && ( nSize > 0 ) )
		{
			pURL.m_bSize = TRUE;
			pURL.m_nSize = nSize;
		}
	}

	Downloads.Add( pURL );

	if ( ! Network.IsWellConnected() ) Network.Connect( TRUE );

	CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();
	pMainWnd->m_pWindows.Open( RUNTIME_CLASS(CDownloadsWnd) );

	CSkinDialog::OnOK();
}

void CUpgradeDlg::OnCancel()
{
	ParseCheckAgain();

	CSkinDialog::OnCancel();
}

void CUpgradeDlg::ParseCheckAgain()
{
	UpdateData();

	if ( m_bCheck )
		VersionChecker.SetNextCheck( 31 );	// ToDo: Set Update Delay
}
