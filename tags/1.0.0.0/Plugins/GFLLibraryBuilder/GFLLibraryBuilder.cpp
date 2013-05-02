//
// GFLLibraryBuilder.cpp : Implementation of DLL Exports.
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Nikolay Raspopov, 2005.
//
// GFL Library, GFL SDK and XnView
// Copyright (c) 1991-2004 Pierre-E Gougelet
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
#include "GFLLibraryBuilder.h"

#define REG_LIBRARYBUILDER_KEY _T("Software\\PeerProject\\PeerProject\\Plugins\\LibraryBuilder")

class CGFLLibraryBuilderModule : public CAtlDllModuleT< CGFLLibraryBuilderModule >
{
public :
	DECLARE_LIBID(LIBID_GFLLibraryBuilderLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_GFLLIBRARYBUILDER, "{F74AD137-A43F-46FD-A1FE-6532C3FC3E88}")
};

CGFLLibraryBuilderModule _AtlModule;
HINSTANCE				_hModuleInstance = NULL;
typedef ATL::CAtlMap <ATL::CString, ATL::CString> CAtlStrStrMap;
CAtlStrStrMap			_ExtMap;

inline void FillExtMap()
{
	CString tmp;
	_ExtMap.RemoveAll();
	GFL_INT32 count = gflGetNumberOfFormat();
	ATLTRACE( "Total %d formats:\n", count );
	for (GFL_INT32 i = 0; i < count; ++i)
	{
		GFL_FORMAT_INFORMATION info;
		GFL_ERROR err = gflGetFormatInformationByIndex(i, &info);
		if (err == GFL_NO_ERROR && (info.Status & GFL_READ))
		{
			CString name (info.Name);
			CString desc (info.Description);
			ATLTRACE( "%3d. %7s %32s :", i, info.Name, info.Description );
			for (GFL_UINT32 j = 0; j < info.NumberOfExtension; ++j)
			{
				CString ext (info.Extension [j]);
				ext.MakeLower();
				ATLTRACE( " .%s", ext );
				if (!_ExtMap.Lookup(ext, tmp))
					_ExtMap.SetAt(ext, name);
			}
			ATLTRACE( "\n" );
		}
	}
}

BOOL SafeGFLInit() throw()
{
	__try
	{
		// Library initialization
		if ( gflLibraryInit() != GFL_NO_ERROR )
		{
			ATLTRACE( "gflLibraryInit failed\n" );
			return FALSE;
		}
		gflEnableLZW( GFL_TRUE );
		FillExtMap();
		return TRUE;
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		ATLTRACE( "Exception in DLL_PROCESS_ATTACH\n" );
		return FALSE;
	}
}

void SafeGFLExit() throw()
{
	__try
	{
		gflLibraryExit();
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		ATLTRACE( "Exception in DLL_PROCESS_DETACH\n" );
	}
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (_AtlModule.DllMain( dwReason, lpReserved ) )
	{
		switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			_hModuleInstance = hInstance;
			return SafeGFLInit();

		case DLL_PROCESS_DETACH:
			SafeGFLExit();
			break;
		}
		return TRUE;
	}
	else
	{
		ATLTRACE( "FALSE in _AtlModule.DllMain() call\n" );
		return FALSE;
	}
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
	HRESULT hr = _AtlModule.DllRegisterServer();

	// Registering extensions using GFL
	CString ext, tmp;
	POSITION pos = _ExtMap.GetStartPosition();
	while ( pos )
	{
		_ExtMap.GetNextAssoc(pos, ext, tmp);
		if ( ext == _T("pdf") || ext == _T("ps") || ext == _T("eps") || ext == _T("vst") ) continue;
		ext.Insert(0, _T('.'));
		ATLTRACE( "Add %s\n", CT2A(ext) );
		SHSetValue(HKEY_CURRENT_USER, REG_LIBRARYBUILDER_KEY, ext, REG_SZ,
			_T("{C937FE9E-FC47-49F8-A115-1925D95E1FE5}"),
			38 * sizeof(TCHAR));
	}

	return hr;
}

STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();

	// Unregistering extensions using GFL
	CString ext, tmp;
	POSITION pos = _ExtMap.GetStartPosition();
	while ( pos )
	{
		_ExtMap.GetNextAssoc(pos, ext, tmp);
		if ( ext == _T("pdf") || ext == _T("ps") || ext == _T("eps") || ext == _T("vst") ) continue;
		ext.Insert(0, _T('.'));
		ATLTRACE( "Remove %s\n", CT2A(ext) );
		SHDeleteValue(HKEY_CURRENT_USER, REG_LIBRARYBUILDER_KEY, ext);
	}

	return hr;
}

STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

#if defined(_MSC_VER) && (_MSC_VER >= 1500)	// No VS2005
	if ( pszCmdLine != NULL )
	{
		if ( _wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0 )
			AtlSetPerUserRegistration(true);
	}
#endif

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
