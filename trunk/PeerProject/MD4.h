//
// MD4.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#ifndef MD4_H_INCLUDED
#define MD4_H_INCLUDED

#pragma once

struct MD4State
{
	static const std::size_t blockSize = 64;
	uint64	m_nCount;
	uint32	m_nState[ 4 ];
	uchar m_oBuffer[ blockSize ];
};

#ifdef PEERPROJECT_USE_ASM
extern "C" void __stdcall MD4_Add_p5(MD4State*, const void* pData, std::size_t nLength);
#endif
class CMD4
{
// Construction
public:
	CMD4() { Reset(); }
	
	struct MD4Digest
	{
		uint32& operator[](std::size_t i) { return data[ i ]; }
		const uint32& operator[](std::size_t i) const { return data[ i ]; }
		uint32 data[ 4 ];
	};
// Attributes
private:
	MD4State m_State;

// Operations
public:
	void	GetHash(MD4Digest& oHash) const;
	void	Reset();
	void	Finish();
// MD4 block update operation. Continues an MD4 message-digest operation,
// processing another message block, and updating the context
#ifdef PEERPROJECT_USE_ASM
	void	Add(const void* pData, std::size_t nLength)
	{
		MD4_Add_p5( &m_State, pData, nLength );
	}
#else
	void	Add(const void* pData, std::size_t nLength);
private:
	void	Transform(const uint32* data);
#endif
};

#endif // #ifndef MD4_H_INCLUDED
