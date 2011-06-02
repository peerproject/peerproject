//
// Neighbours.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "Neighbours.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

// Create the single global Neighbours object that holds the list of neighbour computers we are connected to.
// This line creates a single global instance of a CNeighbours object, called Neighbours, when PeerProject starts running.

CNeighbours Neighbours;

//////////////////////////////////////////////////////////////////////
// CNeighbours construction

// The line above creates the one CNeighbours object named Neighbours
// Creating that object calls this empty constructor, then the CNeighboursWithConnect constructor, and so on all the way down to CNeighboursBase

CNeighbours::CNeighbours()
{
}

CNeighbours::~CNeighbours()
{
}
