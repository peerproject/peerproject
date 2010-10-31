//
// DownloadTransferBT.h
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
	BOOL			m_bAvailable;
	DWORD			m_nAvailable;
	DWORD			m_tRunThrottle;
	DWORD			m_tSourceRequest;
	Fragments::Queue m_oRequested;

// Operations
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
// ToDo: Remove these-
//	BOOL	SendRequests();
//	BOOL	SelectFragment(const Fragments::List& oPossible, QWORD& nOffset, QWORD& nLength);

// Overides
public:
	virtual BOOL	Initiate();
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	Boost();
	virtual void	Close(TRISTATE bKeepSource, DWORD nRetryAfter = 0);
	virtual DWORD	GetMeasuredSpeed();
	virtual CString	GetStateText(BOOL bLong);
	virtual BOOL	SubtractRequested(Fragments::List& ppFragments);
	virtual bool	UnrequestRange(QWORD nOffset, QWORD nLength);
protected:
	virtual bool	SendFragmentRequests();
};
