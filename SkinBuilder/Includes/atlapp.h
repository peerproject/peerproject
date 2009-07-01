// Windows Template Library - WTL version 8.0
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// This file is a part of the Windows Template Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/osi3.0/licenses/cpl1.0.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice,
// or any other, from this software.

// This file has been modified to remove extraneous content.

#ifndef __ATLAPP_H__
#define __ATLAPP_H__


#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
	#error atlapp.h requires atlbase.h to be included first
#endif

#ifdef _WIN32_WCE
	#error Windows CE is not supported by this package
#endif

#if _ATL_VER < 0x0700
	#error ATL versions under 7.0 are unsupported by this package
#endif

#ifdef _ATL_NO_COMMODULE
	#error WTL requires that _ATL_NO_COMMODULE is not defined
#endif

#include <limits.h>
#if !defined(_ATL_MIN_CRT) && defined(_MT)
  #include <process.h>	// for _beginthreadex
#endif

#if (_ATL_VER < 0x0800) && !defined(_DEBUG)
  #include <stdio.h>
#endif

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "atlres.h"


// Disable this warning for template class arguments
#pragma warning(disable: 4127)


///////////////////////////////////////////////////////////////////////////////
// WTL version number

#define _WTL_VER	0x0800


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CMessageFilter
// CIdleHandler
// CMessageLoop
//
// CAppModule
// CServerAppModule
//
// Global functions:
//   AtlGetDefaultGuiFont()
//   AtlCreateBoldFont()
//   AtlInitCommonControls()


///////////////////////////////////////////////////////////////////////////////
// No Global support for Windows CE

// #ifdef _WIN32_WCE


///////////////////////////////////////////////////////////////////////////////
// No Global support for using original VC++ 6.0 headers with WTL

// #ifndef _ATL_NO_OLD_HEADERS_WIN64


///////////////////////////////////////////////////////////////////////////////
// Global support for SecureHelper functions

#ifndef _TRUNCATE
  #define _TRUNCATE ((size_t)-1)
#endif

#ifndef _ERRCODE_DEFINED
  #define _ERRCODE_DEFINED
  typedef int errno_t;
#endif

#ifndef _SECURECRT_ERRCODE_VALUES_DEFINED
  #define _SECURECRT_ERRCODE_VALUES_DEFINED
  #define EINVAL          22
  #define STRUNCATE       80
#endif

#ifndef _countof
  #define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif


///////////////////////////////////////////////////////////////////////////////
// Miscellaneous global support

// define useful macros from winuser.h
#ifndef IS_INTRESOURCE
  #define IS_INTRESOURCE(_r) (((ULONG_PTR)(_r) >> 16) == 0)
#endif

// protect template members from windowsx.h macros
#ifdef _INC_WINDOWSX
  #undef SubclassWindow
#endif

// define useful macros from windowsx.h
#ifndef GET_X_LPARAM
  #define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
  #define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

// Dummy structs for compiling with /CLR
#if (_MSC_VER >= 1300) && defined(_MANAGED)
  __if_not_exists(_IMAGELIST::_IMAGELIST) { struct _IMAGELIST { }; }
  __if_not_exists(_TREEITEM::_TREEITEM) { struct _TREEITEM { }; }
  __if_not_exists(_PSP::_PSP) { struct _PSP { }; }
#endif

// No macro for ATL3

namespace WTL
{
  DECLARE_TRACE_CATEGORY(atlTraceUI);
  #ifdef _DEBUG
    __declspec(selectany) ATL::CTraceCategory atlTraceUI(_T("atlTraceUI"));
  #endif // _DEBUG

// Windows version helper
inline bool AtlIsOldWindows()
{
	OSVERSIONINFO ovi = { 0 };
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bRet = ::GetVersionEx(&ovi);
	return (!bRet || !((ovi.dwMajorVersion >= 5) || (ovi.dwMajorVersion == 4 && ovi.dwMinorVersion >= 90)));
}

// default GUI font helper
inline HFONT AtlGetDefaultGuiFont()
{
	return (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
}

// bold font helper (NOTE: Caller owns the font, and should destroy it when done using it)
inline HFONT AtlCreateBoldFont(HFONT hFont = NULL)
{
	if(hFont == NULL)
		hFont = AtlGetDefaultGuiFont();
	ATLASSERT(hFont != NULL);
	HFONT hFontBold = NULL;
	LOGFONT lf = { 0 };
	if(::GetObject(hFont, sizeof(LOGFONT), &lf) == sizeof(LOGFONT))
	{
		lf.lfWeight = FW_BOLD;
		hFontBold =  ::CreateFontIndirect(&lf);
		ATLASSERT(hFontBold != NULL);
	}
	else
	{
		ATLASSERT(FALSE);
	}
	return hFontBold;
}

// Common Controls initialization helper
inline BOOL AtlInitCommonControls(DWORD dwFlags)
{
	INITCOMMONCONTROLSEX iccx = { sizeof(INITCOMMONCONTROLSEX), dwFlags };
	BOOL bRet = ::InitCommonControlsEx(&iccx);
	ATLASSERT(bRet);
	return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// RunTimeHelper - helper functions for Windows version and structure sizes

#ifndef _WTL_NO_RUNTIME_STRUCT_SIZE

#ifndef _SIZEOF_STRUCT
  #define _SIZEOF_STRUCT(structname, member)  (((int)((LPBYTE)(&((structname*)0)->member) - ((LPBYTE)((structname*)0)))) + sizeof(((structname*)0)->member))
#endif

#if (_WIN32_WINNT >= 0x0600) && !defined(REBARBANDINFO_V6_SIZE)
  #define REBARBANDINFO_V6_SIZE   _SIZEOF_STRUCT(REBARBANDINFO, cxHeader)
#endif

#if (_WIN32_WINNT >= 0x0600) && !defined(LVGROUP_V5_SIZE)
  #define LVGROUP_V5_SIZE   _SIZEOF_STRUCT(LVGROUP, uAlign)
#endif

#if (_WIN32_WINNT >= 0x0600) && !defined(LVTILEINFO_V5_SIZE)
  #define LVTILEINFO_V5_SIZE   _SIZEOF_STRUCT(LVTILEINFO, puColumns)
#endif 

#if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN) && !defined(MCHITTESTINFO_V1_SIZE)
  #define MCHITTESTINFO_V1_SIZE   _SIZEOF_STRUCT(MCHITTESTINFO, st)
#endif

#if (WINVER >= 0x0600) && !defined(NONCLIENTMETRICS_V1_SIZE)
  #define NONCLIENTMETRICS_V1_SIZE   _SIZEOF_STRUCT(NONCLIENTMETRICS, lfMessageFont)
#endif

#endif // !_WTL_NO_RUNTIME_STRUCT_SIZE

namespace RunTimeHelper
{
	inline bool IsCommCtrl6()
	{
		DWORD dwMajor = 0, dwMinor = 0;
		HRESULT hRet = ATL::AtlGetCommCtrlVersion(&dwMajor, &dwMinor);
		return (SUCCEEDED(hRet) && (dwMajor >= 6));
	}

	inline bool IsVista()
	{
		OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
		BOOL bRet = ::GetVersionEx(&ovi);
		return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6));
	}

	inline int SizeOf_REBARBANDINFO()
	{
		int nSize = sizeof(REBARBANDINFO);
#if !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && (_WIN32_WINNT >= 0x0600)
		if(!(IsVista() && IsCommCtrl6()))
			nSize = REBARBANDINFO_V6_SIZE;
#endif // !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && (_WIN32_WINNT >= 0x0600)
		return nSize;
	}

#if (_WIN32_WINNT >= 0x501)
  	inline int SizeOf_LVGROUP()
	{
		int nSize = sizeof(LVGROUP);
#if !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && (_WIN32_WINNT >= 0x0600)
		if(!IsVista())
			nSize = LVGROUP_V5_SIZE;
#endif // !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && (_WIN32_WINNT >= 0x0600)
		return nSize;
	}

	inline int SizeOf_LVTILEINFO()
	{
		int nSize = sizeof(LVTILEINFO);
#if !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && (_WIN32_WINNT >= 0x0600)
		if(!IsVista())
			nSize = LVTILEINFO_V5_SIZE;
#endif // !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && (_WIN32_WINNT >= 0x0600)
		return nSize;
	}
#endif // (_WIN32_WINNT >= 0x501)

	inline int SizeOf_MCHITTESTINFO()
	{
		int nSize = sizeof(MCHITTESTINFO);
#if !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)
		if(!(IsVista() && IsCommCtrl6()))
			nSize = MCHITTESTINFO_V1_SIZE;
#endif // !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)
		return nSize;
	}

	inline int SizeOf_NONCLIENTMETRICS()
	{
		int nSize = sizeof(NONCLIENTMETRICS);
#if !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && (WINVER >= 0x0600)
		if(!IsVista())
			nSize = NONCLIENTMETRICS_V1_SIZE;
#endif // !defined(_WTL_NO_RUNTIME_STRUCT_SIZE) && (WINVER >= 0x0600)
		return nSize;
	}
};


///////////////////////////////////////////////////////////////////////////////
// ModuleHelper - helper functions for ATL3 and ATL7 module classes

namespace ModuleHelper
{
	inline HINSTANCE GetModuleInstance()
	{
		return ATL::_AtlBaseModule.GetModuleInstance();
	}

	inline HINSTANCE GetResourceInstance()
	{
		return ATL::_AtlBaseModule.GetResourceInstance();
	}

	inline void AddCreateWndData(ATL::_AtlCreateWndData* pData, void* pObject)
	{
		ATL::_AtlWinModule.AddCreateWndData(pData, pObject);
	}

	inline void* ExtractCreateWndData()
	{
		return ATL::_AtlWinModule.ExtractCreateWndData();
	}
};


///////////////////////////////////////////////////////////////////////////////
// SecureHelper - helper functions for VS2005 secure CRT

namespace SecureHelper
{
	inline void strcpyA_x(char* lpstrDest, size_t cchDest, const char* lpstrSrc)
	{
#if _SECURE_ATL
		ATL::Checked::strcpy_s(lpstrDest, cchDest, lpstrSrc);
#else
		if(cchDest > (size_t)lstrlenA(lpstrSrc))
			ATLVERIFY(lstrcpyA(lpstrDest, lpstrSrc) != NULL);
		else
			ATLASSERT(FALSE);
#endif
	}

	inline void strcpyW_x(wchar_t* lpstrDest, size_t cchDest, const wchar_t* lpstrSrc)
	{
#if _SECURE_ATL
		ATL::Checked::wcscpy_s(lpstrDest, cchDest, lpstrSrc);
#else
		if(cchDest > (size_t)lstrlenW(lpstrSrc))
			ATLVERIFY(lstrcpyW(lpstrDest, lpstrSrc) != NULL);
		else
			ATLASSERT(FALSE);
#endif
	}

	inline void strcpy_x(LPTSTR lpstrDest, size_t cchDest, LPCTSTR lpstrSrc)
	{
#ifdef _UNICODE
		strcpyW_x(lpstrDest, cchDest, lpstrSrc);
#else
		strcpyA_x(lpstrDest, cchDest, lpstrSrc);
#endif
	}

	inline errno_t strncpyA_x(char* lpstrDest, size_t cchDest, const char* lpstrSrc, size_t cchCount)
	{
#if _SECURE_ATL
		return ATL::Checked::strncpy_s(lpstrDest, cchDest, lpstrSrc, cchCount);
#else
		errno_t nRet = 0;
		if(lpstrDest == NULL || cchDest == 0 || lpstrSrc == NULL)
		{
			nRet = EINVAL;
		}
		else if(cchCount == _TRUNCATE)
		{
			cchCount = min(cchDest - 1, size_t(lstrlenA(lpstrSrc)));
			nRet = STRUNCATE;
		}
		else if(cchDest <= cchCount)
		{
			lpstrDest[0] = 0;
			nRet = EINVAL;
		}
		if(nRet == 0 || nRet == STRUNCATE)
			nRet = (lstrcpynA(lpstrDest, lpstrSrc, (int)cchCount + 1) != NULL) ? nRet : EINVAL;
		ATLASSERT(nRet == 0 || nRet == STRUNCATE);
		return nRet;
#endif
	}

	inline errno_t strncpyW_x(wchar_t* lpstrDest, size_t cchDest, const wchar_t* lpstrSrc, size_t cchCount)
	{
#if _SECURE_ATL
		return ATL::Checked::wcsncpy_s(lpstrDest, cchDest, lpstrSrc, cchCount);
#else
		errno_t nRet = 0;
		if(lpstrDest == NULL || cchDest == 0 || lpstrSrc == NULL)
		{
			nRet = EINVAL;
		}
		else if(cchCount == _TRUNCATE)
		{
			cchCount = min(cchDest - 1, size_t(lstrlenW(lpstrSrc)));
			nRet = STRUNCATE;
		}
		else if(cchDest <= cchCount)
		{
			lpstrDest[0] = 0;
			nRet = EINVAL;
		}
		if(nRet == 0 || nRet == STRUNCATE)
			nRet = (lstrcpynW(lpstrDest, lpstrSrc, (int)cchCount + 1) != NULL) ? nRet : EINVAL;
		ATLASSERT(nRet == 0 || nRet == STRUNCATE);
		return nRet;
#endif
	}

	inline errno_t strncpy_x(LPTSTR lpstrDest, size_t cchDest, LPCTSTR lpstrSrc, size_t cchCount)
	{
#ifdef _UNICODE
		return strncpyW_x(lpstrDest, cchDest, lpstrSrc, cchCount);
#else
		return strncpyA_x(lpstrDest, cchDest, lpstrSrc, cchCount);
#endif
	}

	inline void strcatA_x(char* lpstrDest, size_t cchDest, const char* lpstrSrc)
	{
#if _SECURE_ATL
		ATL::Checked::strcat_s(lpstrDest, cchDest, lpstrSrc);
#else
		if(cchDest > (size_t)lstrlenA(lpstrSrc))
			ATLVERIFY(lstrcatA(lpstrDest, lpstrSrc) != NULL);
		else
			ATLASSERT(FALSE);
#endif
	}

	inline void strcatW_x(wchar_t* lpstrDest, size_t cchDest, const wchar_t* lpstrSrc)
	{
#if _SECURE_ATL
		ATL::Checked::wcscat_s(lpstrDest, cchDest, lpstrSrc);
#else
		if(cchDest > (size_t)lstrlenW(lpstrSrc))
			ATLVERIFY(lstrcatW(lpstrDest, lpstrSrc) != NULL);
		else
			ATLASSERT(FALSE);
#endif
	}

	inline void strcat_x(LPTSTR lpstrDest, size_t cchDest, LPCTSTR lpstrSrc)
	{
#ifdef _UNICODE
		strcatW_x(lpstrDest, cchDest, lpstrSrc);
#else
		strcatA_x(lpstrDest, cchDest, lpstrSrc);
#endif
	}

	inline void memcpy_x(void* pDest, size_t cbDest, const void* pSrc, size_t cbSrc)
	{
#if _SECURE_ATL
		ATL::Checked::memcpy_s(pDest, cbDest, pSrc, cbSrc);
#else
		if(cbDest >= cbSrc)
			memcpy(pDest, pSrc, cbSrc);
		else
			ATLASSERT(FALSE);
#endif
	}

	inline void memmove_x(void* pDest, size_t cbDest, const void* pSrc, size_t cbSrc)
	{
#if _SECURE_ATL
		ATL::Checked::memmove_s(pDest, cbDest, pSrc, cbSrc);
#else
		if(cbDest >= cbSrc)
			memmove(pDest, pSrc, cbSrc);
		else
			ATLASSERT(FALSE);
#endif
	}

	inline int vsprintf_x(LPTSTR lpstrBuff, size_t cchBuff, LPCTSTR lpstrFormat, va_list args)
	{
#if !defined(_ATL_MIN_CRT) // && _SECURE_ATL
		return _vstprintf_s(lpstrBuff, cchBuff, lpstrFormat, args);
#else
		cchBuff;   // Avoid unused argument warning
		return _vstprintf(lpstrBuff, lpstrFormat, args);
#endif
	}

	inline int wvsprintf_x(LPTSTR lpstrBuff, size_t cchBuff, LPCTSTR lpstrFormat, va_list args)
	{
#if _SECURE_ATL && !defined(_ATL_MIN_CRT)
		return _vstprintf_s(lpstrBuff, cchBuff, lpstrFormat, args);
#else
		cchBuff;   // Avoid unused argument warning
		return ::wvsprintf(lpstrBuff, lpstrFormat, args);
#endif
	}

	inline int sprintf_x(LPTSTR lpstrBuff, size_t cchBuff, LPCTSTR lpstrFormat, ...)
	{
		va_list args;
		va_start(args, lpstrFormat);
		int nRes = vsprintf_x(lpstrBuff, cchBuff, lpstrFormat, args);
		va_end(args);
		return nRes;
	}

	inline int wsprintf_x(LPTSTR lpstrBuff, size_t cchBuff, LPCTSTR lpstrFormat, ...)
	{
		va_list args;
		va_start(args, lpstrFormat);
		int nRes = wvsprintf_x(lpstrBuff, cchBuff, lpstrFormat, args);
		va_end(args);
		return nRes;
	}
}; // namespace SecureHelper


///////////////////////////////////////////////////////////////////////////////
// CMessageFilter - Interface for message filter support

class CMessageFilter
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
};


///////////////////////////////////////////////////////////////////////////////
// CIdleHandler - Interface for idle processing

class CIdleHandler
{
public:
	virtual BOOL OnIdle() = 0;
};

#ifndef _ATL_NO_OLD_NAMES
  // for compatilibility with old names only
  typedef CIdleHandler CUpdateUIObject;
  #define DoUpdate OnIdle
#endif // !_ATL_NO_OLD_NAMES


///////////////////////////////////////////////////////////////////////////////
// CMessageLoop - message loop implementation

class CMessageLoop
{
public:
	ATL::CSimpleArray<CMessageFilter*> m_aMsgFilter;
	ATL::CSimpleArray<CIdleHandler*> m_aIdleHandler;
	MSG m_msg;

// Message filter operations
	BOOL AddMessageFilter(CMessageFilter* pMessageFilter)
	{
		return m_aMsgFilter.Add(pMessageFilter);
	}

	BOOL RemoveMessageFilter(CMessageFilter* pMessageFilter)
	{
		return m_aMsgFilter.Remove(pMessageFilter);
	}

// Idle handler operations
	BOOL AddIdleHandler(CIdleHandler* pIdleHandler)
	{
		return m_aIdleHandler.Add(pIdleHandler);
	}

	BOOL RemoveIdleHandler(CIdleHandler* pIdleHandler)
	{
		return m_aIdleHandler.Remove(pIdleHandler);
	}

#ifndef _ATL_NO_OLD_NAMES
	// for compatilibility with old names only
	BOOL AddUpdateUI(CIdleHandler* pIdleHandler)
	{
		ATLTRACE2(atlTraceUI, 0, _T("CUpdateUIObject and AddUpdateUI are deprecated. Please change your code to use CIdleHandler and OnIdle\n"));
		return AddIdleHandler(pIdleHandler);
	}

	BOOL RemoveUpdateUI(CIdleHandler* pIdleHandler)
	{
		ATLTRACE2(atlTraceUI, 0, _T("CUpdateUIObject and RemoveUpdateUI are deprecated. Please change your code to use CIdleHandler and OnIdle\n"));
		return RemoveIdleHandler(pIdleHandler);
	}
#endif // !_ATL_NO_OLD_NAMES

// message loop
	int Run()
	{
		BOOL bDoIdle = TRUE;
		int nIdleCount = 0;
		BOOL bRet;

		for(;;)
		{
			while(bDoIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if(!OnIdle(nIdleCount++))
					bDoIdle = FALSE;
			}

			bRet = ::GetMessage(&m_msg, NULL, 0, 0);

			if(bRet == -1)
			{
				ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
				continue;   // error, don't process
			}
			else if(!bRet)
			{
				ATLTRACE2(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
				break;   // WM_QUIT, exit message loop
			}

			if(!PreTranslateMessage(&m_msg))
			{
				::TranslateMessage(&m_msg);
				::DispatchMessage(&m_msg);
			}

			if(IsIdleMessage(&m_msg))
			{
				bDoIdle = TRUE;
				nIdleCount = 0;
			}
		}

		return (int)m_msg.wParam;
	}

	static BOOL IsIdleMessage(MSG* pMsg)
	{
		// These messages should NOT cause idle processing
		switch(pMsg->message)
		{
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		case WM_PAINT:
		case 0x0118:	// WM_SYSTIMER (caret blink)
			return FALSE;
		}

		return TRUE;
	}

// Overrideables
	// Override to change message filtering
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		// loop backwards
		for(int i = m_aMsgFilter.GetSize() - 1; i >= 0; i--)
		{
			CMessageFilter* pMessageFilter = m_aMsgFilter[i];
			if(pMessageFilter != NULL && pMessageFilter->PreTranslateMessage(pMsg))
				return TRUE;
		}
		return FALSE;   // not translated
	}

	// override to change idle processing
	virtual BOOL OnIdle(int /*nIdleCount*/)
	{
		for(int i = 0; i < m_aIdleHandler.GetSize(); i++)
		{
			CIdleHandler* pIdleHandler = m_aIdleHandler[i];
			if(pIdleHandler != NULL)
				pIdleHandler->OnIdle();
		}
		return FALSE;   // don't continue
	}
};


///////////////////////////////////////////////////////////////////////////////
// CStaticDataInitCriticalSectionLock and CWindowCreateCriticalSectionLock
// internal classes to manage critical sections for both ATL3 and ATL7

class CStaticDataInitCriticalSectionLock
{
public:
	ATL::CComCritSecLock<ATL::CComCriticalSection> m_cslock;

	CStaticDataInitCriticalSectionLock() : m_cslock(ATL::_pAtlModule->m_csStaticDataInitAndTypeInfo, false)
	{ }

	HRESULT Lock()
	{
		return m_cslock.Lock();
	}

	void Unlock()
	{
		m_cslock.Unlock();
	}
};


class CWindowCreateCriticalSectionLock
{
public:
	ATL::CComCritSecLock<ATL::CComCriticalSection> m_cslock;

	CWindowCreateCriticalSectionLock() : m_cslock(ATL::_AtlWinModule.m_csWindowCreate, false)
	{ }

	HRESULT Lock()
	{
		return m_cslock.Lock();
	}

	void Unlock()
	{
		m_cslock.Unlock();
	}
};


///////////////////////////////////////////////////////////////////////////////
// CTempBuffer - helper class for stack allocations for ATL3

#ifndef _WTL_STACK_ALLOC_THRESHOLD
  #define _WTL_STACK_ALLOC_THRESHOLD   512
#endif

using ATL::CTempBuffer;


///////////////////////////////////////////////////////////////////////////////
// CAppModule - module class for an application

class CAppModule : public ATL::CComModule
{
public:
	DWORD m_dwMainThreadID;
	ATL::CSimpleMap<DWORD, CMessageLoop*>* m_pMsgLoopMap;
	ATL::CSimpleArray<HWND>* m_pSettingChangeNotify;

// Overrides of CComModule::Init and Term
	HRESULT Init(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL)
	{
		HRESULT hRet = CComModule::Init(pObjMap, hInstance, pLibID);
		if(FAILED(hRet))
			return hRet;

		m_dwMainThreadID = ::GetCurrentThreadId();
		typedef ATL::CSimpleMap<DWORD, CMessageLoop*>   _mapClass;
		m_pMsgLoopMap = NULL;
		ATLTRY(m_pMsgLoopMap = new _mapClass);
		if(m_pMsgLoopMap == NULL)
			return E_OUTOFMEMORY;
		m_pSettingChangeNotify = NULL;

		return hRet;
	}

	void Term()
	{
		TermSettingChangeNotify();
		delete m_pMsgLoopMap;
		CComModule::Term();
	}

// Message loop map methods
	BOOL AddMessageLoop(CMessageLoop* pMsgLoop)
	{
		CStaticDataInitCriticalSectionLock lock;
		if(FAILED(lock.Lock()))
		{
			ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::AddMessageLoop.\n"));
			ATLASSERT(FALSE);
			return FALSE;
		}

		ATLASSERT(pMsgLoop != NULL);
		ATLASSERT(m_pMsgLoopMap->Lookup(::GetCurrentThreadId()) == NULL);   // not in map yet

		BOOL bRet = m_pMsgLoopMap->Add(::GetCurrentThreadId(), pMsgLoop);

		lock.Unlock();

		return bRet;
	}

	BOOL RemoveMessageLoop()
	{
		CStaticDataInitCriticalSectionLock lock;
		if(FAILED(lock.Lock()))
		{
			ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::RemoveMessageLoop.\n"));
			ATLASSERT(FALSE);
			return FALSE;
		}

		BOOL bRet = m_pMsgLoopMap->Remove(::GetCurrentThreadId());

		lock.Unlock();

		return bRet;
	}

	CMessageLoop* GetMessageLoop(DWORD dwThreadID = ::GetCurrentThreadId()) const
	{
		CStaticDataInitCriticalSectionLock lock;
		if(FAILED(lock.Lock()))
		{
			ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::GetMessageLoop.\n"));
			ATLASSERT(FALSE);
			return NULL;
		}

		CMessageLoop* pLoop =  m_pMsgLoopMap->Lookup(dwThreadID);

		lock.Unlock();

		return pLoop;
	}

// Setting change notify methods
	// Note: Call this from the main thread for MSDI apps
	BOOL InitSettingChangeNotify(DLGPROC pfnDlgProc = _SettingChangeDlgProc)
	{
		CStaticDataInitCriticalSectionLock lock;
		if(FAILED(lock.Lock()))
		{
			ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::InitSettingChangeNotify.\n"));
			ATLASSERT(FALSE);
			return FALSE;
		}

		if(m_pSettingChangeNotify == NULL)
		{
			typedef ATL::CSimpleArray<HWND>   _notifyClass;
			ATLTRY(m_pSettingChangeNotify = new _notifyClass);
			ATLASSERT(m_pSettingChangeNotify != NULL);
		}

		BOOL bRet = (m_pSettingChangeNotify != NULL);
		if(bRet && m_pSettingChangeNotify->GetSize() == 0)
		{
			// init everything
			_ATL_EMPTY_DLGTEMPLATE templ;
			HWND hNtfWnd = ::CreateDialogIndirect(GetModuleInstance(), &templ, NULL, pfnDlgProc);
			ATLASSERT(::IsWindow(hNtfWnd));
			if(::IsWindow(hNtfWnd))
			{
// Need conditional code because types don't match in winuser.h
#ifdef _WIN64
				::SetWindowLongPtr(hNtfWnd, GWLP_USERDATA, (LONG_PTR)this);
#else
				::SetWindowLongPtr(hNtfWnd, GWLP_USERDATA, PtrToLong(this));
#endif
				bRet = m_pSettingChangeNotify->Add(hNtfWnd);
			}
			else
			{
				bRet = FALSE;
			}
		}

		lock.Unlock();

		return bRet;
	}

	void TermSettingChangeNotify()
	{
		CStaticDataInitCriticalSectionLock lock;
		if(FAILED(lock.Lock()))
		{
			ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::TermSettingChangeNotify.\n"));
			ATLASSERT(FALSE);
			return;
		}

		if(m_pSettingChangeNotify != NULL && m_pSettingChangeNotify->GetSize() > 0)
			::DestroyWindow((*m_pSettingChangeNotify)[0]);
		delete m_pSettingChangeNotify;
		m_pSettingChangeNotify = NULL;

		lock.Unlock();
	}

	BOOL AddSettingChangeNotify(HWND hWnd)
	{
		CStaticDataInitCriticalSectionLock lock;
		if(FAILED(lock.Lock()))
		{
			ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::AddSettingChangeNotify.\n"));
			ATLASSERT(FALSE);
			return FALSE;
		}

		ATLASSERT(::IsWindow(hWnd));
		BOOL bRet = FALSE;
		if(InitSettingChangeNotify() != FALSE)
			bRet = m_pSettingChangeNotify->Add(hWnd);

		lock.Unlock();

		return bRet;
	}

	BOOL RemoveSettingChangeNotify(HWND hWnd)
	{
		CStaticDataInitCriticalSectionLock lock;
		if(FAILED(lock.Lock()))
		{
			ATLTRACE2(atlTraceUI, 0, _T("ERROR : Unable to lock critical section in CAppModule::RemoveSettingChangeNotify.\n"));
			ATLASSERT(FALSE);
			return FALSE;
		}

		BOOL bRet = FALSE;
		if(m_pSettingChangeNotify != NULL)
			bRet = m_pSettingChangeNotify->Remove(hWnd);

		lock.Unlock();

		return bRet;
	}

// Implementation - setting change notify dialog template and dialog procedure
	struct _ATL_EMPTY_DLGTEMPLATE : DLGTEMPLATE
	{
		_ATL_EMPTY_DLGTEMPLATE()
		{
			memset(this, 0, sizeof(_ATL_EMPTY_DLGTEMPLATE));
			style = WS_POPUP;
		}
		WORD wMenu, wClass, wTitle;
	};

#ifdef _WIN64
	static INT_PTR CALLBACK _SettingChangeDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
#else
	static BOOL CALLBACK _SettingChangeDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
#endif
	{
		if(uMsg == WM_SETTINGCHANGE)
		{
// Need conditional code because types don't match in winuser.h
#ifdef _WIN64
			CAppModule* pModule = (CAppModule*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
#else
			CAppModule* pModule = (CAppModule*)LongToPtr(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
#endif
			ATLASSERT(pModule != NULL);
			ATLASSERT(pModule->m_pSettingChangeNotify != NULL);
			const UINT uTimeout = 1500;   // ms
			for(int i = 1; i < pModule->m_pSettingChangeNotify->GetSize(); i++)
			{
				::SendMessageTimeout((*pModule->m_pSettingChangeNotify)[i], uMsg, wParam, lParam, SMTO_ABORTIFHUNG, uTimeout, NULL);
			}
			return TRUE;
		}
		return FALSE;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CServerAppModule - module class for a COM server application

class CServerAppModule : public CAppModule
{
public:
	HANDLE m_hEventShutdown;
	bool m_bActivity;
	DWORD m_dwTimeOut;
	DWORD m_dwPause;

// Override of CAppModule::Init
	HRESULT Init(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL)
	{
		m_dwTimeOut = 5000;
		m_dwPause = 1000;
		return CAppModule::Init(pObjMap, hInstance, pLibID);
	}

	void Term()
	{
		if(m_hEventShutdown != NULL && ::CloseHandle(m_hEventShutdown))
			m_hEventShutdown = NULL;
		CAppModule::Term();
	}

// COM Server methods
	LONG Unlock()
	{
		LONG lRet = CComModule::Unlock();
		if(lRet == 0)
		{
			m_bActivity = true;
			::SetEvent(m_hEventShutdown); // tell monitor that we transitioned to zero
		}
		return lRet;
	}

	void MonitorShutdown()
	{
		for(;;)
		{
			::WaitForSingleObject(m_hEventShutdown, INFINITE);
			DWORD dwWait = 0;
			do
			{
				m_bActivity = false;
				dwWait = ::WaitForSingleObject(m_hEventShutdown, m_dwTimeOut);
			}
			while(dwWait == WAIT_OBJECT_0);
			// timed out
			if(!m_bActivity && m_nLockCnt == 0) // if no activity let's really bail
			{
#ifdef _ATL_FREE_THREADED
				::CoSuspendClassObjects();
				if(!m_bActivity && m_nLockCnt == 0)
#endif
					break;
			}
		}
		// This handle should be valid now. If it isn't,
		// check if _Module.Term was called first (it shouldn't)
		if(::CloseHandle(m_hEventShutdown))
			m_hEventShutdown = NULL;
		::PostThreadMessage(m_dwMainThreadID, WM_QUIT, 0, 0);
	}

	bool StartMonitor()
	{
		m_hEventShutdown = ::CreateEvent(NULL, false, false, NULL);
		if(m_hEventShutdown == NULL)
			return false;
		DWORD dwThreadID = 0;
#if !defined(_ATL_MIN_CRT) && defined(_MT)
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, (UINT (WINAPI*)(void*))MonitorProc, this, 0, (UINT*)&dwThreadID);
#else
		HANDLE hThread = ::CreateThread(NULL, 0, MonitorProc, this, 0, &dwThreadID);
#endif
		bool bRet = (hThread != NULL);
		if(bRet)
			::CloseHandle(hThread);
		return bRet;
	}

	static DWORD WINAPI MonitorProc(void* pv)
	{
		CServerAppModule* p = (CServerAppModule*)pv;
		p->MonitorShutdown();
		return 0;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CString forward reference (enables CString use in atluser.h and atlgdi.h)

#if defined(_WTL_FORWARD_DECLARE_CSTRING) && !defined(_WTL_USE_CSTRING)
  #define _WTL_USE_CSTRING
#endif // defined(_WTL_FORWARD_DECLARE_CSTRING) && !defined(_WTL_USE_CSTRING)

#ifdef _WTL_USE_CSTRING
  class CString;   // forward declaration (include atlmisc.h for the whole class)
#endif // _WTL_USE_CSTRING

// CString namespace
#ifndef _CSTRING_NS
  #ifdef __ATLSTR_H__
    #define _CSTRING_NS	ATL
  #else
    #define _CSTRING_NS	WTL
  #endif
#endif // _CSTRING_NS

// Type classes namespace
#ifndef _WTYPES_NS
  #ifdef __ATLTYPES_H__
    #define _WTYPES_NS
  #else
    #define _WTYPES_NS	WTL
  #endif
#endif // _WTYPES_NS

}; // namespace WTL


///////////////////////////////////////////////////////////////////////////////
// General DLL version helpers (excluded from atlbase.h if _ATL_DLL is defined)

//#if (_ATL_VER < 0x0700) && defined(_ATL_DLL)

// Common Control Versions:
//   Win95/WinNT 4.0    maj=4 min=00
//   IE 3.x     maj=4 min=70
//   IE 4.0     maj=4 min=71

//#endif // (_ATL_VER < 0x0700 Unsupported)


// These are always included
#include "atlwinx.h"
#include "atluser.h"
#include "atlgdi.h"

#ifndef _WTL_NO_AUTOMATIC_NAMESPACE
using namespace WTL;
#endif // !_WTL_NO_AUTOMATIC_NAMESPACE

#endif // __ATLAPP_H__
