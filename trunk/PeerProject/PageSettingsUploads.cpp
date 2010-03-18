//
// PageSettingsUploads.cpp
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
#include "Transfers.h"
#include "UploadQueue.h"
#include "UploadQueues.h"
#include "PageSettingsUploads.h"
#include "DlgQueueProperties.h"
#include "DlgHelp.h"
#include "LiveList.h"
#include "CoolInterface.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CUploadsSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CUploadsSettingsPage, CSettingsPage)
	ON_CBN_SELCHANGE(IDC_AGENT_LIST, OnSelChangeAgentList)
	ON_CBN_EDITCHANGE(IDC_AGENT_LIST, OnEditChangeAgentList)
	ON_BN_CLICKED(IDC_AGENT_ADD, OnAgentAdd)
	ON_BN_CLICKED(IDC_AGENT_REMOVE, OnAgentRemove)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_QUEUES, OnItemChangedQueues)
	ON_BN_CLICKED(IDC_QUEUE_NEW, OnQueueNew)
	ON_BN_CLICKED(IDC_QUEUE_EDIT, OnQueueEdit)
	ON_BN_CLICKED(IDC_QUEUE_DELETE, OnQueueDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_QUEUES, OnDblClkQueues)
	ON_NOTIFY(LVN_DRAGDROP, IDC_QUEUES, OnQueueDrop)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUploadsSettingsPage property page

CUploadsSettingsPage::CUploadsSettingsPage()
	: CSettingsPage 	( CUploadsSettingsPage::IDD )
	, m_bHubUnshare		( FALSE )
	, m_bSharePartials	( FALSE )
	, m_bSharePreviews	( FALSE )
	, m_bThrottleMode	( FALSE )
	, m_bFairUseMode	( FALSE )
	, m_nMaxPerHost		( 0ul )
{
}

CUploadsSettingsPage::~CUploadsSettingsPage()
{
}

void CUploadsSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SHARE_PARTIALS, m_bSharePartials);
	DDX_Check(pDX, IDC_HUB_UNSHARE, m_bHubUnshare);
	DDX_Check(pDX, IDC_SHARE_PREVIEW, m_bSharePreviews);
	DDX_Text(pDX, IDC_MAX_HOST, m_nMaxPerHost);
	DDX_Control(pDX, IDC_MAX_HOST_SPIN, m_wndMaxPerHost);
	DDX_Control(pDX, IDC_AGENT_LIST, m_wndAgentList);
	DDX_Control(pDX, IDC_AGENT_ADD, m_wndAgentAdd);
	DDX_Control(pDX, IDC_AGENT_REMOVE, m_wndAgentRemove);

	DDX_Control(pDX, IDC_UPLOADS_BANDWIDTH_LIMIT, m_wndBandwidthLimit);
	DDX_CBString(pDX, IDC_UPLOADS_BANDWIDTH_LIMIT, m_sBandwidthLimit);
	DDX_CBIndex(pDX, IDC_THROTTLE_MODE, m_bThrottleMode);

	DDX_Control(pDX, IDC_QUEUES, m_wndQueues);
	DDX_Control(pDX, IDC_QUEUE_EDIT, m_wndQueueEdit);
	DDX_Control(pDX, IDC_QUEUE_DELETE, m_wndQueueDelete);
}

/////////////////////////////////////////////////////////////////////////////
// CUploadsSettingsPage message handlers

BOOL CUploadsSettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	CRect rcList;
	m_wndQueues.GetClientRect( &rcList );
	rcList.right -= GetSystemMetrics( SM_CXVSCROLL );

	CoolInterface.SetImageListTo( m_wndQueues, LVSIL_SMALL );
	m_wndQueues.SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP );
	m_wndQueues.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, rcList.right - 104 - 74 - 60, -1 );
	m_wndQueues.InsertColumn( 1, _T("Criteria"), LVCFMT_LEFT, 104, 0 );
	m_wndQueues.InsertColumn( 2, _T("Minimum"), LVCFMT_CENTER, 74, 1 );
	m_wndQueues.InsertColumn( 3, _T("Slots"), LVCFMT_CENTER, 60, 2 );
	m_wndQueues.InsertColumn( 4, _T("Priority"), LVCFMT_CENTER, 0, 3 );
	Skin.Translate( _T("CUploadQueueList"), m_wndQueues.GetHeaderCtrl() );

	m_wndQueues.EnableToolTips();

	CLiveList::Sort( &m_wndQueues, 4, FALSE );
	//CLiveList::Sort( &m_wndQueues, 4, FALSE );	//Repeat

	m_nMaxPerHost		= Settings.Uploads.MaxPerHost;
	m_bHubUnshare		= Settings.Uploads.HubUnshare;
	m_bSharePartials	= Settings.Uploads.SharePartials;
	m_bSharePreviews	= Settings.Uploads.SharePreviews;
	m_bThrottleMode		= Settings.Uploads.ThrottleMode;
	m_bFairUseMode		= Settings.Uploads.FairUseMode;

	Settings.SetRange( &Settings.Uploads.MaxPerHost, m_wndMaxPerHost );

	for ( string_set::const_iterator i = Settings.Uploads.BlockAgents.begin() ;
		i != Settings.Uploads.BlockAgents.end(); i++ )
	{
		m_wndAgentList.AddString( *i );
	}

	UpdateData( FALSE );
	UpdateQueues();

	m_wndAgentAdd.EnableWindow( m_wndAgentList.GetWindowTextLength() > 0 );
	m_wndAgentRemove.EnableWindow( m_wndAgentList.GetCurSel() >= 0 );
	m_wndQueueEdit.EnableWindow( m_wndQueues.GetSelectedCount() == 1 );
	m_wndQueueDelete.EnableWindow( m_wndQueues.GetSelectedCount() > 0 );

	m_bQueuesChanged = FALSE;

	// Update value in limit combo box
	if ( Settings.Bandwidth.Uploads )
		m_sBandwidthLimit = Settings.SmartSpeed( Settings.Bandwidth.Uploads );
	else
		m_sBandwidthLimit	= _T("MAX");

	UpdateData( FALSE );

	return TRUE;
}

BOOL CUploadsSettingsPage::OnSetActive()
{
	UpdateQueues();
	return CSettingsPage::OnSetActive();
}

void CUploadsSettingsPage::UpdateQueues()
{
	UpdateData( TRUE );

	QWORD nTotal = Settings.Connection.OutSpeed * Kilobits / Bytes;
	QWORD nLimit = Settings.ParseVolume( m_sBandwidthLimit );

	if ( nLimit == 0 || nLimit > nTotal ) nLimit = nTotal;

	CSingleLock pLock( &UploadQueues.m_pSection, TRUE );
	CLiveList pQueues( 5 );
	int nIndex = 1;

	for ( POSITION pos = UploadQueues.GetIterator() ; pos ; nIndex++ )
	{
		BOOL bDonkeyOnlyDisabled = FALSE;

		CUploadQueue* pQueue = UploadQueues.GetNext( pos );

		// If queue is ed2k only and we need to be connected to upload, Then queue is inactive ed2k isn't enabled
		if ( ( ( pQueue->m_nProtocols & ( 1 << PROTOCOL_ED2K ) ) != 0 ) && ( Settings.Connection.RequireForTransfers ) )
			bDonkeyOnlyDisabled = !( Settings.eDonkey.EnableAlways | Settings.eDonkey.EnableToday );


		// If the queue is inactive and we're in basic GUI mode
		if ( ( bDonkeyOnlyDisabled ) && (Settings.General.GUIMode == GUI_BASIC) )
			continue;	// Skip drawing this queue

		CLiveItem* pItem = pQueues.Add( pQueue );

		if ( ( pQueue->m_bEnable ) && ( ! bDonkeyOnlyDisabled ) )
		{
			QWORD nBandwidth = nLimit * pQueue->m_nBandwidthPoints / max( 1, UploadQueues.GetTotalBandwidthPoints( TRUE ) );
			pItem->Set( 2, Settings.SmartSpeed( nBandwidth ) + '+' );
			pItem->Format( 3, _T("%i-%i /%i"), pQueue->m_nMinTransfers, pQueue->m_nMaxTransfers, pQueue->m_nCapacity );

			pItem->m_nImage = CoolInterface.ImageForID( ID_VIEW_UPLOADS );
		}
		else
		{
			pItem->Set( 2, _T("- ") );
			pItem->Format( 3, _T("-"));

			pItem->m_nImage = CoolInterface.ImageForID( ID_SYSTEM_CLEAR );
		}

		pItem->Set( 0, pQueue->m_sName );
		pItem->Set( 1, pQueue->GetCriteriaString() );

		pItem->Format( 4, _T("%i"), nIndex );

	}

	pLock.Unlock();
	pQueues.Apply( &m_wndQueues, TRUE );
}

void CUploadsSettingsPage::OnSelChangeAgentList()
{
	m_wndAgentRemove.EnableWindow( m_wndAgentList.GetCurSel() >= 0 );
}

void CUploadsSettingsPage::OnEditChangeAgentList()
{
	m_wndAgentAdd.EnableWindow( m_wndAgentList.GetWindowTextLength() > 0 );
}

void CUploadsSettingsPage::OnAgentAdd()
{
	CString strType;
	m_wndAgentList.GetWindowText( strType );

	ToLower( strType );

	strType.Trim();
	if ( strType.IsEmpty() ) return;

	if ( m_wndAgentList.FindString( -1, strType ) >= 0 ) return;

	m_wndAgentList.AddString( strType );
	m_wndAgentList.SetWindowText( _T("") );
}

void CUploadsSettingsPage::OnAgentRemove()
{
	int nItem = m_wndAgentList.GetCurSel();
	if ( nItem >= 0 ) m_wndAgentList.DeleteString( nItem );
	m_wndAgentRemove.EnableWindow( FALSE );
}

void CUploadsSettingsPage::OnItemChangedQueues(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_wndQueueEdit.EnableWindow( m_wndQueues.GetSelectedCount() == 1 );
	m_wndQueueDelete.EnableWindow( m_wndQueues.GetSelectedCount() > 0 );
	*pResult = 0;
}

void CUploadsSettingsPage::OnDblClkQueues(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	PostMessage( WM_COMMAND, MAKELONG( IDC_QUEUE_EDIT, BN_CLICKED ) );
	*pResult = 0;
}

void CUploadsSettingsPage::OnQueueNew()
{
	CString strQueueName;
	LoadString( strQueueName, IDS_UPLOAD_QUEUE_NEW );
	CUploadQueue* pQueue = UploadQueues.Create( strQueueName, TRUE );
	UpdateQueues();

	CQueuePropertiesDlg dlg( pQueue, TRUE, this );
	if ( dlg.DoModal() != IDOK ) UploadQueues.Delete( pQueue );

	UploadQueues.Save();
	UpdateQueues();
	m_bQueuesChanged = TRUE;
}

void CUploadsSettingsPage::OnQueueEdit()
{
	int nSelected = m_wndQueues.GetNextItem( -1, LVNI_SELECTED );
	if ( nSelected < 0 ) return;

	CUploadQueue* pQueue = (CUploadQueue*)m_wndQueues.GetItemData( nSelected );

	CQueuePropertiesDlg dlg( pQueue, FALSE, this );
	dlg.DoModal();

	UploadQueues.Save();
	UpdateQueues();
	m_bQueuesChanged = TRUE;
}

void CUploadsSettingsPage::OnQueueDelete()
{
	for ( int nItem = -1 ; ( nItem = m_wndQueues.GetNextItem( nItem, LVNI_SELECTED ) ) >= 0 ; )
	{
		CUploadQueue* pQueue = (CUploadQueue*)m_wndQueues.GetItemData( nItem );
		UploadQueues.Delete( pQueue );
	}

	UploadQueues.Save();
	UpdateQueues();
	m_bQueuesChanged = TRUE;
}

void CUploadsSettingsPage::OnQueueDrop(NMHDR* pNMHDR, LRESULT* pResult)
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	NM_LISTVIEW* pNM = (NM_LISTVIEW*)pNMHDR;

	CUploadQueue* pTarget = NULL;

	if ( pNM->iItem >= 0 && pNM->iItem < m_wndQueues.GetItemCount() )
	{
		pTarget = (CUploadQueue*)m_wndQueues.GetItemData( pNM->iItem );
		if ( ! UploadQueues.Check( pTarget ) ) pTarget = NULL;
	}

	for ( int nItem = -1 ; ( nItem = m_wndQueues.GetNextItem( nItem, LVNI_SELECTED ) ) >= 0 ; )
	{
		CUploadQueue* pQueue = (CUploadQueue*)m_wndQueues.GetItemData( nItem );

		if ( UploadQueues.Check( pQueue ) && pQueue != pTarget )
			UploadQueues.Reorder( pQueue, pTarget );
	}

	UploadQueues.Save();
	UpdateQueues();
	*pResult = 0;
}

BOOL CUploadsSettingsPage::OnKillActive()
{
	UpdateData();

	if ( IsLimited( m_sBandwidthLimit ) && ! Settings.ParseVolume( m_sBandwidthLimit ) )
	{
		CString strMessage;
		LoadString( strMessage, IDS_SETTINGS_NEED_BANDWIDTH );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		GetDlgItem( IDC_UPLOADS_BANDWIDTH_LIMIT )->SetFocus();
		return FALSE;
	}

	return CSettingsPage::OnKillActive();
}

void CUploadsSettingsPage::OnOK()
{
	UpdateData();

	DWORD nOldLimit = Settings.Bandwidth.Uploads;

	Settings.Uploads.MaxPerHost			= m_nMaxPerHost;
	Settings.Uploads.HubUnshare			= m_bHubUnshare != FALSE;
	Settings.Uploads.SharePartials		= m_bSharePartials != FALSE;
	Settings.Uploads.SharePreviews		= m_bSharePreviews != FALSE;
	Settings.Uploads.ThrottleMode		= m_bThrottleMode != FALSE;
	Settings.Uploads.FairUseMode		= m_bFairUseMode != FALSE;
	Settings.Bandwidth.Uploads			= static_cast< DWORD >( Settings.ParseVolume( m_sBandwidthLimit ) );


	// Warn the user about the effects of upload limiting
	if ( ! Settings.Live.UploadLimitWarning && Settings.Bandwidth.Uploads > 0 && Settings.Bandwidth.Uploads != nOldLimit )
	{
		QWORD nDownload = max( Settings.Bandwidth.Downloads, Settings.Connection.InSpeed  * Kilobits / Bytes );
		QWORD nUpload	= min( Settings.Bandwidth.Uploads,   Settings.Connection.OutSpeed * Kilobits / Bytes );

		if ( nUpload * 16 < nDownload )
		{
			CHelpDlg::Show( _T("GeneralHelp.UploadWarning") );
			Settings.Live.UploadLimitWarning = TRUE;
		}
	}

	// Set blocked user agents/strings
	Settings.Uploads.BlockAgents.clear();

	for ( int nItem = 0 ; nItem < m_wndAgentList.GetCount() ; nItem++ )
	{
		CString str;
		m_wndAgentList.GetLBText( nItem, str );
		if ( str.GetLength() )
			Settings.Uploads.BlockAgents.insert( str );
	}

	// Create/Validate queues
	if ( UploadQueues.GetCount() == 0 )
	{
		UploadQueues.CreateDefault();
		m_bQueuesChanged = TRUE;
	}

	UploadQueues.Validate();

	UpdateQueues();
}


void CUploadsSettingsPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CSettingsPage::OnShowWindow(bShow, nStatus);
	if ( bShow )
	{
		// Update the bandwidth limit combo values

		// Update speed units
		if ( Settings.Bandwidth.Uploads )
			m_sBandwidthLimit	= Settings.SmartSpeed( Settings.Bandwidth.Uploads );
		else
			m_sBandwidthLimit	= _T("MAX");

		// Remove any existing strings
		m_wndBandwidthLimit.ResetContent();

		// Add the new ones
		const DWORD nSpeeds[] =
		{
			Settings.Connection.OutSpeed / 4,			//  25%
			Settings.Connection.OutSpeed / 2,			//  50%
			( Settings.Connection.OutSpeed * 3 ) / 4,	//  75%
			( Settings.Connection.OutSpeed * 9 ) / 10,	//  90%
			Settings.Connection.OutSpeed				// 100%
		};
		for ( int nSpeed = 0 ; nSpeed < sizeof( nSpeeds ) / sizeof( DWORD ) ; nSpeed++ )
		{
			CString strSpeed = Settings.SmartSpeed( nSpeeds[ nSpeed ], Kilobits );
			if ( Settings.ParseVolume( strSpeed, Kilobits )
				&& m_wndBandwidthLimit.FindStringExact( -1, strSpeed ) == CB_ERR )
			{
				m_wndBandwidthLimit.AddString( strSpeed );
			}
		}
		m_wndBandwidthLimit.AddString( _T("MAX") );

		UpdateData( FALSE );

		// Update queue window to show current limit
		UpdateQueues();
	}
}

bool CUploadsSettingsPage::IsLimited(CString& strText) const
{
	if ( ( _tcslen( strText ) == 0 ) ||
		 ( _tcsistr( strText, _T("MAX") ) != NULL ) ||
		 ( _tcsistr( strText, _T("NONE") ) != NULL ) )
		return false;
	else
		return true;
}
