//
// NeighboursWithED2K.h
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

#include "NeighboursWithG2.h"

class CEDNeighbour;
class CDownload;

// Add methods helpful for eDonkey2000 that use the list of connected neighbours
class CNeighboursWithED2K : public CNeighboursWithG2 // Continue the inheritance column CNeighbours : CNeighboursWithConnect : Routing : ED2K : G2 : G1 : CNeighboursBase
{
protected:
	CNeighboursWithED2K();							// Zero the memory of the sources arrays
	virtual ~CNeighboursWithED2K();

public:
	// Get an eDonkey2000 neighbour from the list that's through the handshake and has a client ID
	CEDNeighbour* GetDonkeyServer() const;

	// Do things to all the eDonkey2000 computers we're connected to
	void CloseDonkeys();                           // Disconnect from all the eDonkey2000 computers we're connected to
	void SendDonkeyDownload(CDownload* pDownload); // Tell all the connected eDonkey2000 computers about pDownload

	// Send eDonkey2000 packets
	BOOL PushDonkey(DWORD nClientID, const IN_ADDR& pServerAddress, WORD nServerPort); // Send a callback request packet
	BOOL FindDonkeySources(const Hashes::Ed2kHash& oED2K, IN_ADDR* pServerAddress, WORD nServerPort);

// Classes that inherit from this one can get to protected members, but unrelated classes can't
protected:
	// Hash arrays used by FindDonkeySources
	DWORD            m_tEDSources[256]; // 256 MD4 hashes
	Hashes::Ed2kHash m_oEDSources[256];
};
