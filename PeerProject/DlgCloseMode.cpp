//
// DlgCloseMode.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008
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
#include "DlgCloseMode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CCloseModeDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CCloseModeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCloseModeDlg dialog

CCloseModeDlg::CCloseModeDlg(CWnd* pParent) : CSkinDialog( CCloseModeDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CCloseModeDlg)
	m_nMode = -1;
	//}}AFX_DATA_INIT
}

void CCloseModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCloseModeDlg)
	DDX_Radio(pDX, IDC_CLOSE_0, m_nMode);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCloseModeDlg message handlers

BOOL CCloseModeDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CCloseModeDlg"), IDR_MAINFRAME );

	switch ( Settings.General.CloseMode )
	{
	case 0: case 2:
		m_nMode = 0;
		break;
	case 1:
		m_nMode = 1;
		break;
	case 3:
		m_nMode = 2;
		break;
	}

	UpdateData( FALSE );

	return TRUE;
}

void CCloseModeDlg::OnOK()
{
	UpdateData();

	switch ( m_nMode )
	{
	case 0:
		Settings.General.CloseMode = 2;
		break;
	case 1:
		Settings.General.CloseMode = 1;
		break;
	case 2:
		Settings.General.CloseMode = 3;
		break;
	}

	CSkinDialog::OnOK();
}
