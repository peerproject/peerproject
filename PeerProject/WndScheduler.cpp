//
// WndScheduler.cpp
//
// This file is part of PeerProject (peerproject.org) © 2010
// Portions Copyright Shareaza Development Team, 2010.
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
#include "Scheduler.h"
#include "DlgScheduleTask.h"
#include "WndScheduler.h"
#include "Network.h"
#include "LiveList.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Skin.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//const static UINT nImageID[] =
//{
//	IDR_SCHEDULERFRAME,
//	IDI_NOTASK,
//	ID_SCHEDULER_ACTIVATE,
//	ID_SCHEDULER_DEACTIVATE,
//	NULL
//};

IMPLEMENT_SERIAL(CSchedulerWnd, CPanelWnd, 0)

BEGIN_MESSAGE_MAP(CSchedulerWnd, CPanelWnd)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SCHEDULE, OnCustomDrawList)
	ON_NOTIFY(NM_DBLCLK, IDC_SCHEDULE, OnDblClkList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_SCHEDULE, OnSortList)
	ON_COMMAND(ID_SCHEDULER_ADD, OnSchedulerAdd)
	ON_UPDATE_COMMAND_UI(ID_SCHEDULER_ACTIVATE, OnUpdateSchedulerActivate)
	ON_COMMAND(ID_SCHEDULER_ACTIVATE, OnSchedulerActivate)
	ON_UPDATE_COMMAND_UI(ID_SCHEDULER_DEACTIVATE, OnUpdateSchedulerDeactivate)
	ON_COMMAND(ID_SCHEDULER_DEACTIVATE, OnSchedulerDeactivate)
	ON_UPDATE_COMMAND_UI(ID_SCHEDULER_EDIT, OnUpdateSchedulerEdit)
	ON_COMMAND(ID_SCHEDULER_EDIT, OnSchedulerEdit)
	ON_UPDATE_COMMAND_UI(ID_SCHEDULER_REMOVE, OnUpdateSchedulerRemove)
	ON_COMMAND(ID_SCHEDULER_REMOVE, OnSchedulerRemove)
	ON_UPDATE_COMMAND_UI(ID_SCHEDULER_REMOVE_ALL, OnUpdateSchedulerRemoveAll)
	ON_COMMAND(ID_SCHEDULER_REMOVE_ALL, OnSchedulerRemoveAll)
	ON_UPDATE_COMMAND_UI(ID_SCHEDULER_EXPORT, OnUpdateSchedulerExport)
	ON_COMMAND(ID_SCHEDULER_EXPORT, OnSchedulerExport)
	ON_COMMAND(ID_SCHEDULER_IMPORT, OnSchedulerImport)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSchedulerWnd construction

CSchedulerWnd::CSchedulerWnd()
{
	Create( IDR_SCHEDULERFRAME );
}

CSchedulerWnd::~CSchedulerWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSchedulerWnd message handlers

int CSchedulerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPanelWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_BORDER_TOP );

	m_wndList.Create( WS_VISIBLE|LVS_ICON|LVS_AUTOARRANGE|LVS_REPORT|LVS_SHOWSELALWAYS, rectDefault, this, IDC_SCHEDULE );
	m_wndList.SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP );

	m_pSizer.Attach( &m_wndList );

	m_wndList.InsertColumn( 0, _T("Task"), LVCFMT_LEFT, 240, -1 );
	m_wndList.InsertColumn( 1, _T("Date"), LVCFMT_CENTER, 220, -1 );
	m_wndList.InsertColumn( 2, _T("Time"), LVCFMT_CENTER, 90, -1 );
	m_wndList.InsertColumn( 3, _T("Status"), LVCFMT_CENTER, 90, -1);
	m_wndList.InsertColumn( 4, _T("Activity"), LVCFMT_CENTER, 90, -1 );
	m_wndList.InsertColumn( 5, _T("Description"), LVCFMT_LEFT, 280, -1 );

	m_wndList.SetFont( &theApp.m_gdiFont );

	LoadState( _T("CSchedulerWnd"), TRUE );

	Update();

	return 0;
}

void CSchedulerWnd::OnDestroy()
{
	Scheduler.Save();

	Settings.SaveList( _T("CSchedulerWnd"), &m_wndList );
	SaveState( _T("CSchedulerWnd") );

	CPanelWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CSchedulerWnd operations

void CSchedulerWnd::Update(int nColumn, BOOL bSort)
{
	CQuickLock oLock( Scheduler.m_pSection );
	CLiveList pLiveList( 6, Scheduler.GetCount() + Scheduler.GetCount() / 4u );

	int nCount = 1;
	for ( POSITION pos = Scheduler.GetIterator() ; pos ; nCount++ )
	{
		CScheduleTask* pSchTask = Scheduler.GetNext( pos );

		// Adding tasks we got from Scheduler to temp list and getting a handle
		// to modify their properties according to scheduler item.
		CLiveItem* pItem = pLiveList.Add( pSchTask );

		if ( pSchTask->m_bActive )
			pItem->m_nImage = SCHEDULE_ITEM_ACTIVE;
		else
			pItem->m_nImage = SCHEDULE_ITEM_INACTIVE;

		// Action column
		switch ( pSchTask->m_nAction )
		{
		case BANDWIDTH_FULL:
			pItem->Set( 0, LoadString( IDS_SCHEDULER_BANDWIDTH_FULL ) );
			break;
		case BANDWIDTH_LIMITED:
			pItem->Set( 0, LoadString( IDS_SCHEDULER_BANDWIDTH_LIMITED ) );
			break;
		case BANDWIDTH_STOP:
			pItem->Set( 0, LoadString( IDS_SCHEDULER_BANDWIDTH_STOP ) );
			break;
		case SYSTEM_EXIT:
			pItem->Set( 0, LoadString( IDS_SCHEDULER_SYSTEM_EXIT ) );
			break;
		case SYSTEM_SHUTDOWN:
			pItem->Set( 0, LoadString( IDS_SCHEDULER_SYSTEM_SHUTDOWN ) );
			break;
		case SYSTEM_DISCONNECT:
			pItem->Set( 0, LoadString( IDS_SCHEDULER_SYSTEM_DIALUP_OFF ) );
			break;
		case SYSTEM_NOTICE:
			pItem->Set( 0, LoadString( IDS_SCHEDULER_SYSTEM_NOTICE ) );
			break;
		}

		// Date column
		if ( ! pSchTask->m_bSpecificDays )		// One time event
		{
			pItem->Set( 1, pSchTask->m_tScheduleDateTime.Format( _T("%A, %B %m, %Y") ) );
		}
		else if ( pSchTask->m_nDays == 0x7F )	// All days flagged
		{
			pItem->Set( 1, LoadString( IDS_DAY_EVERYDAY ) );
		}
		else	// Specific days flagged
		{
			CString strDays;
			if ( pSchTask->m_nDays & MONDAY )
				strDays += LoadString( IDS_DAY_MONDAY ) + _T(" ");
			if ( pSchTask->m_nDays & TUESDAY )
				strDays += LoadString( IDS_DAY_TUESDAY ) + _T(" ");
			if ( pSchTask->m_nDays & WEDNESDAY )
				strDays += LoadString( IDS_DAY_WEDNESDAY ) + _T(" ");
			if ( pSchTask->m_nDays & THURSDAY )
				strDays += LoadString( IDS_DAY_THURSDAY ) + _T(" ");
			if ( pSchTask->m_nDays & FRIDAY )
				strDays += LoadString( IDS_DAY_FRIDAY ) + _T(" ");
			if ( pSchTask->m_nDays & SATURDAY )
				strDays += LoadString( IDS_DAY_SATURDAY ) + _T(" ");
			if ( pSchTask->m_nDays & SUNDAY )
				strDays += LoadString( IDS_DAY_SUNDAY );

			pItem->Set( 1, strDays );
		}

		// Time column
		pItem->Set( 2, pSchTask->m_tScheduleDateTime.Format( _T("%I:%M:%S %p") ) );

		// Status column
		if ( pSchTask->m_bActive)
			pItem->Set( 3, LoadString( pSchTask->m_bExecuted ? IDS_SCHEDULER_TASK_DONETODAY : IDS_SCHEDULER_TASK_WAITING ) );
		else
			pItem->Set( 3, LoadString( pSchTask->m_bExecuted ? IDS_SCHEDULER_TASK_DONE : IDS_SCHEDULER_TASK_INACTIVE ) );

		// Active column
		pItem->Set( 4, LoadString( pSchTask->m_bActive ? IDS_SCHEDULER_TASK_ACTIVE : IDS_SCHEDULER_TASK_INACTIVE ) );

		// Description column
		pItem->Set( 5, pSchTask->m_sDescription );
	}

	// In case scheduler gave nothing
	if ( nCount == 1 )
	{
		CLiveItem* pDefault = pLiveList.Add( (LPVOID)0 );
		pDefault->Set( 0, LoadString( IDS_SCHEDULER_NOTASK ) );
		pDefault->m_nImage = SCHEDULE_NO_ITEM;
	}

	if ( nColumn >= 0 )
		SetWindowLong( m_wndList.GetSafeHwnd(), GWLP_USERDATA, 0 - nColumn - 1 );

	pLiveList.Apply( &m_wndList, bSort );	// Putting items in the main list

	tLastUpdate = GetTickCount();	// Update time after it's done doing its work

}

CScheduleTask* CSchedulerWnd::GetItem(int nItem)
{
	if ( nItem > -1 )
	{
		CScheduleTask* pSchTask = (CScheduleTask*)m_wndList.GetItemData( nItem );
		if ( Scheduler.Check( pSchTask ) )
			return pSchTask;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CSchedulerWnd message handlers

void CSchedulerWnd::OnSize(UINT nType, int cx, int cy)
{
	CPanelWnd::OnSize( nType, cx, cy );

	SizeListAndBar( &m_wndList, &m_wndToolBar );
	m_wndList.SetWindowPos( NULL, 0, 0, cx, cy - 28, SWP_NOZORDER );
}

void CSchedulerWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1 && IsPartiallyVisible() )
	{
		DWORD tTicks = GetTickCount();
		DWORD tDelay = max( ( 2 * (DWORD)Scheduler.GetCount() ), 1000ul ); // Delay based on size of list

		if ( ( tTicks - tLastUpdate ) > tDelay )
		{
			if ( tDelay < 2000 )
				Update();				// Sort if list is under 1000
			else
				Update( -1, FALSE );	// Otherwise just refresh values
		}
	}
}

void CSchedulerWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
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

		switch ( pItem.iImage )
		{
		case 2:
			pDraw->clrText = RGB( 0, 127, 0 );
			break;
		case 3:
			pDraw->clrText = RGB( 255, 0, 0 );
			break;
		}

		*pResult = CDRF_DODEFAULT;
	}
}

void CSchedulerWnd::OnDblClkList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	OnSchedulerEdit();
	*pResult = 0;
}

void CSchedulerWnd::OnSortList(NMHDR* pNotifyStruct, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNotifyStruct;
	CLiveList::Sort( &m_wndList, pNMListView->iSubItem );
	*pResult = 0;
}

void CSchedulerWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	Skin.TrackPopupMenu( _T("CSchedulerWnd"), point, ID_SCHEDULER_EDIT );
}

void CSchedulerWnd::OnUpdateSchedulerEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() == 1 );
}

void CSchedulerWnd::OnSchedulerEdit()
{
	CScheduleTask* pEditableItem;
	{
		CQuickLock oLock( Scheduler.m_pSection );

		CScheduleTask* pSchTask = GetItem( m_wndList.GetNextItem( -1, LVIS_SELECTED ) );
		if ( ! pSchTask ) return;
		pEditableItem = new CScheduleTask( *pSchTask );
	}

	CScheduleTaskDlg dlg( NULL, pEditableItem );
	if ( dlg.DoModal() == IDOK )
	{
		Scheduler.Save();
		Update();
	}
	else
		delete pEditableItem;
}

void CSchedulerWnd::OnUpdateSchedulerRemove(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CSchedulerWnd::OnSchedulerRemove()
{
	CQuickLock oLock( Scheduler.m_pSection);

	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
	{
		if ( CScheduleTask* pSchTask = GetItem( nItem ) )
			Scheduler.Remove( pSchTask );
	}

	Scheduler.Save();
	Update();
}

void CSchedulerWnd::OnSchedulerAdd()
{
	CScheduleTaskDlg dlg;

	if ( dlg.DoModal() == IDOK )
	{
		Scheduler.Save();
		Update();
	}
}

void CSchedulerWnd::OnSkinChange()
{
	OnSize( 0, 0, 0 );
	CPanelWnd::OnSkinChange();

	Settings.LoadList( _T("CSchedulerWnd"), &m_wndList, -3 );
	Skin.CreateToolBar( _T("CSchedulerWnd"), &m_wndToolBar );

	m_gdiImageList.Create( 16, 16, ILC_MASK|ILC_COLOR32, 3, 1 ) ||
		m_gdiImageList.Create( 16, 16, ILC_MASK|ILC_COLOR24, 3, 1 ) ||
		m_gdiImageList.Create( 16, 16, ILC_MASK|ILC_COLOR16, 3, 1 );
	m_gdiImageList.Add( CoolInterface.ExtractIcon( IDR_SCHEDULERFRAME, FALSE ) );
	m_gdiImageList.Add( CoolInterface.ExtractIcon( IDI_NOTASK, FALSE ) );
	m_gdiImageList.Add( CoolInterface.ExtractIcon( ID_SCHEDULER_ACTIVATE, FALSE ) );
	m_gdiImageList.Add( CoolInterface.ExtractIcon( ID_SCHEDULER_DEACTIVATE, FALSE ) );

	//CoolInterface.LoadIconsTo( m_gdiImageList, nImageID );
	m_wndList.SetImageList( &m_gdiImageList, LVSIL_SMALL );

	if ( m_wndList.SetBkImage( Skin.GetWatermark( _T("CSchedulerWnd") ) ) )
		m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_LABELTIP|LVS_EX_SUBITEMIMAGES );	// No LVS_EX_DOUBLEBUFFER
	else
		m_wndList.SetBkColor( Colors.m_crWindow );
}

BOOL CSchedulerWnd::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_DELETE )
		{
			OnSchedulerRemove();
			return TRUE;
		}
		else if ( pMsg->wParam == VK_INSERT )
		{
			PostMessage( WM_COMMAND, ID_SCHEDULER_ADD );
			return TRUE;
		}
	}

	return CPanelWnd::PreTranslateMessage( pMsg );
}

void CSchedulerWnd::OnUpdateSchedulerDeactivate(CCmdUI* pCmdUI)
{
	CQuickLock oLock( Scheduler.m_pSection );
	CScheduleTask* pSchTask = GetItem( m_wndList.GetNextItem( -1, LVIS_SELECTED ) );

	if ( ! pSchTask )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable( (m_wndList.GetSelectedCount() > 0 ) && (pSchTask->m_bActive));
}

void CSchedulerWnd::OnSchedulerDeactivate()
{
	CQuickLock oLock( Scheduler.m_pSection );

	CScheduleTask* pSchTask = GetItem( m_wndList.GetNextItem( -1, LVIS_SELECTED ) );

	if ( ! pSchTask ) return;

	pSchTask->m_bActive = false;

	// PUT HERE (MoJo)

	Update();
}

void CSchedulerWnd::OnUpdateSchedulerActivate(CCmdUI* pCmdUI)
{
	CQuickLock oLock( Scheduler.m_pSection );

	CScheduleTask* pSchTask = GetItem( m_wndList.GetNextItem( -1, LVIS_SELECTED ) );

	if ( ! pSchTask )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable( (m_wndList.GetSelectedCount() > 0 ) && (!pSchTask->m_bActive));
}

void CSchedulerWnd::OnSchedulerActivate()
{
	CQuickLock oLock( Scheduler.m_pSection );

	CScheduleTask* pSchTask = GetItem( m_wndList.GetNextItem( -1, LVIS_SELECTED ) );
	if ( ! pSchTask ) return;

	if (! Scheduler.IsScheduledTimePassed( pSchTask ) || pSchTask->m_bSpecificDays)
	{
		pSchTask->m_bActive = true;
		pSchTask->m_bExecuted = false;
	}
	else
	{
		OnSchedulerEdit();
	}

	Update();
}

void CSchedulerWnd::OnUpdateSchedulerRemoveAll(CCmdUI* pCmdUI)
{
	CScheduleTask *pSchTask = (CScheduleTask *)m_wndList.GetItemData (0);
	pCmdUI->Enable( pSchTask != NULL );
}

void CSchedulerWnd::OnSchedulerRemoveAll()
{
	CString strMessage;
	LoadString( strMessage, IDS_SCHEDULER_REMOVEALL_CONFIRM );
	if ( AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) != IDYES ) return;

	CQuickLock oLock( Scheduler.m_pSection);

	for ( int nItem = 0 ;  nItem < m_wndList.GetItemCount() ; nItem++)
	{
		if ( CScheduleTask* pSchTask = GetItem( nItem ) )
			Scheduler.Remove( pSchTask );
	}

	Scheduler.Save();
	Update();
}

void CSchedulerWnd::OnUpdateSchedulerExport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CSchedulerWnd::OnSchedulerExport()
{
	CFileDialog dlg( FALSE, _T("xml"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("XML Scheduler Files|*.xml|") );

	if ( dlg.DoModal() != IDOK ) return;

	CString strText;
	CFile pFile;

	if ( ! pFile.Open( dlg.GetPathName(), CFile::modeWrite|CFile::modeCreate ) )
	{
		AfxMessageBox( _T("Error: Can not export Scheduler list to file."), MB_ICONSTOP|MB_OK );	// ToDo: Translate?
		return;
	}

	CWaitCursor pCursor;

	CXMLElement* pXML = new CXMLElement( NULL, _T("scheduler") );

	pXML->AddAttribute( _T("xmlns"), CScheduler::xmlns );

	for ( int nItem = -1 ; ( nItem = m_wndList.GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
	{
		CQuickLock oLock( Scheduler.m_pSection );

		if ( CScheduleTask* pTask = GetItem( nItem ) )
			pXML->AddElement( pTask->ToXML() );
	}

	strText = pXML->ToString( TRUE, TRUE );

	int nBytes = WideCharToMultiByte( CP_ACP, 0, strText, strText.GetLength(), NULL, 0, NULL, NULL );
	LPSTR pBytes = new CHAR[nBytes];
	WideCharToMultiByte( CP_ACP, 0, strText, strText.GetLength(), pBytes, nBytes, NULL, NULL );
	pFile.Write( pBytes, nBytes );
	delete [] pBytes;

	delete pXML;

	pFile.Close();
}

void CSchedulerWnd::OnSchedulerImport()
{
	CFileDialog dlg( TRUE, _T("xml"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("XML Scheduler Files|*.xml|All Files|*.*||") );

	if ( dlg.DoModal() != IDOK ) return;

	CWaitCursor pCursor;

	if ( Scheduler.Import( dlg.GetPathName() ) )
		Scheduler.Save();
	else
		AfxMessageBox( _T("Error: Can not import Scheduler list from file."), MB_ICONSTOP|MB_OK );	// ToDo: Translate?
}
