//
// PageProfileFavourites.cpp
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
#include "GProfile.h"
#include "XML.h"
#include "PageProfileFavourites.h"
#include "Skin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CFavouritesProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CFavouritesProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CFavouritesProfilePage)
	ON_EN_CHANGE(IDC_WEB_NAME, OnChangeWebName)
	ON_EN_CHANGE(IDC_WEB_URL, OnChangeWebUrl)
	ON_BN_CLICKED(IDC_WEB_ADD, OnWebAdd)
	ON_BN_CLICKED(IDC_WEB_REMOVE, OnWebRemove)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_WEB_LIST, OnItemChangedWebList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFavouritesProfilePage property page

CFavouritesProfilePage::CFavouritesProfilePage() : CSettingsPage( CFavouritesProfilePage::IDD )
{
	//{{AFX_DATA_INIT(CFavouritesProfilePage)
	m_sURL = _T("");
	m_sTitle = _T("");
	//}}AFX_DATA_INIT
}

CFavouritesProfilePage::~CFavouritesProfilePage()
{
}

void CFavouritesProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFavouritesProfilePage)
	DDX_Control(pDX, IDC_WEB_REMOVE, m_wndRemove);
	DDX_Control(pDX, IDC_WEB_ADD, m_wndAdd);
	DDX_Control(pDX, IDC_WEB_LIST, m_wndList);
	DDX_Text(pDX, IDC_WEB_URL, m_sURL);
	DDX_Text(pDX, IDC_WEB_NAME, m_sTitle);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFavouritesProfilePage message handlers

BOOL CFavouritesProfilePage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_gdiImageList.Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 1 ) ||
	m_gdiImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 1, 1 ) ||
	m_gdiImageList.Create( 16, 16, ILC_COLOR16|ILC_MASK, 1, 1 );
	AddIcon( IDI_WEB_URL, m_gdiImageList );

	CRect rc;
	m_wndList.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL ) + 1;

	m_wndList.SetImageList( &m_gdiImageList, LVSIL_SMALL );
	m_wndList.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, rc.right / 2, -1 );
	m_wndList.InsertColumn( 1, _T("URL"), LVCFMT_LEFT, rc.right / 2, 0 );

	m_wndList.SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE,
		LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );

	if ( CXMLElement* pBookmarks = MyProfile.GetXML( _T("bookmarks") ) )
	{
		for ( POSITION pos = pBookmarks->GetElementIterator() ; pos ; )
		{
			CXMLElement* pBookmark = pBookmarks->GetNextElement( pos );

			if ( pBookmark->IsNamed( _T("bookmark") ) )
			{
				CString strTitle	= pBookmark->GetAttributeValue( _T("title") );
				CString strURL		= pBookmark->GetAttributeValue( _T("url") );
				if ( Settings.General.LanguageRTL ) strURL = _T("\x202A") + strURL;

				int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE,
					m_wndList.GetItemCount(), strTitle, 0, 0, 0, 0 );
				m_wndList.SetItemText( nItem, 1, strURL );
			}
		}
	}

	Skin.Translate( _T("CFavouritesProfileList"), m_wndList.GetHeaderCtrl() );
	UpdateData( FALSE );
	m_wndAdd.EnableWindow( FALSE );
	m_wndRemove.EnableWindow( FALSE );

	return TRUE;
}

void CFavouritesProfilePage::OnChangeWebName()
{
	UpdateData();
	m_wndAdd.EnableWindow( m_sTitle.GetLength() && m_sURL.Find( _T("http://") ) == 0 );
}

void CFavouritesProfilePage::OnChangeWebUrl()
{
	UpdateData();
	m_wndAdd.EnableWindow( m_sTitle.GetLength() && m_sURL.Find( _T("http://") ) == 0 );
}

void CFavouritesProfilePage::OnItemChangedWebList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;
	m_wndRemove.EnableWindow( m_wndList.GetSelectedCount() > 0 );
}

void CFavouritesProfilePage::OnWebAdd()
{
	UpdateData();

	int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE,
		m_wndList.GetItemCount(), m_sTitle, 0, 0, 0, 0 );
	m_wndList.SetItemText( nItem, 1, Settings.General.LanguageRTL ? _T("\x202A") + m_sURL : m_sURL );

	m_sTitle.Empty();
	m_sURL.Empty();

	UpdateData( FALSE );
	m_wndAdd.EnableWindow( FALSE );
}

void CFavouritesProfilePage::OnWebRemove()
{
	for ( int nItem = m_wndList.GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		if ( m_wndList.GetItemState( nItem, LVIS_SELECTED ) )
		{
			m_wndList.DeleteItem( nItem );
		}
	}

	m_wndRemove.EnableWindow( FALSE );
}

void CFavouritesProfilePage::OnOK()
{
	UpdateData();

	if ( CXMLElement* pBookmarks = MyProfile.GetXML( _T("bookmarks"), TRUE ) )
	{
		pBookmarks->DeleteAllElements();

		for ( int nItem = 0 ; nItem < m_wndList.GetItemCount() ; nItem++ )
		{
			CXMLElement* pBookmark = pBookmarks->AddElement( _T("bookmark") );
			pBookmark->AddAttribute( _T("title"), m_wndList.GetItemText( nItem, 0 ) );
			pBookmark->AddAttribute( _T("url"), m_wndList.GetItemText( nItem, 1 ) );
		}

		if ( pBookmarks->GetElementCount() == 0 ) pBookmarks->Delete();
	}

	CSettingsPage::OnOK();
}

