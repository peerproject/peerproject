//
// TigerTree.h
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

#ifndef TIGERTREE_H_INCLUDED
#define TIGERTREE_H_INCLUDED

#pragma once

class CTigerNode;

class CTigerTree
{
// Construction
public:
	CTigerTree();
	virtual ~CTigerTree();

// Operations
public:
	void	SetupAndAllocate(DWORD nHeight, uint64 nLength);
	void	SetupParameters(uint64 nLength);
	void	Clear();
	void	Serialize(CArchive& ar);
	DWORD	GetSerialSize() const;
public:
	BOOL	GetRoot(Hashes::TigerHash& oTiger) const;
	void	Assume(CTigerTree* pSource);
public:
	void	BeginFile(DWORD nHeight, uint64 nLength);
	void	AddToFile(const void* pInput, DWORD nLength);
	BOOL	FinishFile();
public:
	void	BeginBlockTest();
	void	AddToTest(const void* pInput, DWORD nLength);
	BOOL	FinishBlockTest(DWORD nBlock);
public:
	BOOL	ToBytes(BYTE** pOutput, DWORD* pnOutput, DWORD nHeight = 0);
	BOOL	FromBytes(const BYTE* pOutput, DWORD nOutput, DWORD nHeight, uint64 nLength);
	BOOL	CheckIntegrity();
	void	Dump();

// Inlines
public:
	inline BOOL		IsAvailable() const { CQuickLock oLock( m_pSection ); return m_pNode != NULL; }
	inline DWORD	GetHeight() const { CQuickLock oLock( m_pSection ); return m_nHeight; }
	inline DWORD	GetBlockLength() const { CQuickLock oLock( m_pSection ); return 1024 * m_nBlockCount; }
	inline DWORD	GetBlockCount() const { CQuickLock oLock( m_pSection ); return m_nBaseUsed; }

// Attributes
private:
	DWORD		m_nHeight;
	CTigerNode*	m_pNode;
	DWORD		m_nNodeCount;

// Processing Data
private:
	DWORD		m_nNodeBase;
	DWORD		m_nNodePos;
	DWORD		m_nBaseUsed;
	DWORD		m_nBlockCount;
	DWORD		m_nBlockPos;
	CTigerNode*	m_pStackBase;
	CTigerNode*	m_pStackTop;

	mutable CCriticalSection	m_pSection;

// Implementation
private:
	void	Collapse();
	void	BlocksToNode();
	static void	Tiger(LPCVOID pInput, uint64 nInput, uint64* pOutput, uint64* pInput1 = NULL, uint64* pInput2 = NULL);
};

class CTigerNode
{
// Construction
public:
	CTigerNode() : bValid( false ) { ZeroMemory( value, sizeof( value ) ) ; }

// Attributes
public:
	uint64	value[3];
	bool bValid;
};

#endif // #ifndef TIGERTREE_H_INCLUDED
