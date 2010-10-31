//
// DownloadTransferED2K.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "DownloadTransfer.h"

class CEDClient;
class CEDPacket;

class CDownloadTransferED2K : public CDownloadTransfer
{
// Construction
public:
	CDownloadTransferED2K(CDownloadSource* pSource);
	virtual ~CDownloadTransferED2K();

// Attributes
public:
	CEDClient*		m_pClient;
	bool			m_bUDP;
	bool			m_bHashset;
	DWORD			m_tRequest;				//When request was last sent
	DWORD			m_tSources;				//When source request was last sent
	DWORD			m_tRanking;				//When queue ranking was last received
	Fragments::Queue m_oRequested;
protected:
	LPVOID			m_pInflatePtr;
	CBuffer*		m_pInflateBuffer;
	QWORD			m_nInflateOffset;
	QWORD			m_nInflateLength;
	QWORD			m_nInflateRead;
	QWORD			m_nInflateWritten;

// Operations
public:
	BOOL	OnRunEx(DWORD tNow);
	BOOL	OnFileReqAnswer(CEDPacket* pPacket);
	BOOL	OnFileNotFound(CEDPacket* pPacket);
	BOOL	OnFileStatus(CEDPacket* pPacket);
	BOOL	OnHashsetAnswer(CEDPacket* pPacket);
	BOOL	OnQueueRank(CEDPacket* pPacket);
	BOOL	OnRankingInfo(CEDPacket* pPacket);
	BOOL	OnFileComment(CEDPacket* pPacket);
	BOOL	OnStartUpload(CEDPacket* pPacket);
	BOOL	OnFinishUpload(CEDPacket* pPacket);
	BOOL	OnSendingPart(CEDPacket* pPacket);
	BOOL	OnCompressedPart(CEDPacket* pPacket);
	void	SetQueueRank(int nRank);
	BOOL	OnSendingPart64(CEDPacket* pPacket);		// 64bit Large file support
	BOOL	OnCompressedPart64(CEDPacket* pPacket);
protected:
	void	Send(CEDPacket* pPacket, BOOL bRelease = TRUE);
	BOOL	SendPrimaryRequest();
	BOOL	SendSecondaryRequest();
	void	ClearRequests();
	BOOL	RunQueued(DWORD tNow);
//	BOOL	SelectFragment(const Fragments::List& oPossible, QWORD& nOffset, QWORD& nLength);

public:
	virtual BOOL	Initiate();
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual void	Close(TRISTATE bKeepSource, DWORD nRetryAfter = 0);
	virtual void	Boost();
	virtual DWORD	GetMeasuredSpeed();
	virtual BOOL	SubtractRequested(Fragments::List& ppFragments);
protected:
	virtual bool	SendFragmentRequests();
};
