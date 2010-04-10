//
// StdAfx.h
//
// This file is part of PeerProject (peerproject.org) © 2010
// TextViewer plugin is released under the Persistent Public Domain license.
//
// This code may be treated as Public Domain, provided:
// the work in all its forms and attendant uses shall remain available as
// persistently "Public Domain" until it naturally enters the public domain.
// History remains immutable:  Authors do not disclaim copyright, but do disclaim
// all rights beyond asserting the reach and duration and spirit of this license.

#pragma once

#define STRICT
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#define _WIN32_IE 0x0500
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
