//
// DownloadWithTorrent.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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

#include "DownloadWithFile.h"
#include "BTTrackerRequest.h"
#include "BTInfo.h"

class CDownloadTransferBT;
class CUploadTransferBT;
class CBTClient;
class CBTPacket;


class CDownloadWithTorrent : public CDownloadWithFile, public CTrackerEvent
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

// Operations
public:
	void			AddUpload(CUploadTransferBT* pUpload);
	void			RemoveUpload(CUploadTransferBT* pUpload);
	float			GetRatio() const;
	bool			IsSeeding() const;
	bool			IsTorrent() const;
	bool			IsSingleFileTorrent() const;
	bool			IsMultiFileTorrent() const;
	BOOL			UploadExists(in_addr* pIP) const;
	BOOL			UploadExists(const Hashes::BtGuid& oGUID) const;
	virtual void	OnTrackerEvent(bool bSuccess, LPCTSTR pszReason, LPCTSTR pszTip, CBTTrackerRequest* pEvent);
	void			ChokeTorrent(DWORD tNow = 0);
	CDownloadTransferBT*	CreateTorrentTransfer(CBTClient* pClient);
	CBTPacket*		CreateBitfieldPacket();
	BOOL			GenerateTorrentDownloadID();			// Generate Peer ID
	// Apply new .torrent file to download or update from existing one
	BOOL			SetTorrent(const CBTInfo* pTorrent = NULL);

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
};
