//
// QueryHashTable.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2006.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

class CPacket;
class CBuffer;
class CNeighbour;
class CPeerProjectFile;
class CQueryHashGroup;
class CQuerySearch;
class CXMLElement;


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

public:
	// Split phrase to keywords
	static void		MakeKeywords(const CString& strPhrase, CStringList& oKeywords);
	static DWORD	HashWord(LPCTSTR pszString, size_t nStart, size_t nLength, DWORD nBits);

protected:
	// Split word to keywords (Katakana/Hiragana/Kanji helper)
	static void		MakeKeywords(const CString& strWord, WORD nWordType, CStringList& oKeywords);
	static DWORD	HashNumber(DWORD nNumber, int nBits);

// Operations
public:
	void	Create();
	void	Clear();
	bool	Merge(const CQueryHashTable* pSource);
	bool	Merge(const CQueryHashGroup* pSource);
	bool	PatchTo(const CQueryHashTable* pTarget, CNeighbour* pNeighbour);
	bool	OnPacket(CPacket* pPacket);
	void	AddFile(const CPeerProjectFile& oFile);		// Add filename and hashes split on keywords
	void	AddHashes(const CPeerProjectFile& oFile);	// Add file hashes
	void	AddString(const CString& strString);		// Add string with streaming
	void	AddExactString(const CString& strString);	// Add string exactly
	bool	CheckString(const CString& strString) const;
	bool	Check(const CQuerySearch* pSearch) const;
	bool	CheckHash(const DWORD nHash) const;
	int		GetPercent() const;
	void	Draw(HDC hDC, const RECT* pRC);
protected:
	bool	OnReset(CPacket* pPacket);
	bool	OnPatch(CPacket* pPacket);
	void	Add(LPCTSTR pszString, int nStart, int nLength);
	void	AddExact(LPCTSTR pszString, int nStart, int nLength);
	bool	PatchToOldPeerProject(const CQueryHashTable* pTarget, CNeighbour* pNeighbour);
};
