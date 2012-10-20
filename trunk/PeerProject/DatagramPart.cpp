//
// DatagramPart.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2006.
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
#include "Settings.h"
#include "PeerProject.h"
#include "DatagramPart.h"
#include "Datagrams.h"
#include "G2Packet.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CDatagramOut construction

CDatagramOut::CDatagramOut()
	: m_pNextHash	( NULL )
	, m_pPrevHash	( NULL )
	, m_pNextTime	( NULL )
	, m_pPrevTime	( NULL )
	, m_pHost		()
	, m_nSequence	( 0 )
	, m_pBuffer		( NULL )
	, m_pToken		( NULL )
	, m_bCompressed ( FALSE )
	, m_nPacket		( 0 )
	, m_nCount		( 0 )
	, m_nAcked		( 0 )
	, m_nLocked		( 0 )
	, m_pLocked		( NULL )
	, m_tSent		( 0 )
	, m_bAck		( FALSE )
{
}

CDatagramOut::~CDatagramOut()
{
	if ( m_pLocked ) delete [] m_pLocked;
}

//////////////////////////////////////////////////////////////////////
// CDatagramOut create

void CDatagramOut::Create(const SOCKADDR_IN* pHost, const CG2Packet* pPacket, WORD nSequence, CBuffer* pBuffer, BOOL bAck)
{
	ASSERT( m_pBuffer == NULL );

	m_pHost		= *pHost;
	m_nSequence	= nSequence;
	m_pBuffer	= pBuffer;

	pPacket->ToBuffer( m_pBuffer, false );

	m_bCompressed = m_pBuffer->Deflate( TRUE );

	m_nPacket	= Settings.Gnutella2.UdpMTU;
	m_nCount	= (BYTE)( ( m_pBuffer->m_nLength + m_nPacket - 1 ) / m_nPacket );
	m_nAcked	= m_nCount;

	SGP_HEADER pHeader;

	memcpy( pHeader.szTag, SGP_TAG_2, 3 );
	pHeader.nFlags = m_bCompressed ? SGP_DEFLATE : 0;
	m_bAck = bAck;
	if ( bAck ) pHeader.nFlags |= SGP_ACKNOWLEDGE;

	pHeader.nSequence	= m_nSequence;
	pHeader.nCount		= m_nCount;

	DWORD nOffset = 0;
	DWORD nPacket = m_nPacket + sizeof(SGP_HEADER);

	for ( BYTE nPart = 0 ; nPart < m_nCount && m_pBuffer ; nPart++, nOffset += nPacket )
	{
		pHeader.nPart = nPart + 1;
		m_pBuffer->Insert( nOffset, &pHeader, sizeof(pHeader) );
	}

	if ( ! m_pLocked || m_nLocked < m_nCount )
	{
		if ( m_pLocked ) delete [] m_pLocked;

		m_nLocked = m_nCount;
		m_pLocked = new DWORD[ m_nLocked ];
	}

	ZeroMemory( m_pLocked, sizeof(DWORD) * m_nCount );

	m_tSent = GetTickCount();
}

//////////////////////////////////////////////////////////////////////
// CDatagramOut fragment packet selection

BOOL CDatagramOut::GetPacket(DWORD tNow, BYTE** ppPacket, DWORD* pnPacket, BOOL bResend)
{
	if ( ! m_pLocked )
		return FALSE;

	int nPart = 0;
	for ( ; nPart < m_nCount ; nPart++ )
	{
		if ( m_pLocked[ nPart ] < 0xFFFFFFFF )
		{
			if ( bResend )
			{
				if ( tNow >= m_pLocked[ nPart ] + Settings.Gnutella2.UdpOutResend )
					break;
			}
			else
			{
				if ( m_pLocked[ nPart ] == 0 )
					break;
			}
		}
	}

	if ( nPart >= m_nCount )
		return FALSE;

	m_pLocked[ nPart ] = bResend ? tNow : 0xFFFFFFFF;

	DWORD nPacket = m_nPacket + sizeof(SGP_HEADER);

	*ppPacket = m_pBuffer->m_pBuffer + ( nPart * nPacket );
	*pnPacket = min( nPacket, m_pBuffer->m_nLength - ( nPart * nPacket ) );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatagramOut fragment acknowledgement

BOOL CDatagramOut::Acknowledge(BYTE nPart)
{
	if ( nPart > 0 && nPart <= m_nCount && m_nAcked > 0 )
	{
		if ( m_pLocked && m_pLocked[ nPart - 1 ] < 0xFFFFFFFF )
		{
			m_pLocked[ nPart - 1 ] = 0xFFFFFFFF;

			if ( --m_nAcked == 0 )
				return TRUE;
		}
	}

	return FALSE;
}
