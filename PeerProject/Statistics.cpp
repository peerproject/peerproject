//
// Statistics.cpp
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Statistics.h"
#include "Network.h"
#include "Neighbours.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CStatistics Statistics;


//////////////////////////////////////////////////////////////////////
// CStatistics construction

CStatistics::CStatistics()
{
	ZeroMemory( &Today, sizeof(Today) );
	ZeroMemory( &Ever, sizeof(Ever) );
	ZeroMemory( &Current, sizeof(Current) );

	m_tSeconds = 0;
}

CStatistics::~CStatistics()
{
}

//////////////////////////////////////////////////////////////////////
// CStatistics update

void CStatistics::Update()
{
	const DWORD tNow = GetTickCount();

	if ( tNow >= m_tSeconds + 1000 )
	{
		if ( Network.IsWellConnected() )
		{
			Current.Timer.Connected ++;
			if ( Neighbours.IsG2Hub() ) Current.Timer.Hub ++;
			if ( Neighbours.IsG1Ultrapeer() ) Current.Timer.Ultrapeer ++;
		}

		m_tSeconds = tNow;
	}

	CopyMemory( &Last, &Current, sizeof(Current) );
	Add( &Today, &Current, sizeof(Current) );
	Add( &Ever, &Current, sizeof(Current) );
	ZeroMemory( &Current, sizeof(Current) );
}

//////////////////////////////////////////////////////////////////////
// CStatistics adder

void CStatistics::Add(LPVOID pTarget, LPCVOID pSource, int nCount)
{
	QWORD* pqwTarget = (QWORD*)pTarget;
	QWORD* pqwSource = (QWORD*)pSource;

	for ( nCount /= sizeof(QWORD) ; nCount ; nCount-- )
	{
		*pqwTarget += *pqwSource;

		pqwTarget++;
		pqwSource++;
	}
}
