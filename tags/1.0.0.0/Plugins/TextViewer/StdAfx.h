//
// StdAfx.h
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently
//

#pragma once

#define STRICT
//#define WINVER 0x0500
//#define _WIN32_WINNT 0x0500
//#define _WIN32_WINDOWS 0x0500
//#define _WIN32_IE 0x0500
#define _WIN32_DCOM
#define _ATL_APARTMENT_THREADED
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_ALL_WARNINGS

#pragma warning( push, 0 )

#include "Resource.h"

#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>

using namespace ATL;

#pragma warning( pop )
