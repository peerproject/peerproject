//
// Download.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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

#define DOWNLOAD_SER_VERSION	1000	// 42+
// nVersion History:
// 33 - added m_sSearchKeyword to CDownloadBase (CyberBob)
// 34 - added m_bSeeding and m_sServingFileName to CDownloadWithTorrent (Rolandas)
// 35 - added m_sCountry to CDownloadSource (dcat)
// 36 - nothing (Rolandas) (Shareaza 2.2.4.0?)
// 37 - added m_oBTH to CDownloadBase, m_bBTH and m_bMD5 to CDownloadSource (Ryo-oh-ki)
// 38 - added m_sCountryName to CDownloadSource (dcat)
// 39 - added m_bClientExtended to CDownloadSource (Ryo-oh-ki)
// 40 - added virtual fragmented file (Ryo-oh-ki)
// 41 - added m_sName to CFragmentedFile (Ryo-oh-ki)
// 42 - added m_bMetaIgnore to CDownloadSource (Ry-oh-ki) (Shareaza 2.5.2.0)
// 1000 - add m_tDate, remove sSearchKeyword in CDownloadBase	(PeerProject 1.0) (42+)

#include "DownloadWithExtras.h"

class CDownload : public CDownloadWithExtras
{
// Construction
public:
	CDownload();
	virtual ~CDownload();

// Attributes
public:
	DWORD		m_nSerID;
	BOOL		m_bExpanded;
	BOOL		m_bSelected;
	DWORD		m_tCompleted;
	int			m_nRunCookie;
	int			m_nGroupCookie;
	BOOL		m_bClearing;	// Briefly marked for removal or deletion (rarely visible, but may take longer than expected)
private:
	BOOL		m_bTempPaused;
	BOOL		m_bPaused;
	BOOL		m_bBoosted;
	BOOL		m_bShared;
	bool		m_bComplete;
	bool		m_bDownloading;	// Store if a download is downloading, as performance tweak. Count transfers for 100% current answer.
	DWORD		m_tBegan;		// Time when this download began trying to download (Started searching, etc). 0 means not tried this session.
	DWORD		m_tSaved;
// Operations
public:
	void		Pause(BOOL bRealPause = TRUE);
	void		Resume();
	void		Remove();
	void		Boost();
	void		Share(BOOL bShared);
	bool		IsStarted() const;		// Has the download actually downloaded anything?
	bool		IsDownloading() const;	// Is the download receiving data?
	bool		IsBoosted() const;
	bool		IsShared() const;
	CString		GetDownloadSources() const;
	CString		GetDownloadStatus() const;
	int			GetClientStatus() const;
	void		ForceComplete();
	BOOL		Launch(int nIndex, CSingleLock* pLock, BOOL bForceOriginal);
	BOOL		Enqueue(int nIndex, CSingleLock* pLock);
	BOOL		Load(LPCTSTR pszPath);
	BOOL		Save(BOOL bFlush = FALSE);
	void		OnTaskComplete(const CDownloadTask* pTask);
	void		OnRun();
private:
	void		StartTrying();
	void		StopTrying();
	DWORD		GetStartTimer() const;
	void		OnDownloaded();
	void		OnMoved();
//	void		SerializeOld(CArchive& ar, int nVersion);	// Legacy DOWNLOAD_SER_VERSION < 11, for reference

// Overrides
public:
	virtual bool IsPaused(bool bRealState = false) const;
	virtual bool IsCompleted() const;
	virtual bool IsTrying() const;		// Is the download currently trying to download?
	virtual void Serialize(CArchive& ar, int nVersion);	// DOWNLOAD_SER_VERSION

	friend class CDownloadTransfer;		// GetVerifyLength
	friend class CDownloadWithTorrent;	// m_bComplete
	friend class CDownloadsWnd;			// m_pTask
	friend class CDownloads;			// m_bComplete for Load()
};
