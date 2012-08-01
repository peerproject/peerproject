//
// DownloadWithSources.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#include "DownloadBase.h"

class CDownloadSource;
class CQueryHit;
class CMatchFile;
class CPeerProjectURL;
class CXMLElement;

class CFailedSource
{
public:
	CFailedSource(LPCTSTR pszURL, bool bLocal=true, bool bOffline=false)
		: m_nTimeAdded		( GetTickCount() )
		, m_nPositiveVotes	( 0 )
		, m_nNegativeVotes	( 0 )
		, m_sURL			( pszURL )
		, m_bLocal			( bLocal )
		, m_bOffline		( bOffline )
	{
	}

	DWORD	m_nTimeAdded;
	int 	m_nPositiveVotes;
	int 	m_nNegativeVotes;
	CString	m_sURL;
	bool	m_bLocal;
	bool	m_bOffline;
};

class CDownloadWithSources : public CDownloadBase
{
// Construction
protected:
	CDownloadWithSources();
	virtual ~CDownloadWithSources();

// Attributes
private:
	CList< CDownloadSource* >	m_pSources;		// Download sources
	CList< CFailedSource* >	m_pFailedSources;	// Failed source with a timestamp when added
	int				m_nG1SourceCount;
	int				m_nG2SourceCount;
	int				m_nEdSourceCount;
	int				m_nDCSourceCount;
	int				m_nBTSourceCount;
	int				m_nHTTPSourceCount;
	int				m_nFTPSourceCount;

// Operations
public:
	CXMLElement*	m_pXML;

	CString			GetSourceURLs(CList< CString >* pState, int nMaximum, PROTOCOLID nProtocol, CDownloadSource* pExcept);
	CString			GetTopFailedSources(int nMaximum, PROTOCOLID nProtocol);
	DWORD			GetEffectiveSourceCount() const;
	DWORD			GetSourceCount(BOOL bNoPush = FALSE, BOOL bSane = FALSE) const;
	DWORD			GetBTSourceCount(BOOL bNoPush = FALSE) const;
	DWORD			GetED2KCompleteSourceCount() const;
	BOOL			CheckSource(CDownloadSource* pSource) const;
	CFailedSource*	LookupFailedSource(LPCTSTR pszUrl, bool bReliable = false);
	void			AddFailedSource(const CDownloadSource* pSource, bool bLocal = true, bool bOffline = false);
	void			AddFailedSource(LPCTSTR pszUrl, bool bLocal = true, bool bOffline = false);
	void			ExpireFailedSources();
	void			ClearSources();
	void			ClearFailedSources();
	void			MergeMetadata(const CXMLElement* pXML);
	BOOL			AddSourceHit(const CQueryHit* pHit, BOOL bForce = FALSE);
	BOOL			AddSourceHit(const CMatchFile* pMatchFile, BOOL bForce = FALSE);
	BOOL			AddSourceHit(const CPeerProjectURL& oURL, BOOL bForce = FALSE);
	BOOL			AddSourceED2K(DWORD nClientID, WORD nClientPort, DWORD nServerIP, WORD nServerPort, const Hashes::Guid& oGUID);
	BOOL			AddSourceBT(const Hashes::BtGuid& oGUID, const IN_ADDR* pAddress, WORD nPort);
	BOOL			AddSourceURL(LPCTSTR pszURL, BOOL bURN = FALSE, FILETIME* pLastSeen = NULL, int nRedirectionCount = 0, BOOL bFailed = FALSE);
	int				AddSourceURLs(LPCTSTR pszURLs, BOOL bURN = FALSE, BOOL bFailed = FALSE);
	void			RemoveSource(const CDownloadSource* pSource, BOOL bBan);
					// Remove source from list, add it to failed sources if bBan == TRUE, and destroy source itself

	virtual BOOL	OnQueryHits(const CQueryHit* pHits);
	virtual void	Serialize(CArchive& ar, int nVersion);	// DOWNLOAD_SER_VERSION
	int				GetSourceColor();

public:
	CDownloadSource* GetNext(POSITION& rPosition) const;	// Get next source
	POSITION		GetIterator() const;					// Get source iterator (first source position)
	INT_PTR 		GetCount() const;						// Get source count

	bool			HasMetadata() const;

// Implementation
protected:
	BOOL			AddSource(const CPeerProjectFile* pHit, BOOL bForce = FALSE);
	BOOL			AddSourceInternal(CDownloadSource* pSource);
	void			RemoveOverlappingSources(QWORD nOffset, QWORD nLength);
	void			SortSource(CDownloadSource* pSource, BOOL bTop);
	void			SortSource(CDownloadSource* pSource);
	void			InternalAdd(const CDownloadSource* pSource);		// Add new source to list, update counters
	void			InternalRemove(const CDownloadSource* pSource);		// Remove existing source from list, update counters
	void			VoteSource(LPCTSTR pszUrl, bool bPositively);
};
