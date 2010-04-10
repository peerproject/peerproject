//
// LibraryDictionary.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "SharedFile.h"

class CQueryHashTable;
class CQuerySearch;


class CLibraryDictionary
{
public:
	CLibraryDictionary();
	virtual ~CLibraryDictionary();

	void					AddFile(const CLibraryFile& oFile);
	void					RemoveFile(const CLibraryFile& oFile);
	void					BuildHashTable();		// Build hash table if needed
	const CQueryHashTable*	GetHashTable();
	void					Invalidate();			// Force dictionary and hash table to re-build
	void					Clear();
	void					Serialize(CArchive& ar, int nVersion);
	CFileList*				Search(const CQuerySearch* pSearch, int nMaximum = 0, bool bLocal = false, bool bAvailableOnly = true);

private:
	typedef CMap< CString, const CString&, CFileList*, CFileList*& > CWordMap;

	CWordMap				m_oWordMap;
	CQueryHashTable*		m_pTable;
	bool					m_bValid;				// Table is up to date
	DWORD					m_nSearchCookie;

	void					ProcessFile(const CLibraryFile& oFile, bool bAdd, bool bCanUpload);
	void					ProcessPhrase(const CLibraryFile& oFile, const CString& strPhrase, bool bAdd, bool bCanUpload);
	void					ProcessWord(const CLibraryFile& oFile, const CString& strWord, bool bAdd, bool bCanUpload);
};

extern CLibraryDictionary LibraryDictionary;
