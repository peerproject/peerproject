//
// DlgSelect.cpp
//
// This file is part of PeerProject (peerproject.org) © 2009
// Portions Copyright Shareaza Development Team, 2009.
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
#include "DlgSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CSelectDialog dialog

IMPLEMENT_DYNAMIC(CSelectDialog, CSkinDialog)

CSelectDialog::CSelectDialog(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CSelectDialog::IDD, pParent),
	m_nData( 0 )
{
}

void CSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CSelectDialog, CSkinDialog)
END_MESSAGE_MAP()

// CSelectDialog message handlers

BOOL CSelectDialog::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CSelectDialog"), IDR_MAINFRAME );

	int select = 0;
	for ( POSITION pos = m_List.GetHeadPosition(); pos; )
	{
		CItem it = m_List.GetNext( pos );
		int index = m_ListCtrl.AddString( it.m_sItem.Mid( it.m_sItem.Find( '\\' ) + 1 ) );	// Use Truncated Path
		m_ListCtrl.SetItemData( index, it.m_nData );
		if ( it.m_nData == m_nData )
			select = index;
	}
	m_ListCtrl.SetCurSel( select );

	return FALSE;
}

void CSelectDialog::OnOK()
{
	m_nData = m_ListCtrl.GetItemData( m_ListCtrl.GetCurSel() );

	CSkinDialog::OnOK();
}

void CSelectDialog::OnCancel()
{
	m_nData = m_ListCtrl.GetItemData( m_ListCtrl.GetCurSel() );

	CSkinDialog::OnCancel();
}
