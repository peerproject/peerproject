//
// StdAfx.h : include file for standard system include files
//
// This file is part of PeerProject (peerproject.org) © 2009
// Portions Copyright Shareaza Development Team, 2009.
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

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "TargetVer.h"

#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT

#include "Resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <exdispid.h>

using namespace ATL;
