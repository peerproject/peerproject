//
// StdAfx.h
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently
//

#pragma once

#if !defined(_UNICODE) || !defined(UNICODE)
	#error Unicode Required
#endif

// TargetVer.h: (WINVER)
#define NTDDI_VERSION	0x05010200	// NTDDI_WINXPSP2
#define _WIN32_WINNT	0x0501		// XP

#define VC_EXTRALEAN
#define STRICT

//#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_APARTMENT_THREADED

#include "Resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlenc.h>
#include <atlfile.h>
#include <atlstr.h>

//using namespace ATL;
