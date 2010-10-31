//
// Registry.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

//////////////////////////////////////////////////////////////////////
// CRegistry read a string value

CString CRegistry::GetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszDefault, LPCTSTR pszSubKey, BOOL bIgnoreHKCU)
{
	CString strSection( pszSubKey ? pszSubKey : _T("Software\\PeerProject\\PeerProject") );
	if ( pszSection && *pszSection )
	{
		strSection += _T("\\");
		strSection += pszSection;
	}

	// Read from HKCU then from HKLM
	DWORD nType = 0, nSize = 0;
	LONG nErrorCode = SHRegGetUSValue( (LPCTSTR)strSection, pszName, &nType,
		NULL, &nSize, bIgnoreHKCU, NULL, 0 );
	if ( nErrorCode == ERROR_SUCCESS && nType == REG_SZ && nSize >= sizeof( TCHAR ) &&
		( nSize & 1 ) == 0 )
	{
		CString strValue;
		nErrorCode = SHRegGetUSValue( (LPCTSTR)strSection, pszName, &nType,
			strValue.GetBuffer( nSize / sizeof( TCHAR ) ), &nSize, bIgnoreHKCU, NULL, 0 );
		strValue.ReleaseBuffer( nSize / sizeof( TCHAR ) - 1 );
		if ( nErrorCode == ERROR_SUCCESS )
			return strValue;
	}

	return pszDefault ? CString( pszDefault ) : CString();
}

//////////////////////////////////////////////////////////////////////
// CRegistry read an integer value

DWORD CRegistry::GetDword(LPCTSTR pszSection, LPCTSTR pszName, DWORD nDefault, LPCTSTR pszSubKey)
{
	CString strSection( pszSubKey ? pszSubKey : _T("Software\\PeerProject\\PeerProject") );
	if ( pszSection && *pszSection )
	{
		strSection += _T("\\");
		strSection += pszSection;
	}

	// Read from HKCU then from HKLM
	DWORD nValue = 0;
	DWORD nType = 0, nSize = sizeof( nValue );
	LONG nErrorCode = SHRegGetUSValue( (LPCTSTR)strSection, pszName, &nType,
		(PBYTE)&nValue, &nSize, FALSE, NULL, 0 );
	if ( nErrorCode == ERROR_SUCCESS && nType == REG_DWORD && nSize == sizeof( nValue ) )
	{
		return nValue;
	}

	return nDefault;
}

//////////////////////////////////////////////////////////////////////
// CRegistry write a string value

BOOL CRegistry::SetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszValue, LPCTSTR pszSubKey)
{
	CString strSection( pszSubKey ? pszSubKey : _T("Software\\PeerProject\\PeerProject") );
	if ( pszSection && *pszSection )
	{
		strSection += _T("\\");
		strSection += pszSection;
	}

	LONG nErrorCode = SHRegSetUSValue( (LPCTSTR)strSection, pszName, REG_SZ,
		(LPCVOID)pszValue, ( lstrlen( pszValue ) + 1 ) * sizeof( TCHAR ),
		SHREGSET_FORCE_HKCU );
	return ( nErrorCode == ERROR_SUCCESS );
}

//////////////////////////////////////////////////////////////////////
// CRegistry write an int value

BOOL CRegistry::SetDword(LPCTSTR pszSection, LPCTSTR pszName, DWORD nValue, LPCTSTR pszSubKey)
{
	CString strSection( pszSubKey ? pszSubKey : _T("Software\\PeerProject\\PeerProject") );
	if ( pszSection && *pszSection )
	{
		strSection += _T("\\");
		strSection += pszSection;
	}

	LONG nErrorCode = SHRegSetUSValue( (LPCTSTR)strSection, pszName, REG_DWORD,
		(LPCVOID)&nValue, sizeof( nValue ), SHREGSET_FORCE_HKCU );
	return ( nErrorCode == ERROR_SUCCESS );
}
