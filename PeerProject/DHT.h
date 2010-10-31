//
// DHT.h
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

class CBENode;


class CDHT
{
public:
	BOOL OnPacket(SOCKADDR_IN* pHost, const CBENode* pRoot);

protected:
	CCriticalSection m_pSection;

	//BOOL Ping(SOCKADDR_IN* pHost);
	BOOL Pong(SOCKADDR_IN* pHost, LPCSTR szTransID, size_t nTransIDLength);
	//BOOL GetPeers(SOCKADDR_IN* pHost, const Hashes::BtGuid& oNodeGUID, const Hashes::BtHash& oGUID);
};

extern CDHT DHT;
