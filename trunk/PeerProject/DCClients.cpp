//
// DCClients.cpp
//
// This file is part of PeerProject (peerproject.org) � 2010
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
#include "Connection.h"
#include "DCClient.h"
#include "DCClients.h"
#include "DCNeighbour.h"
#include "DownloadSource.h"
#include "DownloadTransfer.h"
#include "DownloadTransferDC.h"
#include "GProfile.h"
#include "Neighbours.h"
#include "Network.h"
#include "Transfers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CDCClients DCClients;

CDCClients::CDCClients()
{
}

CDCClients::~CDCClients()
{
	Clear();
}

void CDCClients::Clear()
{
	CSingleLock oLock( &m_pSection, TRUE );

	while ( ! m_pList.IsEmpty() )
	{
		CDCClient* pClient = m_pList.GetHead();
		oLock.Unlock();
		pClient->Remove();
		oLock.Lock();
	}
}

void CDCClients::Add(CDCClient* pClient)
{
	CQuickLock oLock( m_pSection );

	if ( m_pList.Find( pClient ) == NULL )
		m_pList.AddTail( pClient );
}

void CDCClients::Remove(CDCClient* pClient)
{
	CQuickLock oLock( m_pSection );

	if ( POSITION pos = m_pList.Find( pClient ) )
		m_pList.RemoveAt( pos );
}

int CDCClients::GetCount() const
{
	CQuickLock oLock( m_pSection );

	return m_pList.GetCount();
}

void CDCClients::OnRun()
{
	CSingleLock oLock( &m_pSection, TRUE );
	for ( POSITION pos = m_pList.GetHeadPosition(); pos; )
	{
		CDCClient* pClient = m_pList.GetNext( pos );
		if ( ! pClient->IsValid() )
		{
			oLock.Unlock();

			{
				CSingleLock oTransfersLock( &Transfers.m_pSection, FALSE );
				if ( ! oTransfersLock.Lock( 250 ) )
					break;

				pClient->OnRun();
			}

			oLock.Lock();
		}
	}
}

CString CDCClients::GetDefaultNick() const
{
	CString sNick = MyProfile.GetNick();
	if ( sNick.GetLength() < 3 )
		sNick.Format( CLIENT_NAME_T _T("%04u"), GetRandomNum( 0, 9999 ) );
	else
	{
		sNick.Replace( _T(' '), _T('_') );
		sNick.Replace( _T('&'), _T('_') );
		sNick.Replace( _T('|'), _T('_') );
		sNick.Replace( _T('$'), _T('_') );
		sNick.Replace( _T('<'), _T('_') );
		sNick.Replace( _T('>'), _T('_') );
		sNick = sNick.Left( 32 );
	}

	return sNick;
}

BOOL CDCClients::Connect(const IN_ADDR& pHubAddress, WORD nHubPort, const CString& sNick, BOOL& bSuccess)
{
	bSuccess = FALSE;

	CSingleLock oLock( &Network.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		// Network core overload
		return FALSE;

	// Get existing hub
	CNeighbour* pNeighbour = Neighbours.Get( pHubAddress );
	if ( ! pNeighbour )
	{
		// Connect to new hub
		pNeighbour = new CDCNeighbour();
		if ( ! pNeighbour )
			// Out of memory
			return FALSE;

		if ( ! pNeighbour->ConnectTo( &pHubAddress, nHubPort, FALSE ) )
		{
			// Can't connect
			pNeighbour->Close();
			return FALSE;
		}
	}

	if ( pNeighbour->m_nProtocol != PROTOCOL_DC )
		// Hmmm...
		return FALSE;

	// Ask (via this hub) source for call-back connection
	bSuccess = static_cast< CDCNeighbour* >( pNeighbour )->ConnectToMe( sNick );

	return TRUE;
}

BOOL CDCClients::OnAccept(CConnection* pConnection)
{
	if ( ! Network.IsConnected() || ( Settings.Connection.RequireForTransfers && ! Settings.DC.EnableToday ) )
	{
		theApp.Message( MSG_ERROR, _T("Refusing DC++ client link from %s because network is disabled."),
			(LPCTSTR)pConnection->m_sAddress );
		return FALSE;
	}

	CSingleLock oTransfersLock( &Transfers.m_pSection );
	if ( ! oTransfersLock.Lock( 250 ) )
	{
		theApp.Message( MSG_ERROR, _T("Rejecting DC++ connection from %s, network core overloaded."),
			(LPCTSTR)pConnection->m_sAddress );
		return FALSE;
	}

	CQuickLock oLock( m_pSection );

	CDCClient* pClient = new CDCClient();
	if ( ! pClient )
		// Out of memory
		return FALSE;

	pClient->AttachTo( pConnection );

	return TRUE;
}

BOOL CDCClients::Merge(CDCClient* pClient)
{
	CQuickLock oLock( m_pSection );

	ASSERT( pClient != NULL );

	for ( POSITION pos = m_pList.GetHeadPosition(); pos; )
	{
		CDCClient* pOther = m_pList.GetNext( pos );

		if ( pOther != pClient && pOther->Equals( pClient ) )
		{
			pClient->Merge( pOther );
			return TRUE;
		}
	}

	return FALSE;
}

std::string CDCClients::MakeKey(const std::string& aLock) const
{
	if ( aLock.size() < 3 )
		return std::string();

	auto_array< BYTE > temp( new BYTE[ aLock.size() ] );
	size_t extra = 0;
	BYTE v1 = (BYTE)( (BYTE)aLock[ 0 ] ^ 5 );
	v1 = (BYTE)( ( ( v1 >> 4 ) | ( v1 << 4 ) ) & 0xff );
	temp[ 0 ] = v1;
	for ( size_t i = 1; i < aLock.size(); i++ )
	{
		v1 = (BYTE)( (BYTE)aLock[ i ] ^ (BYTE)aLock[ i - 1 ] );
		v1 = (BYTE)( ( ( v1 >> 4 ) | ( v1 << 4 ) ) & 0xff );
		temp[ i ] = v1;
		if ( IsExtra( temp[ i ] ) )
			extra++;
	}
	temp[ 0 ] = (BYTE)( temp[ 0 ] ^ temp[ aLock.size() - 1 ] );
	if ( IsExtra( temp[ 0 ] ) )
		extra++;

	return KeySubst( &temp[ 0 ], aLock.size(), extra );
}

std::string CDCClients::KeySubst(const BYTE* aKey, size_t len, size_t n) const
{
	auto_array< BYTE > temp( new BYTE[ len + n * 9 ] );
	size_t j = 0;
	for ( size_t i = 0; i < len; i++ )
	{
		if ( IsExtra( aKey[ i ] ) )
		{
			temp[ j++ ] = '/'; temp[ j++ ] = '%'; temp[ j++ ] = 'D';
			temp[ j++ ] = 'C'; temp[ j++ ] = 'N';
			switch ( aKey[ i ] )
			{
			case 0:   temp[ j++ ] = '0'; temp[ j++ ] = '0'; temp[ j++ ] = '0'; break;
			case 5:   temp[ j++ ] = '0'; temp[ j++ ] = '0'; temp[ j++ ] = '5'; break;
			case 36:  temp[ j++ ] = '0'; temp[ j++ ] = '3'; temp[ j++ ] = '6'; break;
			case 96:  temp[ j++ ] = '0'; temp[ j++ ] = '9'; temp[ j++ ] = '6'; break;
			case 124: temp[ j++ ] = '1'; temp[ j++ ] = '2'; temp[ j++ ] = '4'; break;
			case 126: temp[ j++ ] = '1'; temp[ j++ ] = '2'; temp[ j++ ] = '6'; break;
			}
			temp[ j++ ] = '%'; temp[ j++ ] = '/';
		}
		else
			temp[ j++ ] = aKey[ i ];
	}
	return std::string( (const char*)&temp[ 0 ], j );
}

BOOL CDCClients::IsExtra(BYTE b) const
{
	return ( b == 0 || b == 5 || b == 124 || b == 96 || b == 126 || b == 36 );
}
