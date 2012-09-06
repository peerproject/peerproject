//
// DlgSelect.cpp
//
// This file is part of PeerProject (peerproject.org) © 2009-2010
// Portions copyright Shareaza Development Team, 2009.
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
#include "PeerProject.h"
#include "DlgSelect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

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
