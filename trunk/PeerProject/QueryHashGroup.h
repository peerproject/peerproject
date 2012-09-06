//
// QueryHashGroup.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
