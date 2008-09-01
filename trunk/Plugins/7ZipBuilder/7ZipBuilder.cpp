//
// 7ZipBuilder.cpp : Implementation of DLL Exports.
//
// This file is part of PeerProject (peerproject.org) � 2008
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
	DECLARE_REGISTRY_APPID_RESOURCEID( IDR_APP, "{B69F80CD-FB15-45E8-B359-92A41CC571A7}" )

protected:
	HMODULE	m_h7zxr;

	bool Load7zxr();
	void Unload7zxr();
};

CModule::CModule() :
	m_h7zxr( NULL )
{
	if ( ! Load7zxr() )
		Unload7zxr();
}

CModule::~CModule()
{
	Unload7zxr();
}

bool CModule::Load7zxr()
{
	m_h7zxr = LoadLibrary( _T("7zxr.dll") );
	if ( ! m_h7zxr )
	{
		TCHAR szPath[ MAX_PATH ] = {};
		GetModuleFileName( _AtlBaseModule.GetModuleInstance(), szPath, MAX_PATH );
		LPTSTR c = _tcsrchr( szPath, _T('\\') );
		if ( ! c )
			return false;
		lstrcpy( c + 1, _T("7zxr.dll") );
		m_h7zxr = LoadLibrary( szPath );
		if ( ! m_h7zxr )
		{
			*c = _T('\0');
			c = _tcsrchr( szPath, _T('\\') );
			if ( ! c )
				return false;
			lstrcpy( c + 1, _T("7zxr.dll") );
			m_h7zxr = LoadLibrary( szPath );
			if ( ! m_h7zxr )
				return false;
		}
	}
	fnCreateObject = (tCreateObject)GetProcAddress( m_h7zxr, "CreateObject");
	return ( fnCreateObject != NULL );
}

void CModule::Unload7zxr()
{
	if ( m_h7zxr )
	{
		fnCreateObject = NULL;
		FreeLibrary( m_h7zxr );
		m_h7zxr = NULL;
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
