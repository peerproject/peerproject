//
// BTClients.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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
#include "Settings.h"
#include "Transfers.h"
#include "BTClients.h"
#include "BTClient.h"
#include "GProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
	ASSERT( pConnection != NULL );

	CSingleLock pLock( &Transfers.m_pSection );
	if ( ! pLock.Lock( 250 ) )
		return FALSE;

	CBTClient* pClient = new CBTClient();
	pClient->AttachTo( pConnection );

	return TRUE;
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
