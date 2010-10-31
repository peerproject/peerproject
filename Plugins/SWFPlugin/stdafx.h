//
// stdafx.h
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently
//

#pragma once

#define STRICT
#define _WIN32_DCOM
//#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_FREE_THREADED
#define _ATL_ALL_WARNINGS

#pragma warning( push, 0 )

#include "Resource.h"

#include <atlbase.h>
#include <atlwin.h>
#include <atlcom.h>
#include <atlstr.h>

//using namespace ATL;

const long cx = 256;
const long cy = 256;

typedef struct {
	HBITMAP			hBitmap;
	BITMAPINFOHEADER	bmiHeader;
} MY_DATA;

extern MY_DATA*			_Data;
extern CRITICAL_SECTION	_CS;

#pragma warning( pop )
