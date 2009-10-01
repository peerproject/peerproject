//
// StdAfx.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2007-2008.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once

#define WINVER			0x0500
#define _WIN32_WINNT	0x0500
#define _WIN32_WINDOWS	0x0500
#define _WIN32_IE		0x0500
#define BOOST_USE_WINDOWS_H
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxmt.h>			// MFC multithreading
#include <shlobj.h>			// Shell objects
#include <shlwapi.h>

#include <Boost\type_traits\is_same.hpp>
#include <Boost\checked_delete.hpp>
#include "..\PeerProject\Augment\auto_ptr.hpp"
#include "..\PeerProject\Augment\auto_array.hpp"

#include "..\HashLib\HashLib.h"

typedef unsigned __int64 QWORD;

#define BIF_NEWDIALOGSTYLE	0x00000040
#define OFN_ENABLESIZING	0x00800000

using namespace augment;
