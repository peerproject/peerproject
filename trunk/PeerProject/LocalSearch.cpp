//
// LocalSearch.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
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
#include "PeerProject.h"
#include "Settings.h"
#include "LocalSearch.h"
#include "QueryHit.h"

#include "Library.h"
#include "LibraryFolders.h"
#include "SharedFile.h"
#include "SharedFolder.h"
#include "AlbumFolder.h"

#include "QuerySearch.h"
#include "GProfile.h"
#include "Network.h"
#include "Neighbours.h"
#include "DCNeighbour.h"
#include "Neighbour.h"
#include "Datagrams.h"
#include "G1Packet.h"
#include "G2Packet.h"
#include "DCPacket.h"
#include "Buffer.h"
#include "ZLib.h"
#include "GGEP.h"
#include "BTClients.h"

#include "Transfers.h"
#include "Downloads.h"
#include "Download.h"
#include "Uploads.h"
#include "UploadQueue.h"
#include "UploadQueues.h"
#include "ImageServices.h"

#include "XML.h"
#include "Schema.h"
#include "SchemaCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

//////////////////////////////////////////////////////////////////////
// CLocalSearch construction

CLocalSearch::CLocalSearch(CQuerySearch* pSearch, PROTOCOLID nProtocol)
	: m_pSearch		( pSearch )
	, m_pEndpoint	( pSearch->m_pEndpoint )
	, m_pBuffer		( NULL )
	, m_bUDP		( TRUE )
	, m_nProtocol	( nProtocol )	// PROTOCOL_G2
{
}

CLocalSearch::CLocalSearch(CQuerySearch* pSearch, const CNeighbour* pNeighbour)
	: m_pSearch		( pSearch )
	, m_pEndpoint	( pNeighbour->m_pHost )
	, m_pBuffer		( NULL )
	, m_bUDP		( FALSE )
	, m_nProtocol	( pNeighbour->m_nProtocol )
{
}

CLocalSearch::CLocalSearch(CQuerySearch* pSearch, CBuffer* pBuffer, PROTOCOLID nProtocol)
	: m_pSearch		( pSearch )
	, m_pEndpoint	( )
	, m_pBuffer		( pBuffer )
	, m_bUDP		( FALSE )
	, m_nProtocol	( nProtocol )
{
}


//////////////////////////////////////////////////////////////////////
// CLocalSearch

template<>
bool CLocalSearch::IsValidForHit< CDownload >(const CDownload* pDownload) const
{
	return
		// If download is shareable, active, and matches:
		pDownload->IsShared() && ( pDownload->IsTorrent() || pDownload->IsStarted() ) &&
		m_pSearch->Match( pDownload->GetSearchName(), NULL, NULL, pDownload );
}

template<>
bool CLocalSearch::IsValidForHit< CLibraryFile >(const CLibraryFile* pFile) const
{
	switch ( m_nProtocol )
	{
	case PROTOCOL_G1:
		// Browse request, or real file (not ghost)
		return Settings.Gnutella1.Enabled &&
			( ! m_pSearch || pFile->IsAvailable() );
	case PROTOCOL_G2:
		// Browse request, or comments request, or real file (not ghost)
		return Settings.Gnutella2.Enabled &&
			( ! m_pSearch || m_pSearch->m_bWantCOM || pFile->IsAvailable() );
	case PROTOCOL_DC:
		// Real file (not ghost)
		return Settings.DC.Enabled &&
			pFile->IsAvailable();
	default:
		ASSERT( FALSE );
		return false;
	}
}

	// Note IsValidForHit for PROTOCOL_G1:
	// Check that a free queue exists that can upload this file.
	//&& ( UploadQueues.QueueRank( PROTOCOL_HTTP, pFile ) <= Settings.Gnutella1.HitQueueLimit );  // Causes Deadlock?
	// NOTE: Very CPU intensive operation!
	// Normally this isn't a problem, default queue length is 8-10, so this check (50) will never be activated.
	// However, sometimes users configure bad settings, such as a 2000 user HTTP queue.
	// Although the remote client should handle this by itself,
	// give Gnutella some protection against extreme settings (to reduce un-necessary traffic.)


//////////////////////////////////////////////////////////////////////
// CLocalSearch execute

bool CLocalSearch::Execute(INT_PTR nMaximum, bool bPartial, bool bShared)
{
	ASSERT( bPartial || bShared );

	if ( nMaximum < 0 )
		nMaximum = Settings.Gnutella.MaxHits;

	if ( m_pSearch )
		m_oGUID = m_pSearch->m_oGUID;
	else
		Network.CreateID( m_oGUID );

	INT_PTR nHits = 0;
	if ( bPartial )
	{
		if ( ! ExecutePartialFiles( nMaximum, nHits ) )
			return false;
	}

	if ( bShared && ( ! nMaximum || nHits < nMaximum ) )
	{
		if ( ! ExecuteSharedFiles( nMaximum, nHits ) )
			return false;
	}

	ASSERT( ! nMaximum || nHits <= nMaximum );

	return true;
}

//////////////////////////////////////////////////////////////////////
// CLocalSearch execute partial files

bool CLocalSearch::ExecutePartialFiles(INT_PTR nMaximum, INT_PTR& nHits)
{
	CSingleLock pLock( &Transfers.m_pSection );
	if ( ! pLock.Lock( 250 ) )
		return false;

	// Browse request, or no partials requested, or non Gnutella 2 request
	if ( ! m_pSearch || ! m_pSearch->m_bWantPFS || m_nProtocol != PROTOCOL_G2 )
		return true;

	CList< const CDownload* > oFilesInPacket;

	for ( POSITION pos = Downloads.GetIterator() ;
		pos && ( ! nMaximum || ( nHits + oFilesInPacket.GetCount() < nMaximum ) ); )
	{
		const CDownload* pDownload = Downloads.GetNext( pos );

		if ( IsValidForHit( pDownload ) )
			oFilesInPacket.AddTail( pDownload );

		// Obsolete for reference:
		//	if ( ( Settings.Gnutella.HitsPerPacket && (DWORD)oFilesInPacket.GetCount() >= Settings.Gnutella.HitsPerPacket ) ||
		//		( m_pPacket && m_pPacket->m_nLength >= MAX_QUERY_PACKET_SIZE ) )
		//	{
		//		nHits += SendHits( oFilesInPacket );	// Packet full, send it
		//		oFilesInPacket.RemoveAll();
		//	}
	}

	SendHits( oFilesInPacket );

	nHits += oFilesInPacket.GetCount();

	return true;
}

//////////////////////////////////////////////////////////////////////
// CLocalSearch execute shared files

bool CLocalSearch::ExecuteSharedFiles(INT_PTR nMaximum, INT_PTR& nHits)
{
	CSingleLock oLock( &Library.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return false;

	auto_ptr< CFileList > pFiles( Library.Search(
		m_pSearch, nMaximum, FALSE, m_nProtocol != PROTOCOL_G2 ) );	// Ghost files only for G2

	if ( ! pFiles.get() )
		return true;	// No files found

	CFileList oFilesInPacket;

	for ( POSITION pos = pFiles->GetHeadPosition() ;
		pos && ( ! nMaximum || ( nHits + oFilesInPacket.GetCount() < nMaximum ) ) ; )
	{
		const CLibraryFile* pFile = pFiles->GetNext( pos );

		// Select valid files
		if ( IsValidForHit( pFile ) )
			oFilesInPacket.AddTail( pFile );

		// Obsolete for reference:
		//	if ( ( Settings.Gnutella.HitsPerPacket && (DWORD)oFilesInPacket.GetCount() >= Settings.Gnutella.HitsPerPacket ) ||
		//		( m_pPacket && m_pPacket->m_nLength >= MAX_QUERY_PACKET_SIZE ) )
		//	{
		//		nHits += SendHits( oFilesInPacket );	// Packet full, send it
		//		oFilesInPacket.RemoveAll();
		//	}
	}

	SendHits( oFilesInPacket );

	nHits += oFilesInPacket.GetCount();

	return true;
}

template< typename T >
void CLocalSearch::SendHits(const CList< const T * >& oFiles)
{
	CPacket* pPacket = NULL;
	CSchemaMap pSchemas;

	BYTE nHits = 0;
	for ( POSITION pos = oFiles.GetHeadPosition() ; pos ; )
	{
		if ( ! pPacket )
			pPacket = CreatePacket();

		AddHit( pPacket, pSchemas, oFiles.GetNext( pos ), nHits ++ );

		// Send full packet
		if ( m_nProtocol == PROTOCOL_DC ||	// One hit per packet in DC++ protocol
			nHits >= Settings.Gnutella.HitsPerPacket ||
			pPacket->m_nLength >= MAX_QUERY_PACKET_SIZE )
		{
			WriteTrailer( pPacket, pSchemas, nHits );
			DispatchPacket( pPacket );
			pPacket = NULL;
			nHits = 0;
		}
	}

	if ( nHits )
	{
		WriteTrailer( pPacket, pSchemas, nHits );
		DispatchPacket( pPacket );
		pPacket = NULL;
	}

	ASSERT( pPacket == NULL );
	ASSERT( pSchemas.IsEmpty() );
}

//////////////////////////////////////////////////////////////////////
// CLocalSearch add library file hit

template<>
void CLocalSearch::AddHit< CLibraryFile >(CPacket* pPacket, CSchemaMap& pSchemas, const CLibraryFile* pFile, int nIndex)
{
	ASSERT( pPacket != NULL );

	switch ( m_nProtocol )
	{
	case PROTOCOL_G1:
		AddHitG1( static_cast< CG1Packet* >( pPacket ), pSchemas, pFile, nIndex );
		break;
	case PROTOCOL_G2:
		AddHitG2( static_cast< CG2Packet* >( pPacket ), pSchemas, pFile, nIndex );
		break;
	case PROTOCOL_DC:
		AddHitDC( static_cast< CDCPacket* >( pPacket ), pSchemas, pFile, nIndex );
		break;
	default:
		ASSERT( FALSE );
	}
}

void CLocalSearch::AddHitG1(CG1Packet* pPacket, CSchemaMap& pSchemas, CLibraryFile const * const pFile, int nIndex)
{
	pPacket->WriteLongLE( pFile->m_nIndex );
	pPacket->WriteLongLE( (DWORD)min( pFile->GetSize(), 0xFFFFFFFF ) );
	if ( Settings.Gnutella1.QueryHitUTF8 ) // Support UTF-8 Query
		pPacket->WriteStringUTF8( pFile->m_sName );
	else
		pPacket->WriteString( pFile->m_sName );

	if ( pFile->m_oSHA1 )
	{
		CString strHash = pFile->m_oSHA1.toUrn();
		pPacket->WriteString( strHash );

		//CGGEPBlock pBlock;

		//CGGEPItem* pItem = pBlock.Add( GGEP_HEADER_HASH );
		//pItem->WriteByte( 1 );
		//pItem->Write( &pFile->m_pSHA1, 20 );

		//pBlock.Write( pPacket );
		//pPacket->WriteByte( 0 );
	}
	else if ( pFile->m_oTiger )
	{
		CString strHash = pFile->m_oTiger.toUrn();
		pPacket->WriteString( strHash );
	}
	else if ( pFile->m_oED2K )
	{
		CString strHash = pFile->m_oED2K.toUrn();
		pPacket->WriteString( strHash );
	}
	else if ( pFile->m_oBTH )
	{
		CString strHash = pFile->m_oBTH.toUrn();
		pPacket->WriteString( strHash );
	}
	else if ( pFile->m_oMD5 )
	{
		CString strHash = pFile->m_oMD5.toUrn();
		pPacket->WriteString( strHash );
	}
	else
	{
		pPacket->WriteByte( 0 );
	}

	// Add Metadata
	if ( pFile->m_pSchema != NULL && pFile->m_pMetadata != NULL && ( ! m_pSearch || m_pSearch->m_bWantXML ) )
	{
		CXMLElement* pGroup = NULL;
		if ( ! pSchemas.Lookup( pFile->m_pSchema, pGroup ) )
		{
			pGroup = pFile->m_pSchema->Instantiate();
			pSchemas.SetAt( pFile->m_pSchema, pGroup );
		}

		CString strIndex;
		strIndex.Format( _T("%lu"), nIndex );

		CXMLElement* pXML = pFile->m_pMetadata->Clone();
		pXML->AddAttribute( _T("index"), strIndex );
		pGroup->AddElement( pXML );
	}
}

void CLocalSearch::AddHitG2(CG2Packet* pPacket, CSchemaMap& /*pSchemas*/, CLibraryFile const * const pFile, int /*nIndex*/)
{
	// Pass 1: Calculate child group size
	// Pass 2: Write the child packet
	DWORD nGroup = 0;
	bool bCalculate = false;
	do
	{
		bCalculate = ! bCalculate;

		if ( ! bCalculate )
			pPacket->WritePacket( G2_PACKET_HIT_DESCRIPTOR, nGroup, TRUE );

		if ( pFile->m_oTiger && pFile->m_oSHA1 )
		{
			const char prefix[] = "bp";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::Sha1Hash::byteCount + Hashes::TigerHash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::Sha1Hash::byteCount + Hashes::TigerHash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pFile->m_oSHA1 );
				pPacket->Write( pFile->m_oTiger );
			}
		}
		else if ( pFile->m_oTiger )
		{
			const char prefix[] = "ttr";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::TigerHash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::TigerHash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pFile->m_oTiger );
			}
		}
		else if ( pFile->m_oSHA1 )
		{
			const char prefix[] = "sha1";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::Sha1Hash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::Sha1Hash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pFile->m_oSHA1 );
			}
		}

		if ( pFile->m_oED2K )
		{
			const char prefix[] = "ed2k";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::Ed2kHash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::Ed2kHash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pFile->m_oED2K );
			}
		}

		if ( pFile->m_oBTH )
		{
			const char prefix[] = "btih";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::BtHash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::BtHash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pFile->m_oBTH );
			}
		}

		if ( pFile->m_oMD5 )
		{
			const char prefix[] = "md5";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::Md5Hash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::Md5Hash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pFile->m_oMD5 );
			}
		}

		if ( ! m_pSearch || m_pSearch->m_bWantDN )
		{
			if ( pFile->GetSize() <= 0xFFFFFFFF )
			{
				if ( bCalculate )
				{
					nGroup += G2_PACKET_LEN( G2_PACKET_DESCRIPTIVE_NAME, sizeof( DWORD ) + pPacket->GetStringLen( pFile->m_sName ) );
				}
				else
				{
					pPacket->WritePacket( G2_PACKET_DESCRIPTIVE_NAME, sizeof( DWORD ) + pPacket->GetStringLen( pFile->m_sName ) );
					pPacket->WriteLongBE( (DWORD)pFile->GetSize() );
					pPacket->WriteString( pFile->m_sName, FALSE );
				}
			}
			else // size = 0xFFFFFFFF
			{
				if ( bCalculate )
				{
					nGroup += G2_PACKET_LEN( G2_PACKET_SIZE, sizeof( QWORD ) ) +
						G2_PACKET_LEN( G2_PACKET_DESCRIPTIVE_NAME, pPacket->GetStringLen( pFile->m_sName ) );
				}
				else
				{
					pPacket->WritePacket( G2_PACKET_SIZE, sizeof( QWORD ) );
					pPacket->WriteInt64( pFile->GetSize() );
					pPacket->WritePacket( G2_PACKET_DESCRIPTIVE_NAME, pPacket->GetStringLen( pFile->m_sName ) );
					pPacket->WriteString( pFile->m_sName, FALSE );
				}
			}

			if ( LPCTSTR pszType = _tcsrchr( pFile->m_sName, '.' ) )
			{
				if ( _tcsicmp( pszType, _T(".co") ) == 0 ||
					 _tcsicmp( pszType, _T(".collection") ) == 0 ||
					 _tcsicmp( pszType, _T(".emulecollection") ) == 0 )
				{
					if ( ! pFile->m_bBogus )
					{
						if ( bCalculate )
							nGroup += G2_PACKET_LEN( G2_PACKET_COLLECTION, 0 );
						else
							pPacket->WritePacket( G2_PACKET_COLLECTION, 0 );
					}
				}
			}
		}

		if ( ! m_pSearch || m_pSearch->m_bWantURL )
		{
			if ( bCalculate )
				nGroup += G2_PACKET_LEN( G2_PACKET_URL, 0 );
			else
				pPacket->WritePacket( G2_PACKET_URL, 0 );

			if ( INT_PTR nCount = pFile->m_pSources.GetCount() )
			{
				if ( bCalculate )
				{
					nGroup += G2_PACKET_LEN( G2_PACKET_CACHED_SOURCES, sizeof( WORD ) );
				}
				else
				{
					pPacket->WritePacket( G2_PACKET_CACHED_SOURCES, sizeof( WORD ) );
					pPacket->WriteShortBE( (WORD)nCount );
				}
			}

			if ( Settings.Uploads.SharePreviews &&
				( pFile->m_bCachedPreview || ( Settings.Uploads.DynamicPreviews &&
				CImageServices::IsFileViewable( (LPCTSTR)pFile->m_sName ) ) ) )
			{
				if ( bCalculate )
					nGroup += G2_PACKET_LEN( G2_PACKET_PREVIEW_URL, 0 );
				else
					pPacket->WritePacket( G2_PACKET_PREVIEW_URL, 0 );
			}
		}

		if ( pFile->m_pMetadata != NULL && ( ! m_pSearch || m_pSearch->m_bWantXML ) )
		{
			CString strMetadata = pFile->m_pMetadata->ToString();
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_METADATA, pPacket->GetStringLen( strMetadata ) );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_METADATA, pPacket->GetStringLen( strMetadata ) );
				pPacket->WriteString( strMetadata, FALSE );
			}
		}

		{
			CQuickLock pQueueLock( UploadQueues.m_pSection );
			CUploadQueue* pQueue = UploadQueues.SelectQueue( PROTOCOL_HTTP, pFile );
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_GROUP_ID, sizeof( BYTE ) );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_GROUP_ID, sizeof( BYTE ) );
				pPacket->WriteByte( BYTE( pQueue ? pQueue->m_nIndex + 1 : 0 ) );
			}
		}

		if ( ! m_pSearch || m_pSearch->m_bWantCOM )
		{
			if ( pFile->IsRated() )
			{
				CString strComment;
				if ( pFile->m_nRating > 0 )
					strComment.Format( _T("<comment rating=\"%i\">"), pFile->m_nRating - 1 );
				else
					strComment = _T("<comment>");
				CXMLNode::ValueToString( pFile->m_sComments, strComment );
				if ( strComment.GetLength() > 2048 ) strComment = strComment.Left( 2048 );
				strComment += _T("</comment>");
				strComment.Replace( _T("\r\n"), _T("{n}") );
				if ( bCalculate )
				{
					nGroup += G2_PACKET_LEN( G2_PACKET_COMMENT, pPacket->GetStringLen( strComment ) );
				}
				else
				{
					pPacket->WritePacket( G2_PACKET_COMMENT, pPacket->GetStringLen( strComment ) );
					pPacket->WriteString( strComment, FALSE );
				}
			}

			if ( pFile->m_bBogus )
			{
				if ( bCalculate )
					nGroup += G2_PACKET_LEN( G2_PACKET_BOGUS, 0 );
				else
					pPacket->WritePacket( G2_PACKET_BOGUS, 0 );
			}
		}

		if ( ! m_pSearch )
		{
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_OBJECT_ID, sizeof( DWORD ) );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_OBJECT_ID, sizeof( DWORD ) );
				pPacket->WriteLongBE( pFile->m_nIndex );
			}
		}
	}
	while( bCalculate );
}

void CLocalSearch::AddHitDC(CDCPacket* pPacket, CSchemaMap& /*pSchemas*/, CLibraryFile const * const pFile, int /*nIndex*/)
{
	// $SR Nick FileName<0x05>FileSize FreeSlots/TotalSlots<0x05>HubName (HubIP:HubPort)|

	if ( ! m_pSearch )
		return;

	CUploadQueue* pQueue = UploadQueues.SelectQueue(
		PROTOCOL_DC, NULL, 0, CUploadQueue::ulqBoth, NULL );
	int nTotalSlots = pQueue ? pQueue->m_nMaxTransfers : 0;
	int nActiveSlots = pQueue ? pQueue->GetActiveCount() : 0;
	int nFreeSlots = nTotalSlots > nActiveSlots ? ( nTotalSlots - nActiveSlots ) : 0;

	CString sHubName = _T("Hub"), sNick = _T("User");
	{
		// Retrieving user and hub names
		CQuickLock oLock( Network.m_pSection );
		if ( CNeighbour* pNeighbour = Neighbours.Get( m_pSearch->m_pEndpoint.sin_addr ) )
		{
			if ( pNeighbour->m_nProtocol == m_nProtocol )
			{
				CDCNeighbour* pDCNeighbour = static_cast< CDCNeighbour* >( pNeighbour );
				sNick = pDCNeighbour->m_sNick;
				sHubName = pDCNeighbour->m_sServerName;
			}
		}
	}

	if ( pFile->m_oTiger )	// It's TTH search
		sHubName = _T("TTH:") + pFile->m_oTiger.toString();

	CString strAnswer;

	CBuffer pAnswer;
	pAnswer.Add( _P("$SR ") );
	pAnswer.Print( sNick );
	pAnswer.Add( _P(" ") );
	pAnswer.Print( pFile->m_sName );
	pAnswer.Add( _P("\x05") );
	strAnswer.Format( _T("%I64u %d/%d"), pFile->m_nSize, nFreeSlots, nTotalSlots );
	pAnswer.Print( strAnswer );
	pAnswer.Add( _P("\x05") );
	strAnswer.Format( _T("%s (%s:%u)"),
		(LPCTSTR)sHubName,
		(LPCTSTR)CString( inet_ntoa( m_pSearch->m_pEndpoint.sin_addr ) ),
		ntohs( m_pSearch->m_pEndpoint.sin_port ) );
	pAnswer.Print( strAnswer );
	pAnswer.Add( _P("|") );

	pPacket->Write( pAnswer.m_pBuffer, pAnswer.m_nLength );
}

//////////////////////////////////////////////////////////////////////
// CLocalSearch add download hit

template<>
void CLocalSearch::AddHit< CDownload >(CPacket* pPacket, CSchemaMap& pSchemas, const CDownload* pDownload, int nIndex)
{
	ASSERT( pPacket != NULL );

	switch ( m_nProtocol )
	{
	case PROTOCOL_G1:
		AddHitG1( static_cast< CG1Packet* >( pPacket ), pSchemas, pDownload, nIndex );
		break;
	case PROTOCOL_G2:
		AddHitG2( static_cast< CG2Packet* >( pPacket ), pSchemas, pDownload, nIndex );
		break;
	case PROTOCOL_DC:
		AddHitDC( static_cast< CDCPacket* >( pPacket ), pSchemas, pDownload, nIndex );
		break;
	default:
		ASSERT( FALSE );
	}
}

void CLocalSearch::AddHitG1(CG1Packet* /*pPacket*/, CSchemaMap& /*pSchemas*/, CDownload const * const /*pDownload*/, int /*nIndex*/)
{
	// ToDo: Add Gnutella(1) active-download hit packet!
}

void CLocalSearch::AddHitG2(CG2Packet* pPacket, CSchemaMap& /*pSchemas*/, CDownload const * const pDownload, int /*nIndex*/)
{
	// Pass 1: Calculate child group size
	// Pass 2: Write the child packet
	DWORD nGroup = 0;
	bool bCalculate = false;
	do
	{
		bCalculate = ! bCalculate;

		if ( ! bCalculate )
			pPacket->WritePacket( G2_PACKET_HIT_DESCRIPTOR, nGroup, TRUE );

		if ( pDownload->m_oTiger && pDownload->m_oSHA1 )
		{
			const char prefix[] = "bp";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::Sha1Hash::byteCount + Hashes::TigerHash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::Sha1Hash::byteCount + Hashes::TigerHash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pDownload->m_oSHA1 );
				pPacket->Write( pDownload->m_oTiger );
			}
		}
		else if ( pDownload->m_oTiger )
		{
			const char prefix[] = "ttr";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::TigerHash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::TigerHash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pDownload->m_oTiger );
			}
		}
		else if ( pDownload->m_oSHA1 )
		{
			const char prefix[] = "sha1";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::Sha1Hash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::Sha1Hash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pDownload->m_oSHA1 );
			}
		}

		if ( pDownload->m_oED2K )
		{
			const char prefix[] = "ed2k";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::Ed2kHash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::Ed2kHash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pDownload->m_oED2K );
			}
		}

		if ( pDownload->m_oBTH )
		{
			const char prefix[] = "btih";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::BtHash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::BtHash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pDownload->m_oBTH );
			}
		}

		if ( pDownload->m_oMD5 )
		{
			const char prefix[] = "md5";
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_URN, sizeof( prefix ) + Hashes::Md5Hash::byteCount );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_URN, sizeof( prefix ) + Hashes::Md5Hash::byteCount );
				pPacket->WriteString( prefix );
				pPacket->Write( pDownload->m_oMD5 );
			}
		}

		if ( m_pSearch->m_bWantDN )
		{
			if ( pDownload->m_nSize <= 0xFFFFFFFF )
			{
				if ( bCalculate )
				{
					nGroup += G2_PACKET_LEN( G2_PACKET_DESCRIPTIVE_NAME, sizeof( DWORD ) + pPacket->GetStringLen( pDownload->m_sName ) );
				}
				else
				{
					pPacket->WritePacket( G2_PACKET_DESCRIPTIVE_NAME, sizeof( DWORD ) + pPacket->GetStringLen( pDownload->m_sName ) );
					pPacket->WriteLongBE( (DWORD)pDownload->m_nSize );
					pPacket->WriteString( pDownload->m_sName, FALSE );
				}
			}
			else // Size > 0xFFFFFFFF
			{
				if ( bCalculate )
				{
					nGroup += G2_PACKET_LEN( G2_PACKET_SIZE, sizeof( QWORD ) ) +
						G2_PACKET_LEN( G2_PACKET_DESCRIPTIVE_NAME, pPacket->GetStringLen( pDownload->m_sName ) );
				}
				else
				{
					pPacket->WritePacket( G2_PACKET_SIZE, sizeof( QWORD ) );
					pPacket->WriteInt64( pDownload->m_nSize );
					pPacket->WritePacket( G2_PACKET_DESCRIPTIVE_NAME, pPacket->GetStringLen( pDownload->m_sName ) );
					pPacket->WriteString( pDownload->m_sName, FALSE );
				}
			}
		}

		if ( m_pSearch->m_bWantURL )
		{
			if ( bCalculate )
				nGroup += G2_PACKET_LEN( G2_PACKET_URL, 0 );
			else
				pPacket->WritePacket( G2_PACKET_URL, 0 );
		}

		QWORD nComplete = pDownload->GetVolumeComplete();
		if ( nComplete <= 0xFFFFFFFF )
		{
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_PARTIAL, sizeof( DWORD ) );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_PARTIAL, sizeof( DWORD ) );
				pPacket->WriteLongBE( (DWORD)nComplete );
			}
		}
		else // Downloaded size > 0xFFFFFFFF
		{
			if ( bCalculate )
			{
				nGroup += G2_PACKET_LEN( G2_PACKET_PARTIAL, sizeof( QWORD ) );
			}
			else
			{
				pPacket->WritePacket( G2_PACKET_PARTIAL, sizeof( QWORD ) );
				pPacket->WriteInt64( nComplete );
			}
		}
	}
	while( bCalculate );
}

void CLocalSearch::AddHitDC(CDCPacket* /*pPacket*/, CSchemaMap& /*pSchemas*/, CDownload const * const /*pDownload*/, int /*nIndex*/)
{
	// ToDo: Add DC++ active-download hit packet
}

//////////////////////////////////////////////////////////////////////
// CLocalSearch create packet

CPacket* CLocalSearch::CreatePacket()
{
	switch ( m_nProtocol )
	{
	case PROTOCOL_G1:
		return static_cast< CPacket* >( CreatePacketG1() );
	case PROTOCOL_G2:
		return static_cast< CPacket* >( CreatePacketG2() );
	case PROTOCOL_DC:
		return static_cast< CPacket* >( CreatePacketDC() );
	default:
		ASSERT( FALSE );
	}
	return NULL;
}

CG1Packet* CLocalSearch::CreatePacketG1()
{
	CG1Packet* pPacket = CG1Packet::New( G1_PACKET_HIT,
		( m_pSearch ? m_pSearch->m_nTTL : Settings.Gnutella1.SearchTTL ), m_oGUID );

	pPacket->WriteByte( 0 ); // Hit count will be set latter
	pPacket->WriteShortLE( htons( Network.m_pHost.sin_port ) );
	pPacket->WriteLongLE( Network.m_pHost.sin_addr.S_un.S_addr );

	if ( Uploads.m_bStable )
		pPacket->WriteLongLE( Uploads.m_nBestSpeed * 8 / 1024 );
	else
		pPacket->WriteLongLE( Settings.Connection.OutSpeed );

	return pPacket;
}

CG2Packet* CLocalSearch::CreatePacketG2()
{
	CG2Packet* pPacket = CG2Packet::New( G2_PACKET_HIT, TRUE );

	pPacket->WritePacket( G2_PACKET_NODE_GUID, 16 );
	pPacket->Write( Hashes::Guid( MyProfile.oGUID ) );

	//if ( Network.IsListening() )
	//{
	pPacket->WritePacket( G2_PACKET_NODE_ADDRESS, 6 );
	pPacket->WriteLongLE( Network.m_pHost.sin_addr.S_un.S_addr );
	pPacket->WriteShortBE( htons( Network.m_pHost.sin_port ) );
	//}

	pPacket->WritePacket( G2_PACKET_VENDOR, 4 );
	pPacket->WriteString( VENDOR_CODE, FALSE );

	if ( Network.IsFirewalled() )
		pPacket->WritePacket( G2_PACKET_PEER_FIREWALLED, 0 );

	{
		CSingleLock pNetLock( &Network.m_pSection );
		if ( pNetLock.Lock( 50 ) )
		{
			for ( POSITION pos = Neighbours.GetIterator() ; pos ; )
			{
				CNeighbour* pNeighbour = Neighbours.GetNext( pos );

				if ( pNeighbour->m_nNodeType != ntLeaf &&
					 pNeighbour->m_nProtocol == PROTOCOL_G2 )
				{
					pPacket->WritePacket( G2_PACKET_NEIGHBOUR_HUB, 6 );
					pPacket->WriteLongLE( pNeighbour->m_pHost.sin_addr.S_un.S_addr );
					pPacket->WriteShortBE( htons( pNeighbour->m_pHost.sin_port ) );
				}
			}
			pNetLock.Unlock();
		}
	}

	if ( ! Uploads.m_bStable )
		pPacket->WritePacket( G2_PACKET_PEER_UNSTABLE, 0 );

	int nQueue = 1;

	CSingleLock pQueueLock( &UploadQueues.m_pSection );
	if ( pQueueLock.Lock( 2000 ) )
	{
		for ( POSITION pos = UploadQueues.GetIterator() ; pos ; nQueue++ )
		{
			CUploadQueue* pQueue = UploadQueues.GetNext( pos );
			pPacket->WritePacket( G2_PACKET_HIT_GROUP, ( 4 + 7 ) + 2, TRUE );
			pPacket->WritePacket( G2_PACKET_PEER_STATUS, 7 );
			pPacket->WriteShortBE( WORD( pQueue->GetQueuedCount() + pQueue->GetTransferCount() ) );
			pPacket->WriteByte( BYTE( pQueue->GetTransferCount( TRUE ) ) );
			pPacket->WriteLongBE( pQueue->GetPredictedBandwidth() * 8 / 1024 );
			pPacket->WriteByte( 0 );
			pPacket->WriteByte( BYTE( nQueue ) );
		}

		pQueueLock.Unlock();
	}

	CString strNick = MyProfile.GetNick();
	if ( ! strNick.IsEmpty() )
	{
		if ( strNick.GetLength() > 32 )
			strNick = strNick.Left( 32 );

		int nNick = pPacket->GetStringLen( strNick );
		pPacket->WritePacket( G2_PACKET_PROFILE, nNick + 6, TRUE );
		pPacket->WritePacket( G2_PACKET_NICK, nNick );
		pPacket->WriteString( strNick, FALSE );
	}

	if ( Settings.Community.ServeProfile ) pPacket->WritePacket( G2_PACKET_BROWSE_PROFILE, 0 );
	if ( Settings.Community.ServeFiles ) pPacket->WritePacket( G2_PACKET_BROWSE_HOST, 0 );
	if ( Settings.Community.ChatEnable ) pPacket->WritePacket( G2_PACKET_PEER_CHAT, 0 );

	return pPacket;
}

CDCPacket* CLocalSearch::CreatePacketDC()
{
	return CDCPacket::New();
}

//////////////////////////////////////////////////////////////////////
// CLocalSearch core trailer

void CLocalSearch::WriteTrailer(CPacket* pPacket, CSchemaMap& pSchemas, BYTE nHits)
{
	ASSERT( pPacket != NULL );

	switch ( m_nProtocol )
	{
	case PROTOCOL_G1:
		WriteTrailerG1( static_cast< CG1Packet* >( pPacket ), pSchemas, nHits );
		break;
	case PROTOCOL_G2:
		WriteTrailerG2( static_cast< CG2Packet* >( pPacket ), pSchemas, nHits );
		break;
	case PROTOCOL_DC:
		WriteTrailerDC( static_cast< CDCPacket* >( pPacket ), pSchemas, nHits );
		break;
	default:
		ASSERT( FALSE );
	}
}

void CLocalSearch::WriteTrailerG1(CG1Packet* pPacket, CSchemaMap& pSchemas, BYTE nHits)
{
	*pPacket->m_pBuffer = nHits;	// Correct the number of files sent

	pPacket->WriteString( _T( VENDOR_CODE ), FALSE );

	BYTE nFlags[2] = { 0, 0 };

	nFlags[0] |= G1_QHD_BUSY|G1_QHD_STABLE|G1_QHD_SPEED;
	nFlags[1] |= G1_QHD_PUSH;

	if ( Network.IsFirewalled() ) nFlags[0] |= G1_QHD_PUSH;
	if ( Uploads.m_bStable ) nFlags[1] |= G1_QHD_STABLE;
	if ( Uploads.m_bStable ) nFlags[1] |= G1_QHD_SPEED;
	if ( ! UploadQueues.IsTransferAvailable() ) nFlags[1] |= G1_QHD_BUSY;

	if ( Settings.Community.ServeFiles && Settings.Gnutella1.EnableGGEP )
	{
		nFlags[0] |= G1_QHD_GGEP;
		nFlags[1] |= G1_QHD_GGEP;
	}

	CString strXML;
	for ( POSITION pos1 = pSchemas.GetStartPosition() ; pos1 ; )
	{
		CXMLElement* pGroup;
		CSchemaPtr pSchema;
		pSchemas.GetNextAssoc( pos1, pSchema, pGroup );
		strXML += pGroup->ToString( TRUE );
		delete pGroup;
	}
	pSchemas.RemoveAll();

	DWORD nCompressed	= 0;
	auto_array< BYTE > pCompressed;

	pPacket->WriteByte( strXML.IsEmpty() ? 2 : 4 );
	pPacket->WriteByte( nFlags[0] );
	pPacket->WriteByte( nFlags[1] );

	LPSTR pszXML = NULL;
	int nXML = 0;

	if ( ! strXML.IsEmpty() )
	{
		nXML = WideCharToMultiByte( CP_ACP, 0, strXML, -1, NULL, 0, NULL, NULL );
		pszXML = new CHAR[ nXML ];
		WideCharToMultiByte( CP_ACP, 0, strXML, -1, pszXML, nXML, NULL, NULL );
		if ( nXML > 0 ) nXML --;

		pCompressed = CZLib::Compress( pszXML, nXML, &nCompressed );

		// 9 = "{deflate}", 11 = "{plaintext}"
		if ( nCompressed + 9 < (DWORD)nXML + 11 && pCompressed.get() != NULL )
		{
			pPacket->WriteShortLE( (WORD)( nCompressed + 9 + 1 ) );
		}
		else
		{
			pPacket->WriteShortLE( WORD( nXML + 11 + 1 ) );
			pCompressed.reset();
		}
	}

	pPacket->WriteByte( Settings.Community.ChatEnable ? 1 : 0 );

	if ( Settings.Community.ServeFiles && Settings.Gnutella1.EnableGGEP )
	{
		pPacket->WriteByte( GGEP_MAGIC );
		pPacket->WriteByte( GGEP_HDR_LAST | 2 );
		pPacket->WriteByte( 'B' );
		pPacket->WriteByte( 'H' );
		pPacket->WriteByte( GGEP_LEN_LAST );
	}

	if ( pCompressed.get() != NULL )
	{
		pPacket->Write( "{deflate}", 9 );
		pPacket->Write( pCompressed.get(), nCompressed );
		pPacket->WriteByte( 0 );
	}
	else if ( pszXML != NULL )
	{
		pPacket->Write( "{plaintext}", 11 );
		pPacket->Write( pszXML, nXML );
	}

	if ( pszXML != NULL ) delete [] pszXML;

	pPacket->Write( Hashes::Guid( MyProfile.oGUID ) );

#ifdef _DEBUG
	// Test created hit
	if ( CQueryHit* pDebugHit = CQueryHit::FromG1Packet( pPacket ) )
	{
		pDebugHit->Delete();
		pPacket->m_nPosition = 0;
	}
	else
		theApp.Message( MSG_ERROR | MSG_FACILITY_SEARCH, _T("[G1] PeerProject produced search packet above but cannot parse it back.") );
#endif // _DEBUG
}

void CLocalSearch::WriteTrailerG2(CG2Packet* pPacket, CSchemaMap& /*pSchemas*/, BYTE /*nHits*/)
{
	pPacket->WriteByte( 0 );	// End of packet
	pPacket->WriteByte( 0 );	// nHops
	pPacket->Write( m_oGUID );	// SearchID

#ifdef _DEBUG
	// Test created hit
	if ( CQueryHit* pDebugHit = CQueryHit::FromG2Packet( pPacket ) )
	{
		pDebugHit->Delete();
		pPacket->m_nPosition = 0;
	}
	else
		theApp.Message( MSG_ERROR | MSG_FACILITY_SEARCH, _T("[G2] PeerProject produced search packet above but cannot parse it back.") );
#endif // _DEBUG
}

void CLocalSearch::WriteTrailerDC(CDCPacket* /*pPacket*/, CSchemaMap& /*pSchemas*/, BYTE /*nHits*/)
{
}

//////////////////////////////////////////////////////////////////////
// CLocalSearch dispatch packet

void CLocalSearch::DispatchPacket(CPacket* pPacket)
{
	ASSERT( pPacket != NULL );

	if ( m_pBuffer )
	{
		pPacket->ToBuffer( m_pBuffer );
	}
	else if ( m_bUDP )
	{
		Datagrams.Send( &m_pEndpoint, pPacket, FALSE );
	}
	else
	{
		CQuickLock oLock( Network.m_pSection );
		if ( CNeighbour* pNeighbour = Neighbours.Get( m_pEndpoint.sin_addr ) )
		{
			if ( pNeighbour->m_nProtocol == m_nProtocol )
				pNeighbour->Send( pPacket, FALSE, TRUE );
		}
	}

	pPacket->Release();
}

//////////////////////////////////////////////////////////////////////
// CLocalSearch physical and virtual folder tree

void CLocalSearch::WriteVirtualTree()
{
	CSingleLock oLock( &Library.m_pSection );
	if ( oLock.Lock( 1000 ) )
	{
		CG2Packet* pPacket = AlbumToPacket( Library.GetAlbumRoot() );
		oLock.Unlock();
		if ( pPacket ) DispatchPacket( pPacket );
	}

	if ( oLock.Lock( 1000 ) )
	{
		CG2Packet* pPacket = FoldersToPacket();
		oLock.Unlock();
		if ( pPacket ) DispatchPacket( pPacket );
	}
}

CG2Packet* CLocalSearch::AlbumToPacket(CAlbumFolder* pFolder)
{
	if ( pFolder == NULL ) return NULL;

	if ( pFolder->m_pSchema != NULL && pFolder->m_pSchema->m_bPrivate ) return NULL;
	if ( pFolder->GetSharedCount() == 0 ) return NULL;

	CG2Packet* pPacket = CG2Packet::New( G2_PACKET_VIRTUAL_FOLDER, TRUE );

	if ( pFolder->m_pSchema != NULL )
	{
		CXMLElement* pXML = pFolder->m_pSchema->Instantiate( TRUE );

		if ( pFolder->m_pXML != NULL )
		{
			pXML->AddElement( pFolder->m_pXML->Clone() );
		}
		else
		{
			CXMLElement* pBody = pXML->AddElement( pFolder->m_pSchema->m_sSingular );
			pBody->AddAttribute( pFolder->m_pSchema->GetFirstMemberName(), pFolder->m_sName );
		}

		CString strXML = pXML->ToString();
		delete pXML;

		pPacket->WritePacket( G2_PACKET_METADATA, pPacket->GetStringLen( strXML ) );
		pPacket->WriteString( strXML, FALSE );
	}

	for ( POSITION pos = pFolder->GetFolderIterator() ; pos ; )
	{
		if ( CG2Packet* pChild = AlbumToPacket( pFolder->GetNextFolder( pos ) ) )
		{
			pPacket->WritePacket( pChild );
			pChild->Release();
		}
	}

	pPacket->WritePacket( G2_PACKET_FILES, static_cast< DWORD >( pFolder->GetFileCount() * 4 ) );

	for ( POSITION pos = pFolder->GetFileIterator() ; pos ; )
	{
		CLibraryFile* pFile = pFolder->GetNextFile( pos );
		pPacket->WriteLongBE( pFile->m_nIndex );
	}

	return pPacket;
}

CG2Packet* CLocalSearch::FoldersToPacket()
{
	CG2Packet* pPacket = CG2Packet::New( G2_PACKET_PHYSICAL_FOLDER, TRUE );

	for ( POSITION pos = LibraryFolders.GetFolderIterator() ; pos ; )
	{
		if ( CG2Packet* pChild = FolderToPacket( LibraryFolders.GetNextFolder( pos ) ) )
		{
			pPacket->WritePacket( pChild );
			pChild->Release();
		}
	}

	return pPacket;
}

CG2Packet* CLocalSearch::FolderToPacket(CLibraryFolder* pFolder)
{
	if ( pFolder == NULL ) return NULL;

	if ( pFolder->GetSharedCount() == 0 ) return NULL;

	CG2Packet* pPacket = CG2Packet::New( G2_PACKET_PHYSICAL_FOLDER, TRUE );

	pPacket->WritePacket( G2_PACKET_DESCRIPTIVE_NAME, pPacket->GetStringLen( pFolder->m_sName ) );
	pPacket->WriteString( pFolder->m_sName, FALSE );

	for ( POSITION pos = pFolder->GetFolderIterator() ; pos ; )
	{
		if ( CG2Packet* pChild = FolderToPacket( pFolder->GetNextFolder( pos ) ) )
		{
			pPacket->WritePacket( pChild );
			pChild->Release();
		}
	}

	pPacket->WritePacket( G2_PACKET_FILES, static_cast< DWORD >( pFolder->GetFileCount() * 4 ) );

	for ( POSITION pos = pFolder->GetFileIterator() ; pos ; )
	{
		CLibraryFile* pFile = pFolder->GetNextFile( pos );
		pPacket->WriteLongBE( pFile->m_nIndex );
	}

	return pPacket;
}
