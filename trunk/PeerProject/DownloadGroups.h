//
// DownloadGroups.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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

class CDownload;
class CDownloadGroup;


class CDownloadGroups
{
// Construction
public:
	CDownloadGroups();
	virtual ~CDownloadGroups();

// Attributes
public:
	CCriticalSection	m_pSection;
protected:
	CList< CDownloadGroup* > m_pList;
	CDownloadGroup*		m_pSuper;
	int					m_nBaseCookie;
	int					m_nSaveCookie;
	int					m_nGroupCookie;

// Operations
public:
	CDownloadGroup*		GetSuperGroup();
	CDownloadGroup*		Add(LPCTSTR pszName = NULL, BOOL bTemporary = FALSE, BOOL bUseExisting = FALSE);
	void				Remove(CDownloadGroup* pGroup);
	void				MoveRight(CDownloadGroup* pGroup);
	void				MoveLeft(CDownloadGroup* pGroup);
	void				Link(CDownload* pDownload);
	void				Unlink(CDownload* pDownload, BOOL bAndSuper = TRUE);
	void				CreateDefault();
	CString				GetCompletedPath(CDownload* pDownload);
public:
	void				Clear();
	BOOL				Load();
	BOOL				Save(BOOL bForce = TRUE);
protected:
	void				Serialize(CArchive& ar);
	void				CleanTemporary();

// Inlines
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}

	inline CDownloadGroup* GetNext(POSITION& pos) const
	{
		return m_pList.GetNext( pos );
	}

	inline INT_PTR GetCount() const
	{
		return m_pList.GetCount();
	}

	inline BOOL Check(CDownloadGroup* pGroup) const
	{
		return m_pList.Find( pGroup ) != NULL;
	}

	inline int GetGroupCookie() const
	{
		return m_nGroupCookie;
	}

	inline void IncBaseCookie()
	{
		m_nBaseCookie ++;
	}

private:
	CDownloadGroups(const CDownloadGroups&);
	CDownloadGroups& operator=(const CDownloadGroups&);
};

extern CDownloadGroups DownloadGroups;
