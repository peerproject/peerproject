//
// ZLib.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
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

// CZLib makes it easier to use the zlib compression library
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CZLib

// Make the compiler only include the lines here once, this is the same thing as pragma once
#if !defined(AFX_ZLIB_H__3ABC5B39_501F_41B0_828A_B7CDFAD0F73B__INCLUDED_)
#define AFX_ZLIB_H__3ABC5B39_501F_41B0_828A_B7CDFAD0F73B__INCLUDED_

// Only include the lines beneath this one once
#pragma once

// Wraps the compress and decompress data compression functions of the ZLib compression library
class CZLib
{

// Operations
public:

	// Compress and decompress nInput bytes at pInput to a new returned buffer of size pnOutput
	static auto_array< BYTE > Compress(LPCVOID pInput, DWORD nInput, DWORD* pnOutput, DWORD nSuggest = 0);
	static auto_array< BYTE > Decompress(LPCVOID pInput, DWORD nInput, DWORD* pnOutput);
};

// End the group of lines to only include once, pragma once doesn't require an endif at the bottom
#endif // !defined(AFX_ZLIB_H__3ABC5B39_501F_41B0_828A_B7CDFAD0F73B__INCLUDED_)
