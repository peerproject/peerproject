//
// StdAfx.h
//
// This file is part of PeerProject (peerproject.org) © 2014
// Portions Copyright Nikolay Raspopov, 2014.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
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
#include <sdkddkver.h>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define STRICT

#define _SECURE_ATL 1
#define _CRT_SECURE_NO_WARNINGS

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT

#include "Resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlsafe.h>
#include <atlstr.h>
#include <atlhost.h>
#include <ExDisp.h>
#include <Shlobj.h>
#include <Shellapi.h>
#include <Wininet.h>

using namespace ATL;

#include "..\..\PeerProject\Strings.h"

CString LoadString( UINT nID );
CString GetURLs();
BOOL SaveURLs(const CString& sURLs);
