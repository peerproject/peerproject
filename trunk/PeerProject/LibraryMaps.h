//
// LibraryMaps.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "SharedFile.h"

class CLibrary;
class CQuerySearch;


class CLibraryMaps : public CComObject
{
	DECLARE_DYNAMIC(CLibraryMaps)

public:
	CLibraryMaps();
	virtual ~CLibraryMaps();

public:
	POSITION		GetFileIterator() const;
	CLibraryFile*	GetNextFile(POSITION& pos) const;
	INT_PTR 		GetFileCount() const { return m_pIndexMap.GetCount(); }
	INT_PTR 		GetNameCount() const { return m_pNameMap.GetCount(); }	// For Debug Benchmark
	INT_PTR 		GetPathCount() const { return m_pPathMap.GetCount(); }	// For Debug Benchmark
	void			GetStatistics(DWORD* pnFiles, QWORD* pnVolume);

	CLibraryFile*	LookupFile(DWORD_PTR nIndex, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileByName(LPCTSTR pszName, QWORD nSize, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileByPath(LPCTSTR pszPath, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileByURN(LPCTSTR pszURN, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileBySHA1(const Hashes::Sha1Hash& oSHA1, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileByTiger(const Hashes::TigerHash& oTiger, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileByED2K(const Hashes::Ed2kHash& oED2K, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileByBTH(const Hashes::BtHash& oBTH, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileByMD5(const Hashes::Md5Hash& oMD5, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CLibraryFile*	LookupFileByHash(const CPeerProjectFile* pFilter, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	CFileList*		LookupFilesByHash(const CPeerProjectFile* pFilter, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE, int nMaximum = 1) const;

protected:
	CIndexMap		m_pIndexMap;
	CFileMap		m_pNameMap;
	CFileMap		m_pPathMap;
	CLibraryFile**	m_pSHA1Map;
	CLibraryFile**	m_pTigerMap;
	CLibraryFile**	m_pED2KMap;
	CLibraryFile**	m_pBTHMap;
	CLibraryFile**	m_pMD5Map;
	CFileList		m_pDeleted;
	DWORD			m_nNextIndex;
	DWORD			m_nFiles;
	QWORD			m_nVolume;

	void			Clear();
	DWORD			AllocateIndex();
	void			OnFileAdd(CLibraryFile* pFile);
	void			OnFileRemove(CLibraryFile* pFile);
	void			CullDeletedFiles(CLibraryFile* pMatch);
	CFileList*		Browse(int nMaximum) const;
	CFileList*		WhatsNew(const CQuerySearch* pSearch, int nMaximum) const;
	void			Serialize1(CArchive& ar, int nVersion);
	void			Serialize2(CArchive& ar, int nVersion);

// COM
protected:
	BEGIN_INTERFACE_PART(LibraryFiles, ILibraryFiles)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get__NewEnum)(IUnknown FAR* FAR* ppEnum);
		STDMETHOD(get_Item)(VARIANT vIndex, ILibraryFile FAR* FAR* ppFile);
		STDMETHOD(get_Count)(LONG FAR* pnCount);
	END_INTERFACE_PART(LibraryFiles)

	DECLARE_INTERFACE_MAP()

	friend class CLibrary;
	friend class CLibraryBuilder;
};

extern CLibraryMaps LibraryMaps;
