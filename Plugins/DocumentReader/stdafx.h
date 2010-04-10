//
// stdafx.h
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently


#pragma once

#define _CRT_SECURE_NO_DEPRECATE
#define STRICT
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#define _WIN32_IE 0x0500
#define _WIN32_DCOM
#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_ALL_WARNINGS

#pragma warning( push, 0 )

#include "Resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <olectl.h>    // For OleCreatePictureIndirect

using namespace ATL;
using namespace std;

#include <unzip.h>
#include <iowin32.h>

#pragma warning( pop )

#include "Globals.h"
