//
// MatchObjects.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#if !defined(MATCHOBJECTS_H)
#define MATCHOBJECTS_H

#pragma once

#include "PeerProjectFile.h"

class CSchema;
class CSchemaMember;
class CQuerySearch;
class CQueryHit;
class CMatchFile;
class CResultFilters;
class CDownload;
class CMetaList;
class CXMLElement;
class Review;
class CBaseMatchWnd;

typedef struct
{
	bool	bHadSHA1;
	bool	bHadTiger;
	bool	bHadED2K;
	bool	bHadBTH;
	bool	bHadMD5;
} FILESTATS;

class CMatchList
{
public:
	CMatchList(CBaseMatchWnd* pParent);
	virtual ~CMatchList();
	
public:
	CMutex			m_pSection;
	CString			m_sFilter;
	BOOL			m_bFilterBusy;
	BOOL			m_bFilterPush;
	BOOL			m_bFilterUnstable;
	BOOL			m_bFilterReject;
	BOOL			m_bFilterLocal;
	BOOL			m_bFilterBogus;
	BOOL			m_bFilterDRM;
	BOOL			m_bFilterAdult;
	BOOL			m_bFilterSuspicious;
	BOOL			m_bRegExp;
	QWORD			m_nFilterMinSize;
	QWORD			m_nFilterMaxSize;
	DWORD			m_nFilterSources;
	int				m_nSortColumn;
	BOOL			m_bSortDir;
	CSchema*		m_pSchema;
	BOOL			m_bNew;
	CResultFilters*	m_pResultFilters;
	CMatchFile**	m_pFiles;
	DWORD			m_nFiles;
	DWORD			m_nItems;
	DWORD			m_nFilteredFiles;
	DWORD			m_nFilteredHits;
	DWORD			m_nGnutellaHits;
	DWORD			m_nED2KHits;
	BOOL			m_bUpdated;
	DWORD			m_nUpdateMin;
	DWORD			m_nUpdateMax;
	CList< CMatchFile* > m_pSelectedFiles;
	CList< CQueryHit* > m_pSelectedHits;
protected:
	DWORD			m_nBuffer;
	CMatchFile**	m_pSizeMap;
	CMatchFile**	m_pMapSHA1;
	CMatchFile**	m_pMapTiger;
	CMatchFile**	m_pMapED2K;
	CMatchFile**	m_pMapBTH;
	CMatchFile**	m_pMapMD5;
	LPTSTR			m_pszFilter;
	LPTSTR			m_pszRegexPattern;
	CSchemaMember**	m_pColumns;
	int				m_nColumns;
	CBaseMatchWnd*	m_pParent;

	enum findType
	{
		fSHA1	= 0,
		fTiger	= 1,
		fED2K	= 2,
		fSize	= 3,
		fBTH	= 4,
		fMD5	= 5
	};
	
public:
	void		AddHits(CQueryHit* pHits, CQuerySearch* pFilter = NULL);
	DWORD		FileToItem(CMatchFile* pFile);
	void		Clear();
	BOOL		Select(CMatchFile* pFile, CQueryHit* pHit, BOOL bSelected = TRUE);
	CMatchFile*	GetSelectedFile(BOOL bFromHit = FALSE) const;
	CQueryHit*	GetSelectedHit() const;
	INT_PTR		GetSelectedCount() const;
	BOOL		ClearSelection();
	void		Filter();
	bool		CreateRegExpFilter(CString strPattern, CString& strFilter);
	void		SelectSchema(CSchema* pSchema, CList< CSchemaMember* >* pColumns);
	void		SetSortColumn(int nColumn = -1, BOOL bDirection = FALSE);
	void		UpdateRange(DWORD nMin = 0, DWORD nMax = 0xFFFFFFFF);
	void		ClearUpdated();
	void		ClearNew();
	void		Serialize(CArchive& ar);
	void		SanityCheck();

	CBaseMatchWnd* GetParent() const
	{
		return m_pParent;
	}

protected:
	CMatchFile* FindFileAndAddHit(CQueryHit* pHit, const findType nFindFlag, FILESTATS* Stats);
	void		InsertSorted(CMatchFile* pFile);
	BOOL		FilterHit(CQueryHit* pHit);
	
	friend class CMatchFile;

};


class CMatchFile : public CPeerProjectFile
{
public:
	CMatchFile(CMatchList* pList, CQueryHit* pHit = NULL);
	virtual ~CMatchFile();
	
public:
	CMatchList*	m_pList;
	DWORD		m_nTotal;
	DWORD		m_nFiltered;
	DWORD		m_nSources;
	CMatchFile*	m_pNextSize;
	CMatchFile*	m_pNextSHA1;
	CMatchFile*	m_pNextTiger;
	CMatchFile*	m_pNextED2K;
	CMatchFile*	m_pNextBTH;
	CMatchFile*	m_pNextMD5;
	CString		m_sSize;
	TRISTATE	m_bBusy;
	TRISTATE	m_bPush;
	TRISTATE	m_bStable;
	BOOL		m_bPreview;
	DWORD		m_nSpeed;
	CString		m_sSpeed;
	int			m_nRating;				// Total value of all ratings
	int			m_nRated;				// Number of ratings recieved
	BOOL		m_bDRM;					// Appears to have DRM
	BOOL		m_bSuspicious;			// Appears to be a suspicious file (small exe, vbs, etc)
	BOOL		m_bCollection;			// Appears to be a collection
	BOOL		m_bTorrent;				// Appears to be a torrent
	BOOL		m_bExpanded;
	BOOL		m_bSelected;
	BOOL		m_bDownload;
	BOOL		m_bNew;
	BOOL		m_bOneValid;
	int			m_nShellIndex;
	CString*	m_pColumns;
	int			m_nColumns;
	BYTE*		m_pPreview;
	DWORD		m_nPreview;
	CTime		m_pTime;				// Found time

	BOOL		Add(CQueryHit* pHit, BOOL bForce = FALSE);
	BOOL		Check(CQueryHit* pHit) const;
	BOOL		Expand(BOOL bExpand = TRUE);
	inline int	Compare(CMatchFile* pFile) const;
	CString		GetURN() const;
	void		Serialize(CArchive& ar, int nVersion);
	void		Ban(int nBanLength);	// Ban by hashes and by hit host IPs
	
	inline DWORD GetFilteredCount()
	{
		if ( ( m_pList->m_bFilterDRM && m_bDRM ) ||
			( m_pList->m_bFilterSuspicious && m_bSuspicious ) ||
			( m_nSources < m_pList->m_nFilterSources ) ||
			( m_pBest == NULL ) ||
			( m_pList->m_bFilterLocal && GetLibraryStatus() == TRI_FALSE ) ) return 0;

		return m_nFiltered;
	}
	
	inline DWORD GetItemCount()
	{
		if ( ( m_pList->m_bFilterDRM && m_bDRM ) ||
			( m_pList->m_bFilterSuspicious && m_bSuspicious ) ||
			( m_nSources < m_pList->m_nFilterSources ) ||
			( m_pBest == NULL ) ||
			( m_pList->m_bFilterLocal && GetLibraryStatus() == TRI_FALSE ) ) return 0;

		if ( m_nFiltered == 1 || ! m_bExpanded )
			return 1;
		else
			return m_nFiltered + 1;
	}
	
//	int			GetRating() const;
	DWORD		Filter();
	void		Added(CQueryHit* pHit);
	void		ClearNew();

	// Access to Hits list first element.
	// Use with CAUTION. If Hit was changed then certainly call RefreshStatus().
	CQueryHit*	GetHits() const;

	// Access to best Hit.
	// Use with CAUTION. If Hit was changed then certainly call RefreshStatus().
	CQueryHit*	GetBest() const;

	// Refresh file status (name, uri, etc.) in accord with Hits list
	void		RefreshStatus();

	// Count bogus status setted Hits
	DWORD		GetBogusHitsCount() const;

	// Count Hits
	DWORD		GetTotalHitsCount() const;

	// Sum Hits speeds
	DWORD		GetTotalHitsSpeed() const;

	// Get first available Hits Schema
	CSchema*	GetHitsSchema() const;

	// Change Hits bogus status
	void		SetBogus( BOOL bBogus = TRUE );

	// Clear selection of file itself and all Hits
	BOOL		ClearSelection();

	// File has hits
	BOOL		IsValid() const;

	// Get partial count of best Hit
	DWORD		GetBestPartial() const;
	
	// Get rating of best Hit
	int			GetBestRating() const;

	// Get address of best Hit
	IN_ADDR		GetBestAddress() const;

	// Get vendor name of best Hit
	LPCTSTR		GetBestVendorName() const;

	// Get country code of best Hit
	LPCTSTR		GetBestCountry() const;

	// Get schema of best Hit
	LPCTSTR		GetBestSchemaURI() const;
	
	// Get measured of best Hit
	TRISTATE	GetBestMeasured() const;

	// Get browse host flag of best Hit
	BOOL		GetBestBrowseHost() const;
	
	// Is this file known (i.e. exist in Library)?
	// TRI_UNKNOWN	- Not
	// TRI_FALSE		- Yes
	// TRI_TRUE		- Yes, Ghost
	TRISTATE	GetLibraryStatus();

	// Get some data for interface
	void		GetQueueTip(CString& sPartial) const;
	void		GetPartialTip(CString& sQueue) const;
	void		GetUser(CString& sUser) const;
	void		GetStatusTip(CString& sStatus, COLORREF& crStatus);

	// Output some data
	void		AddHitsToDownload(CDownload* pDownload, BOOL bForce = FALSE) const;
	void		AddHitsToXML(CXMLElement* pXML) const;
	CSchema*	AddHitsToMetadata(CMetaList& oMetadata) const;
	BOOL		AddHitsToPreviewURLs(CList < CString > & oPreviewURLs) const;
	void		AddHitsToReviews(CList < Review* >& oReviews) const;

	void		SanityCheck();

protected:
	CQueryHit*	m_pHits;
	CQueryHit*	m_pBest;
	TRISTATE	m_bLibraryStatus;

protected:
	TRISTATE	m_bExisting;
};


#endif // !defined(MATCHOBJECTS_H)
