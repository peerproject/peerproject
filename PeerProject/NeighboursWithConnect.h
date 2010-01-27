//
// NeighboursWithConnect.h
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

// Determine our hub or leaf role, count connections for each, and make new ones or close them to have the right number
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CNeighboursWithConnect

#pragma once

#include "NeighboursWithRouting.h"

class CConnection;

// Determine our hub or leaf role, count connections for each, and make new ones or close them to have the right number
class CNeighboursWithConnect : public CNeighboursWithRouting // Continue the inheritance column CNeighbours : CNeighboursWithConnect : Routing : ED2K : G2 : G1 : CNeighboursBase
{

public:
	// Set up and clean up anything CNeighboursWithConnect adds to the CNeighbours class
	CNeighboursWithConnect(); // Zero the tick counts in the m_tPresent array
	virtual ~CNeighboursWithConnect();

public:
	// Connect to a computer at an IP address, and accept a connection from a computer that has connected to us
	CNeighbour* ConnectTo(const IN_ADDR* pAddress, WORD nPort, PROTOCOLID nProtocol, BOOL bAutomatic = FALSE, BOOL bNoUltraPeer = FALSE);
	CNeighbour* OnAccept(CConnection* pConnection);

public:
	// Determine our role on the Gnutella2 network
	BOOL  IsG2Leaf();							// Returns true if we are acting as a Gnutella2 leaf on at least one connection
	BOOL  IsG2Hub();							// Returns true if we are acting as a Gnutella2 hub on at least one connection
	DWORD IsG2HubCapable(BOOL bDebug = FALSE); // Returns true if we have a computer and Internet connection powerful enough to become a Gnutella2 hub

	// Determine our role on the Gnutella network
	BOOL  IsG1Leaf();							// Returns true if we are acting as a Gnutella leaf on at least one connection
	BOOL  IsG1Ultrapeer();						// Returns true if we are acting as a Gnutella ultrapeer on at least one connection
	DWORD IsG1UltrapeerCapable(BOOL bDebug = FALSE); // Returns true if we have a computer and Internet connection powerful enough to become a Gnutella ultrapeer

	// Determine our needs on the given network, Gnutella or Gnutella2
	BOOL NeedMoreHubs(PROTOCOLID nProtocol);	// Returns true if we need more hub connections on the given network
	BOOL NeedMoreLeafs(PROTOCOLID nProtocol);	// Returns true if we need more leaf connections on the given network
	BOOL IsHubLoaded(PROTOCOLID nProtocol);		// Returns true if we have more than 75% of the number of hub connections settings says is our limit

protected:
	// Member variables that tell our current role on the Gnutella and Gnutella2 networks
	BOOL m_bG2Leaf;				// True if we are a leaf to at least one computer on the Gnutella2 network
	BOOL m_bG2Hub;				// True if we are a hub to at least one computer on the Gnutella2 network
	BOOL m_bG1Leaf;				// True if we are a leaf to at least one computer on the Gnutella network
	BOOL m_bG1Ultrapeer;		// True if we are an ultrapeer to at least one computer on the Gnutella network

	DWORD	m_tHubG2Promotion;	// Time we were promoted to a G2 hub

public:
	// Methods implimented by several classes in the CNeighbours inheritance column
	virtual void OnRun();		// Call DoRun on each neighbour in the list, and maintain the network auto connection

protected:
	// Make new connections and close existing ones
	void Maintain();							// Count how many connections we have, and initiate or close them to match the ideal numbers in settings
	void PeerPrune(PROTOCOLID nProtocol);		// Close hub to hub connections when we get demoted to the leaf role (do)

protected:
	// The tick count when we last connected to a hub for each network
	DWORD m_tPresent[8];		// The index is a protocol identifier, like 3 eDonkey2000, 2 Gnutella2, and 1 Gnutella
};
