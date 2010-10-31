//
// QueryKeys.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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


class CQueryKeys
{
// Construction
public:
	CQueryKeys();
	virtual ~CQueryKeys();

// Attributes
protected:
	DWORD	m_nBits;
	DWORD*	m_pTable;
	DWORD	m_nTable;
	DWORD*	m_pMap;

	void	Alloc();

// Operations
public:
	DWORD	Create(DWORD nAddress);
	BOOL	Check(DWORD nAddress, DWORD nKey);
};
