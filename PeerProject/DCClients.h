//
// DCClients.h
//
// This file is part of PeerProject (peerproject.org) © 2010-2012
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
class CDCNeighbour;


class CDCClients
{
public:
	CDCClients();
	~CDCClients();

public:
	void				Add(CDCClient* pClient);					// Add client
	void				Remove(CDCClient* pClient); 				// Remove client
	void				Clear();									// Remove all clients
	int					GetCount() const;							// Get client count

	void				OnRun();									// Maintain not-connected (queued) clients
	CDCClient*			GetClient(const CString& sNick) const;		// Find client by GUID
	CDCNeighbour*		GetHub(const CString& sNick) const; 		// Find hub by user nick
	BOOL 				Connect(const IN_ADDR* pHubAddress, WORD nHubPort, const CString& sRemoteNick, BOOL& bSuccess);		// Initiate connection to hub
	BOOL				ConnectTo(const IN_ADDR* pAddress, WORD nPort, CDCNeighbour* pHub, const CString& sRemoteNick);		// Initiate connection to client
	BOOL				OnAccept(CConnection* pConnection); 		// Accept incoming TCP connection
	BOOL				Merge(CDCClient* pClient);					// Merge same connections into one

	static std::string	MakeKey(const std::string& aLock);			// Calculate key
	static CString		CreateNick(LPCTSTR szNick = NULL);			// Create DC++ compatible nick
	static void			CreateGUID(const CString& sNick, Hashes::Guid& oGUID);	// Create GUID from nick

private:
	CList< CDCClient* >	m_pList;
	mutable CMutexEx	m_pSection;		// Object guard

	static std::string	KeySubst(const BYTE* aKey, size_t len, size_t n);
	static BOOL			IsExtra(BYTE b);
};

extern CDCClients DCClients;

// PeerProject Client-Client & Client-Hub capabilities
#define DC_CLIENT_SUPPORTS	"$Supports MiniSlots XmlBZList ADCGet TTHL TTHF ZLIG|"
#define DC_HUB_SUPPORTS 	"$Supports NoHello NoGetINFO UserIP2 TTHSearch ZPipe0|"
