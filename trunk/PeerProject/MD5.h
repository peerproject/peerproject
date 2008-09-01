//
// MD5.h
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

#ifndef MD5_H_INCLUDED
#define MD5_H_INCLUDED

#pragma once

struct MD5State
{
	static const std::size_t blockSize = 64;
	uint64	m_nCount;
	uint32	m_nState[ 4 ];
	uchar m_oBuffer[ blockSize ];
};

#ifdef PEERPROJECT_USE_ASM
extern "C" void __stdcall MD5_Add_p5(MD5State*, const void* pData, std::size_t nLength);
#endif

class CMD5
{
// Construction
public:
	CMD5() { Reset(); }

// Attributes
private:
	MD5State m_State;

// Operations
public:
	void	GetHash(Hashes::Md5Hash& oHash) const;
	void	Reset();
	void	Finish();
// MD5 block update operation. Continues an MD5 message-digest operation,
// processing another message block, and updating the context
#ifdef PEERPROJECT_USE_ASM
	void	Add(const void* pData, std::size_t nLength)
	{
		MD5_Add_p5( &m_State, pData, nLength );
	}
#else
	void	Add(const void* pData, std::size_t nLength);
private:
	void	Transform(const uint32* data);
#endif
};

#endif // #ifndef MD5_H_INCLUDED
