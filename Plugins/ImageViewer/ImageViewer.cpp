//
// ImageViewer.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Original author Michael Stokes released portions into the public domain.
// You are free to redistribute and modify this page without any restrictions.
//

#include "StdAfx.h"
#include "Resource.h"
#include "ImageViewer.h"

class CModule : public CAtlDllModuleT< CModule >
{
public :
	DECLARE_LIBID( LIBID_ImageViewerLib )
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
