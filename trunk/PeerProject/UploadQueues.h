//
// UploadQueues.h
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

class CUploadQueue;
class CUploadTransfer;

class CLibraryFile;
class CDownload;


class CUploadQueues
{
// Construction
public:
	CUploadQueues();
	virtual ~CUploadQueues();

// Attributes
public:
	CMutex				m_pSection;
	CUploadQueue*		m_pTorrentQueue;
	CUploadQueue*		m_pHistoryQueue;
protected:
	CList< CUploadQueue* > m_pList;

	BOOL				m_bDonkeyLimited;

// Operations
public:
	BOOL	Enqueue(CUploadTransfer* pUpload, BOOL bForce = FALSE);
	BOOL	Dequeue(CUploadTransfer* pUpload);
	int		GetPosition(CUploadTransfer* pUpload, BOOL bStart);
	BOOL	StealPosition(CUploadTransfer* pTarget, CUploadTransfer* pSource);
public:
	CUploadQueue*	Create(LPCTSTR pszName = NULL, BOOL bTop = FALSE);
	void			Delete(CUploadQueue* pQueue);
	BOOL			Reorder(CUploadQueue* pQueue, CUploadQueue* pBefore);
	CUploadQueue*	SelectQueue(PROTOCOLID nProtocol, CLibraryFile const * const pFile);
	CUploadQueue*	SelectQueue(PROTOCOLID nProtocol, CDownload const * const pFile);
	CUploadQueue*	SelectQueue(PROTOCOLID nProtocol, LPCTSTR pszName, QWORD nSize, DWORD nFileState, LPCTSTR pszShareTags = NULL);
public:
	int		GetTotalBandwidthPoints( BOOL ActiveOnly = FALSE );
	int		GetQueueCapacity();
	INT_PTR	GetQueuedCount();
	INT_PTR	GetQueueRemaining();
	INT_PTR	GetTransferCount();
	BOOL	IsTransferAvailable();
	DWORD	GetMinimumDonkeyBandwidth();
	DWORD	GetCurrentDonkeyBandwidth();
	BOOL	CanUpload(PROTOCOLID nProtocol, CLibraryFile const * const pFile, BOOL bCanQueue = FALSE );	// Can this file be uploaded with the current queue setup?
	DWORD	QueueRank(PROTOCOLID nProtocol, CLibraryFile const * const pFile );	// What queue position would this file be in?

public:
	void	Clear();
	BOOL	Load();
	BOOL	Save();
	void	CreateDefault();
	void	Validate();
protected:
	void	Serialize(CArchive& ar);

// Inline Access
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}

	inline CUploadQueue* GetNext(POSITION& pos) const
	{
		return m_pList.GetNext( pos );
	}

	inline INT_PTR GetCount() const
	{
		return m_pList.GetCount();
	}

	inline BOOL Check(CUploadQueue* pQueue) const
	{
		if ( pQueue == NULL ) return FALSE;
		return m_pList.Find( pQueue ) != NULL;
	}

	inline BOOL	IsDonkeyRatioActive() const 
	{ 
		return ( m_bDonkeyLimited ); 
	}

};

extern CUploadQueues UploadQueues;
