//
// QueryHashGroup.h
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

#if !defined(AFX_QUERYHASHGROUP_H__27ED18EE_517A_4CFC_91C9_235C74686518__INCLUDED_)
#define AFX_QUERYHASHGROUP_H__27ED18EE_517A_4CFC_91C9_235C74686518__INCLUDED_

#pragma once

class CQueryHashTable;


class CQueryHashGroup
{
// Construction
public:
	CQueryHashGroup(DWORD nHash = 0);
	virtual ~CQueryHashGroup();

// Attributes
public:
	BYTE*		m_pHash;
	DWORD		m_nHash;
	DWORD		m_nCount;
protected:
	CList< CQueryHashTable* > m_pTables;

// Operations
public:
	void	Add(CQueryHashTable* pTable);
	void	Remove(CQueryHashTable* pTable);
protected:
	void	Operate(CQueryHashTable* pTable, BOOL nAdd);

// Inlines
public:
	inline POSITION GetIterator() const
	{
		return m_pTables.GetHeadPosition();
	}

	inline CQueryHashTable* GetNext(POSITION& pos) const
	{
		return m_pTables.GetNext( pos );
	}

	inline INT_PTR GetCount() const
	{
		return m_pTables.GetCount();
	}
};

#endif // !defined(AFX_QUERYHASHGROUP_H__27ED18EE_517A_4CFC_91C9_235C74686518__INCLUDED_)
