//
// Statistics.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#include "StdAfx.h"
#include "PeerProject.h"
#include "Statistics.h"
#include "Network.h"
#include "Neighbours.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

CStatistics Statistics;


//////////////////////////////////////////////////////////////////////
// CStatistics construction

CStatistics::CStatistics()
{
	ZeroMemory( &Ever, sizeof( Ever ) );
	ZeroMemory( &Today, sizeof( Today ) );
	ZeroMemory( &Current, sizeof( Current ) );

	m_tSeconds = GetMicroCount() / 1000;
}

CStatistics::~CStatistics()
{
}

//////////////////////////////////////////////////////////////////////
// CStatistics update

void CStatistics::Update()
{
	const QWORD tNow = GetMicroCount() / 1000;	// ms

	if ( tNow >= m_tSeconds + 1000 )
	{
		if ( Network.IsWellConnected() )
		{
			const QWORD nElapsed = ( tNow - m_tSeconds ) / 1000;	// s
			Current.Timer.Connected += nElapsed;
			if ( Neighbours.IsG2Hub() ) Current.Timer.Hub += nElapsed;
			if ( Neighbours.IsG1Ultrapeer() ) Current.Timer.Ultrapeer += nElapsed;
		}

		m_tSeconds = tNow;
	}

	CopyMemory( &Last, &Current, sizeof( Current ) );
	Add( &Today, &Current, sizeof( Current ) );
	Add( &Ever, &Current, sizeof( Current ) );
	ZeroMemory( &Current, sizeof( Current ) );
}

//////////////////////////////////////////////////////////////////////
// CStatistics adder

void CStatistics::Add(LPVOID pTarget, LPCVOID pSource, int nCount)
{
	QWORD* pqwTarget = (QWORD*)pTarget;
	QWORD* pqwSource = (QWORD*)pSource;

	for ( nCount /= sizeof( QWORD ) ; nCount ; nCount-- )
	{
		*pqwTarget += *pqwSource;

		pqwTarget++;
		pqwSource++;
	}
}
