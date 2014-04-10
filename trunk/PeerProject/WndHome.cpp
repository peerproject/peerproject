//
// WndHome.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "RichDocument.h"
#include "RichElement.h"
#include "Network.h"
#include "Neighbours.h"
#include "Library.h"
#include "LibraryHistory.h"
#include "SharedFile.h"
#include "VersionChecker.h"
#include "GraphItem.h"
#include "Statistics.h"

#include "Skin.h"
#include "WndHome.h"
#include "WndSearch.h"
#include "WndBrowseHost.h"
#include "WindowManager.h"
#include "XML.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CHomeWnd, CPanelWnd)

BEGIN_MESSAGE_MAP(CHomeWnd, CPanelWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MDIACTIVATE()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_LIBRARY_CLEAR_HISTORY, OnUpdateLibraryClear)
	ON_COMMAND(ID_LIBRARY_CLEAR_HISTORY, OnLibraryClear)
	ON_COMMAND(ID_LIBRARY_CLEAR_HISTORY_ALL, OnLibraryClearAll)
	ON_NOTIFY(RVN_CLICK, IDC_HOME_VIEW, OnClickView)
	ON_NOTIFY(RVN_CLICK, 1, OnClickView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHomeWnd construction

CHomeWnd::CHomeWnd() : CPanelWnd( TRUE )
{
	Create( IDR_HOMEFRAME );
}

/////////////////////////////////////////////////////////////////////////////
// CHomeWnd message handlers

int CHomeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	if ( ! m_wndView.Create( rectDefault, this ) ) return -1;
	if ( ! m_wndPanel.Create( this ) ) return -1;

	LoadState();

	OnSkinChange();

	return 0;
}

void CHomeWnd::OnDestroy()
{
	SaveState();
	CPanelWnd::OnDestroy();
}

void CHomeWnd::OnSkinChange()
{
	CRect rc;
	GetClientRect( &rc );
	OnSize( 0, rc.Width(), rc.Height() );

	CPanelWnd::OnSkinChange();

	m_wndView.OnSkinChange();
	m_wndPanel.OnSkinChange();
}

void CHomeWnd::OnSize(UINT nType, int cx, int cy)
{
	CPanelWnd::OnSize( nType, cx, cy );

	if ( m_wndPanel.m_hWnd ) m_wndPanel.SetWindowPos( NULL, 0, 0, Settings.Skin.SidebarWidth, cy, SWP_NOZORDER );
	if ( m_wndView.m_hWnd )  m_wndView.SetWindowPos( NULL, Settings.Skin.SidebarWidth, 0, cx - Settings.Skin.SidebarWidth, cy, SWP_NOZORDER|SWP_SHOWWINDOW );
}

void CHomeWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 2 || ( nIDEvent == 1 && IsActive() ) )
	{
		m_wndView.Update();
		m_wndPanel.Update();
	}
}

void CHomeWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate )
	{
		m_wndView.Update();
		m_wndPanel.Update();

		m_wndView.Activate();
	}

	CPanelWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}

void CHomeWnd::OnClickView(NMHDR* pNotify, LRESULT* /*pResult*/)
{
	if ( CRichElement* pElement = ((RVN_ELEMENTEVENT*)pNotify)->pElement )
		theApp.InternalURI( pElement->m_sLink );

	PostMessage( WM_TIMER, 2 );
}

void CHomeWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if ( point.x == -1 && point.y == -1 )	// Keyboard fix
		ClientToScreen( &point );

	Skin.TrackPopupMenu( _T("CHomeWnd"), point );
}

// Context menu command handlers for CtrlHomePanel:

void CHomeWnd::OnUpdateLibraryClear(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndPanel.m_boxLibrary.m_nIndex >= 0 );
}

void CHomeWnd::OnLibraryClear()
{
	if ( m_wndPanel.m_boxLibrary.m_nIndex < 0 ) return;

	CSingleLock pLock( &Library.m_pSection );
	if ( ! SafeLock( pLock ) ) return;

	if ( CLibraryFile* pFile = Library.LookupFile( m_wndPanel.m_boxLibrary.m_nIndex ) )
	{
		LibraryHistory.OnFileDelete( pFile );
		Library.Update();
		pLock.Unlock();
		m_wndPanel.m_boxLibrary.Invalidate();
	}
}

void CHomeWnd::OnLibraryClearAll()
{
	CSingleLock pLock( &Library.m_pSection );
	if ( ! SafeLock( pLock ) ) return;

	//Settings.ClearSearches();
	LibraryHistory.Clear();
	Library.Update();
	pLock.Unlock();
	m_wndPanel.Invalidate();
}
