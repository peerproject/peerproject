//
// BTPacket.cpp
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "BTPacket.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBTPacket::CBTPacketPool CBTPacket::POOL;


//////////////////////////////////////////////////////////////////////
// CBTPacket construction

CBTPacket::CBTPacket() : CPacket( PROTOCOL_BT )
{
	m_nType	= 0;
}

CBTPacket::~CBTPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CBTPacket serialize

void CBTPacket::ToBuffer(CBuffer* pBuffer) const
{
	ASSERT( pBuffer != NULL );

	if ( m_nType == BT_PACKET_KEEPALIVE )
	{
		DWORD nZero = 0;
		pBuffer->Add( &nZero, 4 );
	}
	else
	{
		DWORD nLength = swapEndianess( m_nLength + 1 );
		pBuffer->Add( &nLength, 4 );
		pBuffer->Add( &m_nType, 1 );
		pBuffer->Add( m_pBuffer, m_nLength );
	}
}

//////////////////////////////////////////////////////////////////////
// CBTPacket unserialize

CBTPacket* CBTPacket::ReadBuffer(CBuffer* pBuffer)
{
	ASSERT( pBuffer != NULL );

	DWORD nLength = (DWORD) - 1;
	bool bKeepAlive = false;
	bool bValid = true;

	// Skip subsequent keep-alive packets
	do
	{
		if ( pBuffer->m_nLength < sizeof( DWORD ) )
			bValid = false;
		else
		{
			nLength = transformFromBE( pBuffer->ReadDWORD() );
			if ( pBuffer->m_nLength - sizeof( DWORD ) < nLength )
				bValid = false;
		}

		if ( !bKeepAlive && nLength == 0 )
			bKeepAlive = true;

		if ( bValid )
			pBuffer->Remove( sizeof( DWORD ) );		// remove size marker
	} 
	while ( bKeepAlive && bValid && nLength == 0 );

	CBTPacket* pPacket = NULL;
	if ( bKeepAlive )
	{
		pPacket = CBTPacket::New( BT_PACKET_KEEPALIVE );
	}
	else if ( bValid )
	{
		pPacket = CBTPacket::New( pBuffer->m_pBuffer[0] );
		pPacket->Write( pBuffer->m_pBuffer + 1, nLength - 1 );
		pBuffer->Remove( nLength );
	}
	else
		return NULL;

	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CBTPacket debugging

CString CBTPacket::GetType() const
{
	CString tmp;
	tmp.Format( _T("%i"), int( m_nType ) );
	return tmp;
}
