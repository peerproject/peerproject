//
// WizardNetworksPage.cpp
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
#include "HostCache.h"
#include "WizardNetworksPage.h"
#include "DlgDonkeyImport.h"
#include "Skin.h"
#include "DlgHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardNetworksPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardNetworksPage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardNetworksPage)
	ON_WM_XBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardNetworksPage property page

CWizardNetworksPage::CWizardNetworksPage() : CWizardPage(CWizardNetworksPage::IDD)
{
	//{{AFX_DATA_INIT(CWizardNetworksPage)
	m_bG2Enable = FALSE;
	m_bG1Enable = FALSE;
	m_bEDEnable = FALSE;
	//}}AFX_DATA_INIT
}

CWizardNetworksPage::~CWizardNetworksPage()
{
}

void CWizardNetworksPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardNetworksPage)
	DDX_Check(pDX, IDC_G2_ENABLE, m_bG2Enable);
	DDX_Check(pDX, IDC_G1_ENABLE, m_bG1Enable);
	DDX_Check(pDX, IDC_ED2K_ENABLE, m_bEDEnable);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardNetworksPage message handlers

BOOL CWizardNetworksPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardNetworksPage"), this );

	m_bG2Enable = Settings.Gnutella2.EnableAlways;
	m_bG1Enable = Settings.Gnutella1.EnableAlways;
	m_bEDEnable = Settings.eDonkey.EnableAlways;

#ifdef LAN_MODE
//	GetDlgItem( IDC_G2_ENABLE )->EnableWindow( FALSE );
	GetDlgItem( IDC_G1_ENABLE )->EnableWindow( FALSE );
	GetDlgItem( IDC_ED2K_ENABLE )->EnableWindow( FALSE );
#endif // LAN_MOD

	UpdateData( FALSE );

	return TRUE;
}

BOOL CWizardNetworksPage::OnSetActive()
{
	//Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

void CWizardNetworksPage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

LRESULT CWizardNetworksPage::OnWizardNext()
{
	UpdateData();

	if ( ! m_bG2Enable )
	{
		CString strMessage;
		LoadString( strMessage, IDS_NETWORK_DISABLE_G2 );

		if ( AfxMessageBox( strMessage, MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2 ) != IDYES )
		{
			m_bG2Enable = TRUE;
			UpdateData( FALSE );
		}
	}

	Settings.Gnutella2.EnableAlways	= m_bG2Enable != FALSE;
	Settings.Gnutella2.EnableToday	= m_bG2Enable != FALSE;
	Settings.Gnutella1.EnableAlways	= m_bG1Enable != FALSE;
	Settings.Gnutella1.EnableToday	= m_bG1Enable != FALSE;
	Settings.eDonkey.EnableAlways	= m_bEDEnable != FALSE;
	Settings.eDonkey.EnableToday	= m_bEDEnable != FALSE;

	DoDonkeyImport();

	return 0;
}

void CWizardNetworksPage::DoDonkeyImport()
{
	CString strPrograms( theApp.GetProgramFilesFolder() ), strFolder;
	CDonkeyImportDlg dlg( this );

	LPCTSTR pszFolders[] =
	{
		_T("<%PROGRAMFILES%>\\eMule\\temp"),
		_T("<%PROGRAMFILES%>\\aMule\\temp"),
		_T("<%PROGRAMFILES%>\\Neo Mule\\temp"),
		_T("<%PROGRAMFILES%>\\eDonkey2000\\temp"),
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

	if ( nCount > 0 ) dlg.DoModal();
}
