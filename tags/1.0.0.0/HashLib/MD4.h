//
// MD4.h (ED2K)
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions Copyright Shareaza Development Team, 2002-2006.
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

class HASHLIB_API CMD4
{
public:
	CMD4();
	~CMD4() {}

public:
	void Reset();
	void Finish();
	void Add(const void* pData, size_t nLength);

	struct HASHLIB_API Digest // 128 bit
	{
		uint32& operator[](size_t i) { return data[ i ]; }
		const uint32& operator[](size_t i) const { return data[ i ]; }
		uint32 data[ 4 ];
	};

	void GetHash(__in_bcount(16) uchar* pHash) const;

#if !defined(WIN64) && defined(_MSC_VER) && (_MSC_VER >= 1700)		// VS2012 Targeting workaround
public:
#else
private:
#endif
	struct MD4State
	{
		static const size_t blockSize = 64;
		uint64	m_nCount;
		uint32	m_nState[ 4 ];
		uchar	m_oBuffer[ blockSize ];
	};
	MD4State m_State;

#ifndef HASHLIB_USE_ASM
	__forceinline void Transform(const uint32* data);
#endif
};
