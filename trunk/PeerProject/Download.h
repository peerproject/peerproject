//
// Download.h
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

#define DOWNLOAD_SER_VERSION	1000	//42
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
// 1000 - (PeerProject 1.0) (42)

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
private:
	BOOL		m_bTempPaused;
	BOOL		m_bPaused;
	BOOL		m_bBoosted;
	BOOL		m_bShared;
	bool		m_bComplete;
	DWORD		m_tSaved;
	DWORD		m_tBegan;		// The time when this download began trying to download
								// (Started searching, etc). 0 means not tried this session.
	bool		m_bDownloading;	// This is used to store if a download is downloading. (Performance tweak)
								// You should count the transfers if you need a 100% current answer.
// Operations
public:
	void		Pause(BOOL bRealPause = TRUE);
	void		Resume();
	void		Remove(bool bDelete = false);
	void		Boost();
	void		Share(BOOL bShared);
	bool		IsStarted() const;		// Has the download actually downloaded anything?
	bool		IsDownloading() const;	// Is the download receiving data?
	bool		IsBoosted() const;
	bool		IsShared() const;
	BOOL		Load(LPCTSTR pszPath);
	BOOL		Save(BOOL bFlush = FALSE);
	void		OnRun();
	void		ForceComplete();
	BOOL		Launch(int nIndex, CSingleLock* pLock, BOOL bForceOriginal);
	BOOL		Enqueue(int nIndex, CSingleLock* pLock);
	void		OnTaskComplete(const CDownloadTask* pTask);
private:
	void		StartTrying();
	void		StopTrying();
	DWORD		GetStartTimer() const;
	void		OnDownloaded();
	void		OnMoved();
//	void		SerializeOld(CArchive& ar, int nVersion);	// For DOWNLOAD_SER_VERSION < 11

// Overrides
public:
	virtual bool	IsPaused(bool bRealState = false) const;
	virtual bool	IsCompleted() const;
	virtual bool	IsTrying() const;	//Is the download currently trying to download?
	virtual void	Serialize(CArchive& ar, int nVersion);	// DOWNLOAD_SER_VERSION

	friend class CDownloadTransfer;		// GetVerifyLength
	friend class CDownloadWithTorrent;	// m_bComplete
	friend class CDownloadsWnd;			// m_pTask
	friend class CDownloads;			// m_bComplete for Load()
};
