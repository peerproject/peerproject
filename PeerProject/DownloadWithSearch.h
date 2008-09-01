//
// DownloadWithSearch.h
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

#if !defined(AFX_DOWNLOADWITHSEARCH_H__0ED8A270_13B0_47A6_9917_727CBDD82B27__INCLUDED_)
#define AFX_DOWNLOADWITHSEARCH_H__0ED8A270_13B0_47A6_9917_727CBDD82B27__INCLUDED_

#pragma once

#include "DownloadWithTiger.h"
#include "ManagedSearch.h"

class CManagedSearch;


class CDownloadWithSearch : public CDownloadWithTiger
{
// Construction
protected:
	CDownloadWithSearch();
	virtual ~CDownloadWithSearch();

// Attributes
private:
	CManagedSearch*	m_pSearch;
	DWORD			m_tSearchTime;		// Timer for manual search
	DWORD			m_tSearchCheck;		// Limit auto searches
public:
	DWORD			m_tLastED2KGlobal;	// Time the last ed2k UDP GetSources was done on this download
	DWORD			m_tLastED2KLocal;	// Time the last ed2k TCP GetSources was done on this download

// Operations
public:
	BOOL			FindSourcesAllowed(DWORD tNow) const;
	virtual BOOL	FindMoreSources();
	inline BOOL		IsSearching() const { return m_pSearch != NULL && m_pSearch->IsActive(); };
protected:
	void	RunSearch(DWORD tNow);
	void	StopSearch();
private:
	void	StartManualSearch();
	void	StartAutomaticSearch();
	BOOL	CanSearch() const;
	void	PrepareSearch();

};

#endif // !defined(AFX_DOWNLOADWITHSEARCH_H__0ED8A270_13B0_47A6_9917_727CBDD82B27__INCLUDED_)
