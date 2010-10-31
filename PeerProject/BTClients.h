//
// BTClients.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
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
class CBTClient;


class CBTClients //: private boost::noncopyable
{
// Construction
public:
	CBTClients();
	virtual ~CBTClients();

// Attributes
protected:
	CList< CBTClient* >	m_pList;
	CMutex				m_pListSection;	// m_pList guard

// Operations
public:
	void		Clear();
	BOOL		OnAccept(CConnection* pConnection);

protected:
	void		Add(CBTClient* pClient);
	void		Remove(CBTClient* pClient);

	friend class CBTClient;

private:
	CBTClients(const CBTClients&);
	CBTClients& operator=(const CBTClients&);
};

extern CBTClients BTClients;

#define BT_PROTOCOL_HEADER			"\023BitTorrent protocol"
#define BT_PROTOCOL_HEADER_LEN		20
