//
// DlgHitColumns.cpp
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
#include "Schema.h"
#include "DlgHitColumns.h"
#include "CoolMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CSchemaColumnsDlg, CSkinDialog)
	ON_CBN_SELCHANGE(IDC_SCHEMAS, OnSelChangeSchemas)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSchemaColumnsDlg dialog

CSchemaColumnsDlg::CSchemaColumnsDlg(CWnd* pParent ) : CSkinDialog(CSchemaColumnsDlg::IDD, pParent)
{
}

void CSchemaColumnsDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COLUMNS, m_wndColumns);
	DDX_Control(pDX, IDC_SCHEMAS, m_wndSchemas);
}

/////////////////////////////////////////////////////////////////////////////
// CSchemaColumnsDlg message handlers

BOOL CSchemaColumnsDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CSchemaColumnsDlg"), IDR_SEARCHFRAME );

	m_wndColumns.InsertColumn( 0, _T("Member"), LVCFMT_LEFT, 128, -1 );
	m_wndColumns.InsertColumn( 1, _T("Type"), LVCFMT_LEFT, 0, 0 );
	ListView_SetExtendedListViewStyle( m_wndColumns.GetSafeHwnd(), LVS_EX_CHECKBOXES );

	LoadString( m_wndSchemas.m_sNoSchemaText, IDS_SEARCH_NO_SCHEMA );
	m_wndSchemas.Load( m_pSchema ? m_pSchema->GetURI() : _T("") );

	OnSelChangeSchemas();

	for ( int nMember = 0 ; nMember < m_wndColumns.GetItemCount() ; nMember++ )
	{
		BOOL bChecked = m_pColumns.Find(
			reinterpret_cast< CSchemaMember* >( m_wndColumns.GetItemData( nMember ) ) ) != NULL;
		m_wndColumns.SetItemState( nMember, INDEXTOSTATEIMAGEMASK( bChecked + 1 ), LVIS_STATEIMAGEMASK );
	}

	return TRUE;
}

void CSchemaColumnsDlg::OnSelChangeSchemas()
{
	CSchema* pSchema = m_wndSchemas.GetSelected();

	m_wndColumns.DeleteAllItems();
	if ( ! pSchema ) return;

	CString strMembers = theApp.GetProfileString( _T("Interface"),
		_T("SchemaColumns.") + pSchema->m_sSingular, _T("(EMPTY)") );

	if ( strMembers == _T("(EMPTY)") ) strMembers = pSchema->m_sDefaultColumns;

	for ( POSITION pos = pSchema->GetMemberIterator() ; pos ; )
	{
		CSchemaMember* pMember = pSchema->GetNextMember( pos );

		if ( !pMember->m_bHidden )
		{
			LV_ITEM pItem = {};
			pItem.mask		= LVIF_TEXT|LVIF_PARAM;
			pItem.iItem		= m_wndColumns.GetItemCount();
			pItem.lParam	= (LPARAM)pMember;
			pItem.pszText	= (LPTSTR)(LPCTSTR)pMember->m_sTitle;
			pItem.iItem		= m_wndColumns.InsertItem( &pItem );
			pItem.mask		= LVIF_TEXT;
			pItem.iSubItem	= 1;
			pItem.pszText	= (LPTSTR)(LPCTSTR)pMember->m_sType;
			m_wndColumns.SetItem( &pItem );

			if ( strMembers.Find( _T("|") + pMember->m_sName + _T("|") ) >= 0 )
			{
				m_wndColumns.SetItemState( pItem.iItem, INDEXTOSTATEIMAGEMASK( 2 ),
					LVIS_STATEIMAGEMASK );
			}
		}
	}
}

void CSchemaColumnsDlg::OnOK()
{
	m_pSchema = m_wndSchemas.GetSelected();

	if ( m_pSchema )
	{
		m_pColumns.RemoveAll();

		for ( int nMember = 0 ; nMember < m_wndColumns.GetItemCount() ; nMember++ )
		{
			if ( ListView_GetCheckState( m_wndColumns.GetSafeHwnd(), nMember ) )
			{
				CSchemaMember* pMember = (CSchemaMember*)m_wndColumns.GetItemData( nMember );
				m_pColumns.AddTail( pMember );
			}
		}

		SaveColumns( m_pSchema, &m_pColumns );
	}

	CSkinDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CSchemaColumnsDlg load columns utility

BOOL CSchemaColumnsDlg::LoadColumns(CSchema* pSchema, CList< CSchemaMember* >* pColumns)
{
	if ( ! pSchema || ! pColumns ) return FALSE;
	pColumns->RemoveAll();

	CString strMembers = theApp.GetProfileString( _T("Interface"),
		_T("SchemaColumns.") + pSchema->m_sSingular, _T("(EMPTY)") );

	if ( strMembers == _T("(EMPTY)") ) strMembers = pSchema->m_sDefaultColumns;

	for ( POSITION pos = pSchema->GetMemberIterator() ; pos ; )
	{
		CSchemaMember* pMember = pSchema->GetNextMember( pos );
		if ( !pMember->m_bHidden && strMembers.Find( _T("|") + pMember->m_sName + _T("|") ) >= 0 )
			pColumns->AddTail( pMember );
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSchemaColumnsDlg save columns utility

BOOL CSchemaColumnsDlg::SaveColumns(CSchema* pSchema, CList< CSchemaMember* >* pColumns)
{
	if ( ! pSchema || ! pColumns ) return FALSE;

	CString strMembers;

	for ( POSITION pos = pColumns->GetHeadPosition() ; pos ; )
	{
		CSchemaMember* pMember = (CSchemaMember*)pColumns->GetNext( pos );
		strMembers += '|';
		strMembers += pMember->m_sName;
		strMembers += '|';
	}

	theApp.WriteProfileString( _T("Interface"),
		_T("SchemaColumns.") + pSchema->m_sSingular, strMembers );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSchemaColumnsDlg menu builder utility

CMenu* CSchemaColumnsDlg::BuildColumnMenu(CSchema* pSchema, CList< CSchemaMember* >* pColumns)
{
	if ( ! pSchema ) return NULL;

	CMenu* pMenu = new CMenu();

	pMenu->CreatePopupMenu();

	UINT nID = ID_SCHEMA_MENU_MIN;

	for ( POSITION pos = pSchema->GetMemberIterator() ; pos ; nID++ )
	{
		CSchemaMember* pMember = pSchema->GetNextMember( pos );

		if ( !pMember->m_bHidden )
		{
			UINT nFlags = MF_STRING;

			if ( nID > ID_SCHEMA_MENU_MIN && ( ( nID - ID_SCHEMA_MENU_MIN ) % 16 ) == 0 )
				nFlags |= MF_MENUBREAK;
			if ( pColumns && pColumns->Find( pMember ) != NULL )
				nFlags |= MF_CHECKED;

			pMenu->AppendMenu( nFlags, nID, pMember->m_sTitle );
		}
		else
			nID--;
	}

	return pMenu;
}

/////////////////////////////////////////////////////////////////////////////
// CSchemaColumnsDlg column toggle utility

BOOL CSchemaColumnsDlg::ToggleColumnHelper(CSchema* pSchema, CList< CSchemaMember* >* pSource, CList< CSchemaMember* >* pTarget, UINT nToggleID, BOOL bSave)
{
	if ( ! pSchema ) return FALSE;

	UINT nID = ID_SCHEMA_MENU_MIN;

	pTarget->RemoveAll();
	pTarget->AddTail( pSource );

	for ( POSITION pos = pSchema->GetMemberIterator() ; pos ; nID++ )
	{
		CSchemaMember* pMember = pSchema->GetNextMember( pos );

		if ( pMember->m_bHidden )
		{
			nID--;
			continue;
		}
		if ( nID == nToggleID )
		{
			if ( ( pos = pTarget->Find( pMember ) ) != 0 )
			{
				pTarget->RemoveAt( pos );
			}
			else
			{
				pTarget->AddTail( pMember );
			}

			if ( bSave ) SaveColumns( pSchema, pTarget );

			return TRUE;
		}
	}

	return FALSE;
}

