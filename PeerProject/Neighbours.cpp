//
// Neighbours.cpp
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
// Creating that object calls this constructor, then the CNeighboursWithConnect constructor, and so on all the way down to CNeighboursBase
// CNeighbours doesn't add anything to the inheritance column that needs to be set up

CNeighbours::CNeighbours()
{
}

CNeighbours::~CNeighbours()
{
}

//////////////////////////////////////////////////////////////////////
// CNeighbours connect

void CNeighbours::Connect() 			// Set the ping route duration and setup the hub horizon pool
{
	CNeighboursWithConnect::Connect();	// There is no Connect method in CNeighboursWithConnect,
										// so this calls the highest one in the inheritance column:  CNeighboursWithG2::Connect()
}

//////////////////////////////////////////////////////////////////////
// CNeighbours close

void CNeighbours::Close()				// Call Close on each neighbour in list, reset member variables to 0, and clear ping & pong route caches
{
	CNeighboursWithConnect::Close();	// There isn't a Close method in CNeighbours with connect, so this calls CNeighboursWithG1::Close()
}

//////////////////////////////////////////////////////////////////////
// CNeighbours run callback

void CNeighbours::OnRun()				// Call DoRun on each neighbour in the list, and maintain the network auto connection
{
	CNeighboursWithConnect::OnRun();	// Calls CNeighboursBase::OnRun(), and uses the network object to maintain the connection
}
