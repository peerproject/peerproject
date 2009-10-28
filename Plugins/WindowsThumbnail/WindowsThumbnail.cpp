//
// WindowsThumbnail.cpp : Implementation of WinMain
//
// This file is part of PeerProject (peerproject.org) © 2009
// Portions Copyright Nikolay Raspopov, 2009.
//
// PeerProject is free software; you can redistribute it
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

#include "StdAfx.h"
#include "Resource.h"
#include "WindowsThumbnail.h"

class CWindowsThumbnailModule : public CAtlExeModuleT< CWindowsThumbnailModule >
{
public :
	DECLARE_LIBID(LIBID_WindowsThumbnailLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WINDOWSTHUMBNAIL, "{3C5A89BA-2446-4310-9B37-5CEE375E08BE}")
};

CWindowsThumbnailModule _AtlModule;

extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	SetErrorMode( SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX |
		SEM_NOALIGNMENTFAULTEXCEPT | SEM_FAILCRITICALERRORS );

    return _AtlModule.WinMain( nShowCmd );
}
