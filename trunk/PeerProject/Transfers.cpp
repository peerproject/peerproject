//
// Transfers.cpp
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
#include "Settings.h"
#include "Transfers.h"
#include "Transfer.h"
#include "TransferFile.h"
#include "Downloads.h"
#include "Uploads.h"
#include "EDClients.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CTransfers Transfers;


//////////////////////////////////////////////////////////////////////
// CTransfers construction

CTransfers::CTransfers()
	: m_nRunCookie	( 0 )
{
}

CTransfers::~CTransfers()
{
	StopThread();
}

//////////////////////////////////////////////////////////////////////
// CTransfers list tests

INT_PTR CTransfers::GetActiveCount()
{
	return Downloads.GetCount( TRUE ) + Uploads.GetTransferCount();
}

BOOL CTransfers::IsConnectedTo(const IN_ADDR* pAddress) const
{
	CSingleLock pLock( &m_pSection );
	if ( ! pLock.Lock( 250 ) )
		return FALSE;

	for ( POSITION pos = m_pList.GetHeadPosition() ; pos ; )
	{
		if ( m_pList.GetNext( pos )->m_pHost.sin_addr.S_un.S_addr == pAddress->S_un.S_addr )
			return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTransfers thread start and stop

BOOL CTransfers::StartThread()
{
	if ( theApp.m_bClosing )
		return FALSE;

	CQuickLock oLock( m_pSection );

	if ( m_pList.GetCount() == 0 && Downloads.GetCount() == 0 )
		return FALSE;

	return BeginThread( "Transfers" );
}

void CTransfers::StopThread()
{
	CloseThread();

	Downloads.m_nTransfers	= 0;
	Downloads.m_nBandwidth	= 0;
	Uploads.m_nCount		= 0;
	Uploads.m_nBandwidth	= 0;
}

//////////////////////////////////////////////////////////////////////
// CTransfers registration

void CTransfers::Add(CTransfer* pTransfer)
{
	CQuickLock oLock( m_pSection );

	ASSERT( pTransfer->IsValid() );
	WSAEventSelect( pTransfer->m_hSocket, GetWakeupEvent(), FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );

	POSITION pos = m_pList.Find( pTransfer );
	ASSERT( pos == NULL );
	if ( pos == NULL )
		m_pList.AddHead( pTransfer );

	StartThread();
}

void CTransfers::Remove(CTransfer* pTransfer)
{
	CQuickLock oLock( m_pSection );

	if ( pTransfer->IsValid() )
		WSAEventSelect( pTransfer->m_hSocket, GetWakeupEvent(), 0 );

	if ( POSITION pos = m_pList.Find( pTransfer ) )
		m_pList.RemoveAt( pos );
}

//////////////////////////////////////////////////////////////////////
// CTransfers thread run

void CTransfers::OnRun()
{
	while ( IsThreadEnabled() )
	{
		Sleep( Settings.General.MinTransfersRest );
		Doze( 50 );

		EDClients.OnRun();

		if ( ! IsThreadEnabled() ) break;

		OnRunTransfers();

		if ( ! IsThreadEnabled() ) break;

		Downloads.OnRun();

		if ( ! IsThreadEnabled() ) break;

		Uploads.OnRun();

		OnCheckExit();

		TransferFiles.CommitDeferred();
	}

	Downloads.m_nTransfers = Downloads.m_nBandwidth = 0;
	Uploads.m_nCount = Uploads.m_nBandwidth = 0;
}

void CTransfers::OnRunTransfers()
{
	// Quick check to avoid locking
	if ( m_pList.IsEmpty() )
		return;

	// Overload protection: Spend no more than 300 ms here at once
	DWORD nBegin = GetTickCount();
	CSingleLock oLock( &m_pSection, FALSE );
	if ( ! oLock.Lock( 250 ) )
		return;

	++m_nRunCookie;

	while ( ! m_pList.IsEmpty() && GetTickCount() - nBegin < 300 && m_pList.GetHead()->m_nRunCookie != m_nRunCookie )
	{
		CTransfer* pTransfer = m_pList.RemoveHead();
		m_pList.AddTail( pTransfer );
		pTransfer->m_nRunCookie = m_nRunCookie;
		pTransfer->DoRun();
	}
}

void CTransfers::OnCheckExit()
{
	CSingleLock oLock( &m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return;

	if ( m_pList.GetCount() == 0 && Downloads.GetCount() == 0 )
		Exit();

	if ( Settings.Live.AutoClose && GetActiveCount() == 0 )
	{
		Settings.Live.AutoClose = false;
		PostMainWndMessage( WM_CLOSE );
	}
}
