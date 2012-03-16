//
// BTClients.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "Network.h"
#include "Transfers.h"
#include "BTClients.h"
#include "BTClient.h"
#include "GProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CBTClients BTClients;


//////////////////////////////////////////////////////////////////////
// CBTClients construction

CBTClients::CBTClients()
{
}

CBTClients::~CBTClients()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CBTClients clear

void CBTClients::Clear()
{
	CSingleLock oLock( &m_pListSection, TRUE );
	while ( ! m_pList.IsEmpty() )
	{
		oLock.Unlock();
		m_pList.GetHead()->Close();
		oLock.Lock();
	}
}

//////////////////////////////////////////////////////////////////////
// CBTClients accept new connections

BOOL CBTClients::OnAccept(CConnection* pConnection)
{
	if ( ! Network.IsConnected() || ( Settings.Connection.RequireForTransfers && ! Settings.BitTorrent.Enabled ) )
	{
		theApp.Message( MSG_ERROR, IDS_BT_CLIENT_DROP_CONNECTED, (LPCTSTR)pConnection->m_sAddress );
		return FALSE;
	}

	CSingleLock pLock( &Transfers.m_pSection );
	if ( pLock.Lock( 250 ) )
	{
		if ( CBTClient* pClient = new CBTClient() )
		{
			pClient->AttachTo( pConnection );
			return TRUE;
		}
	}

	theApp.Message( MSG_ERROR, _T("Rejecting BitTorrent connection from %s, network core overloaded."), (LPCTSTR)pConnection->m_sAddress );		// protocolNames[ PROTOCOL_BT ]
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CBTClients add and remove

void CBTClients::Add(CBTClient* pClient)
{
	CQuickLock oLock( m_pListSection );

	ASSERT( m_pList.Find( pClient ) == NULL );
	m_pList.AddHead( pClient );
}

void CBTClients::Remove(CBTClient* pClient)
{
	CQuickLock oLock( m_pListSection );

	POSITION pos = m_pList.Find( pClient );
	ASSERT( pos != NULL );
	m_pList.RemoveAt( pos );
}
