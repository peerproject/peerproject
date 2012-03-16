//
// MetaList.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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

#include "Schema.h"

class CMetaList;
class CMetaItem;
class CXMLElement;
class CAlbumFolder;

class CMetaList
{
// Construction
public:
	CMetaList();
	virtual ~CMetaList();

// Attributes
protected:
	CList< CMetaItem* >	m_pItems;
	BOOL		m_bMusicBrainz;
	CBitmap		m_bmMusicBrainz;
	int			m_nHeight;

// Operations
public:
	CMetaItem*	Add(LPCTSTR pszKey, LPCTSTR pszValue);
	CMetaItem*	Find(LPCTSTR pszKey) const;
	void		Remove(LPCTSTR pszKey);
	void		Clear();
	void		Vote();
	void		Shuffle();
	void		Setup(CSchemaPtr pSchema, BOOL bClear = TRUE);
	void		Setup(const CMetaList* pMetaList);				// For copying data from the external list
	void		Combine(const CXMLElement* pXML);
	void		CreateLinks();
	void		Clean(int nMaxLength = 128);
	void		ComputeWidth(CDC* pDC, int& nKeyWidth, int& nValueWidth);
	CMetaItem*	HitTest(const CPoint& point, BOOL bLinksOnly = FALSE);
	BOOL		IsMusicBrainz() const;
	BOOL		OnSetCursor(CWnd* pWnd);
	BOOL		OnClick(const CPoint& point);
	void		Paint(CDC* pDC, const CRect* prcArea);
	int			Layout(CDC* pDC, int nWidth);

// Inline Operations
public:
	inline POSITION GetIterator() const
	{
		return m_pItems.GetHeadPosition();
	}

	inline CMetaItem* GetNext(POSITION& pos) const
	{
		return pos ? m_pItems.GetNext( pos ) : NULL;
	}

	inline INT_PTR GetCount() const
	{
		return m_pItems.GetCount();
	}

	inline CMetaItem* GetFirst() const
	{
		return m_pItems.IsEmpty() ? NULL : m_pItems.GetHead();
	}

	inline int GetHeight() const
	{
		return m_nHeight;
	}

	INT_PTR	GetCount(BOOL bVisibleOnly) const;
};


class CMetaItem : public CRect
{
// Construction
public:
	CMetaItem(CSchemaMember* pMember = NULL);

// Attributes
public:
	CSchemaMember*	m_pMember;
	CMap< CString, const CString&, int, int > m_pVote;
	CString			m_sKey;
	CString			m_sValue;
	BOOL			m_bValueDefined;
	BOOL			m_bLink;
	CString			m_sLink;
	CString			m_sLinkName;
	BOOL			m_bFullWidth;
	int				m_nHeight;

// Operations
public:
	BOOL			Combine(const CXMLElement* pXML);
	void			Vote();
	BOOL			Limit(int nMaxLength);
	BOOL			CreateLink();
	CAlbumFolder*	GetLinkTarget(BOOL bHTTP = TRUE) const;
	CString			GetMusicBrainzLink() const;

	inline CString GetDisplayValue() const
	{
		if ( m_bLink && ! m_sLinkName.IsEmpty() )
			return m_sLinkName;
		return m_sValue;
	}
};
