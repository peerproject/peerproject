//
// ImageFile.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#pragma once


class CImageFile : boost::noncopyable
{
// Construction
public:
	CImageFile();
	virtual ~CImageFile();

public:
	BOOL			m_bScanned;
	int				m_nWidth;
	int				m_nHeight;
	DWORD			m_nComponents;
	BOOL			m_bLoaded;
	LPBYTE			m_pImage;
	WORD			m_nFlags;

// Operations
public:
	BOOL	LoadFromMemory(LPCTSTR pszType, LPCVOID pData, DWORD nLength, BOOL bScanOnly = FALSE, BOOL bPartialOk = FALSE);
	BOOL	LoadFromFile(LPCTSTR pszFile, BOOL bScanOnly = FALSE, BOOL bPartialOk = FALSE);
	BOOL	LoadFromURL(LPCTSTR pszURL);
	BOOL	LoadFromBitmap(HBITMAP hBitmap, BOOL bScanOnly = FALSE);	// Get image copy from HBITMAP (24/32-bit only)
	BOOL	LoadFromResource(HINSTANCE hInstance, UINT nResourceID, LPCTSTR pszType, BOOL bScanOnly = FALSE, BOOL bPartialOk = FALSE);
	BOOL	SaveToMemory(LPCTSTR pszType, int nQuality, LPBYTE* ppBuffer, DWORD* pnLength);
//	BOOL	SaveToFile(LPCTSTR pszType, int nQuality, HANDLE hFile, DWORD* pnLength = NULL);
//	BOOL	SaveToFile(LPCTSTR pszFile, int nQuality);
	DWORD	GetSerialSize() const;
	void	Serialize(CArchive& ar);
	HBITMAP	CreateBitmap(HDC hUseDC = 0);
	BOOL	FitTo(int nNewWidth, int nNewHeight);
	BOOL	Resample(int nNewWidth, int nNewHeight);
//	BOOL	FastResample(int nNewWidth, int nNewHeight);
	BOOL	EnsureRGB(COLORREF crBack = 0xFFFFFFFF);
	BOOL	SwapRGB();

	static HBITMAP LoadBitmapFromFile(LPCTSTR pszFile)
	{
		if ( _tcsicmp( PathFindExtension( pszFile ), _T(".bmp") ) == 0 )
			return (HBITMAP)LoadImage( NULL, pszFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

		CImageFile pFile;
		return ( pFile.LoadFromFile( pszFile, FALSE, FALSE ) && pFile.EnsureRGB() ) ?
			pFile.CreateBitmap() : NULL;
	}

	static HBITMAP LoadBitmapFromResource(UINT nResourceID, LPCTSTR pszType, HINSTANCE hInstance = AfxGetResourceHandle())
	{
		if ( pszType == RT_BITMAP || _tcscmp( pszType, RT_BMP ) == 0 )
			return (HBITMAP)LoadImage( hInstance,
				MAKEINTRESOURCE( nResourceID ), IMAGE_BITMAP, 0, 0, 0 );

		CImageFile pFile;
		return ( pFile.LoadFromResource( hInstance, nResourceID, pszType ) &&
			pFile.EnsureRGB() ) ? pFile.CreateBitmap() : NULL;
	}

	enum ImageFlags
	{
		idRemote = 0x1
	};

protected:
	void	Clear();
	BOOL	MonoToRGB();
	BOOL	AlphaToRGB(COLORREF crBack);
};
