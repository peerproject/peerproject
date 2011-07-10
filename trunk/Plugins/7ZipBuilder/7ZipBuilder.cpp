//
// 7ZipBuilder.cpp : Implementation of DLL Exports.
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions Copyright Shareaza Development Team, 2007.
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
#include "Resource.h"
#include "7ZipBuilder.h"

class CModule : public CAtlDllModuleT< CModule >
{
public :
	CModule();
	virtual ~CModule();
	DECLARE_LIBID( LIBID_SevenZipBuilderLib )
	DECLARE_REGISTRY_APPID_RESOURCEID( IDR_APP, "{C2D1E91C-5C0B-4F01-BA5A-447D1F28A53B}" )

protected:
	HMODULE	m_h7zx;

	bool Load7zx();
	void Unload7zx();
};

CModule::CModule() :
	m_h7zx( NULL )
{
	if ( ! Load7zx() )
		Unload7zx();
}

CModule::~CModule()
{
	Unload7zx();
}

bool CModule::Load7zx()
{
	LPCTSTR sz7zxa = _T("7zxa.dll");
	m_h7zx = LoadLibrary( sz7zxa );
	if ( ! m_h7zx )
	{
		TCHAR szPath[ MAX_PATH ] = {};
		GetModuleFileName( _AtlBaseModule.GetModuleInstance(), szPath, MAX_PATH );
		LPTSTR c = _tcsrchr( szPath, _T('\\') );
		if ( ! c )
			return false;
		lstrcpy( c + 1, sz7zxa );
		m_h7zx = LoadLibrary( szPath );
		if ( ! m_h7zx )
		{
			*c = _T('\0');
			c = _tcsrchr( szPath, _T('\\') );
			if ( ! c )
				return false;
			lstrcpy( c + 1, sz7zxa );
			m_h7zx = LoadLibrary( szPath );
			if ( ! m_h7zx )
				return false;
		}
	}
	fnCreateObject = (tCreateObject)GetProcAddress( m_h7zx, "CreateObject" );
	return ( fnCreateObject != NULL );
}

void CModule::Unload7zx()
{
	if ( m_h7zx )
	{
		fnCreateObject = NULL;
		FreeLibrary( m_h7zx );
		m_h7zx = NULL;
	}
}

CModule _AtlModule;

extern "C" BOOL WINAPI DllMain(HINSTANCE /*hInstance*/, DWORD dwReason, LPVOID lpReserved)
{
	return _AtlModule.DllMain( dwReason, lpReserved );
}

STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject( rclsid, riid, ppv );
}

STDAPI DllRegisterServer(void)
{
	return _AtlModule.DllRegisterServer();
}

STDAPI DllUnregisterServer(void)
{
	return _AtlModule.DllUnregisterServer();
}

STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if ( pszCmdLine != NULL )
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1500)		// No VS2005
		if ( _wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0 )
			AtlSetPerUserRegistration(true);
#endif
	}

	if ( bInstall )
	{
		hr = DllRegisterServer();
		if ( FAILED(hr) )
			DllUnregisterServer();
	}
	else
		hr = DllUnregisterServer();

	return hr;
}
