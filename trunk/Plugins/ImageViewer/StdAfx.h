//
// StdAfx.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Original author Michael Stokes released portions into the public domain.
// You are free to redistribute and modify this page without any restrictions.
//

#pragma once

#pragma warning( disable : 4127 )	// conditional expression is constant

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

#include "Resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atlcoll.h>

using namespace ATL;
