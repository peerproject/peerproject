//
// CtrlMediaList.cpp
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
#include "CtrlMediaList.h"
#include "WndMedia.h"
#include "Library.h"
#include "AlbumFolder.h"
#include "SharedFile.h"
#include "ShellIcons.h"
#include "Skin.h"
#include "Colors.h"
#include "Buffer.h"
#include "LiveList.h"
#include "DlgCollectionExport.h"
#include "DlgFilePropertiesSheet.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CMediaListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CMediaListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeyDown)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
	ON_COMMAND(ID_MEDIA_ADD, OnMediaAdd)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_RATE, OnUpdateMediaRate)
	ON_COMMAND(ID_MEDIA_RATE, OnMediaRate)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_PROPERTIES, OnUpdateMediaProperties)
	ON_COMMAND(ID_MEDIA_PROPERTIES, OnMediaProperties)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_REMOVE, OnUpdateMediaRemove)
	ON_COMMAND(ID_MEDIA_REMOVE, OnMediaRemove)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_CLEAR, OnUpdateMediaClear)
	ON_COMMAND(ID_MEDIA_CLEAR, OnMediaClear)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_SELECT, OnUpdateMediaSelect)
	ON_COMMAND(ID_MEDIA_SELECT, OnMediaSelect)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_SAVE, OnUpdateMediaSave)
	ON_COMMAND(ID_MEDIA_SAVE, OnMediaSave)
	ON_COMMAND(ID_MEDIA_OPEN, OnMediaOpen)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_PREVIOUS, OnUpdateMediaPrevious)
	ON_COMMAND(ID_MEDIA_PREVIOUS, OnMediaPrevious)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_NEXT, OnUpdateMediaNext)
	ON_COMMAND(ID_MEDIA_NEXT, OnMediaNext)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_REPEAT, OnUpdateMediaRepeat)
	ON_COMMAND(ID_MEDIA_REPEAT, OnMediaRepeat)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_RANDOM, OnUpdateMediaRandom)
	ON_COMMAND(ID_MEDIA_RANDOM, OnMediaRandom)
	ON_COMMAND(ID_MEDIA_ADD_FOLDER, OnMediaAddFolder)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_EXPORT_COLLECTION, OnUpdateMediaCollection)
	ON_COMMAND(ID_MEDIA_EXPORT_COLLECTION, OnMediaCollection)
END_MESSAGE_MAP()

#define STATE_CURRENT	(1<<12)
#define STATE_PLAYED	(1<<13)


/////////////////////////////////////////////////////////////////////////////
// CMediaListCtrl construction

CMediaListCtrl::CMediaListCtrl()
	: m_pDragImage		( NULL )
	, m_bCreateDragImage( FALSE )
//	, m_nSelectedCount	( 0 )	// Using static
//	, m_tLastUpdate		( 0 )	// Using static
{
}

CMediaListCtrl::~CMediaListCtrl()
{
}

UINT CMediaListCtrl::GetSelectedCount()
{
	static UINT nSelectedCount = 0;
	static DWORD tLastUpdate = 0;
	const DWORD tNow = GetTickCount();
	if ( tNow > tLastUpdate + Settings.Interface.RefreshRateUI || tNow < tLastUpdate )
	{
		nSelectedCount = CListCtrl::GetSelectedCount();
		tLastUpdate = tNow;
	}
	return nSelectedCount;
}

/////////////////////////////////////////////////////////////////////////////
// CMediaListCtrl operations

BOOL CMediaListCtrl::Create(CWnd* pParentWnd, UINT nID)
{
	CRect rect;

	return CListCtrl::Create(
		WS_CHILD|WS_VSCROLL|LVS_ICON|LVS_REPORT|LVS_SHAREIMAGELISTS|LVS_NOCOLUMNHEADER,
		rect, pParentWnd, nID );
}

BOOL CMediaListCtrl::Open(LPCTSTR pszFile)
{
	Clear();

	Enqueue( pszFile, FALSE );

	if ( GetItemCount() == 0 ) return FALSE;

	GetNext();

	return TRUE;
}

BOOL CMediaListCtrl::Enqueue(LPCTSTR pszFile, BOOL bStart)
{
	if ( _tcsistr( pszFile, _T(".m3u") ) != NULL ||
		 _tcsistr( pszFile, _T(".pls") ) != NULL )
	{
		LoadTextList( pszFile );
	}
	else
	{
		Add( pszFile );
	}

	if ( bStart && GetItemCount() == 1 )
		GetNext();

	return TRUE;
}

int CMediaListCtrl::RecursiveEnqueue(LPCTSTR pszPath)
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	HANDLE hSearch;
	int nCount = 0;

	strPath.Format( _T("%s\\*.*"), pszPath );

	hSearch = FindFirstFile( strPath, &pFind );

	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( pFind.cFileName[0] == '.' ) continue;
			if ( _tcsicmp( pFind.cFileName, _T("Metadata") ) == 0 ) continue;

			strPath.Format( _T("%s\\%s"), pszPath, pFind.cFileName );

			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				nCount += RecursiveEnqueue( strPath );
			else
				nCount += Enqueue( strPath, FALSE );
		}
		while ( FindNextFile( hSearch, &pFind ) );

		FindClose( hSearch );
	}

	return nCount;
}

void CMediaListCtrl::Remove(LPCTSTR pszFile)
{
	for ( int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		if ( GetItemText( nItem, 1 ).CompareNoCase( pszFile ) == 0 )
			Remove( nItem );
	}
}

BOOL CMediaListCtrl::LoadTextList(LPCTSTR pszFile)
{
	CString strPath = pszFile;
	strPath = strPath.Left( strPath.ReverseFind( '\\' ) + 1 );

	CFile pFile;
	if ( ! pFile.Open( pszFile, CFile::modeRead ) ) return FALSE;

	CBuffer pBuffer;
	pBuffer.EnsureBuffer( (DWORD)pFile.GetLength() );
	pBuffer.m_nLength = (DWORD)pFile.Read( pBuffer.m_pBuffer, (DWORD)pFile.GetLength() );

	CString strItem;
	while ( pBuffer.ReadLine( strItem ) )
	{
		strItem.Trim();

		if ( strItem.GetLength() && strItem.GetAt( 0 ) != '#' )
		{
			if ( strItem.Find( '\\' ) != 0 && strItem.Find( ':' ) != 1 )
				strItem = strPath + strItem;	// Relative path

			if ( GetFileAttributes( strItem ) != 0xFFFFFFFF )
				Enqueue( strItem, FALSE );
		}
	}

	return TRUE;
}

BOOL CMediaListCtrl::SaveTextList(LPCTSTR pszFile)
{
	CString strPath = pszFile;
	strPath = strPath.Left( strPath.ReverseFind( '\\' ) + 1 );

	CString strFile;
	for ( int nItem = 0 ; nItem < GetItemCount() ; nItem++ )
	{
		CString strItem = GetItemText( nItem, 1 );

		if ( _tcsnicmp( strPath, strItem, strPath.GetLength() ) == 0 )
			strItem = strItem.Mid( strPath.GetLength() );	// Relative path

		strFile += strItem + _T("\r\n");
	}

	CFile pFile;
	if ( ! pFile.Open( pszFile, CFile::modeWrite | CFile::modeCreate ) )
		return FALSE;

	CT2CA strFileA( (LPCTSTR)strFile );

	pFile.Write( strFileA, static_cast< UINT >( strlen( strFileA ) ) );

	return TRUE;
}

int CMediaListCtrl::Add(LPCTSTR pszPath, int nItem)
{
	LPCTSTR pszFile = _tcsrchr( pszPath, '\\' );
	pszFile = pszFile ? pszFile + 1 : pszPath;

	DWORD nFile = 0;
	{
		CQuickLock oLock( Library.m_pSection );
		if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( pszPath ) )
			nFile = pFile->m_nIndex;
	}

	CString strTemp = (LPTSTR)pszFile;
	int nDotPos = strTemp.ReverseFind( '.' );
	if ( nDotPos != -1 ) strTemp = strTemp.Left( nDotPos );
	LPTSTR pszFileTmp = strTemp.GetBuffer( strTemp.GetLength() );

	LV_ITEM pItem = {};
	pItem.mask		= LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	pItem.iItem		= nItem >= 0 ? nItem : GetItemCount();
	pItem.iImage	= ShellIcons.Get( pszFile, 16 );
	pItem.lParam	= nFile;
	pItem.pszText	= pszFileTmp;
	pItem.iItem		= InsertItem( &pItem );
	pItem.iSubItem	= 1;
	pItem.mask		= LVIF_TEXT;
	pItem.pszText	= (LPTSTR)pszPath;
	SetItem( &pItem );
	strTemp.ReleaseBuffer();
	return pItem.iItem;
}

void CMediaListCtrl::Remove(int nItem)
{
	if ( nItem < 0 || nItem >= GetItemCount() ) return;

	BOOL bActive = GetItemState( nItem, STATE_CURRENT );
	DeleteItem( nItem );

	if ( bActive ) SetCurrent( nItem );
}

int CMediaListCtrl::GetCount()
{
	return GetItemCount();
}

void CMediaListCtrl::Clear()
{
	DeleteAllItems();

	SetCurrent( -1 );
}

int CMediaListCtrl::GetCurrent()
{
	for ( int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		if ( GetItemState( nItem, STATE_CURRENT ) ) return nItem;
	}

	return -1;
}

void CMediaListCtrl::SetCurrent(int nCurrent)
{
	for ( int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		if ( GetItemState( nItem, STATE_CURRENT ) )
		{
			if ( nItem != nCurrent )
				SetItemState( nItem, 0, STATE_CURRENT );
		}
		else
		{
			if ( nItem == nCurrent )
				SetItemState( nItem, STATE_CURRENT, STATE_CURRENT );
		}
	}

	NMHDR pNM = { GetSafeHwnd(), GetDlgCtrlID(), MLN_NEWCURRENT };
	CString strPath = GetPath( nCurrent );
	GetSafeOwner()->SendMessage( WM_NOTIFY, pNM.idFrom, (LPARAM)&pNM );
}

int CMediaListCtrl::GetNext(BOOL bSet)
{
	int nItem = GetCurrent();

	if ( nItem >= 0 ) SetItemState( nItem, STATE_PLAYED, STATE_PLAYED );

	if ( Settings.MediaPlayer.Random )
	{
		int nCount = 0;

		for ( nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
		{
			if ( GetItemState( nItem, STATE_PLAYED ) == 0 ) nCount++;
		}

		if ( nCount > 0 )
		{
			nCount = GetRandomNum( 0, nCount - 1 );

			for ( nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
			{
				if ( GetItemState( nItem, STATE_PLAYED ) == 0 )
				{
					if ( nCount-- == 0 ) break;
				}
			}
		}
	}
	else if ( ++nItem >= GetItemCount() )
	{
		nItem = -1;
	}

	if ( bSet )
		SetCurrent( nItem );

	return nItem;
}

void CMediaListCtrl::Reset(BOOL bNext)
{
	for ( int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
		SetItemState( nItem, 0, STATE_PLAYED );
	if ( bNext ) SetCurrent( -1 );
}

CString CMediaListCtrl::GetPath(int nItem)
{
	if ( nItem < 0 || nItem >= GetItemCount() )
		return CString();

	return GetItemText( nItem, 1 );
}

/////////////////////////////////////////////////////////////////////////////
// CMediaListCtrl message handlers

int CMediaListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CListCtrl::OnCreate( lpCreateStruct ) == -1 ) return -1;

	ShellIcons.AttachTo( this, 16 );	// SetImageList()
	InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 100, -1 );
	InsertColumn( 1, _T("Path"), LVCFMT_LEFT, 0, 0 );

	SetExtendedStyle( LVS_EX_DOUBLEBUFFER|LVS_EX_LABELTIP|LVS_EX_FULLROWSELECT );

	m_wndTip.Create( this, &Settings.Interface.TipMedia );

	return 0;
}

void CMediaListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize( nType, cx, cy );

	CRect rc;
	GetWindowRect( &rc );

	LV_COLUMN pColumn;
	pColumn.mask	= LVCF_WIDTH;
	pColumn.cx		= rc.Width() - GetSystemMetrics( SM_CXVSCROLL );
	SetColumn( 0, &pColumn );
}

void CMediaListCtrl::OnCustomDraw(NMHDR* pNotify, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNotify;
//	int nItem = (int)pDraw->nmcd.dwItemSpec;

	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		if ( GetItemState( static_cast< int >( pDraw->nmcd.dwItemSpec ), LVIS_SELECTED ) == 0 &&
			 GetItemState( static_cast< int >( pDraw->nmcd.dwItemSpec ), STATE_CURRENT ) != 0 )
		{
			pDraw->clrText		= Colors.m_crMediaPanelActiveText;
			pDraw->clrTextBk	= Colors.m_crMediaPanelActiveBack;
		}
		else
		{
			pDraw->clrText		= Colors.m_crMediaPanelText;
			pDraw->clrTextBk	= Colors.m_crMediaPanelBack;
		}

		if ( m_bCreateDragImage )
			pDraw->clrTextBk = DRAG_COLOR_KEY;

		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

void CMediaListCtrl::OnDoubleClick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	OnMediaSelect();
	*pResult = 0;
}

void CMediaListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if ( point.x == -1 && point.y == -1 )	// Keyboard fix
		ClientToScreen( &point );

	Skin.TrackPopupMenu( _T("CMediaList"), point, ID_MEDIA_SELECT );
}

void CMediaListCtrl::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	if ( pLVKeyDow->wVKey == VK_RETURN )
		PostMessage( WM_COMMAND, ID_MEDIA_SELECT );
	else if ( pLVKeyDow->wVKey == VK_DELETE )
		PostMessage( WM_COMMAND, ID_MEDIA_REMOVE );

	m_wndTip.Hide();
	*pResult = 0;
}

void CMediaListCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	m_wndTip.Hide();

	CPoint ptAction( pNMListView->ptAction );

	m_bCreateDragImage = TRUE;
	m_pDragImage = CLiveList::CreateDragImage( this, ptAction );
	m_bCreateDragImage = FALSE;

	if ( m_pDragImage == NULL ) return;
	m_nDragDrop = -1;

	UpdateWindow();

	CRect rcClient;
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	ClipCursor( &rcClient );
	SetCapture();

	SetFocus();
	UpdateWindow();

	m_pDragImage->DragEnter( this, ptAction );
}

void CMediaListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_wndTip.Hide();
	CListCtrl::OnLButtonDown( nFlags, point );
}

void CMediaListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_wndTip.Hide();
	CListCtrl::OnRButtonDown( nFlags, point );
}

void CMediaListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	int nHit = HitTest( point );

	if ( m_pDragImage != NULL )
	{
		m_pDragImage->DragMove( point );

		if ( nHit != m_nDragDrop )
		{
			CImageList::DragShowNolock( FALSE );
			if ( m_nDragDrop >= 0 )
				SetItemState( m_nDragDrop, 0, LVIS_DROPHILITED );
			m_nDragDrop = nHit;
			if ( m_nDragDrop >= 0 )
				SetItemState( m_nDragDrop, LVIS_DROPHILITED, LVIS_DROPHILITED );
			UpdateWindow();
			CImageList::DragShowNolock( TRUE );
		}
	}
	else
	{
		DWORD_PTR nFile = nHit >= 0 ? GetItemData( nHit ) : 0;

		if ( nFile > 0 && ! Library.LookupFile( nFile ) )
			nFile = 0;

		if ( nFile > 0 )
			m_wndTip.Show( nFile );
		else
			m_wndTip.Hide();
	}

	CListCtrl::OnMouseMove( nFlags, point );
}

void CMediaListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_pDragImage == NULL )
	{
		CListCtrl::OnLButtonUp( nFlags, point );
		return;
	}

	ClipCursor( NULL );
	ReleaseCapture();

	m_pDragImage->DragLeave( this );
	m_pDragImage->EndDrag();
	delete m_pDragImage;
	m_pDragImage = NULL;

	if ( m_nDragDrop >= 0 )
		SetItemState( m_nDragDrop, 0, LVIS_DROPHILITED );
	else
		m_nDragDrop = GetItemCount();

	CArray< CString > pPaths;
	CArray< UINT > pStates;

	for ( ;; )
	{
		int nItem = GetNextItem( -1, LVNI_SELECTED );
		if ( nItem < 0 ) break;

		pPaths.Add( GetItemText( nItem, 1 ) );
		pStates.Add( GetItemState( nItem, 0xFFFFFFFF ) );

		DeleteItem( nItem );

		if ( m_nDragDrop > nItem ) m_nDragDrop--;
	}

	for ( int nItem = 0 ; nItem < pPaths.GetSize() ; nItem++ )
	{
		int nIndex = Add( pPaths.GetAt( nItem ), m_nDragDrop++ );
		SetItemState( nIndex, pStates.GetAt( nItem ), 0xFFFFFFFF );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMediaListCtrl command handlers

BOOL CMediaListCtrl::AreSelectedFilesInLibrary()
{
	if ( GetSelectedCount() )
	{
		// If at least one selected file is in the library then enable
		for ( int nItem = -1 ; ( nItem = GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
		{
			CString strPath = GetPath( nItem );

			CQuickLock oLock( Library.m_pSection );
			if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( strPath ) )
				return TRUE;
		}
	}
	return FALSE;
}

void CMediaListCtrl::ShowFilePropertiesDlg( int nPage )
{
	CFilePropertiesSheet dlg;

	for ( int nItem = -1 ; ( nItem = GetNextItem( nItem, LVIS_SELECTED ) ) >= 0 ; )
	{
		CString strPath = GetPath( nItem );

		CQuickLock oLock( Library.m_pSection );
		if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( strPath ) )
			dlg.Add( pFile );
	}

	dlg.DoModal( nPage );
}

void CMediaListCtrl::OnUpdateMediaRate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( AreSelectedFilesInLibrary() );
}

void CMediaListCtrl::OnMediaRate()
{
	ShowFilePropertiesDlg( 2 );
}

void CMediaListCtrl::OnUpdateMediaProperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( AreSelectedFilesInLibrary() );
}

void CMediaListCtrl::OnMediaProperties()
{
	ShowFilePropertiesDlg();
}

void CMediaListCtrl::OnUpdateMediaSelect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetSelectedCount() == 1 );
}

void CMediaListCtrl::OnMediaSelect()
{
	int nItem = GetNextItem( -1, LVNI_SELECTED );

	if ( nItem >= 0 )
	{
		SetCurrent( nItem );
		SetFocus();
		Invalidate();
	}
}

void CMediaListCtrl::OnMediaAdd()
{
	if ( ! AfxGetMainWnd()->IsWindowEnabled() ) return;

	CFileDialog dlg( TRUE, NULL, NULL,
		OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING,
		LoadString( IDS_MEDIA_FILTER ), this );

	const int nLimit = 81920;

	auto_array< TCHAR > szFiles( new TCHAR[ nLimit ] );
	ZeroMemory( szFiles.get(), nLimit * sizeof( TCHAR ) );
	dlg.m_ofn.lpstrFile	= szFiles.get();
	dlg.m_ofn.nMaxFile	= nLimit;

	if ( dlg.DoModal() != IDOK ) return;

	CString strFolder( szFiles.get() );
	LPCTSTR pszFile = szFiles.get() + strFolder.GetLength() + 1;

	const BOOL bWasEmpty = ( GetItemCount() == 0 );

	if ( *pszFile )
	{
		for ( strFolder += '\\' ; *pszFile ; )
		{
			Enqueue( strFolder + pszFile, FALSE );
			pszFile += _tcslen( pszFile ) + 1;
		}
	}
	else
	{
		Enqueue( strFolder, FALSE );
	}

	if ( bWasEmpty && GetItemCount() > 0 )
		GetNext();
}

void CMediaListCtrl::OnMediaAddFolder()
{
	if ( ! AfxGetMainWnd()->IsWindowEnabled() ) return;

	CString strPath( BrowseForFolder( LoadString( ID_MEDIA_ADD_FOLDER ) ) );	// "Select folder to add to playlist:"
	if ( strPath.IsEmpty() )
		return;

	BOOL bWasEmpty = ( GetItemCount() == 0 );

	RecursiveEnqueue( strPath );

	if ( bWasEmpty && GetItemCount() > 0 ) GetNext();
}

void CMediaListCtrl::OnUpdateMediaRemove(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetSelectedCount() );
}

void CMediaListCtrl::OnMediaRemove()
{
	for ( int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		if ( GetItemState( nItem, LVIS_SELECTED ) )
			Remove( nItem );
	}
}

void CMediaListCtrl::OnUpdateMediaClear(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetItemCount() > 0 );
}

void CMediaListCtrl::OnMediaClear()
{
	Clear();
}

void CMediaListCtrl::OnMediaOpen()
{
	CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING, LoadString( IDS_MEDIA_FILTER ), this );

	if ( dlg.DoModal() != IDOK ) return;

	Open( dlg.GetPathName() );
}

void CMediaListCtrl::OnUpdateMediaSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetItemCount() > 0 );
}

void CMediaListCtrl::OnMediaSave()
{
	CFileDialog dlg( FALSE, _T("m3u"), NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING,
		_T("Media Playlists|*.m3u|") + LoadString( IDS_FILES_ALL ) + _T("|*.*||"), this );

	if ( dlg.DoModal() != IDOK ) return;

	SaveTextList( dlg.GetPathName() );
}

void CMediaListCtrl::OnUpdateMediaPrevious(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetCurrent() > 0 );
}

void CMediaListCtrl::OnMediaPrevious()
{
	int nCurrent = GetCurrent();
	if ( nCurrent > 0 ) SetCurrent( nCurrent - 1 );
}

void CMediaListCtrl::OnUpdateMediaNext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetItemCount() > 1 );
}

void CMediaListCtrl::OnMediaNext()
{
	GetNext();
}

void CMediaListCtrl::OnUpdateMediaRepeat(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( Settings.MediaPlayer.Repeat );
}

void CMediaListCtrl::OnMediaRepeat()
{
	Settings.MediaPlayer.Repeat = ! Settings.MediaPlayer.Repeat;
}

void CMediaListCtrl::OnUpdateMediaRandom(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( Settings.MediaPlayer.Random );
}

void CMediaListCtrl::OnMediaRandom()
{
	Settings.MediaPlayer.Random = ! Settings.MediaPlayer.Random;
}

void CMediaListCtrl::OnUpdateMediaCollection(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetItemCount() > 0 && GetItemCount() <= 200 );
}

void CMediaListCtrl::OnMediaCollection()
{
	// The album title name is a collection folder name
	// Leave it empty to have the collection mounted under collections folder

	//CAlbumFolder* pCollection = new CAlbumFolder( NULL, NULL, _T(""), TRUE );		// Obsolete
	CAutoPtr< CAlbumFolder > pCollection( new CAlbumFolder( NULL, NULL, _T(""), TRUE ) );
	if ( pCollection )
	{
		for ( int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
		{
			CString strPath = GetPath( nItem );

			CQuickLock oLock( Library.m_pSection );
			if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( strPath, FALSE, TRUE ) )
				pCollection->AddFile( pFile );
		}
	}

	CCollectionExportDlg dlg( pCollection );
	dlg.DoModal();
//	delete pCollection;		// Obsolete
}

void CMediaListCtrl::OnSkinChange()
{
	SetBkColor( Colors.m_crMediaPanelBack );
	SetBkImage( Skin.GetWatermark( _T("CMediaList") ) );

	// Update Dropshadow
	m_wndTip.DestroyWindow();
	m_wndTip.Create( this, &Settings.Interface.TipMedia );
}
