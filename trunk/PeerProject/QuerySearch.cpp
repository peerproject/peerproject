//
// QuerySearch.cpp
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
#include "QuerySearch.h"
#include "Network.h"
#include "Datagrams.h"
#include "G1Packet.h"
#include "G2Packet.h"
#include "EDPacket.h"
#include "DCPacket.h"
#include "PeerProjectURL.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "QueryHashTable.h"
#include "GGEP.h"
#include "XML.h"

#include "WndSearch.h"
#include "DlgHelp.h"

#include "Download.h"
#include "Downloads.h"
#include "Transfers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CQuerySearch, CComObject)


//////////////////////////////////////////////////////////////////////
// CQuerySearch construction

CQuerySearch::CQuerySearch(BOOL bGUID)
	: m_bAutostart	( true )
	, m_pSchema 	( NULL )
	, m_pXML		( NULL )
	, m_nMinSize	( 0 )
	, m_nMaxSize	( SIZE_UNKNOWN )
	, m_bWantURL	( TRUE )
	, m_bWantDN 	( TRUE )
	, m_bWantXML	( TRUE )
	, m_bWantCOM	( TRUE )
	, m_bWantPFS	( TRUE )
	, m_bAndG1		( Settings.Gnutella1.Enabled )
	, m_bUDP		( FALSE )
	, m_nHops		( 0 )
	, m_nKey		( 0 )
	, m_bFirewall	( false )
	, m_bDynamic	( false )
	, m_bBinHash	( false )
	, m_bOOB		( false )
	, m_bOOBv3		( false )
	, m_bPartial	( false )
	, m_bNoProxy	( false )
	, m_bExtQuery	( false )
	, m_bWhatsNew	( false )
	, m_bDropMe 	( false )
	, m_oWords		( )
	, m_oNegWords	( )
	, m_nProtocol	( PROTOCOL_ANY )	// Monitor display
{
	if ( bGUID ) Network.CreateID( m_oGUID );

	ZeroMemory( &m_pEndpoint, sizeof( m_pEndpoint ) );
	m_pEndpoint.sin_family = AF_INET;

	ZeroMemory( &m_pMyHub, sizeof( m_pMyHub ) );
	m_pMyHub.sin_family = AF_INET;

	m_dwRef = 0;
}

CQuerySearch::~CQuerySearch()
{
	delete m_pXML;
}


//////////////////////////////////////////////////////////////////////
// CQuerySearch to G1 packet

CG1Packet* CQuerySearch::ToG1Packet(DWORD nTTL) const
{
	CG1Packet* pPacket = CG1Packet::New( G1_PACKET_QUERY,
		( nTTL ? min( nTTL, Settings.Gnutella1.SearchTTL ) : Settings.Gnutella1.SearchTTL ),
		m_oGUID );

	BOOL bWhatsNew = FALSE;
	const BOOL bFirewall = Network.IsFirewalled( CHECK_TCP );
	const BOOL bOOB = ( Settings.Gnutella1.EnableOOB && ! Network.IsFirewalled( CHECK_UDP ) );

	WORD nFlags = G1_QF_TAG | G1_QF_BIN_HASH | G1_QF_DYNAMIC;
	if ( bFirewall )
		nFlags |= G1_QF_FIREWALLED;
	// ToDo: nFlags |= G1_QF_FWTRANS;
	if ( bOOB )		// Currently disabled by default.  ToDo: Verify and enable Out of Band query hits
		nFlags |= G1_QF_OOB;
	if ( m_bWantXML )
		nFlags |= G1_QF_XML;
	pPacket->WriteShortLE( nFlags );

	CString strQuery, strFullQuery;
	if ( ! m_sPosKeywords.IsEmpty() )
	{
		strQuery = m_sPosKeywords;
	}
	else if ( m_pSchema && m_pXML )
	{
		strQuery = m_pSchema->GetIndexedWords( m_pXML->GetFirstElement() );
		MakeKeywords( strQuery, false );
	}
	if ( strQuery.GetLength() > OLD_LW_MAX_QUERY_FIELD_LEN )
	{
		strFullQuery = strQuery;

		strQuery = strQuery.Left( OLD_LW_MAX_QUERY_FIELD_LEN );
		int nPos = strQuery.ReverseFind( _T(' ') );
		if ( nPos > 0 )
			strQuery = strQuery.Left( nPos );
	}

	if ( strQuery.IsEmpty() && ! HasHash() )
	{
		// Whats New search
		bWhatsNew = TRUE;
		strQuery = _T( WHAT_IS_NEW_QUERY_STRING );
	}

	if ( ! strQuery.IsEmpty() )
	{
		if ( Settings.Gnutella1.QuerySearchUTF8 )
			pPacket->WriteStringUTF8( strQuery );
		else
			pPacket->WriteString( strQuery );
	}
	else	// Hash search
	{
		pPacket->WriteString( _T( DEFAULT_URN_QUERY ) );
	}

	// HUGE extension  -Deprecated, replaced by GGEP_HEADER_HASH (G1_QF_BIN_HASH).
	if ( ! m_bBinHash || ! Settings.Gnutella1.EnableGGEP )
	{
		if ( m_oSHA1 && m_oTiger )
		{
			pPacket->WriteString( _T("urn:bitprint:") + m_oSHA1.toString() + _T('.') + m_oTiger.toString(), FALSE );
			pPacket->WriteByte( G1_PACKET_HIT_SEP );
		}
		else if ( m_oSHA1 )
		{
			pPacket->WriteString( _T("urn:sha1:") + m_oSHA1.toString(), FALSE );
			pPacket->WriteByte( G1_PACKET_HIT_SEP );
		}
		else if ( m_oTiger )
		{
			pPacket->WriteString( _T("urn:ttroot:") + m_oTiger.toString(), FALSE );
			pPacket->WriteByte( G1_PACKET_HIT_SEP );
		}
		if ( m_oED2K )
		{
			pPacket->WriteString( m_oED2K.toUrn(), FALSE );
			pPacket->WriteByte( G1_PACKET_HIT_SEP );
		}
		if ( m_oMD5 )
		{
			pPacket->WriteString( m_oMD5.toUrn(), FALSE );
			pPacket->WriteByte( G1_PACKET_HIT_SEP );
		}
		if ( m_oBTH )
		{
			pPacket->WriteString( m_oBTH.toUrn(), FALSE );
			pPacket->WriteByte( G1_PACKET_HIT_SEP );
		}
	}

	// XML extension

	if ( m_pXML )
	{
		pPacket->WriteString( m_pXML->ToString( TRUE ), FALSE );
		pPacket->WriteByte( G1_PACKET_HIT_SEP );
	}

	// GGEP extension (last)

	if ( Settings.Gnutella1.EnableGGEP )
	{
		CGGEPBlock pBlock;

		// ToDo: GGEP_HEADER_QUERY_KEY_SUPPORT + query key

		// ToDo: GGEP_HEADER_NO_PROXY

		// GGEP_HEADER_META
		if ( m_pSchema )
		{
			BYTE nMeta = 0;
			if ( m_pSchema->CheckURI( CSchema::uriAudio ) )
				nMeta = GGEP_META_AUDIO;
			else if ( m_pSchema->CheckURI( CSchema::uriVideo ) )
				nMeta = GGEP_META_VIDEO;
			else if ( m_pSchema->CheckURI( CSchema::uriDocument ) )
				nMeta = GGEP_META_DOCUMENTS;
			else if ( m_pSchema->CheckURI( CSchema::uriImage ) )
				nMeta = GGEP_META_IMAGES;
			else if ( m_pSchema->CheckURI( CSchema::uriApplication ) )
				nMeta = GGEP_META_WINDOWS;
			if ( nMeta )
			{
				if ( CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_META ) )
				{
					pItem->WriteByte( nMeta );
				}
			}
		}

		if ( bOOB )				// Currently disabled by default.  ToDo: Verify and enable Out of Band query hits
			pBlock.Add( GGEP_HEADER_SECURE_OOB );

		if ( m_bWantPFS )
			pBlock.Add( GGEP_HEADER_PARTIAL_RESULT_PREFIX );

		if ( bWhatsNew )
			pBlock.Add( GGEP_HEADER_FEATURE_QUERY );

		if ( ! strFullQuery.IsEmpty() )
		{
			CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_EXTENDED_QUERY );
			pItem->WriteUTF8( strFullQuery );
		}

		if ( m_bBinHash )
		{
			if ( m_oSHA1 && m_oTiger )
			{
				CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_HASH );
				pItem->WriteByte( GGEP_H_BITPRINT );
				pItem->Write( &m_oSHA1[ 0 ], 20 );
				pItem->Write( &m_oTiger[ 0 ], 24 );
			}
			else if ( m_oSHA1 )
			{
				CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_HASH );
				pItem->WriteByte( GGEP_H_SHA1 );
				pItem->Write( &m_oSHA1[ 0 ], 20 );
			}
			if ( m_oMD5 )
			{
				CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_HASH );
				pItem->WriteByte( GGEP_H_MD5 );
				pItem->Write( &m_oMD5[ 0 ], 16 );
			}
			if ( m_oED2K )
			{
				CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_URN );
				pItem->WriteUTF8( CString( _T("ed2k:") ) + m_oED2K.toString() );
			}
			if ( m_oBTH )
			{
				CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_URN );
				pItem->WriteUTF8( CString( _T("btih:") ) + m_oBTH.toString() );
			}
		}

		pBlock.Write( pPacket );
	}

	// End of hit  (like Limewire)
	pPacket->WriteByte( 0 );

	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch to G2 packet

CG2Packet* CQuerySearch::ToG2Packet(SOCKADDR_IN* pUDP, DWORD nKey) const
{
	CG2Packet* pPacket = CG2Packet::New( G2_PACKET_QUERY, TRUE );

	if ( pUDP )
	{
		pPacket->WritePacket( G2_PACKET_UDP, nKey ? 10 : 6 );
		pPacket->WriteLongLE( pUDP->sin_addr.S_un.S_addr );
		pPacket->WriteShortBE( htons( pUDP->sin_port ) );
		if ( nKey ) pPacket->WriteLongBE( nKey );
	}
	else if ( nKey )
	{
		pPacket->WritePacket( G2_PACKET_QKY, 4 );
		pPacket->WriteLongBE( nKey );
	}

	if ( m_oTiger && m_oSHA1 )
	{
		pPacket->WritePacket( G2_PACKET_URN, Hashes::Sha1Hash::byteCount + Hashes::TigerHash::byteCount + 3 );
		pPacket->WriteString( "bp" );
		pPacket->Write( m_oSHA1 );
		pPacket->Write( m_oTiger );
	}
	else if ( m_oSHA1 )
	{
		pPacket->WritePacket( G2_PACKET_URN, Hashes::Sha1Hash::byteCount + 5 );
		pPacket->WriteString( "sha1" );
		pPacket->Write( m_oSHA1 );
	}
	else if ( m_oTiger )
	{
		pPacket->WritePacket( G2_PACKET_URN, Hashes::TigerHash::byteCount + 4 );
		pPacket->WriteString( "ttr" );
		pPacket->Write( m_oTiger );
	}

	// If the target source has only ed2k hash (w/o SHA1) it will allow to find such files
	if ( m_oED2K )
	{
		pPacket->WritePacket( G2_PACKET_URN, Hashes::Ed2kHash::byteCount + 5 );
		pPacket->WriteString( "ed2k" );
		pPacket->Write( m_oED2K );
	}

	if ( m_oMD5 )
	{
		pPacket->WritePacket( G2_PACKET_URN, Hashes::Md5Hash::byteCount + 4 );
		pPacket->WriteString( "md5" );
		pPacket->Write( m_oMD5 );
	}

	if ( m_oBTH )
	{
		pPacket->WritePacket( G2_PACKET_URN, Hashes::BtHash::byteCount + 5 );
		pPacket->WriteString( "btih" );
		pPacket->Write( m_oBTH );
	}

	if ( ! HasHash() && ! m_sG2Keywords.IsEmpty() )
	{
		// Randomly select keywords or exact search string
		if ( m_sSearch.IsEmpty() || GetRandomNum( FALSE, TRUE ) )
		{
			pPacket->WritePacket( G2_PACKET_DESCRIPTIVE_NAME, pPacket->GetStringLen( m_sG2Keywords ) );
			pPacket->WriteString( m_sG2Keywords, FALSE );
		}
		else
		{
			pPacket->WritePacket( G2_PACKET_DESCRIPTIVE_NAME, pPacket->GetStringLen( m_sSearch ) );
			pPacket->WriteString( m_sSearch, FALSE );
		}
	}

	if ( m_pXML != NULL )
	{
		CString strXML;
		if ( CXMLElement* pBody = m_pXML->GetFirstElement() )
			strXML = pBody->ToString();
		pPacket->WritePacket( G2_PACKET_METADATA, pPacket->GetStringLen( strXML ) );
		pPacket->WriteString( strXML, FALSE );
	}

	if ( m_nMinSize != 0 || m_nMaxSize != SIZE_UNKNOWN )
	{
		if ( m_nMinSize == m_nMaxSize )
		{
			// Security patch for Anti-p2p/spammer protection faking QueryHit
			// for query both hash & filesize specified gives 1MB size frame in query  gives 1MB size frame in query
			// so anti-p2p/spammer hosts based on replying /QH2 packet generated on the info stored in Query packet,
			// can not determine actual size of file which searcher is really looking for.

			if ( m_nMinSize < 0xFFFFFFFF && ( m_nMaxSize < 0xFFFFFFFF || m_nMaxSize == SIZE_UNKNOWN ) )
			{
				pPacket->WritePacket( G2_PACKET_SIZE_RESTRICTION, 8 );
				pPacket->WriteLongBE( DWORD( m_nMinSize & 0xfffffffffff00000 ) );
				pPacket->WriteLongBE( m_nMaxSize == SIZE_UNKNOWN ? 0xFFFFFFFF : DWORD( m_nMaxSize | 0x00000000000fffff ) );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_SIZE_RESTRICTION, 16 );
				pPacket->WriteInt64( m_nMinSize & 0xfffffffffff00000 );
				pPacket->WriteInt64( m_nMaxSize | 0x00000000000fffff );
			}
		}
		else
		{
			if ( m_nMinSize < 0xFFFFFFFF && ( m_nMaxSize < 0xFFFFFFFF || m_nMaxSize == SIZE_UNKNOWN ) )
			{
				pPacket->WritePacket( G2_PACKET_SIZE_RESTRICTION, 8 );
				pPacket->WriteLongBE( (DWORD)m_nMinSize );
				pPacket->WriteLongBE( m_nMaxSize == SIZE_UNKNOWN ? 0xFFFFFFFF : (DWORD)m_nMaxSize );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_SIZE_RESTRICTION, 16 );
				pPacket->WriteInt64( m_nMinSize );
				pPacket->WriteInt64( m_nMaxSize );
			}
		}
	}

	if ( m_bWantURL || m_bWantDN || m_bWantXML || m_bWantCOM || m_bWantPFS )
	{
		pPacket->WritePacket( G2_PACKET_INTEREST,
			( m_bWantURL ? 4 : 0 ) + ( m_bWantDN ? 3 : 0 ) + ( m_bWantXML ? 3 : 0 ) +
			( m_bWantCOM ? 4 : 0 ) + ( m_bWantPFS ? 4 : 0 ) );

		if ( m_bWantURL ) pPacket->WriteString( "URL" );
		if ( m_bWantDN ) pPacket->WriteString( "DN" );
		if ( m_bWantXML ) pPacket->WriteString( "MD" );
		if ( m_bWantCOM ) pPacket->WriteString( "COM" );
		if ( m_bWantPFS ) pPacket->WriteString( "PFS" );
	}

	if ( m_bFirewall ) pPacket->WritePacket( G2_PACKET_NAT_DESC, 0 );
	//if ( m_bAndG1 ) pPacket->WritePacket( G2_PACKET_G1, 0 );

	pPacket->WriteByte( 0 );
	pPacket->Write( m_oGUID );

	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch to ED2K packet

CEDPacket* CQuerySearch::ToEDPacket(BOOL bUDP, DWORD nServerFlags) const
{
	BOOL bUTF8, bGetS2;

	CEDPacket* pPacket = NULL;

	CString strWords = m_pSchema->GetIndexedWords( m_pXML->GetFirstElement() );

	if ( bUDP )
	{
		bUTF8 = nServerFlags & ED2K_SERVER_UDP_UNICODE;
		bGetS2 = nServerFlags & ED2K_SERVER_UDP_GETSOURCES2;
	}
	else
	{
		bUTF8 = nServerFlags & ED2K_SERVER_TCP_UNICODE;
		bGetS2 = nServerFlags & ED2K_SERVER_TCP_GETSOURCES2;
	}

	if ( m_oED2K )
	{
		if ( m_bWantDN && Settings.eDonkey.MagnetSearch )
		{
			// We need the size- do a search by magnet (hash)
			pPacket = CEDPacket::New( bUDP ? ED2K_C2SG_SEARCHREQUEST : ED2K_C2S_SEARCHREQUEST );
			pPacket->WriteByte( 1 );
			pPacket->WriteEDString( _T("magnet:?xt=ed2k:") + m_oED2K.toString(), bUTF8 );
		}
		else
		{
			// Don't need the size- use GETSOURCES

			// For newer servers, send the file size if it's valid (and not over 4GB)
			if ( bGetS2 && m_nMinSize == m_nMaxSize && m_nMaxSize < 0xFFFFFFFF )
			{
				// theApp.Message( MSG_DEBUG, ( _T("Creating multi-hash capable GetSources2 for: ") + m_oED2K.toString() ) );

				// Newer server, send size as well as hash
				pPacket = CEDPacket::New( bUDP ? ED2K_C2SG_GETSOURCES2 : ED2K_C2S_GETSOURCES );
				// Add the hash/size this packet is for
				pPacket->Write( m_oED2K );
				pPacket->WriteLongLE( (DWORD)m_nMaxSize );
				// Add any other hashes that need to be searched for.
				WriteHashesToEDPacket( pPacket, bUDP );
			}
			else
			{
				// Old style GetSources, with no size attached
				pPacket = CEDPacket::New( bUDP ? ED2K_C2SG_GETSOURCES : ED2K_C2S_GETSOURCES );
				pPacket->Write( m_oED2K );
			}
		}
	}
	else if ( ! m_sKeywords.IsEmpty() && ! m_sSearch.IsEmpty() || ! strWords.IsEmpty() )
	{
		pPacket = CEDPacket::New( bUDP ? ED2K_C2SG_SEARCHREQUEST : ED2K_C2S_SEARCHREQUEST );

		if ( m_nMinSize > 0 || m_nMaxSize < 0xFFFFFFFF )
		{
			// Add size limits to search (if available)
			pPacket->WriteByte( 0 );		// Boolean AND (min/max) / (name/type)
			pPacket->WriteByte( 0 );

			pPacket->WriteByte( 0 );		// Boolean AND (Min/Max)
			pPacket->WriteByte( 0 );

			// Size limit (min)
			pPacket->WriteByte( 3 );
			pPacket->WriteLongLE( (DWORD)m_nMinSize );
			pPacket->WriteByte( 1 );
			pPacket->WriteShortLE( 1 );
			pPacket->WriteByte( ED2K_FT_FILESIZE );

			// Size limit (max)
			pPacket->WriteByte( 3 );
			pPacket->WriteLongLE( (DWORD)min( m_nMaxSize, (QWORD)0xFFFFFFFF ) );
			pPacket->WriteByte( 2 );
			pPacket->WriteShortLE( 1 );
			pPacket->WriteByte( ED2K_FT_FILESIZE );
		}

		if ( m_pSchema == NULL || m_pSchema->m_sDonkeyType.IsEmpty() )
		{
			// ed2k search without file type
			// Name / Key Words
			pPacket->WriteByte( 1 );
			// Check if this is a "search for similar files"
			if ( ( m_oSimilarED2K ) && ( ! bUDP ) && ( nServerFlags & ED2K_SERVER_TCP_RELATEDSEARCH ) )
				pPacket->WriteEDString( _T("related::") + m_oSimilarED2K.toString(), bUTF8 );
			else	// Regular search
				pPacket->WriteEDString( ! m_sSearch.IsEmpty() ? m_sSearch : strWords, bUTF8 );
		}
		else
		{
			// ed2k search including file type
			pPacket->WriteByte( 0 );		// Boolean AND (name/type)
			pPacket->WriteByte( 0 );

			// Name / Key Words
			pPacket->WriteByte( 1 );
			pPacket->WriteEDString( ! m_sSearch.IsEmpty() ? m_sSearch : strWords, bUTF8 );

			// Metadata (file type)
			pPacket->WriteByte( 2 );
			pPacket->WriteEDString( m_pSchema->m_sDonkeyType, bUTF8 );
			pPacket->WriteShortLE( 1 );
			pPacket->WriteByte( ED2K_FT_FILETYPE );
		}
	}

	return pPacket;
}

BOOL CQuerySearch::WriteHashesToEDPacket(CEDPacket* pPacket, BOOL bUDP) const
{
	ASSERT( ( pPacket->m_nType == bUDP ) ? ED2K_C2SG_GETSOURCES2 : ED2K_C2S_GETSOURCES );

	CSingleLock pLock( &Transfers.m_pSection );
	if ( ! pLock.Lock( 250 ) ) return FALSE;

	int nFiles = 1;		// There's one hash in the packet to begin with
	const DWORD tNow = GetTickCount();

	// Run through all active downloads
	for ( POSITION pos = Downloads.GetIterator() ; pos ; )
	{
		CDownload* pDownload = Downloads.GetNext( pos );

		// Basic check
		if ( pDownload->m_oED2K &&					// Must have an ed2k hash
			 pDownload->IsTrying() &&				// Must be active
			 pDownload->m_nSize < 0xFFFFFFFF &&		// Must have a valid size
			 pDownload->IsCompleted() == false &&	// Must not be complete
			 pDownload->NeedHashset() == FALSE &&	// Must have hashset
			 validAndUnequal( pDownload->m_oED2K, m_oED2K ) )// Must not be already added to packet
		{
			// If a download is allowed to ask for more sources
			DWORD tNextQuery = bUDP ? pDownload->m_tLastED2KGlobal + Settings.eDonkey.QueryFileThrottle : pDownload->m_tLastED2KLocal + Settings.eDonkey.QueryFileThrottle;
			if ( tNow > tNextQuery )
			{
				// If we want more sources for this file
				DWORD nSources = pDownload->GetSourceCount( FALSE, TRUE );
				if ( nSources < ( Settings.Downloads.SourcesWanted / 4u ) )
				{
					if ( nSources < Settings.Downloads.MinSources || nFiles < 10 ||
						 tNow > pDownload->m_tReceived + Settings.Downloads.StarveTimeout )		// ~45 Minutes
					{
						// Add the hash/size for this download
						pPacket->Write( pDownload->m_oED2K );
						pPacket->WriteLongLE( (DWORD)pDownload->m_nSize );
						if ( bUDP )
							pDownload->m_tLastED2KGlobal = tNow;
						else
							pDownload->m_tLastED2KLocal = tNow;
						nFiles ++;
						if ( nFiles >= ED2K_MAXFILESINPACKET )
							return TRUE;
					}
				}
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch to DC packet

CDCPacket* CQuerySearch::ToDCPacket() const
{
	// Active user / Passive user
	// $Search SenderIP:SenderPort (T|F)?(T|F)?Size?Type?String|
	// $Search Hub:Nick (T|F)?(T|F)?Size?Type?String|

	int nType = 1;
	if ( m_oTiger )
	{
		nType = 9;
	}
	else if ( m_pSchema )
	{
		if ( m_pSchema->CheckURI( CSchema::uriAudio ) )
			nType = 2;
		else if ( m_pSchema->CheckURI( CSchema::uriArchive ) )
			nType = 3;
		else if ( m_pSchema->CheckURI( CSchema::uriDocument ) )
			nType = 4;
		else if ( m_pSchema->CheckURI( CSchema::uriApplication ) )
			nType = 5;
		else if ( m_pSchema->CheckURI( CSchema::uriImage ) )
			nType = 6;
		else if ( m_pSchema->CheckURI( CSchema::uriVideo ) )
			nType = 7;
		else if ( m_pSchema->CheckURI( CSchema::uriFolder ) )
			nType = 8;
		else if ( m_pSchema->CheckURI( CSchema::uriROM ) )
			nType = 10;
	}

	CDCPacket* pPacket = CDCPacket::New();

	const bool bSizeRestriced = m_nMinSize != 0 || m_nMaxSize != SIZE_UNKNOWN;
	const bool bIsMaxSize = m_nMaxSize != SIZE_UNKNOWN || ! bSizeRestriced;

	CString strSearch = m_sSearch;
	strSearch.Replace( _T(' '), _T('$') );
	strSearch.Replace( _T('|'), _T('$') );

	CString strAddress;
	if ( Network.IsFirewalled( CHECK_IP ) )
		strAddress = _T("Hub:") + m_sMyNick;			// Passive search
	else
		strAddress = HostToString( &Network.m_pHost );	// Active search

	CString strQuery;
	strQuery.Format( _T("$Search %s %c?%c?%I64u?%d?%s|"),
		(LPCTSTR)strAddress,
		( bSizeRestriced ? _T('T') : _T('F') ),
		( bIsMaxSize ? _T('T') : _T('F') ),
		( bSizeRestriced ? ( bIsMaxSize ? m_nMaxSize : m_nMinSize ) : 0ull ),
		nType,
		(LPCTSTR)( m_oTiger ? ( _T("TTH:") + m_oTiger.toString() ) : strSearch ) );

	pPacket->WriteString( strQuery, FALSE );

	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch from packet root

CQuerySearchPtr CQuerySearch::FromPacket(CPacket* pPacket, const SOCKADDR_IN* pEndpoint, BOOL bGUID)
{
	CQuerySearchPtr pSearch = new CQuerySearch( bGUID );

	try
	{
		if ( pPacket->m_nProtocol == PROTOCOL_G1 )
		{
			pSearch->m_nProtocol = PROTOCOL_G1; 	// Display convenience
			if ( pSearch->ReadG1Packet( (CG1Packet*)pPacket, pEndpoint ) )
				return pSearch;
		}
		else if ( pPacket->m_nProtocol == PROTOCOL_G2 )
		{
			pSearch->m_nProtocol = PROTOCOL_G2; 	// Display convenience
			//if ( ((CG2Packet*)pPacket)->IsType( G2_PACKET_QUERY_WRAP ) )
			//	theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("CQuerySearch::FromPacket dropping obsolete wrapped packet") );
			//else
				if ( pSearch->ReadG2Packet( (CG2Packet*)pPacket, pEndpoint ) )
					return pSearch;
		}
		else if ( pPacket->m_nProtocol == PROTOCOL_DC )
		{
			pSearch->m_nProtocol = PROTOCOL_DC; 	// Display convenience
			if ( pSearch->ReadDCPacket( (CDCPacket*)pPacket, pEndpoint ) )
				return pSearch;
		}
		else if ( pPacket->m_nProtocol == PROTOCOL_ED2K )
		{
			pSearch->m_nProtocol = PROTOCOL_ED2K;	// Display convenience
		}
	}
	catch ( CException* pException )
	{
		pException->Delete();
	}

	return CQuerySearchPtr();
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch from G1 packet

BOOL CQuerySearch::ReadG1Packet(CG1Packet* pPacket, const SOCKADDR_IN* pEndpoint)
{
	m_nHops = pPacket->m_nHops;

	if ( pEndpoint )
		m_pEndpoint = *pEndpoint;

	if ( pPacket->m_nProtocol == PROTOCOL_G2 )
	{
		GNUTELLAPACKET pG1;
		if ( ! ((CG2Packet*)pPacket)->SeekToWrapped() )
			return NULL;
		pPacket->Read( &pG1, sizeof(pG1) );
		m_oGUID = pG1.m_oGUID;
	}
	else
	{
		m_oGUID = pPacket->m_oGUID;
	}
	m_oGUID.validate();

	if ( pPacket->GetRemaining() < 4 )
	{
		// Too short
		theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got too short query packet (%d bytes)"), pPacket->GetRemaining() );
		return FALSE;
	}

	WORD nFlags = pPacket->ReadShortLE();
	m_bFirewall	= ( nFlags & G1_QF_TAG ) && ( nFlags & G1_QF_FIREWALLED );
	m_bWantXML	= ( nFlags & G1_QF_TAG ) && ( nFlags & G1_QF_XML );
	m_bDynamic	= ( nFlags & G1_QF_TAG ) && ( nFlags & G1_QF_DYNAMIC );
	m_bBinHash	= ( nFlags & G1_QF_TAG ) && ( nFlags & G1_QF_BIN_HASH );
	m_bOOB		= ( nFlags & G1_QF_TAG ) && ( nFlags & G1_QF_OOB );

	if ( Settings.Gnutella1.QueryHitUTF8 )
		m_sKeywords = m_sSearch	= pPacket->ReadStringUTF8();
	else
		m_sKeywords = m_sSearch	= pPacket->ReadStringASCII();

	if ( ! CheckOverflow( m_sSearch ) )
	{
		// Overflow
		m_bDropMe = true;
		return TRUE;
	}

	while ( pPacket->GetRemaining() )
	{
		BYTE nPeek = pPacket->PeekByte();

		if ( nPeek == GGEP_MAGIC )
		{
			// GGEP extension
			ReadGGEP( pPacket );

			// Must be last ...
			if ( DWORD nLength = pPacket->GetRemaining() )
			{
				// ... but skip one extra null byte
				if ( nLength != 1 || pPacket->PeekByte() != 0 )
					theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with extra bytes after GGEP (%d bytes)"), pPacket->GetRemaining() );
			}

			break;
		}
		else if ( nPeek == 'u' )
		{
			// HUGE extensions
			pPacket->ReadHUGE( this );
		}
		else if ( nPeek == '<' || nPeek == '{' )
		{
			// XML extensions
			pPacket->ReadXML( m_pSchema, m_pXML );
		}
		else	// if ( nPeek == 0 || nPeek == G1_PACKET_HIT_SEP )
		{
			// Skip extra null or separate byte
			pPacket->ReadByte();
		}
	}

	// Legacy detection of "Whats New" search
	if ( m_sSearch.CompareNoCase( _T( WHAT_IS_NEW_QUERY_STRING ) ) == 0 )
	{
		m_sSearch.Empty();
		m_bWhatsNew = true;
	}

	m_bAndG1 = TRUE;

	return CheckValid( false );
}

void CQuerySearch::ReadGGEP(CG1Packet* pPacket)
{
	CGGEPBlock pGGEP;
	if ( pGGEP.ReadFromPacket( pPacket ) )
	{
		if ( ! Settings.Gnutella1.EnableGGEP )
			return;

		Hashes::Sha1Hash	oSHA1;
		Hashes::TigerHash	oTiger;
		Hashes::Ed2kHash	oED2K;
		Hashes::BtHash		oBTH;
		Hashes::Md5Hash		oMD5;

		CGGEPItem* pItemPos = pGGEP.GetFirst();
		for ( BYTE nItemCount = 0 ; pItemPos && nItemCount < pGGEP.GetCount() ;
			nItemCount++, pItemPos = pItemPos->m_pNext )
		{
			if ( pItemPos->IsNamed( GGEP_HEADER_HASH ) )
			{
				switch ( pItemPos->m_pBuffer[0] )
				{
				case GGEP_H_SHA1:
					if ( pItemPos->m_nLength == 20 + 1 )
					{
						oSHA1 = reinterpret_cast< Hashes::Sha1Hash::RawStorage& >(
							pItemPos->m_pBuffer[ 1 ] );
					}
					else
						theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with GGEP \"H\" type SH1 unknown size (%d bytes)"), pItemPos->m_nLength );
					break;

				case GGEP_H_BITPRINT:
					if ( pItemPos->m_nLength == 24 + 20 + 1 )
					{
						oSHA1 = reinterpret_cast< Hashes::Sha1Hash::RawStorage& >(
							pItemPos->m_pBuffer[ 1 ] );
						oTiger = reinterpret_cast< Hashes::TigerHash::RawStorage& >(
							pItemPos->m_pBuffer[ 21 ] );
					}
					else
						theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with GGEP \"H\" type SH1+TTR unknown size (%d bytes)"), pItemPos->m_nLength );
					break;

				case GGEP_H_MD5:
					if ( pItemPos->m_nLength == 16 + 1 )
					{
						oMD5 = reinterpret_cast< Hashes::Md5Hash::RawStorage& >(
							pItemPos->m_pBuffer[ 1 ] );
					}
					else
						theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with GGEP \"H\" type MD5 unknown size (%d bytes)"), pItemPos->m_nLength );
					break;

				case GGEP_H_MD4:
					if ( pItemPos->m_nLength == 16 + 1 )
					{
						oED2K = reinterpret_cast< Hashes::Ed2kHash::RawStorage& >(
							pItemPos->m_pBuffer[ 1 ] );
					}
					else
						theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with GGEP \"H\" type MD4 unknown size (%d bytes)"), pItemPos->m_nLength );
					break;

				case GGEP_H_UUID:
					// Unsupported
					break;

				default:
					theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with GGEP \"H\" unknown type %d (%d bytes)"), pItemPos->m_pBuffer[0], pItemPos->m_nLength );
				}
			}
			else if ( pItemPos->IsNamed( GGEP_HEADER_URN ) )
			{
				CString strURN( _T("urn:") + pItemPos->ToString() );
				if (      oSHA1.fromUrn(  strURN ) ) ;	// Got SHA1
				else if ( oTiger.fromUrn( strURN ) ) ;	// Got Tiger
				else if ( oED2K.fromUrn(  strURN ) ) ;	// Got ED2K
				else if ( oMD5.fromUrn(   strURN ) ) ;	// Got MD5
				else if ( oBTH.fromUrn(   strURN ) ) ;	// Got BTH base32
				else if ( oBTH.fromUrn< Hashes::base16Encoding >( strURN ) ) ;	// Got BTH base16
				else
					theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with GGEP \"u\" unknown URN: \"%s\" (%d bytes)"), strURN, pItemPos->m_nLength );
			}
			else if ( pItemPos->IsNamed( GGEP_HEADER_SECURE_OOB ) )
			{
				m_bOOBv3 = true;
			}
			else if ( pItemPos->IsNamed( GGEP_HEADER_META ) )
			{
				switch ( pItemPos->m_pBuffer[0] & 0xfc )
				{
				case GGEP_META_AUDIO:
					m_pSchema = SchemaCache.Get( CSchema::uriAudio );
					break;
				case GGEP_META_VIDEO:
					m_pSchema = SchemaCache.Get( CSchema::uriVideo );
					break;
				case GGEP_META_DOCUMENTS:
					m_pSchema = SchemaCache.Get( CSchema::uriDocument );
					break;
				case GGEP_META_IMAGES:
					m_pSchema = SchemaCache.Get( CSchema::uriImage );
					break;
				case GGEP_META_WINDOWS:
				case GGEP_META_UNIX:
					m_pSchema = SchemaCache.Get( CSchema::uriApplication );
					break;
				}
			}
			else if ( pItemPos->IsNamed( GGEP_HEADER_PARTIAL_RESULT_PREFIX ) )
			{
				m_bPartial = true;
			}
			else if ( pItemPos->IsNamed( GGEP_HEADER_NO_PROXY ) )
			{
				m_bNoProxy = true;
			}
			else if ( pItemPos->IsNamed( GGEP_HEADER_EXTENDED_QUERY ) )
			{
				m_bExtQuery = true;
			}
			else if ( pItemPos->IsNamed( GGEP_HEADER_FEATURE_QUERY ) )
			{
				m_bWhatsNew = true;
			}
			else
				theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with unknown GGEP \"%s\" (%d bytes)"), pItemPos->m_sID, pItemPos->m_nLength );
		}

		if ( oSHA1  && ! m_oSHA1 )  m_oSHA1  = oSHA1;
		if ( oTiger && ! m_oTiger ) m_oTiger = oTiger;
		if ( oED2K  && ! m_oED2K )  m_oED2K  = oED2K;
		if ( oBTH   && ! m_oBTH )   m_oBTH   = oBTH;
		if ( oMD5   && ! m_oMD5 )   m_oMD5   = oMD5;
	}
	else
	{
		DEBUG_ONLY( pPacket->Debug( _T("G1 Malformed GGEP.") ) );
	//	theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[G1] Got query packet with malformed GGEP") );
	}
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch from G2 packet

BOOL CQuerySearch::ReadG2Packet(CG2Packet* pPacket, const SOCKADDR_IN* pEndpoint)
{
	if ( ! pPacket->m_bCompound )
		return FALSE;

	G2_PACKET nType;
	DWORD nLength;

	m_bAndG1 = FALSE;

	while ( pPacket->ReadPacket( nType, nLength ) )
	{
		DWORD nOffset = pPacket->m_nPosition + nLength;

		if ( nType == G2_PACKET_QKY && nLength >= 4 )
		{
			if ( m_pEndpoint.sin_addr.S_un.S_addr == 0 && pEndpoint != NULL )
				m_pEndpoint = *pEndpoint;
			m_bUDP = ! Network.IsFirewalledAddress( &m_pEndpoint.sin_addr );

			m_nKey = pPacket->ReadLongBE();
			DWORD* pZero = (DWORD*)( pPacket->m_pBuffer + pPacket->m_nPosition - 4 );
			*pZero = 0;
		}
		else if ( nType == G2_PACKET_UDP && nLength >= 6 )
		{
			m_pEndpoint.sin_addr.S_un.S_addr = pPacket->ReadLongLE();
			m_pEndpoint.sin_port = htons( pPacket->ReadShortBE() );

			if ( m_pEndpoint.sin_addr.S_un.S_addr == 0 && pEndpoint != NULL )
				m_pEndpoint = *pEndpoint;
			m_bUDP = ! Network.IsFirewalledAddress( &m_pEndpoint.sin_addr );
			if ( m_bUDP ) m_pEndpoint.sin_family = PF_INET;

			if ( nLength >= 10 )
			{
				m_nKey = pPacket->ReadLongBE();
				DWORD* pZero = (DWORD*)( pPacket->m_pBuffer + pPacket->m_nPosition - 4 );
				*pZero = 0;
			}
		}
		else if ( nType == G2_PACKET_INTEREST )
		{
			m_bWantURL = m_bWantDN = m_bWantXML = m_bWantCOM = m_bWantPFS = FALSE;

			while ( nLength > 0 )
			{
				CString str = pPacket->ReadString( nLength );
				nLength -= str.GetLength() + 1;

				if ( str == _T("URL") )			m_bWantURL = TRUE;
				else if ( str == _T("DN") )		m_bWantDN  = TRUE;
				else if ( str == _T("SZ") )		m_bWantDN  = TRUE;	// Hack
				else if ( str == _T("MD") )		m_bWantXML = TRUE;
				else if ( str == _T("COM") )	m_bWantCOM = TRUE;
				else if ( str == _T("PFS") )	m_bWantPFS = TRUE;
			}
		}
		else if ( nType == G2_PACKET_URN )
		{
			CString strURN = pPacket->ReadString( nLength );
			if ( strURN.GetLength() + 1 >= (int)nLength )
				return FALSE;
			nLength -= strURN.GetLength() + 1;

			if ( nLength >= 20 && strURN == _T("sha1") )
			{
				pPacket->Read( m_oSHA1 );
			}
			else if ( nLength >= 44 && ( strURN == _T("bp") || strURN == _T("bitprint") ) )
			{
				pPacket->Read( m_oSHA1 );
				pPacket->Read( m_oTiger );
			}
			else if ( nLength >= 24 && ( strURN == _T("ttr") || strURN == _T("tree:tiger/") ) )
			{
				pPacket->Read( m_oTiger );
			}
			else if ( nLength >= 16 && strURN == _T("ed2k") )
			{
				pPacket->Read( m_oED2K );
			}
			else if ( nLength >= 20 && strURN == _T("btih") )
			{
				pPacket->Read( m_oBTH );
			}
			else if ( nLength >= 16 && strURN == _T("md5") )
			{
				pPacket->Read( m_oMD5 );
			}
		}
		else if ( nType == G2_PACKET_DESCRIPTIVE_NAME )
		{
			m_sSearch = pPacket->ReadString( nLength );
			m_sKeywords = m_sSearch;
			// These called from CheckValid at end of this function:
			//ToLower( m_sKeywords );
			//MakeKeywords( m_sKeywords, false );
		}
		else if ( nType == G2_PACKET_METADATA )
		{
			CString strXML = pPacket->ReadString( nLength );

			m_pXML->Delete();
			m_pXML = CXMLElement::FromString( strXML );
			m_pSchema = NULL;

			if ( m_pXML != NULL )
			{
				if ( CXMLAttribute *pURI = m_pXML->GetAttribute( CXMLAttribute::schemaName ) )
				{
					m_pSchema = SchemaCache.Get( pURI->GetValue() );
				}
				else if ( m_pSchema = SchemaCache.Guess( m_pXML->GetName() ) )
				{
					CXMLElement* pRoot = m_pSchema->Instantiate( TRUE );
					pRoot->AddElement( m_pXML );
					m_pXML = pRoot;
				}
			}
		}
		else if ( nType == G2_PACKET_SIZE_RESTRICTION )
		{
			if ( nLength == 8 )
			{
				m_nMinSize = pPacket->ReadLongBE();
				m_nMaxSize = pPacket->ReadLongBE();
				if ( m_nMaxSize == 0xFFFFFFFF ) m_nMaxSize = SIZE_UNKNOWN;
			}
			else if ( nLength == 16 )
			{
				m_nMinSize = pPacket->ReadInt64();
				m_nMaxSize = pPacket->ReadInt64();
			}
		}
		else if ( nType == G2_PACKET_G1 )
		{
			m_bAndG1 = TRUE;
		}
		else if ( nType == G2_PACKET_NAT_DESC )
		{
			m_bFirewall = TRUE;
		}

		pPacket->m_nPosition = nOffset;
	}

	if ( pPacket->GetRemaining() < 16 )
		return FALSE;

	pPacket->Read( m_oGUID );

	if ( ! m_oGUID )
		return FALSE;

	return CheckValid( true );
}

BOOL CQuerySearch::ReadDCPacket(CDCPacket* pPacket, const SOCKADDR_IN* pEndpoint)
{
	// Active user
	// $Search SenderIP:SenderPort (F|T)?(F|T)?Size?Type?String|
	// Passive user
	// $Search Hub:Nick (F|T)?(F|T)?Size?Type?String|

	if ( pPacket->GetRemaining() < 24 )		// Too short
	{
		theApp.Message( MSG_DEBUG | MSG_FACILITY_SEARCH, _T("[DC] Got too short query packet (%d bytes)"), pPacket->GetRemaining() );
		return FALSE;
	}

	if ( pEndpoint )
		m_pEndpoint = *pEndpoint;

	LPSTR szAddress = (LPSTR)&pPacket->m_pBuffer[ 8 ];
	pPacket->m_pBuffer[ pPacket->m_nLength - 1 ] = 0;	// ASCIIZ

	LPSTR szPort = strchr( szAddress, ':' );
	if ( ! szPort ) return FALSE;
	*szPort++ = 0;

	LPSTR szFlag1 = strchr( szPort, ' ' );
	if ( ! szFlag1 ) return FALSE;
	*szFlag1++ = 0;

	LPSTR szFlag2 = strchr( szFlag1, '?' );
	if ( ! szFlag2 ) return FALSE;
	*szFlag2++ = 0;

	LPSTR szSize = strchr( szFlag2, '?' );
	if ( ! szSize ) return FALSE;
	*szSize++ = 0;

	LPSTR szType = strchr( szSize, '?' );
	if ( ! szType ) return FALSE;
	*szType++ = 0;

	LPSTR szString = strchr( szType, '?' );
	if ( ! szString ) return FALSE;
	*szString++ = 0;

	IN_ADDR nAddress;
	nAddress.s_addr = inet_addr( szAddress );
	int nPort = atoi( szPort );
	if ( nAddress.s_addr == INADDR_NONE || nPort <= 0 || nPort > USHRT_MAX )
	{
		// Passive user request (send answer via TCP)
		m_bUDP = FALSE;
		m_sUserNick = UTF8Decode( szPort );
	}
	else
	{
		// Active user request (send answer via UDP)
		m_bUDP = TRUE;
		m_sUserNick.Empty();
		m_pEndpoint.sin_addr = nAddress;
		m_pEndpoint.sin_port = htons( (WORD)nPort );
	}

	bool bSizeRestriced = ( *szFlag1 == 'T' );
	bool bIsMaxSize = ( *szFlag2 == 'T' );
	if ( ( ! bSizeRestriced && *szFlag1 != 'F' ) ||
		 ( ! bIsMaxSize     && *szFlag2 != 'F' ) )
		return FALSE;
	QWORD nSize = 0;
	if ( sscanf_s( szSize, "%I64u", &nSize ) != 1 )
		return FALSE;
	int nType = atoi( szType );
	if ( nType < 1 )
		return FALSE;

	if ( bSizeRestriced )
	{
		if ( bIsMaxSize )
			m_nMaxSize = nSize;
		else
			m_nMinSize = nSize;
	}
	else if ( nSize )
	{
		m_nMinSize = m_nMaxSize = nSize;
	}

	if ( nType == 9 )	// Hash search
	{
		if ( _strnicmp( szString, "TTH:", 4 ) != 0 )
			return FALSE;	// Unknown hash prefix

		if ( ! m_oTiger.fromString( CA2W( szString + 4 ) ) )
			return FALSE;	// Invalid TigerTree hash encoding
	}
	else	// Keywords search
	{
		m_sSearch = UTF8Decode( szString );
		m_sSearch.Replace( _T('$'), _T(' ') );

		switch ( nType )
		{
		case 1:		// Any file
			break;
		case 2:		// Audio
			m_pSchema = SchemaCache.Get( CSchema::uriAudio );
			break;
		case 3:		// Archive
			m_pSchema = SchemaCache.Get( CSchema::uriArchive );
			break;
		case 4:		// Document
			m_pSchema = SchemaCache.Get( CSchema::uriDocument );
			break;
		case 5:		// Executable
			m_pSchema = SchemaCache.Get( CSchema::uriApplication );
			break;
		case 6:		// Image
			m_pSchema = SchemaCache.Get( CSchema::uriImage );
			break;
		case 7:		// Video
			m_pSchema = SchemaCache.Get( CSchema::uriVideo );
			break;
		case 8:		// Folders
			m_pSchema = SchemaCache.Get( CSchema::uriFolder );
			break;
		case 9:		// TTH (see above)
			break;
		case 10:	// DVD/CD
			m_pSchema = SchemaCache.Get( CSchema::uriROM );
			break;
		//default:	// Unknown
		//	;
		}
	}

	return CheckValid( true );
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch validity check

CQuerySearch::CHistoryMap CQuerySearch::m_oSearchHistory;

#define OVERFLOW_HISTORY	20
#define OVERFLOW_TIMEOUT	10	// Maximum one same search per 10 seconds

BOOL CQuerySearch::CheckOverflow(const CString& sSearch)
{
	if ( sSearch.IsEmpty() )
		return TRUE;

	DWORD tNow = static_cast< DWORD >( time( NULL ) );

	if ( m_oSearchHistory.GetCount() > OVERFLOW_HISTORY )
	{
		for ( const CHistoryMap::CPair* pData = m_oSearchHistory.PGetFirstAssoc() ; pData ; pData = m_oSearchHistory.PGetNextAssoc( pData ) )
		{
			if ( tNow > pData->value + OVERFLOW_TIMEOUT )
			{
				// In range
				m_oSearchHistory.RemoveKey( pData->key );
				break;
			}
		}
	}

	if ( const CHistoryMap::CPair* pData = m_oSearchHistory.PLookup( sSearch ) )
	{
		if ( tNow < pData->value + OVERFLOW_TIMEOUT )
		{
			// Too often
			m_oSearchHistory.SetAt( sSearch, tNow );
			return FALSE;
		}
	}

	m_oSearchHistory.SetAt( sSearch, tNow );
	return TRUE;
}

BOOL CQuerySearch::CheckValid(bool bExpression)
{
	BuildWordList( bExpression );

	// Searches by hash are ok
	if ( HasHash() )
	{
		if ( m_bWhatsNew )
			return FALSE;	// Invalid hashed "Whats New" search

		if ( m_oURNs.empty() )
		{
			if ( m_oSHA1 )
			{
				CString strurn = m_oSHA1.toUrn();
				m_oURNs.push_back( CQueryHashTable::HashWord( strurn, 0, strurn.GetLength(), 32 ) );
			}

			if ( m_oTiger )
			{
				CString strurn = m_oTiger.toUrn();
				m_oURNs.push_back( CQueryHashTable::HashWord( strurn, 0, strurn.GetLength(), 32 ) );
			}

			if ( m_oED2K )
			{
				CString strurn = m_oED2K.toUrn();
				m_oURNs.push_back( CQueryHashTable::HashWord( strurn, 0, strurn.GetLength(), 32 ) );
			}

			if ( m_oBTH )
			{
				CString strurn = m_oBTH.toUrn();
				m_oURNs.push_back( CQueryHashTable::HashWord( strurn, 0, strurn.GetLength(), 32 ) );
			}

			if ( m_oMD5 )
			{
				CString strurn = m_oMD5.toUrn();
				m_oURNs.push_back( CQueryHashTable::HashWord( strurn, 0, strurn.GetLength(), 32 ) );
			}
		}
	}

	if ( m_oKeywordHashList.size() )
		return TRUE;

	if ( ! m_oWords.empty() )
	{
		DWORD nValidWords = 0;
		DWORD nCommonWords = 0;

		// Check we aren't just searching for broad terms - set counters, etc
		for ( const_iterator pWord = begin() ; pWord != end() ; pWord++ )
		{
			TCHAR  szChar = *(pWord->first);
			size_t nLength = pWord->second;
			size_t nValidCharacters = 0;

			//bool bExtendChar = false;	// Clear the flag used for extended char
			// NOTE: Because of how oWord acts, each keyword in oWords gets sorted in ascending order with HEX code of char,
			// thus Extended chars are always located at end of oWords.  Therefore it is not necessary to Clear the flag inside the loop.

			// After the char inspection:
			if ( ! IsCharacter( szChar ) )	// Check if the char is valid
				continue;					// Do nothing here

			//if ( nLength > 8 )			// Longer than any common word, no further check needed
			//	return TRUE;

			if ( nLength > 3 )				// Any char string longer than 3 byte are counted.
			{
				nValidCharacters = nLength;
			}
			else if ( 0x00 <= szChar && 0x7f >= szChar )	// Check if the char is 1 byte length in UTF8 (non-char will not reach here)
			{
				nValidCharacters = nLength;
			}
			else if ( 0x80 <= szChar && 0x7ff >= szChar)	// Check if the char is 2 byte length in UTF8 (non-char will not reach here)
			{
				nValidCharacters = nLength * 2;
			}
			else if ( 0x3041 <= szChar && 0x30fe >= szChar ) // These regions are for Japanese Hiragana/Katakana chars (3Bytes).
			{												// Because of number of chars exist in that region they are counted as 2byte chars, making only 2 or longer chars accepted on Query.
				nValidCharacters = nLength * 2;
				//bExtendChar = true;						// Set Extended char flag
			}
			else if ( 0x800 <= szChar && 0xffff >= szChar)  // Check if the char is 3 byte length in UTF8 (non-char will not reach here)
			{
				nValidCharacters = nLength * 3;
				//bExtendChar = true;						// Set Extended char flag
			}
			else if ( nLength > 1 )
			{
				// char inspection
				bool bWord, bDigit, bMix;
				IsType( &szChar, 0, nLength, bWord, bDigit, bMix );
				if ( bWord || bMix )
					nValidCharacters = nLength;
			}

			// Check char is longer than 3byte in utf8 (Gnutella standard)
			if ( nValidCharacters < 2 )
				continue;

			if ( nValidCharacters > 8 )
			{
				// Lengthy assume valid keyword
				nValidWords++;
			}
			else if ( nValidCharacters == 2 )
			{
				// Use invalid 2chars as common term
				nCommonWords++;
			}
			else
			{
				// Set additional common keyword list (non-filetypes)
				static const CString strCommom = L"dvd xxx sex fuck porn";

				const CString strWord = CString( pWord->first, (int)pWord->second ).MakeLower();

				if ( SchemaCache.IsFilter( strWord ) || strCommom.Find( strWord ) >= 0 )
					nCommonWords++;		// Common term
				else if ( nValidCharacters == 3 )
					nCommonWords += 2;	// Count short valid keywords
				else
					nValidWords++;		// Count any other as valid keywords
			}

			DWORD nHash = CQueryHashTable::HashWord( pWord->first, 0, pWord->second, 32 );
			m_oKeywordHashList.push_back( nHash );

			// Obsolete:
			//static const LPCTSTR common[] =
			//{
			//	L"mp3", L"ogg", L"ac3", L"aac",
			//	L"jpg", L"gif", L"png", L"bmp", L"pdf",
			//	L"avi", L"mpg", L"mkv", L"wmv", L"mov", L"ogm",
			//	L"exe", L"zip", L"rar", L"iso", L"bin",
			//	L"dvd", L"divx", L"xvid", L"mpeg", L"mp4",
			//	L"xxx", L"sex", L"fuck", L"torrent"
			//};
			//static const size_t commonWords = sizeof common / sizeof common[ 0 ];
			//if ( std::find_if( common, common + commonWords, FindStr( *pWord ) ) != common + commonWords ) nCommonWords++;
		}

		if ( nValidWords || nCommonWords > 2 || ( m_pSchema && nCommonWords > 1 ) )
			return TRUE;
	}

	if ( HasHash() || m_bWhatsNew || Settings.Experimental.LAN_Mode )
		return TRUE;

	m_oKeywordHashList.clear();
	m_oWords.clear();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch matching

BOOL CQuerySearch::Match(LPCTSTR pszFilename, LPCTSTR pszSchemaURI, const CXMLElement* pXML, const CPeerProjectFile* pFile) const
{
	if ( pFile->m_nSize == SIZE_UNKNOWN ||
		 pFile->m_nSize < m_nMinSize ||
		 pFile->m_nSize > m_nMaxSize ||
		 *this != *pFile )
		return FALSE;

	if ( *this == *pFile )
		return TRUE;

	if ( pszSchemaURI && *pszSchemaURI )
	{
		if ( m_pSchema && ! m_pSchema->CheckURI( pszSchemaURI ) )
			return FALSE;

		if ( pXML )
		{
			TRISTATE bResult = MatchMetadata( pszSchemaURI, pXML );
			if ( bResult != TRI_UNKNOWN && ! Settings.Search.SchemaTypes )
				return ( bResult == TRI_TRUE );

			if ( ! m_sKeywords.IsEmpty() )
			{
				bool bReject = false;
				if ( MatchMetadataShallow( pszSchemaURI, pXML, &bReject ) )
				{
					// If searching in Local library return true
					if ( ! HasHash() && ! m_oSimilarED2K )
						return TRUE;

					// Otherwise, only return WordMatch when negative terms are used to filter out filenames from the search window
					BOOL bNegative = FALSE;
					if ( m_sKeywords.GetLength() > 1 )
					{
						int nMinusPos = -1;
						while ( ! bNegative )
						{
							nMinusPos = m_sKeywords.Find( '-', nMinusPos + 1 );
							if ( nMinusPos != -1 )
							{
								bNegative = ( IsCharacter( m_sKeywords.GetAt( nMinusPos + 1 ) ) != 0 );
								if ( nMinusPos > 0 )
									bNegative &= ( IsCharacter( m_sKeywords.GetAt( nMinusPos - 1 ) ) == 0 );
							}
							else
								break;
						}
					}
					return bNegative ? WordMatch( pszFilename, m_sKeywords ) : TRUE;
				}
				else if ( bReject )
					return FALSE;
			}
		}
	}

	// If it's a search for similar files, the text doesn't have to match
	if ( m_oSimilarED2K )
		return TRUE;

	return m_sKeywords.GetLength() && WordMatch( pszFilename, m_sKeywords );
}

TRISTATE CQuerySearch::MatchMetadata(LPCTSTR pszSchemaURI, const CXMLElement* pXML) const
{
	if ( ! m_pSchema || ! m_pXML )
		return TRI_UNKNOWN;
	if ( ! pszSchemaURI || ! *pszSchemaURI || ! pXML )
		return TRI_UNKNOWN;
	if ( ! m_pSchema->CheckURI( pszSchemaURI ) )
		return TRI_FALSE;

	int nCount = 0;
	const CXMLElement* pRoot = m_pXML->GetFirstElement();

	for ( POSITION pos = m_pSchema->GetMemberIterator() ; pos ; )
	{
		const CSchemaMember* pMember = m_pSchema->GetNextMember( pos );

		CString strSearch = pMember->GetValueFrom( pRoot );
		CString strTarget = pMember->GetValueFrom( pXML );

		if ( ! strSearch.IsEmpty() )
		{
			if ( strTarget.IsEmpty() )
				return TRI_FALSE;

			if ( pMember->m_bNumeric )
			{
				if ( ! NumberMatch( strTarget, strSearch ) )
					return TRI_FALSE;
			}
			else
			{
				if ( ! WordMatch( strTarget, strSearch ) )
					return TRI_FALSE;
			}

			nCount++;
		}
	}

	return ( nCount > 0 ) ? TRI_TRUE : TRI_UNKNOWN;
}

BOOL CQuerySearch::MatchMetadataShallow(LPCTSTR pszSchemaURI, const CXMLElement* pXML, bool* bReject) const
{
	if ( ! pXML || m_sSearch.IsEmpty() )
		return FALSE;

	if ( CSchemaPtr pSchema = SchemaCache.Get( pszSchemaURI ) )
	{
		for ( POSITION pos = pSchema->GetMemberIterator() ; pos ; )
		{
			CSchemaMember* pMember = pSchema->GetNextMember( pos );

			if ( pMember->m_bSearched )
			{
				CString strTarget = pMember->GetValueFrom( pXML, _T(""), FALSE );
				if ( WordMatch( strTarget, m_sKeywords, bReject ) )
					return TRUE;
				if ( bReject && *bReject )
					return FALSE;
			}
		}
	}
	else
	{
		for ( POSITION pos = pXML->GetAttributeIterator() ; pos ; )
		{
			const CXMLAttribute* pAttribute = pXML->GetNextAttribute( pos );

			CString strTarget = pAttribute->GetValue();

			if ( WordMatch( strTarget, m_sKeywords, bReject ) )
				return TRUE;
			if ( bReject && *bReject )
				return FALSE;
		}
	}

	return FALSE;
}

BOOL CQuerySearch::WordMatch(LPCTSTR pszString, LPCTSTR pszFind, bool* bReject)
{
	LPCTSTR pszWord	= pszFind;
	LPCTSTR pszPtr	= pszFind;
	BOOL bQuote		= FALSE;
	BOOL bNegate	= FALSE;
	BOOL bSpace		= TRUE;
	int nCount		= 0;

	for ( ; *pszPtr ; pszPtr++ )
	{
		if ( ( bQuote && *pszPtr == '\"' ) || ( ! bQuote && ( *pszPtr <= ' ' || *pszPtr == '\t' || *pszPtr == '-' || *pszPtr == '\"' ) ) )
		{
			if ( pszWord < pszPtr )
			{
				if ( bNegate )
				{
					if ( _tcsnistr( pszString, pszWord, pszPtr - pszWord ) )
					{
						if ( bReject )
							*bReject = true;
						return FALSE;
					}
				}
				else
				{
					if ( ! _tcsnistr( pszString, pszWord, pszPtr - pszWord ) )
						return FALSE;
				}

				nCount++;
			}

			pszWord	= pszPtr + 1;

			if ( *pszPtr == '\"' )
			{
				bQuote = ! bQuote;
				bSpace = TRUE;
			}
			else if ( *pszPtr == '-' && pszPtr[1] != ' ' && bSpace && ! bQuote )
			{
				bNegate = TRUE;
				bSpace = FALSE;
			}
			else
			{
				bSpace = ( *pszPtr == ' ' );
			}

			if ( bNegate && ! bQuote && *pszPtr != '-' )
				bNegate = FALSE;
		}
		else
		{
			bSpace = FALSE;
		}
	}

	if ( pszWord < pszPtr )
	{
		if ( bNegate )
		{
			if ( _tcsnistr( pszString, pszWord, pszPtr - pszWord ) )
			{
				if ( bReject )
					*bReject = true;
				return FALSE;
			}
		}
		else
		{
			if ( ! _tcsnistr( pszString, pszWord, pszPtr - pszWord ) )
				return FALSE;
		}

		nCount++;
	}

	return nCount > 0;
}

BOOL CQuerySearch::NumberMatch(const CString& strValue, const CString& strRange)
{
	double nValue, nMinimum, nMaximum = 0;

	if ( _stscanf( strValue, _T("%lf"), &nValue ) != 1 )
		return FALSE;

	const int nPos = strRange.Find( '-' );

	if ( nPos < 0 )
		return _stscanf( strRange, _T("%lf"), &nMinimum ) == 1 && nValue == nMinimum;

	if ( nPos == 0 )
		return _stscanf( (LPCTSTR)strRange + 1, _T("%lf"), &nMaximum ) && nValue <= nMaximum;

	if ( nPos == strRange.GetLength() - 1 )
		return _stscanf( strRange, _T("%lf"), &nMinimum ) && nValue >= nMinimum;

	if ( _stscanf( strRange.Left( nPos ), _T("%lf"), &nMinimum ) != 1 ||
		 _stscanf( strRange.Mid( nPos + 1 ), _T("%lf"), &nMaximum ) != 1 )
		return FALSE;

	return nValue >= nMinimum && nValue <= nMaximum;
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch word list builder

void CQuerySearch::BuildWordList(bool bExpression, bool /*bLocal*/ )
{
	m_sSearch.Trim();
	ToLower( m_sSearch );

	// Parse "download-like" searches
	if ( 0 == _tcsncmp( m_sSearch, _T("magnet:?"), 8 ) )
	{
		CPeerProjectURL pURL;
		if ( pURL.Parse( m_sSearch, FALSE ) )
		{
			m_sSearch = pURL.m_sName;
			if ( m_nMinSize == SIZE_UNKNOWN && m_nMaxSize == SIZE_UNKNOWN &&
				pURL.m_nSize != SIZE_UNKNOWN && pURL.m_nSize != 0 )
				m_nMinSize = m_nMaxSize = m_nSize;
			if ( ! m_oSHA1 && pURL.m_oSHA1 )
				m_oSHA1 = pURL.m_oSHA1;
			if ( ! m_oTiger && pURL.m_oTiger )
				m_oTiger = pURL.m_oTiger;
			if ( ! m_oED2K && pURL.m_oED2K )
				m_oED2K = pURL.m_oED2K;
			if ( ! m_oBTH && pURL.m_oBTH )
				m_oBTH = pURL.m_oBTH;
			if ( ! m_oMD5 && pURL.m_oMD5 )
				m_oMD5 = pURL.m_oMD5;
		}
	}

	// Parse searches started from (multiple) URN string(s)
	while ( 0 == _tcsncmp( m_sSearch, _T("urn:"), 4 ) )
	{
		BOOL bHash = FALSE;
		if ( ! m_oSHA1 )
		{
			if ( m_oSHA1.fromUrn( m_sSearch ) )
				bHash = TRUE;
			else
				m_oSHA1.clear();
		}
		if ( ! m_oTiger )
		{
			if ( m_oTiger.fromUrn( m_sSearch ) )
				bHash = TRUE;
			else
				m_oTiger.clear();
		}
		if ( ! m_oED2K )
		{
			if ( m_oED2K.fromUrn( m_sSearch ) )
				bHash = TRUE;
			else
				m_oED2K.clear();
		}
		if ( ! m_oBTH )
		{
			if ( m_oBTH.fromUrn( m_sSearch ) ||
				 m_oBTH.fromUrn< Hashes::base16Encoding >( m_sSearch ) )
				bHash = TRUE;
			else
				m_oBTH.clear();
		}
		if ( ! m_oMD5 )
		{
			if ( m_oMD5.fromUrn( m_sSearch ) )
				bHash = TRUE;
			else
				m_oMD5.clear();
		}
		if ( ! bHash )
			break;
		int nFirstSpace = m_sSearch.Find( _T(" ") );
		if ( nFirstSpace != -1 )
		{
			m_sSearch = m_sSearch.Mid( nFirstSpace + 1 );
			m_sSearch.Trim();
		}
		else
		{
			m_sSearch.Empty();
			break;
		}
	}

	// Split search string to keywords
	CString strKeywords = m_sSearch;

	// Split metadata to keywords
	if ( m_pXML && m_pSchema )
	{
		CString strWords = m_pSchema->GetIndexedWords( m_pXML->GetFirstElement() );
		if ( ! strWords.IsEmpty() )
		{
			if ( ! strKeywords.IsEmpty() )
				strKeywords += _T(" ");
			strKeywords += strWords;
		}
	}
	strKeywords.TrimRight();

	m_sKeywords = MakeKeywords( strKeywords, bExpression );

	// Build word pos/neg tables (m_oWords/m_oNegWords) from m_sKeywords
	BuildWordTable( m_sKeywords, m_oWords, m_oNegWords );

	// Build m_sPosKeywords/m_sG2Keywords from m_oWords/m_oNegWords

	// Clear QueryStrings.
	m_sPosKeywords.Empty();
	m_sG2Keywords.Empty();

	// Create string with positive keywords.
	for ( const_iterator pWord = begin() ; pWord != end() ; ++pWord )
	{
		m_sPosKeywords.Append( pWord->first, int(pWord->second) );
		m_sPosKeywords += _T(' ');
	}

	m_sG2Keywords = m_sPosKeywords;	// Copy Positive keywords string to G2 keywords string.
	m_sPosKeywords.TrimRight();		// Trim off extra space char at the end of string.

	// Append negative keywords to G2 keywords string.
	for ( const_iterator pWord = beginNeg() ; pWord != endNeg() ; ++pWord )
	{
		m_sG2Keywords += _T('-');
		m_sG2Keywords.Append( pWord->first, int(pWord->second) );
		m_sG2Keywords += _T(' ');
	}
	m_sG2Keywords.TrimRight();		// Trim off extra space char at the end of string.
}


//////////////////////////////////////////////////////////////////////
// CQuerySearch serialization

#define QUERYSEARCH_SER_VERSION		1000	// 8
// nVersion History:
// 8 - Added m_nMinSize & m_nMaxSize - Shareaza 2.2 (ryo-oh-ki)
// 1000 - Added m_nProtocol for display (PeerProject 1.0) (8)

void CQuerySearch::Serialize(CArchive& ar)
{
	int nVersion = QUERYSEARCH_SER_VERSION;	// ToDo: INTERNAL_VERSION

	CString strURI;

	if ( ar.IsStoring() )
	{
		ar << nVersion;

		ar.Write( &m_oGUID[ 0 ], Hashes::Guid::byteCount );

		ar << m_sSearch;

		SerializeOut( ar, m_oSHA1 );
		SerializeOut( ar, m_oTiger );
		SerializeOut( ar, m_oED2K );
		SerializeOut( ar, m_oBTH );
		SerializeOut( ar, m_oMD5 );

		if ( m_pSchema != NULL && m_pXML != NULL )
		{
			ar << m_pSchema->GetURI();
			m_pXML->Serialize( ar );
		}
		else
		{
			ar << strURI;
		}

		ar << m_bWantURL;
		ar << m_bWantDN;
		ar << m_bWantXML;
		ar << m_bWantCOM;
		ar << m_bWantPFS;
		ar << m_nMinSize;
		ar << m_nMaxSize;

		ar << m_nProtocol;
	}
	else // Loading
	{
		ar >> nVersion;
		if ( nVersion < 8 )
			AfxThrowUserException();

		ReadArchive( ar, &m_oGUID[ 0 ], Hashes::Guid::byteCount );

		ar >> m_sSearch;

		SerializeIn( ar, m_oSHA1, nVersion );
		SerializeIn( ar, m_oTiger, nVersion );
		SerializeIn( ar, m_oED2K, nVersion );
		SerializeIn( ar, m_oBTH, nVersion );
		SerializeIn( ar, m_oMD5, nVersion );

		ar >> strURI;

		if ( ! strURI.IsEmpty() )
		{
			m_pSchema = SchemaCache.Get( strURI );
			m_pXML = new CXMLElement();
			m_pXML->Serialize( ar );
		}

		ar >> m_bWantURL;
		ar >> m_bWantDN;
		ar >> m_bWantXML;
		ar >> m_bWantCOM;
		ar >> m_bWantPFS;

		//if ( nVersion >= 8 )
		//{
			ar >> m_nMinSize;
			ar >> m_nMaxSize;
		//}

		//if ( nVersion >= 1000 )
			ar >> m_nProtocol;

		BuildWordList();
	}
}

//////////////////////////////////////////////////////////////////////
// CQuerySearch open window

CSearchWnd* CQuerySearch::OpenWindow(CQuerySearch* pSearch)
{
	if ( pSearch && pSearch->CheckValid( false ) )
		return new CSearchWnd( pSearch );

	return NULL;
}

void CQuerySearch::PrepareCheck()
{
	m_oWords.clear();
	m_oNegWords.clear();
	m_oURNs.clear();
	m_oKeywordHashList.clear();
	m_sKeywords.Empty();
}

void CQuerySearch::SearchHelp()
{
	static int nLastSearchHelp = 0;
	switch ( ++nLastSearchHelp )
	{
	case 1:  CHelpDlg::Show( _T("SearchHelp.BadSearch1") );
		break;
	case 2:  CHelpDlg::Show( _T("SearchHelp.BadSearch2") );
		break;
	default: CHelpDlg::Show( _T("SearchHelp.BadSearch3") );
		nLastSearchHelp = 0;
	}
}

CString CQuerySearch::BuildRegExp(const CString& strPattern) const
{
	CString strFilter;
	bool bChanged = false;
	int nTotal = 0;

	// <%>, <$>, <_> - Insert all query keywords.
	// <1>...<9> - Insert query keyword number 1-9.
	// <> - Insert next query keyword.

	for ( LPCTSTR pszPattern = strPattern ; *pszPattern ; ++pszPattern )
	{
		LPCTSTR pszLt = _tcschr( pszPattern, _T('<') );
		if ( pszLt )
		{
			bChanged = true;

			int nLength = pszLt - pszPattern;
			if ( nLength )
				strFilter.Append( pszPattern, nLength );

			pszPattern = pszLt + 1;
			bool bEnds = false;
			bool bAll = ( *pszPattern == '%' || *pszPattern == '$' || *pszPattern == '_' );
			bool bNumber = bAll ? false : ( *pszPattern >= '1' && *pszPattern <= '9' );

			// ToDo: Check only after single char?
			for ( ; *pszPattern ; pszPattern++ )
			{
				if ( *pszPattern == '>' )
				{
					bEnds = true;
					break;
				}
			}

			if ( bEnds )	// Closed bracket
			{
				if ( bAll )		// <%>,<$>,<_>,<>
				{
					// Add all keywords at the "<%>,<$>,<_>" position
					for ( CQuerySearch::const_iterator i = begin() ; i != end() ; ++i )
					{
						strFilter.AppendFormat( L"%s\\s*",
							CString( i->first, (int)( i->second ) ) );
					}
				}
				else if ( bNumber )		// <1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>
				{
					pszPattern--;	// Go back
					int nNumber = 0, nWord = 1;

					// Numbers from 1 to 9, no more
					if ( _stscanf( &pszPattern[0], L"%i", &nNumber ) != 1 )
						nNumber = ++nTotal;

					// Add specified keyword at the "< >" position
					for ( CQuerySearch::const_iterator i = begin() ; i != end() ; ++i, ++nWord )
					{
						if ( nWord == nNumber )
						{
							strFilter.AppendFormat( L"%s\\s*", CString( i->first, (int)( i->second ) ) );
							break;
						}
					}
					pszPattern++;	// Return to the last position
				}
			}
			else // No closing '>'
			{
				strFilter.Empty();
				break;
			}
		}
		else // No replacing
		{
			strFilter += pszPattern;
			break;
		}
	}

	return bChanged ? strFilter : strPattern;
}