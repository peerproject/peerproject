//
// TextViewer.cpp
//
// This file is part of PeerProject (peerproject.org) © 2010
// TextViewer plugin is released under the Persistent Public Domain license.
//
// This code may be treated as Public Domain, provided:
// the work in all its forms and attendant uses shall remain available as
// persistently "Public Domain" until it naturally enters the public domain.
// History remains immutable:  Authors do not disclaim copyright, but do disclaim
// all rights beyond asserting the reach and duration and spirit of this license.

#include "StdAfx.h"
#include "Resource.h"
#include "TextViewer.h"

class CModule : public CAtlDllModuleT< CModule >
{
public :
	DECLARE_LIBID( LIBID_TextViewerLib )
};

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
#if defined(_MSC_VER) && (_MSC_VER >= 1500)	// No VS2005
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
