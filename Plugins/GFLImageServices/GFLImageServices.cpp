//
// GFLImageServices.cpp : Implementation of DLL Exports.
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
#include "GFLImageServices.h"	// Generated

#define REG_IMAGESERVICE_KEY _T("Software\\PeerProject\\PeerProject\\Plugins\\ImageService")

class CGFLImageServicesModule : public CAtlDllModuleT< CGFLImageServicesModule >
{
public :
	DECLARE_LIBID (LIBID_GFLImageServicesLib)
	DECLARE_REGISTRY_APPID_RESOURCEID (IDR_GFLIMAGESERVICES, "{4DD7500D-4ACC-4833-AB1D-887C59199DC5}")
};

CGFLImageServicesModule	_AtlModule;
HINSTANCE				_hModuleInstance = NULL;
typedef ATL::CAtlMap <ATL::CString, ATL::CString> CAtlStrStrMap;
CAtlStrStrMap			_ExtMap;

inline void FillExtMap()
{
	CString tmp;
	_ExtMap.RemoveAll();
	GFL_INT32 count = gflGetNumberOfFormat();
	ATLTRACE( "Total %d formats:\n", count );
	for ( GFL_INT32 i = 0; i < count; ++i )
	{
		GFL_FORMAT_INFORMATION info;
		GFL_ERROR err = gflGetFormatInformationByIndex (i, &info);
		if ( err == GFL_NO_ERROR && (info.Status & GFL_READ) )
		{
			CString name (info.Name);
			CString desc (info.Description);
			ATLTRACE( "%3d. %7s %32s :", i, name, desc );
			for ( GFL_UINT32 j = 0; j < info.NumberOfExtension; ++j )
			{
				CString ext (info.Extension [j]);
				ext = ext.MakeLower();
				ATLTRACE( " .%s", ext );
				if ( !_ExtMap.Lookup(ext, tmp) )
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
	while (pos)
	{
		_ExtMap.GetNextAssoc(pos, ext, tmp);
		if ( ext == _T("vst") ) continue;
		ext.Insert (0, _T('.'));
		ATLTRACE( "Add %s\n", ext);
		SHSetValue(HKEY_CURRENT_USER, REG_IMAGESERVICE_KEY, ext, REG_SZ,
			_T("{C9314782-CB91-40B8-B375-F631FF30C1C8}"),
			38 * sizeof (TCHAR));
	}

	return hr;
}

STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();

	// Unregistering extensions using GFL
	CString ext, tmp;
	POSITION pos = _ExtMap.GetStartPosition();
	while (pos)
	{
		_ExtMap.GetNextAssoc(pos, ext, tmp);
		if ( ext == _T("vst") ) continue;
		ext.Insert(0, _T('.'));
		ATLTRACE( "Remove %s\n", ext);
		SHDeleteValue(HKEY_CURRENT_USER, REG_IMAGESERVICE_KEY, ext);
	}

	return hr;
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


HRESULT SAFEgflLoadBitmap(const char * filename, GFL_BITMAP **bitmap, const GFL_LOAD_PARAMS *params, GFL_FILE_INFORMATION *info) throw()
{
	HRESULT hr = E_FAIL;
	__try
	{
		GFL_ERROR err = gflLoadBitmap(filename, bitmap, params, info);
		if (err == GFL_NO_ERROR)
			hr = S_OK;
		else
			ATLTRACE( "gflLoadBitmap() error : %s\n", CA2T(gflGetErrorString(err)));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		ATLTRACE( "gflLoadBitmap() exception\n" );
	}
	return hr;
}

HRESULT SAFEgflLoadBitmapFromMemory(const GFL_UINT8 * data, GFL_UINT32 data_length, GFL_BITMAP **bitmap, const GFL_LOAD_PARAMS *params, GFL_FILE_INFORMATION *info) throw()
{
	HRESULT hr = E_FAIL;
	__try
	{
		GFL_ERROR err = gflLoadBitmapFromMemory(data, data_length, bitmap, params, info);
		if (err == GFL_NO_ERROR)
			hr = S_OK;
		else
			ATLTRACE( "gflLoadBitmapFromMemory() error : %s\n", CA2T(gflGetErrorString(err)));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		ATLTRACE( "gflLoadBitmapFromMemory() exception\n" );
	}
	return hr;
}

HRESULT SAFEgflSaveBitmapIntoMemory(GFL_UINT8 ** data, GFL_UINT32 * data_length, const GFL_BITMAP *bitmap, const GFL_SAVE_PARAMS *params) throw()
{
	HRESULT hr = E_FAIL;
	__try
	{
		GFL_ERROR err = gflSaveBitmapIntoMemory(data, data_length, bitmap, params);
		if (err == GFL_NO_ERROR)
			hr = S_OK;
		else
			ATLTRACE( "gflSaveBitmapIntoMemory() error : %s\n", CA2T(gflGetErrorString(err)));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		ATLTRACE( "gflSaveBitmapIntoMemory() exception\n" );
	}
	return hr;
}

HRESULT SAFEgflSaveBitmap(char *filename, const GFL_BITMAP *bitmap, const GFL_SAVE_PARAMS *params) throw()
{
	HRESULT hr = E_FAIL;
	__try
	{
		GFL_ERROR err = gflSaveBitmap(filename, bitmap, params);
		if (err == GFL_NO_ERROR)
			hr = S_OK;
		else
			ATLTRACE( "gflSaveBitmap() error : %s\n", CA2T(gflGetErrorString(err)));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		ATLTRACE( "gflSaveBitmap() exception\n" );
	}
	return hr;
}

int GetFormatIndexByExt(LPCTSTR ext)
{
	CString name;
	if (_ExtMap.Lookup(ext, name))
		return gflGetFormatIndexByName(CT2CA(name));

	return -1;
}
