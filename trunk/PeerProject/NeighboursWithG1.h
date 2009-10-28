//
// NeighboursWithG1.h
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

#include "NeighboursBase.h"

class CG1Neighbour;
class CRouteCache;
class CPongCache;

// Add the ping route and pong caches to the CNeighbours object
class CNeighboursWithG1 : public CNeighboursBase // Continue the inheritance column CNeighbours : CNeighboursWithConnect : Routing : ED2K : G2 : G1 : CNeighboursBase
{

public:
	// Called when the program runs and creates the CNeighbours object, and when it closes and deletes it
	CNeighboursWithG1();          // Setup the ping route and pong caches
	virtual ~CNeighboursWithG1(); // Delete the ping route and pong cache objects

public:
	// The ping route and pong caches
	CRouteCache* m_pPingRoute;
	CPongCache*  m_pPongCache;

public:
	// Methods implimented by several classes in the CNeighbours inheritance column
	virtual void Connect(); // Sets the ping route duration from settings
	virtual void Close();   // Call Close on each neighbour in the list, reset member variables to 0, and clear the ping route and pong caches

protected:
	// Remove a neighbour from the ping route and pong caches, network object, and the list
	virtual void Remove(CNeighbour* pNeighbour);

public:
	// Relay ping and pong packets to other neighbours
	void OnG1Ping();
	void OnG1Pong(CG1Neighbour* pFrom, IN_ADDR* pAddress, WORD nPort, BYTE nHops, DWORD nFiles, DWORD nVolume);
};
