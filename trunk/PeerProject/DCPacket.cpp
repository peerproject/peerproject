//
// DCPacket.cpp
//
// This file is part of PeerProject (peerproject.org) © 2010-2011
// Portions copyright Shareaza Development Team, 2010.
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
#include "Settings.h"
#include "DCPacket.h"
#include "DCClient.h"
#include "Network.h"
#include "QueryHit.h"
#include "Security.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CDCPacket::CDCPacketPool CDCPacket::POOL;

CDCPacket::CDCPacket() : CPacket( PROTOCOL_DC )
{
}

CDCPacket::~CDCPacket()
{
}

void CDCPacket::Reset()
{
	CPacket::Reset();
}

void CDCPacket::ToBuffer(CBuffer* pBuffer, bool /*bTCP*/) const
{
	ASSERT( m_pBuffer && m_nLength );

	pBuffer->Add( m_pBuffer, m_nLength );
}

#ifdef _DEBUG
void CDCPacket::Debug(LPCTSTR pszReason) const
{
	CString strOutput;
	strOutput.Format( L"[DC++] %s ", pszReason );
	CPacket::Debug( strOutput );
}
#endif	// Debug

BOOL CDCPacket::OnPacket(const SOCKADDR_IN* pHost)
{
	SmartDump( pHost, TRUE, FALSE );

	if ( m_nLength > 4 && memcmp( m_pBuffer, "$SR ", 4 ) == 0 )
	{
		if ( ! OnCommonHit( pHost ) )
			theApp.Message( MSG_ERROR, IDS_PROTOCOL_BAD_HIT, (LPCTSTR)CString( inet_ntoa( pHost->sin_addr ) ) );

		return TRUE;
	}

#ifdef _DEBUG
	CString str;
	str.Format( _T("Unknown DC packet from %s:%u."),
		(LPCTSTR)CString( inet_ntoa( pHost->sin_addr ) ),
		htons( pHost->sin_port ) );
	Debug( str );
#endif	// Debug

	return FALSE;	// Unknown packet
}

BOOL CDCPacket::OnCommonHit(const SOCKADDR_IN* /*pHost*/)
{
	if ( CQueryHit* pHit = CQueryHit::FromDCPacket( this ) )
		Network.OnQueryHits( pHit );

	return TRUE;
}
