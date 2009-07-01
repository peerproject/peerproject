//
// RARBuilder.cpp : Implementation of DLL Exports.
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "stdafx.h"
#include "Resource.h"
#include "RARBuilder.h"

class CModule : public CAtlDllModuleT< CModule >
{
public:
	CModule();
	virtual ~CModule();
	DECLARE_LIBID( LIBID_RARBuilderLib )
	DECLARE_REGISTRY_APPID_RESOURCEID( IDR_APP, "{C2F61D04-BE35-4AE3-84A8-BE140613E26D}" )

protected:
	HMODULE	m_hUnrar;

	bool LoadUnrar();
	void UnloadUnrar();
};

CModule::CModule() :
	m_hUnrar( NULL )
{
	if ( ! LoadUnrar() )
		UnloadUnrar();
}

CModule::~CModule()
{
	UnloadUnrar();
}

bool CModule::LoadUnrar()
{
#ifdef _WIN64
	LPCTSTR szUnRAR = _T("Unrar64.dll");
#else
	LPCTSTR szUnRAR = _T("Unrar.dll");
#endif
	m_hUnrar = LoadLibrary( szUnRAR );
	if ( ! m_hUnrar )
	{
		TCHAR szPath[ MAX_PATH ] = {};
		GetModuleFileName( _AtlBaseModule.GetModuleInstance(), szPath, MAX_PATH );
		LPTSTR c = _tcsrchr( szPath, _T('\\') );
		if ( ! c )
			return false;
		lstrcpy( c + 1, szUnRAR );
		m_hUnrar = LoadLibrary( szPath );
		if ( ! m_hUnrar )
		{
			*c = _T('\0');
			c = _tcsrchr( szPath, _T('\\') );
			if ( ! c )
				return false;
			lstrcpy( c + 1, szUnRAR );
			m_hUnrar = LoadLibrary( szPath );
			if ( ! m_hUnrar )
				return false;
		}
	}
	fnRAROpenArchiveEx = (tRAROpenArchiveEx)GetProcAddress( m_hUnrar, "RAROpenArchiveEx");
	fnRARCloseArchive = (tRARCloseArchive)GetProcAddress( m_hUnrar, "RARCloseArchive");
	fnRARReadHeaderEx = (tRARReadHeaderEx)GetProcAddress( m_hUnrar, "RARReadHeaderEx");
	fnRARProcessFileW = (tRARProcessFileW)GetProcAddress( m_hUnrar, "RARProcessFileW");
	return ( fnRAROpenArchiveEx && fnRARCloseArchive && fnRARReadHeaderEx &&
		fnRARProcessFileW );
}

void CModule::UnloadUnrar()
{
	if ( m_hUnrar )
	{
		fnRAROpenArchiveEx = NULL;
		fnRARCloseArchive = NULL;
		fnRARReadHeaderEx = NULL;
		fnRARProcessFileW = NULL;
		FreeLibrary( m_hUnrar );
		m_hUnrar = NULL;
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
