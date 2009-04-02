//
// LibraryHistory.h
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

#include "PeerProjectFile.h"

class CLibraryRecent;
class CLibraryFile;


class CLibraryHistory
{
public:
	CLibraryHistory();
	virtual ~CLibraryHistory();

	struct sTorrentDetails
	{
		CString			m_sName;
		CString			m_sPath;
		Hashes::BtHash  m_oBTH;
		DWORD			m_tLastSeeded;
		QWORD			m_nUploaded;
		QWORD			m_nDownloaded;
	};

	sTorrentDetails	LastSeededTorrent;		// Most recently seeded torrent (for home page button)
	sTorrentDetails	LastCompletedTorrent;	// Most recently completed torrent that didn't reach 100% ratio

	POSITION		GetIterator() const;
	CLibraryRecent*	GetNext(POSITION& pos) const;
	INT_PTR			GetCount() const { return m_pList.GetCount(); }
	void			Clear();

	BOOL			Check(CLibraryRecent* pRecent, int nScope = 0) const;
    void			Add(
						LPCTSTR pszPath,
						const Hashes::Sha1ManagedHash& oSHA1,
						const Hashes::TigerManagedHash& oTiger,
						const Hashes::Ed2kManagedHash& oED2K,
						const Hashes::BtManagedHash& oBTH,
						const Hashes::Md5ManagedHash& oMD5,
						LPCTSTR pszSources = _T(""));
	void			Submit(CLibraryFile* pFile);
	void			OnFileDelete(CLibraryFile* pFile);
	void			Serialize(CArchive& ar, int nVersion);

protected:
	CList< CLibraryRecent* > m_pList;

	CLibraryRecent*	GetByPath(LPCTSTR pszPath) const;
//	void			ClearTodays();
	void			Prune();
};


class CLibraryRecent : public CPeerProjectFile
{
public:
	FILETIME					m_tAdded;
//	BOOL						m_bToday;
	CLibraryFile*				m_pFile;
	CString						m_sSources;
    Hashes::Sha1ManagedHash		m_oSHA1;
    Hashes::TigerManagedHash	m_oTiger;
    Hashes::Md5ManagedHash		m_oMD5;
    Hashes::Ed2kManagedHash		m_oED2K;
    Hashes::BtManagedHash		m_oBTH;

protected:
	CLibraryRecent();
    CLibraryRecent(
		LPCTSTR pszPath,
		const Hashes::Sha1ManagedHash& oSHA1,
		const Hashes::TigerManagedHash&	oTiger,
		const Hashes::Ed2kManagedHash& oED2K,
		const Hashes::BtManagedHash& oBTH,
		const Hashes::Md5ManagedHash& oMD5,
		LPCTSTR pszSources);

	void	RunVerify(CLibraryFile* pFile);
	void	Serialize(CArchive& ar, int nVersion);

	friend class CLibraryHistory;
};

extern CLibraryHistory LibraryHistory;
