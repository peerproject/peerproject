//
// DownloadTransferBT.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
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
#include "DownloadTransferBT.h"
#include "DownloadSource.h"
#include "Downloads.h"
#include "Download.h"
#include "BTClients.h"
#include "BTClient.h"
#include "BTPacket.h"
#include "FragmentedFile.h"
#include "Transfers.h"
#include "Network.h"
#include "Buffer.h"
#include "BENode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT construction

CDownloadTransferBT::CDownloadTransferBT(CDownloadSource* pSource, CBTClient* pClient)
	: CDownloadTransfer	( pSource, PROTOCOL_BT )
	, m_pClient			( pClient )
	, m_bChoked			( TRUE )
	, m_bInterested		( FALSE )
	, m_bAvailable		( FALSE )
	, m_nAvailable		( 0 )
	, m_tRunThrottle	( 0 )
	, m_tSourceRequest	( GetTickCount() )
{
	ASSUME_LOCK( Transfers.m_pSection );

	m_nState			= pClient ? dtsConnecting : dtsNull;
	m_sUserAgent		= _T("BitTorrent");
}

CDownloadTransferBT::~CDownloadTransferBT()
{
	ASSUME_LOCK( Transfers.m_pSection );
	ASSERT( m_pClient == NULL );

	if ( m_pClient )	// This never happens
		m_pClient->m_mInput.pLimit = m_pClient->m_mOutput.pLimit = NULL;
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT initiate

BOOL CDownloadTransferBT::Initiate()
{
	ASSUME_LOCK( Transfers.m_pSection );
	ASSERT( m_pClient == NULL );
	ASSERT( m_nState == dtsNull );

	theApp.Message( MSG_DEBUG, _T("Connecting to BitTorrent host %s..."), (LPCTSTR)CString( inet_ntoa( m_pSource->m_pAddress ) ) );

	m_pClient = new CBTClient();
	if ( ! m_pClient->Connect( this ) )
	{
		delete m_pClient;
		m_pClient = NULL;
		Close( TRI_FALSE );
		return FALSE;
	}
	SetState( dtsConnecting );
	m_tConnected	= GetTickCount();
	m_pHost			= m_pClient->m_pHost;
	m_sAddress		= m_pClient->m_sAddress;
	UpdateCountry();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT close

void CDownloadTransferBT::Close(TRISTATE bKeepSource, DWORD nRetryAfter)
{
	if ( m_pClient != NULL )
	{
		if ( Settings.General.DebugBTSources )
			theApp.Message( MSG_DEBUG, L"Closing BT download connection: %s", m_pClient->m_sAddress );

		m_pClient->m_pDownloadTransfer = NULL;
		if ( m_pClient->IsOnline() )
		{
			m_pClient->m_mInput.pLimit = &Settings.Bandwidth.Request;
			m_pClient->NotInterested();		// BT_PACKET_NOT_INTERESTED
		}
		else
		{
			m_pClient->Close();
		}
		m_pClient = NULL;
	}
	CDownloadTransfer::Close( bKeepSource, nRetryAfter );
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT bandwidth control

void CDownloadTransferBT::Boost()
{
	if ( m_pClient == NULL ) return;
	m_pClient->m_mInput.pLimit = NULL;
}

DWORD CDownloadTransferBT::GetMeasuredSpeed()
{
	// Return if there is no client
	if ( m_pClient == NULL ) return 0;

	// Calculate Input
	m_pClient->MeasureIn();

	// Return calculated speed
	return m_pClient->m_mInput.nMeasure;
}

CString CDownloadTransferBT::GetStateText(BOOL bLong)
{
	if ( m_nState == dtsTorrent )
	{
		CString str;
		if ( ! m_bInterested )
			LoadString( str, IDS_STATUS_UNINTERESTED );
		else if ( m_bChoked )
			LoadString( str, IDS_STATUS_CHOKED );
		else
			LoadString( str, IDS_STATUS_REQUESTING );
		return str;
	}
	return CDownloadTransfer::GetStateText( bLong );
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT run event

BOOL CDownloadTransferBT::OnRun()
{
	const DWORD tNow = GetTickCount();
	BOOL bShowInterest = tNow > m_tRunThrottle + 2000;

	const QWORD nBlockSize  = m_pDownload->m_pTorrent.m_nBlockSize;
	const DWORD nBlockCount = m_pDownload->m_pTorrent.m_nBlockCount;

	if ( ! m_bAvailable && nBlockSize && nBlockCount && m_nAvailable )
	{
		m_bAvailable = TRUE;

		if ( m_nAvailable != nBlockCount )
		{
			BYTE* pAvailable = m_pAvailable;
			DWORD nAvailable = m_nAvailable;

			m_nAvailable = nBlockCount;
			m_pAvailable = new BYTE[ nBlockCount ];
			ZeroMemory( m_pAvailable, nBlockCount );
			if ( pAvailable && nAvailable )
			{
				if ( nAvailable > nBlockCount )
					nAvailable = nBlockCount;
				memcpy( m_pAvailable, pAvailable, nAvailable );
			}
			delete [] pAvailable;
		}

		for ( DWORD nBlock = 0 ; nBlock < nBlockCount ; nBlock++ )
		{
			if ( m_pAvailable[ nBlock ] )
			{
				QWORD nOffset = nBlockSize * nBlock;
				QWORD nLength = min( nBlockSize, m_pDownload->m_nSize - nOffset );
				m_pSource->m_oAvailable.insert( m_pSource->m_oAvailable.end(),
					Fragments::Fragment( nOffset, nOffset + nLength ) );
			}
		}
		bShowInterest = TRUE;
	}

	if ( bShowInterest )
	{
		m_tRunThrottle = tNow;
		ShowInterest();
		if ( m_nState == dtsTorrent || m_nState == dtsRequesting || m_nState == dtsDownloading )
		{
			if ( ! SendFragmentRequests() )
				return FALSE;
		}
	}

	if ( tNow >= m_tSourceRequest + Settings.BitTorrent.SourceExchangePeriod * 60000 )	// 1 minute
	{
		m_pClient->SendSourceRequest();		// BT_PACKET_SOURCE_REQUEST
		m_tSourceRequest = tNow;
	}

	return CDownloadTransfer::OnRun();
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT connection established event

BOOL CDownloadTransferBT::OnConnected()
{
	ASSUME_LOCK( Transfers.m_pSection );
	ASSERT( m_pClient != NULL );
	ASSERT( m_pSource != NULL );

	SetState( dtsTorrent );
	m_pHost		= m_pClient->m_pHost;
	m_sAddress	= m_pClient->m_sAddress;
	UpdateCountry();

	// not deleting source for source exchange.
	if ( m_pDownload->IsCompleted() )
	{
		// This source is only here to push start torrent uploads. (We don't want to download)
		m_bInterested = FALSE;
		theApp.Message( MSG_INFO, _T("Initiated push start for upload to %s"), (LPCTSTR)m_sAddress );
	}
	else
	{
		// Regular download
		m_pClient->m_mInput.pLimit = &m_nBandwidth;
		theApp.Message( MSG_INFO, IDS_DOWNLOAD_CONNECTED, (LPCTSTR)m_sAddress );
		if ( ! m_pDownload->PrepareFile() )
		{
			Close( TRI_TRUE );
			return FALSE;
		}
		m_pClient->m_mInput.pLimit = &m_nBandwidth;
	}

	m_pSource->SetLastSeen();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT bitfields

BOOL CDownloadTransferBT::OnBitfield(CBTPacket* pPacket)
{
	ASSUME_LOCK( Transfers.m_pSection );

	QWORD nBlockSize	= m_pDownload->m_pTorrent.m_nBlockSize;
	DWORD nBlockCount	= m_pDownload->m_pTorrent.m_nBlockCount;

	m_pSource->m_oAvailable.clear();

	delete [] m_pAvailable;
	m_pAvailable = NULL;

	if ( nBlockCount == 0 )
		nBlockCount = pPacket->GetRemaining() * 8;

	if ( nBlockCount == 0 )
		return TRUE;

	m_nAvailable = nBlockCount;
	m_bAvailable = ( nBlockSize > 0 );
	m_pAvailable = new BYTE[ m_nAvailable ];
	ZeroMemory( m_pAvailable, nBlockCount );

	for ( DWORD nBlock = 0 ; nBlock < nBlockCount && pPacket->GetRemaining() ; )
	{
		BYTE nByte = pPacket->ReadByte();

		for ( int nBit = 7 ; nBit >= 0 && nBlock < nBlockCount ; nBit--, nBlock++ )
		{
			if ( nByte & ( 1 << nBit ) )
			{
				if ( m_bAvailable )
				{
					QWORD nOffset = nBlockSize * nBlock;
					QWORD nLength = min( nBlockSize, m_pDownload->m_nSize - nOffset );
					m_pSource->m_oAvailable.insert( m_pSource->m_oAvailable.end(),
						Fragments::Fragment( nOffset, nOffset + nLength ) );
				}
				m_pAvailable[ nBlock ] = TRUE;
			}
		}
	}

	ShowInterest();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT have block updates

void CDownloadTransferBT::SendFinishedBlock(DWORD nBlock)
{
	if ( ! m_pClient || ! m_pClient->IsOnline() )
		return;
	m_pClient->Have( nBlock );		// BT_PACKET_HAVE
}

BOOL CDownloadTransferBT::OnHave(CBTPacket* pPacket)
{
	ASSUME_LOCK( Transfers.m_pSection );

	if ( pPacket->GetRemaining() != sizeof( int ) ) return TRUE;
	QWORD nBlockSize	= m_pDownload->m_pTorrent.m_nBlockSize;
	DWORD nBlockCount	= m_pDownload->m_pTorrent.m_nBlockCount;
	DWORD nBlock		= pPacket->ReadLongBE();

	if ( nBlockCount == 0 )
		nBlockCount = nBlock + 1;

	if ( nBlock >= nBlockCount )
		return TRUE;

	if ( nBlockSize > 0 )
	{
		QWORD nOffset = nBlockSize * nBlock;
		QWORD nLength = min( nBlockSize, m_pDownload->m_nSize - nOffset );
		m_pSource->m_oAvailable.insert( Fragments::Fragment( nOffset, nOffset + nLength ) );
	}

	if ( m_nAvailable < nBlockCount )
	{
		BYTE* pAvailable = m_pAvailable;
		DWORD nAvailable = m_nAvailable;

		m_nAvailable = nBlockCount;
		m_pAvailable = new BYTE[ nBlockCount ];
		ZeroMemory( m_pAvailable, nBlockCount );
		if ( pAvailable && nAvailable )
			memcpy( m_pAvailable, pAvailable, nAvailable );

		delete [] pAvailable;
	}

	m_pAvailable[ nBlock ] = TRUE;
	ShowInterest();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT interest control

void CDownloadTransferBT::ShowInterest()
{
	BOOL bInterested = FALSE;

	// ToDo: Use an algorithm similar to CDownloadWithTiger::FindNext..,
	// rather than relying on that algorithm to complete verifications here.

	// We can only be interested if we know what they have
	if ( m_pAvailable )
	{
		const QWORD nBlockSize = m_pDownload->m_pTorrent.m_nBlockSize;
		if ( ! nBlockSize )
		{
			theApp.Message( MSG_DEBUG, _T("Torrent block size missing.") );
			return;
		}

		Fragments::List oList( m_pDownload->GetWantedFragmentList() );
		Fragments::List::const_iterator pItr = oList.begin();
		const Fragments::List::const_iterator pEnd = oList.end();
		for ( ; ! bInterested && pItr != pEnd ; ++pItr )
		{
			QWORD nBlock = pItr->begin() / nBlockSize;
			QWORD nEnd = ( pItr->end() - 1 ) / nBlockSize;
			for ( ; nBlock <= nEnd ; ++nBlock )
			{
				if ( m_pAvailable[ nBlock ] )
				{
					bInterested = TRUE;
					break;
				}
			}
		}
	}

	if ( m_bInterested == bInterested )
		return;

	m_bInterested = bInterested;

	if ( bInterested )
	{
		m_pClient->Interested();		// BT_PACKET_INTERESTED
	}
	else
	{
		m_pClient->NotInterested();		// BT_PACKET_NOT_INTERESTED
		m_oRequested.clear();
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT choking

BOOL CDownloadTransferBT::OnChoked(CBTPacket* /*pPacket*/)
{
	if ( m_bChoked )
		return TRUE;

	m_bChoked = TRUE;
	SetState( dtsTorrent );

	theApp.Message( MSG_DEBUG, _T("Download from %s was choked."), (LPCTSTR)m_sAddress );

	//for ( Fragments::Queue::const_iterator pFragment = m_oRequested.begin() ;
	//	pFragment != m_oRequested.end() ; ++pFragment )
	//{
	//	m_pClient->Cancel(				// BT_PACKET_CANCEL
	//		(DWORD)( pFragment->begin() / m_pDownload->m_pTorrent.m_nBlockSize ),
	//		(DWORD)( pFragment->begin() % m_pDownload->m_pTorrent.m_nBlockSize ),
	//		(DWORD)( pFragment->size() ) );
	//}

	m_oRequested.clear();

	return TRUE;
}

BOOL CDownloadTransferBT::OnUnchoked(CBTPacket* /*pPacket*/)
{
	m_bChoked = FALSE;
	SetState( dtsTorrent );
	m_oRequested.clear();

	theApp.Message( MSG_DEBUG, _T("Download from %s was Unchoked."), (LPCTSTR)m_sAddress );

	return SendFragmentRequests();
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT fragment request manager

bool CDownloadTransferBT::SendFragmentRequests()
{
	ASSUME_LOCK( Transfers.m_pSection );

	ASSERT( m_nState == dtsTorrent || m_nState == dtsRequesting || m_nState == dtsDownloading );
	if ( m_bChoked || ! m_bInterested )
	{
		if ( m_oRequested.empty() )
			SetState( dtsTorrent );

		return true;
	}

	if ( m_oRequested.size() >= (int)Settings.BitTorrent.RequestPipe )
	{
		if ( m_nState != dtsDownloading )
		{
			theApp.Message( MSG_DEBUG, L"Too many requests per host, staying in the requested state" );
			SetState( dtsRequesting );
		}

		return true;
	}

	QWORD nBlockSize = m_pDownload->m_pTorrent.m_nBlockSize;
	ASSERT( nBlockSize != 0 );
	if ( ! nBlockSize )
		return true;

	Fragments::List oPossible( m_pDownload->GetWantedFragmentList() );

	if ( ! m_pDownload->m_bTorrentEndgame )
	{
		for ( CDownloadTransfer* pTransfer = m_pDownload->GetFirstTransfer() ; pTransfer && ! oPossible.empty() ; pTransfer = pTransfer->m_pDlNext )
		{
			pTransfer->SubtractRequested( oPossible );
		}
	}

	while ( m_oRequested.size() < (int)Settings.BitTorrent.RequestPipe )
	{
		QWORD nOffset, nLength;
		if ( ! SelectFragment( oPossible, nOffset, nLength, m_pDownload->m_bTorrentEndgame ) )
			break;

		ChunkifyRequest( &nOffset, &nLength, Settings.BitTorrent.RequestSize, FALSE );

		Fragments::Fragment Selected( nOffset, nOffset + nLength );
		oPossible.erase( Selected );

		m_oRequested.push_back( Selected );

		if ( m_nDownloaded == 0 || ( nOffset % nBlockSize ) == 0 )
			theApp.Message( MSG_INFO, IDS_DOWNLOAD_FRAGMENT_REQUEST, nOffset, nOffset + nLength - 1, (LPCTSTR)m_pDownload->GetDisplayName(), (LPCTSTR)m_sAddress );

		m_pClient->Request(						// BT_PACKET_REQUEST
			(DWORD)( nOffset / nBlockSize ),
			(DWORD)( nOffset % nBlockSize ),
			(DWORD)( nLength ) );
	}

	// If there are no more possible chunks to request, and endgame is available but not active
	if ( oPossible.empty() && Settings.BitTorrent.Endgame && ! m_pDownload->m_bTorrentEndgame )
	{
		// And the torrent is at least 95% complete
		if ( m_pDownload->GetProgress() > 95.0f )
		{
			// Then activate endgame
			m_pDownload->m_bTorrentEndgame = true;
			theApp.Message( MSG_DEBUG, _T("Torrent EndGame mode activated for %s"), m_pDownload->m_sName );
		}
	}

	if ( ! m_oRequested.empty() && m_nState != dtsDownloading )
	{
		theApp.Message( MSG_DEBUG, L"Request for piece sent, switching to the requested state" );
		SetState( dtsRequesting );
	}

	if ( m_oRequested.empty() )
		SetState( dtsTorrent );

	return true;
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT fragment selection	(Obsolete: remove)

//BOOL CDownloadTransferBT::SelectFragment(const Fragments::List& oPossible, QWORD& nOffset, QWORD& nLength)
//{
//	Fragments::Fragment oSelection(
//		selectBlock( oPossible, m_pDownload->m_pTorrent.m_nBlockSize, m_pAvailable ) );
//
//	if ( oSelection.size() == 0 ) return FALSE;
//	nOffset = oSelection.begin();
//	nLength = oSelection.size();
//	return TRUE;
//}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT multi-source fragment handling

BOOL CDownloadTransferBT::SubtractRequested(Fragments::List& ppFragments)
{
	if ( m_oRequested.empty() || m_bChoked )
		return FALSE;
	ppFragments.erase( m_oRequested.begin(), m_oRequested.end() );
	return TRUE;
}

bool CDownloadTransferBT::UnrequestRange(QWORD nOffset, QWORD nLength)
{
	if ( m_oRequested.empty() )
		return false;

	ASSERT( m_pDownload->m_pTorrent.m_nBlockSize != 0 );
	if ( m_pDownload->m_pTorrent.m_nBlockSize == 0 )
		return false;

	Fragments::Queue oUnrequests = extract_range( m_oRequested,
		Fragments::Fragment( nOffset, nOffset + nLength ) );

	for ( Fragments::Queue::const_iterator pFragment = oUnrequests.begin() ;
		pFragment != oUnrequests.end() ; ++pFragment )
	{
		m_pClient->Cancel(							// BT_PACKET_CANCEL
			(DWORD)( pFragment->begin() / m_pDownload->m_pTorrent.m_nBlockSize ),
			(DWORD)( pFragment->begin() % m_pDownload->m_pTorrent.m_nBlockSize ),
			(DWORD)( pFragment->size() ) );
	}

	return ! oUnrequests.empty();
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT piece reception

BOOL CDownloadTransferBT::OnPiece(CBTPacket* pPacket)
{
	ASSUME_LOCK( Transfers.m_pSection );

	ASSERT( m_pClient != NULL );
	if ( pPacket->GetRemaining() < 8 ) return TRUE;
	if ( m_nState != dtsRequesting && m_nState != dtsDownloading ) return TRUE;

	SetState( dtsDownloading );
	DWORD nBlock	= pPacket->ReadLongBE();
	QWORD nOffset	= pPacket->ReadLongBE();
	QWORD nLength	= pPacket->GetRemaining();
	nOffset += (QWORD)nBlock * m_pDownload->m_pTorrent.m_nBlockSize;
	m_nDownloaded += nLength;
	m_pDownload->m_nTorrentDownloaded += nLength;
	m_pDownload->m_pTorrent.m_nTotalDownload += nLength;
	m_pSource->AddFragment( nOffset, nLength );
	m_pSource->SetValid();
	m_oRequested.erase( Fragments::Fragment( nOffset, nOffset + nLength ) );

	BOOL bSuccess = m_pDownload->SubmitData( nOffset, pPacket->m_pBuffer + pPacket->m_nPosition, nLength );
	if ( ! bSuccess )
		TRACE( _T("[BT] Failed to submit data %I64u-%I64u to \"%s\".\n"), nOffset, nOffset + nLength, m_pDownload->m_sPath );

	// ToDo: SendRequests and ShowInterest could be combined...
	// SendRequests is probably going to tell us if we are interested or not
	ShowInterest();
	return SendFragmentRequests();
}

//////////////////////////////////////////////////////////////////////
// CDownloadTransferBT source exchange

BOOL CDownloadTransferBT::OnSourceResponse(CBTPacket* pPacket)
{
	const CBENode* pRoot = pPacket->m_pNode.get();

	const CBENode* pPeers = pRoot->GetNode( BT_DICT_PEERS );				// "peers"
	if ( ! pPeers->IsType( CBENode::beList ) )
	{
		delete pRoot;
		return TRUE;
	}

	int nCount = 0;

	for ( int nPeer = 0 ; nPeer < pPeers->GetCount() ; nPeer++ )
	{
		const CBENode* pPeer = pPeers->GetNode( nPeer );
		if ( ! pPeer->IsType( CBENode::beDict ) ) continue;

		const CBENode* pURL = pPeer->GetNode( BT_DICT_PEER_URL );			// "url"

		if ( pURL->IsType( CBENode::beString ) )
		{
			nCount += m_pDownload->AddSourceURL( pURL->GetString(), TRUE );
		}
		else
		{
			const CBENode* pIP = pPeer->GetNode( BT_DICT_PEER_IP );			// "ip"
			if ( ! pIP->IsType( CBENode::beString ) ) continue;

			const CBENode* pPort = pPeer->GetNode( BT_DICT_PEER_PORT );		// "port"
			if ( ! pPort->IsType( CBENode::beInt ) ) continue;

			SOCKADDR_IN saPeer = {};
			if ( ! Network.Resolve( pIP->GetString(), (int)pPort->GetInt(), &saPeer ) ) continue;

			theApp.Message( MSG_DEBUG, _T("CDownloadTransferBT::OnSourceResponse(): %s: %s:%i"),
				(LPCTSTR)m_sAddress, (LPCTSTR)CString( inet_ntoa( saPeer.sin_addr ) ), htons( saPeer.sin_port ) );

			const CBENode* pID = pPeer->GetNode( BT_DICT_PEER_ID );			// "peer id"
			if ( ! pID->IsType( CBENode::beString ) || pID->m_nValue != Hashes::BtGuid::byteCount )
			{
				nCount += m_pDownload->AddSourceBT( Hashes::BtGuid(), &saPeer.sin_addr, htons( saPeer.sin_port ) );
			}
			else
			{
				Hashes::BtGuid tmp( *static_cast< const Hashes::BtGuid::RawStorage* >( pID->m_pValue ) );
				nCount += m_pDownload->AddSourceBT( tmp, &saPeer.sin_addr, htons( saPeer.sin_port ) );
			}
		}

		if ( nCount > Settings.Downloads.SourcesWanted && m_pDownload->GetEffectiveSourceCount() > Settings.Downloads.SourcesWanted )
			break;
	}

	theApp.Message( MSG_INFO, IDS_BT_CLIENT_EXCHANGE, nCount, (LPCTSTR)m_sAddress );

	return TRUE;
}
