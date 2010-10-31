//
// UploadTransferED2K.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "UploadTransfer.h"
#include "FileFragments.hpp"

class CEDClient;
class CEDPacket;

class CUploadTransferED2K : public CUploadTransfer
{
// Construction
public:
	CUploadTransferED2K(CEDClient* pClient);
	virtual ~CUploadTransferED2K();

// Attributes
public:
	CEDClient*		m_pClient;					// The remote client.
	DWORD			m_tRequest;					// The time a request was sent.
	int				m_nRanking;					// The last queue position the remote client was sent.
	DWORD			m_tRankingSent;				// The time a queue ranking packet was last sent.
	DWORD			m_tRankingCheck;			// The time the queue position was last checked.
	DWORD			m_tLastRun;					// The time the transfer was last run
private:
	Fragments::Queue m_oRequested;
	Fragments::Queue m_oServed;

// Operations
public:
	BOOL			Request(const Hashes::Ed2kHash& oED2K);
	virtual void	Close(BOOL bMessage = FALSE);
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual void	OnQueueKick();
	virtual DWORD	GetMeasuredSpeed();
public:
	BOOL	OnRunEx(DWORD tNow);
	BOOL	OnQueueRelease(CEDPacket* pPacket);
	BOOL	OnRequestParts(CEDPacket* pPacket);
	BOOL	OnReask();
protected:
	void	Cleanup(BOOL bDequeue = TRUE);
	void	Send(CEDPacket* pPacket, BOOL bRelease = TRUE);
	BOOL	CheckRanking();		// Check the client's Q rank. Start upload or send notification if required.
	void	AddRequest(QWORD nOffset, QWORD nLength);
	BOOL	ServeRequests();
	BOOL	StartNextRequest();
	BOOL	DispatchNextChunk();
	BOOL	CheckFinishedRequest();

public:
	BOOL	OnRequestParts64(CEDPacket* pPacket);		// 64bit Large file support
};
