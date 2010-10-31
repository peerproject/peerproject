//
// ResultFilters.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
//
// Original Author : roo_koo_too@yahoo.com
//

#pragma once

// Create a class with some of the CMatchObjects members

class CFilterOptions
{
public:
	CFilterOptions();

	CString m_sName; // The options set name
	CString	m_sFilter;
	BOOL	m_bFilterPush;
	BOOL	m_bFilterBusy;
	BOOL	m_bFilterUnstable;
	BOOL	m_bFilterLocal;
	BOOL	m_bFilterReject;
	BOOL	m_bFilterBogus;
	BOOL	m_bFilterDRM;
	BOOL	m_bFilterRestricted;
	BOOL	m_bFilterSuspicious;
	BOOL	m_bFilterAdult;
	BOOL	m_bRegExp;
	DWORD	m_nFilterSources;
	QWORD	m_nFilterMinSize;
	QWORD	m_nFilterMaxSize;

	void	Serialize(CArchive& ar, int nVersion);
};

const DWORD NONE = ~0u;

class CResultFilters
{
public:
	CResultFilters(void);
	~CResultFilters(void);

	CFilterOptions **	m_pFilters; // Array of filter options
	DWORD				m_nFilters; // Count of filter options
	DWORD				m_nDefault; // Index of the default filter options

	int  Search(const CString& strName);
	void Serialize(CArchive& ar);
	void Add(CFilterOptions *pOptions);
	void Remove(DWORD index);
	void Load();
	BOOL Save();
};
