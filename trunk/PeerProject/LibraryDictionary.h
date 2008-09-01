//
// LibraryDictionary.h
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

#if !defined(AFX_LIBRARYDICTIONARY_H__6C8B6129_DF70_4AE4_895B_7C7CE7CB3192__INCLUDED_)
#define AFX_LIBRARYDICTIONARY_H__6C8B6129_DF70_4AE4_895B_7C7CE7CB3192__INCLUDED_

#pragma once

class CLibraryWord;
class CLibraryFile;
class CQueryHashTable;
class CQuerySearch;


class CLibraryDictionary
{
// Construction
public:
	CLibraryDictionary();
	virtual ~CLibraryDictionary();

// Attributes
public:
	CQueryHashTable*	m_pTable;
protected:
	CMap< CString, const CString&, CLibraryWord*, CLibraryWord* > m_pWords;
	BOOL				m_bTable;
	DWORD				m_nSearchCookie;

// Operations
public:
	void				Add(CLibraryFile* pFile);
	void				Remove(CLibraryFile* pFile);
	BOOL				BuildHashTable();					//Build hash table if needed
	void				RebuildHashTable();					//Force hash table to re-build
	CQueryHashTable*	GetHashTable();
	void				Clear();
	CList< CLibraryFile* >*	Search(CQuerySearch* pSearch, int nMaximum = 0, BOOL bLocal = FALSE, BOOL bAvailableOnly = TRUE);
	void				Serialize(CArchive& ar, int nVersion);
protected:
	void				ProcessFile(CLibraryFile* pFile, BOOL bAdd);
	int					ProcessPhrase(CLibraryFile* pFile, const CString& strPhrase, BOOL bAdd, BOOL bLowercase = TRUE);
	int					MakeKeywords(CLibraryFile* pFile, const CString& strWord, BOOL bAdd);
	inline void			ProcessWord(CLibraryFile* pFile, const CString& strWord, BOOL bAdd);
	static BOOL			IsValidKeyword(CString& strKeyword);
};


class CLibraryWord
{
// Construction
public:
	CLibraryWord();
	~CLibraryWord();

// Attributes
public:
	CLibraryFile**	m_pList;
	DWORD			m_nCount;

// Operations
public:
	inline void		Add(CLibraryFile* pFile);
	inline BOOL		Remove(CLibraryFile* pFile);

};

extern CLibraryDictionary LibraryDictionary;

#endif // !defined(AFX_LIBRARYDICTIONARY_H__6C8B6129_DF70_4AE4_895B_7C7CE7CB3192__INCLUDED_)
