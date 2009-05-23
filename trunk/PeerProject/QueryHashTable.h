//
// QueryHashTable.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

class CPacket;
class CBuffer;
class CQuerySearch;
class CXMLElement;
class CNeighbour;
class CQueryHashGroup;


class CQueryHashTable
{
// Construction
public:
	CQueryHashTable();
	virtual ~CQueryHashTable();

// Attributes
public:
	bool				m_bLive;
	DWORD				m_nCookie;
	BYTE*				m_pHash;
	DWORD				m_nHash;
	DWORD				m_nBits;
	DWORD				m_nInfinity;
	DWORD				m_nCount;
	CBuffer*			m_pBuffer;
	CQueryHashGroup*	m_pGroup;

// Statics
public:
	static DWORD	HashWord(LPCTSTR pszString, size_t nStart, size_t nLength, DWORD nBits);
protected:
	static DWORD	HashNumber(DWORD nNumber, int nBits);

// Operations
public:
	void	Create();
	void	Clear();
	bool	Merge(const CQueryHashTable* pSource);
	bool	Merge(const CQueryHashGroup* pSource);
	bool	PatchTo(const CQueryHashTable* pTarget, CNeighbour* pNeighbour);
	bool	OnPacket(CPacket* pPacket);
	int		AddString(const CString& strString);
	int		AddExactString(const CString& strString);
	bool	CheckString(const CString& strString) const;
	bool	Check(const CQuerySearch& oSearch) const;
	bool	CheckHash(const DWORD nHash) const;
	int		GetPercent() const;
	void	Draw(HDC hDC, const RECT* pRC);
protected:
	bool	OnReset(CPacket* pPacket);
	bool	OnPatch(CPacket* pPacket);
	int		Add(LPCTSTR pszString, size_t nStart, size_t nLength);
	int		AddExact(LPCTSTR pszString, size_t nStart, size_t nLength);
	bool	PatchToOldPeerProject(const CQueryHashTable* pTarget, CNeighbour* pNeighbour);
};
