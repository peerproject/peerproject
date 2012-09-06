//
// DownloadWithSearch.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "DownloadWithTiger.h"
#include "ManagedSearch.h"

class CManagedSearch;


class CDownloadWithSearch : public CDownloadWithTiger
{
protected:
	CDownloadWithSearch();
	virtual ~CDownloadWithSearch();
public:
	BOOL			m_bUpdateSearch;	// Search must be updated
	DWORD			m_tLastED2KGlobal;	// Time the last ed2k UDP GetSources was done on this download
	DWORD			m_tLastED2KLocal;	// Time the last ed2k TCP GetSources was done on this download

	BOOL			IsSearching() const;
	virtual BOOL	FindMoreSources();

protected:
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
