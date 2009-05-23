//
// EDNeighbour.h
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

#include "Neighbour.h"

class CEDPacket;
class CDownload;


class CEDNeighbour : public CNeighbour
{
// Construction
public:
	CEDNeighbour();
	virtual ~CEDNeighbour();

// Attributes
public:
	DWORD		m_nClientID;
	DWORD		m_nUserCount;
	DWORD		m_nUserLimit;
	DWORD		m_nFileLimit;
	DWORD		m_nTCPFlags;
	DWORD		m_nUDPFlags;
	CString		m_sServerName;
	CList< Hashes::Guid > m_pQueries;
	DWORD		m_nFilesSent;

// Operations
public:
	BOOL	SendSharedDownload(CDownload* pDownload);
	DWORD	GetID() const;
private:
	BOOL	OnPacket(CEDPacket* pPacket);
	BOOL	OnRejected(CEDPacket* pPacket);
	BOOL	OnServerMessage(CEDPacket* pPacket);
	BOOL	OnIdChange(CEDPacket* pPacket);
	BOOL	OnServerList(CEDPacket* pPacket);
	BOOL	OnServerStatus(CEDPacket* pPacket);
	BOOL	OnServerIdent(CEDPacket* pPacket);
	bool	OnCallbackRequested(CEDPacket* pPacket);
	BOOL	OnSearchResults(CEDPacket* pPacket);
	BOOL	OnFoundSources(CEDPacket* pPacket);
	void	SendSharedFiles();
	bool	IsGoodSize(QWORD nFileSize) const;			// Is file has good size for current ed2k-server?

// Overrides
public:
	virtual BOOL	ConnectTo(IN_ADDR* pAddress, WORD nPort, BOOL bAutomatic);
	virtual BOOL	Send(CPacket* pPacket, BOOL bRelease = TRUE, BOOL bBuffered = FALSE);
protected:
	virtual BOOL	SendQuery(CQuerySearch* pSearch, CPacket* pPacket, BOOL bLocal);
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnRead();
};
