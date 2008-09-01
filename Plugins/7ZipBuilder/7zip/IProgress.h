//
// IProgress.h
//
// This file is part of PeerProject (peerproject.org) © 2008

// Portions Copyright 7Zip (7-zip.org)
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

#ifndef __IPROGRESS_H
#define __IPROGRESS_H

#include "MyUnknown.h"
#include "Types.h"

// {23170F69-40C1-278A-0000-000000050000}
DEFINE_GUID(IID_IProgress, 
0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00);
MIDL_INTERFACE("23170F69-40C1-278A-0000-000000050000")
IProgress: public IUnknown
{
  STDMETHOD(SetTotal)(UInt64 total) PURE;
  STDMETHOD(SetCompleted)(const UInt64 *completeValue) PURE;
};

/*
// {23170F69-40C1-278A-0000-000000050002}
DEFINE_GUID(IID_IProgress2, 
0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x02);
MIDL_INTERFACE("23170F69-40C1-278A-0000-000000050002")
IProgress2: public IUnknown
{
public:
  STDMETHOD(SetTotal)(const UInt64 *total) PURE;
  STDMETHOD(SetCompleted)(const UInt64 *completeValue) PURE;
};
*/

#endif
