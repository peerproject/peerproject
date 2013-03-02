//
// DlgCloseMode.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "DlgCloseMode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

BEGIN_MESSAGE_MAP(CCloseModeDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CCloseModeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCloseModeDlg dialog

CCloseModeDlg::CCloseModeDlg(CWnd* pParent) : CSkinDialog( CCloseModeDlg::IDD, pParent )
	, m_nMode ( -1 )
{
	//{{AFX_DATA_INIT(CCloseModeDlg)
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

	if ( Settings.General.CloseMode )
		m_nMode = Settings.General.CloseMode - 1;
	else
		m_nMode = 0;

	UpdateData( FALSE );

	return TRUE;
}

void CCloseModeDlg::OnOK()
{
	UpdateData();

	Settings.General.CloseMode = m_nMode + 1;

	if ( Settings.General.CloseMode == 2 )
		CSkinDialog::OnOK();
	else
		CSkinDialog::OnCancel();
}
