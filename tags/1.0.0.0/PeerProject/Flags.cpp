//
// Flags.cpp
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
#include "ImageServices.h"
#include "ImageFile.h"
#include "Flags.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

#define SOURCE_FLAG_WIDTH 		16
#define SOURCE_FLAG_HEIGHT		12
#define IMAGELIST_FLAG_WIDTH	16
#define IMAGELIST_FLAG_HEIGHT	16


CFlags Flags;

CFlags::CFlags()
{
}

CFlags::~CFlags()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CFlags load

BOOL CFlags::Load()
{
	Clear();

	const CString strFile = Settings.General.Path + _T("\\Data\\Flags.png");	// Settings.General.DataPath ?

	m_pImage.Create( IMAGELIST_FLAG_WIDTH, IMAGELIST_FLAG_HEIGHT, ILC_COLOR32|ILC_MASK, 26 * 26, 8 ) ||
		m_pImage.Create( IMAGELIST_FLAG_WIDTH, IMAGELIST_FLAG_HEIGHT, ILC_COLOR24|ILC_MASK, 26 * 26, 8 ) ||
		m_pImage.Create( IMAGELIST_FLAG_WIDTH, IMAGELIST_FLAG_HEIGHT, ILC_COLOR16|ILC_MASK, 26 * 26, 8 );

	CImageFile pImage;

	if ( ! pImage.LoadFromFile( strFile ) ||
		 ! pImage.EnsureRGB( GetSysColor( COLOR_WINDOW ) ) ||
		 ! pImage.SwapRGB() ||
		 pImage.m_nWidth < SOURCE_FLAG_WIDTH * 26 ||
		 pImage.m_nHeight < SOURCE_FLAG_HEIGHT * 26 )
	{
		return FALSE;
	}

	const COLORREF crMask = RGB( 0, 255, 0 );

	for ( int i = 0 ; i < 26 ; i++ )
	{
		for ( int j = 0 ; j < 26 ; j++ )
		{
			CRect rc(
				i * SOURCE_FLAG_WIDTH,
				j * SOURCE_FLAG_HEIGHT,
				(i + 1) * SOURCE_FLAG_WIDTH,
				(j + 1) * SOURCE_FLAG_HEIGHT );

			AddFlag( &pImage, &rc, crMask );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFlags add a flag

void CFlags::AddFlag(CImageFile* pImage, CRect* pRect, COLORREF crBack)
{
	//ASSERT( pImage->m_bLoaded && pImage->m_nComponents == 3 );	// Should allow alpha
	//ASSERT( pRect->left >= 0 && pRect->left + SOURCE_FLAG_WIDTH <= pImage->m_nWidth );
	//ASSERT( pRect->top >= 0 && pRect->top <= pImage->m_nHeight + SOURCE_FLAG_HEIGHT );
	//ASSERT( pRect->right == pRect->left + SOURCE_FLAG_WIDTH );
	//ASSERT( pRect->bottom == pRect->top + SOURCE_FLAG_HEIGHT );

	DWORD nPitch = pImage->m_nWidth * pImage->m_nComponents;
	while ( nPitch & 3 ) nPitch++;

	BYTE* pSource = pImage->m_pImage;
	pSource += pRect->top * nPitch + pRect->left * pImage->m_nComponents;

	HDC hDC = GetDC( NULL );					// Get screen DC
	HDC hDCMem1 = CreateCompatibleDC( hDC );	// Create source memory DC
	HDC hDCMem2 = CreateCompatibleDC( hDC );	// Create destination memory DC

	CBitmap bmOriginal, bmMoved;
	CDC* pDC = CDC::FromHandle( hDC );

	bmOriginal.CreateCompatibleBitmap( pDC, SOURCE_FLAG_WIDTH, SOURCE_FLAG_HEIGHT );	// Source bitmap
	bmMoved.CreateCompatibleBitmap( pDC, IMAGELIST_FLAG_WIDTH, IMAGELIST_FLAG_HEIGHT );	// Destination bitmap

	BITMAPINFOHEADER pInfo = {};
	pInfo.biSize		= sizeof( BITMAPINFOHEADER );
	pInfo.biWidth		= SOURCE_FLAG_WIDTH;
	pInfo.biHeight		= SOURCE_FLAG_HEIGHT;
	pInfo.biPlanes		= 1;
	pInfo.biBitCount	= 24;
	pInfo.biCompression	= BI_RGB;
	pInfo.biSizeImage	= SOURCE_FLAG_WIDTH * SOURCE_FLAG_HEIGHT * 3;

	for ( int nY = SOURCE_FLAG_HEIGHT - 1 ; nY >= 0 ; nY-- )
	{
		SetDIBits( hDCMem1, bmOriginal, nY, 1, pSource, (BITMAPINFO*)&pInfo, DIB_RGB_COLORS );
		pSource += nPitch;
	}

	HBITMAP hOld_bm1 = (HBITMAP)SelectObject( hDCMem1, bmOriginal.m_hObject );
	HBITMAP hOld_bm2 = (HBITMAP)SelectObject( hDCMem2, bmMoved.m_hObject );
	CDC* pDC2 = CDC::FromHandle( hDCMem2 );
	pDC2->SetBkMode( TRANSPARENT );
	pDC2->FillSolidRect( 0, 0, IMAGELIST_FLAG_WIDTH, IMAGELIST_FLAG_HEIGHT, crBack );

	if ( Settings.General.LanguageRTL )
		SetLayout( hDCMem2, LAYOUT_RTL );

	StretchBlt( hDCMem2, 0, 3, SOURCE_FLAG_WIDTH, SOURCE_FLAG_HEIGHT,
				hDCMem1, 0, 0, SOURCE_FLAG_WIDTH, SOURCE_FLAG_HEIGHT, SRCCOPY );

	SelectObject( hDCMem1, hOld_bm1 );
	SelectObject( hDCMem2, hOld_bm2 );
	VERIFY( DeleteDC( hDCMem1 ) );
	VERIFY( DeleteDC( hDCMem2 ) );
	ReleaseDC( NULL, hDC );
	m_pImage.Add( &bmMoved, crBack );
	bmMoved.DeleteObject();
	bmOriginal.DeleteObject();
}

//////////////////////////////////////////////////////////////////////
// CFlags clear

void CFlags::Clear()
{
	if ( m_pImage.m_hImageList )
		m_pImage.DeleteImageList();
}

int CFlags::GetCount() const
{
	return m_pImage.GetImageCount();
}

int CFlags::GetFlagIndex(const CString& sCountry) const
{
	if ( sCountry.GetLength() == 2 )
	{
		char nFirstLetter  = (char)( sCountry[0] - 'A' );
		char nSecondLetter = (char)( sCountry[1] - 'A' );
		// Currently only the letters A-Z are in the flag matrix
		// but GeoIP can also return some combinations that aren't all letters (A1, A2, etc.)
		if ( nFirstLetter >= 0 && nFirstLetter < 26 && nSecondLetter >= 0 && nSecondLetter < 26 )
			return nFirstLetter * 26 + nSecondLetter;
	}
	return -1;
}

HICON CFlags::ExtractIcon(int i)
{
	return m_pImage.ExtractIcon( i );
}

BOOL CFlags::Draw(int i, HDC hdcDst, int x, int y, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle)
{
	return ImageList_DrawEx( m_pImage, i, hdcDst, x, y,
		IMAGELIST_FLAG_WIDTH, IMAGELIST_FLAG_HEIGHT, rgbBk, rgbFg, fStyle );
}
