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

#ifndef QUERYHASHTABLE_H_INCLUDED
#define QUERYHASHTABLE_H_INCLUDED

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
	BOOL				m_bLive;
	DWORD				m_nCookie;
	BYTE*				m_pHash;
	DWORD				m_nHash;
	DWORD				m_nBits;
	DWORD				m_nInfinity;
	DWORD				m_nCount;
	CBuffer*			m_pBuffer;
	CQueryHashGroup*	m_pGroup;

// Operations
public:
	void	Create();
	void	Clear();
	BOOL	Merge(const CQueryHashTable* pSource);
	BOOL	Merge(const CQueryHashGroup* pSource);
	BOOL	PatchTo(CQueryHashTable* pTarget, CNeighbour* pNeighbour);
	BOOL	OnPacket(CPacket* pPacket);
//	int		AddPhrase(LPCTSTR pszPhrase);
	int		AddString(const CString& strString);
	int		AddExactString(const CString& strString);
//	BOOL	CheckPhrase(LPCTSTR pszSearch) const;
	BOOL	CheckString(const CString& strString) const;
	BOOL	Check(const CQuerySearch* pSearch) const;
	BOOL	CheckHash(const DWORD nHash) const;
	int		GetPercent() const;
	void	Draw(HDC hDC, const RECT* pRC);

protected:
	BOOL	OnReset(CPacket* pPacket);
	BOOL	OnPatch(CPacket* pPacket);
	int		Add(LPCTSTR pszString, size_t nStart, size_t nLength);
	int		AddExact(LPCTSTR pszString, size_t nStart, size_t nLength);
	BOOL	PatchToOldPeerProject(CQueryHashTable* pTarget, CNeighbour* pNeighbour);
public:
	static			DWORD	HashWord(LPCTSTR pszString, size_t nStart, size_t nLength, DWORD nBits);
protected:
	static	inline	DWORD	HashNumber(DWORD nNumber, int nBits);

};

#endif // #ifndef QUERYHASHTABLE_H_INCLUDED
