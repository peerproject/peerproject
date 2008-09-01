//
// DownloadGroup.h
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

class CDownload;


class CDownloadGroup
{
// Construction
public:
	CDownloadGroup(const LPCTSTR szName = NULL, const BOOL bTemporary = FALSE);
	virtual ~CDownloadGroup();

// Attributes
protected:
	CList< CDownload* >	m_pDownloads;
	// Temporary group:
	//	TRI_UNKNOWN	- Persistent group;
	//	TRI_FALSE	- Temporary group, not completed yet;
	//	TRI_TRUE	- Temporary group, feel free to delete.
	TRISTATE			m_bTemporary;

public:
	CString				m_sName;
	CString				m_sSchemaURI;
	CString				m_sFolder;
	CList< CString >	m_pFilters;
	int					m_nImage;
	BOOL				m_bRemoteSelected;

// Operations
public:
	void		Add(CDownload* pDownload);
	void		Remove(CDownload* pDownload);
	void		Clear();
	void		SetCookie(int nCookie);
	void		CopyList(CList< CDownload* >& pList);
	BOOL		Link(CDownload* pDownload);
	int			LinkAll();
	void		AddFilter(LPCTSTR pszFilter);
	void		SetSchema(LPCTSTR pszURI);
	void		Serialize(CArchive& ar, const int nVersion);
	BOOL		IsTemporary();

// Inlines
public:
	inline POSITION GetIterator() const
	{
		return m_pDownloads.GetHeadPosition();
	}

	inline CDownload* GetNext(POSITION& pos) const
	{
		return m_pDownloads.GetNext( pos );
	}

	inline BOOL Contains(CDownload* pDownload) const
	{
		return m_pDownloads.Find( pDownload ) != NULL;
	}

	inline INT_PTR GetCount() const
	{
		return m_pDownloads.GetCount();
	}
};
