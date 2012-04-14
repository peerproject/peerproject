//
// Neighbours.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

// Complete the CNeighbours inheritance column, calling Close on each neighbour when the program exits
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CNeighbours

#pragma once

#include "NeighboursWithConnect.h"

// Complete the CNeighbours inheritance column, calling Close on each neighbour when the program exits
// End the inheritance column CNeighbours : CNeighboursWithConnect : Routing : ED2K : G2 : G1 : CNeighboursBase

class CNeighbours : public CNeighboursWithConnect
{
public:
	CNeighbours();
	virtual ~CNeighbours();

public:
	CString GetName(const CNeighbour* pNeighbour) const;
	CString GetAgent(const CNeighbour* pNeighbour) const;
	CString GetNick(const CNeighbour* pNeighbour) const;
	CString GetNeighbourList(LPCTSTR szFormat) const;

	// Let CNeighbour and CShakeNeighbour look at private members of CNeighbours class
	friend class CNeighbour;
	friend class CShakeNeighbour;
};

// Access the single global Neighbours object that holds the list of neighbour computers we are connected to
// When PeerProject starts running, this line creates a single global instance of a CNeighbours object, called Neighbours

extern CNeighbours Neighbours;
