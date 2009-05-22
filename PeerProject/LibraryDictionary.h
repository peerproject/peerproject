//
// LibraryDictionary.h
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

#pragma once

class CLibraryFile;
class CQueryHashTable;
class CQuerySearch;

typedef CList< const CLibraryFile* > FilePtrList;


class CLibraryDictionary : private ::boost::noncopyable
{
private:
	typedef CMap< CString, const CString&, FilePtrList*, FilePtrList* > WordMap;

// Construction
public:
	CLibraryDictionary();
	virtual ~CLibraryDictionary();

// Attributes
private:
	WordMap				m_oWordMap;
	CQueryHashTable*	m_pTable;
	bool				m_bValid;							// Table is up to date
	DWORD				m_nSearchCookie;

// Operations
public:
	void					AddFile(const CLibraryFile& oFile);
	void					RemoveFile(const CLibraryFile& oFile);
	bool					BuildHashTable();					// Build hash table if needed
	void					RebuildHashTable();					// Force hash table to re-build
	CQueryHashTable*		GetHashTable();
	void					Clear();
	FilePtrList*			Search(const CQuerySearch& oSearch, int nMaximum = 0, bool bLocal = false, bool bAvailableOnly = true);
	void					Serialize(CArchive& ar, int nVersion);
private:
	void					ProcessFile(const CLibraryFile& oFile, bool bAdd, bool bCanUpload);
	void					ProcessPhrase(const CLibraryFile& oFile, const CString& strPhrase, bool bAdd, bool bCanUpload);
	void					MakeKeywords(const CLibraryFile& oFile, const CString& strWord, WORD nWordType, bool bAdd, bool bCanUpload);
	void					ProcessWord(const CLibraryFile& oFile, const CString& strWord, bool bAdd, bool bCanUpload);
	void					AddHashes(const CLibraryFile& oFile);
};

extern CLibraryDictionary LibraryDictionary;
