//
// ED2K.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#if !defined(AFX_ED2K_H__0ED688AE_E4F5_49C6_8EC8_5C80EFA6EF6C__INCLUDED_)
#define AFX_ED2K_H__0ED688AE_E4F5_49C6_8EC8_5C80EFA6EF6C__INCLUDED_

#pragma once

#include "MD4.h"


class CED2K
{
// Construction
public:
	CED2K();
	virtual ~CED2K();

// Operations
public:
	void	Clear();
	void	Serialize(CArchive& ar);
	DWORD	GetSerialSize() const;
    BOOL	GetRoot(Hashes::Ed2kHash& oHash) const;
public:
	void	BeginFile(QWORD nLength);
	void	AddToFile(LPCVOID pInput, DWORD nLength);
	BOOL	FinishFile();
public:
	void	BeginBlockTest();
	void	AddToTest(LPCVOID pInput, DWORD nLength);
	BOOL	FinishBlockTest(DWORD nBlock);
public:
	LPCVOID	GetRawPtr() const;
	BOOL	ToBytes(BYTE** pOutput, DWORD* pnOutput);
	BOOL	FromBytes(BYTE* pOutput, DWORD nOutput, QWORD nSize = 0);
    BOOL	FromRoot(const Hashes::Ed2kHash& oHash);
	BOOL	CheckIntegrity();

// Inlines
public:
	inline BOOL		IsAvailable() const { return m_pList != NULL; }
	inline DWORD	GetBlockCount() const { return m_nList; }

// Attributes
protected:
    CMD4::MD4Digest m_pRoot;
    CMD4::MD4Digest* m_pList;
	DWORD	m_nList;
protected:
	CMD4	m_pSegment;
	DWORD	m_nCurHash;
	DWORD	m_nCurByte;
	bool	m_bNullBlock;

};

const size_t ED2K_PART_SIZE	= 9500 * 1024u;

#endif // !defined(AFX_ED2K_H__0ED688AE_E4F5_49C6_8EC8_5C80EFA6EF6C__INCLUDED_)
