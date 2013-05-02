//
// stdafx.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Nikolay Raspopov, 2005.
//
// GFL Library, GFL SDK and XnView
// Copyright (c) 1991-2004 Pierre-E Gougelet
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently


#pragma once

#define STRICT

#define _WIN32_DCOM
//#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_FREE_THREADED
#define _ATL_ALL_WARNINGS

#pragma warning( push, 0 )

#include "LibGFL.h"
#include "Resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <atlcoll.h>

//using namespace ATL;

#pragma warning( pop )
