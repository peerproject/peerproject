//
// Downloads.h
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

class CDownload;
class CDownloadSource;
class CConnection;
class CQueryHit;
class CMatchFile;
class CBuffer;
class CPeerProjectURL;
class CEDClient;


class CDownloads
{
public:
	enum { dlPathNull, dlPathComplete, dlPathIncomplete };

// Construction
public:
	CDownloads();
	virtual ~CDownloads();

// Attributes
public:
	DWORD			m_tBandwidthAtMax;			// The last time download bandwidth was all in use
	DWORD			m_tBandwidthAtMaxED2K;		// The last time all ed2k bandwidth was used
	DWORD			m_nTransfers;
	DWORD			m_nBandwidth;
	DWORD			m_tLastConnect;
	bool			m_bClosing;
private:
	CList< CDownload* >	m_pList;
	CMap< ULONG, ULONG, DWORD, DWORD > m_pHostLimits;
	int				m_nRunCookie;
	DWORD			m_tBandwidthLastCalc;		// The last time the bandwidth was calculated
	DWORD			m_nLimitGeneric;
	DWORD			m_nLimitDonkey;
	DWORD			m_nTryingCount;
	DWORD			m_nBTTryingCount;
	bool			m_bAllowMoreDownloads;
	bool			m_bAllowMoreTransfers;

// Operations
public:
	CDownload*	Add();
	CDownload*	Add(CQueryHit* pHit, BOOL bAddToHead = FALSE);
	CDownload*	Add(CMatchFile* pFile, BOOL bAddToHead = FALSE);
	CDownload*	Add(const CPeerProjectURL& oURL);
	void		PauseAll();
	void		ClearCompleted();
	void		ClearPaused();
	void		Clear(bool bClosing = false);
	void		CloseTransfers();
	void		StartTrying(bool bIsTorrent = false);
	void		StopTrying(bool bIsTorrent = false);

	int			GetSeedCount() const;
	DWORD		GetCount(BOOL bActiveOnly = FALSE) const;
	DWORD		GetTryingCount(bool bTorrentsOnly = false) const;
	DWORD		GetConnectingTransferCount() const;
	BOOL		Check(CDownloadSource* pSource) const;
	BOOL		CheckActive(CDownload* pDownload, int nScope) const;
	BOOL		Move(CDownload* pDownload, int nDelta);
	BOOL		Reorder(CDownload* pDownload, CDownload* pBefore);
	QWORD		GetAmountDownloadedFrom(IN_ADDR* pAddress);

	CDownload*	FindByPath(const CString& sPath) const;
	CDownload*	FindByURN(LPCTSTR pszURN, BOOL bSharedOnly = FALSE) const;
	CDownload*	FindBySHA1(const Hashes::Sha1Hash& oSHA1, BOOL bSharedOnly = FALSE) const;
	CDownload*	FindByTiger(const Hashes::TigerHash& oTiger, BOOL bSharedOnly = FALSE) const;
	CDownload*	FindByED2K(const Hashes::Ed2kHash& oED2K, BOOL bSharedOnly = FALSE) const;
	CDownload*	FindByBTH(const Hashes::BtHash& oBTH, BOOL bSharedOnly = FALSE) const;
	CDownload*	FindByMD5(const Hashes::Md5Hash& oMD5, BOOL bSharedOnly = FALSE) const;
	CDownload*	FindBySID(DWORD nSerID) const;
	DWORD		GetFreeSID();

	void		Load();
	void		Save(BOOL bForce = TRUE);
	void		OnRun();
	BOOL		OnPush(const Hashes::Guid& oGUID, CConnection* pConnection);
	bool		OnQueryHits(const CQueryHit* pHits);
	void		OnVerify(LPCTSTR pszPath, BOOL bVerified);
	void		SetPerHostLimit(IN_ADDR* pAddress, DWORD nLimit);
	BOOL		IsSpaceAvailable(QWORD nVolume, int nPath = dlPathNull);

	void		UpdateAllows();
	bool		AllowMoreDownloads() const;
	bool		AllowMoreTransfers() const;
	bool		AllowMoreTransfers(IN_ADDR* pAdress) const;
	void		Remove(CDownload* pDownload);

	POSITION	GetIterator() const;
	POSITION	GetReverseIterator() const;
	CDownload*	GetNext(POSITION& pos) const;
	CDownload*	GetPrevious(POSITION& pos) const;
	BOOL		Check(CDownload* pDownload) const;
private:
	int			GetActiveTorrentCount() const;
	DWORD		GetTransferCount() const;
	BOOL		Swap(CDownload* p1, CDownload* p2);
	DWORD		GetBandwidth() const;
	BOOL		OnDonkeyCallback(CEDClient* pClient, CDownloadSource* pExcept = NULL);
	void		LoadFromCompoundFiles();
	BOOL		LoadFromCompoundFile(LPCTSTR pszFile);
	BOOL		LoadFromTimePair();
	void		SerializeCompound(CArchive& ar);
	void		PurgePreviews();
};

extern CDownloads Downloads;
