//
// stdafx.h
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently
//

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

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>

using namespace ATL;

#pragma warning( pop )

#pragma pack(push,1)

#include "unrar.h"

#pragma pack(pop)

typedef HANDLE (PASCAL *tRAROpenArchiveEx)(struct RAROpenArchiveDataEx *ArchiveData);
typedef int    (PASCAL *tRARCloseArchive)(HANDLE hArcData);
typedef int    (PASCAL *tRARReadHeaderEx)(HANDLE hArcData,struct RARHeaderDataEx *HeaderData);
typedef int    (PASCAL *tRARProcessFileW)(HANDLE hArcData,int Operation,wchar_t *DestPath,wchar_t *DestName);

extern tRAROpenArchiveEx fnRAROpenArchiveEx;
extern tRARCloseArchive fnRARCloseArchive;
extern tRARReadHeaderEx fnRARReadHeaderEx;
extern tRARProcessFileW fnRARProcessFileW;
