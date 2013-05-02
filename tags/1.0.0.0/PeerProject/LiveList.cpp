//
// LiveList.cpp
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
#include "PeerProject.h"
#include "LiveList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CLiveList construction

IMPLEMENT_DYNAMIC( CLiveList, CObject )

CLiveList::CLiveList(int nColumns, UINT nHash)
	: m_nColumns ( nColumns )
{
	m_pItems.InitHashTable( GetBestHashTableSize( nHash ) );
}

CLiveList::~CLiveList()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CLiveList clear

void CLiveList::Clear()
{
	ASSERT_VALID( this );

	CQuickLock oLock( m_pSection );

	for ( POSITION pos = m_pItems.GetStartPosition() ; pos ; )
	{
		CLiveItem* pItem;
		DWORD_PTR nParam;

		m_pItems.GetNextAssoc( pos, nParam, pItem );
		ASSERT_VALID( pItem );
		delete pItem;
	}
	m_pItems.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CLiveList add

CLiveItem* CLiveList::Add(DWORD_PTR nParam)
{
	ASSERT_VALID( this );

	CLiveItem* pItem = new CLiveItem( m_nColumns, nParam );
	ASSERT_VALID( pItem );
	m_pItems.SetAt( nParam, pItem );
	return pItem;
}

CLiveItem* CLiveList::Add(LPVOID pParam)
{
	ASSERT_VALID( this );

	return Add( (DWORD_PTR)pParam );
}

//////////////////////////////////////////////////////////////////////
// CLiveList apply

void CLiveList::Apply(CListCtrl* pCtrl, BOOL bSort)
{
	ASSERT_VALID( this );

	BOOL bModified = FALSE;

	int nCount = pCtrl->GetItemCount();

// Start flicker workaround for VS2012
//#if defined(_MSC_VER) && (_MSC_VER >= 1700)		// #ifdef _USING_V110_SDK71_
#if defined(_MSC_PLATFORM_TOOLSET) && (_MSC_PLATFORM_TOOLSET >= 110)		// Custom define
	pCtrl->SendMessage( WM_SETREDRAW, FALSE );
	const int nSelected = pCtrl->GetNextItem( -1, LVNI_SELECTED );
#endif

	CQuickLock oLock( m_pSection );

	for ( int nItem = 0 ; nItem < nCount ; nItem++ )
	{
		DWORD nParam = (DWORD)pCtrl->GetItemData( nItem );
		CLiveItem* pItem;

		if ( m_pItems.Lookup( nParam, pItem ) )
		{
			ASSERT_VALID( pItem );
			if ( pItem->Update( pCtrl, nItem, m_nColumns ) )
				bModified = TRUE;

			delete pItem;
			m_pItems.RemoveKey( nParam );
		}
		else
		{
			pCtrl->DeleteItem( nItem-- );
			bModified = TRUE;
			nCount--;
		}
	}

	nCount = pCtrl->GetItemCount();		// ToDo: Is this double-check needed?

	for ( POSITION pos = m_pItems.GetStartPosition() ; pos ; )
	{
		CLiveItem* pItem;
		DWORD_PTR nParam;

		m_pItems.GetNextAssoc( pos, nParam, pItem );
		ASSERT_VALID( pItem );
		pItem->Add( pCtrl, nCount++, m_nColumns );
		bModified = TRUE;

		delete pItem;
	}

	m_pItems.RemoveAll();

	if ( bModified && bSort )
		Sort( pCtrl, -1 );

// End flicker workaround for VS2012
//#if defined(_MSC_VER) && (_MSC_VER >= 1700)		// #ifdef _USING_V110_SDK71_
#if defined(_MSC_PLATFORM_TOOLSET) && (_MSC_PLATFORM_TOOLSET >= 110)		// Custom define
	pCtrl->SendMessage( WM_SETREDRAW, TRUE );
	if ( nSelected >= 0 )
	{
		pCtrl->SetItemState( nSelected, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		pCtrl->SetSelectionMark( nSelected );
	}
#endif
}


//////////////////////////////////////////////////////////////////////
// CLiveItem construction

IMPLEMENT_DYNAMIC( CLiveItem, CObject )

CLiveItem::CLiveItem(int nColumns, DWORD_PTR nParam)
	: m_bModified	( true )
	, m_nModified	( 0xffffffff )
	, m_nParam		( nParam )
	, m_nMaskOverlay ( 0 )
	, m_nMaskState	( 0 )
	, m_bOld		( false )
{
	m_pColumn.SetSize( nColumns );
	m_nImage.SetSize( nColumns );

	for ( int i = 0 ; i < nColumns ; ++i )
		m_nImage[ i ] = -1;
}

CLiveItem::~CLiveItem()
{
}

//////////////////////////////////////////////////////////////////////
// CLiveItem set

void CLiveItem::Set(int nColumn, const CString& sText)
{
	ASSERT_VALID( this );
	ASSERT( nColumn >= 0 && nColumn < m_pColumn.GetSize() );

	if ( m_pColumn[ nColumn ] != sText )
	{
		m_bModified = true;
		m_nModified = m_nModified | ( 1 << nColumn );
	}
	m_pColumn[ nColumn ] = sText;
}

void CLiveItem::SetImage(int nColumn, int nImage)
{
	ASSERT_VALID( this );
	ASSERT( nColumn >= 0 && nColumn < m_pColumn.GetSize() );
	if ( nColumn >= m_pColumn.GetSize() )
		return;

	m_bModified = ( m_nImage[ nColumn ] != nImage );
	m_nImage[ nColumn ] = nImage;
}

void CLiveItem::SetImage(int nImage)
{
	ASSERT_VALID( this );

	m_bModified = ( m_nImage[ 0 ] != nImage );
	m_nImage[ 0 ] = nImage;
}

void CLiveItem::SetMaskOverlay(UINT nMaskOverlay)
{
	ASSERT_VALID( this );

	m_bModified = ( m_nMaskOverlay != nMaskOverlay );
	m_nMaskOverlay = nMaskOverlay;
}

//////////////////////////////////////////////////////////////////////
// CLiveItem format

void CLiveItem::Format(int nColumn, LPCTSTR pszFormat, ...)
{
	ASSERT_VALID( this );
	ASSERT( pszFormat );
	ASSERT( nColumn >= 0 && nColumn < m_pColumn.GetSize() );

	TCHAR szBuffer[1024];
	va_list pArgs;

	va_start( pArgs, pszFormat );
	_vsntprintf( szBuffer, sizeof( szBuffer ) / sizeof( TCHAR ), pszFormat, pArgs );
	szBuffer[ sizeof( szBuffer ) / sizeof( TCHAR ) - 1 ] = 0;
	va_end( pArgs );

	if ( m_pColumn[ nColumn ] != szBuffer )
	{
		m_bModified = true;
		m_nModified = m_nModified | ( 1 << nColumn );
	}
	m_pColumn[ nColumn ] = szBuffer;
}

//////////////////////////////////////////////////////////////////////
// CLiveItem add

int CLiveItem::Add(CListCtrl* pCtrl, int nItem, int nColumns)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pCtrl );

	LV_ITEM pItem = {};
	pItem.mask		= LVIF_PARAM|LVIF_TEXT|LVIF_STATE;
	pItem.iItem		= nItem >= 0 ? nItem : pCtrl->GetItemCount();
	if ( m_nImage[ 0 ] >= 0 )
	{
		pItem.mask |= LVIF_IMAGE;
		pItem.iImage = m_nImage[ 0 ];
	}
	pItem.lParam	= (LPARAM)m_nParam;
	pItem.state		= INDEXTOOVERLAYMASK( m_nMaskOverlay ) | INDEXTOSTATEIMAGEMASK( m_nMaskState );
	pItem.stateMask	= LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
	pItem.pszText	= (LPTSTR)(LPCTSTR)m_pColumn[0];
	pItem.iItem		= pCtrl->InsertItem( &pItem );
	if ( pItem.iItem != -1 )
	{
		for ( pItem.iSubItem = 1 ; pItem.iSubItem < nColumns ; pItem.iSubItem++ )
		{
			pItem.mask = LVIF_TEXT;
			pItem.pszText = (LPTSTR)(LPCTSTR)m_pColumn[ pItem.iSubItem ];
			if ( m_nImage[ pItem.iSubItem ] >= 0 )
			{
				pItem.mask |= LVIF_IMAGE;
				pItem.iImage = m_nImage[ pItem.iSubItem ];
			}
			VERIFY( pCtrl->SetItem( &pItem ) );
		}
	}

	return pItem.iItem;
}

//////////////////////////////////////////////////////////////////////
// CLiveItem update

BOOL CLiveItem::Update(CListCtrl* pCtrl, int nItem, int nColumns)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pCtrl );

	BOOL bModified = FALSE;

	LV_ITEM pMainItem = {
		LVIF_PARAM | LVIF_IMAGE | LVIF_STATE,	// .mask
		nItem,									// .iItem
		0,
		0,
		LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK	// .stateMask
	};

	if ( ! pCtrl->GetItem( &pMainItem ) || pMainItem.lParam != (LPARAM)m_nParam )
		return FALSE;

	if ( ( pMainItem.state & ( LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK ) ) != ( INDEXTOOVERLAYMASK( m_nMaskOverlay ) | INDEXTOSTATEIMAGEMASK( m_nMaskState ) ) )
	{
		pMainItem.state		= INDEXTOOVERLAYMASK( m_nMaskOverlay ) | INDEXTOSTATEIMAGEMASK( m_nMaskState );
		pMainItem.stateMask	= LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
		bModified = TRUE;
	}

	if ( bModified )
		VERIFY( pCtrl->SetItem( &pMainItem ) );

	CString buf;
	for ( int i = 0 ; i < nColumns ; ++i )
	{
		LV_ITEM pItem = {
			LVIF_IMAGE | LVIF_TEXT,
			nItem,
			i,
			0,
			0,
			buf.GetBuffer( 1024 ),
			1024
		};

		BOOL bResult = pCtrl->GetItem( &pItem );
		buf.ReleaseBuffer();
		if ( ! bResult )
			return FALSE;

		pItem.mask = 0;
		if ( ! pItem.pszText || m_pColumn[ pItem.iSubItem ] != pItem.pszText )
		{
			pItem.mask |= LVIF_TEXT;
			pItem.pszText = (LPTSTR)(LPCTSTR)m_pColumn[ pItem.iSubItem ];
		}

		if ( m_nImage[ pItem.iSubItem ] >= 0 &&
			 m_nImage[ pItem.iSubItem ] != pItem.iImage )
		{
			pItem.mask |= LVIF_IMAGE;
			pItem.iImage = m_nImage[ pItem.iSubItem ];
		}

		if ( pItem.mask )
		{
			VERIFY( pCtrl->SetItem( &pItem ) );
			bModified = TRUE;
		}
	}

	return bModified;
}

//BOOL CLiveItem::SetImage(CListCtrl* pCtrl, LPARAM nParam, int nColumn, int nImageIndex)
//{
//	ASSERT_VALID( this );
//	ASSERT_VALID( pCtrl );
//
//	BOOL bModified = FALSE;
//	LV_FINDINFO pFind = {};
//	pFind.flags  = LVFI_PARAM;
//	pFind.lParam = nParam;
//	int nItem = pCtrl->FindItem( &pFind );
//	if ( nItem < 0 ) return FALSE;
//
//	LV_ITEM pItem = {};
//	pItem.mask  = LVIF_IMAGE;
//	pItem.iItem = nItem;
//	pItem.iSubItem = nColumn;
//
//	if ( ! pCtrl->GetItem( &pItem ) ) return FALSE;
//
//	if ( pItem.iImage != nImageIndex )
//		bModified = TRUE;
//
//	if ( bModified )
//	{
//		pItem.iImage = nImageIndex;
//		pCtrl->SetItem( &pItem );
//	}
//
//	return bModified;
//}


//////////////////////////////////////////////////////////////////////
// CLiveList sort method

CBitmap CLiveList::m_bmSortAsc;
CBitmap CLiveList::m_bmSortDesc;

void CLiveList::Sort(CListCtrl* pCtrl, int nColumn, BOOL bGraphic)
{
	ASSERT_VALID( pCtrl );

	int nOldColumn = (int)GetWindowLongPtr( pCtrl->GetSafeHwnd(), GWLP_USERDATA );

	if ( nColumn == -1 )
	{
		nColumn = nOldColumn;
	}
	else
	{
		if ( nColumn == abs( nOldColumn ) - 1 )
		{
			if ( nOldColumn > 0 )
				nColumn = 0 - nOldColumn;
			else
				nColumn = 0;
		}
		else
		{
			nColumn++;
		}

		SetWindowLongPtr( pCtrl->GetSafeHwnd(), GWLP_USERDATA, nColumn );
	}

#ifdef IDB_SORT_ASC
	if ( bGraphic )
	{
		if ( ! m_bmSortAsc.m_hObject )
		{
			// Palette .bmp - 192,192,192 swapped to system color
			m_bmSortAsc.LoadMappedBitmap( IDB_SORT_ASC );
			m_bmSortDesc.LoadMappedBitmap( IDB_SORT_DESC );
		}

		CHeaderCtrl* pHeader = (CHeaderCtrl*)CWnd::FromHandle( (HWND)pCtrl->SendMessage( LVM_GETHEADER ) );
		ASSERT_VALID( pHeader );
		for ( int nCol = 0 ; ; nCol++ )
		{
			HDITEM pColumn = {};
			pColumn.mask = HDI_BITMAP|HDI_FORMAT;

			if ( ! pHeader->GetItem( nCol, &pColumn ) ) break;

			HBITMAP hbm;
			int fmt;
			if ( nCol == abs( nColumn ) - 1 )
			{
				fmt = pColumn.fmt | HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
				hbm = (HBITMAP)( nColumn > 0 ? m_bmSortAsc.GetSafeHandle() : m_bmSortDesc.GetSafeHandle() );
			}
			else
			{
				fmt = pColumn.fmt & ~HDF_BITMAP;
				hbm = NULL;
			}
			if ( pColumn.fmt != fmt || pColumn.hbm != hbm )
			{
				pColumn.fmt = fmt;
				pColumn.hbm = hbm;
				VERIFY( pHeader->SetItem( nCol, &pColumn ) );
			}
		}
	}
#endif // IDB_SORT_ASC/DESC

	if ( nColumn )
		pCtrl->SendMessage( LVM_SORTITEMS, (WPARAM)pCtrl, (LPARAM)SortCallback );
}

//////////////////////////////////////////////////////////////////////
// CLiveList sort callback

int CALLBACK CLiveList::SortCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pList = (CListCtrl*)lParamSort;
	ASSERT_VALID( pList );

	int nColumn = (int)GetWindowLongPtr( pList->GetSafeHwnd(), GWLP_USERDATA );

	LV_FINDINFO pFind;
	pFind.flags  = LVFI_PARAM;
	pFind.lParam = lParam1;
	int nA = pList->FindItem( &pFind );
	pFind.lParam = lParam2;
	int nB = pList->FindItem( &pFind );

	CString sA( pList->GetItemText( nA, abs( nColumn ) - 1 ) );
	CString sB( pList->GetItemText( nB, abs( nColumn ) - 1 ) );

	return ( nColumn > 0 ) ? SortProc( sB, sA ) : SortProc( sA, sB );
}

bool CLiveList::Less(const CLiveItemPtr& _Left, const CLiveItemPtr& _Right, int nSortColumn)
{
	if ( nSortColumn < 0 )
		return ( CLiveList::SortProc( _Left->m_pColumn[ - nSortColumn - 1 ],
			_Right->m_pColumn[ - nSortColumn - 1 ] ) < 0 );

	if ( nSortColumn > 0 )
		return ( CLiveList::SortProc( _Right->m_pColumn[ nSortColumn - 1 ],
			_Left->m_pColumn[ nSortColumn - 1 ] ) < 0 );

	return false;
}

//////////////////////////////////////////////////////////////////////
// Parse IP addresses like "xxx.xxx.xxx.xxx\0" and "xxx.xxx.xxx.xxx/"
// (IP stored in host byte order)

inline BOOL atoip (LPCTSTR c, DWORD& addr)
{
	DWORD digit = 0;
	DWORD num = 0;
	addr = 0;
	for ( ; ; c++ )
	{
		if ( *c >= _T('0') && *c <= _T('9') )
		{
			num = num * 10 + ( *c - _T('0') );
			if ( num > 255 )
				break;				// Octet too big
		}
		else if ( *c == _T('.') || *c == _T('\0') || *c == _T('/') )
		{
			addr = ( addr << 8 ) | num;
			num = 0;
			digit++;
			if ( digit == 4 )
			{
				if ( *c == _T('.') )
					break;			// Too long

				return TRUE;		// IP found
			}
			if ( *c == _T('\0') || *c == _T('/') )
				break;				// Too short
		}
		else
			break;
	}
	addr = 0xffffffff;
	return FALSE;					// Invalid symbol
}

int CLiveList::SortProc(LPCTSTR sB, LPCTSTR sA, BOOL bNumeric)
{
	// ToDo: Fix Click-order properly (Revert above sB-sA workaround!)

	DWORD ipA, ipB;
	if ( atoip( sA, ipA ) && atoip( sB, ipB ) )
	{
		TCHAR* pA = (TCHAR*)_tcschr( sA, '/' );
		TCHAR* pB = (TCHAR*)_tcschr( sB, '/' );
		DWORD maskA = 0xffffffff, maskB = 0xffffffff;
		if ( ( ! pA || atoip( pA + 1, maskA ) ) &&
			 ( ! pB || atoip( pB + 1, maskB ) ) )
		{
			QWORD nA = ( ( (QWORD) ipA ) << 32 ) | maskA;
			QWORD nB = ( ( (QWORD) ipB ) << 32 ) | maskB;
			if ( nA < nB )
				return -1;
			if ( nA > nB )
				return 1;

			return 0;
		}
	}

	if ( bNumeric || ( IsNumber( sA ) && IsNumber( sB ) ) )
	{
		double nA = 0, nB = 0;

			if ( *sA == '(' || *sA == 'Q' )
				_stscanf( sA+1, _T("%lf"), &nA );
			else
				_stscanf( sA, _T("%lf (%lf)"), &nA, &nA );

			if ( *sB == '(' || *sB == 'Q' )
				_stscanf( sB+1, _T("%lf"), &nB );
			else
				_stscanf( sB, _T("%lf (%lf)"), &nB, &nB );

			if ( _tcsstr( sA, _T(" K") ) ) nA *= 1024;
			if ( _tcsstr( sA, _T(" M") ) ) nA *= 1024*1024;
			if ( _tcsstr( sA, _T(" G") ) ) nA *= 1024*1024*1024;
			if ( _tcsstr( sA, _T(" T") ) ) nA *= 1099511627776.0f;

			if ( _tcsstr( sB, _T(" K") ) ) nB *= 1024;
			if ( _tcsstr( sB, _T(" M") ) ) nB *= 1024*1024;
			if ( _tcsstr( sB, _T(" G") ) ) nB *= 1024*1024*1024;
			if ( _tcsstr( sB, _T(" T") ) ) nB *= 1099511627776.0f;

		if ( nB < nA )
			return -1;
		if ( nB > nA )
			return 1;

		return 0;
	}

	return _tcsicoll( sA, sB );
}

BOOL CLiveList::IsNumber(LPCTSTR pszString)
{
	if ( ! *pszString ) return FALSE;

	// ToDo: Is this the best way to do this?
	if ( *pszString == '(' && _tcsstr( pszString, _T(" source") ) != NULL ) return TRUE;
	if ( *pszString == 'Q' && _istdigit( pszString[1] ) ) return TRUE;

	BOOL bSpace = FALSE;
	int nNonDigit = 0;

	for ( ; *pszString ; pszString++ )
	{
		if ( _istdigit( *pszString ) || *pszString == '.' )
		{
			// if ( bSpace ) return FALSE;
		}
		else if ( *pszString == ' ' )
		{
			if ( bSpace ) return FALSE;
			bSpace = TRUE;
		}
		else if ( *pszString == 'k' && ( ! pszString[1] || pszString[1] == '~' ) )
		{
			// Allows ###k and ###k~
			return TRUE;
		}
		else if ( *pszString == '(' || *pszString == ')' )
		{
			// Do nothing?
		}
		else
		{
			if ( ! bSpace ) return FALSE;
			if ( ++nNonDigit > 4 ) return FALSE;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CLiveList dragging helper

HBITMAP CLiveList::CreateDragImage(CListCtrl* pList, const CPoint& ptMouse, CPoint& ptMiddle)
{
	ASSERT_VALID( pList );

	CRect rcClient, rcOne, rcAll( 32000, 32000, -32000, -32000 );
	int nIndex;

	if ( pList->GetSelectedCount() == 0 ) return NULL;

	pList->SetFocus();
	pList->GetClientRect( &rcClient );

	for ( nIndex = -1 ; ( nIndex = pList->GetNextItem( nIndex, LVNI_SELECTED ) ) >= 0 ; )
	{
		pList->GetItemRect( nIndex, rcOne, LVIR_BOUNDS );

		if ( rcOne.IntersectRect( &rcClient, &rcOne ) )
		{
			rcAll.left		= min( rcAll.left, rcOne.left );
			rcAll.top		= min( rcAll.top, rcOne.top );
			rcAll.right		= max( rcAll.right, rcOne.right );
			rcAll.bottom	= max( rcAll.bottom, rcOne.bottom );
		}

		pList->SetItemState( nIndex, 0, LVIS_FOCUSED );
	}

	BOOL bClipped = rcAll.Height() > MAX_DRAG_SIZE;

	if ( bClipped )
	{
		rcAll.left		= max( rcAll.left, ptMouse.x - MAX_DRAG_SIZE_2 );
		rcAll.right		= max( rcAll.right, ptMouse.x + MAX_DRAG_SIZE_2 );
		rcAll.top		= max( rcAll.top, ptMouse.y - MAX_DRAG_SIZE_2 );
		rcAll.bottom	= max( rcAll.bottom, ptMouse.y + MAX_DRAG_SIZE_2 );
	}

	CClientDC dcClient( pList );
	CBitmap bmAll, bmDrag;
	CDC dcAll, dcDrag;

	if ( ! dcAll.CreateCompatibleDC( &dcClient ) )
		return NULL;
	if ( ! bmAll.CreateCompatibleBitmap( &dcClient, rcClient.Width(), rcClient.Height() ) )
		return NULL;

	if ( ! dcDrag.CreateCompatibleDC( &dcClient ) )
		return NULL;
	if ( ! bmDrag.CreateCompatibleBitmap( &dcClient, rcAll.Width(), rcAll.Height() ) )
		return NULL;

	CBitmap *pOldAll = dcAll.SelectObject( &bmAll );

	dcAll.FillSolidRect( &rcClient, DRAG_COLOR_KEY );

	COLORREF crBack = pList->GetBkColor();
	pList->SetBkColor( DRAG_COLOR_KEY );
	pList->SendMessage( WM_PAINT, (WPARAM)dcAll.GetSafeHdc() );
	pList->SetBkColor( crBack );

	CBitmap *pOldDrag = dcDrag.SelectObject( &bmDrag );

	dcDrag.FillSolidRect( 0, 0, rcAll.Width(), rcAll.Height(), DRAG_COLOR_KEY );

	CRgn pRgn;

	ptMiddle.SetPoint( ptMouse.x - rcAll.left, ptMouse.y - rcAll.top );
	if ( bClipped )
	{
		pRgn.CreateEllipticRgn(	ptMiddle.x - MAX_DRAG_SIZE_2, ptMiddle.y - MAX_DRAG_SIZE_2,
								ptMiddle.x + MAX_DRAG_SIZE_2, ptMiddle.y + MAX_DRAG_SIZE_2 );
		dcDrag.SelectClipRgn( &pRgn );
	}

	for ( nIndex = -1 ; ( nIndex = pList->GetNextItem( nIndex, LVNI_SELECTED ) ) >= 0 ; )
	{
		pList->GetItemRect( nIndex, rcOne, LVIR_BOUNDS );

		if ( rcOne.IntersectRect( &rcAll, &rcOne ) )
		{
			dcDrag.BitBlt( rcOne.left - rcAll.left, rcOne.top - rcAll.top,
				rcOne.Width(), rcOne.Height(), &dcAll, rcOne.left, rcOne.top, SRCCOPY );
		}
	}

	dcDrag.SelectObject( pOldDrag );
	dcAll.SelectObject( pOldAll );

	dcDrag.DeleteDC();
	bmAll.DeleteObject();
	dcAll.DeleteDC();

	return (HBITMAP) bmDrag.Detach ();
}

CImageList* CLiveList::CreateDragImage(CListCtrl* pList, const CPoint& ptMouse)
{
	ASSERT_VALID( pList );

	CPoint ptOffset( 0, 0 );
	CBitmap bmDrag;
	bmDrag.Attach( CreateDragImage( pList, ptMouse, ptOffset) );
	BITMAP bmpInfo;
	bmDrag.GetBitmap( &bmpInfo );
	CImageList* pAll = new CImageList();
	pAll->Create( bmpInfo.bmWidth, bmpInfo.bmHeight, ILC_COLOR32|ILC_MASK, 1, 1 ) ||
	pAll->Create( bmpInfo.bmWidth, bmpInfo.bmHeight, ILC_COLOR24|ILC_MASK, 1, 1 ) ||
	pAll->Create( bmpInfo.bmWidth, bmpInfo.bmHeight, ILC_COLOR16|ILC_MASK, 1, 1 );
	pAll->Add( &bmDrag, DRAG_COLOR_KEY );
	bmDrag.DeleteObject();
	pAll->BeginDrag( 0, ptOffset );

	return pAll;
}


//////////////////////////////////////////////////////////////////////
// CLiveListCtrl

IMPLEMENT_DYNAMIC(CLiveListCtrl, CListCtrl)

CLiveListCtrl::CLiveListCtrl()
	: m_nColumns	( 0 )
{
}

CLiveListCtrl::~CLiveListCtrl()
{
	for ( CLiveMap::iterator i = m_pItems.begin() ; i != m_pItems.end() ; ++i )
	{
		CLiveItemPtr pItem = (*i).second;
		delete pItem;
	}
}

BEGIN_MESSAGE_MAP(CLiveListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFOW, OnLvnGetDispInfoW)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFOA, OnLvnGetDispInfoA)
	ON_NOTIFY_REFLECT(LVN_ODFINDITEMW, OnLvnOdFindItem)
	ON_NOTIFY_REFLECT(LVN_ODFINDITEMA, OnLvnOdFindItem)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnLvnOdCacheHint)
END_MESSAGE_MAP()

BOOL CLiveListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, int nColumns)
{
	m_nColumns = nColumns;

	return CListCtrl::Create( dwStyle | LVS_OWNERDATA, rect, pParentWnd, nID );
}

CLiveItemPtr CLiveListCtrl::Add(DWORD_PTR nParam)
{
	CLiveItemPtr pItem;
	CLiveMap::iterator i = m_pItems.find( nParam );
	if ( i == m_pItems.end() )
	{
		// Create new item
		pItem = new CLiveItem( m_nColumns, nParam );
		m_pItems.insert( CLiveMapPair( nParam, pItem ) );
	}
	else
	{
		// Update existing item
		pItem = (*i).second;
		pItem->m_bOld = false;
	}
	return pItem;
}

CLiveItemPtr CLiveListCtrl::Add(LPVOID pParam)
{
	return Add( (DWORD_PTR)pParam );
}

void CLiveListCtrl::Apply()
{
	// Remove old items
	for ( CLiveMap::iterator i = m_pItems.begin() ; i != m_pItems.end() ; )
	{
		CLiveItemPtr pItem = (*i).second;
		if ( pItem->m_bOld )
		{
			delete pItem;
			i = m_pItems.erase( i );
		}
		else
		{
			++i;
		}
	}

	// Recreate index
	m_pIndex.clear();
	m_pIndex.reserve( m_pItems.size() );
	for ( CLiveMap::iterator i = m_pItems.begin() ; i != m_pItems.end() ; ++i )
	{
		CLiveItemPtr pItem = (*i).second;
		pItem->m_bOld = true;

		m_pIndex.push_back( pItem );
	}

	// Tune virtual list
	SetItemCountEx( (int)m_pItems.size() );

	Sort();
}

void CLiveListCtrl::Sort(int nColumn)
{
	const int nOldColumn = (int)GetWindowLongPtr( GetSafeHwnd(), GWLP_USERDATA );

	if ( nColumn == -1 )
	{
		nColumn = nOldColumn;
	}
	else
	{
		nColumn++;

		if ( nColumn == abs( nOldColumn ) )
		{
			if ( nOldColumn > 0 )
				nColumn = 0 - nOldColumn;
			else
				nColumn = 0;
		}

		SetWindowLongPtr( GetSafeHwnd(), GWLP_USERDATA, nColumn );
	}

#ifdef IDB_SORT_ASC
	if ( ! CLiveList::m_bmSortAsc.m_hObject )
	{
		// Palette .bmp - 192,192,192 swapped to system color
		CLiveList::m_bmSortAsc.LoadMappedBitmap( IDB_SORT_ASC );
		CLiveList::m_bmSortDesc.LoadMappedBitmap( IDB_SORT_DESC );
	}

	CHeaderCtrl* pHeader = GetHeaderCtrl();
	for ( int nCol = 0 ; ; nCol++ )
	{
		HDITEM pColumn = {};
		pColumn.mask = HDI_BITMAP|HDI_FORMAT;

		if ( ! pHeader->GetItem( nCol, &pColumn ) ) break;

		HBITMAP hbm;
		int fmt;
		if ( nCol == abs( nColumn ) - 1 )
		{
			fmt = pColumn.fmt | HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
			hbm = (HBITMAP)( nColumn > 0 ? CLiveList::m_bmSortAsc.GetSafeHandle() : CLiveList::m_bmSortDesc.GetSafeHandle() );
		}
		else
		{
			fmt = pColumn.fmt & ~HDF_BITMAP;
			hbm = NULL;
		}
		if ( pColumn.fmt != fmt || pColumn.hbm != hbm )
		{
			pColumn.fmt = fmt;
			pColumn.hbm = hbm;
			VERIFY( pHeader->SetItem( nCol, &pColumn ) );
		}
	}
#endif // IDB_SORT_ASC/DESC

	if ( nColumn )
	{
		std::stable_sort( m_pIndex.begin(), m_pIndex.end(),
			boost::bind( CLiveList::Less, _1, _2, nColumn ) );
	}

	InvalidateRect( NULL );
}

void CLiveListCtrl::ClearSelection()
{
	int nCount = GetItemCount();
	for ( int i = 0 ; i < nCount ; ++i )
	{
		SetItemState( i, 0, LVIS_SELECTED );
	}
}

DWORD_PTR CLiveListCtrl::GetItemData(int nItem) const
{
	return ( nItem >= 0 && nItem < (int)m_pIndex.size() ) ?
		m_pIndex[ nItem ]->m_nParam : NULL;
}

UINT CLiveListCtrl::GetItemOverlayMask(int nItem) const
{
	return ( nItem >= 0 && nItem < (int)m_pIndex.size() ) ?
		m_pIndex[ nItem ]->m_nMaskOverlay : 0;
}

void CLiveListCtrl::OnLvnGetDispInfoW(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	OnLvnGetDispInfo( pNMHDR, TRUE );
}

void CLiveListCtrl::OnLvnGetDispInfoA(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	OnLvnGetDispInfo( pNMHDR, FALSE );
}

void CLiveListCtrl::OnLvnGetDispInfo(NMHDR *pNMHDR, BOOL bWide)
{
	LVITEM& pDispInfo = reinterpret_cast< NMLVDISPINFO* >( pNMHDR )->item;

	if ( pDispInfo.iItem < 0 || pDispInfo.iItem >= (int)m_pIndex.size() )
		return;

	const CLiveItemPtr pItem = m_pIndex[ pDispInfo.iItem ];

	if ( pDispInfo.mask & LVIF_TEXT )
	{
		if ( pDispInfo.iSubItem >= 0 && pDispInfo.iSubItem < pItem->m_pColumn.GetSize() )
		{
			CString strText = pItem->m_pColumn.GetAt( pDispInfo.iSubItem );
			if ( bWide )	// W
				wcsncpy_s( (LPWSTR)pDispInfo.pszText, pDispInfo.cchTextMax, strText, pDispInfo.cchTextMax - 1 );
			else			// A
				WideCharToMultiByte( CP_ACP, 0, strText, -1, (LPSTR)pDispInfo.pszText, pDispInfo.cchTextMax, NULL, NULL );
		}
	}

	if ( pDispInfo.mask & LVIF_IMAGE )
	{
		if ( pDispInfo.iSubItem >= 0 && pDispInfo.iSubItem < pItem->m_pColumn.GetSize() )
			pDispInfo.iImage = pItem->m_nImage[ pDispInfo.iSubItem ];
	}

	if ( pDispInfo.mask & LVIF_STATE )
	{
		pDispInfo.state = INDEXTOOVERLAYMASK( pItem->m_nMaskOverlay ) |
			INDEXTOSTATEIMAGEMASK( pItem->m_nMaskState );
	}

	if ( pDispInfo.mask & LVFI_PARAM )
	{
		pDispInfo.lParam = pItem->m_nParam;
	}
}

void CLiveListCtrl::OnLvnOdFindItem(NMHDR * /*pNMHDR*/, LRESULT *pResult)	// OnLvnOdFindItemW/OnLvnOdFindItemA
{
	//LPNMLVFINDITEM pFindInfo = reinterpret_cast<LPNMLVFINDITEM>(pNMHDR);
	//UNUSED_ALWAYS( pFindInfo );

	*pResult = 0;
}

void CLiveListCtrl::OnLvnOdCacheHint(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	//LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);
	//UNUSED_ALWAYS( pCacheHint );

	*pResult = 0;
}

// To implement list skinning elsewhere:
//void CLiveListCtrl::OnSkinChange()
//{
//	SetBkColor( Colors.m_crWindow );
//	SetBkImage( Skin.GetWatermark( _T("CLiveList") ) );
//}
