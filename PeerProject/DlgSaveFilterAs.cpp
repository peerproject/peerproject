//
// SaveFilterAsDlg.cpp
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
//
// Author : roo_koo_too@yahoo.com
//
#include "stdafx.h"
#include "PeerProject.h"
#include "DlgSaveFilterAs.h"
#include "DlgFilterSearch.h"
#include "ResultFilters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

// CSaveFilterAsDlg dialog

CSaveFilterAsDlg::CSaveFilterAsDlg( CWnd* pParent /*=NULL*/ )
	: CSkinDialog( CSaveFilterAsDlg::IDD, pParent )
	, m_sName( _T( "" ) )
{
}

CSaveFilterAsDlg::~CSaveFilterAsDlg()
{
}

void CSaveFilterAsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text( pDX, IDC_NAME, m_sName );
}

BEGIN_MESSAGE_MAP(CSaveFilterAsDlg, CSkinDialog)
	ON_EN_CHANGE(IDC_NAME, OnEnChangeName)
END_MESSAGE_MAP()


// CSaveFilterAsDlg message handlers

void CSaveFilterAsDlg::OnOK()
{
	if ( m_sName.IsEmpty() )
	{
		CString strMessage;
		LoadString( strMessage, IDS_FILTER_NO_NAME );
		AfxMessageBox( strMessage, MB_OK );
		return;
	}
	if ( ( (CFilterSearchDlg*)m_pParentWnd)->m_pResultFilters->Search( m_sName ) >= 0 )
	{
		CString strFormat, strMessage;
		LoadString( strFormat, IDS_FILTER_REPLACE );
		strMessage.Format( strFormat, (LPCTSTR)m_sName );
		if ( AfxMessageBox( strMessage, MB_ICONQUESTION | MB_YESNO ) == IDNO )
			return;
	}
	CSkinDialog::OnOK();
}


BOOL CSaveFilterAsDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T( "CSaveFilterAsDlg" ), IDR_SEARCHFRAME );

	return TRUE;
}

void CSaveFilterAsDlg::OnEnChangeName()
{
	UpdateData(TRUE);

	GetDlgItem( IDOK )->EnableWindow( ! m_sName.IsEmpty() );
}
