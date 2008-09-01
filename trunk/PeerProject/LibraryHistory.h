//
// LibraryHistory.h
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

#if !defined(AFX_LIBRARYHISTORY_H__71D206EC_CDA8_46AD_9800_4A11C9EBAEA5__INCLUDED_)
#define AFX_LIBRARYHISTORY_H__71D206EC_CDA8_46AD_9800_4A11C9EBAEA5__INCLUDED_

#pragma once

#include "PeerProjectFile.h"

class CLibraryRecent;
class CLibraryFile;


class CLibraryHistory
{
// Construction
public:
	CLibraryHistory();
	virtual ~CLibraryHistory();

// Attributes
protected:
	CList< CLibraryRecent* > m_pList;

public:
	struct sTorrentDetails
	{
		CString			m_sName;
		CString			m_sPath;
		Hashes::BtHash  m_oBTH;
		DWORD			m_tLastSeeded;
		QWORD			m_nUploaded;
		QWORD			m_nDownloaded;
	};
	
	sTorrentDetails	LastSeededTorrent;			// Most recently seeded torrent (for home page button)
	sTorrentDetails	LastCompletedTorrent;		// Most recently completed torrent that didn't reach 100% ratio


// Operations
public:
	POSITION		GetIterator() const;
	CLibraryRecent*	GetNext(POSITION& pos) const;
	INT_PTR			GetCount() const { return m_pList.GetCount(); }
	void			Clear();
public:
	BOOL			Check(CLibraryRecent* pRecent, int nScope = 0) const;
	CLibraryRecent*	GetByPath(LPCTSTR pszPath) const;
    CLibraryRecent*	Add(
						LPCTSTR pszPath,
						const Hashes::Sha1ManagedHash& oSHA1,
						const Hashes::Ed2kManagedHash& oED2K,
						const Hashes::BtManagedHash& oBTH,
						const Hashes::Md5ManagedHash& oMD5,
						LPCTSTR pszSources );
	BOOL			Submit(CLibraryFile* pFile);
	void			OnFileDelete(CLibraryFile* pFile);
	void			ClearTodays();
	int				Prune();
	void			Serialize(CArchive& ar, int nVersion);

};


class CLibraryRecent : public CPeerProjectFile
{
// Construction
public:
	CLibraryRecent();
    CLibraryRecent(
		LPCTSTR pszPath,
		const Hashes::Sha1ManagedHash& oSHA1,
		const Hashes::Ed2kManagedHash& oED2K,
		const Hashes::BtManagedHash& oBTH,
		const Hashes::Md5ManagedHash& oMD5,
		LPCTSTR pszSources );
	virtual ~CLibraryRecent();

// Attributes
public:
	FILETIME		m_tAdded;
	BOOL			m_bToday;
public:
	CLibraryFile*	m_pFile;
	CString			m_sSources;
public:
    Hashes::Sha1ManagedHash m_oSHA1;
    Hashes::TigerManagedHash m_oTiger;
    Hashes::Md5ManagedHash m_oMD5;
    Hashes::Ed2kManagedHash m_oED2K;
    Hashes::BtManagedHash m_oBTH;

// Operations
public:
	void	RunVerify(CLibraryFile* pFile);
	void	Serialize(CArchive& ar, int nVersion);

};

extern CLibraryHistory LibraryHistory;

#endif // !defined(AFX_LIBRARYHISTORY_H__71D206EC_CDA8_46AD_9800_4A11C9EBAEA5__INCLUDED_)
