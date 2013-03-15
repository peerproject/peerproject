//
// DlgDonkeyImport.cpp
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
#include "DlgDonkeyImport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

BEGIN_MESSAGE_MAP(CDonkeyImportDlg, CSkinDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDonkeyImportDlg dialog

CDonkeyImportDlg::CDonkeyImportDlg(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDonkeyImportDlg::IDD, pParent)
{
}

void CDonkeyImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, m_wndClose);
	DDX_Control(pDX, IDCANCEL, m_wndCancel);
	DDX_Control(pDX, IDC_IMPORT, m_wndImport);
	DDX_Control(pDX, IDC_LOG, m_wndLog);
}

/////////////////////////////////////////////////////////////////////////////
// CDonkeyImportDlg message handlers

BOOL CDonkeyImportDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CDonkeyImportDlg"), IDR_MAINFRAME );

	CString str;
	m_wndCancel.GetWindowText( str );
	int nPos = str.Find( '|' );
	if ( nPos > 0 )
	{
		m_sCancel = str.Mid( nPos + 1 );
		m_wndCancel.SetWindowText( str.Left( nPos ) );
	}

	return TRUE;
}

void CDonkeyImportDlg::OnImport()
{
	m_wndImport.EnableWindow( FALSE );
	m_wndCancel.SetWindowText( m_sCancel );
	m_pImporter.Start( &m_wndLog );
	SetTimer( 1, 1000, NULL );
}

void CDonkeyImportDlg::OnCancel()
{
	m_pImporter.Stop();
	CSkinDialog::OnCancel();
}

void CDonkeyImportDlg::OnTimer(UINT_PTR /*nIDEvent*/)
{
	if ( ! m_pImporter.IsRunning() )
	{
		KillTimer( 1 );
		m_wndCancel.EnableWindow( FALSE );
		m_wndClose.ModifyStyle( 0, BS_DEFPUSHBUTTON );
		m_wndClose.ShowWindow( SW_SHOW );
		m_wndClose.SetFocus();
	}
}

void CDonkeyImportDlg::OnClose()
{
	EndDialog( IDOK );
}
