//
// BTPacket.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
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
#include "BENode.h"
#include "BTClient.h"
#include "BTPacket.h"
#include "Buffer.h"
#include "Datagrams.h"
#include "HostCache.h"
#include "GProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CBTPacket::CBTPacketPool CBTPacket::POOL;


//////////////////////////////////////////////////////////////////////
// CBTPacket construction

CBTPacket::CBTPacket()
	: CPacket( PROTOCOL_BT )
	, m_nType		( BT_PACKET_EXTENSION )
	, m_nExtension	( BT_EXTENSION_NOP )
	, m_pNode		( new CBENode() )
{
}

CBTPacket::~CBTPacket()
{
}

void CBTPacket::Reset()
{
	CPacket::Reset();

	m_nType			= BT_PACKET_EXTENSION;
	m_nExtension	= BT_EXTENSION_NOP;
	m_pNode.reset	( new CBENode() );
}

CBTPacket* CBTPacket::New(BYTE nType, BYTE nExtension, const BYTE* pBuffer, DWORD nLength)
{
	CBTPacket* pPacket = (CBTPacket*)POOL.New();
	ASSERT( pPacket && pPacket->m_pNode.get() );
	if ( pPacket )
	{
		pPacket->m_nType = nType;
		pPacket->m_nExtension = nExtension;
		if ( pBuffer && nLength )
		{
			DWORD nRead = 0;
			if ( IsEncoded( nType ) )
			{
				pPacket->m_pNode.reset( CBENode::Decode( pBuffer, nLength, &nRead ) );
				if ( ! pPacket->m_pNode.get() )
				{
					pPacket->Release();
					return NULL;
				}
			}
			// Read rest of packet (if any) as raw data
			if ( nLength > nRead )
			{
				if ( ! pPacket->Write( pBuffer + nRead, nLength - nRead ) )
				{
					pPacket->Release();
					return NULL;
				}
			}
		}
	}
	return pPacket;
}

bool CBTPacket::HasEncodedData() const
{
	return m_pNode.get() && ! m_pNode->IsType( CBENode::beNull );
}

//////////////////////////////////////////////////////////////////////
// CBTPacket serialize

void CBTPacket::ToBuffer(CBuffer* pBuffer, bool /*bTCP*/) const
{
	if ( m_nType == BT_PACKET_KEEPALIVE )
	{
		// Keep-Alive packet
		DWORD nZero = 0;
		pBuffer->Add( &nZero, 4 );
	}
	else if ( m_nType == BT_PACKET_EXTENSION && m_nExtension == BT_EXTENSION_NOP )
	{
		// Packet without header
		if ( HasEncodedData() )
			m_pNode->Encode( pBuffer );

		pBuffer->Add( m_pBuffer, m_nLength );
	}
	else	// Full packet
	{
		// Reserve memory for packet length field
		DWORD nZero = 0;
		pBuffer->Add( &nZero, 4 );
		DWORD nOldLength = pBuffer->m_nLength;

		// Add packet type
		pBuffer->Add( &m_nType, 1 );

		// Add packet extension
		if ( m_nType == BT_PACKET_EXTENSION )
			pBuffer->Add( &m_nExtension, 1 );

		// Add bencoded data
		if ( HasEncodedData() )
			m_pNode->Encode( pBuffer );

		// Add payload
		pBuffer->Add( m_pBuffer, m_nLength );

		// Set packet total length
		*(DWORD*)( pBuffer->m_pBuffer + nOldLength - 4 ) =
			swapEndianess( pBuffer->m_nLength - nOldLength );
	}

	ASSERT( pBuffer->m_nLength );
}

//////////////////////////////////////////////////////////////////////
// CBTPacket unserialize

CBTPacket* CBTPacket::ReadBuffer(CBuffer* pBuffer)
{
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

		if ( ! bKeepAlive && nLength == 0 )
			bKeepAlive = true;

		if ( bValid )
			pBuffer->Remove( sizeof( DWORD ) );		// Remove size marker
	}
	while ( bKeepAlive && bValid && nLength == 0 );

	CBTPacket* pPacket = NULL;
	if ( bKeepAlive )
	{
		pPacket = CBTPacket::New( BT_PACKET_KEEPALIVE );
	}
	else if ( bValid )
	{
		if ( pBuffer->m_pBuffer[ 0 ] == BT_PACKET_EXTENSION )
		{
			// Read extension packet
			pPacket = CBTPacket::New( BT_PACKET_EXTENSION,
				pBuffer->m_pBuffer[ 1 ], pBuffer->m_pBuffer + 2, nLength - 2 );
		}
		else
		{
			// Read standard packet
			pPacket = CBTPacket::New( pBuffer->m_pBuffer[ 0 ],
				BT_EXTENSION_NOP, pBuffer->m_pBuffer + 1, nLength - 1 );
		}

		pBuffer->Remove( nLength );
	}

	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CBTPacket debugging
void CBTPacket::SmartDump(const SOCKADDR_IN* pAddress, BOOL bUDP, BOOL bOutgoing, DWORD_PTR nNeighbourUnique) const
{
	switch ( m_nType )
	{
	case BT_PACKET_REQUEST:
	case BT_PACKET_PIECE:
	case BT_PACKET_HAVE:
		return; 	// Ignore uninterested packets
	}

	CPacket::SmartDump( pAddress, bUDP, bOutgoing, nNeighbourUnique );
}

CString CBTPacket::GetType() const
{
	CString sType;
	switch ( m_nType )
	{
	case BT_PACKET_CHOKE:
		sType = _T("Choke");
		break;
	case BT_PACKET_UNCHOKE:
		sType = _T("Unchoke");
		break;
	case BT_PACKET_INTERESTED:
		sType = _T("Interested");
		break;
	case BT_PACKET_NOT_INTERESTED:
		sType = _T("NotInterested");
		break;
	case BT_PACKET_HAVE:
		sType = _T("Have");
		break;
	case BT_PACKET_BITFIELD:
		sType = _T("Bitfield");
		break;
	case BT_PACKET_REQUEST:
		sType = _T("Request");
		break;
	case BT_PACKET_PIECE:
		sType = _T("Piece");
		break;
	case BT_PACKET_CANCEL:
		sType = _T("Cancel");
		break;
	case BT_PACKET_DHT_PORT:
		sType = _T("DHT port");
		break;
	case BT_PACKET_EXTENSION:
		switch( m_nExtension )
		{
		case BT_EXTENSION_HANDSHAKE:
			sType = _T("Handshake");
			break;
		case BT_EXTENSION_NOP:
			sType = _T("DHT");
			break;
	//	case BT_EXTENSION_UT_METADATA:
	//		sType = _T("UT Metadata");
	//		break;
	//	case BT_EXTENSION_UT_PEX:
	//		sType = _T("UT PEX");
	//		break;
	//	case BT_EXTENSION_LT_TEX:
	//		sType = _T("LT TEX");
	//		break;
		default:
			sType.Format( _T("Extension %d"), m_nExtension );
		}
		break;
	case BT_PACKET_HANDSHAKE:
		sType = _T("ExtHandshake");
		break;
	case BT_PACKET_SOURCE_REQUEST:
		sType = _T("SrcRequest");
		break;
	case BT_PACKET_SOURCE_RESPONSE:
		sType = _T("SrcResponse");
		break;
	case BT_PACKET_KEEPALIVE:
		sType = _T("Keep-Alive");
		break;
	default:
		sType.Format( _T("%d"), m_nType );
	}
	return sType;
}

CString CBTPacket::ToHex() const
{
	return CPacket::ToHex();
}

CString CBTPacket::ToASCII() const
{
	return HasEncodedData() ? m_pNode->Encode() : CPacket::ToASCII();
}

BOOL CBTPacket::OnPacket(const SOCKADDR_IN* pHost)
{
	ASSERT( m_nType == BT_PACKET_EXTENSION );
	ASSERT( m_nExtension == BT_EXTENSION_NOP );

	SmartDump( pHost, TRUE, FALSE );

	if ( ! m_pNode->IsType( CBENode::beDict ) )
		return FALSE;

	// Get packet type and transaction id
	CBENode* pType = m_pNode->GetNode( BT_DICT_TYPE );				// "y"
	CBENode* pTransID = m_pNode->GetNode( BT_DICT_TRANSACT_ID );	// "t"
	if ( ! pType || ! pType->IsType( CBENode::beString ) ||
		 ! pTransID || ! pTransID->IsType( CBENode::beString ) )
		 return FALSE;

	CQuickLock oLock( HostCache.BitTorrent.m_pSection );

	CHostCacheHostPtr pCache = HostCache.BitTorrent.Add(
		&pHost->sin_addr, htons( pHost->sin_port ) );
	if ( ! pCache )
		return FALSE;
	pCache->m_bDHT = TRUE;
	pCache->m_tFailure = 0;
	pCache->m_nFailures = 0;
	pCache->m_bCheckedLocally = TRUE;

	HostCache.BitTorrent.m_nCookie++;

	// Get version
	CBENode* pVersion = m_pNode->GetNode( BT_DICT_VENDOR );			// "v"
	if ( pVersion && pVersion->IsType( CBENode::beString ) )
	{
		pCache->m_sName = CBTClient::GetUserAgentAzureusStyle(
			(LPBYTE)pVersion->m_pValue, 4 );
	}

	CString sType = pType->GetString();
	if ( sType == BT_DICT_QUERY )
	{
		// Query message
		CBENode* pQueryMethod = m_pNode->GetNode( BT_DICT_QUERY );	// "q"
		if ( ! pQueryMethod || ! pQueryMethod->IsType( CBENode::beString ) )
			return FALSE;

		CString sQueryMethod = pQueryMethod->GetString();
		if ( sQueryMethod == L"ping" )
			return OnPing( pHost );
		//else if ( sQueryMethod == "find_node" )
		//	; // ToDo: Find node
		//else if ( sQueryMethod == "get_peers" )
		//	; // ToDo: Get peers
		//else if ( sQueryMethod == "announce_peer" )
		//	; // ToDo: Announce peer
		// else if ( sQueryMethod == "error" )
		//	; // ToDo: ??

		return TRUE;
	}
	else if ( sType == BT_DICT_RESPONSE )
	{
		// Response message
		CBENode* pResponse = m_pNode->GetNode( BT_DICT_RESPONSE );	// "r"
		if ( ! pResponse || ! pResponse->IsType( CBENode::beDict ) )
			 return FALSE;

		Hashes::BtGuid oNodeGUID;
		CBENode* pNodeID = pResponse->GetNode( BT_DICT_ID );		// "id"
		if ( ! pNodeID || ! pNodeID->GetString( oNodeGUID ) )
			return FALSE;

		pCache->m_oBtGUID = oNodeGUID;
		pCache->m_sDescription = oNodeGUID.toString();

		// ToDo: Check queries pool for pTransID

		// Save access token
		CBENode* pToken = pResponse->GetNode( BT_DICT_TOKEN );		// "token"
		if ( pToken && pToken->IsType( CBENode::beString ) )
		{
			pCache->m_Token.SetSize( (INT_PTR)pToken->m_nValue );
			CopyMemory( pCache->m_Token.GetData(), pToken->m_pValue, (size_t)pToken->m_nValue );
		}

		CBENode* pPeers = pResponse->GetNode( BT_DICT_VALUES );		// "values"
		if ( pPeers && pPeers->IsType( CBENode::beList) )
		{
		}

		CBENode* pNodes = pResponse->GetNode( BT_DICT_NODES );		// "nodes"
		if ( pNodes && pNodes->IsType( CBENode::beString ) )
		{
		}

		return TRUE;
	}
	else if ( sType == BT_DICT_ERROR )
	{
		// Error message
		CBENode* pError = m_pNode->GetNode( BT_DICT_ERROR );		// "e"
		if ( ! pError || ! pError->IsType( CBENode::beList ) )
			return FALSE;

		return OnError( pHost );
	}

	return FALSE;
}

BOOL CBTPacket::OnPing(const SOCKADDR_IN* pHost)
{
	CBENode* pTransID = m_pNode->GetNode( BT_DICT_TRANSACT_ID );	// "t"

	CBENode* pQueryData = m_pNode->GetNode( BT_DICT_DATA );			// "a"
	if ( ! pQueryData || ! pQueryData->IsType( CBENode::beDict ) )
		return FALSE;

	Hashes::BtGuid oNodeGUID;
	CBENode* pNodeID = pQueryData->GetNode( BT_DICT_ID );			// "id"
	if ( ! pNodeID || ! pNodeID->GetString( oNodeGUID ) )
		return FALSE;

	{
		CQuickLock oLock( HostCache.BitTorrent.m_pSection );

		if ( CHostCacheHostPtr pCache = HostCache.BitTorrent.Add( &pHost->sin_addr, htons( pHost->sin_port ) ) )
		{
			pCache->m_oBtGUID = oNodeGUID;
			pCache->m_sDescription = oNodeGUID.toString();

			HostCache.BitTorrent.m_nCookie++;
		}
	}

	// Send pong

	CBTPacket* pPacket = CBTPacket::New();
	CBENode* pRoot = pPacket->m_pNode.get();
	ASSERT( pRoot );

	pRoot->Add( BT_DICT_RESPONSE )->Add( BT_DICT_ID )->SetString( MyProfile.oGUIDBT );		// "r" "id"
	pRoot->Add( BT_DICT_TYPE )->SetString( BT_DICT_RESPONSE );								// "y" "r"
	pRoot->Add( BT_DICT_TRANSACT_ID )->SetString( (LPCSTR)pTransID->m_pValue, (size_t)pTransID->m_nValue ); 	// "t"
	pRoot->Add( BT_DICT_VENDOR )->SetString( theApp.m_pBTVersion, 4 );						// "v"

	return Datagrams.Send( pHost, pPacket );
}

BOOL CBTPacket::OnError(const SOCKADDR_IN* /*pHost*/)
{
	return TRUE;
}

//BOOL CBTPacket::Ping(const SOCKADDR_IN* pHost)
//{
//	CBENode pPing;
//	CBENode* pPingData = pPing.Add( BT_DICT_DATA );					// "a"
//	pPingData->Add( BT_DICT_ID )->SetString( MyProfile.oGUIDBT );
//	pPing.Add( BT_DICT_TYPE )->SetString( BT_DICT_QUERY );			// "q"
//	pPing.Add( BT_DICT_TRANSACT_ID )->SetString( "1234" );			// ToDo:
//	pPing.Add( BT_DICT_QUERY )->SetString( "ping" );
//	pPing.Add( BT_DICT_VENDOR )->SetString( theApp.m_pBTVersion, 4 );
//	CBuffer pOutput;
//	pPing.Encode( &pOutput );
//	return Datagrams.Send( pHost, pOutput );
//}

//BOOL CBTPacket::GetPeers(const SOCKADDR_IN* pHost, const Hashes::BtGuid& oNodeGUID, const Hashes::BtHash& oGUID)
//{
//	CBENode pGetPeers;
//	CBENode* pGetPeersData = pGetPeers.Add( BT_DICT_DATA );			// "a"
//	pGetPeersData->Add( BT_DICT_ID )->SetString( oNodeGUID );
//	pGetPeersData->Add( "info_hash" )->SetString( oGUID );
//	pGetPeers.Add( BT_DICT_TYPE )->SetString( BT_DICT_QUERY );		// "q"
//	pGetPeers.Add( BT_DICT_TRANSACT_ID )->SetString( "4567" );		// ToDo:
//	pGetPeers.Add( BT_DICT_QUERY )->SetString( "get_peers" );
//	pGetPeers.Add( BT_DICT_VENDOR )->SetString( theApp.m_pBTVersion, 4 );
//	CBuffer pOutput;
//	pGetPeers.Encode( &pOutput );
//	return Datagrams.Send( pHost, pOutput );
//}
