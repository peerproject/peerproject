//
// CtrlUploads.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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
#include "CtrlUploads.h"
#include "UploadQueues.h"
#include "UploadQueue.h"
#include "UploadFiles.h"
#include "UploadFile.h"
#include "UploadTransfer.h"
#include "UploadTransferBT.h"
#include "Transfers.h"
#include "FragmentBar.h"
#include "ShellIcons.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Images.h"
#include "Flags.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CUploadsCtrl, CWnd)

BEGIN_MESSAGE_MAP(CUploadsCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_NOTIFY(HDN_ITEMCHANGEDW, AFX_IDW_PANE_FIRST, OnChangeHeader)
	ON_NOTIFY(HDN_ITEMCHANGEDA, AFX_IDW_PANE_FIRST, OnChangeHeader)
	ON_NOTIFY(HDN_ENDDRAG, AFX_IDW_PANE_FIRST, OnChangeHeader)
END_MESSAGE_MAP()

#define HEADER_HEIGHT			20
//#define ITEM_HEIGHT			17	// Settings.Skin.RowSize

#define UPLOAD_COLUMN_TITLE		0
#define UPLOAD_COLUMN_SIZE		1
#define UPLOAD_COLUMN_PROGRESS	2
#define UPLOAD_COLUMN_TRANSFER	3
#define UPLOAD_COLUMN_SPEED		4
#define UPLOAD_COLUMN_RATING	5
#define UPLOAD_COLUMN_USER		6
#define UPLOAD_COLUMN_CLIENT	7
#define UPLOAD_COLUMN_COUNTRY	8


//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl construction

CUploadsCtrl::CUploadsCtrl()
	: m_nFocus		( 0 )
	, m_pDeselect	( NULL )
{
}

//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl operations

BOOL CUploadsCtrl::Create(CWnd* pParentWnd, UINT nID)
{
	CRect rc( 0, 0, 0, 0 );
	return CWnd::CreateEx( WS_EX_CONTROLPARENT, NULL, _T("CUploadsCtrl"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | WS_GROUP, rc, pParentWnd, nID );
}

BOOL CUploadsCtrl::Update()
{
	OnSize( 1982, 0, 0 );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl system message handlers

int CUploadsCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	CRect rect( 0, 0, 0, 0 );
	m_wndHeader.Create( WS_CHILD|HDS_DRAGDROP|HDS_HOTTRACK|HDS_FULLDRAG, rect, this, AFX_IDW_PANE_FIRST );
	m_wndHeader.SetFont( &theApp.m_gdiFont );

	m_wndTip.Create( this, &Settings.Interface.TipUploads );

	InsertColumn( UPLOAD_COLUMN_TITLE, _T("Uploaded File"), LVCFMT_LEFT, 300 );
	InsertColumn( UPLOAD_COLUMN_SIZE, _T("Size"), LVCFMT_CENTER, 64 );
	InsertColumn( UPLOAD_COLUMN_PROGRESS, _T("Progress"), LVCFMT_CENTER, 100 );
	InsertColumn( UPLOAD_COLUMN_TRANSFER, _T("Transfer"), LVCFMT_CENTER, 64 );
	InsertColumn( UPLOAD_COLUMN_SPEED, _T("Speed"), LVCFMT_CENTER, 76 );
	InsertColumn( UPLOAD_COLUMN_RATING, _T("Rating"), LVCFMT_CENTER, 0 );
	InsertColumn( UPLOAD_COLUMN_USER, _T("Remote User"), LVCFMT_CENTER, 140 );
	InsertColumn( UPLOAD_COLUMN_CLIENT, _T("Client"), LVCFMT_CENTER, 100 );
	InsertColumn( UPLOAD_COLUMN_COUNTRY, _T("Country"), LVCFMT_LEFT, 54 );

	LoadColumnState();

//	CoolInterface.LoadIconsTo( m_gdiProtocols, protocolIDs );

	m_nFocus	= 0;
	m_pDeselect	= NULL;

	return 0;
}

void CUploadsCtrl::OnDestroy()
{
	SaveColumnState();
	CWnd::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl column helpers

void CUploadsCtrl::InsertColumn(int nColumn, LPCTSTR pszCaption, int nFormat, int nWidth)
{
	HDITEM pColumn = {};

	pColumn.mask	= HDI_FORMAT | HDI_LPARAM | HDI_TEXT | HDI_WIDTH;
	pColumn.cxy		= nWidth;
	pColumn.pszText	= (LPTSTR)pszCaption;
	pColumn.fmt		= nFormat;
	pColumn.lParam	= nColumn;

	m_wndHeader.InsertItem( m_wndHeader.GetItemCount(), &pColumn );
}

void CUploadsCtrl::SaveColumnState()
{
	HDITEM pItem = { HDI_WIDTH|HDI_ORDER };

	CString strOrdering, strWidths, strItem;

	for ( int nColumns = 0 ; m_wndHeader.GetItem( nColumns, &pItem ) ; nColumns++ )
	{
		m_wndHeader.GetItem( nColumns, &pItem );

		strItem.Format( _T("%.2x"), pItem.iOrder );
		strOrdering += strItem;

		strItem.Format( _T("%.4x"), pItem.cxy );
		strWidths += strItem;
	}

	theApp.WriteProfileString( _T("ListStates"), _T("CUploadCtrl.Ordering"), strOrdering );
	theApp.WriteProfileString( _T("ListStates"), _T("CUploadCtrl.Widths"), strWidths );
}

BOOL CUploadsCtrl::LoadColumnState()
{
	CString strOrdering, strWidths, strItem;

	strOrdering = theApp.GetProfileString( _T("ListStates"), _T("CUploadCtrl.Ordering"), _T("") );
	strWidths = theApp.GetProfileString( _T("ListStates"), _T("CUploadCtrl.Widths"), _T("") );

	HDITEM pItem = { HDI_WIDTH|HDI_ORDER };

	if ( _tcsncmp( strWidths, _T("0000"), 4 ) == 0 &&
		 _tcsncmp( strOrdering, _T("00"), 2 ) == 0 )
	{
		strWidths = strWidths.Mid( 4 );
		strOrdering = strOrdering.Mid( 2 );
	}

	for ( int nColumns = 0 ; m_wndHeader.GetItem( nColumns, &pItem ) ; nColumns++ )
	{
		if ( strWidths.GetLength() < 4 || strOrdering.GetLength() < 2 )
			return FALSE;

		if ( _stscanf( strWidths.Left( 4 ), _T("%x"), &pItem.cxy ) != 1 ||
			 _stscanf( strOrdering.Left( 2 ), _T("%x"), &pItem.iOrder ) != 1 )
			return FALSE;

		strWidths = strWidths.Mid( 4 );
		strOrdering = strOrdering.Mid( 2 );

		m_wndHeader.SetItem( nColumns, &pItem );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl item helpers

void CUploadsCtrl::SelectTo(int nIndex)
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	BOOL bShift		= GetAsyncKeyState( VK_SHIFT ) & 0x8000;
	BOOL bControl	= GetAsyncKeyState( VK_CONTROL ) & 0x8000;
	BOOL bRight		= GetAsyncKeyState( VK_RBUTTON ) & 0x8000;

	if ( ! bShift && ! bControl && ! bRight && m_pDeselect == NULL ) DeselectAll();

	Update();

	INT nMin, nMax;
	GetScrollRange( SB_VERT, &nMin, &nMax );
	nIndex = max( 0, min( nIndex, nMax - 1 ) );

	CUploadFile* pFile;
	CUploadQueue* pQueue;

	if ( bShift )
	{
		if ( m_nFocus < nIndex )
		{
			for ( m_nFocus ++ ; m_nFocus <= nIndex ; m_nFocus ++ )
			{
				GetAt( m_nFocus, &pQueue, &pFile );
				if ( pQueue ) pQueue->m_bSelected = TRUE;
				if ( pFile )  pFile->m_bSelected = TRUE;
			}
		}
		else if ( m_nFocus > nIndex )
		{
			for ( m_nFocus -- ; m_nFocus >= nIndex ; m_nFocus -- )
			{
				GetAt( m_nFocus, &pQueue, &pFile );
				if ( pQueue != NULL ) pQueue->m_bSelected = TRUE;
				if ( pFile != NULL ) pFile->m_bSelected = TRUE;
			}
		}

		m_nFocus = nIndex;
	}
	else
	{
		m_nFocus = nIndex;
		GetAt( m_nFocus, &pQueue, &pFile );

		if ( bControl )
		{
			if ( pQueue != NULL ) pQueue->m_bSelected = ! pQueue->m_bSelected;
			if ( pFile != NULL ) pFile->m_bSelected = ! pFile->m_bSelected;
		}
		else
		{
			if ( pQueue != NULL ) pQueue->m_bSelected = TRUE;
			if ( pFile != NULL ) pFile->m_bSelected = TRUE;
		}
	}

	pLock.Unlock();

	CRect rcClient;
	GetClientRect( &rcClient );

	int nScroll = GetScrollPos( SB_VERT );
	int nHeight = ( rcClient.bottom - HEADER_HEIGHT ) / Settings.Skin.RowSize - 1;
	nHeight = max( 0, nHeight );

	if ( m_nFocus < nScroll )
	{
		SetScrollPos( SB_VERT, m_nFocus );
		Update();
	}
	else if ( m_nFocus > nScroll + nHeight )
	{
		SetScrollPos( SB_VERT, max( 0, m_nFocus - nHeight ) );
		Update();
	}
	else
	{
		Invalidate();
	}
}

void CUploadsCtrl::DeselectAll(CUploadFile* /*pExcept*/)
{
	CSingleLock pLock( &UploadQueues.m_pSection, TRUE );

	UploadQueues.m_pTorrentQueue->m_bSelected = FALSE;
	UploadQueues.m_pHistoryQueue->m_bSelected = FALSE;

	for ( POSITION pos = UploadQueues.GetIterator() ; pos ; )
	{
		CUploadQueue* pQueue = UploadQueues.GetNext( pos );
		pQueue->m_bSelected = FALSE;
	}

	for ( POSITION pos = UploadFiles.GetIterator() ; pos ; )
	{
		CUploadFile* pFile = UploadFiles.GetNext( pos );
		pFile->m_bSelected = FALSE;
	}

	pLock.Unlock();

	Invalidate();
}

BOOL CUploadsCtrl::HitTest(const CPoint& point, CUploadQueue** ppQueue, CUploadFile** ppFile, int* pnIndex, RECT* prcItem)
{
	ASSUME_LOCK( Transfers.m_pSection );

	CRect rcClient, rcItem;

	GetClientRect( &rcClient );
	rcClient.top += HEADER_HEIGHT;

	rcItem.CopyRect( &rcClient );
	rcItem.left -= GetScrollPos( SB_HORZ );
	rcItem.bottom = rcItem.top + Settings.Skin.RowSize;

	int nScroll = GetScrollPos( SB_VERT );
	int nIndex = 0;

	if ( ppQueue != NULL ) *ppQueue = NULL;
	if ( ppFile != NULL ) *ppFile = NULL;

	CSingleLock pLock( &UploadQueues.m_pSection, FALSE );
	if ( ! pLock.Lock( 500 ) )
		return FALSE;

	for ( POSITION posQueue = GetQueueIterator() ; posQueue && rcItem.top < rcClient.bottom ; )
	{
		CUploadQueue* pQueue = GetNextQueue( posQueue );

		POSITION posFile = GetFileIterator( pQueue );
		if ( posFile == NULL ) continue;

		if ( nScroll > 0 )
		{
			nScroll --;
		}
		else
		{
			if ( rcItem.PtInRect( point ) )
			{
				if ( ppQueue != NULL ) *ppQueue = pQueue;
				if ( pnIndex != NULL ) *pnIndex = nIndex;
				if ( prcItem != NULL ) *prcItem = rcItem;
				return TRUE;
			}
			rcItem.OffsetRect( 0, Settings.Skin.RowSize );
		}

		nIndex ++;
		if ( ! pQueue->m_bExpanded ) continue;

		while ( posFile && rcItem.top < rcClient.bottom )
		{
			CUploadFile* pFile = GetNextFile( pQueue, posFile );
			if ( pFile == NULL ) continue;

			if ( nScroll > 0 )
			{
				nScroll --;
			}
			else
			{
				if ( rcItem.PtInRect( point ) )
				{
					if ( ppFile != NULL ) *ppFile = pFile;
					if ( pnIndex != NULL ) *pnIndex = nIndex;
					if ( prcItem != NULL ) *prcItem = rcItem;
					return TRUE;
				}
				rcItem.OffsetRect( 0, Settings.Skin.RowSize );
			}

			nIndex ++;
		}
	}

	return FALSE;
}

BOOL CUploadsCtrl::GetAt(int nSelect, CUploadQueue** ppQueue, CUploadFile** ppFile)
{
	ASSUME_LOCK( Transfers.m_pSection );

	/*int nScroll =*/ GetScrollPos( SB_VERT );
	int nIndex = 0;

	if ( ppQueue != NULL ) *ppQueue = NULL;
	if ( ppFile != NULL ) *ppFile = NULL;

	CSingleLock pLock( &UploadQueues.m_pSection, FALSE );
	if ( ! pLock.Lock( 500 ) )
		return FALSE;

	for ( POSITION posQueue = GetQueueIterator() ; posQueue ; )
	{
		CUploadQueue* pQueue = GetNextQueue( posQueue );

		POSITION posFile = GetFileIterator( pQueue );
		if ( posFile == NULL ) continue;

		if ( nIndex++ == nSelect )
		{
			if ( ppQueue != NULL ) *ppQueue = pQueue;
			return TRUE;
		}

		if ( ! pQueue->m_bExpanded ) continue;

		while ( posFile )
		{
			CUploadFile* pFile = GetNextFile( pQueue, posFile );
			if ( pFile == NULL ) continue;

			if ( nIndex++ == nSelect )
			{
				if ( ppFile != NULL )
					*ppFile = pFile;
				return TRUE;
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl queue / file abstractation layer

POSITION CUploadsCtrl::GetQueueIterator()
{
	ASSUME_LOCK( UploadQueues.m_pSection );

	if ( Settings.Uploads.FilterMask & ULF_TORRENT )
		return (POSITION)UploadQueues.m_pTorrentQueue;
	if ( Settings.Uploads.FilterMask & ( ULF_ACTIVE | ULF_QUEUED ) )
		return UploadQueues.GetIterator();
	if ( Settings.Uploads.FilterMask & ULF_HISTORY )
		return (POSITION)UploadQueues.m_pHistoryQueue;

	return NULL;
}

CUploadQueue* CUploadsCtrl::GetNextQueue(POSITION& pos)
{
	ASSUME_LOCK( UploadQueues.m_pSection );
	ASSERT( pos != NULL );

	if ( pos == (POSITION)UploadQueues.m_pTorrentQueue )
	{
		if ( Settings.Uploads.FilterMask & ( ULF_ACTIVE | ULF_QUEUED ) )
		{
			pos = UploadQueues.GetIterator();
			if ( pos == NULL )
				pos = (POSITION)UploadQueues.m_pHistoryQueue;
		}
		else if ( Settings.Uploads.FilterMask & ULF_HISTORY )
		{
			pos = (POSITION)UploadQueues.m_pHistoryQueue;
		}
		else
		{
			pos = NULL;
		}

		return UploadQueues.m_pTorrentQueue;
	}
	else if ( pos == (POSITION)UploadQueues.m_pHistoryQueue )
	{
		pos = NULL;
		return UploadQueues.m_pHistoryQueue;
	}
	else // Normal queue
	{
		CUploadQueue* pQueue = UploadQueues.GetNext( pos );

		if ( pos == NULL && ( Settings.Uploads.FilterMask & ULF_HISTORY ) )
			pos = (POSITION)UploadQueues.m_pHistoryQueue;

		return pQueue;
	}
}

POSITION CUploadsCtrl::GetFileIterator(CUploadQueue* pQueue)
{
	ASSUME_LOCK( UploadQueues.m_pSection );

	if ( pQueue == UploadQueues.m_pTorrentQueue )
	{
		for ( POSITION posNext = UploadFiles.GetIterator() ; posNext ; )
		{
			POSITION posThis = posNext;
			CUploadFile* pFile = UploadFiles.GetNext( posNext );
			CUploadTransfer* pTransfer = pFile->GetActive();
			if ( pTransfer == NULL || pTransfer->m_nState == upsNull ) continue;
			if ( pTransfer->m_nProtocol != PROTOCOL_BT ) continue;
			return posThis;
		}
	}
	else if ( pQueue == UploadQueues.m_pHistoryQueue )
	{
		for ( POSITION posNext = UploadFiles.GetIterator() ; posNext ; )
		{
			POSITION posThis = posNext;
			CUploadFile* pFile = UploadFiles.GetNext( posNext );
			CUploadTransfer* pTransfer = pFile->GetActive();
			if ( pTransfer != NULL )
			{
				if ( pTransfer->m_nProtocol == PROTOCOL_BT && pTransfer->m_nState != upsNull ) continue;
				if ( pTransfer->m_pQueue != NULL ) continue;
			}
			return posThis;
		}
	}
	else // Normal queue
	{
		if ( Settings.Uploads.FilterMask & ULF_ACTIVE )
		{
			if ( pQueue->GetActiveCount() > 0 )
				return pQueue->GetActiveIterator();
		}

		if ( Settings.Uploads.FilterMask & ULF_QUEUED )
		{
			if ( pQueue->GetQueuedCount() > 0 )
				return (POSITION)1;
		}
	}

	return NULL;
}

CUploadFile* CUploadsCtrl::GetNextFile(CUploadQueue* pQueue, POSITION& pos, int* pnPosition)
{
	ASSUME_LOCK( Transfers.m_pSection );
	ASSERT( pos != NULL );

	if ( pnPosition != NULL ) *pnPosition = -1;

	if ( pQueue == UploadQueues.m_pTorrentQueue )
	{
		CUploadFile* pReturn = UploadFiles.GetNext( pos );

		for ( ; pos ; )
		{
			POSITION posThis = pos;
			CUploadFile* pFile = UploadFiles.GetNext( pos );
			CUploadTransfer* pTransfer = pFile->GetActive();
			if ( pTransfer == NULL || pTransfer->m_nState == upsNull ) continue;
			if ( pTransfer->m_nProtocol != PROTOCOL_BT ) continue;
			pos = posThis;
			break;
		}

		return pReturn;
	}
	else if ( pQueue == UploadQueues.m_pHistoryQueue )
	{
		CUploadFile* pReturn = UploadFiles.GetNext( pos );

		for ( ; pos ; )
		{
			POSITION posThis = pos;
			CUploadFile* pFile = UploadFiles.GetNext( pos );
			CUploadTransfer* pTransfer = pFile->GetActive();
			if ( pTransfer != NULL )
			{
				//if ( pTransfer->m_nProtocol == PROTOCOL_BT && pTransfer->m_nState != upsNull ) continue;
				if ( pTransfer->m_nState != upsNull ) continue;
				if ( pTransfer->m_pQueue != NULL ) continue;
			}
			pos = posThis;
			break;
		}

		return pReturn;
	}
	else if ( (DWORD_PTR)pos > pQueue->GetQueuedCount() )
	{
		CUploadTransfer* pTransfer = pQueue->GetNextActive( pos );

		if ( pos == NULL )
		{
			if ( Settings.Uploads.FilterMask & ULF_QUEUED )
			{
				if ( pQueue->GetQueuedCount() > 0 )
					pos = (POSITION)1;
			}
		}

		if ( pnPosition != NULL ) *pnPosition = 0;
		return pTransfer->m_pBaseFile;
	}
	else
	{
		DWORD nPos = (DWORD_PTR)pos;
		CUploadTransfer* pTransfer = pQueue->GetQueuedAt( nPos - 1 );
		if ( pnPosition != NULL )
			*pnPosition = static_cast< int >( nPos );
		++nPos;
		if ( nPos > pQueue->GetQueuedCount() ) nPos = 0;
		pos = (POSITION)nPos;
		return pTransfer->m_pBaseFile;
	}
}

//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl presentation message handlers

void CUploadsCtrl::OnSize(UINT nType, int cx, int cy)
{
	int nWidth = 0, nHeight = 0;
	CRect rcClient;

	if ( nType != 1982 ) CWnd::OnSize( nType, cx, cy );

	GetClientRect( &rcClient );

	HDITEM pColumn = {};
	pColumn.mask = HDI_WIDTH;

	for ( int nColumn = 0 ; m_wndHeader.GetItem( nColumn, &pColumn ) ; nColumn ++ )
		nWidth += pColumn.cxy;

	SCROLLINFO pScroll = {};
	pScroll.cbSize	= sizeof(pScroll);
	pScroll.fMask	= SIF_RANGE|SIF_PAGE;
	pScroll.nMin	= 0;
	pScroll.nMax	= nWidth;
	pScroll.nPage	= rcClient.right;
	SetScrollInfo( SB_HORZ, &pScroll, TRUE );

	int nScroll = GetScrollPos( SB_HORZ );
	m_wndHeader.SetWindowPos( NULL, -nScroll, 0, rcClient.right + nScroll, HEADER_HEIGHT, SWP_SHOWWINDOW );

	CSingleLock pTransfersLock( &Transfers.m_pSection, FALSE );		// For GetNextFile()
	CSingleLock pUploadQueuesLock( &UploadQueues.m_pSection, FALSE );
	if ( ! pUploadQueuesLock.Lock( 250 ) )
		return;

	for ( POSITION posQueue = GetQueueIterator() ; posQueue ; )
	{
		CUploadQueue* pQueue = GetNextQueue( posQueue );

		POSITION posFile = GetFileIterator( pQueue );

		if ( posFile == NULL )
		{
			pQueue->m_bSelected = FALSE;
			continue;
		}

		nHeight++;

		if ( ! pQueue->m_bExpanded )
			continue;

		if ( ! pTransfersLock.Lock( 200 ) )
			continue;

		while ( posFile )
		{
			if ( GetNextFile( pQueue, posFile ) )
				nHeight++;
		}

		pTransfersLock.Unlock();
	}

	pUploadQueuesLock.Unlock();

	ZeroMemory( &pScroll, sizeof(pScroll) );
	pScroll.cbSize	= sizeof(pScroll);
	pScroll.fMask	= SIF_RANGE|SIF_PAGE;
	pScroll.nMin	= 0;
	pScroll.nMax	= nHeight;
	pScroll.nPage	= ( rcClient.bottom - HEADER_HEIGHT ) / Settings.Skin.RowSize + 1;
	SetScrollInfo( SB_VERT, &pScroll, TRUE );

	m_nFocus = min( m_nFocus, max( 0, nHeight - 1 ) );

	Invalidate();
}

//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl painting

void CUploadsCtrl::OnPaint()
{
	CRect rcClient, rcItem;
	CPaintDC dc( this );

	CSingleLock pTransfersLock( &Transfers.m_pSection, FALSE );
	if ( ! pTransfersLock.Lock( 250 ) )
		return;

	CSingleLock pUploadQueuesLock( &UploadQueues.m_pSection, FALSE );
	if ( ! pUploadQueuesLock.Lock( 250 ) )
		return;

	if ( Settings.General.LanguageRTL )
		dc.SetTextAlign( TA_RTLREADING );

	GetClientRect( &rcClient );
	rcClient.top += HEADER_HEIGHT;

	rcItem.CopyRect( &rcClient );
	rcItem.left -= GetScrollPos( SB_HORZ );
	rcItem.bottom = rcItem.top + Settings.Skin.RowSize;

	int nScroll = GetScrollPos( SB_VERT );
	int nIndex = 0;

	CFont* pfOld = (CFont*)dc.SelectObject( &CoolInterface.m_fntNormal );
	BOOL bFocus = ( GetFocus() == this );

	for ( POSITION posQueue = GetQueueIterator() ; posQueue && rcItem.top < rcClient.bottom ; )
	{
		CUploadQueue* pQueue = GetNextQueue( posQueue );

		POSITION posFile = GetFileIterator( pQueue );
		if ( posFile == NULL )
			continue;

		if ( nScroll > 0 )
		{
			nScroll --;
		}
		else
		{
			if ( rcItem.bottom > rcClient.top )
				PaintQueue( dc, rcItem, pQueue, bFocus && ( m_nFocus == nIndex ) );
			rcItem.OffsetRect( 0, Settings.Skin.RowSize );
		}

		nIndex ++;

		if ( ! pQueue->m_bExpanded )
			continue;

		while ( posFile && rcItem.top < rcClient.bottom )
		{
			int nPosition;
			CUploadFile* pFile = GetNextFile( pQueue, posFile, &nPosition );
			if ( pFile == NULL )
				continue;

			if ( nScroll > 0 )
			{
				nScroll --;
			}
			else
			{
				if ( rcItem.bottom > rcClient.top )
					PaintFile( dc, rcItem, pQueue, pFile, nPosition, bFocus && ( m_nFocus == nIndex ) );
				rcItem.OffsetRect( 0, Settings.Skin.RowSize );
			}

			nIndex ++;
		}
	}

	pUploadQueuesLock.Unlock();
	pTransfersLock.Unlock();

	dc.SelectObject( pfOld );

	rcClient.top = rcItem.top;
	if ( rcClient.top < rcClient.bottom )
		dc.FillSolidRect( &rcClient, Colors.m_crWindow );
}

void CUploadsCtrl::PaintQueue(CDC& dc, const CRect& rcRow, CUploadQueue* pQueue, BOOL bFocus)
{
	ASSUME_LOCK( UploadQueues.m_pSection );

	const BOOL bSelected = pQueue->m_bSelected;
	//const BOOL bActive = bSelected && ( GetFocus() == this );
	BOOL bLeftMargin = TRUE;

	COLORREF crNatural	= Colors.m_crWindow;
	COLORREF crBack		= bSelected ? Colors.m_crHighlight : crNatural;
	COLORREF crLeftMargin = crBack;

	// Skinnable Selection Highlight
	BOOL bSelectmark = FALSE;
	if ( bSelected && Images.m_bmSelected.m_hObject )
	{
		CRect rcDraw = rcRow;	// non-const
		if ( Images.m_bmSelectedGrey.m_hObject && GetFocus() != this )
			CoolInterface.DrawWatermark( &dc, &rcDraw, &Images.m_bmSelectedGrey );
		else
			CoolInterface.DrawWatermark( &dc, &rcDraw, &Images.m_bmSelected );
		bSelectmark = TRUE;
	}
	else
	{
		// Update Full Row Highlight
		dc.FillSolidRect( rcRow, crBack );
		dc.SetBkColor( crBack );
	}

	dc.SetBkMode( bSelectmark ? TRANSPARENT : OPAQUE );
	dc.SetTextColor( bSelected ? Colors.m_crHiText : Colors.m_crText );

	int nTextLeft = rcRow.right, nTextRight = rcRow.left;
	HDITEM pColumn = {};

	pColumn.mask = HDI_FORMAT | HDI_LPARAM;

	dc.SelectObject( &CoolInterface.m_fntBold );

	for ( int nColumn = 0 ; m_wndHeader.GetItem( nColumn, &pColumn ) ; nColumn++ )
	{
		CString strText;
		CRect rcCell;

		m_wndHeader.GetItemRect( nColumn, &rcCell );
		rcCell.left		+= rcRow.left;
		rcCell.right	+= rcRow.left;
		rcCell.top		= rcRow.top;
		rcCell.bottom	= rcRow.bottom;

		POINT ptHover;
		RECT  rcTick = { rcCell.left + 2, rcCell.top + 2, rcCell.left + 14, rcCell.bottom - 2 };
		GetCursorPos(&ptHover);
		ScreenToClient(&ptHover);

		switch ( pColumn.lParam )
		{
		case UPLOAD_COLUMN_TITLE:
			bLeftMargin = rcRow.left == rcCell.left;
			crLeftMargin = ( bLeftMargin ? crNatural : bSelected ? -1 : crBack );

			if ( bLeftMargin || ! bSelected && rcCell.Height() > 16 )
				dc.FillSolidRect( rcCell.left, rcCell.top + 16, 32, rcCell.Height() - 16, crLeftMargin );

			if ( pQueue->m_bExpanded )
			{
				CoolInterface.Draw( &dc, PtInRect( &rcTick, ptHover ) ? IDI_CLOSETICK_HOVER : IDI_CLOSETICK,
					16, rcCell.left, rcCell.top, crLeftMargin );
			}
			else
			{
				CoolInterface.Draw( &dc, PtInRect( &rcTick, ptHover ) ? IDI_OPENTICK_HOVER : IDI_OPENTICK,
					16, rcCell.left, rcCell.top, crLeftMargin );
			}

			rcCell.left += 16;
			if ( pQueue == UploadQueues.m_pTorrentQueue )
			{
				ImageList_DrawEx( m_gdiProtocols, PROTOCOL_BT, dc.GetSafeHdc(),
						rcCell.left, rcCell.top, 16, 16, crLeftMargin, CLR_DEFAULT, pQueue->m_bSelected ? ILD_SELECTED : ILD_NORMAL );
			}
			else if ( pQueue->m_nProtocols == ( 1 << PROTOCOL_HTTP ) )
			{
				ImageList_DrawEx( m_gdiProtocols, PROTOCOL_HTTP, dc.GetSafeHdc(),
						rcCell.left, rcCell.top, 16, 16, crLeftMargin, CLR_DEFAULT, pQueue->m_bSelected ? ILD_SELECTED : ILD_NORMAL );
			}
			else if ( pQueue->m_nProtocols == ( 1 << PROTOCOL_ED2K ) )
			{
				ImageList_DrawEx( m_gdiProtocols, PROTOCOL_ED2K, dc.GetSafeHdc(),
						rcCell.left, rcCell.top, 16, 16, crLeftMargin, CLR_DEFAULT, pQueue->m_bSelected ? ILD_SELECTED : ILD_NORMAL );
			}
			else
			{
				CoolInterface.Draw( &dc, pQueue->m_bExpanded ? IDI_FOLDER_OPEN : IDI_FOLDER_CLOSED,
					16, rcCell.left, rcCell.top, crLeftMargin, bSelected );
			}
			rcCell.left += 16;
			if ( bLeftMargin || ! bSelected )
				dc.FillSolidRect( rcCell.left, rcCell.top, 1, rcCell.Height(), crLeftMargin );
			rcCell.left++;

			strText = pQueue->m_sName;
			break;

		case UPLOAD_COLUMN_SIZE:
			if ( pQueue == UploadQueues.m_pTorrentQueue )
				strText.Format( _T("%u/%u"), pQueue->m_nMinTransfers, pQueue->m_nMaxTransfers );	//No. Clients was loaded into these variables
			else if ( pQueue != UploadQueues.m_pHistoryQueue )
				strText.Format( _T("%u/%u"), pQueue->GetTransferCount(), pQueue->GetQueuedCount() );
			break;

		case UPLOAD_COLUMN_SPEED:
			if ( pQueue != UploadQueues.m_pHistoryQueue )
				strText = Settings.SmartSpeed( pQueue->GetMeasuredSpeed() );
			break;
		}

		nTextLeft	= min( nTextLeft, rcCell.left );
		nTextRight	= max( nTextRight, rcCell.right );

		if ( rcCell.Width() < 8 )
			strText.Empty();

		if ( dc.GetTextExtent( strText ).cx > rcCell.Width() - 8 )
		{
			while ( dc.GetTextExtent( strText + _T('\x2026') ).cx > ( rcCell.Width() - 8 ) && ! strText.IsEmpty() )
			{
				strText.Truncate( strText.GetLength() - 1 );
			}

			if ( ! strText.IsEmpty() )
				strText += _T('\x2026');
		}

		int nWidth		= dc.GetTextExtent( strText ).cx;
		int nPosition	= 0;

		switch ( pColumn.fmt & LVCFMT_JUSTIFYMASK )
		{
		default:
			nPosition = rcCell.left + 4;
			break;
		case LVCFMT_CENTER:
			nPosition = ( ( rcCell.left + rcCell.right ) / 2 ) - ( nWidth / 2 );
			break;
		case LVCFMT_RIGHT:
			nPosition = rcCell.right - 4 - nWidth;
			break;
		}

		dc.SetBkColor( bSelectmark ? CLR_NONE : crBack );
		dc.ExtTextOut( nPosition, rcCell.top + 2,
			ETO_CLIPPED|( bSelectmark ? 0 : ETO_OPAQUE ),
			&rcCell, strText, NULL );
	}

	// Non-column whitespace area (redundant)
	//if ( nTextRight < rcRow.right && ! bSelectmark )
	//	dc.FillSolidRect( nTextRight, rcRow.top, rcRow.right, rcRow.bottom, crBack );

	dc.SelectObject( &CoolInterface.m_fntNormal );

	if ( bFocus )
	{
		CRect rcFocus( nTextLeft, rcRow.top, max( rcRow.right, nTextRight ), rcRow.bottom );
		dc.Draw3dRect( &rcFocus, Colors.m_crHiBorder, Colors.m_crHiBorder );

		if ( Settings.Skin.RoundedSelect )
		{
			dc.SetPixel( rcFocus.left, rcFocus.top, crNatural );
			dc.SetPixel( rcFocus.left, rcFocus.bottom - 1, crNatural );
			dc.SetPixel( rcRow.right - 1, rcRow.top, crNatural );
			dc.SetPixel( rcRow.right - 1, rcRow.bottom - 1, crNatural );
		}

		if ( Colors.m_crHiBorderIn )
		{
			rcFocus.DeflateRect( 1, 1 );
			dc.Draw3dRect( &rcFocus, Colors.m_crHiBorderIn, Colors.m_crHiBorderIn );
		}
	}
}

void CUploadsCtrl::PaintFile(CDC& dc, const CRect& rcRow, CUploadQueue* /*pQueue*/, CUploadFile* pFile, int nPosition, BOOL bFocus)
{
	//ASSUME_LOCK( Transfers.m_pSection );

	const BOOL bSelected = pFile->m_bSelected;
	//const BOOL bFocus = bSelected && ( GetFocus() == this );
	BOOL bLeftMargin = TRUE;

	CUploadTransfer* pTransfer = pFile->GetActive();
	COLORREF crNatural		= Colors.m_crWindow;
	COLORREF crBorder		= bSelected ? Colors.m_crFragmentBorderSelected : Colors.m_crFragmentBorder;
	COLORREF crBack			= bSelected ? Colors.m_crHighlight : crNatural;
	COLORREF crLeftMargin	= crBack;

	// Skinnable Selection Highlight
	BOOL bSelectmark = FALSE;
	if ( bSelected && Images.m_bmSelected.m_hObject )
	{
		CRect rcDraw = rcRow;
		if ( Images.m_bmSelectedGrey.m_hObject && GetFocus() != this )
			CoolInterface.DrawWatermark( &dc, &rcDraw, &Images.m_bmSelectedGrey );
		else
			CoolInterface.DrawWatermark( &dc, &rcDraw, &Images.m_bmSelected );
		bSelectmark = TRUE;
	}
	else
	{
		// Update Full Row Highlight
		dc.FillSolidRect( rcRow, crBack );
		dc.SetBkColor( crBack );
	}

	dc.SetBkMode( bSelectmark ? TRANSPARENT : OPAQUE );

	if ( pFile->m_bSelected )
		dc.SetTextColor( Colors.m_crHiText );
	else if ( pTransfer == NULL || pTransfer->m_nState == upsNull )
		dc.SetTextColor( Colors.m_crDisabled );
	else
		dc.SetTextColor( Colors.m_crText );

	int nTextLeft = rcRow.right, nTextRight = rcRow.left;

	// Crude torrent icon workaround fix
	const BOOL bMultifile = pTransfer->m_nProtocol == PROTOCOL_BT &&
		( pFile->m_sName.ReverseFind( '.' ) < 1 ||
		( pFile->m_sName.ReverseFind( '.' ) < pFile->m_sName.GetLength() - 5 ) );

	HDITEM pColumn = {};
	pColumn.mask = HDI_FORMAT | HDI_LPARAM;

	for ( int nColumn = 0 ; m_wndHeader.GetItem( nColumn, &pColumn ) ; nColumn++ )
	{
		CString strText;
		CRect rcCell;

		m_wndHeader.GetItemRect( nColumn, &rcCell );
		rcCell.left		+= rcRow.left;
		rcCell.right	+= rcRow.left;
		rcCell.top		= rcRow.top;
		rcCell.bottom	= rcRow.bottom;

		switch ( pColumn.lParam )
		{
		case UPLOAD_COLUMN_TITLE:
			bLeftMargin = rcRow.left == rcCell.left;
			crLeftMargin = ( bLeftMargin ? crNatural : bSelectmark ? -1 : crBack );

			if ( bLeftMargin || ! bSelectmark )
				dc.FillSolidRect( rcCell.left, rcCell.top, 24, rcCell.Height(), crLeftMargin );
			rcCell.left += 24;
			if ( bLeftMargin || ! bSelectmark && Settings.Skin.RowSize > 16 )
				dc.FillSolidRect( rcCell.left, rcCell.top + 16, 16, rcCell.Height() - 16, crLeftMargin );

			{
				int nIcon = ShellIcons.Get( bMultifile ? _T(".torrent") :
					( lstrcmpi( PathFindExtension( pFile->m_sPath ), _T(".partial") ) ?
					pFile->m_sPath : pFile->m_sName ), 16 );
				ShellIcons.Draw( &dc, nIcon, 16, rcCell.left, rcCell.top, crBack, bSelected );
			}

			rcCell.left += 16;
			if ( bLeftMargin || ! bSelectmark )
				dc.FillSolidRect( rcCell.left, rcCell.top, 1, rcCell.Height(), crLeftMargin );
			rcCell.left++;
			strText = pFile->m_sName;
			break;

		case UPLOAD_COLUMN_SIZE:
			strText = Settings.SmartVolume( pFile->m_nSize );
			break;

		case UPLOAD_COLUMN_PROGRESS:
			rcCell.DeflateRect( 1, 2 );
			dc.Draw3dRect( &rcCell, crBorder, crBorder );
			rcCell.DeflateRect( 1, 1 );
			CFragmentBar::DrawUpload( &dc, &rcCell, pFile, crNatural );
			break;

		case UPLOAD_COLUMN_TRANSFER:
			if ( pTransfer->m_nUploaded != NULL )
				strText = Settings.SmartVolume( pTransfer->m_nUploaded );
			break;

		case UPLOAD_COLUMN_SPEED:
			if ( pTransfer == NULL || pTransfer->m_nState == upsNull )
				LoadString( strText, IDS_STATUS_COMPLETED );
			else if ( pTransfer->m_nProtocol == PROTOCOL_BT )
			{
				CUploadTransferBT* pBT = (CUploadTransferBT*)pTransfer;

				if ( ! pBT->m_bInterested )
					LoadString( strText, IDS_STATUS_UNINTERESTED );
				else if ( pBT->m_bChoked )
					LoadString( strText, IDS_STATUS_CHOKED );
				else
				{
					DWORD nSpeed = pTransfer->GetMeasuredSpeed();
					if ( nSpeed )
						strText = Settings.SmartSpeed( nSpeed );
				}
			}
			else if ( nPosition > 0 )
			{
				CString strQ;
				LoadString( strQ, IDS_STATUS_Q );
				strText.Format( _T("%s %i"), (LPCTSTR)strQ, nPosition );
			}
			else
			{
				DWORD nSpeed = pTransfer->GetMeasuredSpeed();
				if ( nSpeed )
					strText = Settings.SmartSpeed( nSpeed );
				else
					LoadString( strText, IDS_STATUS_NEXT );
			}
			break;

		case UPLOAD_COLUMN_RATING:
			strText.Format(_T("%u"), pTransfer->m_nUserRating );
			break;

		case UPLOAD_COLUMN_USER:
			if ( pTransfer == NULL )
				strText.Empty();
			else if ( pTransfer->m_sRemoteNick.IsEmpty() )
				strText = pTransfer->m_sAddress;
			else
				strText = pTransfer->m_sRemoteNick + _T(" (") + pTransfer->m_sAddress + _T(")");
			break;

		case UPLOAD_COLUMN_CLIENT:
			if ( pTransfer != NULL ) strText = pTransfer->m_sUserAgent;
			break;

		case UPLOAD_COLUMN_COUNTRY:
			int nFlagImage = Flags.GetFlagIndex( pTransfer->m_sCountry );

			if ( ! bSelectmark )
				dc.FillSolidRect( rcCell.left, rcCell.top, 20, rcCell.Height(), crBack );
			rcCell.left += 2;
			if ( nFlagImage >= 0 )
				Flags.Draw( nFlagImage, dc.GetSafeHdc(), rcCell.left, rcCell.top,
					crBack, CLR_DEFAULT, bSelected ? ILD_SELECTED : ILD_NORMAL );
			rcCell.left += 16;

			strText = pTransfer->m_sCountry;
			break;
		}

		nTextLeft	= min( nTextLeft, rcCell.left );
		nTextRight	= max( nTextRight, rcCell.right );

		if ( pColumn.lParam == UPLOAD_COLUMN_PROGRESS )
			continue;

		if ( rcCell.Width() < 8 ) strText.Empty();

		if ( dc.GetTextExtent( strText ).cx > rcCell.Width() - 8 )
		{
			while ( dc.GetTextExtent( strText + _T('\x2026') ).cx > ( rcCell.Width() - 8 ) && ! strText.IsEmpty() )
			{
				strText.Truncate( strText.GetLength() - 1 );
			}

			if ( ! strText.IsEmpty() )
				strText += _T('\x2026');
		}

		int nWidth	= dc.GetTextExtent( strText ).cx;
		int nPos	= 0;

		switch ( pColumn.fmt & LVCFMT_JUSTIFYMASK )
		{
		default:
			nPos = ( rcCell.left + 4 );
			break;
		case LVCFMT_CENTER:
			nPos = ( ( rcCell.left + rcCell.right ) / 2 ) - ( nWidth / 2 );
			break;
		case LVCFMT_RIGHT:
			nPos = ( rcCell.right - 4 - nWidth );
			break;
		}

		dc.SetBkColor( bSelectmark ? CLR_NONE : crBack );
		dc.ExtTextOut( nPos, rcCell.top + 2,
			ETO_CLIPPED|( bSelectmark ? 0 : ETO_OPAQUE ),
			&rcCell, strText, NULL );
	}

	// Non-column whitespace area (redundant)
	//if ( nTextRight < rcRow.right && ! bSelectmark )
	//	dc.FillSolidRect( nTextRight, rcRow.top, rcRow.right, rcRow.bottom, crBack );

	if ( bFocus )
	{
		CRect rcFocus( nTextLeft, rcRow.top, max( rcRow.right, nTextRight ), rcRow.bottom );
		dc.Draw3dRect( &rcFocus, Colors.m_crHiBorder, Colors.m_crHiBorder );

		if ( Settings.Skin.RoundedSelect )
		{
			dc.SetPixel( rcFocus.left, rcFocus.top, crNatural );
			dc.SetPixel( rcFocus.left, rcFocus.bottom - 1, crNatural );
			dc.SetPixel( rcRow.right - 1, rcRow.top, crNatural );
			dc.SetPixel( rcRow.right - 1, rcRow.bottom - 1, crNatural );
		}

		if ( Colors.m_crHiBorderIn )
		{
			rcFocus.DeflateRect( 1, 1 );
			dc.Draw3dRect( &rcFocus, Colors.m_crHiBorderIn, Colors.m_crHiBorderIn );
		}
	}
}

void CUploadsCtrl::OnSkinChange()
{
	m_wndHeader.SetFont( &CoolInterface.m_fntNormal );

	CoolInterface.LoadIconsTo( m_gdiProtocols, protocolIDs );

	// Update Dropshadow
	m_wndTip.DestroyWindow();
	m_wndTip.Create( this, &Settings.Interface.TipUploads );
}

//////////////////////////////////////////////////////////////////////////////
// CUploadsCtrl interaction message handlers

void CUploadsCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	SCROLLINFO pInfo = {};

	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_ALL & ~SIF_TRACKPOS;

	GetScrollInfo( SB_VERT, &pInfo );
	int nDelta = pInfo.nPos;

	switch ( nSBCode )
	{
	case SB_BOTTOM:
		pInfo.nPos = pInfo.nMax - pInfo.nPage;
		break;
	case SB_LINEDOWN:
		pInfo.nPos ++;
		break;
	case SB_LINEUP:
		pInfo.nPos --;
		break;
	case SB_PAGEDOWN:
		pInfo.nPos += pInfo.nPage;
		break;
	case SB_PAGEUP:
		pInfo.nPos -= pInfo.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pInfo.nPos = nPos;
		break;
	case SB_TOP:
		pInfo.nPos = 0;
		break;
	}

	pInfo.nPos = max( 0, min( pInfo.nPos, pInfo.nMax - (int)pInfo.nPage + 1 ) );
	if ( pInfo.nPos == nDelta ) return;

	SetScrollInfo( SB_VERT, &pInfo, TRUE );
	Invalidate();
}

void CUploadsCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	SCROLLINFO pInfo = {};

	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_ALL & ~SIF_TRACKPOS;

	GetScrollInfo( SB_HORZ, &pInfo );
	int nDelta = pInfo.nPos;

	switch ( nSBCode )
	{
	case SB_BOTTOM:
		pInfo.nPos = pInfo.nMax - pInfo.nPage;
		break;
	case SB_LINEDOWN:
		pInfo.nPos ++;
		break;
	case SB_LINEUP:
		pInfo.nPos --;
		break;
	case SB_PAGEDOWN:
		pInfo.nPos += pInfo.nPage;
		break;
	case SB_PAGEUP:
		pInfo.nPos -= pInfo.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pInfo.nPos = nPos;
		break;
	case SB_TOP:
		pInfo.nPos = 0;
		break;
	}

	pInfo.nPos = max( 0, min( pInfo.nPos, pInfo.nMax - (int)pInfo.nPage + 1 ) );
	if ( pInfo.nPos == nDelta ) return;

	SetScrollInfo( SB_HORZ, &pInfo, TRUE );

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndHeader.SetWindowPos( NULL, -pInfo.nPos, 0,
		rcClient.right + pInfo.nPos, HEADER_HEIGHT, SWP_NOZORDER );

	RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW );
}

BOOL CUploadsCtrl::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/)
{
	OnVScroll( SB_THUMBPOSITION, GetScrollPos( SB_VERT ) - zDelta / WHEEL_DELTA * theApp.m_nMouseWheel );
	return TRUE;
}

void CUploadsCtrl::OnChangeHeader(NMHDR* /*pNotifyStruct*/, LRESULT* /*pResult*/)
{
	Update();
}

void CUploadsCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CSingleLock pLock( &Transfers.m_pSection, TRUE );
	CUploadFile* pFile;
	CUploadQueue* pQueue;

	m_wndTip.Hide();

	switch ( nChar )
	{
	case VK_HOME:
		SelectTo( 0 );
		return;
	case VK_END:
		{
			INT nMin, nMax;
			GetScrollRange( SB_VERT, &nMin, &nMax );
			SelectTo( max( 0, nMax - 1 ) );
		}
		return;
	case VK_UP:
		SelectTo( m_nFocus - 1 );
		return;
	case VK_DOWN:
		SelectTo( m_nFocus + 1 );
		return;
	case VK_PRIOR:
		SelectTo( m_nFocus - 10 );
		return;
	case VK_NEXT:
		SelectTo( m_nFocus + 10 );
		return;
	case VK_LEFT:
	case '-':
		if ( GetAt( m_nFocus, &pQueue, &pFile ) )
		{
			if ( pFile != NULL && pFile->GetActive() != NULL )
				pQueue = pFile->GetActive()->m_pQueue;
			if ( pQueue != NULL && pQueue->m_bExpanded == TRUE )
			{
				pQueue->m_bExpanded = FALSE;
				Update();
			}
		}
		return;
	case VK_RIGHT:
	case '+':
		if ( GetAt( m_nFocus, &pQueue, NULL ) && pQueue != NULL && pQueue->m_bExpanded == FALSE )
		{
			pQueue->m_bExpanded = TRUE;
			Update();
		}
		return;
	}

	CWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}

void CUploadsCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CUploadFile* pFile;
	CUploadQueue* pQueue;
	CRect rcItem;
	int nIndex;

	SetFocus();
	m_wndTip.Hide();

	CSingleLock pLock( &Transfers.m_pSection, FALSE );
	if ( ! pLock.Lock( 500 ) ) return;

	if ( HitTest( point, &pQueue, &pFile, &nIndex, &rcItem ) )
	{
		int nTitleStarts = GetExpandableColumnX();
		if ( point.x > nTitleStarts && point.x <= nTitleStarts + rcItem.left + 16 )
		{
			if ( pQueue != NULL )
			{
				pQueue->m_bExpanded = ! pQueue->m_bExpanded;

				if ( ! pQueue->m_bExpanded )
				{
					for ( POSITION posActive = pQueue->GetActiveIterator() ; posActive ; )
					{
						CUploadTransfer* pTransfer = pQueue->GetNextActive( posActive );
						if ( pTransfer->m_pBaseFile != NULL )
							pTransfer->m_pBaseFile->m_bSelected = FALSE;
					}

					for ( DWORD nPos = 0 ; nPos < pQueue->GetQueuedCount() ; nPos ++ )
					{
						CUploadTransfer* pTransfer = (CUploadTransfer*)pQueue->GetQueuedAt( nPos );
						if ( pTransfer->m_pBaseFile != NULL )
							pTransfer->m_pBaseFile->m_bSelected = FALSE;
					}
				}

				Update();
			}
		}
		else
		{
			if ( pFile != NULL && pFile->m_bSelected )
			{
				if ( ( nFlags & ( MK_SHIFT | MK_CONTROL | MK_RBUTTON ) ) == 0 )
					m_pDeselect = pFile;
			}
			else if ( nFlags & MK_RBUTTON )
			{
				DeselectAll();
			}

			SelectTo( nIndex );
		}
	}
	else if ( ( nFlags & ( MK_SHIFT | MK_CONTROL ) ) == 0 )
	{
		DeselectAll();
		Update();
	}
}

void CUploadsCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_wndTip.Hide();
	OnLButtonDown( nFlags, point );
	CWnd::OnRButtonDown( nFlags, point );
}

void CUploadsCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CUploadFile* pFile;
	CUploadQueue* pQueue;
	CRect rcItem;

	SetFocus();

	CSingleLock pLock( &Transfers.m_pSection, FALSE );
	if ( ! pLock.Lock( 500 ) ) return;

	if ( HitTest( point, &pQueue, &pFile, NULL, &rcItem ) )
	{
		int nTitleStarts = GetExpandableColumnX();
		if ( pQueue != NULL && point.x > nTitleStarts && point.x <= nTitleStarts + rcItem.left + 16 )
		{
			pQueue->m_bExpanded = ! pQueue->m_bExpanded;

			if ( ! pQueue->m_bExpanded )
			{
				for ( POSITION posActive = pQueue->GetActiveIterator() ; posActive ; )
				{
					CUploadTransfer* pTransfer = pQueue->GetNextActive( posActive );
					if ( pTransfer->m_pBaseFile != NULL )
						pTransfer->m_pBaseFile->m_bSelected = FALSE;
				}

				for ( DWORD nPos = 0 ; nPos < pQueue->GetQueuedCount() ; nPos ++ )
				{
					CUploadTransfer* pTransfer = (CUploadTransfer*)pQueue->GetQueuedAt( nPos );
					if ( pTransfer->m_pBaseFile != NULL )
						pTransfer->m_pBaseFile->m_bSelected = FALSE;
				}
			}

			Update();
		}
		else if ( pQueue != NULL )
		{
			GetOwner()->PostMessage( WM_TIMER, 5 );
			GetOwner()->PostMessage( WM_COMMAND, ID_UPLOADS_SETTINGS );
		}
		else if ( pFile != NULL )
		{
			GetOwner()->PostMessage( WM_TIMER, 5 );
			GetOwner()->PostMessage( WM_COMMAND, ID_UPLOADS_LAUNCH );
		}
	}

	CWnd::OnLButtonDblClk( nFlags, point );
}

void CUploadsCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove( nFlags, point );

	if ( ( nFlags & ( MK_LBUTTON|MK_RBUTTON) ) == 0 )
	{
		CUploadFile* pFile;
		CRect rcItem;

		CSingleLock pLock( &Transfers.m_pSection, FALSE );
		if ( ! pLock.Lock( 500 ) ) return;

		if ( HitTest( point, NULL, &pFile, NULL, &rcItem ) )
		{
			// Tick Hoverstates
			if ( point.x < rcItem.left + 18 )
			{
				CRect rcRefresh( 1, rcItem.top - 32, 18, rcItem.bottom + 32 );
				RedrawWindow(rcRefresh);
			}
			if ( pFile != NULL )
			{
				m_wndTip.Show( pFile );
				return;
			}
		}
	}

	m_wndTip.Hide();
}

void CUploadsCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_pDeselect )
	{
		DeselectAll( m_pDeselect );
		m_pDeselect = NULL;
	}

	CWnd::OnLButtonUp( nFlags, point );
}

void CUploadsCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	if ( m_pDeselect )
	{
		DeselectAll( m_pDeselect );
		m_pDeselect = NULL;
	}

	CWnd::OnRButtonUp( nFlags, point );
}

void CUploadsCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus( pOldWnd );
	Invalidate();
}

void CUploadsCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus( pNewWnd );
	Invalidate();
}

int CUploadsCtrl::GetExpandableColumnX() const
{
	int nTitleStarts = 0;

	HDITEM pColumn = {};
	pColumn.mask = HDI_LPARAM | HDI_WIDTH;

	for ( int nColumn = 0 ; m_wndHeader.GetItem( m_wndHeader.OrderToIndex( nColumn ), &pColumn ) ; nColumn++ )
	{
		if ( pColumn.lParam == UPLOAD_COLUMN_TITLE )
			break;

		nTitleStarts += pColumn.cxy;
	}

	return nTitleStarts;
}

UINT CUploadsCtrl::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}
