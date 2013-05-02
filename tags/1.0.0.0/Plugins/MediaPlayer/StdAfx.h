//
// StdAfx.h
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently
//

#pragma once

#pragma warning( disable : 4127 )	// conditional expression is constant


#ifndef STRICT
#define STRICT
#endif

#include "TargetVer.h"

#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_ALL_WARNINGS

#define _ATL_DEBUG_QI
#define _ATL_DEBUG_INTERFACES

#include "Resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <atltypes.h>

#include <dshow.h>
#include <mmreg.h>

using namespace ATL;
