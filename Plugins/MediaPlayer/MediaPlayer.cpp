//
// MediaPlayer.cpp : Implementation of DLL Exports.
//
// This file is part of PeerProject (peerproject.org) © 2008-2009
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
#include "MediaPlayer_h.h"
#include "MediaPlayer_i.c"

class CMediaPlayerModule : public CAtlDllModuleT< CMediaPlayerModule >
{
public :
	DECLARE_LIBID(LIBID_MediaPlayerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MEDIAPLAYER, "{7F669B06-74D9-42A9-A157-DD08EE5F30BA}")
};

extern class CMediaPlayerModule _AtlModule;

CMediaPlayerModule _AtlModule;

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

    if ( pszCmdLine && ! _wcsnicmp( pszCmdLine, szUserSwitch, _countof( szUserSwitch ) ) )
   		AtlSetPerUserRegistration(true);

    if ( bInstall )
    {	
    	hr = DllRegisterServer();
    	if ( FAILED( hr ) )
    		DllUnregisterServer();
    }
    else
    	hr = DllUnregisterServer();

    return hr;
}
