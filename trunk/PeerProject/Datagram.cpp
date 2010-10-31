//
// Datagram.cpp
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "Datagram.h"
#include "Datagrams.h"
#include "Buffer.h"
#include "G2Packet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename


//////////////////////////////////////////////////////////////////////
// CDatagramIn construction

CDatagramIn::CDatagramIn()
	: m_pBuffer	( NULL )
	, m_pLocked	( NULL )
	, m_nBuffer	( 0 )
{
}

CDatagramIn::~CDatagramIn()
{
	if ( m_pLocked ) delete [] m_pLocked;
	if ( m_pBuffer ) delete [] m_pBuffer;
}

//////////////////////////////////////////////////////////////////////
// CDatagramIn prepare to handle a datagram

void CDatagramIn::Create(SOCKADDR_IN* pHost, BYTE nFlags, WORD nSequence, BYTE nCount)
{
	CopyMemory( &m_pHost, pHost, sizeof(SOCKADDR_IN) );

	m_bCompressed	= ( nFlags & SGP_DEFLATE ) ? TRUE : FALSE;
	m_nSequence		= nSequence;
	m_nCount		= nCount;
	m_nLeft			= nCount;

	m_tStarted	= GetTickCount();

	if ( m_nBuffer < m_nCount )
	{
		if ( m_pLocked ) delete [] m_pLocked;
		if ( m_pBuffer ) delete [] m_pBuffer;

		m_nBuffer	= m_nCount;
		m_pBuffer	= new CBuffer*[ m_nBuffer ];
		m_pLocked	= new BOOL[ m_nBuffer ];
	}

	ZeroMemory( m_pBuffer, sizeof(CBuffer*) * m_nBuffer );
	ZeroMemory( m_pLocked, sizeof(BOOL) * m_nBuffer );
}

//////////////////////////////////////////////////////////////////////
// CDatagramIn add a datagram part

BOOL CDatagramIn::Add(BYTE nPart, LPCVOID pData, DWORD nLength)
{
	if ( nPart < 1 || nPart > m_nCount ) return FALSE;
	if ( m_nLeft == 0 ) return FALSE;

	if ( m_pLocked[ nPart - 1 ] == FALSE )
	{
		m_pLocked[ nPart - 1 ] = TRUE;
		m_pBuffer[ nPart - 1 ]->Add( pData, nLength );

		if ( --m_nLeft == 0 ) return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CDatagramIn convert to a packet

CG2Packet* CDatagramIn::ToG2Packet()
{
	if ( m_nCount != 1 )
	{
		for ( int nPart = 1 ; nPart < m_nCount ; nPart++ )
		{
			m_pBuffer[0]->AddBuffer( m_pBuffer[ nPart ] );
		}
	}

	if ( m_bCompressed && ! m_pBuffer[0]->Inflate() ) return NULL;

	return CG2Packet::ReadBuffer( m_pBuffer[0] );
}
