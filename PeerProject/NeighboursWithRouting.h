//
// NeighboursWithRouting.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

// Adds methods that send packets and Gnutella queries to all the computers we're connected to
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CNeighboursWithRouting

#pragma once

#include "NeighboursWithED2K.h"

class CPacket;
class CQuerySearch;

// Add methods to broadcast packets and Gnutella queries to all the neighbours in the list
// Continue the inheritance column CNeighbours : CNeighboursWithConnect : Routing : ED2K : G2 : G1 : CNeighboursBase
class CNeighboursWithRouting : public CNeighboursWithED2K
{
protected:
	CNeighboursWithRouting();			// The constructor and destructor don't do anything
	virtual ~CNeighboursWithRouting();

public:
	// Send a packet to all the computers we're connected to
	int Broadcast(CPacket* pPacket, CNeighbour* pExcept = NULL, BOOL bGGEP = FALSE);
	// Send a query packet to all the computers we're connected to, translating it to Gnutella and Gnutella2 for computers running that software
	int RouteQuery(const CQuerySearch* pSearch, CPacket* pPacket, CNeighbour* pFrom, BOOL bToHubs);
};
