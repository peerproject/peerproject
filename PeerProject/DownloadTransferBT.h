//
// DownloadTransferBT.h
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

#include "DownloadTransfer.h"

class CBTClient;
class CBTPacket;


class CDownloadTransferBT : public CDownloadTransfer
{
// Construction
public:
	CDownloadTransferBT(CDownloadSource* pSource, CBTClient* pClient);
	virtual ~CDownloadTransferBT();

// Attributes
public:
	CBTClient*		m_pClient;
	BOOL			m_bChoked;
	BOOL			m_bInterested;
public:
	BYTE*			m_pAvailable;
	Fragments::Queue m_oRequested;
	DWORD			m_tRunThrottle;
	DWORD			m_tSourceRequest;

// Operations
public:
	virtual BOOL	Initiate();
	virtual void	Close(TRISTATE bKeepSource, DWORD nRetryAfter = 0);
	virtual void	Boost();
	virtual DWORD	GetMeasuredSpeed();
	virtual CString	GetStateText(BOOL bLong);
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual BOOL	SubtractRequested(Fragments::List& ppFragments);
	virtual BOOL	UnrequestRange(QWORD nOffset, QWORD nLength);
public:
	BOOL	OnBitfield(CBTPacket* pPacket);
	BOOL	OnHave(CBTPacket* pPacket);
	BOOL	OnChoked(CBTPacket* pPacket);
	BOOL	OnUnchoked(CBTPacket* pPacket);
	BOOL	OnPiece(CBTPacket* pPacket);
	BOOL	OnSourceResponse(CBTPacket* pPacket);
	void	SendFinishedBlock(DWORD nBlock);
protected:
	void	Send(CBTPacket* pPacket, BOOL bRelease = TRUE);
	void	ShowInterest();
	BOOL	SendRequests();
	BOOL	SelectFragment(const Fragments::List& oPossible, QWORD& nOffset, QWORD& nLength);
};
