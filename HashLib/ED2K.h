//
// ED2K.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "MD4.h"


class HASHLIB_API CED2K
{
public:
	CED2K();
	virtual ~CED2K();

	void	Clear();
	void	Save(uchar* pBuf) const;
	void	Load(const uchar* pBuf);
	uint32	GetSerialSize() const;
	LPCVOID	GetRawPtr() const;

	void	GetRoot(__in_bcount(16) uchar* pHash) const;
	void	FromRoot(__in_bcount(16) const uchar* pHash);

	void	BeginFile(uint64 nLength);
	void	AddToFile(LPCVOID pInput, uint32 nLength);
	BOOL	FinishFile();

	void	BeginBlockTest();
	void	AddToTest(LPCVOID pInput, uint32 nLength);
	BOOL	FinishBlockTest(uint32 nBlock);

	BOOL	ToBytes(BYTE** ppOutput, uint32* pnOutput);	// To free ppOutput, use GlobalFree function
	BOOL	FromBytes(BYTE* pOutput, uint32 nOutput, uint64 nSize = 0);
	BOOL	CheckIntegrity();

	BOOL	IsAvailable() const;
	void	SetSize(uint32 nSize);
	uint32	GetSize() const;
	uint32	GetBlockCount() const;

private:
	CMD4::Digest m_pRoot;
	CMD4::Digest* m_pList;
	uint32	m_nList;
	uint32	m_nCurHash;
	uint32	m_nCurByte;
	CMD4	m_pSegment;
	bool	m_bNullBlock;
};

const DWORD ED2K_PART_SIZE	= 9500 * 1024u;
