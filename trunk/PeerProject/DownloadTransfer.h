//
// DownloadTransfer.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "Transfer.h"
#include "FileFragments.hpp"

class CDownload;
class CDownloadSource;

class CDownloadTransfer : public CTransfer
{
// Construction
public:
	CDownloadTransfer(CDownloadSource* pSource, PROTOCOLID nProtocol);
	virtual ~CDownloadTransfer();

// Attributes
public:
	CDownload*			m_pDownload;
	CDownloadTransfer*	m_pDlPrev;
	CDownloadTransfer*	m_pDlNext;
	CDownloadSource*	m_pSource;
public:
	int			m_nState;
	DWORD		m_nQueuePos;
	DWORD		m_nQueueLen;
	CString		m_sQueueName;
	DWORD		m_nBandwidth;			// Bandwidth allocated
public:
	QWORD		m_nOffset;
	QWORD		m_nLength;
	QWORD		m_nPosition;
	QWORD		m_nDownloaded;
public:
	BOOL		m_bWantBackwards;
	BOOL		m_bRecvBackwards;		// Got "Content-Encoding: backwards"

// Operations
public:
	virtual BOOL	Initiate() = 0;
	virtual void	Close(TRISTATE bKeepSource, DWORD nRetryAfter = 0);
	virtual void	Boost();
	virtual DWORD	GetAverageSpeed();
	virtual DWORD	GetMeasuredSpeed();
	virtual BOOL	SubtractRequested(Fragments::List& ppFragments) = 0;
	virtual BOOL	UnrequestRange(QWORD /*nOffset*/, QWORD /*nLength*/) { return FALSE; }
	virtual CString	GetStateText(BOOL bLong);
	virtual BOOL	OnRun();
	void	SetState(int nState);
protected:
	CTimeAverage< DWORD, 2000 > m_AverageSpeed;

	void	ChunkifyRequest(QWORD* pnOffset, QWORD* pnLength, QWORD nChunk, BOOL bVerifyLock);
};

enum
{
	dtsNull, dtsConnecting, dtsRequesting, dtsHeaders, dtsDownloading,
	dtsFlushing, dtsTiger, dtsHashset, dtsMetadata, dtsBusy, dtsEnqueue, dtsQueued,
	dtsTorrent,

	dtsCountAll = -1,
	dtsCountNotQueued = -2,
	dtsCountNotConnecting = -3,
	dtsCountTorrentAndActive = -4
};
