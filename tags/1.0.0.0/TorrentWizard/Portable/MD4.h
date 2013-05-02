//
// MD4.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008-2010
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


#include <StdAfx.h>

class CHashMD4
{
public:
	union
	{
		BYTE	m_b[ 16 ];
		WORD	m_w[  8 ];
		DWORD	m_d[  4 ];
		QWORD	m_q[  2 ];
	};
	inline	CHashMD4();
	inline	CHashMD4(const CHashMD4 &oHash);
	inline	~CHashMD4();
	inline	void	operator =(const CHashMD4 &oHash);
	inline	BOOL	operator ==(const CHashMD4 &oHash);
	inline	BOOL	operator !=(const CHashMD4 &oHash);
};

class CMD4 : public CHashMD4
{
public:
	inline	CMD4();
	inline	~CMD4();
protected:
			QWORD	m_nCount;
			BYTE	m_pBuffer[64];
	static	BYTE	MD4_PADDING[ 64 ];
public:
	inline	void	Reset();
	inline	void	Add(LPCVOID pData, DWORD nLength);
	inline	void	Finish();
};

inline CHashMD4::CHashMD4()
{
}

inline CHashMD4::CHashMD4(const CHashMD4 &oHash)
{
	memcpy( &m_b, &oHash.m_b, sizeof m_b );
}

inline CHashMD4::~CHashMD4()
{
}

inline void CHashMD4::operator =(const CHashMD4 &oHash)
{
	memcpy( &m_b, &oHash.m_b, sizeof m_b );
}

inline BOOL CHashMD4::operator ==(const CHashMD4 &oHash)
{
	return memcmp( &m_b, &oHash.m_b, sizeof m_b ) == 0;
}

inline BOOL CHashMD4::operator !=(const CHashMD4 &oHash)
{
	return memcmp( &m_b, &oHash.m_b, sizeof m_b ) != 0;
}

inline CMD4::CMD4()
{
	Reset();
}

inline CMD4::~CMD4()
{
}

// MD4 initialization. Begins an MD4 operation, writing a new context
inline void CMD4::Reset()
{
	// Clear count
	m_nCount = 0;
	// Load magic initialization constants
	m_d[0] = 0x67452301;
	m_d[1] = 0xefcdab89;
	m_d[2] = 0x98badcfe;
	m_d[3] = 0x10325476;
}

extern "C" void __stdcall MD4_Add_p5(CMD4*, LPCVOID pData, DWORD nLength);

inline void CMD4::Add(LPCVOID pData, DWORD nLength)
{
	MD4_Add_p5( this, pData, nLength );
}

inline void CMD4::Finish()
{
	QWORD nBits = m_nCount << 3;
	DWORD index = (DWORD)m_nCount & 0x3f;
	MD4_Add_p5( this, MD4_PADDING, (index < 56) ? (56 - index) : (120 - index) );
	MD4_Add_p5( this, &nBits, 8 );
}
