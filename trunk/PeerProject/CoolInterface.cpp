//
// CoolInterface.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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
#include "CoolInterface.h"
#include "Colors.h"
#include "ShellIcons.h"
#include "Flags.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

CCoolInterface CoolInterface;


//////////////////////////////////////////////////////////////////////
// CCoolInterface construction

CCoolInterface::CCoolInterface()
{
	m_czBuffer = CSize( 0, 0 );

	// Experimental values
	m_pNameMap.InitHashTable( 509 );
	m_pImageMap16.InitHashTable( 347 );
	m_pImageMap32.InitHashTable( 61 );
	m_pImageMap48.InitHashTable( 61 );
	m_pWindowIcons.InitHashTable( 61 );
}

CCoolInterface::~CCoolInterface()
{
	Clear();

	HICON hIcon;
	HWND hWnd;
	for ( POSITION pos = m_pWindowIcons.GetStartPosition() ; pos ; )
	{
		m_pWindowIcons.GetNextAssoc( pos, hIcon, hWnd );
		VERIFY( DestroyIcon( hIcon ) );
	}
	m_pWindowIcons.RemoveAll();
}

void CCoolInterface::Load()
{
	//CQuickLock oLock( m_pSection );

	CreateFonts();
}

//////////////////////////////////////////////////////////////////////
// CCoolInterface clear

void CCoolInterface::Clear()
{
	//CQuickLock oLock( m_pSection );

	m_pNameMap.RemoveAll();

	m_pImageMap16.RemoveAll();
	if ( m_pImages16.m_hImageList )
		m_pImages16.DeleteImageList();

	m_pImageMap32.RemoveAll();
	if ( m_pImages32.m_hImageList )
		m_pImages32.DeleteImageList();

	m_pImageMap48.RemoveAll();
	if ( m_pImages48.m_hImageList )
		m_pImages48.DeleteImageList();

	if ( m_bmBuffer.m_hObject )
	{
		m_dcBuffer.SelectObject( CGdiObject::FromHandle( m_bmOldBuffer ) );
		m_dcBuffer.DeleteDC();
		m_bmBuffer.DeleteObject();
	}

	m_czBuffer = CSize( 0, 0 );
}

//////////////////////////////////////////////////////////////////////
// CCoolInterface command management

void CCoolInterface::NameCommand(UINT nID, LPCTSTR pszName)
{
	//CQuickLock oLock( m_pSection );

	m_pNameMap.SetAt( pszName, nID );
}

UINT CCoolInterface::NameToID(LPCTSTR pszName) const
{
	//CQuickLock oLock( m_pSection );

	UINT nID = 0;
	if ( m_pNameMap.Lookup( pszName, nID ) ) return nID;
	return _tcstoul( pszName, NULL, 10 );
}

//////////////////////////////////////////////////////////////////////
// CCoolInterface image management

int CCoolInterface::ImageForID(UINT nID, int nImageListType) const
{
	//CQuickLock oLock( m_pSection );

	int nImage = -1;
	switch ( nImageListType )
	{
	case LVSIL_SMALL:
		return m_pImageMap16.Lookup( nID, nImage ) ? nImage : -1;
	case LVSIL_NORMAL:
		return m_pImageMap32.Lookup( nID, nImage ) ? nImage : -1;
	case LVSIL_BIG:
		return m_pImageMap48.Lookup( nID, nImage ) ? nImage : -1;
	}
	return -1;
}

void CCoolInterface::AddIcon(UINT nID, HICON hIcon, int nImageListType)
{
	//CQuickLock oLock( m_pSection );

	VERIFY( ConfirmImageList() );

	switch ( nImageListType )
	{
	case LVSIL_SMALL:
		m_pImageMap16.SetAt( nID, m_pImages16.Add( hIcon ) );
		break;
	case LVSIL_NORMAL:
		m_pImageMap32.SetAt( nID, m_pImages32.Add( hIcon ) );
		break;
	case LVSIL_BIG:
		m_pImageMap48.SetAt( nID, m_pImages48.Add( hIcon ) );
		break;
	}
}

void CCoolInterface::CopyIcon(UINT nFromID, UINT nToID, int nImageListType)
{
	//CQuickLock oLock( m_pSection );

	int nImage;
	switch ( nImageListType )
	{
	case LVSIL_SMALL:
		if ( m_pImageMap16.Lookup( nFromID, nImage ) )
			m_pImageMap16.SetAt( nToID, nImage );
		break;
	case LVSIL_NORMAL:
		if ( m_pImageMap32.Lookup( nFromID, nImage ) )
			m_pImageMap32.SetAt( nToID, nImage );
		break;
	case LVSIL_BIG:
		if ( m_pImageMap48.Lookup( nFromID, nImage ) )
			m_pImageMap48.SetAt( nToID, nImage );
		break;
	}
}

HICON CCoolInterface::ExtractIcon(UINT nID, BOOL bMirrored, int nImageListType)
{
	//CQuickLock oLock( m_pSection );

	HICON hIcon = NULL;
	int nImage = ImageForID( nID, nImageListType );
	if ( nImage >= 0 )
	{
		switch ( nImageListType )
		{
		case LVSIL_SMALL:
			hIcon = m_pImages16.ExtractIcon( nImage );
			break;
		case LVSIL_NORMAL:
			hIcon = m_pImages32.ExtractIcon( nImage );
			break;
		case LVSIL_BIG:
			hIcon = m_pImages48.ExtractIcon( nImage );
			break;
		}
	}
	if ( ! hIcon )
	{
		int cx = 0;
		switch ( nImageListType )
		{
		case LVSIL_SMALL:
			cx = 16;
			break;
		case LVSIL_NORMAL:
			cx = 32;
			break;
		case LVSIL_BIG:
			cx = 48;
			break;
		}
		hIcon = (HICON)LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE( nID ), IMAGE_ICON, cx, cx, 0 );
		if ( hIcon )
			AddIcon( nID, hIcon, nImageListType );
#ifdef _DEBUG
		else
			theApp.Message( MSG_DEBUG, _T("Failed to load icon %d (%dx%d)."), nID, cx, cx );
#endif // _DEBUG
	}
	if ( hIcon && bMirrored && nID != ID_HELP_ABOUT )
	{
		hIcon = CreateMirroredIcon( hIcon );
		ASSERT( hIcon != NULL );
	}
	return hIcon;
}

int CCoolInterface::ExtractIconID(UINT nID, BOOL bMirrored, int nImageListType)
{
	//CQuickLock oLock( m_pSection );

	DestroyIcon( ExtractIcon( nID, bMirrored, nImageListType ) );
	return ImageForID( nID, nImageListType );
}

void CCoolInterface::SetIcon(UINT nID, BOOL bMirrored, BOOL bBigIcon, CWnd* pWnd)
{
	//CQuickLock oLock( m_pSection );

	HICON hIcon = ExtractIcon( nID, bMirrored, bBigIcon ? LVSIL_NORMAL : LVSIL_SMALL );
	if ( hIcon )
	{
		m_pWindowIcons.SetAt( hIcon, pWnd->GetSafeHwnd() );
		hIcon = pWnd->SetIcon( hIcon, bBigIcon );
		if ( hIcon )
		{
			VERIFY( m_pWindowIcons.RemoveKey( hIcon ) );
			VERIFY( DestroyIcon( hIcon ) );
		}
	}
}

void CCoolInterface::SetIcon(HICON hIcon, BOOL bMirrored, BOOL bBigIcon, CWnd* pWnd)
{
	//CQuickLock oLock( m_pSection );

	if ( hIcon )
	{
		if ( bMirrored ) hIcon = CreateMirroredIcon( hIcon );
		m_pWindowIcons.SetAt( hIcon, pWnd->GetSafeHwnd() );
		hIcon = pWnd->SetIcon( hIcon, bBigIcon );
		if ( hIcon )
		{
			VERIFY( m_pWindowIcons.RemoveKey( hIcon ) );
			VERIFY( DestroyIcon( hIcon ) );
		}
	}
}

//BOOL CCoolInterface::AddImagesFromToolbar(UINT nIDToolBar, COLORREF crBack)
//{
//	VERIFY( ConfirmImageList() );
//	CBitmap pBmp;
//	if ( ! pBmp.LoadBitmap( nIDToolBar ) ) return FALSE;
//	int nBase = m_pImages.Add( &pBmp, crBack );
//	pBmp.DeleteObject();
//	if ( nBase < 0 ) return FALSE;
//
//	BOOL bRet = FALSE;
//	if ( HRSRC hRsrc = FindResource( AfxGetResourceHandle(), MAKEINTRESOURCE(nIDToolBar), RT_TOOLBAR ) )
//	{
//		if ( HGLOBAL hGlobal = LoadResource( AfxGetResourceHandle(), hRsrc ) )
//		{
//			if ( TOOLBAR_RES* pData = (TOOLBAR_RES*)LockResource( hGlobal ) )
//			{
//				for ( WORD nItem = 0 ; nItem < pData->wItemCount ; nItem++ )
//				{
//					if ( pData->items()[ nItem ] != ID_SEPARATOR )
//					{
//						m_pImageMap.SetAt( pData->items()[ nItem ], nBase );
//						nBase++;
//					}
//				}
//				bRet = TRUE;
//			}
//			FreeResource( hGlobal );
//		}
//	}
//	return bRet;
//}

BOOL CCoolInterface::ConfirmImageList()
{
	return
		( m_pImages16.GetSafeHandle() ||
		  m_pImages16.Create( 16, 16, ILC_COLOR32|ILC_MASK, 16, 4 ) ||
		  m_pImages16.Create( 16, 16, ILC_COLOR24|ILC_MASK, 16, 4 ) ||
		  m_pImages16.Create( 16, 16, ILC_COLOR16|ILC_MASK, 16, 4 ) ) &&
		( m_pImages32.GetSafeHandle() ||
		  m_pImages32.Create( 32, 32, ILC_COLOR32|ILC_MASK, 16, 4 ) ||
		  m_pImages32.Create( 32, 32, ILC_COLOR24|ILC_MASK, 16, 4 ) ||
		  m_pImages32.Create( 32, 32, ILC_COLOR16|ILC_MASK, 16, 4 ) ) &&
		( m_pImages48.GetSafeHandle() ||
		  m_pImages48.Create( 48, 48, ILC_COLOR32|ILC_MASK, 16, 4 ) ||
		  m_pImages48.Create( 48, 48, ILC_COLOR24|ILC_MASK, 16, 4 ) ||
		  m_pImages48.Create( 48, 48, ILC_COLOR16|ILC_MASK, 16, 4 ) );
}

void CCoolInterface::LoadIconsTo(CImageList& pImageList, const UINT nID[], BOOL bMirror, int nImageListType)
{
	int nCount = 0;
	for ( ; nID[ nCount ] ; ++nCount );
	ASSERT( nCount != 0 );

	int nSize = 16;		// LVSIL_SMALL
	if ( nImageListType == LVSIL_NORMAL )
		nSize = 32;
	else if ( nImageListType == LVSIL_BIG )
		nSize = 48;

	if ( pImageList.GetSafeHandle() )
		VERIFY( pImageList.DeleteImageList() );

	VERIFY( pImageList.Create( nSize, nSize, ILC_COLOR32|ILC_MASK, nCount, 0 ) ||
		pImageList.Create( nSize, nSize, ILC_COLOR24|ILC_MASK, nCount, 0 ) ||
		pImageList.Create( nSize, nSize, ILC_COLOR16|ILC_MASK, nCount, 0 ) );

	for ( int i = 0 ; nID[ i ] ; ++i )
	{
		if ( HICON hIcon = CoolInterface.ExtractIcon( nID[ i ], bMirror, nImageListType ) )
		{
			VERIFY( pImageList.Add( hIcon ) != -1 );
			VERIFY( DestroyIcon( hIcon ) );
		}
		//else	// Fails at startup
		//	ASSERT( hIcon != NULL );
	}
}

void CCoolInterface::LoadFlagsTo(CImageList& pImageList)
{
	const int nImages = pImageList.GetImageCount();
	const int nFlags  = Flags.GetCount();
	VERIFY( pImageList.SetImageCount( nImages + nFlags ) );
	for ( int nFlag = 0 ; nFlag < nFlags ; nFlag++ )
	{
		if ( HICON hIcon = Flags.ExtractIcon( nFlag ) )
		{
			VERIFY( pImageList.Replace( nImages + nFlag, hIcon ) != -1 );
			VERIFY( DestroyIcon( hIcon ) );
		}
	}
}

// No CCoolInterface::LoadProtocolIconsTo(), Use LoadIconsTo( pImageList, protocolIDs );

// Obsolete alpha-command-strip loading for reference elsewhere:
//	CBitmap bmImages;
//	CImageFile pFile;
//	pFile.LoadFromResource( AfxGetResourceHandle(), IDB_PROTOCOLS, RT_PNG );
//	HBITMAP hBitmap = pFile.CreateBitmap();
//	bmImages.Attach( hBitmap );
//
//	m_gdiImageList.Create( 16, 16, ILC_COLOR32|ILC_MASK, 7, 1 ) ||
//	m_gdiImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 7, 1 ) ||
//	m_gdiImageList.Create( 16, 16, ILC_COLOR16|ILC_MASK, 7, 1 );
//	m_gdiImageList.Add( &bmImages, CLR_NONE );
//	bmImages.DeleteObject();
//
//	// Replace with the skin images (if fails old images remain)
//	for ( int nImage = 1 ; nImage < PROTOCOL_LAST ; nImage++ )
//	{
//		if ( HICON hIcon = CoolInterface.ExtractIcon( (UINT)protocolCmdMap[ nImage ].commandID, FALSE ) )
//		{
//			m_gdiProtocols.Replace( nImage, hIcon );
//			DestroyIcon( hIcon );
//		}
//	}


//////////////////////////////////////////////////////////////////////
// CCoolInterface double buffer

CDC* CCoolInterface::GetBuffer(CDC& dcScreen, const CSize& szItem)
{
	//CQuickLock oLock( m_pSection );

	if ( szItem.cx <= m_czBuffer.cx && szItem.cy <= m_czBuffer.cy )
	{
		m_dcBuffer.SelectClipRgn( NULL );
		return &m_dcBuffer;
	}

	if ( m_bmBuffer.m_hObject )
	{
		m_dcBuffer.SelectObject( CGdiObject::FromHandle( m_bmOldBuffer ) );
		m_bmBuffer.DeleteObject();
	}

	m_czBuffer.cx = max( m_czBuffer.cx, szItem.cx );
	m_czBuffer.cy = max( m_czBuffer.cy, szItem.cy );

	if ( m_dcBuffer.m_hDC == NULL ) m_dcBuffer.CreateCompatibleDC( &dcScreen );
	m_bmBuffer.CreateCompatibleBitmap( &dcScreen, m_czBuffer.cx, m_czBuffer.cy );
	m_bmOldBuffer = (HBITMAP)m_dcBuffer.SelectObject( &m_bmBuffer )->GetSafeHandle();

	return &m_dcBuffer;
}

//////////////////////////////////////////////////////////////////////
// CCoolInterface watermarking

BOOL CCoolInterface::DrawWatermark(CDC* pDC, CRect* pRect, CBitmap* pMark, BOOL bOverdraw, int nOffX, int nOffY)
{
	if ( pDC == NULL || pRect == NULL || pMark == NULL || pMark->m_hObject == NULL )
		return FALSE;

	BITMAP pWatermark;
	CBitmap* pOldMark;
	CDC dcMark;

	dcMark.CreateCompatibleDC( pDC );
	if ( Settings.General.LanguageRTL )
		SetLayout( dcMark.m_hDC, LAYOUT_BITMAPORIENTATIONPRESERVED );
	pOldMark = (CBitmap*)dcMark.SelectObject( pMark );
	pMark->GetBitmap( &pWatermark );

	if ( ! bOverdraw && nOffX < 0 )		// Rare case fix
		pDC->ExcludeClipRect( pRect->left + nOffX, pRect->top, pRect->left, pRect->bottom );

	for ( int nY = pRect->top + nOffY ; nY < pRect->bottom ; nY += pWatermark.bmHeight )
	{
		if ( nY + pWatermark.bmHeight < pRect->top ) continue;

		for ( int nX = pRect->left + nOffX ; nX < pRect->right ; nX += pWatermark.bmWidth )
		{
			if ( nX + pWatermark.bmWidth < pRect->left ) continue;

			pDC->BitBlt( nX, nY,
				bOverdraw ? pWatermark.bmWidth  : min( pWatermark.bmWidth, pRect->right - nX ),
				bOverdraw ? pWatermark.bmHeight : min( pWatermark.bmHeight, pRect->bottom - nY ),
				&dcMark, 0, 0, SRCCOPY );
		}
	}

	dcMark.SelectObject( pOldMark );
	dcMark.DeleteDC();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoolInterface thumbnail painting

void CCoolInterface::DrawThumbnail(CDC* pDC, const CRect& rcThumb,
	BOOL bWaiting, BOOL bSelected, CBitmap& bmThumb, int nIcon48, int nIcon32,
	const CString& strLabel)
{
	CRect rcFrame( &rcThumb );
	rcFrame.DeflateRect( 1, 1, 1, 1 );

	if ( bmThumb.m_hObject )
	{
		BITMAP info = {};
		bmThumb.GetBitmap( &info );

		CDC dcMem;
		dcMem.CreateCompatibleDC( pDC );

		CBitmap* pOld = dcMem.SelectObject( &bmThumb );

		int cx;
		int cy;
		if ( rcFrame.Width() < info.bmWidth || rcFrame.Height() < info.bmHeight )
		{
			cx = rcFrame.Width();
			cy = rcFrame.Height();
		}
		else
		{
			cx = info.bmWidth;
			cy = info.bmHeight;
		}
		if ( info.bmWidth > info.bmHeight )
			cy = ( cx * info.bmHeight ) / info.bmWidth;
		else
			cx = ( cy * info.bmWidth ) / info.bmHeight;

		int x = rcFrame.left + ( rcFrame.Width() - cx ) / 2;
		int y = rcFrame.top + ( rcFrame.Height() - cy ) / 2;

		pDC->SetStretchBltMode( HALFTONE );
		pDC->StretchBlt( x, y, cx, cy, &dcMem, 0, 0, info.bmWidth, info.bmHeight, SRCCOPY );

		dcMem.SelectObject( pOld );

		pDC->ExcludeClipRect( x, y, x + cx, y + cy );
	}
	else
	{
		if ( int size = ( ( nIcon48 >= 0 ) ? 48 : ( ( nIcon32 >= 0 ) ? 32 : 0 ) ) )
		{
			int x = rcFrame.left + ( rcFrame.Width() - 2 - size ) / 2;
			int y = rcFrame.top + ( rcFrame.Height() - 2 - size ) / 2;

			// ImageList_DrawEx()
			ShellIcons.Draw( pDC, ( ( nIcon48 >= 0 ) ? nIcon48 : nIcon32 ),
				size, x, y, ( bWaiting ? Colors.m_crWindow : Colors.m_crBackNormal ), bSelected );

			pDC->ExcludeClipRect( x, y, x + size, y + size );
		}

		if ( ! strLabel.IsEmpty() )
		{
			CRect rcLabel( rcFrame.left + 2, rcFrame.top + ( rcFrame.Height() + 48 + 4 ) / 2,
				rcFrame.right - 2, rcFrame.bottom - 2 );

			CFont* pOldFont = pDC->SelectObject( bWaiting ? &m_fntBold : &m_fntUnder );
			pDC->SetBkColor( bWaiting ? Colors.m_crWindow : Colors.m_crBackNormal );
			pDC->SetTextColor( bWaiting ? Colors.m_crTextAlert : Colors.m_crTextLink );
			pDC->FillSolidRect( &rcLabel, ( bWaiting ? Colors.m_crWindow : Colors.m_crBackNormal ) );
			pDC->DrawText( strLabel, &rcLabel, DT_CENTER | DT_VCENTER | DT_WORDBREAK |
				DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS );
			pDC->ExcludeClipRect( &rcLabel );
			pDC->SelectObject( pOldFont );
		}
	}

	pDC->FillSolidRect( &rcFrame, ( bWaiting ? Colors.m_crWindow : Colors.m_crBackNormal ) );
	pDC->Draw3dRect( &rcThumb, Colors.m_crMargin, Colors.m_crMargin );
	pDC->ExcludeClipRect( &rcThumb );

	if ( bSelected )
	{
		rcFrame.InflateRect( 2, 2 );
		pDC->Draw3dRect( &rcFrame, Colors.m_crBackCheck, Colors.m_crBackCheck );
		rcFrame.InflateRect( 1, 1 );
		pDC->Draw3dRect( &rcFrame, Colors.m_crHighlight, Colors.m_crHighlight );
		rcFrame.InflateRect( 1, 1 );
		pDC->Draw3dRect( &rcFrame, Colors.m_crBackCheck, Colors.m_crBackCheck );
		pDC->ExcludeClipRect( &rcFrame );
	}
}

//////////////////////////////////////////////////////////////////////
// CCoolInterface fonts

void CCoolInterface::CreateFonts(LPCTSTR pszFace, int nSize)
{
	//CQuickLock oLock( m_pSection );

	if ( ! pszFace ) pszFace = Settings.Fonts.DefaultFont;
	if ( ! nSize ) nSize = Settings.Fonts.DefaultSize;

	if ( m_fntNormal.m_hObject )		m_fntNormal.DeleteObject();
	if ( m_fntBold.m_hObject )			m_fntBold.DeleteObject();
	if ( m_fntUnder.m_hObject ) 		m_fntUnder.DeleteObject();
	if ( m_fntItalic.m_hObject )		m_fntItalic.DeleteObject();
	if ( m_fntBoldItalic.m_hObject )	m_fntBoldItalic.DeleteObject();
	if ( m_fntCaption.m_hObject )		m_fntCaption.DeleteObject();
	if ( m_fntNavBar.m_hObject )		m_fntNavBar.DeleteObject();
	if ( m_fntRichDefault.m_hObject )	m_fntRichDefault.DeleteObject();
	if ( m_fntRichHeading.m_hObject )	m_fntRichHeading.DeleteObject();

	m_fntNormal.CreateFont( -nSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntBold.CreateFont( -nSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntUnder.CreateFont( -nSize, 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntCaption.CreateFont( -nSize - 2, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntItalic.CreateFont( -nSize, 0, 0, 0, FW_NORMAL, TRUE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntBoldItalic.CreateFont( -nSize, 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntNavBar.CreateFont( -nSize - 2, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntRichDefault.CreateFont( -nSize - 1, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntRichHeading.CreateFont( -nSize - 6, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theApp.m_nFontQuality,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );
}


//////////////////////////////////////////////////////////////////////
// CCoolInterface Windows XP+ themes

BOOL CCoolInterface::EnableTheme(CWnd* pWnd, BOOL bEnable)
{
	if ( ! theApp.m_pfnSetWindowTheme )
		return FALSE;	// Win2K

	if ( bEnable )
		return SUCCEEDED( theApp.m_pfnSetWindowTheme( pWnd->GetSafeHwnd(), NULL, NULL ) );
	else
		return SUCCEEDED( theApp.m_pfnSetWindowTheme( pWnd->GetSafeHwnd(), L" ", L" " ) );
}

void CCoolInterface::FixTheme(CWnd* pWnd, BOOL bForce /*=TRUE*/)
{
	const BOOL bThemed =
		GetRValue( Colors.m_crDialogText ) < 100 &&
		GetGValue( Colors.m_crDialogText ) < 100 &&
		GetBValue( Colors.m_crDialogText ) < 100;

	if ( bThemed && ! bForce )
		return;

	for ( CWnd* pChild = pWnd->GetWindow( GW_CHILD ) ; pChild ; pChild = pChild->GetNextWindow() )
	{
		TCHAR szName[8];
		GetClassName( pChild->GetSafeHwnd(), szName, 8 );		// Alt detection method for exceptions
		if ( _tcsnicmp( szName, _PT("Button") ) != 0 )
			continue;

		const int nID = pChild->GetDlgCtrlID();
		if ( nID > IDC_STATIC && nID < 1000 )					// BS_PUSHBUTTON BS_DEFPUSHBUTTON
			continue;

		const DWORD nStyle = pChild->GetStyle();
		if ( ( nStyle & BS_CHECKBOX ) ||
			 ( nStyle & BS_RADIOBUTTON ) ||
			 ( nStyle & BS_AUTORADIOBUTTON ) ||
			 ( ( nStyle & BS_GROUPBOX ) && nID == IDC_STATIC ) )
			EnableTheme( pChild, bThemed );
	}
}

int CCoolInterface::GetImageCount(int nImageListType)
{
	//CQuickLock oLock( m_pSection );

	switch ( nImageListType )
	{
	case LVSIL_SMALL:
		return m_pImages16.GetImageCount();
	case LVSIL_NORMAL:
		return m_pImages32.GetImageCount();
	case LVSIL_BIG:
		return m_pImages48.GetImageCount();
	}
	return 0;
}

BOOL CCoolInterface::Add(CSkin* pSkin, CXMLElement* pBase, HBITMAP hbmImage, COLORREF crMask, int nImageListType)
{
	//CQuickLock oLock( m_pSection );

	VERIFY( ConfirmImageList() );

	int nBase = 0;
	switch ( nImageListType )
	{
	case LVSIL_SMALL:
		nBase = m_pImages16.Add( CBitmap::FromHandle( hbmImage ), crMask );
		break;
	case LVSIL_NORMAL:
		nBase = m_pImages32.Add( CBitmap::FromHandle( hbmImage ), crMask );
		break;
	case LVSIL_BIG:
		nBase = m_pImages48.Add( CBitmap::FromHandle( hbmImage ), crMask );
		break;
	}
	if ( nBase < 0 )
		return FALSE;

	const static LPCTSTR pszNames[] = {
		_T("id"),  _T("id1"), _T("id2"), _T("id3"), _T("id4"), _T("id5"), _T("id6"),
		_T("id7"), _T("id8"), _T("id9"), _T("id10"), _T("id11"), _T("id12"), NULL };
	int nIndex = 0;
	int nIndexRev = GetImageCount( nImageListType ) - 1;	// Total number of images
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("image") ) )
		{
			TRACE( _T("Unknown tag \"%s\" inside \"%s:%s\" in CCoolInterface::Add\r\n"),
				pXML->GetName(), pBase->GetName(), pBase->GetAttributeValue( _T("id") ) );
			continue;
		}

		CString strValue = pXML->GetAttributeValue( _T("index") );
		if ( ! strValue.IsEmpty() )
		{
			if ( _stscanf( strValue, _T("%i"), &nIndex ) != 1 )
			{
				TRACE( _T("Image \"%s\" has invalid index \"%s\" in CCoolInterface::Add\r\n"),
					pBase->GetAttributeValue( _T("id") ), strValue );
				continue;
			}
		}

		nIndex += nBase;
		for ( int nName = 0 ; pszNames[ nName ] ; nName++ )
		{
			UINT nID = pSkin->LookupCommandID( pXML, pszNames[ nName ] );
			if ( nID )
			{
				switch ( nImageListType )
				{
				case LVSIL_SMALL:
					m_pImageMap16.SetAt( nID, Settings.General.LanguageRTL ? nIndexRev : nIndex );
					break;
				case LVSIL_NORMAL:
					m_pImageMap32.SetAt( nID, Settings.General.LanguageRTL ? nIndexRev : nIndex );
					break;
				case LVSIL_BIG:
					m_pImageMap48.SetAt( nID, Settings.General.LanguageRTL ? nIndexRev : nIndex );
					break;
				}
			}
			if ( nName && ! nID ) break;
		}
		nIndexRev--;
		nIndex -= nBase;
		nIndex ++;
	}
	return TRUE;
}

CImageList* CCoolInterface::SetImageListTo(CListCtrl& pWnd, int nImageListType)
{
	//CQuickLock oLock( m_pSection );

	switch ( nImageListType )
	{
	case LVSIL_SMALL:
		return pWnd.SetImageList( &m_pImages16, nImageListType );
	case LVSIL_NORMAL:
		return pWnd.SetImageList( &m_pImages32, nImageListType );
	case LVSIL_BIG:
		return pWnd.SetImageList( &m_pImages48, nImageListType );
	}
	return NULL;
}

BOOL CCoolInterface::Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle, int nImageListType) const
{
	//CQuickLock oLock( m_pSection );

	HIMAGELIST hList = NULL;
	switch ( nImageListType )
	{
	case LVSIL_SMALL:
		hList = m_pImages16.GetSafeHandle();
		break;
	case LVSIL_NORMAL:
		hList = m_pImages32.GetSafeHandle();
		break;
	case LVSIL_BIG:
		hList = m_pImages48.GetSafeHandle();
		break;
	}
	return ImageList_Draw( hList, nImage, pDC->GetSafeHdc(), pt.x, pt.y, nStyle );
}

BOOL CCoolInterface::DrawEx(CDC* pDC, int nImage, POINT pt, SIZE sz, COLORREF clrBk, COLORREF clrFg, UINT nStyle, int nImageListType) const
{
	//CQuickLock oLock( m_pSection );

	HIMAGELIST hList = NULL;
	switch ( nImageListType )
	{
	case LVSIL_SMALL:
		hList = m_pImages16.GetSafeHandle();
		break;
	case LVSIL_NORMAL:
		hList = m_pImages32.GetSafeHandle();
		break;
	case LVSIL_BIG:
		hList = m_pImages48.GetSafeHandle();
		break;
	}
	return ImageList_DrawEx( hList, nImage, pDC->GetSafeHdc(),
		pt.x, pt.y, sz.cx, sz.cy, clrBk, clrFg, nStyle );
}

//BOOL CCoolInterface::DrawIndirect(CDC* pDC, int nImage, POINT pt, SIZE sz, COLORREF clrBk, COLORREF clrFg, UINT nStyle, DWORD nState /*=ILS_ALPHA*/, DWORD nAlpha /*=200*/, int nImageListType /*=LVSIL_SMALL*/) const
//{
//	//CQuickLock oLock( m_pSection );
//
//#ifndef WIN64
//	if ( theApp.m_bIsWin2000 )
//		return DrawEx( pDC, nImage, pt, sz, clrBk, clrFg, nStyle, nImageListType );
//#endif
//
//	//return m_pImages16.DrawIndirect( pDC, nImage, pt, sz, (POINT)(CPoint)0,
//	//	nStyle, MERGECOPY, clrBk, clrFg, nState, nAlpha, clrFg );
//
//	IMAGELISTDRAWPARAMS pImageListDrawParams = { 0 };
//
//	pImageListDrawParams.hdcDst = pDC->GetSafeHdc();
//	pImageListDrawParams.i = nImage;
//	pImageListDrawParams.x = pt.x;
//	pImageListDrawParams.y = pt.y;
//	pImageListDrawParams.cx = sz.cx;
//	pImageListDrawParams.cy = sz.cy;
//	pImageListDrawParams.xBitmap = 0;
//	pImageListDrawParams.yBitmap = 0;
//	pImageListDrawParams.rgbBk = clrBk;
//	pImageListDrawParams.rgbFg = clrFg;
//	pImageListDrawParams.crEffect = clrFg;
//	pImageListDrawParams.fStyle = nStyle;		// ILD_BLEND50|ILD_BLEND25|ILD_ROP
//	pImageListDrawParams.fState = nState;		// ILS_ALPHA|ILS_SATURATE|ILS_SHADOW
//	pImageListDrawParams.Frame = nAlpha;
//	pImageListDrawParams.dwRop = MERGECOPY;
//
//	switch ( nImageListType )
//	{
//	case LVSIL_SMALL:
//		pImageListDrawParams.himl = m_pImages16.GetSafeHandle();
//	case LVSIL_NORMAL:
//		pImageListDrawParams.himl = m_pImages32.GetSafeHandle();
//	case LVSIL_BIG:
//		pImageListDrawParams.himl = m_pImages48.GetSafeHandle();
//	}
//
//	return ImageList_DrawIndirect( &pImageListDrawParams );
//}

BOOL CCoolInterface::Draw(CDC* pDC, UINT nID, int nSize, int nX, int nY, COLORREF crBack, BOOL bSelected, BOOL bExclude) const
{
	//CQuickLock oLock( m_pSection );

	HIMAGELIST hList;
	int nType;
	switch ( nSize )
	{
	case 16:
		hList = m_pImages16.GetSafeHandle();
		nType = LVSIL_SMALL;
		break;
	case 32:
		hList = m_pImages32.GetSafeHandle();
		nType = LVSIL_NORMAL;
		break;
	case 48:
		hList = m_pImages48.GetSafeHandle();
		nType = LVSIL_BIG;
		break;
	default:
		ASSERT( FALSE );
		return FALSE;
	}

	int nImage = ImageForID( nID, nType );
	if ( nImage == -1 ) return FALSE;

	BOOL bRet = ImageList_DrawEx( hList, nImage, pDC->GetSafeHdc(),
		nX, nY, nSize, nSize, crBack, CLR_DEFAULT, bSelected ? ILD_SELECTED : ILD_NORMAL );
	if ( bExclude && crBack == CLR_NONE )
		pDC->ExcludeClipRect( nX, nY, nX + nSize, nY + nSize );
	return bRet;
}
