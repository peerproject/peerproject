//
// DCClients.h
//
// This file is part of PeerProject (peerproject.org) © 2010
// Portions copyright Shareaza Development Team, 2010.
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

class CConnection;
class CDCClient;


class CDCClients
{
public:
	CDCClients();
	~CDCClients();

	mutable CMutex		m_pSection;	// Object guard

	// Add client
	void		Add(CDCClient* pClient);
	// Remove client
	void		Remove(CDCClient* pClient);
	// Remove all clients
	void		Clear();
	// Get client count
	int			GetCount() const;

	// Maintain not-connected (queued) clients
	void		OnRun();

	// Initiate connection to hub
	BOOL 		Connect(const IN_ADDR& pHubAddress, WORD nHubPort, const CString& sNick, BOOL& bSuccess);

	// Accept incoming TCP connection
	BOOL		OnAccept(CConnection* pConnection);

	// Merge same connections into one
	BOOL		Merge(CDCClient* pClient);

	// Calculate key
	std::string	MakeKey(const std::string& aLock) const;

	// Create DC++ compatible nick
	CString		GetDefaultNick() const;

private:
	CList< CDCClient* >	m_pList;

	std::string	KeySubst(const BYTE* aKey, size_t len, size_t n) const;
	BOOL		IsExtra(BYTE b) const;
};

extern CDCClients DCClients;
