//
// DownloadSource.h
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

#include "FileFragments.hpp"
#include "DownloadTransfer.h"
#include "Download.h"

class CQueryHit;
class CEDClient;

class CDownloadSource
{
// Construction
public:
	CDownloadSource(const CDownload* pDownload);
	CDownloadSource(const CDownload* pDownload, const CQueryHit* pHit);
	CDownloadSource(const CDownload* pDownload, DWORD nClientID, WORD nClientPort, DWORD nServerIP, WORD nServerPort, const Hashes::Guid& oGUID);
	CDownloadSource(const CDownload* pDownload, const Hashes::BtGuid& oGUID, IN_ADDR* pAddress, WORD nPort);
	CDownloadSource(const CDownload* pDownload, LPCTSTR pszURL, BOOL bSHA1 = FALSE, BOOL bHashAuth = FALSE, FILETIME* pLastSeen = NULL, int nRedirectionCount = 0);
	virtual ~CDownloadSource();
private:
	inline void Construct(const CDownload* pDownload);

// Attributes
public:
	CDownload*			m_pDownload;
	CDownloadTransfer*	m_pTransfer;
	BOOL				m_bSelected;
public:
	CString				m_sURL;
	PROTOCOLID			m_nProtocol;
	Hashes::Guid		m_oGUID;
	IN_ADDR				m_pAddress;
	WORD				m_nPort;
	IN_ADDR				m_pServerAddress;
	WORD				m_nServerPort;
	CString				m_sCountry;
	CString				m_sCountryName;
public:
	CString				m_sName;
	DWORD				m_nIndex;
	BOOL				m_bHashAuth;
	BOOL				m_bSHA1;
	BOOL				m_bTiger;
	BOOL				m_bED2K;
	BOOL				m_bBTH;
	BOOL				m_bMD5;
public:
	CString				m_sServer;
	CString				m_sNick;
	DWORD				m_nSpeed;
	BOOL				m_bPushOnly;
	BOOL				m_bCloseConn;
	BOOL				m_bReadContent;
	FILETIME			m_tLastSeen;
	// Gnutella functionality:
	// 0 - Pure HTTP
	// 1 - Pure G1
	// 2 - Pure G2
	// 3 - Mixed G1/G2
	int					m_nGnutella;
	BOOL				m_bClientExtended;		// Does the user support extended (G2) functions? (In practice, this means can we use G2 chat, browse, etc...)
public:
	DWORD				m_nSortOrder;			// How should this source be sorted in the list?
	int					m_nColor;
	DWORD				m_tAttempt;
	BOOL				m_bKeep;				// Source keeped by NeverDrop == TRUE flag
	int					m_nFailures;			// failure count.
	int					m_nBusyCount;			// busy count. (used for incrementing RetryDelay)
	int					m_nRedirectionCount;
	Fragments::List		m_oAvailable;
	Fragments::List		m_oPastFragments;
public:
	BOOL				m_bPreview;				// Does the user allow previews?
	CString				m_sPreview;				// if empty it has the default /gnutella/preview/v1?urn:xyz format
	BOOL				m_bPreviewRequestSent;

// Operations
public:
	BOOL		ResolveURL();
	void		Serialize(CArchive& ar, int nVersion);
public:
	BOOL		CanInitiate(BOOL bNetwork, BOOL bEstablished);
	void		Remove(BOOL bCloseTransfer, BOOL bBan);
	void		RemoveIf(BOOL bCloseTransfer, BOOL bBan);
	void		OnFailure(BOOL bNondestructive, DWORD nRetryAfter = 0);
	DWORD		CalcFailureDelay(DWORD nRetryAfter = 0) const;
	void		OnResume();
	void		OnResumeClosed();
public:
	void		SetValid();
	void		SetLastSeen();
	void		SetGnutella(int nGnutella);
	BOOL		CheckHash(const Hashes::Sha1Hash& oSHA1);
	BOOL		CheckHash(const Hashes::TigerHash& oTiger);
	BOOL		CheckHash(const Hashes::Ed2kHash& oED2K);
	BOOL		CheckHash(const Hashes::BtHash& oBTH);
	BOOL		CheckHash(const Hashes::Md5Hash& oMD5);
public:
	BOOL		PushRequest();
	BOOL		CheckPush(const Hashes::Guid& oClientID);
	BOOL		CheckDonkey(CEDClient* pClient);
public:
	void		AddFragment(QWORD nOffset, QWORD nLength, BOOL bMerge = FALSE);
	void		SetAvailableRanges(LPCTSTR pszRanges);
	BOOL		HasUsefulRanges() const;
	BOOL		TouchedRange(QWORD nOffset, QWORD nLength) const;
	int			GetColor();

	CDownloadTransfer*	CreateTransfer();

// Inlines
public:
	inline bool CDownloadSource::Equals(CDownloadSource* pSource) const
	{
		if ( m_oGUID.isValid() && pSource->m_oGUID.isValid() )
			return m_oGUID == pSource->m_oGUID;

		if ( m_nServerPort != pSource->m_nServerPort )
		{
			return FALSE;
		}
		else if ( m_nServerPort > 0 )	// Push
		{
			if ( m_pServerAddress.S_un.S_addr != pSource->m_pServerAddress.S_un.S_addr ) return FALSE;
			if ( m_pAddress.S_un.S_addr != pSource->m_pAddress.S_un.S_addr ) return FALSE;
		}
		else	// Direct
		{
			if ( m_pAddress.S_un.S_addr != pSource->m_pAddress.S_un.S_addr ) return FALSE;
			if ( m_nPort != pSource->m_nPort ) return FALSE;
		}

		return TRUE;
	}

	inline bool IsOnline() const
	{
		return m_nBusyCount || ( m_pTransfer && m_pTransfer->m_nState > dtsConnecting );
	}

	inline bool IsHTTPSource() const
	{
		return ( m_nProtocol == PROTOCOL_HTTP || m_nProtocol == PROTOCOL_G2 );
	}
};
