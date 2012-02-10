//
// QueryHashGroup.cpp
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

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "QueryHashGroup.h"
#include "QueryHashTable.h"
#include "QueryHashMaster.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename


//////////////////////////////////////////////////////////////////////
// CQueryHashGroup construction

CQueryHashGroup::CQueryHashGroup(DWORD nHash)
{
	m_nHash = nHash ? nHash : 1u << Settings.Library.QueryRouteSize;
	m_pHash = new BYTE[ m_nHash ];
	ZeroMemory( m_pHash, m_nHash );
	m_nCount = 0;
}

CQueryHashGroup::~CQueryHashGroup()
{
#ifdef _DEBUG
	BYTE* pTest = m_pHash;

	for ( DWORD nHash = m_nHash ; nHash ; nHash-- )
	{
		ASSERT( *pTest++ == 0 );
	}
#endif

	delete [] m_pHash;
}

//////////////////////////////////////////////////////////////////////
// CQueryHashGroup add a table

void CQueryHashGroup::Add(CQueryHashTable* pTable)
{
	ASSERT( pTable->m_pGroup == NULL );
	ASSERT( m_pTables.Find( pTable ) == NULL );

	pTable->m_pGroup = this;
	m_pTables.AddTail( pTable );

	Operate( pTable, TRUE );
	QueryHashMaster.Invalidate();
}

//////////////////////////////////////////////////////////////////////
// CQueryHashGroup remove a table

void CQueryHashGroup::Remove(CQueryHashTable* pTable)
{
	ASSERT( pTable->m_pGroup == this );

	POSITION pos = m_pTables.Find( pTable );
	ASSERT( pos != NULL );

	m_pTables.RemoveAt( pos );
	pTable->m_pGroup = NULL;

	Operate( pTable, FALSE );
	QueryHashMaster.Invalidate();
}

//////////////////////////////////////////////////////////////////////
// CQueryHashGroup operate

void CQueryHashGroup::Operate(CQueryHashTable* pTable, BOOL bAdd)
{
	ASSERT( m_pHash != NULL );
	ASSERT( pTable->m_nHash == m_nHash );
	//ASSERT( pTable->m_nInfinity == 1 );	// This causes problems with G1 leaves	ToDo: Check this

	BYTE* pSource = pTable->m_pHash;
	BYTE* pTarget = m_pHash;

	if ( bAdd )
	{
		for ( DWORD nHash = m_nHash >> 3 ; nHash ; nHash-- )
		{
			register BYTE nSource = *pSource++;

			if ( ( nSource & 0x01 ) == 0 ) (*pTarget++) ++; else pTarget++;
			if ( ( nSource & 0x02 ) == 0 ) (*pTarget++) ++; else pTarget++;
			if ( ( nSource & 0x04 ) == 0 ) (*pTarget++) ++; else pTarget++;
			if ( ( nSource & 0x08 ) == 0 ) (*pTarget++) ++; else pTarget++;
			if ( ( nSource & 0x10 ) == 0 ) (*pTarget++) ++; else pTarget++;
			if ( ( nSource & 0x20 ) == 0 ) (*pTarget++) ++; else pTarget++;
			if ( ( nSource & 0x40 ) == 0 ) (*pTarget++) ++; else pTarget++;
			if ( ( nSource & 0x80 ) == 0 ) (*pTarget++) ++; else pTarget++;
		}
	}
	else
	{
		for ( DWORD nHash = m_nHash >> 3 ; nHash ; nHash-- )
		{
			register BYTE nSource = *pSource++;

			if ( ( nSource & 0x01 ) == 0 ) (*pTarget++) --; else pTarget++;
			if ( ( nSource & 0x02 ) == 0 ) (*pTarget++) --; else pTarget++;
			if ( ( nSource & 0x04 ) == 0 ) (*pTarget++) --; else pTarget++;
			if ( ( nSource & 0x08 ) == 0 ) (*pTarget++) --; else pTarget++;
			if ( ( nSource & 0x10 ) == 0 ) (*pTarget++) --; else pTarget++;
			if ( ( nSource & 0x20 ) == 0 ) (*pTarget++) --; else pTarget++;
			if ( ( nSource & 0x40 ) == 0 ) (*pTarget++) --; else pTarget++;
			if ( ( nSource & 0x80 ) == 0 ) (*pTarget++) --; else pTarget++;
		}
	}
}
