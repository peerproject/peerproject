//
// ZLib.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

// CZLib makes it easier to use the zlib compression library
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CZLib

#pragma once

// Wraps the compress/decompress data functions of the ZLib compression library
class CZLib
{
public:
	// Compress/Decompress nInput bytes at pInput to a new returned buffer of size pnOutput:

	// After use free memory by delete[] function:
	static auto_array< BYTE > Compress(LPCVOID pInput, DWORD nInput, DWORD* pnOutput, DWORD nSuggest = 0);
	static auto_array< BYTE > Decompress(LPCVOID pInput, DWORD nInput, DWORD* pnOutput);

	// Or, after use free memory by free() function:
	static BYTE* Compress2(LPCVOID pInput, DWORD nInput, DWORD* pnOutput, DWORD nSuggest = 0);
	static BYTE* Decompress2(LPCVOID pInput, DWORD nInput, DWORD* pnOutput);
};
