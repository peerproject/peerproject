//
// EDClients.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#include "EDClient.h"

class CConnection;
class CEDPacket;


class CEDClients
{
// Construction
public:
	CEDClients();
	virtual ~CEDClients();

private:
	CEDClients(const CEDClients&);				// Declaration only
	CEDClients& operator=(const CEDClients&);	// Declaration only

// Attributes
public:
	mutable CMutex	m_pSection;	// EDClients Guard
private:
	CEDClient*		m_pFirst;
	CEDClient*		m_pLast;
	int				m_nCount;
	DWORD			m_tLastRun;
	DWORD			m_tLastMaxClients;
	DWORD			m_tLastServerStats;
	in_addr			m_pLastServer;
	DWORD			m_nLastServerKey;
	BOOL			m_bAllServersDone;

// Operations
public:
	void			Add(CEDClient* pClient);
	void			Remove(CEDClient* pClient);
	void			Clear();
	bool			PushTo(DWORD nClientID, WORD nClientPort);
	CEDClient*		GetByIP(IN_ADDR* pAddress) const;
	CEDClient*		Connect(DWORD nClientID, WORD nClientPort, IN_ADDR* pServerAddress, WORD nServerPort, const Hashes::Guid& oGUID);
					// Connect to new or known eD2K-client
					// (nClientPort and nServerPort must be in host byte order)
	BOOL			Merge(CEDClient* pClient);
	void			OnRun();
	BOOL			OnAccept(CConnection* pConnection);
	BOOL			OnPacket(SOCKADDR_IN* pHost, CEDPacket* pPacket);
	bool			IsFull(const CEDClient* pCheckThis = NULL);
	BOOL			IsOverloaded() const;
	BOOL			IsMyDownload(const CDownloadTransferED2K* pDownload) const;

protected:
	CEDClient*		GetByID(DWORD nClientID, IN_ADDR* pServer, const Hashes::Guid& oGUID) const;
	CEDClient*		GetByGUID(const Hashes::Guid& oGUID) const;
	void			OnServerStatus(SOCKADDR_IN* pHost, CEDPacket* pPacket);
	void			RequestServerStatus(IN_ADDR* pHost, WORD nPort);
	void			RunGlobalStatsRequests(DWORD tNow);
};

extern CEDClients EDClients;
