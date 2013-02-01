//
// DownloadSource.cpp
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
#include "DownloadSource.h"
#include "Downloads.h"
#include "Download.h"
#include "DownloadTransferBT.h"
#include "DownloadTransferDC.h"
#include "DownloadTransferED2K.h"
#include "DownloadTransferFTP.h"
#include "DownloadTransferHTTP.h"
#include "DCClient.h"
#include "DCClients.h"
#include "EDClient.h"
#include "EDClients.h"
#include "EDPacket.h"
#include "FragmentBar.h"
#include "FragmentedFile.h"
#include "Neighbours.h"
#include "Network.h"
#include "QueryHit.h"
#include "Transfers.h"
#include "PeerProjectURL.h"
#include "CoolInterface.h"
#include "Colors.h"
#include "Images.h"
#include "VendorCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CDownloadSource construction

CDownloadSource::CDownloadSource(const CDownload* pDownload)
	: m_oAvailable		( pDownload->m_nSize )
	, m_oPastFragments	( pDownload->m_nSize )
{
	Construct( pDownload );
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource construction from a query hit

CDownloadSource::CDownloadSource(const CDownload* pDownload, const CQueryHit* pHit)
	: m_oAvailable		( pDownload->m_nSize )
	, m_oPastFragments	( pDownload->m_nSize )
{
	Construct( pDownload );

	m_bPushOnly	= pHit->m_bPush == TRI_TRUE ? TRUE : FALSE;

	m_sURL		= pHit->m_sURL;
	m_pAddress	= pHit->m_pAddress;	// Not needed? m_pAddress is set in ResolveURL() again
	m_nPort		= pHit->m_nPort;	// Not needed?
	m_nSpeed	= pHit->m_bMeasured == TRI_TRUE ? ( pHit->m_nSpeed * 128 ) : 0;
	m_sServer	= pHit->m_pVendor->m_sName;
	m_sName		= pHit->m_sName;
	m_sNick		= pHit->m_sNick;
	m_nIndex	= pHit->m_nIndex;
	m_bSHA1		= bool( pHit->m_oSHA1 );
	m_bTiger	= bool( pHit->m_oTiger );
	m_bED2K		= bool( pHit->m_oED2K );
	m_bBTH		= bool( pHit->m_oBTH );
	m_bMD5		= bool( pHit->m_oMD5 );

	if ( pHit->m_nProtocol == PROTOCOL_G1 || pHit->m_nProtocol == PROTOCOL_G2 )
	{
		m_oGUID = pHit->m_oClientID;
		m_bClientExtended = TRUE;
		if ( pHit->m_nProtocol == PROTOCOL_G2 )
		{
			m_bPreview = pHit->m_bPreview;
			m_sPreview = pHit->m_sPreview;
		}
		else
			m_bPreview = FALSE;
	}
	else if ( pHit->m_nProtocol == PROTOCOL_ED2K )
	{
		// Add the size if it was missing.
		if ( ( m_sURL.Right( 3 ) == _T("/0/") ) && ( pDownload->m_nSize ) )
		{
			CString strTemp = m_sURL.Left( m_sURL.GetLength() - 2 );
			m_sURL.Format( _T("%s%I64u/"), (LPCTSTR)strTemp, pDownload->m_nSize );
		}
	}
//	else if ( pHit->m_nProtocol == PROTOCOL_DC )	// Obsolete
//	{
//		// Generate source GUID form source nick
//		CMD5 pMD5;
//		pMD5.Add( (LPCTSTR)m_sNick, m_sNick.GetLength() * sizeof( TCHAR ) );
//		pMD5.Finish();
//		pMD5.GetHash( &m_oGUID[ 0 ] );
//		m_oGUID.validate();
//	}

	ResolveURL();

	// If we got hit with BitTorrent hash
	// and url now looks like btc://
	// but hit was received from G1/G2 search
	// and download isn't a multifile torrent
	// then change hit (back) to G1/G2 protocol

	if ( pHit->m_oBTH &&
		m_nProtocol == PROTOCOL_BT &&
		! pDownload->IsMultiFileTorrent() &&
		( pHit->m_nProtocol == PROTOCOL_G2 || pHit->m_nProtocol == PROTOCOL_G1 ) )
	{
		m_nProtocol = pHit->m_nProtocol;
		m_sURL = pHit->GetURL( m_pAddress, m_nPort );
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource construction from eDonkey source transfer

CDownloadSource::CDownloadSource(const CDownload* pDownload, DWORD nClientID,
	WORD nClientPort, DWORD nServerIP, WORD nServerPort, const Hashes::Guid& oGUID)
	: m_oAvailable		( pDownload->m_nSize )
	, m_oPastFragments	( pDownload->m_nSize )
{
	Construct( pDownload );

	if ( ( m_bPushOnly = CEDPacket::IsLowID( nClientID ) ) != FALSE )
	{
		m_sURL.Format( _T("ed2kftp://%lu@%s:%hu/%s/%I64u/"),
			nClientID,
			(LPCTSTR)CString( inet_ntoa( (IN_ADDR&)nServerIP ) ), nServerPort,
			(LPCTSTR)m_pDownload->m_oED2K.toString(), m_pDownload->m_nSize );
	}
	else
	{
		m_sURL.Format( _T("ed2kftp://%s:%hu/%s/%I64u/"),
			(LPCTSTR)CString( inet_ntoa( (IN_ADDR&)nClientID ) ), nClientPort,
			(LPCTSTR)m_pDownload->m_oED2K.toString(), m_pDownload->m_nSize );
	}

	m_bED2K		= TRUE;
	m_oGUID		= oGUID;
	m_sServer	= _T("eDonkey2000");	// protocolNames[ PROTOCOL_ED2K ]

	ResolveURL();
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource construction from BitTorrent

CDownloadSource::CDownloadSource(const CDownload* pDownload,
	const Hashes::BtGuid& oGUID, const IN_ADDR* pAddress, WORD nPort)
	: m_oAvailable		( pDownload->m_nSize )
	, m_oPastFragments	( pDownload->m_nSize )
{
	Construct( pDownload );

	if ( oGUID )
	{
		m_sURL.Format( _T("btc://%s:%i/%s/%s/"),
			(LPCTSTR)CString( inet_ntoa( *pAddress ) ), nPort,
			(LPCTSTR)oGUID.toString(),
			(LPCTSTR)pDownload->m_oBTH.toString() );
	}
	else
	{
		m_sURL.Format( _T("btc://%s:%i//%s/"),
			(LPCTSTR)CString( inet_ntoa( *pAddress ) ), nPort,
			(LPCTSTR)pDownload->m_oBTH.toString() );
	}

	m_bBTH		= TRUE;
	m_oGUID		= transformGuid( oGUID );
	m_sServer	= _T("BitTorrent");

	ResolveURL();
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource construction from URL

CDownloadSource::CDownloadSource(const CDownload* pDownload, LPCTSTR pszURL,
	BOOL /*bSHA1*/, BOOL bHashAuth, FILETIME* pLastSeen, int nRedirectionCount)
	: m_oAvailable		( pDownload->m_nSize )
	, m_oPastFragments	( pDownload->m_nSize )
{
	Construct( pDownload );

	ASSERT( pszURL != NULL );

	m_sURL			= pszURL;
	m_bHashAuth		= bHashAuth;

	if ( pLastSeen )
	{
		FILETIME tNow = m_tLastSeen;
		(LONGLONG&)tNow += 10000000;
		if ( CompareFileTime( pLastSeen, &tNow ) <= 0 )
			m_tLastSeen = *pLastSeen;
	}

	m_nRedirectionCount = nRedirectionCount;

	ResolveURL();
}

void CDownloadSource::Construct(const CDownload* pDownload)
{
	ASSUME_LOCK( Transfers.m_pSection );
	ASSERT( pDownload != NULL );

	m_pTransfer				= NULL;
	m_pDownload				= const_cast< CDownload* >( pDownload );
	m_bSelected				= FALSE;
	m_nProtocol				= PROTOCOL_NULL;
	m_pAddress.s_addr		= 0;
	m_nPort					= 0;
	m_pServerAddress.s_addr	= 0;
	m_nServerPort			= 0;
	m_nIndex				= 0;
	m_bHashAuth				= FALSE;
	m_bSHA1					= FALSE;
	m_bTiger				= FALSE;
	m_bED2K					= FALSE;
	m_bBTH					= FALSE;
	m_bMD5					= FALSE;
	m_nSpeed				= 0;
	m_bPushOnly				= FALSE;
	m_bCloseConn			= FALSE;
	m_bReadContent			= FALSE;
	GetSystemTimeAsFileTime( &m_tLastSeen );
	m_bClientExtended		= FALSE;
	m_nGnutella				= 0;
	m_nSortOrder			= 0xFFFFFFFF;
	m_nColor				= -1;
	m_tAttempt				= 0;
	m_nFailures				= 0;
	m_nBusyCount			= 0;
	m_nRedirectionCount		= 0;
	m_bKeep					= FALSE;
	m_bPreview				= FALSE;
	m_bPreviewRequestSent	= FALSE;
	m_bMetaIgnore			= FALSE;
}

CDownloadSource::~CDownloadSource()
{
	ASSUME_LOCK( Transfers.m_pSection );
	ASSERT( m_pTransfer == NULL );
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource URL resolver

BOOL CDownloadSource::ResolveURL()
{
	CPeerProjectURL pURL;

	if ( ! pURL.Parse( m_sURL ) )
	{
		theApp.Message( MSG_ERROR, _T("Unable to parse URL: %s"), (LPCTSTR)m_sURL );
		return FALSE;
	}

	m_bSHA1		|= static_cast< BOOL >( bool( pURL.m_oSHA1 ) );
	m_bTiger	|= static_cast< BOOL >( bool( pURL.m_oTiger ) );
	m_bED2K		|= static_cast< BOOL >( bool( pURL.m_oED2K ) );
	m_bBTH		|= static_cast< BOOL >( bool( pURL.m_oBTH ) );
	m_bMD5		|= static_cast< BOOL >( bool( pURL.m_oMD5 ) );

	m_nProtocol	= pURL.m_nProtocol;
	m_pAddress	= pURL.m_pAddress;
	m_nPort		= pURL.m_nPort;
	m_sName		= pURL.m_sName;

	switch ( m_nProtocol )
	{
	case PROTOCOL_ED2K:
		m_pServerAddress	= pURL.m_pServerAddress;
		m_nServerPort		= pURL.m_nServerPort;
		if ( m_nServerPort )
			m_bPushOnly		= TRUE;
		break;

	case PROTOCOL_DC:
		m_pServerAddress	= pURL.m_pServerAddress;
		m_nServerPort		= pURL.m_nServerPort;
		m_sNick				= pURL.m_sLogin;
		m_bPushOnly			= TRUE;
		CDCClients::CreateGUID( m_sNick, m_oGUID );
		break;

	case PROTOCOL_BT:
		if ( pURL.m_oBTC )
			m_oGUID = transformGuid( pURL.m_oBTC );
		break;

	//default:
	//	;
	}

	m_sCountry		= theApp.GetCountryCode( m_pAddress );
	m_sCountryName	= theApp.GetCountryName( m_pAddress );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource serialize

void CDownloadSource::Serialize(CArchive& ar, int nVersion)	// DOWNLOAD_SER_VERSION
{
	if ( ar.IsStoring() )
	{
		ar << m_sURL;
		ar << m_nProtocol;

		SerializeOut( ar, m_oGUID );

		ar << m_nPort;
		if ( m_nPort ) ar.Write( &m_pAddress, sizeof( m_pAddress ) );
		ar << m_nServerPort;
		if ( m_nServerPort ) ar.Write( &m_pServerAddress, sizeof( m_pServerAddress ) );

		ar << m_sName;
		ar << m_nIndex;
		ar << m_bHashAuth;
		ar << m_bSHA1;
		ar << m_bTiger;
		ar << m_bED2K;
		ar << m_bBTH;
		ar << m_bMD5;

		ar << m_sServer;
		ar << m_sNick;
		ar << m_sCountry;
		ar << m_sCountryName;
		ar << m_nSpeed;
		ar << m_bPushOnly;
		ar << m_bCloseConn;
		ar << m_bReadContent;
		ar.Write( &m_tLastSeen, sizeof( FILETIME ) );

		SerializeOut2( ar, m_oPastFragments );

		ar << m_bClientExtended;
		ar << m_bMetaIgnore;
	}
	else //if ( nVersion > 20 ) // Loading
	{
		ar >> m_sURL;
		ar >> m_nProtocol;

		SerializeIn( ar, m_oGUID, nVersion);

		ar >> m_nPort;
		if ( m_nPort ) ReadArchive( ar, &m_pAddress, sizeof( m_pAddress ) );
		ar >> m_nServerPort;
		if ( m_nServerPort ) ReadArchive( ar, &m_pServerAddress, sizeof( m_pServerAddress ) );

		ar >> m_sName;
		ar >> m_nIndex;
		ar >> m_bHashAuth;
		ar >> m_bSHA1;
		ar >> m_bTiger;
		ar >> m_bED2K;
		//if ( nVersion >= 37 )
		//{
		ar >> m_bBTH;
		ar >> m_bMD5;
		//}

		ar >> m_sServer;
		ar >> m_sNick;

		//if ( nVersion >= 36 )
		ar >> m_sCountry;
		//else
		//	m_sCountry = theApp.GetCountryCode( m_pAddress );

		//if ( nVersion >= 38 )
		ar >> m_sCountryName;
		//else
		//	m_sCountryName = theApp.GetCountryName( m_pAddress );

		ar >> m_nSpeed;
		ar >> m_bPushOnly;
		ar >> m_bCloseConn;
		ar >> m_bReadContent;
		ReadArchive( ar, &m_tLastSeen, sizeof( FILETIME ) );

		SerializeIn2( ar, m_oPastFragments, nVersion );

		//if ( nVersion >= 39 )
		ar >> m_bClientExtended;
		//else
		//	m_bClientExtended = VendorCache.IsExtended( m_sServer );

		//if ( nVersion >= 42 )	// 1000
			ar >> m_bMetaIgnore;
	}
	//else	// nVersion < 21	Obsolete legacy Shareaza, for reference?
	//{
	//	DWORD nIndex;
	//	ReadArchive( ar, &m_pAddress, sizeof( m_pAddress ) );
	//	ar >> m_nPort;
	//	ar >> m_nSpeed;
	//	ar >> nIndex;
	//	ar >> m_sName;
	//	if ( nVersion >= 4 ) ar >> m_sURL;
	//	if ( nVersion >= 21 ) ar >> m_nProtocol;
	//	ar >> m_bSHA1;
	//	if ( nVersion >= 13 ) ar >> m_bTiger;
	//	if ( nVersion >= 13 ) ar >> m_bED2K;
	//	if ( nVersion >= 10 ) ar >> m_bHashAuth;
	//
	//	if ( nVersion == 8 )
	//	{
	//		DWORD nV;
	//		ar >> nV;
	//		m_sServer.Format( _T("%c%c%c%c"), nV & 0xFF, ( nV >> 8 ) & 0xFF, ( nV >> 16 ) & 0xFF, nV >> 24 );
	//	}
	//	else if ( nVersion >= 9 )
	//		ar >> m_sServer;
	//
	//	ar >> m_bPushOnly;
	//	ar >> m_bReadContent;
	//	ar >> m_bCloseConn;
	//	if ( nVersion >= 12 ) ReadArchive( ar, &m_tLastSeen, sizeof( FILETIME ) );
	//
	//	ReadArchive( ar, &m_oGUID[ 0 ], Hashes::Guid::byteCount );
	//	ReadArchive( ar, &m_oGUID[ 0 ], Hashes::Guid::byteCount );
	//	m_oGUID.validate();
	//
	//	SerializeIn2( ar, m_oPastFragments, nVersion );
	//
	//	ResolveURL();
	//}
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource create transfer

CDownloadTransfer* CDownloadSource::CreateTransfer(LPVOID pParam)
{
	ASSUME_LOCK( Transfers.m_pSection );
	ASSERT( m_pTransfer == NULL );

	switch ( m_nProtocol )
	{
	case PROTOCOL_G1:
		return ( m_pTransfer = new CDownloadTransferHTTP( this ) );
	case PROTOCOL_G2:
		return ( m_pTransfer = new CDownloadTransferHTTP( this ) );
	case PROTOCOL_ED2K:
		return ( m_pTransfer = new CDownloadTransferED2K( this ) );
	case PROTOCOL_HTTP:
		return ( m_pTransfer = new CDownloadTransferHTTP( this ) );
	case PROTOCOL_FTP:
		return ( m_pTransfer = new CDownloadTransferFTP( this ) );
	case PROTOCOL_BT:
		return ( m_pTransfer = new CDownloadTransferBT( this, (CBTClient*)pParam ) );
	case PROTOCOL_DC:
		return ( m_pTransfer = new CDownloadTransferDC( this, (CDCClient*)pParam ) );
	default:	// PROTOCOL_ANY PROTOCOL_NULL
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource check

BOOL CDownloadSource::CanInitiate(BOOL bNetwork, BOOL bEstablished)
{
	if ( ! Network.IsConnected() ) return FALSE;

	if ( Settings.Connection.RequireForTransfers )
	{
		switch ( m_nProtocol )
		{
		case PROTOCOL_G1:
			if ( ! Settings.Gnutella1.Enabled ) return FALSE;
			break;
		case PROTOCOL_G2:
			if ( ! Settings.Gnutella2.Enabled ) return FALSE;
			break;
		case PROTOCOL_ED2K:
			if ( ! Settings.eDonkey.Enabled || ! bNetwork ) return FALSE;
			break;
		case PROTOCOL_HTTP:
			switch ( m_nGnutella )
			{
			case 0:		// Pure HTTP source
				if ( ! bNetwork ) return FALSE;
				break;
			case 1:		// Pure G1 source
				if ( ! Settings.Gnutella1.Enabled ) return FALSE;
				break;
			case 2:		// Pure G2 source
				if ( ! Settings.Gnutella2.Enabled ) return FALSE;
				break;
			case 3:		// Mixed G1/G2 source
				if ( ! Settings.Gnutella1.Enabled &&
					 ! Settings.Gnutella2.Enabled ) return FALSE;
				break;
			}
			break;
		case PROTOCOL_FTP:
			if ( ! bNetwork ) return FALSE;
			break;
		case PROTOCOL_BT:
			if ( ! Settings.BitTorrent.Enabled || ! bNetwork ) return FALSE;
			break;
		case PROTOCOL_DC:
			if ( ! Settings.DC.Enabled || ! bNetwork ) return FALSE;
			break;
		default:	// PROTOCOL_ANY PROTOCOL_NULL
			return FALSE;
		}
	}

	if ( ! bEstablished && m_pDownload->LookupFailedSource( m_sURL ) != NULL )
	{
		// Don't try to connect to sources which we determined were bad
		// We will check them later after 2 hours cleanup
		Close();

		if ( Settings.Downloads.NeverDrop )
		{
			m_bKeep = TRUE;
			m_tAttempt = CalcFailureDelay();

			m_pDownload->SetModified();
		}
		else
		{
			Remove( TRUE, TRUE );
		}

		return FALSE;
	}

	if ( Settings.Connection.IgnoreOwnIP && Network.IsSelfIP( m_pAddress ) )
		return FALSE;

	return bEstablished || Downloads.AllowMoreTransfers( (IN_ADDR*)&m_pAddress );
}

bool CDownloadSource::IsPreviewCapable() const
{
	ASSUME_LOCK( Transfers.m_pSection );

	switch ( m_nProtocol )
	{
	case PROTOCOL_HTTP:
		return ( m_bPreview != FALSE );

	case PROTOCOL_ED2K:
		return ( m_pTransfer &&
			static_cast< CDownloadTransferED2K* >( m_pTransfer )->m_pClient->m_bEmPreview );

	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource remove

void CDownloadSource::Remove(BOOL bCloseTransfer, BOOL bBan, DWORD nRetryAfter)
{
	ASSUME_LOCK( Transfers.m_pSection );

	if ( m_pTransfer != NULL )
	{
		if ( bCloseTransfer )
		{
			Close( nRetryAfter );
			ASSERT( m_pTransfer == NULL );
		}
		else
		{
			// Transfer already closed
			ASSERT( m_pTransfer->GetSource() == NULL );
			m_pTransfer = NULL;
		}
	}

	m_pDownload->RemoveSource( this, m_pDownload->IsSeeding() ? FALSE : bBan );

	delete this;
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource failure handler

void CDownloadSource::OnFailure(BOOL bNondestructive, DWORD nRetryAfter)
{
	ASSUME_LOCK( Transfers.m_pSection );

	if ( m_pTransfer != NULL )
	{
		// Transfer already closed
		ASSERT( m_pTransfer->GetSource() == NULL );
		m_pTransfer = NULL;
	}

	DWORD nDelay = CalcFailureDelay(nRetryAfter);

	// This is not too good because if the source has Uploaded even 1Byte data, Max failure gets set to 40
	//int nMaxFailures = ( m_bReadContent ? 40 : 3 );

	int nMaxFailures = Settings.Downloads.MaxAllowedFailures;

	if ( nMaxFailures < 20 &&
		m_pDownload->GetSourceCount() > Settings.Downloads.StartDroppingFailedSourcesNumber )
		nMaxFailures = 0;

	if ( bNondestructive || ( ++m_nFailures < nMaxFailures ) )
	{
		m_tAttempt = max( m_tAttempt, nDelay );
		m_pDownload->SetModified();
	}
	else
	{
		if ( Settings.Downloads.NeverDrop )
		{
			// Keep source
			m_bKeep = TRUE;
			m_tAttempt = CalcFailureDelay();
			m_pDownload->SetModified();
		}
		else
		{
			Remove( TRUE, TRUE );
			// Add to the bad sources list (X-NAlt) if bBan == TRUE
			//m_pDownload->RemoveSource( this, ! m_pDownload->IsSeeding() );
		}
	}
}

DWORD CDownloadSource::CalcFailureDelay(DWORD nRetryAfter) const
{
	if ( nRetryAfter )
		return GetTickCount() + nRetryAfter * 1000;

	DWORD nDelayFactor = max( ( m_nBusyCount ? m_nBusyCount - 1 : 0 ), m_nFailures );
	DWORD nDelay = Settings.Downloads.RetryDelay * ( 1u << nDelayFactor );

	if ( nDelayFactor < 20 )
	{
		if ( nDelay > 3600000 ) nDelay = 3600000;
	}
	else  // It is nasty to set 1 Day delay (86400000)
	{
		if ( nDelay > 72000000 ) nDelay = 72000000;
	}

	return GetTickCount() + nDelay;
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource resume handler

void CDownloadSource::OnResume()
{
	m_tAttempt = 0;
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource closed connection resume handler

void CDownloadSource::OnResumeClosed()
{
	ASSUME_LOCK( Transfers.m_pSection );

	if ( m_pTransfer != NULL )
	{
		// Transfer already closed
		ASSERT( m_pTransfer->GetSource() == NULL );
		m_pTransfer = NULL;
	}

	m_tAttempt = 0;
	m_pDownload->SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource status

void CDownloadSource::SetValid()
{
	m_bReadContent = TRUE;
	m_nFailures = 0;
	m_bKeep = FALSE;
	m_pDownload->SetModified();
}

void CDownloadSource::SetLastSeen()
{
	SYSTEMTIME pTime;
	GetSystemTime( &pTime );
	SystemTimeToFileTime( &pTime, &m_tLastSeen );
	m_bKeep = FALSE;
	m_pDownload->SetModified();
}

void CDownloadSource::SetGnutella(int nGnutella)
{
	m_nGnutella |= nGnutella;
	m_pDownload->SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource hash check and learn

BOOL CDownloadSource::CheckHash(const Hashes::Sha1Hash& oSHA1)
{
	if ( m_pDownload->m_oSHA1 && ! m_bHashAuth )
	{
		if ( validAndUnequal( m_pDownload->m_oSHA1, oSHA1 ) ) return FALSE;
	}
	else
	{
		if ( m_pDownload->IsMultiFileTorrent() ) return TRUE;

		m_pDownload->m_oSHA1 = oSHA1;
	}

	m_bSHA1 = TRUE;
	m_pDownload->SetModified();

	return TRUE;
}

BOOL CDownloadSource::CheckHash(const Hashes::TigerHash& oTiger)
{
	if ( m_pDownload->m_oTiger && ! m_bHashAuth )
	{
		if ( validAndUnequal( m_pDownload->m_oTiger, oTiger ) ) return FALSE;
	}
	else
	{
		if ( m_pDownload->IsMultiFileTorrent() ) return TRUE;

		m_pDownload->m_oTiger = oTiger;
	}

	m_bTiger = TRUE;
	m_pDownload->SetModified();

	return TRUE;
}

BOOL CDownloadSource::CheckHash(const Hashes::Ed2kHash& oED2K)
{
	if ( m_pDownload->m_oED2K && ! m_bHashAuth )
	{
		if ( validAndUnequal( m_pDownload->m_oED2K, oED2K ) ) return FALSE;
	}
	else
	{
		if ( m_pDownload->IsMultiFileTorrent() ) return TRUE;

		m_pDownload->m_oED2K = oED2K;
	}

	m_bED2K = TRUE;
	m_pDownload->SetModified();

	return TRUE;
}

BOOL CDownloadSource::CheckHash(const Hashes::BtHash& oBTH)
{
	if ( m_pDownload->m_oBTH && ! m_bHashAuth )
	{
		if ( validAndUnequal( m_pDownload->m_oBTH, oBTH ) ) return FALSE;
	}
	else
	{
		if ( m_pDownload->IsTorrent() ) return TRUE;

		m_pDownload->m_oBTH = oBTH;
	}

	m_bBTH = TRUE;
	m_pDownload->SetModified();

	return TRUE;
}

BOOL CDownloadSource::CheckHash(const Hashes::Md5Hash& oMD5)
{
	if ( m_pDownload->m_oMD5 && ! m_bHashAuth )
	{
		if ( validAndUnequal( m_pDownload->m_oMD5, oMD5 ) ) return FALSE;
	}
	else
	{
		if ( m_pDownload->IsMultiFileTorrent() ) return TRUE;

		m_pDownload->m_oMD5 = oMD5;
	}

	m_bMD5 = TRUE;
	m_pDownload->SetModified();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource push request

BOOL CDownloadSource::PushRequest()
{
	switch ( m_nProtocol )
	{
	case PROTOCOL_BT:
		return FALSE;

	case PROTOCOL_ED2K:
		if ( m_nServerPort == 0 )
			return FALSE;
		if ( EDClients.IsFull() )
			return TRUE;
		if ( CEDClient* pClient = EDClients.Connect( m_pAddress.S_un.S_addr, m_nPort, &m_pServerAddress, m_nServerPort, m_oGUID ) )
		{
			if ( pClient != NULL && pClient->m_bConnected )
			{
				pClient->SeekNewDownload();
				return TRUE;
			}
		}
		if ( Neighbours.PushDonkey( m_pAddress.S_un.S_addr, m_pServerAddress, m_nServerPort ) )
		{
			theApp.Message( MSG_INFO, IDS_DOWNLOAD_PUSH_SENT, (LPCTSTR)m_pDownload->m_sName );
			m_tAttempt = GetTickCount() + Settings.Downloads.PushTimeout;
			return TRUE;
		}
		break;

	case PROTOCOL_DC:
		{
			BOOL bSuccess = FALSE;
			if ( DCClients.Connect( &m_pServerAddress, m_nServerPort, m_sNick, bSuccess ) )
			{
				if ( bSuccess )
				{
					theApp.Message( MSG_INFO, IDS_DOWNLOAD_PUSH_SENT, (LPCTSTR)m_pDownload->m_sName );
					m_tAttempt = GetTickCount() + Settings.Downloads.PushTimeout;
				}
				else
				{
					m_tAttempt = GetTickCount() + Settings.Downloads.RetryDelay;
				}
				return TRUE;
			}
		}
		break;

	default:
		if ( ! m_oGUID )
			return FALSE;
		if ( Network.SendPush( m_oGUID, m_nIndex ) )
		{
			theApp.Message( MSG_INFO, IDS_DOWNLOAD_PUSH_SENT, (LPCTSTR)m_pDownload->m_sName );
			m_tAttempt = GetTickCount() + Settings.Downloads.PushTimeout;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CDownloadSource::CheckPush(const Hashes::Guid& oClientID)
{
	return ( m_nProtocol == PROTOCOL_HTTP || m_nProtocol == PROTOCOL_DC ) &&
		validAndEqual( m_oGUID, oClientID );
}

BOOL CDownloadSource::CheckDonkey(const CEDClient* pClient)
{
	if ( m_nProtocol != PROTOCOL_ED2K )
		return FALSE;

	if ( m_oGUID && pClient->m_oGUID )
		return m_oGUID == pClient->m_oGUID;

	if ( m_bPushOnly )
		return	m_pServerAddress.S_un.S_addr == pClient->m_pServer.sin_addr.S_un.S_addr &&
				m_pAddress.S_un.S_addr == pClient->m_nClientID;

	return m_pAddress.S_un.S_addr == pClient->m_pHost.sin_addr.S_un.S_addr;
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource past fragments

void CDownloadSource::AddFragment(QWORD nOffset, QWORD nLength, BOOL /*bMerge*/)
{
	m_bReadContent = TRUE;
	m_oPastFragments.insert( Fragments::Fragment( nOffset, nOffset + nLength ) );
	m_pDownload->SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource available ranges

void CDownloadSource::SetAvailableRanges(LPCTSTR pszRanges)
{
	m_oAvailable.clear();

	if ( ! pszRanges || ! *pszRanges ) return;
	if ( _tcsnicmp( pszRanges, _T("bytes"), 5 ) ) return;

	CString strRanges( pszRanges + 6 );

	for ( strRanges += ',' ; strRanges.GetLength() ; )
	{
		CString strRange = strRanges.SpanExcluding( _T(", \t") );
		strRanges = strRanges.Mid( strRange.GetLength() + 1 );
		if ( strRange.Find( '-' ) < 0 ) continue;
		strRange.Trim();

		QWORD nFirst = 0, nLast = 0;

		// 0 - 0 has special meaning
		if ( _stscanf( strRange, _T("%I64i-%I64i"), &nFirst, &nLast ) == 2 && nLast > nFirst )
		{
			// Sanity check:  Perhaps the file size we expect is incorrect or the source is erronous,
			// in either case we make sure the range fits - so we chop off the end if necessary.
			if ( nFirst < m_oAvailable.limit() )
				m_oAvailable.insert( Fragments::Fragment( nFirst, min( nLast + 1, m_oAvailable.limit() ) ) );
		}
	}

	m_pDownload->SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource range intersection test

BOOL CDownloadSource::HasUsefulRanges() const
{
	if ( m_oAvailable.empty() )
		return m_pDownload->IsRangeUseful( 0, m_pDownload->m_nSize );

	return m_pDownload->AreRangesUseful( m_oAvailable );
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource range intersection

BOOL CDownloadSource::TouchedRange(QWORD nOffset, QWORD nLength) const
{
	if ( m_pTransfer != NULL &&
		 m_pTransfer->m_nState == dtsDownloading &&
		 m_pTransfer->m_nOffset + m_pTransfer->m_nLength > nOffset &&
		 m_pTransfer->m_nOffset < nOffset + nLength )
		return TRUE;

	return m_oPastFragments.overlaps( Fragments::Fragment( nOffset, nOffset + nLength ) );
}

//////////////////////////////////////////////////////////////////////
// CDownloadSource color

int CDownloadSource::GetColor()
{
	if ( m_nColor >= 0 ) return m_nColor;
	m_nColor = m_pDownload->GetSourceColor();
	return m_nColor;
}

void CDownloadSource::Close(DWORD nRetryAfter)
{
	ASSUME_LOCK( Transfers.m_pSection );

	if ( m_pTransfer )
	{
		m_pTransfer->Close( TRI_TRUE, nRetryAfter );
		ASSERT( m_pTransfer == NULL );
	}
}

void CDownloadSource::Draw(CDC* pDC, CRect* prcBar, COLORREF crNatural)
{
	ASSUME_LOCK( Transfers.m_pSection );

	if ( ! IsIdle() )
	{
		//if ( m_pTransfer->m_nLength < SIZE_UNKNOWN )
		//{
			CFragmentBar::DrawStateBar( pDC, prcBar, m_pDownload->m_nSize,
				m_pTransfer->m_nOffset, m_pTransfer->m_nLength,
				Colors.m_crFragmentRequest, TRUE );
		//}

		switch ( GetTransferProtocol() )
		{
		case PROTOCOL_ED2K:
			{
				Fragments::Queue::const_iterator pItr
					= static_cast< CDownloadTransferED2K* >( m_pTransfer )->m_oRequested.begin();
				const Fragments::Queue::const_iterator pEnd
					= static_cast< CDownloadTransferED2K* >( m_pTransfer )->m_oRequested.end();
				for ( ; pItr != pEnd ; ++pItr )
				{
					CFragmentBar::DrawStateBar( pDC, prcBar, m_pDownload->m_nSize,
						pItr->begin(), pItr->size(), Colors.m_crFragmentRequest, true );
				}
			}
			break;

		case PROTOCOL_BT:
			{
				Fragments::Queue::const_iterator pItr
					= static_cast< CDownloadTransferBT* >( m_pTransfer )->m_oRequested.begin();
				const Fragments::Queue::const_iterator pEnd
					= static_cast< CDownloadTransferBT* >( m_pTransfer )->m_oRequested.end();
				for ( ; pItr != pEnd ; ++pItr )
				{
					CFragmentBar::DrawStateBar( pDC, prcBar, m_pDownload->m_nSize,
						pItr->begin(), pItr->size(), Colors.m_crFragmentRequest, true );
				}
			}
			break;

		//default:
		//	;	// Do nothing more
		}
	}

	Draw( pDC, prcBar );

	// Draw empty bar areas
	if ( ! m_oAvailable.empty() )
	{
		Fragments::List::const_iterator pItr = m_oAvailable.begin();
		const Fragments::List::const_iterator pEnd = m_oAvailable.end();
		for ( ; pItr != pEnd ; ++pItr )
		{
			CFragmentBar::DrawFragment( pDC, prcBar, m_pDownload->m_nSize,
				pItr->begin(), pItr->size(), crNatural, FALSE );
		}

		if ( ! Images.DrawButtonState( pDC, prcBar, IMAGE_PROGRESSBAR_NONE ) )
			pDC->FillSolidRect( prcBar, Colors.m_crWindow );
	}
	else if ( IsOnline() && HasUsefulRanges() || ! m_oPastFragments.empty() )
	{
		if ( ! Images.DrawButtonState( pDC, prcBar, IMAGE_PROGRESSBAR_SHADED ) )
			pDC->FillSolidRect( prcBar, crNatural );
	}
	else
	{
		if ( ! Images.DrawButtonState( pDC, prcBar, IMAGE_PROGRESSBAR_NONE ) )
			pDC->FillSolidRect( prcBar, Colors.m_crWindow );
	}
}

void CDownloadSource::Draw(CDC* pDC, CRect* prcBar)
{
	ASSUME_LOCK( Transfers.m_pSection );

	static COLORREF crFill[] =
	{
		Colors.m_crFragmentSource1, Colors.m_crFragmentSource2,
		Colors.m_crFragmentSource3, Colors.m_crFragmentSource4,
		Colors.m_crFragmentSource5, Colors.m_crFragmentSource6
	};

	COLORREF crTransfer = m_bReadContent ? crFill[ GetColor() ] : Colors.m_crFragmentComplete;
	crTransfer = CColors::CalculateColor( crTransfer, Colors.m_crHighlight, 90 );

	if ( ! IsIdle() && GetState() == dtsDownloading )	//&& m_pTransfer->m_nOffset < SIZE_UNKNOWN )
	{
		if ( m_pTransfer->m_bRecvBackwards )
		{
			CFragmentBar::DrawFragment( pDC, prcBar, m_pDownload->m_nSize,
				m_pTransfer->m_nOffset + m_pTransfer->m_nLength - m_pTransfer->m_nPosition,
				m_pTransfer->m_nPosition, crTransfer, TRUE );
		}
		else
		{
			CFragmentBar::DrawFragment( pDC, prcBar, m_pDownload->m_nSize,
				m_pTransfer->m_nOffset,
				m_pTransfer->m_nPosition, crTransfer, TRUE );
		}
	}

	Fragments::List::const_iterator pItr = m_oPastFragments.begin();
	const Fragments::List::const_iterator pEnd = m_oPastFragments.end();
	for ( ; pItr != pEnd ; ++pItr )
	{
		CFragmentBar::DrawFragment( pDC, prcBar, m_pDownload->m_nSize,
			pItr->begin(), pItr->size(), crTransfer, TRUE );
	}
}
