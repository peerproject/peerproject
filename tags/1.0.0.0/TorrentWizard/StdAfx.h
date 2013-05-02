//
// StdAfx.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) � 2008-2012
// Portions Copyright Shareaza Development Team, 2007.
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


// Generate Manifest  (Themed Controls)
#ifdef _UNICODE	// (Required)
#ifdef _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif // _UNICODE


#define WINVER				0x0600
#define _WIN32_WINNT		0x0600
#define _WIN32_WINDOWS		0x0600
#define NTDDI_VERSION		NTDDI_LONGHORN  // Minimum build target = Vista
#define VC_EXTRALEAN		// Exclude rarely-used things from Windows headers
#define BOOST_USE_WINDOWS_H

#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS		// Smaller filesize VS2012+

#include <sdkddkver.h>		// Setup versioning for Windows SDK/DDK
#include <afxwin.h> 		// MFC core and standard components
#include <afxext.h> 		// MFC extensions
#include <afxcmn.h> 		// MFC support for Windows Common Controls
#include <afxmt.h>			// MFC multithreading
#include <shlobj.h> 		// Shell objects
#include <shlwapi.h>		// Shell win api

#include <Augment\auto_ptr.hpp>		// (Dependencies moved to file)
#include <Augment\auto_array.hpp>

#ifndef _PORTABLE
#include "..\HashLib\HashLib.h"
//#include "..\PeerProject\Strings.h"
//#include "..\PeerProject\Buffer.h"	// Common alternative
//#include "..\PeerProject\BENode.h"	// (Remove local files first)
#endif

typedef unsigned __int64 QWORD;

#ifndef OFN_ENABLESIZING
  #define OFN_ENABLESIZING		0x00800000
#endif
#ifndef BIF_NEWDIALOGSTYLE
  #define BIF_NEWDIALOGSTYLE	0x00000040
#endif

using namespace augment;
