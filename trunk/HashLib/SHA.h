//
// SHA.h
//
// Copyright (c) PeerProject Development Team, 2002-2008.
// This file is part of PEERPROJECT (PeerProject.org)
//
// PeerProject is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with PeerProject; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

class HASHLIB_API CSHA
{
public:
	CSHA();
	virtual ~CSHA() {}

	void Reset();
	void Add(const void* pData, size_t nLength);
	void Finish();

	struct HASHLIB_API Digest // 160 bit
	{
		uint32& operator[](size_t i) { return data[ i ]; }
		const uint32& operator[](size_t i) const { return data[ i ]; }
		uint32 data[ 5 ];
	};

	void GetHash(__in_bcount(20) uchar* pHash) const;

#ifndef HASHLIB_USE_ASM
	struct TransformArray
	{
		TransformArray(const uint32* const buffer);
		uint32 operator[](uint32 index) const
		{
			return m_buffer[ index ];
		}
		uint32 m_buffer[ 80 ];
	};
#endif

private:
	struct SHA1State
	{
		static const size_t	blockSize = 64;
		uint64	m_nCount;
		uint32	m_nState[ 5 ];
		uchar	m_oBuffer[ blockSize ];
	};
	SHA1State m_State;

#ifndef HASHLIB_USE_ASM
	void	Transform(const TransformArray w);
#endif
};
