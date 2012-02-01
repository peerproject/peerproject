//
// DCNeighbour.cpp
//
// This file is part of PeerProject (peerproject.org) © 2010-2012
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
#include "Settings.h"
#include "PeerProject.h"
#include "DCNeighbour.h"
#include "DCPacket.h"
#include "DCClient.h"
#include "DCClients.h"
#include "HostCache.h"
#include "GProfile.h"
#include "LibraryMaps.h"
#include "LocalSearch.h"
#include "Network.h"
#include "Neighbours.h"
#include "Security.h"
#include "UploadQueue.h"
#include "UploadQueues.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CDCNeighbour::CDCNeighbour()
	: CNeighbour	( PROTOCOL_DC )
	, m_bExtended	( FALSE )
{
	m_nNodeType = ntHub;
}

CDCNeighbour::~CDCNeighbour()
{
}

BOOL CDCNeighbour::ConnectToMe(const CString& sNick)
{
	// $ConnectToMe RemoteNick SenderIp:SenderPort|

	if ( m_nState != nrsConnected )
		return FALSE;	// Too early

	ASSERT( ! sNick.IsEmpty() );

	CString strRequest;
	strRequest.Format( _T("$ConnectToMe %s %hs:%u|"),
		sNick,
		inet_ntoa( Network.m_pHost.sin_addr ),
		htons( Network.m_pHost.sin_port ) );

	if ( CDCPacket* pPacket = CDCPacket::New() )
	{
		pPacket->WriteString( strRequest, FALSE );

		Send( pPacket );
	}

	return TRUE;
}

BOOL CDCNeighbour::ConnectTo(const IN_ADDR* pAddress, WORD nPort, BOOL bAutomatic)
{
	CString sHost( inet_ntoa( *pAddress ) );

	if ( CConnection::ConnectTo( pAddress, nPort ) )
	{
		WSAEventSelect( m_hSocket, Network.GetWakeupEvent(),
			FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE );

		theApp.Message( MSG_INFO, IDS_CONNECTION_ATTEMPTING,
			(LPCTSTR)sHost, htons( m_pHost.sin_port ) );
	}
	else
	{
		theApp.Message( MSG_ERROR, IDS_CONNECTION_CONNECT_FAIL, (LPCTSTR)sHost );
		return FALSE;
	}

	m_nState = nrsConnecting;

	m_bAutomatic = bAutomatic;

	Neighbours.Add( this );

	return TRUE;
}

BOOL CDCNeighbour::OnRead()
{
	if ( ! CNeighbour::OnRead() )
		return FALSE;

	std::string strLine;
	while ( ReadCommand( strLine ) )
	{
		m_nInputCount++;

		m_tLastPacket = GetTickCount();

		if ( CDCPacket* pPacket = CDCPacket::New( (const BYTE*)strLine.c_str(), (DWORD)strLine.size() ) )
		{
			pPacket->SmartDump( &m_pHost, FALSE, FALSE, (DWORD_PTR)this );
			pPacket->Release();
		}

		std::string strCommand, strParams;
		std::string::size_type nPos = strLine.find( ' ' );
		if ( nPos != std::string::npos )
		{
			strCommand = strLine.substr( 0, nPos );
			strParams = strLine.substr( nPos + 1, strLine.size() - nPos - 2 );
		}
		else
			strCommand = strLine.substr( 0, strLine.size() - 1 );

		if ( ! OnCommand( strCommand, strParams ) )
			return FALSE;
	}

	return TRUE;
}

BOOL CDCNeighbour::ReadCommand(std::string& strLine)
{
	strLine.clear();

	CLockedBuffer pInput( GetInput() );

	if ( ! pInput->m_nLength )
		return FALSE;

	DWORD nLength = 0;
	for ( ; nLength < pInput->m_nLength ; nLength++ )
	{
		if ( pInput->m_pBuffer[ nLength ] == '|' )
			break;
	}

	if ( nLength >= pInput->m_nLength )
		return FALSE;

	strLine.append( (const char*)pInput->m_pBuffer, nLength + 1 );

	pInput->Remove( nLength + 1 );

	return TRUE;
}

BOOL CDCNeighbour::Send(CPacket* pPacket, BOOL bRelease, BOOL /*bBuffered*/)
{
	pPacket->SmartDump( &m_pHost, FALSE, TRUE, (DWORD_PTR)this );

	Write( pPacket );

	if ( bRelease )
		pPacket->Release();

	m_nOutputCount++;

	return TRUE;
}

BOOL CDCNeighbour::OnConnected()
{
	if ( ! CNeighbour::OnConnected() )
		return FALSE;

	theApp.Message( MSG_INFO, IDS_CONNECTION_CONNECTED, (LPCTSTR)m_sAddress );

	m_nState = nrsHandshake1;	// Waiting for $Lock

	return TRUE;
}

void CDCNeighbour::OnDropped()
{
	if ( m_nState == nrsConnecting )
		Close( IDS_CONNECTION_REFUSED );
	else
		Close( IDS_CONNECTION_DROPPED );
}

BOOL CDCNeighbour::OnCommand(const std::string& strCommand, const std::string& strParams)
{
	if ( strCommand.size() == 0 )
		return TRUE;	// Ping, received only one char "|"

	// Custom SwitchMap( Command ) for raw non-unicode
	static std::map < std::string, char > Command;
	if ( Command.empty() )
	{
		Command[ "$Search" ]		= 's';
		Command[ "$MyINFO" ]		= 'i';
		Command[ "$Quit" ] 			= 'q';
		Command[ "$Lock" ] 			= 'l';
		Command[ "$Supports" ]		= 'r';
		Command[ "$Hello" ]			= 'h';
		Command[ "$HubName" ]		= 'b';
		Command[ "$HubTopic" ] 		= 't';
		Command[ "$OpList" ]		= 'o';
	//	Command[ "$BotList" ]		= 'n';
		Command[ "$NickList" ] 		= 'n';
		Command[ "$ForceMove" ]		= 'm';
		Command[ "$ConnectToMe" ]	= 'c';
		Command[ "$ValidateDenide" ] = 'v';	// Not "Denied"
		Command[ "$GetPass" ]		= 'v';
		Command[ "$UserIP" ]		= 'u';

	//	Command[ "$HubIsFull" ]		= 'x';
	//	Command[ "$MCTo" ]			= 'x';
	//	Command[ "$MyPass" ]		= 'x';
	//	Command[ "$BadPass" ]		= 'x';
	//	Command[ "$RevConnectToMe" ] = 'x';
	//	Command[ "$Version" ]		= 'x';
	}

	switch( Command[ strCommand ] )
	{
	case 's':		// $Search SenderIP:SenderPort (F|T)?(F|T)?Size?Type?String|
		{
			std::string::size_type nPos = strParams.find( ' ' );
			if ( nPos != std::string::npos )
			{
				std::string strAddress = strParams.substr( 0, nPos );
				std::string strSearch  = strParams.substr( nPos + 1 );
				nPos = strAddress.find( ':' );
				if ( nPos != std::string::npos )
				{
					const DWORD nAddress = inet_addr( strAddress.substr( 0, nPos ).c_str() );
					const int nPort = atoi( strAddress.substr( nPos + 1 ).c_str() );
					if ( nPort > 0 && nPort <= USHRT_MAX && nAddress != INADDR_NONE &&
						! Network.IsFirewalledAddress( (const IN_ADDR*)&nAddress ) &&
						! Network.IsReserved( (const IN_ADDR*)&nAddress ) &&
						! Security.IsDenied( (const IN_ADDR*)&nAddress ) )
					{
						OnSearch( (const IN_ADDR*)&nAddress, (WORD)nPort, strSearch );
					}
				}
			}
		}
		return TRUE;
	case 'i':		// $MyINFO $ALL nick description<tag>$ $connection$e-mail$sharesize$|
		// Handle user info?
		m_nState = nrsConnected;
		return TRUE;
	case 'q':		// $Quit nick|
		// Handle user leave hub?
		return TRUE;
	case 'h':		// $Hello Nick
		// User logged-in
		m_nState = nrsConnected;
		m_sNick = CA2CT( strParams.c_str() );
		if ( CHostCacheHostPtr pServer = HostCache.DC.Find( &m_pHost.sin_addr ) )
			pServer->m_sUser = m_sNick;
		return OnHello();
	case 'b':		// $HubName Title [Version]|
		m_nState = nrsConnected;
		m_sServerName = CA2CT( strParams.c_str() );
		if ( CHostCacheHostPtr pServer = HostCache.DC.Find( &m_pHost.sin_addr ) )
			pServer->m_sName = m_sServerName;
		return TRUE;
	case 't':		// $HubTopic <topic>|
		theApp.Message( MSG_NOTICE, _T("DC++ hub topic at %s:  %s"), m_sServerName, (LPCTSTR)CA2CT( strParams.c_str() ) );
		return TRUE;
	case 'o':		// $OpList operator1$$operator2|
		// Handle hub operators list, etc.?
		m_nState = nrsConnected;
		return TRUE;
	case 'n':		// $NickList user1$$user2|
		// Handle hub lists?  (ToDo: Could get user count, not sent currently)
		DEBUG_ONLY( theApp.Message( MSG_DEBUG, _T("DC++ $NickList recieved, %i char long from %s"), strParams.length(), m_sServerName ) );
		return TRUE;
	case 'l':		// $Lock [EXTENDEDPROTOCOL]Challenge Pk=Vendor
		m_bExtended = ( strParams.substr( 0, 16 ) == "EXTENDEDPROTOCOL" );
		{
			std::string strLock;
			std::string::size_type nPos = strParams.find( " Pk=" );
			if ( nPos != std::string::npos )
			{
				// Good way
				strLock = strParams.substr( 0, nPos );
				m_sUserAgent = strParams.substr( nPos + 4 ).c_str();
			}
			else
			{
				// Bad way
				nPos = strParams.find( ' ' );
				if ( nPos != std::string::npos )
					strLock = strParams.substr( 0, nPos );
				else	// Very bad way
					strLock = strParams;
			}
			return OnLock( strLock );
		}
	case 'r':		// $Supports [option1]...[optionN]
		m_bExtended = TRUE;
		m_oFeatures.RemoveAll();
		for ( CString strFeatures( strParams.c_str() ) ; ! strFeatures.IsEmpty() ; )
		{
			CString strFeature = strFeatures.SpanExcluding( _T(" ") );
			strFeatures = strFeatures.Mid( strFeature.GetLength() + 1 ).MakeLower();
			if ( strFeature.IsEmpty() )
				continue;
			if ( m_oFeatures.Find( strFeature ) == NULL )
				m_oFeatures.AddTail( strFeature );
		}
		return TRUE;
	case 'c':		// $ConnectToMe SenderNick RemoteNick SenderIp:SenderPort|
		// Client connection request
		{
			std::string::size_type nPos = strParams.rfind( ' ' );
			if ( nPos != std::string::npos )
			{
				std::string strAddress = strParams.substr( nPos + 1 );
				std::string strSenderNick, strRemoteNick = strParams.substr( 0, nPos );
				nPos = strRemoteNick.find( ' ' );
				if ( nPos != std::string::npos )
				{
					strSenderNick = strRemoteNick.substr( nPos + 1 );
					strRemoteNick = strRemoteNick.substr( 0, nPos );
				}
				nPos = strAddress.find( ':' );
				if ( nPos != std::string::npos )
				{
					const DWORD nAddress = inet_addr( strAddress.substr( 0, nPos ).c_str() );
					const int nPort = atoi( strAddress.substr( nPos + 1 ).c_str() );
					if ( nPort > 0 && nPort <= USHRT_MAX && nAddress != INADDR_NONE &&
						m_sNick == strRemoteNick.c_str() &&
						! Network.IsFirewalledAddress( (const IN_ADDR*)&nAddress ) &&
						! Network.IsReserved( (const IN_ADDR*)&nAddress ) )
					{
						// Ok
						if ( CDCClient* pClient = new CDCClient( m_sNick ) )
						pClient->ConnectTo( (const IN_ADDR*)&nAddress, (WORD)nPort );
					}
				//	else
				//		// Wrong nick, bad IP
				}
			}
		}
		return TRUE;
	case 'm':		// $ForceMove IP:Port|
		// User redirection
		{
			CString strAddress;
			int nPort = 0;
			std::string::size_type nPos = strParams.rfind( ':' );
			if ( nPos != std::string::npos )
			{
				strAddress = strParams.substr( 0, nPos ).c_str();
				nPort = atoi( strParams.substr( nPos + 1 ).c_str() );
			}
			else
				strAddress = strParams.c_str();

			Network.ConnectTo( strAddress, nPort, PROTOCOL_DC );
		}
		return TRUE;
	case 'v':		// $ValidateDenide Nick|	OR	$GetPass
		// Bad user nick
		m_sNick.Format( CLIENT_NAME _T("%04u"), GetRandomNum( 0u, 9999u ) );
		if ( CHostCacheHostPtr pServer = HostCache.DC.Find( &m_pHost.sin_addr ) )
			pServer->m_sUser = m_sNick;
		if ( CDCPacket* pPacket = CDCPacket::New() )
		{
			pPacket->WriteString( _T("$ValidateNick ") + m_sNick + _T("|"), FALSE );
			Send( pPacket );
		}
		return TRUE;
	case 'u':		// $UserIP MyNick IP|
		// User address
		{
			std::string::size_type nPos = strParams.find( ' ' );
			if ( nPos != std::string::npos )
			{
				std::string strNick = strParams.substr( 0, nPos );
				if ( m_sNick == strNick.c_str() )
					Network.AcquireLocalAddress( CA2CT( strParams.substr( nPos + 1 ).c_str() ) );
			}
		}
		return TRUE;
	default:
		if ( strCommand[ 0 ] != '$' )	// <Nick> Message|
			return OnChat( strCommand + strParams );
		// Unknown command
	}

	// Unknown command - ignoring
	return TRUE;
}

BOOL CDCNeighbour::OnSearch(const IN_ADDR* pAddress, WORD nPort, std::string& strSearch)
{
	std::string::size_type nPos = strSearch.find( '?' );
	if ( nPos == std::string::npos )
		return TRUE;

	std::string strSizeRestriced = strSearch.substr( 0, nPos );
	bool bSizeRestriced = ( strSizeRestriced == "T" );
	strSearch = strSearch.substr( nPos + 1 );
	nPos = strSearch.find( '?' );
	if ( nPos == std::string::npos || ( ! bSizeRestriced && strSizeRestriced != "F" ) )
		return TRUE;

	std::string strIsMaxSize = strSearch.substr( 0, nPos );
	bool bIsMaxSize = ( strIsMaxSize == "T" );
	strSearch = strSearch.substr( nPos + 1 );
	nPos = strSearch.find( '?' );
	if ( nPos == std::string::npos || ( ! bIsMaxSize && strIsMaxSize != "F" ) )
		return TRUE;

	QWORD nSize = 0;
	std::string strSize = strSearch.substr( 0, nPos );
	strSearch = strSearch.substr( nPos + 1 );
	nPos = strSearch.find( '?' );
	if ( nPos == std::string::npos ||
		sscanf_s( strSize.c_str(), "%I64u", &nSize ) != 1 )
		return TRUE;

	std::string strType = strSearch.substr( 0, nPos );
	int nType = atoi( strType.c_str() );
	strSearch = strSearch.substr( nPos + 1 );
	if ( nType < 1 || nType > 9 )
		return TRUE;	// Unknown search type

	CQuerySearchPtr pSearch = new CQuerySearch( TRUE );

	pSearch->m_pEndpoint = m_pHost;

	if ( bSizeRestriced )
	{
		if ( bIsMaxSize )
			pSearch->m_nMaxSize = nSize;
		else
			pSearch->m_nMinSize = nSize;
	}
	else if ( nSize )
	{
		pSearch->m_nMinSize = pSearch->m_nMaxSize = nSize;
	}

	if ( nType == 9 )	// Hash search
	{
		if ( strSearch.substr( 0, 4 ) != "TTH:" )
			return TRUE;	// Unknown hash prefix

		if ( ! pSearch->m_oTiger.fromString( CA2W( strSearch.substr( 4 ).c_str() ) ) )
			return TRUE;	// Invalid TigerTree hash encoding
	}
	else	// Keywords search
	{
		pSearch->m_sSearch = strSearch.c_str();
		pSearch->m_sSearch.Replace( _T('$'), _T(' ') );
	}

	if ( ! pSearch->CheckValid( true ) )
		return TRUE;	// Invalid search

	CLocalSearch* pLocalSearch = new CLocalSearch( pSearch, this );

	// Send result (if any) directly to client via UDP
	pLocalSearch->m_pEndpoint.sin_addr = *pAddress;
	pLocalSearch->m_pEndpoint.sin_port = htons( nPort );
	pLocalSearch->m_bUDP = TRUE;

	Network.OnQuerySearch( pLocalSearch );

	return TRUE;
}

BOOL CDCNeighbour::OnLock(const std::string& strLock)
{
	if ( m_nState < nrsHandshake2 )
		m_nState = nrsHandshake2;	// Waiting for $Hello

	if ( m_nNodeType == ntHub )
		HostCache.DC.Add( &m_pHost.sin_addr, htons( m_pHost.sin_port ) );

	if ( m_bExtended )
	{
		if ( CDCPacket* pPacket = CDCPacket::New() )
		{
			pPacket->WriteString( _T("$Supports NoHello NoGetINFO UserIP2 TTHSearch |"), FALSE );
			Send( pPacket );
		}
	}

	std::string strKey = DCClients.MakeKey( strLock );
	if ( CDCPacket* pPacket = CDCPacket::New() )
	{
		pPacket->Write( _P("$Key ") );
		pPacket->Write( strKey.c_str(), (DWORD)strKey.size() );
		pPacket->Write( _P("|") );
		Send( pPacket );
	}

	if ( CHostCacheHostPtr pServer = HostCache.DC.Find( &m_pHost.sin_addr ) )
		m_sNick = pServer->m_sUser;

	if ( m_sNick.IsEmpty() )
		m_sNick = DCClients.GetDefaultNick();

	if ( CDCPacket* pPacket = CDCPacket::New() )
	{
		pPacket->WriteString( _T("$ValidateNick ")  + m_sNick + _T("|"), FALSE );
		Send( pPacket );
	}

	return TRUE;
}

BOOL CDCNeighbour::OnChat(const std::string& strMessage)
{
	theApp.Message( MSG_DEBUG | MSG_FACILITY_INCOMING, _T("%s >> %s"), (LPCTSTR)m_sAddress, (LPCTSTR)CA2CT( strMessage.c_str() ) );

	return TRUE;
}

BOOL CDCNeighbour::OnHello()
{
	// NMDC version
	if ( CDCPacket* pPacket = CDCPacket::New() )
	{
		pPacket->Write( _P("$Version 1,0091|") );
		Send( pPacket );
	}

	// Request nick list
	if ( CDCPacket* pPacket = CDCPacket::New() )
	{
		pPacket->Write( _P("$GetNickList|") );
		Send( pPacket );
	}

	// $MyINFO $ALL nick description<tag>$ $connection$e-mail$sharesize$|

	CUploadQueue* pQueue = UploadQueues.SelectQueue(
		PROTOCOL_DC, NULL, 0, CUploadQueue::ulqBoth, NULL );

	QWORD nMyVolume = 0;
	LibraryMaps.GetStatistics( NULL, &nMyVolume );

	CString sInfo;
	sInfo.Format( _T("$MyINFO $ALL %s %s<%s V:%s,M:%c,H:%u/%u/%u,S:%u>$ $%.2f%c$%s$%I64u$|"),
		m_sNick,													// Registered nick
		WEB_SITE,													// Description
		CLIENT_NAME,												// Client/vendor name
		theApp.m_sVersion,											// Client version
		( Network.IsFirewalled( CHECK_BOTH ) ? _T('P') : _T('A') ),	// User is in active(A), passive(P), or SOCKS5(5) mode
		Neighbours.GetCount( PROTOCOL_DC, nrsConnected, ntHub ),	// Number of connected hubs as regular user
		0,															// Number of connected hubs as VIP
		0,															// Number of connected hubs as operator
		( pQueue ? pQueue->m_nMaxTransfers : 0 ),					// Number of upload slots
		(float)Settings.Bandwidth.Uploads * Bytes / ( Kilobits * Kilobits ),	// Upload speed (Mbit/s)
		1,															// User status: Normal(1), Away(2,3), Server(4,5), Server Away(6,7)
		MyProfile.GetContact( _T("Email") ),						// E-mail
		nMyVolume << 10 );											// Share size (bytes)

	if ( CDCPacket* pPacket = CDCPacket::New() )
	{
		pPacket->WriteString( sInfo, FALSE );
		Send( pPacket );
	}

	return TRUE;
}
