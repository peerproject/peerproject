//
// BTClient.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "BTClient.h"
#include "BTClients.h"
#include "BTPacket.h"
#include "BENode.h"
#include "Buffer.h"

#include "Download.h"
#include "Downloads.h"
#include "DownloadSource.h"
#include "DownloadTransferBT.h"
#include "UploadTransferBT.h"
#include "Uploads.h"
#include "PeerProjectURL.h"
#include "Network.h"
#include "GProfile.h"
#include "Datagrams.h"
#include "Transfers.h"
#include "Statistics.h"
#include "VendorCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CBTClient construction

CBTClient::CBTClient()
	: CTransfer				( PROTOCOL_BT )
	, m_bExtended			( FALSE )
	, m_pUploadTransfer		( NULL )
	, m_pDownloadTransfer	( NULL )
	, m_pDownload			( NULL )
	, m_bShake				( FALSE )
	, m_bOnline				( FALSE )
	, m_bClosing			( FALSE )
	, m_bSeeder				( FALSE )
	, m_bDHTPort			( FALSE )
	, m_bPrefersEncryption	( FALSE )
	, m_tLastKeepAlive		( GetTickCount() )
	, m_tLastUtPex			( 0 )
	, m_nUtMetadataID		( 0 )	// 0 or BT_EXTENSION_UT_METADATA
	, m_nUtMetadataSize		( 0 )
	, m_nUtPexID			( 0 )	// 0 or BT_EXTENSION_UT_PEX
	, m_nLtTexID			( 0 )	// 0 or BT_EXTENSION_LT_TEX
	, m_nSrcExchangeID		( 0 )	// 0 or BT_HANDSHAKE_SOURCE
{
	m_sUserAgent = _T("BitTorrent");
	m_mInput.pLimit = m_mOutput.pLimit = &Settings.Bandwidth.Request;

	if ( Settings.General.DebugBTSources )
		theApp.Message( MSG_DEBUG, L"Adding BT client to collection: %s", m_sAddress );

	BTClients.Add( this );
}

CBTClient::~CBTClient()
{
	ASSERT( ! IsValid() );
	ASSERT( m_pDownload == NULL );
	ASSERT( m_pDownloadTransfer == NULL );
	ASSERT( m_pUploadTransfer == NULL );

	if ( Settings.General.DebugBTSources )
		theApp.Message( MSG_DEBUG, L"Removing BT client from collection: %s", m_sAddress );

	BTClients.Remove( this );
}


//////////////////////////////////////////////////////////////////////
// CBTClient initiate a new connection

BOOL CBTClient::Connect(CDownloadTransferBT* pDownloadTransfer)
{
	ASSUME_LOCK( Transfers.m_pSection );

	if ( m_bClosing ) return FALSE;
	ASSERT( ! IsValid() );
	ASSERT( m_pDownload == NULL );

	const CDownloadSource* pSource = pDownloadTransfer->GetSource();

	if ( ! CTransfer::ConnectTo( &pSource->m_pAddress, pSource->m_nPort ) ) return FALSE;

	m_pDownload			= pDownloadTransfer->GetDownload();
	m_pDownloadTransfer	= pDownloadTransfer;

	theApp.Message( MSG_INFO, IDS_BT_CLIENT_CONNECTING, m_sAddress );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTClient attach to existing connection

void CBTClient::AttachTo(CConnection* pConnection)
{
	if ( m_bClosing ) return;
	ASSERT( ! IsValid() );

	CTransfer::AttachTo( pConnection );
	if ( Settings.General.DebugBTSources )
		theApp.Message( MSG_DEBUG, L"Attaching new BT client connection: %s", m_sAddress );

	ASSERT( m_mInput.pLimit != NULL );
	m_tConnected = GetTickCount();
	theApp.Message( MSG_INFO, IDS_BT_CLIENT_ACCEPTED, m_sAddress );
}

//////////////////////////////////////////////////////////////////////
// CBTClient close

void CBTClient::Close(UINT nError)
{
	ASSERT( this != NULL );

	m_mInput.pLimit = NULL;
	m_mOutput.pLimit = NULL;

	if ( m_bClosing ) return;
	m_bClosing = TRUE;

	if ( Settings.General.DebugBTSources )
		theApp.Message( MSG_DEBUG, L"Deleting BT client: %s", m_sAddress );

	if ( m_pUploadTransfer )
		m_pUploadTransfer->Close();
	ASSERT( m_pUploadTransfer == NULL );

	if ( m_pDownloadTransfer )
	{
		if ( ! m_pDownload || m_pDownload->IsCompleted() )
			m_pDownloadTransfer->Close( TRI_FALSE );
		else
			m_pDownloadTransfer->Close( TRI_UNKNOWN );
	}
	ASSERT( m_pDownloadTransfer == NULL );

	m_pDownload = NULL;

	CTransfer::Close( nError );

	delete this;
}

//////////////////////////////////////////////////////////////////////
// CBTClient send a packet

void CBTClient::Send(CBTPacket* pPacket, BOOL bRelease)
{
	ASSERT( IsValid() );
	ASSERT( m_bOnline );

	if ( pPacket != NULL )
	{
		ASSERT( pPacket->m_nProtocol == PROTOCOL_BT );

		Statistics.Current.BitTorrent.Outgoing++;

		pPacket->SmartDump( &m_pHost, FALSE, TRUE );

		Write( pPacket );
		if ( bRelease ) pPacket->Release();
	}

	OnWrite();
}

//////////////////////////////////////////////////////////////////////
// CBTClient run event

BOOL CBTClient::OnRun()
{
	CTransfer::OnRun();

	const DWORD tNow = GetTickCount();

	if ( ! m_bConnected )
	{
		if ( tNow >= m_tConnected + Settings.Connection.TimeoutConnect )
		{
			Close( IDS_BT_CLIENT_CONNECT_TIMEOUT );
			return FALSE;
		}
	}
	else if ( ! m_bOnline )
	{
		if ( tNow >= m_tConnected + Settings.Connection.TimeoutHandshake )
		{
			Close( IDS_BT_CLIENT_HANDSHAKE_TIMEOUT );
			return FALSE;
		}
	}
	else
	{
		if ( tNow >= m_mInput.tLast + Settings.BitTorrent.LinkTimeout * 2 )
		{
			Close( IDS_BT_CLIENT_LOST );
			return FALSE;
		}
		if ( tNow >= m_tLastKeepAlive + Settings.BitTorrent.LinkPing )
		{
			Send( CBTPacket::New( BT_PACKET_KEEPALIVE ) );
			m_tLastKeepAlive = tNow;
		}

		ASSERT( m_pUploadTransfer != NULL );
		if ( tNow >= m_tLastUtPex + Settings.BitTorrent.UtPexPeriod )
		{
			SendUtPex( m_tLastUtPex );
			m_tLastUtPex = tNow;
		}

		if ( m_pDownloadTransfer && ! m_pDownloadTransfer->OnRun() ) return FALSE;
		if ( ! m_pUploadTransfer || ! m_pUploadTransfer->OnRun() ) return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTClient connection establishment event

BOOL CBTClient::OnConnected()
{
	theApp.Message( MSG_INFO, IDS_BT_CLIENT_HANDSHAKING, (LPCTSTR)m_sAddress );
	SendHandshake( TRUE, TRUE );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTClient connection loss event

void CBTClient::OnDropped()
{
	if ( ! m_bConnected )
		Close( IDS_BT_CLIENT_DROP_CONNECTING );
	else if ( ! m_bOnline )
		Close( IDS_BT_CLIENT_DROP_HANDSHAKE );
	else
		Close( IDS_BT_CLIENT_DROP_CONNECTED );
}

//////////////////////////////////////////////////////////////////////
// CBTClient write event

BOOL CBTClient::OnWrite()
{
	if ( m_bClosing )
		return FALSE;

	CTransfer::OnWrite();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTClient read event

BOOL CBTClient::OnRead()
{
	if ( m_bClosing )
		return FALSE;

	if ( ! CTransfer::OnRead() )
		return FALSE;

	CLockedBuffer pInput( GetInput() );

	BOOL bSuccess = TRUE;
	if ( m_bOnline )
	{
		while ( CBTPacket* pPacket = CBTPacket::ReadBuffer( pInput ) )
		{
			try
			{
				bSuccess = OnPacket( pPacket );
			}
			catch ( CException* pException )
			{
				pException->Delete();
				if ( ! m_bOnline )
					bSuccess = FALSE;
			}

			pPacket->Release();

			if ( ! bSuccess )
				break;

			if ( m_bClosing )
				return FALSE;
		}
	}
	else
	{
		if ( ! m_bShake && pInput->m_nLength >= BT_PROTOCOL_HEADER_LEN + 8 + Hashes::Sha1Hash::byteCount )
			bSuccess = OnHandshake1();

		if ( bSuccess && m_bShake && pInput->m_nLength >= Hashes::Sha1Hash::byteCount )
			bSuccess = OnHandshake2();

	//	else if ( bSuccess && m_bShake )
	//	{
	//		if ( GetTickCount() - m_tConnected > Settings.Connection.TimeoutHandshake / 2 )
	//			theApp.Message( MSG_ERROR, _T("No peer-id received") );
	//	}
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CBTClient handshaking

void CBTClient::SendHandshake(BOOL bPart1, BOOL bPart2)
{
	if ( m_bClosing )
		return;

	ASSERT( m_pDownload != NULL );

	if ( bPart1 )
	{
		const QWORD nFlags = BT_FLAG_EXTENSION | BT_FLAG_DHT_PORT;
		Write( _P(BT_PROTOCOL_HEADER) );
		Write( &nFlags, 8 );
		Write( m_pDownload->m_oBTH );
	}

	if ( bPart2 )
	{
		if ( ! m_pDownload->m_pPeerID )
			m_pDownload->GenerateTorrentDownloadID();

		Write( m_pDownload->m_pPeerID );
	}

	OnWrite();
}

BOOL CBTClient::OnHandshake1()
{
	// First part of the handshake
	//if ( m_bClosing ) return FALSE;

	ASSERT( ! m_bOnline );
	ASSERT( ! m_bShake );

	// Read in the BT protocol header
	if ( ! StartsWith( BT_PROTOCOL_HEADER, BT_PROTOCOL_HEADER_LEN ) )
	{
		Close( IDS_HANDSHAKE_FAIL );
		return FALSE;
	}
	Remove( BT_PROTOCOL_HEADER_LEN );

	QWORD nFlags = 0;
	Read( &nFlags, 8 );
	m_bExtended = ( nFlags & BT_FLAG_EXTENSION ) != 0;
	m_bDHTPort = ( nFlags & BT_FLAG_DHT_PORT ) != 0;

	// Read in the file ID
	Hashes::BtHash oFileHash;
	Read( oFileHash );
	oFileHash.validate();

	if ( m_pDownload != NULL )	// If we initiated download (download has associated, which means we initiated download)
	{
		ASSERT( m_pDownloadTransfer != NULL );

		if ( validAndUnequal( oFileHash, m_pDownload->m_oBTH ) )
		{
			// Display an error and exit
			Close( IDS_BT_CLIENT_WRONG_FILE );
			return FALSE;
		}
		else if ( ! m_pDownload->IsTrying() && ! m_pDownload->IsSeeding() )
		{
			// Display an error and exit
			Close( IDS_BT_CLIENT_INACTIVE_FILE );
			return FALSE;
		}
	}
	else	// Download has initiated from other side (no download has associated, which means connection initiated by G2 PUSH)
	{
		ASSERT( m_pDownloadTransfer == NULL );

		// Find the requested file
		m_pDownload = Downloads.FindByBTH( oFileHash, TRUE );

		if ( m_pDownload == NULL )										// If we can't find the file
		{
			// Display an error and exit
			Close( IDS_BT_CLIENT_UNKNOWN_FILE );
			return FALSE;
		}
		if ( ! m_pDownload->IsTrying() && ! m_pDownload->IsSeeding() )	// If the file isn't active
		{
			// Display an error and exit
			m_pDownload = NULL;
			Close( IDS_BT_CLIENT_INACTIVE_FILE );
			return FALSE;
		}
		if ( m_pDownload->UploadExists( &m_pHost.sin_addr ) )			// If there is already an upload of this file to this client
		{
			// Display an error and exit
			m_pDownload = NULL;
			Close( IDS_BT_CLIENT_DUPLICATE );
			return FALSE;
		}
		if ( m_pDownload->m_bVerify != TRI_TRUE && ( m_pDownload->IsMoving() || m_pDownload->IsCompleted() ) )
		{
			// The file isn't verified yet, close the connection
			Close( IDS_BT_CLIENT_INACTIVE_FILE );
			return FALSE;
		}

		// Check we don't have too many active torrent connections
		// (Prevent routers overloading for very popular torrents)
		if ( ( m_pDownload->GetTransferCount( dtsCountTorrentAndActive ) ) > ( Settings.BitTorrent.DownloadConnections * 1.25 ) )
		{
			Close( IDS_BT_CLIENT_MAX_CONNECTIONS );
			return FALSE;
		}
	}

	// Verify a download and hash
	ASSERT( m_pDownload != NULL );
	ASSERT( validAndEqual( m_pDownload->m_oBTH, oFileHash ) );

	// If we didn't start the connection, then send a handshake
	if ( ! m_bInitiated )
		SendHandshake( TRUE, TRUE );
	m_bShake = TRUE;

	return TRUE;
}

BOOL CBTClient::OnHandshake2()
{
	ASSUME_LOCK( Transfers.m_pSection );

	// Second part of the handshake - Peer ID
	//if ( m_bClosing ) return FALSE;

	Read( m_oGUID );
	m_oGUID.validate();

	ASSERT( m_pDownload != NULL );

	if ( CDownloadSource* pSource = GetSource() )	// Transfer exists, so must be initiated from this side
	{
		pSource->m_oGUID = transformGuid( m_oGUID );

		// ToDo: This seems to trip when it shouldn't. Should be investigated...
		//if ( memcmp( &m_pGUID, &pSource->m_pGUID, 16 ) != 0 )
		//{
		//	Close( IDS_BT_CLIENT_WRONG_GUID );
		//	return FALSE;
		//}
	}
	else	// No transfer exists, so must be initiated from other side.
	{
		if ( m_pDownload->UploadExists( m_oGUID ) )
		{
			Close( IDS_BT_CLIENT_DUPLICATE );
			return FALSE;
		}

		if ( ! m_pDownload->IsMoving() && ! m_pDownload->IsPaused() )
		{
			ASSERT( m_pDownloadTransfer == NULL );

			// Download from uploaders, unless the user has turned off downloading for this torrent
			if ( m_pDownload->m_pTorrent.m_nStartDownloads != CBTInfo::dtNever )
			{
				// This seems to be set to null sometimes... DownloadwithTorrent: if ( pSource->m_pTransfer != NULL )
				// May just be clients sending duplicate connection requests, though...
				m_pDownloadTransfer = m_pDownload->CreateTorrentTransfer( this );

				if ( m_pDownloadTransfer == NULL )
				{
					m_pDownload = NULL;
					Close( IDS_BT_CLIENT_UNKNOWN_FILE );
					return FALSE;
				}
			}
		}
	}

	ASSERT( m_pUploadTransfer == NULL );
	if ( Settings.General.DebugBTSources )
		theApp.Message( MSG_DEBUG, L"Creating new BT upload: %s", m_sAddress );
	m_pUploadTransfer = new CUploadTransferBT( this, m_pDownload );

	m_bOnline = TRUE;

	DetermineUserAgent();

	// OnOnline
	theApp.Message( MSG_INFO, IDS_BT_CLIENT_ONLINE, (LPCTSTR)m_sAddress, (LPCTSTR)m_pDownload->GetDisplayName() );

	if ( ! m_pDownload->IsTorrent() )	// Perhaps download just finished, ToDo: Investigate this.
	{
		m_pDownload = NULL;
		Close( IDS_BT_CLIENT_UNKNOWN_FILE );
		return FALSE;
	}

	if ( m_bExtended )
		SendExtendedHandshake();

	if ( m_bDHTPort )
		SendDHTPort();

	if ( m_bClientExtended )
		SendBeHandshake();

	if ( CBTPacket* pBitfield = m_pDownload->CreateBitfieldPacket() )
		Send( pBitfield );

	if ( m_pDownloadTransfer != NULL && ! m_pDownloadTransfer->OnConnected() )
		return FALSE;

	if ( ! m_pUploadTransfer->OnConnected() )
		return FALSE;

	if ( Uploads.GetTorrentUploadCount() < Settings.BitTorrent.UploadCount )
	{
		m_pUploadTransfer->m_bChoked = FALSE;
		UnChoke();					// Send BT_PACKET_UNCHOKE
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTClient online handler
//
// See http://bittorrent.org/beps/bep_0020.html
// See http://wiki.theory.org/BitTorrentSpecification
// ToDo: Update/Optimize/Abstract these PeerID vendor lists

// Legacy Method:
//CString CBTClient::GetUserAgentAzureusStyle(LPBYTE pVendor, size_t nVendor)
//{
//	CString strUserAgent;
//	if ( ! pVendor ) return strUserAgent;
//
//	// Azureus style "-SSVVVV-"
//
//	struct azureusStyleEntry
//	{
//		uchar signature[ 2 ];
//		LPCTSTR client;
//	};
//
//	static const azureusStyleEntry azureusStyleClients[] =
//	{
//		{ 'A', '~', L"Ares" },
//		{ 'A', 'G', L"Ares" },
//		{ 'A', 'R', L"Arctic" },
//		{ 'A', 'V', L"Avicora" },
//		{ 'A', 'X', L"BitPump" },
//		{ 'A', 'Z', L"Azureus" },	// +Frostwire/etc.
//		{ 'B', 'B', L"BitBuddy" },
//		{ 'B', 'C', L"BitComet" },
//		{ 'B', 'F', L"Bitflu" },
//		{ 'B', 'G', L"BTG" },
//		{ 'b', 'k', L"BitKitten" },
//		{ 'B', 'R', L"BitRocket" },
//		{ 'B', 'S', L"BitSlave" },
//		{ 'B', 'X', L"Bittorrent X" },
//		{ 'C', 'B', L"ShareazaPlus" },
//		{ 'C', 'D', L"Enhanced CTorrent" },
//		{ 'C', 'T', L"CTorrent" },
//		{ 'D', 'E', L"DelugeTorrent" },
//		{ 'E', 'B', L"EBit" },
//		{ 'E', 'S', L"Electric Sheep" },
//		{ 'F', 'C', L"FileCroc" },
//		{ 'F', 'G', L"FlashGet" },	// vXX.XX
//		{ 'G', 'R', L"GetRight" },
//		{ 'H', 'K', L"Hekate" },
//		{ 'H', 'L', L"Halite" },
//		{ 'H', 'N', L"Hydranode" },
//		{ 'K', 'T', L"KTorrent" },
//		{ 'L', 'C', L"Leechcraft" },
//		{ 'L', 'H', L"LH-ABC" },
//		{ 'L', 'K', L"Linkage" },
//		{ 'L', 'P', L"Lphant" },
//		{ 'L', 'T', L"libtorrent" },
//		{ 'l', 't', L"libtorrent" },
//		{ 'L', 'W', L"LimeWire" },
//		{ 'M', 'K', L"Meerkat" },
//		{ 'M', 'R', L"Miro" },
//		{ 'M', 'O', L"Mono Torrent" },
//		{ 'M', 'P', L"MooPolice" },
//		{ 'M', 'T', L"MoonlightTorrent" },
//		{ 'O', 'S', L"OneSwarm" },
//		{ 'P', 'C', L"CacheLogic" },
//		{ 'P', 'D', L"Pando" },
//		{ 'P', 'E', L"PeerProject" },
//		{ 'P', 'T', L"PHPTracker" },
//		{ 'p', 'X', L"pHoeniX" },
//		{ 'q', 'B', L"qBittorrent" },
//		{ 'Q', 'T', L"QT4" },
//		{ 'R', 'T', L"Retriever" },
//		{ 'S', 'B', L"SwiftBit" },
//		{ 'S', 'M', L"SoMud" },
//		{ 'S', 'N', L"ShareNet" },
//		{ 'S', 'S', L"Swarmscope" },
//		{ 's', 't', L"Sharktorrent" },
//		{ 'S', 'T', L"SymTorrent" },
//		{ 'S', 'Z', L"Shareaza" },
//		{ 'S', '~', L"ShareazaBeta" },
//		{ 'T', 'N', L"Torrent.NET" },
//		{ 'T', 'R', L"Transmission" },
//		{ 'T', 'S', L"TorrentStorm" },
//		{ 'T', 'T', L"TuoTu" },
//		{ 'U', 'L', L"uLeecher!" },
//		{ 'U', 'M', L"\x00B5Torrent mac" },
//		{ 'U', 'T', L"\x00B5Torrent" },
//		{ 'W', 'Y', L"FireTorrent" },
//		{ 'X', 'L', L"Xunlei" },
//		{ 'X', 'T', L"XanTorrent" },
//		{ 'X', 'X', L"xTorrent" },
//		{ 'Z', 'T', L"ZipTorrent" }
//	};
//	static const size_t azureusClients =
//		sizeof azureusStyleClients / sizeof azureusStyleEntry;
//
//	for ( size_t i = 0 ; i < azureusClients ; ++i )
//	{
//		if ( pVendor[ 0 ] == azureusStyleClients[ i ].signature[ 0 ] &&
//				pVendor[ 1 ] == azureusStyleClients[ i ].signature[ 1 ] )
//		{
//			if ( nVendor == 6 )
//			{
//				strUserAgent.Format( _T("%s %i.%i.%i.%i"),
//					azureusStyleClients[ i ].client,
//					( pVendor[ 2 ] - '0' ), ( pVendor[ 3 ] - '0' ),
//					( pVendor[ 4 ] - '0' ), ( pVendor[ 5 ] - '0' ) );
//			}
//			else if ( nVendor == 4 )
//			{
//				strUserAgent.Format( _T("%s %i.%i"),
//					azureusStyleClients[ i ].client, pVendor[ 2 ], pVendor[ 3 ] );
//			}
//			break;
//		}
//	}
//
//	if ( strUserAgent.IsEmpty() ) 	// If we don't want the version, etc.
//		strUserAgent.Format( _T("BitTorrent (%c%c)"), pVendor[ 0 ], pVendor[ 1 ] );
//
//	return strUserAgent;
//}

CString CBTClient::GetUserAgentAzureusStyle(LPBYTE pVendor, size_t nVendor)
{
	CString strUserAgent;
	if ( ! pVendor ) return strUserAgent;

	// Azureus style "-SSVVVV-"
	static std::map < const CString, CString > Vendors;
	if ( Vendors.empty() )
	{
		Vendors[ L"A~" ] = L"Ares";
		Vendors[ L"AG" ] = L"Ares";
		Vendors[ L"AR" ] = L"Arctic";
		Vendors[ L"AV" ] = L"Avicora";
		Vendors[ L"AX" ] = L"BitPump";
		Vendors[ L"AZ" ] = L"Azureus";	// +Frostwire/etc.
		Vendors[ L"BB" ] = L"BitBuddy";
		Vendors[ L"BC" ] = L"BitComet";
		Vendors[ L"BF" ] = L"Bitflu";
		Vendors[ L"BG" ] = L"BTG";
	//	Vendors[ L"BO" ] = L"BO ";		// ?
		Vendors[ L"bk" ] = L"BitKitten";
		Vendors[ L"BR" ] = L"BitRocket";
		Vendors[ L"BS" ] = L"BitSlave";
		Vendors[ L"BX" ] = L"Bittorrent X";
		Vendors[ L"CB" ] = L"ShareazaPlus";
		Vendors[ L"CD" ] = L"CTorrent";	// "Enhanced"
		Vendors[ L"CT" ] = L"CTorrent";
		Vendors[ L"DE" ] = L"DelugeTorrent";
		Vendors[ L"EB" ] = L"EBit";
		Vendors[ L"ES" ] = L"Electric Sheep";
		Vendors[ L"FC" ] = L"FileCroc";
		Vendors[ L"FG" ] = L"FlashGet";	// vXX.XX
		Vendors[ L"GR" ] = L"GetRight";
		Vendors[ L"HK" ] = L"Hekate";
		Vendors[ L"HL" ] = L"Halite";
		Vendors[ L"HN" ] = L"Hydranode";
		Vendors[ L"KT" ] = L"KTorrent";
		Vendors[ L"LC" ] = L"Leechcraft";
		Vendors[ L"LH" ] = L"LH-ABC";
		Vendors[ L"LK" ] = L"Linkage";
		Vendors[ L"LP" ] = L"Lphant";
		Vendors[ L"LT" ] = L"Libtorrent";
		Vendors[ L"lt" ] = L"libtorrent";
		Vendors[ L"LW" ] = L"LimeWire";
		Vendors[ L"MK" ] = L"Meerkat";
		Vendors[ L"MR" ] = L"Miro";
		Vendors[ L"MO" ] = L"Mono Torrent";
		Vendors[ L"MP" ] = L"MooPolice";
		Vendors[ L"MT" ] = L"Moonlight";
		Vendors[ L"OS" ] = L"OneSwarm";
		Vendors[ L"PC" ] = L"CacheLogic";
		Vendors[ L"PD" ] = L"Pando";
		Vendors[ L"PE" ] = L"PeerProject";
		Vendors[ L"PP" ] = L"PeerProject";
		Vendors[ L"PT" ] = L"PHPTracker";
		Vendors[ L"pX" ] = L"pHoeniX";
		Vendors[ L"qB" ] = L"qBittorrent";
		Vendors[ L"QD" ] = L"QQDownload";
		Vendors[ L"QT" ] = L"QT4";
		Vendors[ L"RT" ] = L"Retriever";
		Vendors[ L"SB" ] = L"SwiftBit";
		Vendors[ L"SM" ] = L"SoMud";
		Vendors[ L"SN" ] = L"ShareNet";
		Vendors[ L"SS" ] = L"Swarmscope";
		Vendors[ L"st" ] = L"Sharktorrent";
		Vendors[ L"ST" ] = L"SymTorrent";
		Vendors[ L"SZ" ] = L"Shareaza";
		Vendors[ L"S~" ] = L"ShareazaBeta";
		Vendors[ L"TN" ] = L"Torrent.NET";
		Vendors[ L"TR" ] = L"Transmission";
		Vendors[ L"TS" ] = L"TorrentStorm";
		Vendors[ L"TT" ] = L"TuoTu";
		Vendors[ L"UL" ] = L"uLeecher";
		Vendors[ L"UM" ] = L"\x00B5Torrent mac";
		Vendors[ L"UT" ] = L"\x00B5Torrent";
		Vendors[ L"WY" ] = L"FireTorrent";
		Vendors[ L"XC" ] = L"XC ";		// ?
		Vendors[ L"XL" ] = L"Xunlei";
		Vendors[ L"XT" ] = L"XanTorrent";
		Vendors[ L"XX" ] = L"xTorrent";
		Vendors[ L"ZT" ] = L"ZipTorrent";
	}

	strUserAgent = Vendors[ CString( pVendor ).Left( 2 ) ];

	if ( strUserAgent.IsEmpty() ) 	// If we don't want the version, etc.
		strUserAgent.Format( _T("BitTorrent (%c%c)"), pVendor[ 0 ], pVendor[ 1 ] );
	else if ( nVendor == 6 )
		strUserAgent.Format( _T("%s %i.%i.%i.%i"), strUserAgent, ( pVendor[ 2 ] - '0' ), ( pVendor[ 3 ] - '0' ), ( pVendor[ 4 ] - '0' ), ( pVendor[ 5 ] - '0' ) );
	else //if ( nVendor == 4 )
		strUserAgent.Format( _T("%s %i.%i"), strUserAgent, pVendor[ 2 ], pVendor[ 3 ] );

	return strUserAgent;
}

CString CBTClient::GetUserAgentOtherStyle(LPBYTE pVendor, CString* strNick)
{
	CString strUserAgent, strVer;
	int nNickStart = 0, nNickEnd = 13;

	if ( ! pVendor ) return strUserAgent;

	if ( m_oGUID[4] == '-' && m_oGUID[5] == '-' && m_oGUID[6] == '-' && m_oGUID[7] == '-' )
	{
		// Shadow style
		switch ( m_oGUID[0] )
		{
		case 'A':
			strUserAgent = L"ABC";
			break;
		case 'O':
			strUserAgent = L"Osprey";
			break;
		case 'Q':
			strUserAgent = L"BTQueue";
			break;
		case 'R':
			strUserAgent = L"Tribler";
			break;
		case 'S':
			strUserAgent = L"Shadow";
			break;
		case 'T':
			strUserAgent = L"BitTornado";
			break;
		case 'U':
			strUserAgent = L"UPnP NAT BT";
			break;
		default:	// Unknown client using this naming.
			strUserAgent.Format(_T("%c"), m_oGUID[0]);
		}

		strVer.Format( _T(" %i.%i.%i"),
			( m_oGUID[1] - '0' ), ( m_oGUID[2] - '0' ), ( m_oGUID[3] - '0' ) );
		strUserAgent += strVer;

		return strUserAgent;
	}

	if ( m_oGUID[0] == 'M' && m_oGUID[2] == '-' && m_oGUID[4] == '-' && m_oGUID[6] == '-' )
	{
		// BitTorrent (Mainline standard client, newer version)
		strUserAgent.Format( _T("BitTorrent %i.%i.%i"), m_oGUID[1] - '0' , m_oGUID[3] - '0' , m_oGUID[5]- '0' );
	}
	else if ( m_oGUID[0] == 'P' && m_oGUID[1] == 'l' && m_oGUID[2] == 'u' && m_oGUID[3] == 's' )
	{
		// BitTorrent Plus
		strUserAgent.Format( _T("BitTorrent Plus %i.%i%i%c"), m_oGUID[4] - '0', m_oGUID[5] - '0', m_oGUID[6] - '0', m_oGUID[7] );
	}
	else if ( m_oGUID[0] == 'e' && m_oGUID[1] == 'x' && m_oGUID[2] == 'b' && m_oGUID[3] == 'c' )
	{
		// BitLord
		if ( m_oGUID[6] == 'L' && m_oGUID[7] == 'O' && m_oGUID[8] == 'R' && m_oGUID[9] == 'D' )
			strUserAgent.Format( _T("BitLord %i.%02i"), m_oGUID[4], m_oGUID[5] );
		else // Old BitComet
			strUserAgent.Format( _T("BitComet %i.%02i"), m_oGUID[4], m_oGUID[5] );
	}
	else if ( ( m_oGUID[0] == 'B' && m_oGUID[1] == 'S' ) || ( m_oGUID[2] == 'B' && m_oGUID[3] == 'S' ) )
	{
		// BitSpirit
		strUserAgent.Format( _T("BitSpirit") );
	}
	else if ( m_oGUID[0] == 'B' && m_oGUID[1] == 'T' && m_oGUID[2] == 'M' )
	{
		// BTuga Revolution
		strUserAgent.Format( _T("BTuga Rv %i.%i"), m_oGUID[3] - '0', m_oGUID[4] - '0' );
		nNickStart = 5;
	}
	else if ( ( m_oGUID[0] == 'b' && m_oGUID[1] == 't' && m_oGUID[2] == 'u' && m_oGUID[3] == 'g' && m_oGUID[4] == 'a' ) || ( m_oGUID[0] == 'o' && m_oGUID[1] == 'e' && m_oGUID[2] == 'r' && m_oGUID[3] == 'n' && m_oGUID[4] == 'u' ) )
	{
		// BTugaXP
		strUserAgent.Format( _T("BTugaXP") );
	}
	else if ( m_oGUID[0] == 'M' && m_oGUID[1] == 'b' && m_oGUID[2] == 'r' && m_oGUID[3] == 's' && m_oGUID[4] == 't' )
	{
		// Burst
		strUserAgent.Format( _T("Burst %i.%i.%i"), m_oGUID[5] - '0', m_oGUID[7] - '0', m_oGUID[9] - '0' );
	}
	else if ( m_oGUID[0] == 'L' && m_oGUID[1] == 'I' && m_oGUID[2] == 'M' && m_oGUID[3] == 'E' )
	{
		// LimeWire
		strUserAgent = _T("Limewire");
	}
	else if ( m_oGUID[0] == '-' && m_oGUID[1] == 'M' && m_oGUID[2] == 'L' )
	{
		// MLdonkey
		strUserAgent.Format( _T("MLdonkey %i.%i.%i"), m_oGUID[3] - '0' , m_oGUID[5] - '0' , m_oGUID[7] - '0' );
	}
	else if ( m_oGUID[0] == '-' && m_oGUID[1] == 'F' && m_oGUID[2] == 'G' )
	{
		// FlashGet 	// This is never reached (Azureus-style but vXX.XX)
		strUserAgent.Format( _T("FlashGet %i.%i%i"), ( ( m_oGUID[3] - '0' ) * 10 + ( m_oGUID[4] - '0' ) ), m_oGUID[5] - '0', m_oGUID[6] - '0' );
	}
	else if ( m_oGUID[0] == '-' && m_oGUID[1] == 'G' && m_oGUID[2] == '3' )
	{
		// G3 Torrent
		strUserAgent.Format( _T("G3 Torrent") );
		nNickStart = 3;
		nNickEnd = 11;
	}
	else if ( m_oGUID[0] == '1' && m_oGUID[1] == '0' && m_oGUID[2] == '-' && m_oGUID[3] == '-' && m_oGUID[4] == '-' && m_oGUID[5] == '-' && m_oGUID[6] == '-' && m_oGUID[7] == '-' && m_oGUID[8] == '-' )
	{
		strUserAgent = _T("JVTorrent");
	}
	else if ( m_oGUID[2] == 'B' && m_oGUID[3] == 'M' )
	{
		// BitMagnet, former Rufus
		strUserAgent.Format( _T("BitMagnet") );
		nNickStart = 4;
		nNickEnd = 12;
	}
	else if ( m_oGUID[2] == 'R' && m_oGUID[3] == 'S' )
	{
		// Rufus (and BitMagnet) Obsolete. First two bits are version numbers: 0.6.5 = (0)(6+5) = char(0)char(11)
		strUserAgent.Format( _T("Rufus") );
		nNickStart = 4;
		nNickEnd = 12;
	}
	else if ( m_oGUID[0] == 'O' && m_oGUID[1] == 'P' )
	{
		// Opera
		strUserAgent.Format( _T("Opera %i%i%i%i"), m_oGUID[2] - '0', m_oGUID[3] - '0', m_oGUID[4] - '0', m_oGUID[5] - '0' );
	}
	else if ( m_oGUID[0] == 'a' && m_oGUID[1] == '0' && ( m_oGUID[2] == '0' || m_oGUID[2] == '2' ) && m_oGUID[3] == '-' && m_oGUID[4] == '-' && m_oGUID[5] == '-' && m_oGUID[6] == '0' )
	{
		// Swarmy
		strUserAgent.Format( _T("Swarmy") );
	}
	else if ( m_oGUID[0] == '3' && m_oGUID[1] == '4' && m_oGUID[2] == '6' && m_oGUID[3] == '-' )
	{
		// TorrentTopia
		strUserAgent = _T("TorrentTopia");
	}
	else if ( m_oGUID[0] == 'e' && m_oGUID[1] == 'X' )
	{
		// eXeem
		strUserAgent.Format( _T("eXeem") );
		nNickStart = 2;
	}
	else if ( m_oGUID[0] == 'X' && m_oGUID[1] == 'B' && m_oGUID[2] == 'T' )
	{
		// XBT
		strUserAgent.Format( _T("XBT %i.%i.%i"), m_oGUID[3] - '0', m_oGUID[4] - '0', m_oGUID[5] - '0' );
	}
	else if ( ! m_oGUID[0] && ! m_oGUID[1] && ! m_oGUID[2] && ! m_oGUID[3] && ! m_oGUID[4] && ! m_oGUID[5] && ! m_oGUID[6] && ! m_oGUID[7]
			&& m_oGUID[8] && m_oGUID[9] && m_oGUID[10] && m_oGUID[11] && m_oGUID[12] && m_oGUID[13] && m_oGUID[14] && m_oGUID[15]
			&& m_oGUID[16] == 'U' && m_oGUID[17] == 'D' && m_oGUID[18] == 'P' && m_oGUID[19] == '0' )
	{
		// BitSpirit (Spoofed Client ID)
		// GUID 0-7: 0	GUID 8-15: !0 	GUID 16-19: UDP0	// ToDo: Check that other clients don't use this method
		strUserAgent.Format( _T("BitSpirit Spoof") );
	}

	if ( nNickStart > 0 )
	{
		for ( int i = nNickStart ; i <= nNickEnd ; i++ )	// Extract nick from m_oGUID
		{
			if ( m_oGUID[i] == NULL ) break;

			(*strNick).AppendFormat( _T("%c"), m_oGUID[i] );
		}
	}

	return strUserAgent;
}

void CBTClient::DetermineUserAgent()
{
	ASSUME_LOCK( Transfers.m_pSection );

	CString strNick;
	m_sUserAgent.Empty();
	m_bClientExtended = isExtendedBtGuid( m_oGUID );

	if ( m_oGUID[ 0 ] == '-' && m_oGUID[ 7 ] == '-' )		// -PE1000-
		m_sUserAgent = GetUserAgentAzureusStyle( &m_oGUID[1], 6 );
	else
		m_sUserAgent = GetUserAgentOtherStyle( &m_oGUID[0], &strNick );

	if ( ! m_bClientExtended )
		m_bClientExtended = VendorCache.IsExtended( m_sUserAgent );

	if ( m_sUserAgent.IsEmpty() )
	{
		// Unknown peer ID string
		m_sUserAgent = m_bClientExtended ? _T("PeerProject?") : _T("BitTorrent?");
		theApp.Message( MSG_DEBUG, _T("[BT] Unknown client: %.20hs"), (LPCSTR)&m_oGUID[0] );
	}

	if ( m_pDownloadTransfer )
	{
		m_pDownloadTransfer->m_sUserAgent = m_sUserAgent;
		m_pDownloadTransfer->m_bClientExtended = m_bClientExtended;
		if ( CDownloadSource* pSource = GetSource() )
		{
			pSource->m_sServer = m_sUserAgent;
			if ( ! strNick.IsEmpty() )
				pSource->m_sNick = strNick;
			pSource->m_bClientExtended = ( m_bClientExtended && ! pSource->m_bPushOnly);
		}
	}

	if ( m_pUploadTransfer )
	{
		m_pUploadTransfer->m_sUserAgent = m_sUserAgent;
		if ( ! strNick.IsEmpty() )
			m_pUploadTransfer->m_sRemoteNick = strNick;
		m_pUploadTransfer->m_bClientExtended = m_bClientExtended;
	}
}

CDownloadSource* CBTClient::GetSource() const
{
	return m_pDownloadTransfer ? m_pDownloadTransfer->GetSource() : NULL;
}


//////////////////////////////////////////////////////////////////////
// CBTClient packet switch

BOOL CBTClient::OnPacket(CBTPacket* pPacket)
{
	Statistics.Current.BitTorrent.Incoming++;

	pPacket->SmartDump( &m_pHost, FALSE, FALSE );

	switch ( pPacket->m_nType )
	{
	case BT_PACKET_KEEPALIVE:
		break;
	case BT_PACKET_CHOKE:
		if ( m_pDownloadTransfer != NULL && ! m_pDownloadTransfer->OnChoked( pPacket ) ) return FALSE;
		m_pDownload->ChokeTorrent();
		break;
	case BT_PACKET_UNCHOKE:
		if ( m_pDownloadTransfer != NULL && ! m_pDownloadTransfer->OnUnchoked( pPacket ) ) return FALSE;
		m_pDownload->ChokeTorrent();
		break;
	case BT_PACKET_INTERESTED:
		if ( ! m_pUploadTransfer->OnInterested( pPacket ) ) return FALSE;
		m_pDownload->ChokeTorrent();
		break;
	case BT_PACKET_NOT_INTERESTED:
		if ( ! m_pUploadTransfer->OnUninterested( pPacket ) ) return FALSE;
		m_pDownload->ChokeTorrent();
		break;
	case BT_PACKET_HAVE:
		return m_pDownloadTransfer == NULL || m_pDownloadTransfer->OnHave( pPacket );
	case BT_PACKET_BITFIELD:
		return m_pDownloadTransfer == NULL || m_pDownloadTransfer->OnBitfield( pPacket );
	case BT_PACKET_REQUEST:
		return m_pUploadTransfer->OnRequest( pPacket );
	case BT_PACKET_PIECE:
		return m_pDownloadTransfer == NULL || m_pDownloadTransfer->OnPiece( pPacket );
	case BT_PACKET_CANCEL:
		return m_pUploadTransfer->OnCancel( pPacket );
	case BT_PACKET_DHT_PORT:
		return OnDHTPort( pPacket );
	case BT_PACKET_EXTENSION:
		switch ( pPacket->m_nExtension )
		{
		case BT_EXTENSION_HANDSHAKE:
			return OnExtendedHandshake( pPacket );
		case BT_EXTENSION_UT_METADATA:
			return OnMetadataRequest( pPacket );
		case BT_EXTENSION_UT_PEX:
			return OnUtPex( pPacket );
		case BT_EXTENSION_LT_TEX:
			return OnLtTex( pPacket );
		}
		break;
	case BT_PACKET_HANDSHAKE:
		return OnBeHandshake( pPacket );
	case BT_PACKET_SOURCE_REQUEST:
		return OnSourceRequest( pPacket );
	case BT_PACKET_SOURCE_RESPONSE:
		return m_pDownloadTransfer == NULL || m_pDownloadTransfer->OnSourceResponse( pPacket );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTClient advanced handshake

void CBTClient::SendBeHandshake()
{
	//if ( m_bClosing ) return;

	// Send extended handshake (for G2 capable clients: PeerProject/Shareaza)
	CBTPacket* pPacket = CBTPacket::New( BT_PACKET_HANDSHAKE );
	CBENode* pRoot = pPacket->m_pNode.get();
	ASSERT( pRoot );

	CString strNick = MyProfile.GetNick().Left( 255 );								// Truncate to 255 characters
	if ( ! strNick.IsEmpty() )
		pRoot->Add( BT_DICT_NICKNAME )->SetString( strNick );						// "nickname"

	pRoot->Add( BT_DICT_SRC_EXCHANGE )->SetInt( BT_HANDSHAKE_SOURCE );				// "source-exchange"
	pRoot->Add( BT_DICT_USER_AGENT )->SetString( Settings.SmartAgent() );			// "user-agent"

	Send( pPacket );
}

BOOL CBTClient::OnBeHandshake(CBTPacket* pPacket)
{
	ASSUME_LOCK( Transfers.m_pSection );

	// On extended handshake (for G2 capable clients)

	const CBENode* pRoot = pPacket->m_pNode.get();
	ASSERT( pRoot );

	if ( const CBENode* pAgent = pRoot->GetNode( BT_DICT_USER_AGENT ) )				// "user-agent"
	{
		if ( pAgent->IsType( CBENode::beString ) )
		{
			m_sUserAgent = pAgent->GetString();

			if ( m_pDownloadTransfer != NULL )
			{
				m_pDownloadTransfer->m_sUserAgent = m_sUserAgent;
				if ( CDownloadSource* pSource = GetSource() )
				{
					pSource->m_sServer = m_sUserAgent;
					pSource->m_bClientExtended = TRUE;
				}
			}

			if ( m_pUploadTransfer )
			{
				m_pUploadTransfer->m_sUserAgent = m_sUserAgent;
				m_pUploadTransfer->m_bClientExtended = TRUE;
			}
		}
	}

	if ( const CBENode* pNick = pRoot->GetNode( BT_DICT_NICKNAME ) )				// "nickname"
	{
		if ( pNick->IsType( CBENode::beString ) )
		{
			if ( CDownloadSource* pSource = GetSource() )
				pSource->m_sNick = pNick->GetString();
		}
	}

	if ( const CBENode* pExchange = pRoot->GetNode( BT_DICT_SRC_EXCHANGE ) )		// "source-exchange"
	{
		m_nSrcExchangeID = (QWORD)pExchange->GetInt();
		SendSourceRequest();														// BT_PACKET_SOURCE_REQUEST
	}

	theApp.Message( MSG_INFO, IDS_BT_CLIENT_EXTENDED, (LPCTSTR)m_sAddress, (LPCTSTR)m_sUserAgent );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTClient source request

void CBTClient::SendSourceRequest()
{
	if ( m_nSrcExchangeID && m_pDownloadTransfer )
		Send( CBTPacket::New( BT_PACKET_SOURCE_REQUEST ) );
}

BOOL CBTClient::OnSourceRequest(CBTPacket* /*pPacket*/)
{
	//if ( m_bClosing ) return TRUE;

	if ( ! m_pDownload || m_pDownload->m_pTorrent.m_bPrivate )
		return TRUE;

	CBTPacket* pResponse = CBTPacket::New( BT_PACKET_SOURCE_RESPONSE );
	CBENode* pRoot = pResponse->m_pNode.get();
	ASSERT( pRoot );

	CBENode* pPeers = pRoot->Add( BT_DICT_PEERS );									// "peers"
	for ( POSITION posSource = m_pDownload->GetIterator() ; posSource ; )
	{
		CDownloadSource* pSource = m_pDownload->GetNext( posSource );

		if ( ! pSource->IsConnected() )
			continue;

		if ( pSource->m_nProtocol == PROTOCOL_BT )
		{
			CBENode* pPeer = pPeers->Add();
			CPeerProjectURL oURL;

			if ( oURL.Parse( pSource->m_sURL ) && oURL.m_oBTC )
				pPeer->Add( BT_DICT_PEER_ID )->SetString( &*oURL.m_oBTC.begin(), oURL.m_oBTC.byteCount );	// "peer id"

			pPeer->Add( BT_DICT_PEER_IP )->SetString( CString( inet_ntoa( pSource->m_pAddress ) ) );	// "ip"
			pPeer->Add( BT_DICT_PEER_PORT )->SetInt( pSource->m_nPort );			// "port"
		}
		else if ( pSource->m_nProtocol == PROTOCOL_HTTP &&
				pSource->m_bReadContent == TRUE &&
				pSource->m_bPushOnly == FALSE )
		{
			CBENode* pPeer = pPeers->Add();
			pPeer->Add( BT_DICT_PEER_URL )->SetString( pSource->m_sURL );			// "url"
		}
	}

	if ( pPeers->GetCount() == 0 )
	{
		pResponse->Release();
		return TRUE;
	}

	Send( pResponse );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CBTClient DHT

void CBTClient::SendDHTPort()
{
	if ( CBTPacket* pResponse = CBTPacket::New( BT_PACKET_DHT_PORT ) )
	{
		pResponse->WriteShortBE( Network.GetPort() );

		Send( pResponse );
	}
}

BOOL CBTClient::OnDHTPort(CBTPacket* pPacket)
{
	// Test this node via UDP
	if ( pPacket && pPacket->GetRemaining() == 2 )
		DHT.Ping( &m_pHost.sin_addr, pPacket->ReadShortBE() );

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CBTClient Extension  (.torrentless)

void CBTClient::SendExtendedHandshake()
{
	if ( CBTPacket* pResponse = CBTPacket::New( BT_PACKET_EXTENSION, BT_EXTENSION_HANDSHAKE ) )
	{
		if ( CBENode* pRoot = pResponse->m_pNode.get() )
		{
			if ( CBENode* pM = pRoot->Add( BT_DICT_EXT_MSG ) )						// "m"
			{
				if ( m_pDownload->IsTorrent() && ! m_pDownload->m_pTorrent.m_bPrivate )
				{
					pM->Add( BT_DICT_UT_METADATA )->SetInt( BT_EXTENSION_UT_METADATA );		// "ut_metadata"
					if ( m_pDownload->m_pTorrent.GetInfoSize() )
						pRoot->Add( BT_DICT_METADATA_SIZE )->SetInt( m_pDownload->m_pTorrent.GetInfoSize() );	// "metadata_size"

					pM->Add( BT_DICT_LT_TEX )->SetInt( BT_EXTENSION_LT_TEX );		// "lt_tex"
					pRoot->Add( BT_DICT_TRACKERS )->SetString( m_pDownload->m_pTorrent.GetTrackerHash() );
				}

				pM->Add( BT_DICT_UT_PEX )->SetInt( BT_EXTENSION_UT_PEX );			// "ut_pex"

				pRoot->Add( BT_DICT_PORT )->SetInt( Network.GetPort() );			// "p"
				pRoot->Add( BT_DICT_VENDOR )->SetString( Settings.SmartAgent() );	// "v"

				Send( pResponse, FALSE );
			}
		}

		pResponse->Release();
	}
}

BOOL CBTClient::OnExtendedHandshake(CBTPacket* pPacket)
{
	const CBENode* pRoot = pPacket->m_pNode.get();

	CBENode* pYourIP = pRoot->GetNode( BT_DICT_YOURIP );							// "yourip"
	if ( pYourIP && pYourIP->IsType( CBENode::beString ) )
	{
		if ( pYourIP->m_nValue == 4 )	// IPv4
			Network.AcquireLocalAddress( *(IN_ADDR*)pYourIP->m_pValue );
	}

	// BT_EXTENSION_HANDSHAKE
	if ( CBENode* pMetadata = pRoot->GetNode( BT_DICT_EXT_MSG ) )					// "m"
	{
		if ( CBENode* pUtMetadata = pMetadata->GetNode( BT_DICT_UT_METADATA ) )		// "ut_metadata"
		{
			m_nUtMetadataID = (QWORD)pUtMetadata->GetInt();
			if ( m_nUtMetadataID && ! m_pDownload->m_pTorrent.m_pBlockBTH )			// Send first info request
			{
				const int nNextPiece = m_pDownload->m_pTorrent.NextInfoPiece();
				if ( nNextPiece >= 0 )
					SendInfoRequest( nNextPiece );
			}
		}

		if ( CBENode* pUtMetadataSize = pRoot->GetNode( BT_DICT_METADATA_SIZE ) )	// "metadata_size"
		{
			m_nUtMetadataSize = (QWORD)pUtMetadataSize->GetInt();
		}
		if ( CBENode* pUtPex = pMetadata->GetNode( BT_DICT_UT_PEX ) )				// "ut_pex" Peer-exchange
		{
			QWORD nOldUtPexID = m_nUtPexID;
			m_nUtPexID = (QWORD)pUtPex->GetInt();
			if ( ! nOldUtPexID && m_nUtPexID )
			{
				SendUtPex();
				m_tLastUtPex = GetTickCount();
			}
		}

		if ( CBENode* pLtTex = pMetadata->GetNode( BT_DICT_LT_TEX ) )
		{
			m_nLtTexID = (QWORD)pLtTex->GetInt();
		}
		if ( CBENode* pLtTexTrackers = pRoot->GetNode( BT_DICT_TRACKERS ) )
		{
			if ( m_nLtTexID )
			{
				CString strRemoteHash = pLtTexTrackers->GetString();
				CString strLocalHash = m_pDownload->m_pTorrent.GetTrackerHash();
				if ( strRemoteHash != strLocalHash )
					SendLtTex();
			}
		}
	}

	return TRUE;
}

void CBTClient::SendInfoRequest(QWORD nPiece)
{
	BYTE nUtMetadataID = m_nUtMetadataID ? m_nUtMetadataID : BT_EXTENSION_UT_METADATA;
	CBTPacket* pResponse = CBTPacket::New( BT_PACKET_EXTENSION, nUtMetadataID );
	CBENode* pRoot = pResponse->m_pNode.get();

	pRoot->Add( BT_DICT_MSG_TYPE )->SetInt( UT_METADATA_REQUEST );					// "msg_type" Request
	pRoot->Add( BT_DICT_PIECE )->SetInt( nPiece );									// "piece"

	Send( pResponse );
}

void CBTClient::SendMetadataRequest(QWORD nPiece)
{
	ASSERT( m_nUtMetadataID );

	CBTPacket* pResponse = CBTPacket::New( BT_PACKET_EXTENSION, m_nUtMetadataID );
	CBENode* pRoot = pResponse->m_pNode.get();
	ASSERT( pRoot );

	pRoot->Add( BT_DICT_PIECE )->SetInt( nPiece );									// "piece"

	CBuffer pOutput;

	BYTE* pInfoPiece = NULL;
	DWORD InfoLen = m_pDownload->m_pTorrent.GetInfoPiece( nPiece, &pInfoPiece );
	if ( InfoLen == 0 || m_pDownload->m_pTorrent.m_bPrivate )
	{
		pRoot->Add( BT_DICT_MSG_TYPE )->SetInt( UT_METADATA_REJECT );				// "msg_type"
	}
	else
	{
		pRoot->Add( BT_DICT_MSG_TYPE )->SetInt( UT_METADATA_DATA );					// "msg_type"
		pRoot->Add( BT_DICT_TOTAL_SIZE )->SetInt( m_pDownload->m_pTorrent.GetInfoSize() );	// "total_size"
		pResponse->Write( pInfoPiece, InfoLen );
	}

	Send( pResponse );
}

BOOL CBTClient::OnMetadataRequest(CBTPacket* pPacket)
{
	// BT_EXTENSION_UT_METADATA
	ASSUME_LOCK( Transfers.m_pSection );

	const CBENode* pRoot = pPacket->m_pNode.get();

	if ( CBENode* pMsgType = pRoot->GetNode( BT_DICT_MSG_TYPE ) )					// "msg_type"
	{
		if ( CBENode* pPiece = pRoot->GetNode( BT_DICT_PIECE ) )					// "piece"
		{
			QWORD nMsgType = (QWORD)pMsgType->GetInt();
			QWORD nPiece   = (QWORD)pPiece->GetInt();

			if ( nMsgType == UT_METADATA_REQUEST )
			{
				if ( m_nUtMetadataID )
					SendMetadataRequest( nPiece );
			}
			else if ( nMsgType == UT_METADATA_DATA && ! m_pDownload->m_pTorrent.m_pBlockBTH )
			{
				if ( CBENode* pTotalSize = pRoot->GetNode( BT_DICT_TOTAL_SIZE ) )	// "total_size"
				{
					QWORD nTotalSize = (QWORD)pTotalSize->GetInt();
					ASSERT( ! ( m_nUtMetadataSize && m_nUtMetadataSize != nTotalSize ) );
					if ( ! m_nUtMetadataSize )
						m_nUtMetadataSize = nTotalSize;
				}

				if ( m_nUtMetadataSize && ! m_pDownload->m_pTorrent.m_pBlockBTH )
				{
					if ( m_pDownload->m_pTorrent.LoadInfoPiece( pPacket->m_pBuffer, pPacket->m_nLength, m_nUtMetadataSize, nPiece ) )
					{
						// Full info loaded
						m_pDownload->SetTorrent();
					}
					else if ( m_nUtMetadataID )
					{
						const int nNextPiece = m_pDownload->m_pTorrent.NextInfoPiece();
						if ( nNextPiece >= 0 )
							SendInfoRequest( nNextPiece );
					}
				}
			}
		}
	}

	return TRUE;
}


// uTorrent Peer-Exchange
// The PEX message payload is a bencoded dictionary with three keys:
//
// 'added':   The set of peers met since the last PEX
// 'added.f': A flag for every peer, apparently with following values:
//		\x00: unknown, assuming default
//		\x01: Prefers encryption
//		\x02: Is seeder
//		(OR-ing them together is allowed)
// 'dropped': The set of peers dropped since last PEX

void CBTClient::SendUtPex(DWORD tConnectedAfter)
{
	if ( m_nUtPexID == 0 || m_pDownload->m_pTorrent.m_bPrivate )
		return;

	CBuffer pAddedBuffer;
	CBuffer pAddedFlagsBuffer;
	BYTE* pnFlagsByte = NULL;
	DWORD nPeersCount = 0;

	for ( POSITION posSource = m_pDownload->GetIterator() ; posSource ; )
	{
		CDownloadSource* pSource = m_pDownload->GetNext( posSource );

		if ( ! pSource->IsConnected()
			|| pSource->GetTransfer()->m_tConnected < tConnectedAfter
			|| pSource->m_nProtocol != PROTOCOL_BT )
			continue;

		WORD nPort = htons( pSource->m_nPort );
		pAddedBuffer.Add( &pSource->m_pAddress, 4 );
		pAddedBuffer.Add( &nPort, 2 );

		nPeersCount++;

		const CDownloadTransferBT* pBTDownload =
			static_cast< const CDownloadTransferBT* >( pSource->GetTransfer() );

		BYTE nFlag = ( pBTDownload->m_pClient->m_bPrefersEncryption ? 1 : 0 ) |
					 ( pBTDownload->m_pClient->m_bSeeder ? 2 : 0 );

		DWORD nFlagInBytePos = ( nPeersCount - 1 ) % 4;

		if ( nFlagInBytePos == 0 )
		{
			DWORD nLength = nPeersCount / 4 + 1;
			pAddedFlagsBuffer.EnsureBuffer( nLength );
			pAddedFlagsBuffer.m_nLength = nLength;
			pnFlagsByte = &pAddedFlagsBuffer.m_pBuffer[nLength - 1];
			*pnFlagsByte = 0;
		}

		*pnFlagsByte |= ( nFlag & 3 ) << ( 6 - nFlagInBytePos * 2 );
	}

	if ( pAddedBuffer.m_nLength )
	{
		CBTPacket* pResponse = CBTPacket::New( BT_PACKET_EXTENSION, m_nUtPexID );
		CBENode* pRoot = pResponse->m_pNode.get();

		pRoot->Add( BT_DICT_ADDED )->SetString( pAddedBuffer.m_pBuffer, pAddedBuffer.m_nLength );	// "added"
		pRoot->Add( BT_DICT_ADDED_F )->SetString( pAddedFlagsBuffer.m_pBuffer, pAddedFlagsBuffer.m_nLength );	// "added.f"

		Send( pResponse );
	}
}

BOOL CBTClient::OnUtPex(CBTPacket* pPacket)
{
	const CBENode* pRoot = pPacket->m_pNode.get();

	if ( CBENode* pPeersAdd = pRoot->GetNode( BT_DICT_ADDED ) )
	{
		if ( 0 == ( pPeersAdd->m_nValue % 6 ) )		// IPv4?
		{
			const BYTE* pPointer = (const BYTE*)pPeersAdd->m_pValue;

			for ( int nPeer = (int)pPeersAdd->m_nValue / 6, nMax = Settings.Downloads.SourcesWanted ;
				  nPeer > 0 ; nPeer--, pPointer += 6, nMax-- )
			{
				const IN_ADDR* pAddress = (const IN_ADDR*)pPointer;
				WORD nPort = *(const WORD*)( pPointer + 4 );

				m_pDownload->AddSourceBT( Hashes::BtGuid(), pAddress, ntohs( nPort ) );
				if ( nMax < 0 && m_pDownload->GetEffectiveSourceCount() > Settings.Downloads.SourcesWanted )
					break;
			}
		}
	}

	if ( CBENode* pPeersDrop = pRoot->GetNode( BT_DICT_DROPPED ) )
	{
		if ( 0 == ( pPeersDrop->m_nValue % 6 ) )	// IPv4?
		{
			for ( POSITION posSource = m_pDownload->GetIterator() ; posSource ; )
			{
				CDownloadSource* pSource = m_pDownload->GetNext( posSource );

				if ( pSource->IsConnected() || pSource->m_nProtocol != PROTOCOL_BT )
					continue;

				WORD nPort = htons( pSource->m_nPort );
				const BYTE* pPointer = (const BYTE*)pPeersDrop->m_pValue;

				for ( int nPeer = (int)pPeersDrop->m_nValue / 6 ; nPeer > 0 ; nPeer --, pPointer += 6 )
				{
					if ( memcmp( &pSource->m_pAddress, pPointer, 4 ) == 0
						 && memcmp( &nPort, (pPointer + 4), 2 ) == 0 )
						pSource->m_tAttempt = pSource->CalcFailureDelay();
				}
			}
		}
	}

	return TRUE;
}

void CBTClient::SendLtTex()
{
	if ( m_nLtTexID == 0 )
		return;		// Unsupported

	if ( m_pDownload->m_pTorrent.m_bPrivate )
		return;		// Don't send private tracker URLs

	const int nCount = m_pDownload->m_pTorrent.GetTrackerCount();
	if ( nCount == 0 )
		return;

	CBTPacket* pResponse = CBTPacket::New( BT_PACKET_EXTENSION, m_nLtTexID );
	CBENode* pRoot = pResponse->m_pNode.get();

	CBENode* pList = pRoot->Add( BT_DICT_ADDED )->Add();
	for ( int i = 0 ; i < nCount ; i++ )
	{
		pList->Add()->SetString( m_pDownload->m_pTorrent.GetTrackerAddress( i ) );
	}

	Send( pResponse );
}

BOOL CBTClient::OnLtTex(CBTPacket* pPacket)
{
	const CBENode* pRoot = pPacket->m_pNode.get();

	if ( CBENode* pTrackerList = pRoot->GetNode( BT_DICT_ADDED ) )
	{
		const int nCount = pTrackerList->GetCount();
		for ( int i = 0 ; i < nCount ; ++i )
		{
			if ( CBENode* pTracker = pTrackerList->GetNode( i ) )
			{
				m_pDownload->m_pTorrent.AddTracker( CBTInfo::CBTTracker( pTracker->GetString() ) );
			}
		}
	}

	return TRUE;
}

void CBTClient::Choke()
{
	Send( CBTPacket::New( BT_PACKET_CHOKE ) );
}

void CBTClient::UnChoke()
{
	Send( CBTPacket::New( BT_PACKET_UNCHOKE ) );
}

void CBTClient::Interested()
{
	Send( CBTPacket::New( BT_PACKET_INTERESTED ) );
}

void CBTClient::NotInterested()
{
	Send( CBTPacket::New( BT_PACKET_NOT_INTERESTED ) );
}

void CBTClient::Request(DWORD nBlock, DWORD nOffset, DWORD nLength)
{
	CBTPacket* pPacket = CBTPacket::New( BT_PACKET_REQUEST );
	pPacket->WriteLongBE( nBlock );
	pPacket->WriteLongBE( nOffset );
	pPacket->WriteLongBE( nLength );
	Send( pPacket );
}

void CBTClient::Cancel(DWORD nBlock, DWORD nOffset, DWORD nLength)
{
	CBTPacket* pPacket = CBTPacket::New( BT_PACKET_CANCEL );
	pPacket->WriteLongBE( nBlock );
	pPacket->WriteLongBE( nOffset );
	pPacket->WriteLongBE( nLength );
	Send( pPacket );
}

void CBTClient::Have(DWORD nBlock)
{
	CBTPacket* pPacket = CBTPacket::New( BT_PACKET_HAVE );
	pPacket->WriteLongBE( nBlock );
	Send( pPacket );
}

void CBTClient::Piece(DWORD nIndex, DWORD nOffset, DWORD nLength, LPCVOID pBuffer)
{
	CBTPacket* pPacket = CBTPacket::New( BT_PACKET_PIECE );
	pPacket->WriteLongBE( nIndex );
	pPacket->WriteLongBE( nOffset );
	pPacket->Write( pBuffer, nLength );
	Send( pPacket );
}
