//
// ComboListCtrl.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2008.
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

// ComboBoxes and ToolTips currently Disabled (Using Checkboxes and Labeltips)

#include "StdAfx.h"
#include "PeerProject.h"
#include "ComboListCtrl.h"
#include "Settings.h"


IMPLEMENT_DYNAMIC(CComboListCtrl, CListCtrl)

CComboListCtrl::CComboListCtrl()
	: m_iSelectedItem	( -1 )
	, m_iSelectedSubItem( -1 )
//	, m_pCombo			( NULL )	// Disabled
{
	EnableActiveAccessibility();
}

CComboListCtrl::~CComboListCtrl()
{
}

int CComboListCtrl::GetColumnData(int iItem, int iColumn) const
{
	CIntIntMap::const_iterator i = m_oData[ iItem ].find( iColumn );
	return (*i).second;
}

void CComboListCtrl::SetColumnData(int iItem, int iColumn, int iData)
{
	if ( iItem >= (int)m_oData.size() )
		m_oData.resize( iItem + 1 );
	m_oData[ iItem ].insert( CIntIntMap::value_type( iColumn, iData ) );

	CIntIntStringMapMap::const_iterator column = m_oColumns.find( iColumn );
	CIntStringMap::const_iterator value = (*column).second.find( iData );
	SetItemText( iItem, iColumn, (*value).second );
}

void CComboListCtrl::SetColumnValues(int iColumn, const CIntStringMap& oValues)
{
	m_oColumns.insert( CIntIntStringMapMap::value_type( iColumn, oValues ) );
}

///////////////////////////////////////////////////////////////////////////////
// DISABLED: Priority Column Dropdown Boxes, and Custom Hover ToolTips

//BEGIN_MESSAGE_MAP(CComboListCtrl, CListCtrl)
//	ON_NOTIFY_REFLECT(NM_CLICK, &CComboListCtrl::OnNMClick)
//	ON_WM_MOUSEMOVE()
//END_MESSAGE_MAP()
//
// CComboListCtrl message handlers
//
//void CComboListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );
//	*pResult = 0;
//
//	Hide();
//
//	int iItem = HitTest( pNMItemActivate->ptAction );
//	if ( iItem >= 0 && pNMItemActivate->iSubItem > 0 )
//		Show( iItem, pNMItemActivate->iSubItem );
//}
//
//int CComboListCtrl::HitTest(const CPoint& ptAction)
//{
//	CPoint pt( 16, ptAction.y );
//	UINT flags = 0;
//	return CListCtrl::HitTest( pt, &flags );
//}
//
//void CComboListCtrl::OnMouseMove(UINT nFlags, CPoint point)
//{
//	if ( m_pCombo )
//	{
//		if ( m_pTip.get() ) m_pTip->Hide();
//
//		CPoint pt;
//		GetCursorPos( &pt );
//		CRect rc;
//		m_pCombo->GetWindowRect( rc );
//		if ( ! rc.PtInRect( pt ) )
//			Hide();
//	}
//	else if ( m_pTip.get() )
//	{
//		int iItem = HitTest( point );
//		if ( iItem >= 0 )
//			m_pTip->Show( (CPeerProjectFile**)GetItemData( iItem ) );
//		else
//			m_pTip->Hide();
//	}
//	CListCtrl::OnMouseMove(nFlags, point);
//}
//
//void CComboListCtrl::Show(int iItem, int iSubItem)
//{
//	CIntIntStringMapMap::const_iterator column = m_oColumns.find( iSubItem );
//	if ( column != m_oColumns.end() )
//	{
//		CIntIntMap::const_iterator data = m_oData[ iItem ].find( iSubItem );
//
//		CRect rc;
//		if ( GetSubItemRect( iItem, iSubItem, LVIR_BOUNDS, rc ) )
//		{
//			m_pCombo = new CComboBox();
//			m_pCombo->Create( WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST, rc, this, 100 );
//			m_pCombo->SetFont( GetFont() );
//
//			int n = 0, nSelected = 0;
//			for( CIntStringMap::const_iterator i = (*column).second.begin();
//				i != (*column).second.end(); ++i, ++n )
//			{
//				m_pCombo->AddString( (*i).second );
//				if ( (*i).first == (*data).second )
//					nSelected = n;
//			}
//			m_pCombo->SetCurSel( nSelected );
//			m_pCombo->SetFocus();
//			m_pCombo->ShowDropDown();
//			m_iSelectedItem = iItem;
//			m_iSelectedSubItem = iSubItem;
//		}
//	}
//}

//void CComboListCtrl::Hide()
//{
//	if ( m_pCombo )
//	{
//		CIntIntStringMapMap::const_iterator column =
//			m_oColumns.find( m_iSelectedSubItem );
//		CIntStringMap::const_iterator value = (*column).second.begin();
//		int nSelected = m_pCombo->GetCurSel();
//		for( int i = 0; i != nSelected; ++value, ++i );
//
//		m_oData[ m_iSelectedItem ].erase( m_iSelectedSubItem );
//		m_oData[ m_iSelectedItem ].insert(
//			CIntIntMap::value_type( m_iSelectedSubItem, (*value).first ) );
//
//		SetItemText( m_iSelectedItem, m_iSelectedSubItem, (*value).second );
//
//		m_pCombo->DestroyWindow();
//		delete m_pCombo;
//		m_pCombo = NULL;
//
//		m_iSelectedItem = -1;
//		m_iSelectedSubItem = -1;
//	}
//}

//void CComboListCtrl::EnableTips(auto_ptr< CLibraryTipCtrl > pTip)
//{
//	m_pTip = pTip;
//}