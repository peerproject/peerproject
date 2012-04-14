//
// ShellIcons.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#include "StdAfx.h"
#include "Settings.h"
#include "ShellIcons.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CShellIcons ShellIcons;

//////////////////////////////////////////////////////////////////////
// CShellIcons construction

CShellIcons::CShellIcons()
{
	// Experimental values
	m_m16.InitHashTable( 31 );
	m_m32.InitHashTable( 31 );
	m_m48.InitHashTable( 31 );
}

//CShellIcons::~CShellIcons()
//{
//}

//////////////////////////////////////////////////////////////////////
// CShellIcons clear

void CShellIcons::Clear()
{
	m_m16.RemoveAll();
	m_m32.RemoveAll();
	m_m48.RemoveAll();

	if ( m_i16.m_hImageList ) m_i16.DeleteImageList();
	if ( m_i32.m_hImageList ) m_i32.DeleteImageList();
	if ( m_i48.m_hImageList ) m_i48.DeleteImageList();

	m_i16.Create( 16, 16, ILC_COLOR32|ILC_MASK, 0, 16 ) ||
	m_i16.Create( 16, 16, ILC_COLOR24|ILC_MASK, 0, 16 ) ||
	m_i16.Create( 16, 16, ILC_COLOR16|ILC_MASK, 0, 16 );

	m_i32.Create( 32, 32, ILC_COLOR32|ILC_MASK, 0, 16 ) ||
	m_i32.Create( 32, 32, ILC_COLOR24|ILC_MASK, 0, 16 ) ||
	m_i32.Create( 32, 32, ILC_COLOR16|ILC_MASK, 0, 16 );

	m_i48.Create( 48, 48, ILC_COLOR32|ILC_MASK, 0, 16 ) ||
	m_i48.Create( 48, 48, ILC_COLOR24|ILC_MASK, 0, 16 ) ||
	m_i48.Create( 48, 48, ILC_COLOR16|ILC_MASK, 0, 16 );

	// SHI_FILE = 0
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FILE, FALSE, LVSIL_SMALL), m_i16 ) == SHI_FILE );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FILE, FALSE, LVSIL_NORMAL), m_i32 ) == SHI_FILE );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FILE, FALSE, LVSIL_BIG), m_i48 ) == SHI_FILE );

	// SHI_EXECUTABLE = 1
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_EXECUTABLE, FALSE, LVSIL_SMALL), m_i16 ) == SHI_EXECUTABLE );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_EXECUTABLE, FALSE, LVSIL_NORMAL), m_i32 ) == SHI_EXECUTABLE );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_EXECUTABLE, FALSE, LVSIL_BIG), m_i48 ) == SHI_EXECUTABLE );

	// SHI_COMPUTER = 2
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_COMPUTER, FALSE, LVSIL_SMALL), m_i16 ) == SHI_COMPUTER );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_COMPUTER, FALSE, LVSIL_NORMAL), m_i32 ) == SHI_COMPUTER );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_COMPUTER, FALSE, LVSIL_BIG), m_i48 ) == SHI_COMPUTER );

	// SHI_FOLDER_CLOSED = 3
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FOLDER_CLOSED, FALSE, LVSIL_SMALL), m_i16 ) == SHI_FOLDER_CLOSED );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FOLDER_CLOSED, FALSE, LVSIL_NORMAL), m_i32 ) == SHI_FOLDER_CLOSED );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FOLDER_CLOSED, FALSE, LVSIL_BIG), m_i48 ) == SHI_FOLDER_CLOSED );

	// SHI_FOLDER_OPEN = 4
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FOLDER_OPEN, FALSE, LVSIL_SMALL), m_i16 ) == SHI_FOLDER_OPEN );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FOLDER_OPEN, FALSE, LVSIL_NORMAL), m_i32 ) == SHI_FOLDER_OPEN );
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_FOLDER_OPEN, FALSE, LVSIL_BIG), m_i48 ) == SHI_FOLDER_OPEN );

	// SHI_LOCKED = 5 (overlay)
	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_LOCKED, FALSE, LVSIL_SMALL), m_i16 ) == SHI_LOCKED );
//	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_LOCKED, FALSE, LVSIL_NORMAL), m_i32 ) == SHI_LOCKED );
//	VERIFY( AddIcon( CoolInterface.ExtractIcon( IDI_LOCKED, FALSE, LVSIL_BIG), m_i48 ) == SHI_LOCKED );
	m_i16.SetOverlayImage( SHI_LOCKED, SHI_O_LOCKED );
//	m_i32.SetOverlayImage( SHI_LOCKED, SHI_O_LOCKED );
//	m_i48.SetOverlayImage( SHI_LOCKED, SHI_O_LOCKED );

	m_m16.SetAt( _T(".exe"), SHI_EXECUTABLE );
	m_m32.SetAt( _T(".exe"), SHI_EXECUTABLE );
	m_m48.SetAt( _T(".exe"), SHI_EXECUTABLE );

	m_m16.SetAt( _T(".com"), SHI_EXECUTABLE );
	m_m32.SetAt( _T(".com"), SHI_EXECUTABLE );
	m_m48.SetAt( _T(".com"), SHI_EXECUTABLE );
}

//////////////////////////////////////////////////////////////////////
// CShellIcons get

int CShellIcons::Get(LPCTSTR pszFile, int nSize)
{
	if ( m_i16.m_hImageList == NULL )
		Clear();

	CImageList* pImage;
	CIconMap* pIndex;
	switch ( nSize )
	{
	case 16:
		pImage = &m_i16;
		pIndex = &m_m16;
		break;

	case 32:
		pImage = &m_i32;
		pIndex = &m_m32;
		break;

	case 48:
		pImage = &m_i48;
		pIndex = &m_m48;
		break;

	default:
		ASSERT( FALSE );
		return SHI_FILE;
	}

	CString strType = PathFindExtension( pszFile );
	if ( strType.IsEmpty() )
		return SHI_FILE;	// No extension

	strType.MakeLower();
	CString strFilename;
	LPCTSTR pszFilename = PathFindFileName( pszFile );
	if ( pszFilename != pszFile && ( strType == _T(".exe") ) )
	{
		strFilename = pszFilename;
		strFilename.MakeLower();
	}

	HICON hIcon = NULL;
	int nIndex = SHI_FILE;

	if ( strFilename.IsEmpty() )
	{
		if ( pIndex->Lookup( strType, nIndex ) )
			return nIndex;

		Lookup( strType,
			( ( nSize == 16 ) ? &hIcon : NULL ),
			( ( nSize == 32 ) ? &hIcon : NULL ), NULL, NULL,
			( ( nSize == 48 ) ? &hIcon : NULL ) );
	}
	else
	{
		if ( pIndex->Lookup( strFilename, nIndex ) )
			return nIndex;
	}

	if ( ! hIcon && ! strFilename.IsEmpty() )
	{
		LoadIcon( pszFile,
				( ( nSize == 16 ) ? &hIcon : NULL ),
				( ( nSize == 32 ) ? &hIcon : NULL ),
				( ( nSize == 48 ) ? &hIcon : NULL ) );

		if ( hIcon && Settings.General.LanguageRTL )
			hIcon = CreateMirroredIcon( hIcon );
	}

	if ( ! hIcon && ! strFilename.IsEmpty() )
	{
		if ( pIndex->Lookup( strType, nIndex ) )
		{
			pIndex->SetAt( strFilename, nIndex );
			return nIndex;
		}

		Lookup( strType,
			( ( nSize == 16 ) ? &hIcon : NULL ),
			( ( nSize == 32 ) ? &hIcon : NULL ), NULL, NULL,
			( ( nSize == 48 ) ? &hIcon : NULL ) );
	}

	if ( ! hIcon )
	{
		SHFILEINFO sfi = {};
		if ( SHGetFileInfo( pszFile, 0, &sfi, sizeof( SHFILEINFO ),
			( strFilename.IsEmpty() ? SHGFI_USEFILEATTRIBUTES : 0 ) |
			SHGFI_ICON | ( ( nSize == 16 ) ? SHGFI_SMALLICON : SHGFI_LARGEICON ) ) )
		{
			hIcon = Settings.General.LanguageRTL ? CreateMirroredIcon( sfi.hIcon ) : sfi.hIcon;
		}
	}

	nIndex = hIcon ? pImage->Add( hIcon ) : SHI_FILE;
	pIndex->SetAt( ( strFilename.IsEmpty() ? strType : strFilename ), nIndex );

	if ( hIcon ) DestroyIcon( hIcon );

	TRACE( _T("Got %dx%d icon %d for %s\n"),
		nSize, nSize, nIndex, ( strFilename.IsEmpty() ? strType : strFilename ) );

	return nIndex;
}


//////////////////////////////////////////////////////////////////////
// CShellIcons add icon

int CShellIcons::Add(HICON hIcon, int nSize)
{
	if ( m_i16.m_hImageList == NULL ) Clear();

	switch ( nSize )
	{
	case 16:
		return m_i16.Add( hIcon );
	case 32:
		return m_i32.Add( hIcon );
	case 48:
		return m_i48.Add( hIcon );
	default:
		ASSERT( FALSE );
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////
// CShellIcons icon extractor

HICON CShellIcons::ExtractIcon(int nIndex, int nSize)
{
	HICON hIcon;
	switch ( nSize )
	{
	case 16:
		hIcon = m_i16.ExtractIcon( nIndex );
		break;
	case 32:
		hIcon = m_i32.ExtractIcon( nIndex );
		break;
	case 48:
		hIcon = m_i48.ExtractIcon( nIndex );
		break;
	default:
		ASSERT( FALSE );
		return NULL;
	}

	if ( Settings.General.LanguageRTL )
		hIcon = CreateMirroredIcon( hIcon );

	return hIcon;
}

//////////////////////////////////////////////////////////////////////
// CShellIcons common type string

CString	CShellIcons::GetTypeString(LPCTSTR pszFile)
{
	CString strOutput;

	LPCTSTR pszType = _tcsrchr( pszFile, '.' );
	if ( ! pszType ) return strOutput;

	CString strName, strMime;
	Lookup( pszType, NULL, NULL, &strName, &strMime );

	if ( ! strName.IsEmpty() )
	{
		strOutput = strName;
		if ( ! strMime.IsEmpty() )
			strOutput += _T(" (") + strMime + _T(")");
	}
	else
	{
		strOutput = pszType + 1;
	}

	return strOutput;
}

//////////////////////////////////////////////////////////////////////
// CShellIcons lookup

BOOL CShellIcons::Lookup(LPCTSTR pszType, HICON* phSmallIcon, HICON* phLargeIcon, CString* psName, CString* psMIME, HICON* phHugeIcon)
{
	DWORD nType, nResult;
	TCHAR szResult[MAX_PATH];
	HKEY hKey, hSub;

	if ( phSmallIcon ) *phSmallIcon = NULL;
	if ( phLargeIcon ) *phLargeIcon = NULL;
	if ( phHugeIcon )  *phHugeIcon = NULL;
	if ( psName ) *psName = pszType + 1;
	if ( psMIME ) psMIME->Empty();

	if ( pszType == NULL || *pszType == 0 ) return FALSE;

	if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, pszType, 0, KEY_READ, &hKey ) != ERROR_SUCCESS ) return FALSE;

	if ( psMIME )
	{
		nResult = sizeof(TCHAR) * MAX_PATH; nType = REG_SZ;
		if ( RegQueryValueEx( hKey, _T("Content Type"), NULL, &nType, (LPBYTE)szResult, &nResult ) == ERROR_SUCCESS )
		{
			szResult[ nResult / sizeof(TCHAR) ] = 0;
			*psMIME = szResult;
		}
	}

	nResult = sizeof(TCHAR) * MAX_PATH; nType = REG_SZ;
	if ( RegQueryValueEx( hKey, _T(""), NULL, &nType, (LPBYTE)szResult, &nResult ) != ERROR_SUCCESS )
	{
		RegCloseKey( hKey );
		return FALSE;
	}

	RegCloseKey( hKey );
	szResult[ nResult / sizeof(TCHAR) ] = 0;

	if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, szResult, 0, KEY_READ, &hKey ) != ERROR_SUCCESS ) return FALSE;

	if ( psName )
	{
		nResult = sizeof(TCHAR) * MAX_PATH;
		nType = REG_SZ;
		if ( RegQueryValueEx( hKey, _T(""), NULL, &nType, (LPBYTE)szResult, &nResult ) == ERROR_SUCCESS )
		{
			szResult[ nResult / sizeof(TCHAR) ] = 0;
			*psName = szResult;
		}
	}

	if ( RegOpenKeyEx( hKey, _T("DefaultIcon"), 0, KEY_READ, &hSub ) != ERROR_SUCCESS )
	{
		if ( RegOpenKeyEx( hKey, _T("CurVer"), 0, KEY_READ, &hSub ) != ERROR_SUCCESS )
		{
			RegCloseKey( hKey );
			return FALSE;
		}
		nResult = sizeof(TCHAR) * MAX_PATH;
		if ( RegQueryValueEx( hSub, _T(""), NULL, &nType, (LPBYTE)szResult, &nResult ) != ERROR_SUCCESS )
		{
			RegCloseKey( hSub );
			RegCloseKey( hKey );
			return FALSE;
		}
		RegCloseKey( hSub );
		RegCloseKey( hKey );
		szResult[ nResult / sizeof(TCHAR) ] = 0;

		if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, szResult, 0, KEY_READ, &hKey ) != ERROR_SUCCESS )
			return FALSE;

		if ( psName )
		{
			nResult = sizeof(TCHAR) * MAX_PATH; nType = REG_SZ;
			if ( RegQueryValueEx( hKey, _T(""), NULL, &nType, (LPBYTE)szResult, &nResult ) == ERROR_SUCCESS )
			{
				szResult[ nResult / sizeof(TCHAR) ] = 0;
				*psName = szResult;
			}
		}

		if ( RegOpenKeyEx( hKey, _T("DefaultIcon"), 0, KEY_READ, &hSub ) != ERROR_SUCCESS )
		{
			RegCloseKey( hSub );
			RegCloseKey( hKey );
			return FALSE;
		}
	}

	nResult = sizeof(TCHAR) * MAX_PATH;
	nType = REG_SZ;
	if ( RegQueryValueEx( hSub, _T(""), NULL, &nType, (LPBYTE)szResult, &nResult ) != ERROR_SUCCESS )
	{
		RegCloseKey( hSub );
		RegCloseKey( hKey );
		return FALSE;
	}

	RegCloseKey( hSub );
	RegCloseKey( hKey );
	szResult[ nResult / sizeof(TCHAR) ] = 0;

	if ( ! LoadIcon( szResult, phSmallIcon, phLargeIcon, phHugeIcon ) )
		return FALSE;

	if ( Settings.General.LanguageRTL )
	{
		if ( phSmallIcon && *phSmallIcon )
			*phSmallIcon = CreateMirroredIcon( *phSmallIcon );
		if ( phLargeIcon && *phLargeIcon )
			*phLargeIcon = CreateMirroredIcon( *phLargeIcon );
		if ( phHugeIcon && *phHugeIcon )
			*phHugeIcon  = CreateMirroredIcon( *phHugeIcon );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CShellIcons attach image list to controls

void CShellIcons::AttachTo(CListCtrl* const pList, int nSize) const
{
	if ( nSize == 16 )
		pList->SetImageList( const_cast< CImageList* >( &m_i16 ), LVSIL_SMALL );
	else if ( nSize == 32 )
		pList->SetImageList( const_cast< CImageList* >( &m_i32 ), LVSIL_NORMAL );
	else if ( nSize == 48 )
		pList->SetImageList( const_cast< CImageList* >( &m_i48 ), LVSIL_BIG );
	else
		ASSERT( FALSE );
}

void CShellIcons::AttachTo(CTreeCtrl* const pTree) const
{
	pTree->SetImageList( const_cast< CImageList* >( &m_i16 ), LVSIL_NORMAL );
}

//////////////////////////////////////////////////////////////////////
// CShellIcons draw icon

BOOL CShellIcons::Draw(CDC* pDC, int nIcon, int nSize, int nX, int nY, COLORREF crBack, BOOL bSelected) const
{
	HIMAGELIST hImages;
	switch ( nSize )
	{
	case 16:
		hImages = m_i16.GetSafeHandle();
		break;
	case 32:
		hImages = m_i32.GetSafeHandle();
		break;
	case 48:
		hImages = m_i48.GetSafeHandle();
		break;
	default:
		ASSERT( FALSE );
		return FALSE;
	}

	return ImageList_DrawEx( hImages, nIcon, pDC->GetSafeHdc(),
		nX, nY, nSize, nSize, crBack,
		CLR_DEFAULT,	// bSelected ? Colors.m_crHighlight : CLR_NONE,
		bSelected ? ILD_SELECTED : ILD_NORMAL );
}

BOOL CShellIcons::Draw(CDC* pDC, int nIcon, int nSize, int nX, int nY, COLORREF crBack, COLORREF crBlend, UINT nStyle) const
{
	HIMAGELIST hImages;
	switch ( nSize )
	{
	case 16:
		hImages = m_i16.GetSafeHandle();
		break;
	case 32:
		hImages = m_i32.GetSafeHandle();
		break;
	case 48:
		hImages = m_i48.GetSafeHandle();
		break;
	default:
		ASSERT( FALSE );
		return FALSE;
	}

	return ImageList_DrawEx( hImages, nIcon, pDC->GetSafeHdc(),
		nX, nY, nSize, nSize, crBack, crBlend, nStyle );
}
