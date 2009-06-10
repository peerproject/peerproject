//
// WebHook.cpp : Implementation of DLL Exports.
//
// This file is part of PeerProject (peerproject.org) © 2009
// Portions Copyright Shareaza Development Team, 2009.
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
#include "WebHook_i.h"

class CWebHookModule : public CAtlDllModuleT< CWebHookModule >
{
public :
	DECLARE_LIBID(LIBID_WebHookLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WebHook, "{07D1F248-BCA2-4257-8863-C9ACCFBAD83D}")
};

CWebHookModule _AtlModule;

extern "C" BOOL WINAPI DllMain(HINSTANCE /*hInstance*/, DWORD dwReason, LPVOID lpReserved)
{
	// Black-listed processes
	if ( dwReason == DLL_PROCESS_ATTACH )
	{
		TCHAR szName[ MAX_PATH ] = {};
		DWORD dwLength = GetModuleFileName( NULL, szName, _countof( szName ) );

		// Windows Explorer
		if ( lstrcmpi( szName + dwLength - 13, _T("\\explorer.exe") ) == 0 )
			return FALSE;
	}

	return _AtlModule.DllMain( dwReason, lpReserved );
}

STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
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
    static const wchar_t szUserSwitch[] = _T("user");

    if (pszCmdLine != NULL)
    {
    	if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    	{
    		AtlSetPerUserRegistration(true);
    	}
    }

    if (bInstall)
    {
    	hr = DllRegisterServer();
    	if (FAILED(hr))
    	{
    		DllUnregisterServer();
    	}
    }
    else
    {
    	hr = DllUnregisterServer();
    }

    return hr;
}
