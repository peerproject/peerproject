//
// DocumentReader.cpp
//
//	Date:			"$Date: $"
//	Revision:		"$Revision: 1.0 $"
//  Last change by:	"$Author: rolandas $"
//	Created by:		Rolandas Rudomanskis
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
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
#include "stdafx.h"
#include "Globals.h"
#include "resource.h"
#include "RatDVDReader.h"

////////////////////////////////////////////////////////////////////////
// Globals for this module.
//
HINSTANCE         v_hModule;             // DLL module handle
ULONG             v_cLocks;              // Count of server locks
CRITICAL_SECTION  v_csSynch;             // Critical Section
HANDLE            v_hPrivateHeap;        // Private Heap for Component
BOOL              v_fRunningOnNT;        // Flag Set When on Unicode OS

class CRatDVDReaderModule : public CAtlDllModuleT< CRatDVDReaderModule >
{
public :
	CRatDVDReaderModule();
	DECLARE_LIBID(LIBID_RatDVDReaderLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_RATDVDREADER, "{6E8D8F96-2C03-4fbf-8F7C-E92FF5C63C1E}")
	HRESULT DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
};

CRatDVDReaderModule _AtlModule;

CRatDVDReaderModule::CRatDVDReaderModule()
{
}

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		ODS(_T("DllMain - Attach\n"));

		v_hModule = hInstance; v_cLocks = 0;
		v_hPrivateHeap = HeapCreate(0, 0x1000, 0);
		v_fRunningOnNT = ( ( GetVersion() & 0x80000000 ) != 0x80000000 );
		InitializeCriticalSection( &v_csSynch );
		DisableThreadLibraryCalls( hInstance );
		break;

	case DLL_PROCESS_DETACH:
		ODS(_T("DllMain - Detach\n"));

		if ( v_hPrivateHeap ) HeapDestroy( v_hPrivateHeap );
        DeleteCriticalSection( &v_csSynch );
		break;
	}

    return _AtlModule.DllMain( dwReason, lpReserved ); 
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return ( _AtlModule.GetLockCount() == 0 ) ? S_OK : S_FALSE;
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	LPWSTR  pwszModule;

	// If we can't find the path to the DLL, we can't register...
	if (!FGetModuleFileName( v_hModule, &pwszModule) )
		return E_UNEXPECTED;

    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();

	return hr;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	LPWSTR  pwszModule;
	HRESULT hr;
	//If we can't find the path to the DLL, we can't unregister...
	if ( !FGetModuleFileName( v_hModule, &pwszModule) )
		return E_UNEXPECTED;

	hr = _AtlModule.DllUnregisterServer();
	return hr;
}
HRESULT CRatDVDReaderModule::DllGetClassObject(REFCLSID rclsid, REFIID /*riid*/, LPVOID* ppv)
{
	ODS(_T("CRatDVDReaderModule::DllGetClassObject\n"));

	HRESULT hr;

	CRatDVDClassFactory* pcf;

	CHECK_NULL_RETURN(ppv, E_POINTER);
	*ppv = NULL;

 // The only components we can create
	if ( rclsid != CLSID_RatDVDReader )
		return CLASS_E_CLASSNOTAVAILABLE;

 // Create the needed class factory...
	pcf = new CRatDVDClassFactory();
	CHECK_NULL_RETURN( pcf, E_OUTOFMEMORY );

 // Get requested interface.
	if ( SUCCEEDED(hr = pcf->QueryInterface(rclsid, ppv)) )
        { pcf->LockServer(TRUE); }
    else
        { *ppv = NULL; delete pcf; }

	return hr;
}
