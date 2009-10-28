//
// DownloadTransferHTTP.h
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

#include "DownloadTransfer.h"


class CDownloadTransferHTTP : public CDownloadTransfer
{
// Construction
public:
	CDownloadTransferHTTP(CDownloadSource* pSource);
	virtual ~CDownloadTransferHTTP();

// Attributes
protected:
	DWORD			m_nRequests;
	DWORD			m_tRequest;
	DWORD			m_tContent;
	BOOL			m_bBadResponse;
	BOOL			m_bBusyFault;
	BOOL			m_bRangeFault;
	BOOL			m_bKeepAlive;
	BOOL			m_bHashMatch;
	CString			m_sTigerTree;
	BOOL			m_bTigerFetch;
	BOOL			m_bTigerIgnore;
	CString			m_sMetadata;
	BOOL			m_bMetaFetch;
	BOOL			m_bMetaIgnore;
	BOOL			m_bGotRange;
	BOOL			m_bGotRanges;
	BOOL			m_bQueueFlag;
	QWORD			m_nContentLength;
	CString			m_sContentType;
	DWORD			m_nRetryDelay;      // Delay for queuing
	DWORD			m_nRetryAfter;      // Got "Retry-After: x" seconds
	BOOL			m_bRedirect;
	CString			m_sRedirectionURL;
	BOOL			m_bGzip;			// Got "Content-Encoding: gzip" or x-gzip
	BOOL			m_bCompress;		// Got "Content-Encoding: compress" or x-compress
	BOOL			m_bDeflate;			// Got "Content-Encoding: deflate"
	BOOL			m_bChunked;			// Got "Transfer-Encoding: chunked"
	enum ChunkState
	{
		Header,							// Reading chunk header "Length<CR><LF>"
		Body,							// Reading chunk body
		BodyEnd,						// Waiting for chunk body ending "<CR><LF>"
		Footer							// Bypassing data after last chunk
	};
	ChunkState		m_ChunkState;
	QWORD			m_nChunkLength;

// Operations
public:
	virtual BOOL	Initiate();
	BOOL			AcceptPush(CConnection* pConnection);
	virtual void	Close( TRISTATE bKeepSource, DWORD nRetryAfter = 0 );
	virtual void	Boost();
	virtual DWORD	GetAverageSpeed();
	virtual BOOL	SubtractRequested(Fragments::List& ppFragments);
	virtual BOOL	OnRun();
protected:
	BOOL			StartNextFragment();
	BOOL			SendRequest();
	BOOL			ReadResponseLine();
	BOOL			ReadContent();
	BOOL			ReadTiger();
	BOOL			ReadMetadata();
	BOOL			ReadFlush();
protected:
	virtual BOOL	OnConnected();
	virtual BOOL	OnRead();
	virtual void	OnDropped();
	virtual BOOL	OnHeaderLine(CString& strHeader, CString& strValue);
	virtual BOOL	OnHeadersComplete();
};
