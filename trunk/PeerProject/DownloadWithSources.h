//
// DownloadWithSources.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#include "DownloadBase.h"

class CDownloadSource;
class CQueryHit;
class CXMLElement;

class CFailedSource
{
public:
	CFailedSource(LPCTSTR pszURL, bool bLocal=true, bool bOffline=false)
		: m_nTimeAdded( GetTickCount() )
		, m_nPositiveVotes( 0 )
		, m_nNegativeVotes( 0 )
		, m_sURL( pszURL )
		, m_bLocal( bLocal )
		, m_bOffline( bOffline ) { }

	virtual ~CFailedSource() { };

	DWORD	m_nTimeAdded;
	int		m_nPositiveVotes;
	int		m_nNegativeVotes;
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
	CDownloadSource*	m_pSourceFirst;
	CList< CFailedSource* >	m_pFailedSources; // Failed source with a timestamp when added
	CDownloadSource*	m_pSourceLast;
	int					m_nG1SourceCount;
	int					m_nG2SourceCount;
	int					m_nEdSourceCount;
	int					m_nHTTPSourceCount;
	int					m_nBTSourceCount;
	int					m_nFTPSourceCount;
	int					m_nSourceCount;
	CXMLElement*		m_pXML;

// Operations
public:
	CString				GetSourceURLs(CList< CString >* pState, int nMaximum, PROTOCOLID nProtocol, CDownloadSource* pExcept);
	CString				GetTopFailedSources(int nMaximum, PROTOCOLID nProtocol);
	DWORD				GetEffectiveSourceCount() const;
	DWORD				GetSourceCount(BOOL bNoPush = FALSE, BOOL bSane = FALSE) const;
	DWORD				GetBTSourceCount(BOOL bNoPush = FALSE) const;
	DWORD				GetED2KCompleteSourceCount() const;
	BOOL				CheckSource(CDownloadSource* pSource) const;
	void				AddFailedSource(CDownloadSource* pSource, bool bLocal = true, bool bOffline = false);
	void				AddFailedSource(LPCTSTR pszUrl, bool bLocal = true, bool bOffline = false);
	CFailedSource*		LookupFailedSource(LPCTSTR pszUrl, bool bReliable = false);
	void				ExpireFailedSources();
	void				ClearSources();
	void				ClearFailedSources();
	void				MergeMetadata(const CXMLElement* pXML);
	BOOL				AddSourceHit(CQueryHit* pHit, BOOL bForce = FALSE);
	BOOL				AddSourceED2K(DWORD nClientID, WORD nClientPort, DWORD nServerIP, WORD nServerPort, const Hashes::Guid& oGUID);
    BOOL				AddSourceBT(const Hashes::BtGuid& oGUID, IN_ADDR* pAddress, WORD nPort);
	BOOL				AddSourceURL(LPCTSTR pszURL, BOOL bURN = FALSE, FILETIME* pLastSeen = NULL, int nRedirectionCount = 0, BOOL bFailed = FALSE);
	int					AddSourceURLs(LPCTSTR pszURLs, BOOL bURN = FALSE, BOOL bFailed = FALSE);
	void				RemoveSource(CDownloadSource* pSource, BOOL bBan);
	virtual BOOL		OnQueryHits(CQueryHit* pHits);
	virtual void		Serialize(CArchive& ar, int nVersion);
	int					GetSourceColor();

// Implementation
protected:
	void            RemoveOverlappingSources(QWORD nOffset, QWORD nLength);
	BOOL		    AddSourceInternal(CDownloadSource* pSource);
	void		    SortSource(CDownloadSource* pSource, BOOL bTop);
	void		    SortSource(CDownloadSource* pSource);

private:
	void				VoteSource(LPCTSTR pszUrl, bool bPositively);

public:
	inline CDownloadSource* GetFirstSource() const
	{
		return m_pSourceFirst;
	}

	inline int GetCount() const
	{
		return m_nSourceCount;
	}

	inline CXMLElement* GetMetadata() const
	{
		return m_pXML;
	}
};