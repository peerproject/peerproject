//
// ImageFile.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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
#include "ImageServices.h"
#include "ImageFile.h"
#include "HttpRequest.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageFile construction

CImageFile::CImageFile() :
	m_bScanned( FALSE ),
	m_nWidth( 0 ),
	m_nHeight( 0 ),
	m_nComponents( 0 ),
	m_bLoaded( FALSE ),
	m_pImage( NULL ),
	m_nFlags( 0 )
{
}

CImageFile::~CImageFile()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////
// CImageFile clear operation

void CImageFile::Clear()
{
	delete [] m_pImage;

	m_bScanned		= FALSE;
	m_nWidth		= 0;
	m_nHeight		= 0;
	m_nComponents	= 0;
	m_bLoaded		= FALSE;
	m_pImage		= NULL;
	m_nFlags		= 0;
}

/////////////////////////////////////////////////////////////////////////////
// CImageFile load operations

BOOL CImageFile::LoadFromMemory(LPCTSTR pszType, LPCVOID pData, DWORD nLength, BOOL bScanOnly, BOOL bPartialOk)
{
	Clear();

	return m_bLoaded = ImageServices.LoadFromMemory( this, pszType, pData, nLength, bScanOnly, bPartialOk );
}

BOOL CImageFile::LoadFromFile(LPCTSTR pszFile, BOOL bScanOnly, BOOL bPartialOk)
{
	Clear();

	return m_bLoaded = ImageServices.LoadFromFile( this, pszFile, bScanOnly, bPartialOk );
}

BOOL CImageFile::LoadFromResource(HINSTANCE hInstance, UINT nResourceID, LPCTSTR pszType, BOOL bScanOnly, BOOL bPartialOk)
{
	Clear();

	HMODULE hModule = (HMODULE)hInstance;
	HRSRC hRes = FindResource( hModule, MAKEINTRESOURCE( nResourceID ), pszType );
	if ( hRes  )
	{
		DWORD nSize			= SizeofResource( hModule, hRes );
		HGLOBAL hMemory		= LoadResource( hModule, hRes );
		if ( hMemory )
		{
			LPCVOID pMemory	= (LPCVOID)LockResource( hMemory );
			if ( pMemory )
			{
				CString strType;

				if ( _tcscmp( pszType, RT_JPEG ) == 0 )
				{
					pszType = _T(".jpg");
				}
				else if ( _tcscmp( pszType, RT_PNG ) == 0 )
				{
					pszType = _T(".png");
				}
				else
				{
					strType.Format( _T(".%s"), pszType );
					pszType = strType;
				}

				m_bLoaded = ImageServices.LoadFromMemory( this, pszType, pMemory, nSize, bScanOnly, bPartialOk );
			}
			FreeResource( hMemory );
		}
	}
	return m_bLoaded;
}

BOOL CImageFile::LoadFromURL(LPCTSTR pszURL)
{
	CHttpRequest pImageFetcher;

	if ( !pImageFetcher.SetURL( pszURL ) )
		return FALSE;
	pImageFetcher.LimitContentLength( Settings.Search.MaxPreviewLength * 100 );

	if ( !pImageFetcher.Execute( TRUE ) )
		return FALSE;
	while ( pImageFetcher.IsPending() )
		Sleep( 50 );

	if ( pImageFetcher.GetStatusSuccess() )
	{
		CString strMIME = pImageFetcher.GetHeader( L"Content-Type" );

		if ( strMIME.CompareNoCase( L"image/jpeg" ) != 0 &&
			 strMIME.CompareNoCase( L"image/gif" ) != 0 &&
			 strMIME.CompareNoCase( L"image/bmp" ) != 0 &&
			 strMIME.CompareNoCase( L"image/png" ) != 0 )
		{
			theApp.Message( MSG_DEBUG, L"Preview failed: unacceptable content type." );
			return FALSE;
		}

		CBuffer* pBuffer = pImageFetcher.GetResponseBuffer();
		if ( pBuffer == NULL ) return FALSE;

		strMIME.Replace( '/', '.' );
		m_bLoaded = ImageServices.LoadFromMemory( this, strMIME, (LPVOID)pBuffer->m_pBuffer,
													pBuffer->m_nLength );
		if ( m_bLoaded )
			m_nFlags |= idRemote;

		return m_bLoaded;
	}

	return FALSE;
}

BOOL CImageFile::LoadFromBitmap(HBITMAP hBitmap, BOOL bScanOnly)
{
	BITMAP bmInfo;
	if ( ! GetObject( hBitmap, sizeof( BITMAP ), &bmInfo ) )
		return FALSE;

	if ( bmInfo.bmType != 0 || bmInfo.bmPlanes != 1 || ! bmInfo.bmBits ||
		bmInfo.bmWidth < 1 || bmInfo.bmHeight < 1 )
		return FALSE;	// Unsupported format

	m_bScanned = TRUE;
	m_nWidth = bmInfo.bmWidth;
	m_nHeight = bmInfo.bmHeight;
	//if ( bmInfo.bmBitsPixel == 32 )
	//	m_nComponents = 4;
	//else //if ( bmInfo.bmBitsPixel == 24 )
	m_nComponents = 3;

	if ( bScanOnly )
		return TRUE;

	DWORD line_size = ( m_nWidth * m_nComponents + 3 ) & ~3;
	m_pImage = new BYTE[ line_size * m_nHeight ];
	if ( ! m_pImage )
		return FALSE;	// Out of memory

	HDC hDC = GetDC( NULL );
	BITMAPINFOHEADER bmi = { sizeof( BITMAPINFOHEADER ), bmInfo.bmWidth, - bmInfo.bmHeight, 1, 24, BI_RGB };
	GetDIBits( hDC, hBitmap, 0, bmInfo.bmHeight, m_pImage, (BITMAPINFO*)&bmi, DIB_RGB_COLORS );
	ReleaseDC( NULL, hDC );

	// BGR -> RGB
	LPBYTE dst = m_pImage;
	for ( LONG j = 0; j < bmInfo.bmHeight; ++j, dst += line_size )
	{
		BYTE c;
	//	if ( m_nComponents = 3 )
	//	{
			for ( LONG i = 0; i < bmInfo.bmWidth * 3; i += 3 )
			{
				c = dst[i + 0];
				dst[i + 0] = dst[i + 2];
				dst[i + 2] = c;
			}
	//	}
	//	else if ( m_nComponents = 4 )
	//	{
	//		for ( LONG i = 0; i < bmInfo.bmWidth * 3; i += 3 )
	//		{
	//			c = dst[i + 0];
	//			dst[i + 0] = dst[i + 2];
	//			dst[i + 2] = dst[i + 3];
	//			dst[i + 3] = c;
	//		}
	//	}
	}

	m_bLoaded = TRUE;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CImageFile save operations

BOOL CImageFile::SaveToMemory(LPCTSTR pszType, int nQuality, LPBYTE* ppBuffer, DWORD* pnLength)
{
	if ( ! m_bLoaded ) return FALSE;
	return ImageServices.SaveToMemory( this, pszType, nQuality, ppBuffer, pnLength );
}

//BOOL CImageFile::SaveToFile(LPCTSTR pszType, int nQuality, HANDLE hFile, DWORD* pnLength)
//{
//	if ( ! m_bLoaded ) return FALSE;
//	return ImageServices.SaveToFile( this, pszType, nQuality, hFile, pnLength );
//}

//BOOL CImageFile::SaveToFile(LPCTSTR pszFile, int nQuality)
//{
//	if ( ! m_bLoaded ) return FALSE;
//
//	HANDLE hFile = CreateFile( pszFile, GENERIC_WRITE, 0,
//		NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
//
//	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;
//
//	BOOL bResult = ImageServices.SaveToFile( this, pszFile, nQuality, hFile );
//
//	CloseHandle( hFile );
//
//	if ( ! bResult ) DeleteFile( pszFile );
//
//	return bResult;
//}

/////////////////////////////////////////////////////////////////////////////
// CImageFile serialization

DWORD CImageFile::GetSerialSize() const
{
	if ( ! m_bLoaded ) return 4;
	return 12 + ( ( m_nWidth * m_nComponents + 3 ) & ~3 ) * m_nHeight;
}

void CImageFile::Serialize(CArchive& ar)
{
	if ( ar.IsStoring() )
	{
		if ( ! m_bLoaded )
		{
			ar << m_bLoaded;
			return;
		}

		ar << m_nWidth;
		ar << m_nHeight;
		DWORD nCompositeValue = ( m_nFlags << 16 ) | ( m_nComponents );
		ar << nCompositeValue;

		ar.Write( m_pImage, ( ( m_nWidth * m_nComponents + 3 ) & ~3 ) * m_nHeight );
	}
	else
	{
		Clear();

		ar >> m_nWidth;
		if ( ! m_nWidth ) return;
		ar >> m_nHeight;
		DWORD nCompositeValue;
		ar >> nCompositeValue;

		// Get higher bits for flags
		m_nFlags = nCompositeValue >> 16;
		// Clear high bits for components
		m_nComponents = nCompositeValue & 0x0000FFFF;

		int nPitch = ( ( m_nWidth * m_nComponents + 3 ) & ~3 ) * m_nHeight;

		m_pImage = new BYTE[ nPitch  ];
		ReadArchive( ar, m_pImage, nPitch );

		m_bLoaded = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImageFile image access operations

HBITMAP CImageFile::CreateBitmap(HDC hUseDC)
{
	if ( ! m_bLoaded ) return NULL;
	if ( m_nComponents != 3 ) return NULL;

	BITMAPV5HEADER pV5Header = {};

	pV5Header.bV5Size			= sizeof(BITMAPV5HEADER);
	pV5Header.bV5Width			= (LONG)m_nWidth;
	pV5Header.bV5Height			= (LONG)m_nHeight;
	pV5Header.bV5Planes			= 1;
	pV5Header.bV5BitCount		= 24; // Not 32 bit :(
	pV5Header.bV5Compression	= BI_RGB;
	pV5Header.bV5SizeImage		= m_nWidth * m_nHeight * 3;

	// ToDo: The following mask specification specifies a supported 32 BPP alpha format for Windows XP.
	// pV5Header.bV5RedMask   =  0x00FF0000;
	// pV5Header.bV5GreenMask =  0x0000FF00;
	// pV5Header.bV5BlueMask  =  0x000000FF;
	// pV5Header.bV5AlphaMask =  0xFF000000;

	HDC hDC = hUseDC ? hUseDC : GetDC( 0 );
	HBITMAP hBitmap;
	__try
	{
		void* pBits = NULL;
		hBitmap = CreateDIBSection( hDC, (BITMAPINFO*)&pV5Header, DIB_RGB_COLORS, (void**)&pBits, NULL, 0ul );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		hBitmap = NULL;
	}

	if ( hBitmap )
	{
		DWORD nPitch	= ( m_nWidth * 3 + 3 ) & ~3u;
		BYTE* pLine		= m_pImage;

		for ( int nY = m_nHeight; nY--; )
		{
			struct SwapRGB
			{
				void operator()(BYTE* pBegin, BYTE* pEnd)
				{
					for ( ; pBegin != pEnd; pBegin += 3 )
						std::swap( pBegin[ 0 ], pBegin[ 2 ] );
				}
			};

			SwapRGB()( pLine, pLine + m_nWidth * 3 );

			SetDIBits( hDC, hBitmap, nY, 1, pLine, (BITMAPINFO*)&pV5Header, DIB_RGB_COLORS );

			SwapRGB()( pLine, pLine + m_nWidth * 3 );

			pLine += nPitch;
		}
	}

	if ( hDC != hUseDC )
	{
		SelectObject( hDC, GetStockObject( ANSI_VAR_FONT ) ); // font leak fix
		ReleaseDC( 0, hDC );
	}

	return hBitmap;
}

/////////////////////////////////////////////////////////////////////////////
// CImageFile image modification operations

BOOL CImageFile::FitTo(int nNewWidth, int nNewHeight)
{
	int nSize = ( nNewHeight * m_nWidth ) / m_nHeight;
	if ( nSize > nNewWidth )
	{
		nSize = ( nNewWidth * m_nHeight ) / m_nWidth;
		return Resample( nNewWidth, nSize );
	}
	else
	{
		return Resample( nSize, nNewHeight );
	}
}

BOOL CImageFile::Resample(int nNewWidth, int nNewHeight)
{
	if ( m_nWidth <= 0 || m_nHeight <= 0 )
		return FALSE;
	if ( nNewWidth <= 0 || nNewHeight <= 0 )
		return FALSE;
	if ( ! m_bLoaded )
		return FALSE;
	if ( m_nComponents != 3 )
		return FALSE;
	if ( nNewWidth == m_nWidth && nNewHeight == m_nHeight )
		return TRUE;

	DWORD nInPitch	= ( m_nWidth * 3 + 3 ) & ~3u;
	DWORD nOutPitch	= ( nNewWidth * 3 + 3 ) & ~3u;

	BYTE* pNew = new BYTE[ nOutPitch * nNewHeight ];
	if ( ! pNew )
		return FALSE;
	BYTE* pOut = pNew;

	int* pColInfo = new int[ nNewWidth * 2 ];
	if ( ! pColInfo )
	{
		delete [] pNew;
		return FALSE;
	}
	int* pColPtr = pColInfo;

	for ( int nX = 0 ; nX < nNewWidth ; nX++ )
	{
		int nFirst = ( nX * m_nWidth / nNewWidth );
		int nCount = ( (nX+1) * m_nWidth / nNewWidth ) - nFirst + 1;
		if ( nFirst + nCount >= m_nWidth ) nCount = 1;
		*pColPtr++ = nFirst * 3;
		*pColPtr++ = nCount;
	}

	for ( int nY = 0 ; nY < nNewHeight ; nY++ )
	{
		int nFirst = ( nY * m_nHeight / nNewHeight );
		int nCount = ( ( nY + 1 ) * m_nHeight / nNewHeight ) - nFirst + 1;

		if ( nFirst + nCount >= m_nHeight ) nCount = 1;

		BYTE* pRow = m_pImage + nInPitch * nFirst;
		pColPtr = pColInfo;

		for ( int nX = 0 ; nX < nNewWidth ; nX++, pColPtr++ )
		{
			BYTE* pIn = pRow + *pColPtr++;

			DWORD nRed = 0, nGreen = 0, nBlue = 0, nPixels = 0;

			for ( int nYY = nCount ; nYY ; nYY-- )
			{
				for ( int nXX = *pColPtr ; nXX ; nXX-- )
				{
					nRed	+= *pIn++;
					nGreen	+= *pIn++;
					nBlue	+= *pIn++;
					nPixels++;
				}

				pIn += nInPitch - *pColPtr - *pColPtr - *pColPtr;
			}

			*pOut++ = (BYTE)( nRed / nPixels );
			*pOut++ = (BYTE)( nGreen / nPixels );
			*pOut++ = (BYTE)( nBlue / nPixels );

		}
		pOut += ( nOutPitch - nNewWidth * 3 );
	}

	delete [] pColInfo;
	delete [] m_pImage;

	m_pImage	= pNew;
	m_nWidth	= nNewWidth;
	m_nHeight	= nNewHeight;

	return TRUE;
}

//BOOL CImageFile::FastResample(int nNewWidth, int nNewHeight)
//{
//	if ( ! m_bLoaded ) return FALSE;
//	if ( m_nComponents != 3 ) return FALSE;
//	if ( nNewWidth == m_nWidth && nNewHeight == m_nHeight ) return TRUE;

//	DWORD nInPitch	= ( m_nWidth * 3 + 3 ) & ~3u;
//	DWORD nOutPitch	= ( nNewWidth * 3 + 3 ) & ~3u;
//	BYTE *pNew, *pRow, *pIn, *pOut;
//	pOut = pNew = new BYTE[ nOutPitch * nNewHeight ];

//	for ( int nY = 0 ; nY < nNewHeight ; nY++ )
//	{
//		pRow = m_pImage + nInPitch * ( nY * m_nHeight / nNewHeight );

//		for ( int nX = 0 ; nX < nNewWidth ; nX++ )
//		{
//			pIn = pRow + 3 * ( nX * m_nWidth / nNewWidth );
//			*pOut++ = *pIn++;
//			*pOut++ = *pIn++;
//			*pOut++ = *pIn++;
//		}
//		pOut += ( nOutPitch - nNewWidth * 3 );
//	}

//	delete [] m_pImage;
//	m_pImage	= pNew;
//	m_nWidth	= nNewWidth;
//	m_nHeight	= nNewHeight;
//	return TRUE;
//}

/////////////////////////////////////////////////////////////////////////////
// CImageFile image component modification

BOOL CImageFile::EnsureRGB(COLORREF crBack)
{
	if ( ! m_bLoaded || ! m_pImage || m_nWidth < 1 || m_nHeight < 1 )
		return FALSE;
	else if ( m_nComponents == 3 )
		return TRUE;
	else if ( m_nComponents == 1 )
		return MonoToRGB();
	else if ( m_nComponents == 4 )
		return AlphaToRGB( crBack );
	else
		return FALSE;
}

BOOL CImageFile::MonoToRGB()
{
	if ( ! m_bLoaded ) return FALSE;
	if ( m_nComponents == 3 ) return TRUE;
	if ( m_nComponents != 1 ) return FALSE;

	DWORD nInPitch	= ( m_nWidth + 3 ) & ~3u;
	DWORD nOutPitch	= ( m_nWidth * 3 + 3 ) & ~3u;

	BYTE* pNew		= new BYTE[ nOutPitch * m_nHeight ];
	BYTE* pInRow	= m_pImage;
	BYTE* pOutRow	= pNew;

	for ( int nY = m_nHeight ; nY ; nY-- )
	{
		BYTE* pInCol	= pInRow;
		BYTE* pOutCol	= pOutRow;

		for ( int nX = m_nWidth ; nX ; nX-- )
		{
			*pOutCol++ = *pInCol;
			*pOutCol++ = *pInCol;
			*pOutCol++ = *pInCol++;
		}

		pInRow += nInPitch;
		pOutRow += nOutPitch;
	}

	delete [] m_pImage;

	m_pImage		= pNew;
	m_nComponents	= 3;

	return TRUE;
}

BOOL CImageFile::AlphaToRGB(COLORREF crBack)
{
	if ( ! m_bLoaded ) return FALSE;
	if ( m_nComponents == 3 ) return TRUE;
	if ( m_nComponents != 4 ) return FALSE;

	DWORD nInPitch	= ( m_nWidth * 4 + 3 ) & ~3u;
	DWORD nOutPitch	= ( m_nWidth * 3 + 3 ) & ~3u;

	BYTE* pNew		= new BYTE[ nOutPitch * m_nHeight ];
	BYTE* pInRow	= m_pImage;
	BYTE* pOutRow	= pNew;

	for ( int nY = m_nHeight ; nY ; nY-- )
	{
		BYTE* pInCol	= pInRow;
		BYTE* pOutCol	= pOutRow;

		for ( int nX = m_nWidth ; nX ; nX-- )
		{
			DWORD nAlpha = (DWORD)pInCol[3];

			if ( nAlpha == 255 )
			{
				*pOutCol++ = *pInCol++;
				*pOutCol++ = *pInCol++;
				*pOutCol++ = *pInCol++;
				pInCol++;
			}
			else if ( nAlpha == 0 )
			{
				*pOutCol++ = GetRValue( crBack );
				*pOutCol++ = GetGValue( crBack );
				*pOutCol++ = GetBValue( crBack );
				pInCol += 4;
			}
			else
			{
				*pOutCol++ = (BYTE)( ( (DWORD)(*pInCol++) * nAlpha + (DWORD)GetRValue( crBack ) * ( 255 - nAlpha ) ) / 255 );
				*pOutCol++ = (BYTE)( ( (DWORD)(*pInCol++) * nAlpha + (DWORD)GetGValue( crBack ) * ( 255 - nAlpha ) ) / 255 );
				*pOutCol++ = (BYTE)( ( (DWORD)(*pInCol++) * nAlpha + (DWORD)GetBValue( crBack ) * ( 255 - nAlpha ) ) / 255 );
				pInCol++;
			}
		}

		pInRow += nInPitch;
		pOutRow += nOutPitch;
	}

	delete [] m_pImage;

	m_pImage		= pNew;
	m_nComponents	= 3;

	return TRUE;
}

BOOL CImageFile::SwapRGB()
{
	if ( ! m_bLoaded ) return FALSE;
	if ( m_nComponents != 3 ) return FALSE;

	DWORD nPitch = ( m_nWidth * 3 + 3) & ~3u;

	BYTE* pImage = m_pImage;
	BYTE nTemp;

	for ( int nY = m_nHeight ; nY ; nY-- )
	{
		BYTE* pRow = pImage;
		pImage += nPitch;

		for ( int nX = m_nWidth ; nX ; nX-- )
		{
			nTemp = pRow[0];
			pRow[0] = pRow[2];
			pRow[2] = nTemp;
			pRow += 3;
		}
	}

	return TRUE;
}
