//
// Hashes/HashDescriptors.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2005-2007.
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

//! \file       Hashes/HashDescriptors.cpp
//! \brief      Defines urn prefixes for all hash types.

#include "..\StdAfx.h"

// If we are compiling in debug mode, replace the text "THIS_FILE" in the code with the name of this file
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace Hashes
{
	namespace Policies
	{
		const UrnString Sha1Descriptor::urns[ Sha1Descriptor::numUrns ] =
		{
			{ 32 +  9,  9, 9, L"urn:sha1:" },
			{ 32 +  5,  5, 5, L"sha1:" },
			{ 85,      13,13, L"urn:bitprint:" },
			{ 81,       9, 9, L"bitprint:" }
		};

		const UrnString TigerDescriptor::urns[ TigerDescriptor::numUrns ] =
		{
			{ 39 + 16, 16, 16, L"urn:tree:tiger/:" },
			{ 39 + 12, 12, 12, L"tree:tiger/:" },
			{ 39 + 46, 46, 13, L"urn:bitprint:" },
			{ 39 + 42, 42,  9, L"bitprint:" },
			{ 39 + 15, 15, 15, L"urn:tree:tiger:" },
			{ 39 + 11, 11, 11, L"tree:tiger:" },
			{ 39 + 11, 11, 11, L"urn:ttroot:" }
		};

		const UrnString Ed2kDescriptor::urns[ Ed2kDescriptor::numUrns ] =
		{
			{ 32 + 13, 13, 13, L"urn:ed2khash:" },
			{ 32 +  5,  5,  5, L"ed2k:" },
			{ 32 +  9,  9,  9, L"urn:ed2k:" },
			{ 32 +  9,  9,  9, L"ed2khash:" }
		};

		const UrnString Md5Descriptor::urns[ Md5Descriptor::numUrns ] =
		{
			{ 32 +  8,  8,  8, L"urn:md5:" },
			{ 32 +  4,  4,  4, L"md5:" }
		};

		const UrnString BthDescriptor::urns[ BthDescriptor::numUrns ] =
		{
			{ 32 +  9,  9, 9, L"urn:btih:" },
			{ 32 +  5,  5, 5, L"btih:" }
		};
	} // namespace Policies

} // namespace Hashes
