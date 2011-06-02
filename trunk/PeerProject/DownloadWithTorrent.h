//
// DownloadWithTorrent.h
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

#include "DownloadWithFile.h"
#include "BTTrackerRequest.h"
#include "BTInfo.h"

class CDownloadTransferBT;
class CUploadTransferBT;
class CBTClient;
class CBTPacket;


class CDownloadWithTorrent : public CDownloadWithFile
{
// Construction
protected:
	CDownloadWithTorrent();
	virtual ~CDownloadWithTorrent();

// Attributes
public:
	CBTInfo		m_pTorrent;
	bool		m_bTorrentEndgame;
	BOOL		m_bTorrentRequested;
	BOOL		m_bTorrentStarted;
	DWORD		m_tTorrentTracker;
	DWORD		m_tTorrentSources;
	QWORD		m_nTorrentUploaded;
	QWORD		m_nTorrentDownloaded;
	BOOL		m_bTorrentTrackerError;
	CString		m_sTorrentTrackerError;
	CString		m_sKey;
	Hashes::BtGuid m_pPeerID;
protected:
	BOOL		m_bSeeding;
	DWORD		m_nTorrentBlock;
	DWORD		m_nTorrentSuccess;
	DWORD		m_nTorrentSize;
	BYTE*		m_pTorrentBlock;
private:
	CList< CUploadTransferBT* >	m_pTorrentUploads;
	DWORD						m_tTorrentChoke;

	CList< CBTTrackerRequest* >	m_pRequests;			// In-process tracker requests
	CMutex						m_pRequestsSection; 	// m_pRequests guard

// Operations
public:
	void			AddUpload(CUploadTransferBT* pUpload);
	void			RemoveUpload(CUploadTransferBT* pUpload);
	BOOL			SeedTorrent(CString& sErrorMessage);
	float			GetRatio() const;
	bool			IsSeeding() const;
	bool			IsTorrent() const;
	bool			IsSingleFileTorrent() const;
	bool			IsMultiFileTorrent() const;
	BOOL			UploadExists(in_addr* pIP) const;
	BOOL			UploadExists(const Hashes::BtGuid& oGUID) const;
	void			OnTrackerEvent(bool bSuccess, LPCTSTR pszReason, LPCTSTR pszTip = NULL);
	void			ChokeTorrent(DWORD tNow = 0);
	CDownloadTransferBT*	CreateTorrentTransfer(CBTClient* pClient);
	CBTPacket*		CreateBitfieldPacket();
	BOOL			SetTorrent(const CBTInfo& oTorrent);
	BOOL			GenerateTorrentDownloadID();			// Generate Peer ID
protected:
	bool			RunTorrent(DWORD tNow);
	void			SendCompleted();
	void			CloseTorrent();
	void			CloseTorrentUploads();
	BOOL 			CheckTorrentRatio() const;
	void			OnFinishedTorrentBlock(DWORD nBlock);
	virtual BOOL	FindMoreSources();
	virtual void	Serialize(CArchive& ar, int nVersion);
private:
	TCHAR			GenerateCharacter() const;
	DWORD			GetRetryTime() const;
	void			SendStarted(DWORD nNumWant);
	void			SendUpdate(DWORD nNumWant);
	void			SendStopped();
	void			Add(CBTTrackerRequest* pRequest);		// Add tracker request for counting
	void			Remove(CBTTrackerRequest* pRequest);	// Remove tracker request

	friend class CBTTrackerRequest;		// Add(),Remove()
};
