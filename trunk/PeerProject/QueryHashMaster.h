//
// QueryHashMaster.h
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

#include "QueryHashTable.h"

class CQueryHashGroup;


class CQueryHashMaster : public CQueryHashTable
{
// Construction
public:
	CQueryHashMaster();
	virtual ~CQueryHashMaster();

// Attributes
protected:
	CList< CQueryHashGroup* > m_pGroups;
	int			m_nPerGroup;
	BOOL		m_bValid;

// Operations
public:
	void		Create();
	void		Add(CQueryHashTable* pTable);
	void		Remove(CQueryHashTable* pTable);
	void		Build();

// Inlines
public:
	inline POSITION GetIterator() const
	{
		return m_pGroups.GetHeadPosition();
	}

	inline CQueryHashGroup* GetNext(POSITION& pos) const
	{
		return m_pGroups.GetNext( pos );
	}

	inline INT_PTR GetCount() const
	{
		return m_pGroups.GetCount();
	}

	inline void Invalidate()
	{
		m_bValid = FALSE;
	}
};

extern CQueryHashMaster QueryHashMaster;
