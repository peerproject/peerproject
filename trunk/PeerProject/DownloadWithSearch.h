//
// DownloadWithSearch.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "DownloadWithTiger.h"
#include "ManagedSearch.h"

class CManagedSearch;


class CDownloadWithSearch : public CDownloadWithTiger
{
public:
	BOOL			m_bUpdateSearch;	// Search must be updated
	DWORD			m_tLastED2KGlobal;	// Time the last ed2k UDP GetSources was done on this download
	DWORD			m_tLastED2KLocal;	// Time the last ed2k TCP GetSources was done on this download

	BOOL			IsSearching() const;
	virtual BOOL	FindMoreSources();

protected:
	CDownloadWithSearch();
	virtual ~CDownloadWithSearch();

	BOOL			FindSourcesAllowed(DWORD tNow) const;
	void			RunSearch(DWORD tNow);
	void			StopSearch();

private:
	CSearchPtr		m_pSearch;			// Managed search object
	DWORD			m_tSearchTime;		// Timer for manual search
	DWORD			m_tSearchCheck;		// Limit auto searches

	void			StartManualSearch();
	void			StartAutomaticSearch();
	void			PrepareSearch();
	BOOL			CanSearch() const;
};
