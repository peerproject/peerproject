//
// WndSecurity.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "Security.h"
#include "LiveList.h"
#include "WndSecurity.h"
#include "DlgSecureRule.h"
#include "Colors.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSecurityWnd, CPanelWnd, 0)

BEGIN_MESSAGE_MAP(CSecurityWnd, CPanelWnd)
	//{{AFX_MSG_MAP(CSecurityWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_RULES, OnCustomDrawList)
	ON_NOTIFY(NM_DBLCLK, IDC_RULES, OnDblClkList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_RULES, OnSortList)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_EDIT, OnUpdateSecurityEdit)
	ON_COMMAND(ID_SECURITY_EDIT, OnSecurityEdit)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_RESET, OnUpdateSecurityReset)
	ON_COMMAND(ID_SECURITY_RESET, OnSecurityReset)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_REMOVE, OnUpdateSecurityRemove)
	ON_COMMAND(ID_SECURITY_REMOVE, OnSecurityRemove)
	ON_COMMAND(ID_SECURITY_ADD, OnSecurityAdd)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_POLICY_ACCEPT, OnUpdateSecurityPolicyAccept)
	ON_COMMAND(ID_SECURITY_POLICY_ACCEPT, OnSecurityPolicyAccept)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_POLICY_DENY, OnUpdateSecurityPolicyDeny)
	ON_COMMAND(ID_SECURITY_POLICY_DENY, OnSecurityPolicyDeny)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_MOVE_UP, OnUpdateSecurityMoveUp)
	ON_COMMAND(ID_SECURITY_MOVE_UP, OnSecurityMoveUp)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_MOVE_DOWN, OnUpdateSecurityMoveDown)
	ON_COMMAND(ID_SECURITY_MOVE_DOWN, OnSecurityMoveDown)
	ON_UPDATE_COMMAND_UI(ID_SECURITY_EXPORT, OnUpdateSecurityExport)
	ON_COMMAND(ID_SECURITY_EXPORT, OnSecurityExport)
	ON_COMMAND(ID_SECURITY_IMPORT, OnSecurityImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSecurityWnd construction

CSecurityWnd::CSecurityWnd()
{
	Create( IDR_SECURITYFRAME );
}

CSecurityWnd::~CSecurityWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSecurityWnd message handlers

int CSecurityWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_BORDER_TOP );

	m_wndList.Create( WS_VISIBLE|LVS_ICON|LVS_AUTOARRANGE|LVS_REPORT|LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_RULES );

	m_wndList.SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP );

	m_wndList.InsertColumn( 0, _T("Address / Content"), LVCFMT_LEFT, 200, -1 );
	m_wndList.InsertColumn( 1, _T("Hits"), LVCFMT_CENTER, 60, 0 );
	m_wndList.InsertColumn( 2, _T("#"), LVCFMT_CENTER, 30, 1 );
	m_wndList.InsertColumn( 3, _T("Action"), LVCFMT_CENTER, 60, 2 );
	m_wndList.InsertColumn( 4, _T("Expires"), LVCFMT_CENTER, 60, 3 );
	m_wndList.InsertColumn( 5, _T("Comment"), LVCFMT_LEFT, 180, 4 );

	m_pSizer.Attach( &m_wndList );

	CBitmap bmBase;
	bmBase.LoadBitmap( IDB_SECURITY );
	if ( Settings.General.LanguageRTL )
		bmBase.m_hObject = CreateMirroredBitmap( (HBITMAP) bmBase.m_hObject );

	m_gdiImageList.Create( 16, 16, ILC_MASK|ILC_COLOR32, 3, 1 ) ||
	m_gdiImageList.Create( 16, 16, ILC_MASK|ILC_COLOR24, 3, 1 ) ||
	m_gdiImageList.Create( 16, 16, ILC_MASK|ILC_COLOR16, 3, 1 );
	m_gdiImageList.Add( &bmBase, RGB( 0, 255, 0 ) );
	m_wndList.SetImageList( &m_gdiImageList, LVSIL_SMALL );

	m_wndList.SetFont( &theApp.m_gdiFont );

	LoadState( _T("CSecurityWnd"), TRUE );

	Update();

	return 0;
}

void CSecurityWnd::OnDestroy()
{
	Security.Save();

	Settings.SaveList( _T("CSecurityWnd"), &m_wndList );
	SaveState( _T("CSecurityWnd") );

	CPanelWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CSecurityWnd operations

void CSecurityWnd::Update(int nColumn, BOOL bSort)
{
	CQuickLock oLock( Security.m_pSection );

	CLiveList pLiveList( 6, Security.GetCount() + Security.GetCount() / 4u );

	CLiveItem* pDefault = pLiveList.Add( (LPVOID)0 );
	pDefault->Set( 0, _T("Default Policy") );
	pDefault->Set( 3, Security.m_bDenyPolicy ? _T("Deny") : _T("Accept") );
	pDefault->Set( 2, _T(" X ") );
	pDefault->m_nImage = Security.m_bDenyPolicy ? Settings.General.LanguageRTL ? 0 : 2 : 1;

	Security.Expire();

	DWORD nNow = static_cast< DWORD >( time( NULL ) );
	int nCount = 1;

	for ( POSITION pos = Security.GetIterator() ; pos ; nCount++ )
	{
		CSecureRule* pRule = Security.GetNext( pos );

		CLiveItem* pItem = pLiveList.Add( pRule );

		pItem->m_nImage = Settings.General.LanguageRTL ? 2 - pRule->m_nAction : pRule->m_nAction;

		if ( pRule->m_nType == CSecureRule::srAddress )
		{
			if ( *(DWORD*)pRule->m_nMask == 0xFFFFFFFF )
			{
				pItem->Format( 0, _T("%u.%u.%u.%u"),
					unsigned( pRule->m_nIP[0] ), unsigned( pRule->m_nIP[1] ),
					unsigned( pRule->m_nIP[2] ), unsigned( pRule->m_nIP[3] ) );
			}
			else
			{
				pItem->Format( 0, _T("%u.%u.%u.%u/%u.%u.%u.%u"),
					unsigned( pRule->m_nIP[0] ), unsigned( pRule->m_nIP[1] ),
					unsigned( pRule->m_nIP[2] ), unsigned( pRule->m_nIP[3] ),
					unsigned( pRule->m_nMask[0] ), unsigned( pRule->m_nMask[1] ),
					unsigned( pRule->m_nMask[2] ), unsigned( pRule->m_nMask[3] ) );
			}
		}
		else
		{
			pItem->Set( 0, pRule->GetContentWords() );
		}

		switch ( pRule->m_nAction )
		{
		case CSecureRule::srNull:
			pItem->Set( 3, _T("N/A") );
			break;
		case CSecureRule::srAccept:
			pItem->Set( 3, _T("Accept") );
			break;
		case CSecureRule::srDeny:
			pItem->Set( 3, _T("Deny") );
			break;
		}

		if ( pRule->m_nExpire == CSecureRule::srIndefinite )
		{
			pItem->Set( 4, _T("Never") );
		}
		else if ( pRule->m_nExpire == CSecureRule::srSession )
		{
			pItem->Set( 4, _T("Session") );
		}
		else if ( pRule->m_nExpire >= nNow )
		{
			DWORD nTime = ( pRule->m_nExpire - nNow );
			pItem->Format( 4, _T("%ud %uh %um"), nTime / 86400u, (nTime % 86400u) / 3600u, ( nTime % 3600u ) / 60u );
			//pItem->Format( 4, _T("%i:%.2i:%.2i"), nTime / 3600, ( nTime % 3600 ) / 60, nTime % 60 );
		}

		pItem->Format( 1, _T("%u (%u)"), pRule->m_nToday, pRule->m_nEver );
		pItem->Format( 2, _T("%i"), nCount );
		pItem->Set( 5, pRule->m_sComment );
	}

	if ( nColumn >= 0 )
		SetWindowLongPtr( m_wndList.GetSafeHwnd(), GWLP_USERDATA, 0 - nColumn - 1 );

	pLiveList.Apply( &m_wndList, bSort );

	tLastUpdate = GetTickCount();		// Update time after it's done doing its work
}

CSecureRule* CSecurityWnd::GetItem(int nItem)
{
	if ( m_wndList.GetItemState( nItem, LVIS_SELECTED ) )
	{
		CSecureRule* pRule = (CSecureRule*)m_wndList.GetItemData( nItem );
		if ( Security.Check( pRule ) ) return pRule;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CSecurityWnd message handlers

void CSecurityWnd::OnSize(UINT nType, int cx, int cy)
{
	if ( ! m_wndList ) return;

	CPanelWnd::OnSize( nType, cx, cy );
	m_wndList.SetWindowPos( NULL, 0, 0, cx, cy - Skin.m_nToolbarHeight, SWP_NOZORDER );
	SizeListAndBar( &m_wndList, &m_wndToolBar );
}

void CSecurityWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( ( nIDEvent == 1 ) && ( IsPartiallyVisible() ) )
	{
		DWORD tTicks = GetTickCount();
		DWORD tDelay = max( ( 2 * (DWORD)Security.GetCount() ), 1000ul ); // Delay based on size of list

		if ( ( tTicks - tLastUpdate ) > tDelay )
		{
			if ( tDelay < 2000 ) Update();			// Sort if list is under 1000
			else Update( -1, FALSE );				// Otherwise just refresh values
		}
	}
}

void CSecurityWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;

	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		LV_ITEM pItem;
		pItem.mask		= LVIF_IMAGE;
		pItem.iItem		= static_cast< int >( pDraw->nmcd.dwItemSpec );
		pItem.iSubItem	= 0;
		m_wndList.GetItem( &pItem );

		switch ( Settings.General.LanguageRTL ? 2 - pItem.iImage : pItem.iImage )
		{
		case CSecureRule::srAccept:
			pDraw->clrText = Colors.m_crSecurityAllow ;
			break;
		case CSecureRule::srDeny:
			pDraw->clrText = Colors.m_crSecurityDeny ;
			break;
		}

		*pResult = CDRF_DODEFAULT;
	}
}

void CSecurityWnd::OnDblClkList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	OnSecurityEdit();
	*pResult = 0;
}

void CSecurityWnd::OnSortList(NMHDR* pNotifyStruct, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNotifyStruct;
	CLiveList::Sort( &m_wndList, pNMListView->iSubItem );
	*pResult = 0;
}

void CSecurityWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	Skin.TrackPopupMenu( _T("CSecurityWnd"), point, ID_SECURITY_EDIT );
}

void CSecurityWnd::OnUpdateSecurityEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() == 1 );
}

void CSecurityWnd::OnSecurityEdit()
{
	CSecureRule* pEditableRule;
	{
		CQuickLock oLock( Security.m_pSection );

		CSecureRule* pRule = GetItem( m_wndList.GetNextItem( -1, LVIS_SELECTED ) );
		if ( ! pRule ) return;
		pEditableRule = new CSecureRule( *pRule );
	}

	CSecureRuleDlg dlg( NULL, pEditableRule );
	if ( dlg.DoModal() == IDOK )
	{
		Security.Save();
		Update();
	}
	else
		delete pEditableRule;
}

void CSecurityWnd::OnUpdateSecurityReset(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CSecurityWnd::OnSecurityReset()
{
	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
	{
		CQuickLock oLock( Security.m_pSection );

		if ( CSecureRule* pRule = GetItem( nItem ) )
			pRule->Reset();
	}

	Security.Save();
	Update();
}

void CSecurityWnd::OnUpdateSecurityRemove(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CSecurityWnd::OnSecurityRemove()
{
	CString strMessage;
	LoadString( strMessage, IDS_SECURITY_REMOVE_CONFIRM );
	if ( AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) != IDYES ) return;

	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
	{
		CQuickLock oLock( Security.m_pSection );

		if ( CSecureRule* pRule = GetItem( nItem ) )
			Security.Remove( pRule );
	}

	Security.Save();
	Update();
}

void CSecurityWnd::OnUpdateSecurityMoveUp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CSecurityWnd::OnSecurityMoveUp()
{
	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
	{
		CQuickLock oLock( Security.m_pSection );

		if ( CSecureRule* pRule = GetItem( nItem ) )
			Security.MoveUp( pRule );
	}

	Security.Save();
	Update( 2 );
}

void CSecurityWnd::OnUpdateSecurityMoveDown(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CSecurityWnd::OnSecurityMoveDown()
{
	CQuickLock oLock( Security.m_pSection );

	CList< CSecureRule* > pList;

	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
	{
		pList.AddHead( GetItem( nItem ) );
	}

	while ( pList.GetCount() )
	{
		CSecureRule* pRule = pList.RemoveHead();
		if ( pRule ) Security.MoveDown( pRule );
	}

	Security.Save();
	Update( 2 );
}

void CSecurityWnd::OnSecurityAdd()
{
	CSecureRuleDlg dlg;

	if ( dlg.DoModal() == IDOK )
	{
		Security.Save();
		Update();
	}
}

void CSecurityWnd::OnUpdateSecurityExport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CSecurityWnd::OnSecurityExport()
{
	CFileDialog dlg( FALSE, _T("xml"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("XML Security Files|*.xml|NET Security Files|*.net|All Files|*.*||") );

	if ( dlg.DoModal() != IDOK ) return;

	CString strText;
	CFile pFile;

	if ( ! pFile.Open( dlg.GetPathName(), CFile::modeWrite|CFile::modeCreate ) )
	{
		// TODO: Error
		AfxMessageBox( _T("Error") );
		return;
	}

	CWaitCursor pCursor;

	if ( dlg.GetFileExt().CompareNoCase( _T("net") ) == 0 )
	{
		for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
		{
			CQuickLock oLock( Security.m_pSection );

			if ( CSecureRule* pRule = GetItem( nItem ) )
			{
				strText = pRule->ToGnucleusString();

				if ( strText.GetLength() )
				{
					strText += _T("\r\n");

					int nBytes = WideCharToMultiByte( CP_ACP, 0, strText, strText.GetLength(), NULL, 0, NULL, NULL );
					LPSTR pBytes = new CHAR[nBytes];
					WideCharToMultiByte( CP_ACP, 0, strText, strText.GetLength(), pBytes, nBytes, NULL, NULL );
					pFile.Write( pBytes, nBytes );
					delete [] pBytes;
				}
			}
		}
	}
	else
	{
		auto_ptr< CXMLElement > pXML( new CXMLElement( NULL, _T("security") ) );

		pXML->AddAttribute( _T("xmlns"), CSecurity::xmlns );

		for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
		{
			CQuickLock oLock( Security.m_pSection );

			if ( CSecureRule* pRule = GetItem( nItem ) )
				pXML->AddElement( pRule->ToXML() );
		}

		strText = pXML->ToString( TRUE, TRUE );

		int nBytes = WideCharToMultiByte( CP_ACP, 0, strText, strText.GetLength(), NULL, 0, NULL, NULL );
		auto_ptr< CHAR > pBytes( new CHAR[ nBytes ] );
		WideCharToMultiByte( CP_ACP, 0, strText, strText.GetLength(), pBytes.get(), nBytes, NULL, NULL );
		pFile.Write( pBytes.get(), nBytes );
	}

	pFile.Close();
}

void CSecurityWnd::OnSecurityImport()
{
	CFileDialog dlg( TRUE, _T("xml"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("Security Rules|*.xml;*.net|XML Files|*.xml|NET Files|*.net|All Files|*.*||") );

	if ( dlg.DoModal() != IDOK ) return;

	CWaitCursor pCursor;

	if ( Security.Import( dlg.GetPathName() ) )
		Security.Save();
	else
		AfxMessageBox( _T("Error") );	// ToDo: Error message, unable to import rules
}

void CSecurityWnd::OnSkinChange()
{
	OnSize( 0, 0, 0 );
	CPanelWnd::OnSkinChange();
	Settings.LoadList( _T("CSecurityWnd"), &m_wndList, -3 );
	Skin.CreateToolBar( _T("CSecurityWnd"), &m_wndToolBar );

	m_wndList.SetBkImage( Skin.GetWatermark( _T("CSecurityWnd") ) );
}

void CSecurityWnd::OnUpdateSecurityPolicyAccept(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( Security.m_bDenyPolicy == FALSE );
}

void CSecurityWnd::OnSecurityPolicyAccept()
{
	Security.m_bDenyPolicy = FALSE;
	Update();
	m_wndList.RedrawItems( 0, m_wndList.GetItemCount() - 1 );
}

void CSecurityWnd::OnUpdateSecurityPolicyDeny(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( Security.m_bDenyPolicy == TRUE );
}

void CSecurityWnd::OnSecurityPolicyDeny()
{
	Security.m_bDenyPolicy = TRUE;
	Update();
	m_wndList.RedrawItems( 0, m_wndList.GetItemCount() - 1 );
}

BOOL CSecurityWnd::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
		{
			if ( pMsg->wParam == VK_UP )
			{
				OnSecurityMoveUp();
				return TRUE;
			}
			else if ( pMsg->wParam == VK_DOWN )
			{
				OnSecurityMoveDown();
				return TRUE;
			}
		}
		else if ( pMsg->wParam == VK_DELETE )
		{
			OnSecurityRemove();
			return TRUE;
		}
		else if ( pMsg->wParam == VK_INSERT )
		{
			PostMessage( WM_COMMAND, ID_SECURITY_ADD );
			return TRUE;
		}
	}

	return CPanelWnd::PreTranslateMessage( pMsg );
}
