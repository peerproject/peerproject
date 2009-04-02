//
// UploadTransferBT.h
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

#include "UploadTransfer.h"
#include "FileFragments.hpp"

class CBTClient;
class CBTPacket;


class CUploadTransferBT : public CUploadTransfer
{
// Construction
public:
	CUploadTransferBT(CBTClient* pClient, CDownload* pDownload);
	virtual ~CUploadTransferBT();

// Attributes
public:
	CBTClient*		m_pClient;
	CDownload*		m_pDownload;
public:
	BOOL			m_bInterested;
	BOOL			m_bChoked;
	int				m_nRandomUnchoke;
	DWORD			m_tRandomUnchoke;
private:
	Fragments::Queue m_oRequested;
	Fragments::Queue m_oServed;
	
// Operations
public:
	void			SetChoke(BOOL bChoke);
	virtual void	Close(BOOL bMessage = FALSE);
	virtual DWORD	GetMeasuredSpeed();
	virtual BOOL	OnConnected();
	virtual BOOL	OnRun();
public:
	BOOL	OnInterested(CBTPacket* pPacket);
	BOOL	OnUninterested(CBTPacket* pPacket);
	BOOL	OnRequest(CBTPacket* pPacket);
	BOOL	OnCancel(CBTPacket* pPacket);
protected:
	virtual BOOL	OpenFile();
	BOOL	ServeRequests();
};
