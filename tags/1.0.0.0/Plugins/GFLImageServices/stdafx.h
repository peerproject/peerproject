//
// stdafx.h
//
// This file is part of PeerProject (peerproject.org) © 2008,2012
// Copyright (c) Nikolay Raspopov, 2005.
//
// GFL Library, GFL SDK and XnView
// Copyright (c) 1991-2004 Pierre-E Gougelet
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently


#pragma once

#define STRICT
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#define _WIN32_IE 0x0600
#define _WIN32_DCOM
#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_ALL_WARNINGS

#pragma warning( push, 0 )

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <atlcoll.h>

#include "LibGFL.h"
#include "Resource.h"

HRESULT SAFEgflLoadBitmap(LPCWSTR filename, GFL_BITMAP **bitmap, const GFL_LOAD_PARAMS *params, GFL_FILE_INFORMATION *info) throw();
HRESULT SAFEgflLoadBitmapFromMemory(const GFL_UINT8 * data, GFL_UINT32 data_length, GFL_BITMAP **bitmap, const GFL_LOAD_PARAMS *params, GFL_FILE_INFORMATION *info) throw();
HRESULT SAFEgflSaveBitmapIntoMemory(GFL_UINT8 ** data, GFL_UINT32 * data_length, const GFL_BITMAP *bitmap, const GFL_SAVE_PARAMS *params) throw();
HRESULT SAFEgflSaveBitmap(LPCWSTR filename, const GFL_BITMAP *bitmap, const GFL_SAVE_PARAMS *params) throw();
int GetFormatIndexByExt(LPCTSTR ext);

using namespace ATL;

#pragma warning( pop )
