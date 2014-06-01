//
// EDPacket.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
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
#include "EDPacket.h"
#include "EDClient.h"
#include "EDClients.h"
#include "EDNeighbour.h"
#include "Buffer.h"
#include "Network.h"
#include "Kademlia.h"
#include "SharedFile.h"
#include "Statistics.h"
#include "Schema.h"
#include "XML.h"
#include "ZLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

CEDPacket::CEDPacketPool CEDPacket::POOL;

// Packet Types

class CEDPacketTypes
{
public:
	inline CEDPacketTypes() throw()
	{
		static const struct
		{
			BYTE	nType;
			BYTE	nProtocol;							// ED2K_PROTOCOL_EDONKEY or ED2K_PROTOCOL_EMULE
			BOOL	bServer;							// TRUE : client <-> server, FALSE : client <-> client
			BOOL	bUDP;								// TRUE: UDP, FALSE: TCP
			LPCTSTR	pszName;
		}
		pszTypes[] =
		{
		// Client - Server, Local (TCP)
			{ ED2K_C2S_LOGINREQUEST,		ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"LoginRequest" },
			{ ED2K_C2S_GETSERVERLIST,		ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"GetServerList" },
			{ ED2K_C2S_OFFERFILES,			ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"ShareFiles" },
			{ ED2K_C2S_SEARCHREQUEST,		ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"SearchRequest" },
			{ ED2K_C2S_SEARCHUSER,			ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"Browse" },
			{ ED2K_C2S_GETSOURCES,			ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"FindSource" },
			{ ED2K_C2S_CALLBACKREQUEST,		ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"PushRequest" },
			{ ED2K_C2S_MORERESULTS,			ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"MoreResults" },
			{ ED2K_S2C_REJECTED,			ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"Rejected" },
			{ ED2K_S2C_SERVERMESSAGE,		ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"ServerMessage" },
			{ ED2K_S2C_IDCHANGE,			ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"IdChange" },
			{ ED2K_S2C_SERVERLIST,			ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"ServerList" },
			{ ED2K_S2C_SEARCHRESULTS,		ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"SearchResults" },
			{ ED2K_S2C_FOUNDSOURCES,		ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"FoundSources" },
			{ ED2K_S2C_SERVERSTATUS,		ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"ServerStatus" },
			{ ED2K_S2C_SERVERIDENT,			ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"ServerIdent" },
			{ ED2K_S2C_CALLBACKREQUESTED,	ED2K_PROTOCOL_EDONKEY,	TRUE,	FALSE,	L"PushAnswer" },

		// Client - Server, Global (UDP)
			{ ED2K_C2SG_SERVERSTATUSREQUEST,ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"StatusRequest" },
			{ ED2K_S2CG_SERVERSTATUS,		ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"ServerStatus" },
			{ ED2K_C2SG_SEARCHREQUEST,		ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"SearchRequest" },
			{ ED2K_C2SG_SEARCHREQUEST2,		ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"SearchRequest2" },
			{ ED2K_C2SG_SEARCHREQUEST3,		ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"SearchRequest3" },
			{ ED2K_S2CG_SEARCHRESULT,		ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"SearchResult" },
			{ ED2K_C2SG_GETSOURCES,			ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"FindSource" },
			{ ED2K_C2SG_GETSOURCES2,		ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"FindSource2" },
			{ ED2K_S2CG_FOUNDSOURCES,		ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"FoundSources" },
			{ ED2K_C2SG_CALLBACKREQUEST,	ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"PushRequest" },
			{ ED2K_S2CG_CALLBACKFAIL,		ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"PushFail" },
			{ ED2K_C2SG_LIST_REQ,			ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"ListRequest" },
			{ ED2K_S2CG_LIST_RES,			ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"ListAnswer" },
			{ ED2K_C2SG_DESC_REQ,			ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"InfoRequest" },
			{ ED2K_S2CG_DESC_RES,			ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"InfoAnswer" },
			{ ED2K_C2SG_LIST_REQ2,			ED2K_PROTOCOL_EDONKEY,	TRUE,	TRUE,	L"ListRequest2" },

		// Client - Client, TCP
			{ ED2K_C2C_HELLO,				ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"Hello" },
			{ ED2K_C2C_HELLOANSWER,			ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"HelloAnswer" },
			{ ED2K_C2C_FILEREQUEST,			ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"FileRequest" },
			{ ED2K_C2C_FILEREQANSWER,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"FileName" },
			{ ED2K_C2C_FILENOTFOUND,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"FileNotFound" },
			{ ED2K_C2C_FILESTATUS,			ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"FileStatus" },
			{ ED2K_C2C_QUEUEREQUEST,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"EnqueueMe" },
			{ ED2K_C2C_QUEUERELEASE,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"DequeueMe" },
			{ ED2K_C2C_QUEUERANK,			ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"QueueRank" },
			{ ED2K_C2C_STARTUPLOAD,			ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"StartUpload" },
			{ ED2K_C2C_FINISHUPLOAD,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"UploadFinished" },
			{ ED2K_C2C_REQUESTPARTS,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"RequestFrag" },
			{ ED2K_C2C_SENDINGPART,			ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"Fragment" },
			{ ED2K_C2C_FILESTATUSREQUEST,	ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"GetStatus" },
			{ ED2K_C2C_HASHSETREQUEST,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"GetHashset" },
			{ ED2K_C2C_HASHSETANSWER,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"Hashset" },
			{ ED2K_C2C_ASKSHAREDFILES,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"BrowseFiles" },
			{ ED2K_C2C_ASKSHAREDFILESANSWER,ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"BrowseFilesAnswer" },
			{ ED2K_C2C_MESSAGE,				ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"Message" },
			{ ED2K_C2C_ASKSHAREDDIRS,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"BrowseDirs" },
			{ ED2K_C2C_ASKSHAREDDIRSANSWER,	ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"BrowseDirsAnswer" },
			{ ED2K_C2C_ASKSHAREDDIRSDENIED,	ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"BrowseDirsDenied" },
			{ ED2K_C2C_VIEWSHAREDDIR,		ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"ViewDirs" },
			{ ED2K_C2C_VIEWSHAREDDIRANSWER,	ED2K_PROTOCOL_EDONKEY,	FALSE,	FALSE,	L"ViewDirsAnswer" },

		// eMule Client - Client, TCP
			{ ED2K_C2C_EMULEINFO,			ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"eMuleInfo" },
			{ ED2K_C2C_EMULEINFOANSWER,		ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"eMuleInfoAnswer" },
			{ ED2K_C2C_COMPRESSEDPART,		ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"CompFragment" },
			{ ED2K_C2C_QUEUERANKING,		ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"QueueRanking" },
			{ ED2K_C2C_FILEDESC,			ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"FileDescription" },
			{ ED2K_C2C_REQUESTSOURCES,		ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"RequestSources" },
			{ ED2K_C2C_ANSWERSOURCES,		ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"Sources" },
			{ ED2K_C2C_REQUESTPREVIEW,		ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"RequestPreview" },
			{ ED2K_C2C_PREVIEWANWSER,		ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"Preview" },
			{ ED2K_C2C_COMPRESSEDPART_I64,	ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"CompFragment64" },
			{ ED2K_C2C_REQUESTPARTS_I64,	ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"RequestFrag64" },
			{ ED2K_C2C_SENDINGPART_I64,		ED2K_PROTOCOL_EMULE,	FALSE,	FALSE,	L"Fragment64" },

		// Client - Client, UDP
			{ ED2K_C2C_UDP_REASKFILEPING,	ED2K_PROTOCOL_EMULE,	FALSE,	TRUE,	L"Re-AskFile" },
			{ ED2K_C2C_UDP_REASKACK,		ED2K_PROTOCOL_EMULE,	FALSE,	TRUE,	L"Re-AskAnswer" },
			{ ED2K_C2C_UDP_FILENOTFOUND,	ED2K_PROTOCOL_EMULE,	FALSE,	TRUE,	L"FileNotFound" },
			{ ED2K_C2C_UDP_QUEUEFULL,		ED2K_PROTOCOL_EMULE,	FALSE,	TRUE,	L"QueueFull" },
			{ ED2K_C2C_UDP_REASKCALLBACKUDP,  ED2K_PROTOCOL_EMULE,	FALSE,	TRUE,	L"Re-AskCallback" },
			{ ED2K_C2C_UDP_DIRECTCALLBACKREQ, ED2K_PROTOCOL_EMULE,	FALSE,	TRUE,	L"DirectCallback" },
			{ ED2K_C2C_UDP_PORTTEST,		ED2K_PROTOCOL_EMULE,	FALSE,	TRUE,	L"PortTest" }
		};

		m_oTypes.InitHashTable( GetBestHashTableSize( _countof( pszTypes ) ) );

		for ( int i = 0 ; i < _countof( pszTypes ) ; ++i )
		{
			m_oTypes.SetAt( MAKEDWORD( MAKEWORD( pszTypes[ i ].nType, pszTypes[ i ].nProtocol ), MAKEWORD( pszTypes[ i ].bUDP, pszTypes[ i ].bServer ) ), pszTypes[ i ].pszName );
		}
	}

	inline CString GetAt(BYTE nType, BYTE nProtocol, BOOL bUDP, BOOL bServer) const throw()
	{
		CString sType;
		if ( ! m_oTypes.Lookup( MAKEDWORD( MAKEWORD( nType, ( ( bServer && nProtocol == ED2K_PROTOCOL_EMULE ) ? ED2K_PROTOCOL_EDONKEY : nProtocol ) ), MAKEWORD( bUDP, bServer ) ), sType ) )
			sType.Format( L"0x%02x-0x%02x", nType, nProtocol );
		return sType;
	}

private:
	CMap< DWORD, DWORD, CString, const CString& > m_oTypes;
};

static const CEDPacketTypes PacketTypes;

//////////////////////////////////////////////////////////////////////
// CEDPacket construction

CEDPacket::CEDPacket()
	: CPacket		( PROTOCOL_ED2K )
	, m_nEdProtocol	( 0 )
	, m_nType		( 0 )
	, m_bDeflate	( FALSE )
{
}

CEDPacket::~CEDPacket()
{
}

void CEDPacket::Reset()
{
	CPacket::Reset();

	m_nEdProtocol	= 0;
	m_nType			= 0;
	m_bDeflate		= FALSE;
}

//////////////////////////////////////////////////////////////////////
// CEDPacket length prefixed strings

CString CEDPacket::ReadEDString(BOOL bUnicode)
{
	WORD nLen = ReadShortLE();
	return bUnicode ?
		ReadStringUTF8( nLen ) :
		ReadStringASCII( nLen );
}

void CEDPacket::WriteEDString(LPCTSTR psz, BOOL bUnicode)
{
	if ( bUnicode )
	{
		WORD nLen = (WORD)GetStringLenUTF8( psz );
		WriteShortLE( nLen );
		WriteStringUTF8( psz, FALSE );
	}
	else
	{
		WORD nLen = (WORD)GetStringLen( psz );
		WriteShortLE( nLen );
		WriteString( psz, FALSE );
	}
}

CString CEDPacket::ReadLongEDString(BOOL bUnicode)
{
	DWORD nLen = ReadLongLE();
	if ( bUnicode )
		return ReadStringUTF8( nLen );

	return ReadStringASCII( nLen );
}

void CEDPacket::WriteLongEDString(LPCTSTR psz, BOOL bUnicode)
{
	if ( bUnicode )
	{
		DWORD nLen = GetStringLenUTF8( psz );
		WriteLongLE( nLen );
		WriteStringUTF8( psz, FALSE );
	}
	else
	{
		DWORD nLen = GetStringLen( psz );
		WriteLongLE( nLen );
		WriteString( psz, FALSE );
	}
}

void CEDPacket::WriteFile(const CPeerProjectFile* pPeerProjectFile, QWORD nSize,
	const CEDClient* pClient, const CEDNeighbour* pServer, bool bPartial)
{
	ASSERT( ( pClient && ! pServer ) || ( ! pClient && pServer ) );

	const CLibraryFile* pFile = bPartial ?
		NULL : static_cast< const CLibraryFile* >( pPeerProjectFile );

	bool bDeflate = ( pServer && ( pServer->m_nTCPFlags & ED2K_SERVER_TCP_DEFLATE ) != 0 );
	bool bUnicode = ( pServer && ( pServer->m_nTCPFlags & ED2K_SERVER_TCP_UNICODE ) != 0 ) ||
		( pClient && pClient->m_bEmUnicode );
	bool bSmlTags = ( pServer && ( pServer->m_nTCPFlags & ED2K_SERVER_TCP_SMALLTAGS ) != 0 ) ||
		( pClient != NULL );

	// Send the file hash
	Write( pPeerProjectFile->m_oED2K );

	// Send client ID + port
	DWORD nClientID;
	WORD nClientPort;
	if ( pServer )
	{
		// If we have a 'new' ed2k server
		if ( bDeflate )
		{
			if ( bPartial )
			{
				// Partial file
				nClientID = 0xFCFCFCFC;
				nClientPort = 0xFCFC;
			}
			else
			{
				// Complete file
				nClientID = 0xFBFBFBFB;
				nClientPort = 0xFBFB;
			}
		}
		else
		{
			nClientID = pServer->GetID();
			nClientPort = ntohs( Network.m_pHost.sin_port );
		}
	}
	else
	{
		nClientID = pClient ? pClient->GetID() : 0;
		nClientPort = ntohs( Network.m_pHost.sin_port );
	}
	WriteLongLE( nClientID );
	WriteShortLE( nClientPort );

	// Load metadata
	CString strType, strTitle, strArtist, strAlbum, strCodec;
	DWORD nBitrate = 0, nLength = 0;
	BYTE nRating = 0;
	if ( bSmlTags && pFile )
	{
		if ( pFile->m_pSchema && ! pFile->m_pSchema->m_sDonkeyType.IsEmpty() )
		{
			// File type
			strType = pFile->m_pSchema->m_sDonkeyType;

			if ( pFile->m_pMetadata )
			{
				// Title
				if ( pFile->m_pMetadata->GetAttributeValue( L"title" ).GetLength() )
					strTitle = pFile->m_pMetadata->GetAttributeValue( L"title" );

				if ( pFile->IsSchemaURI( CSchema::uriAudio ) )
				{
					// Artist
					if ( pFile->m_pMetadata->GetAttributeValue( L"artist" ).GetLength() )
						strArtist = pFile->m_pMetadata->GetAttributeValue( L"artist" );

					// Album
					if ( pFile->m_pMetadata->GetAttributeValue( L"album" ).GetLength() )
						strAlbum = pFile->m_pMetadata->GetAttributeValue( L"album" );

					// Bitrate
					if ( pFile->m_pMetadata->GetAttributeValue( L"bitrate" ).GetLength() )
						_stscanf( pFile->m_pMetadata->GetAttributeValue( L"bitrate" ), L"%i", &nBitrate );

					// Length
					if ( pFile->m_pMetadata->GetAttributeValue( L"seconds" ).GetLength() )
						_stscanf( pFile->m_pMetadata->GetAttributeValue( L"seconds" ), L"%i", &nLength );
				}
				else if ( pFile->IsSchemaURI( CSchema::uriVideo ) )
				{
					// Codec
					if ( pFile->m_pMetadata->GetAttributeValue( L"codec" ).GetLength() )
						strCodec = pFile->m_pMetadata->GetAttributeValue( L"codec" );

					// Length
					if ( pFile->m_pMetadata->GetAttributeValue( L"minutes" ).GetLength() )
					{
						double nMins = 0.0;
						_stscanf( pFile->m_pMetadata->GetAttributeValue( L"minutes" ), L"%lf", &nMins );
						nLength = (DWORD)( nMins * (double)60 );	// Convert to seconds
					}
				}
			}
		}

		// File rating
		if ( pFile->m_nRating )
			nRating = (BYTE)min( pFile->m_nRating, 5 );
	}

	// Set the number of tags present
	DWORD nTags = 2;	// File name and size are always present
	if ( nSize > MAX_SIZE_32BIT ) nTags++;
	if ( pClient ) nTags += 2; //3;
	if ( ! strType.IsEmpty() ) nTags++;
	if ( ! strTitle.IsEmpty() ) nTags++;
	if ( ! strArtist.IsEmpty() ) nTags++;
	if ( ! strAlbum.IsEmpty() ) nTags++;
	if ( ! strCodec.IsEmpty() ) nTags++;
	if ( nBitrate ) nTags++;
	if ( nLength ) nTags++;
	if ( nRating ) nTags++;
	WriteLongLE( nTags );

	// Filename
	CEDTag( ED2K_FT_FILENAME, pPeerProjectFile->m_sName ).Write( this, bUnicode, bSmlTags );

	// File size
	CEDTag( ED2K_FT_FILESIZE, (DWORD)nSize ).Write( this, bUnicode, bSmlTags );
	if ( nSize > MAX_SIZE_32BIT )
		CEDTag( ED2K_FT_FILESIZE_HI,(DWORD)( nSize >> 32 ) ).Write( this, bUnicode, bSmlTags );

	// Sources
	//if ( pClient )
	//	CEDTag( ED2K_FT_SOURCES, 1ull ).Write( this, bUnicode, bSmlTags );

	// Complete sources
	if ( pClient )
		CEDTag( ED2K_FT_COMPLETE_SOURCES, 1ull ).Write( this, bUnicode, bSmlTags );

	// Last seen
	if ( pClient )
		CEDTag( ED2K_FT_LASTSEENCOMPLETE, 0ull ).Write( this, bUnicode, bSmlTags );

	// File type
	if ( ! strType.IsEmpty() )
		CEDTag( ED2K_FT_FILETYPE, strType ).Write( this, bUnicode, bSmlTags );

	// Title
	if ( ! strTitle.IsEmpty() )
		CEDTag( ED2K_FT_TITLE, strTitle ).Write( this, bUnicode, bSmlTags );

	// Artist
	if ( ! strArtist.IsEmpty() )
		CEDTag( ED2K_FT_ARTIST, strArtist ).Write( this, bUnicode, bSmlTags );

	// Album
	if ( ! strAlbum.IsEmpty() )
		CEDTag( ED2K_FT_ALBUM, strAlbum ).Write( this, bUnicode, bSmlTags );

	// Codec
	if ( ! strCodec.IsEmpty() )
		CEDTag( ED2K_FT_CODEC, strCodec ).Write( this, bUnicode, bSmlTags );

	// Bitrate
	if ( nBitrate )
		CEDTag( ED2K_FT_BITRATE, nBitrate ).Write( this, bUnicode, bSmlTags );

	// Length
	if ( nLength )
		CEDTag( ED2K_FT_LENGTH, nLength ).Write( this, bUnicode, bSmlTags );

	// File rating
	if ( nRating )
		CEDTag( ED2K_FT_FILERATING, nRating ).Write( this, bUnicode, bSmlTags );
}

//////////////////////////////////////////////////////////////////////
// CEDPacket buffers

void CEDPacket::ToBuffer(CBuffer* pBuffer, bool bTCP)
{
	// ZLIB compress if available
	if ( m_bDeflate )
	{
		m_bDeflate = FALSE;
		Deflate();
	}

	if ( bTCP )
	{
		ED2K_TCP_HEADER pHeader = { m_nEdProtocol, m_nLength + 1, m_nType };
		pBuffer->Add( &pHeader, sizeof( pHeader ) );
		pBuffer->Add( m_pBuffer, m_nLength );
	}
	else // UDP
	{
		ED2K_UDP_HEADER pHeader = { m_nEdProtocol, m_nType };
		pBuffer->Add( &pHeader, sizeof( pHeader ) );
		pBuffer->Add( m_pBuffer, m_nLength );
	}
}

CEDPacket* CEDPacket::ReadBuffer(CBuffer* pBuffer)
{
	if ( pBuffer->m_nLength < sizeof( ED2K_TCP_HEADER ) ) return NULL;
	ED2K_TCP_HEADER* pHeader = reinterpret_cast<ED2K_TCP_HEADER*>(pBuffer->m_pBuffer);
	if ( pHeader->nProtocol != ED2K_PROTOCOL_EDONKEY &&
		 pHeader->nProtocol != ED2K_PROTOCOL_EMULE &&
		 pHeader->nProtocol != ED2K_PROTOCOL_EMULE_PACKED ) return NULL;
	if ( pBuffer->m_nLength - sizeof( *pHeader ) + 1 < pHeader->nLength ) return NULL;
	CEDPacket* pPacket = CEDPacket::New( pHeader );
	pBuffer->Remove( sizeof( *pHeader ) + pHeader->nLength - 1 );
	if ( pPacket->Inflate() )
		return pPacket;
	pPacket->Release();
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CEDPacket compression

BOOL CEDPacket::Deflate()
{
	if ( m_nEdProtocol != ED2K_PROTOCOL_EDONKEY &&
		 m_nEdProtocol != ED2K_PROTOCOL_EMULE )
		return FALSE;

	DWORD nOutput = 0;
	auto_array< BYTE > pOutput( CZLib::Compress( m_pBuffer, m_nLength, &nOutput ) );
	if ( ! pOutput.get() )
		return FALSE;

	if ( nOutput >= m_nLength )
		return FALSE;

	m_nEdProtocol = ED2K_PROTOCOL_EMULE_PACKED;

	delete [] m_pBuffer;
	m_pBuffer = pOutput.release();
	m_nLength = nOutput;
	m_nBuffer = nOutput;
	m_nPosition = 0;

	return TRUE;
}

BOOL CEDPacket::Inflate()
{
	if ( m_nEdProtocol != ED2K_PROTOCOL_EMULE_PACKED &&
		 m_nEdProtocol != ED2K_PROTOCOL_KAD_PACKED &&
		 m_nEdProtocol != ED2K_PROTOCOL_REVCONNECT_PACKED )
		return TRUE;

	DWORD nOutput = 0;
	auto_array< BYTE > pOutput( CZLib::Decompress( m_pBuffer, m_nLength, &nOutput ) );
	if ( ! pOutput.get() )
		return FALSE;

	switch ( m_nEdProtocol )
	{
	case ED2K_PROTOCOL_EMULE_PACKED:
		m_nEdProtocol = ED2K_PROTOCOL_EMULE;
		break;
	case ED2K_PROTOCOL_KAD_PACKED:
		m_nEdProtocol = ED2K_PROTOCOL_KAD;
		break;
	case ED2K_PROTOCOL_REVCONNECT_PACKED:
		m_nEdProtocol = ED2K_PROTOCOL_REVCONNECT;
		break;
	}

	delete [] m_pBuffer;
	m_pBuffer = pOutput.release();
	m_nLength = nOutput;
	m_nBuffer = nOutput;
	m_nPosition = 0;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CEDPacket debug tools

CString CEDPacket::GetType() const
{
	const BOOL bServer = ( m_nNeighbourUnique != NULL ) || ( m_bUDP && m_nEdProtocol == ED2K_PROTOCOL_EDONKEY );
	return PacketTypes.GetAt( m_nType, m_nEdProtocol, m_bUDP, bServer );
}

CString CEDPacket::ToASCII() const
{
	CString strOutput;

	const BOOL bServer = ( m_nNeighbourUnique != NULL ) || ( m_bUDP && m_nEdProtocol == ED2K_PROTOCOL_EDONKEY );
	if ( bServer )
	{
		if ( m_bUDP )
		{
			switch ( m_nType )
			{
			case ED2K_C2SG_SERVERSTATUSREQUEST:
				if ( m_nLength >= 4 )
				{
					const DWORD& nChallenge = *(const DWORD*)( m_pBuffer );
					strOutput.Format( L"id: 0x%08x", nChallenge );
				}
				break;

			case ED2K_S2CG_SERVERSTATUS:
				if ( m_nLength >= 4 )
				{
					const DWORD& nChallenge = *(const DWORD*)( m_pBuffer );
					strOutput.Format( L"id: 0x%08x", nChallenge );
					if ( m_nLength >= 12 )
					{
						const DWORD& nUsers = *(const DWORD*)( m_pBuffer + 4 );
						const DWORD& nFiles = *(const DWORD*)( m_pBuffer + 8 );
						strOutput.AppendFormat( L", users: %u, files: %u", nUsers, nFiles );
						if ( m_nLength >= 16 )
						{
							const DWORD& nMaxUsers = *(const DWORD*)( m_pBuffer + 12 );
							strOutput.AppendFormat( L", max users: %u", nMaxUsers );
							if ( m_nLength >= 24 )
							{
								const DWORD& nSoftLimit = *(const DWORD*)( m_pBuffer + 16 );
								const DWORD& nHardLimit = *(const DWORD*)( m_pBuffer + 20 );
								strOutput.AppendFormat( L", soft limit: %u, hard limit: %u", nSoftLimit, nHardLimit );
								if ( m_nLength >= 28 )
								{
									const DWORD& nUDPFlags = *(const DWORD*)( m_pBuffer + 24 );
									strOutput.AppendFormat( L", udp flags: %s", GetED2KServerUDPFlags( nUDPFlags ) );
									if ( m_nLength >= 32 )
									{
										const DWORD& nLowID = *(const DWORD*)( m_pBuffer + 28 );
										strOutput.AppendFormat( L", low id: %u", nLowID );
										if ( m_nLength >= 40 )
										{
											const WORD& nUDPPort = *(const WORD*)( m_pBuffer + 32 );
											const WORD& nTCPPort = *(const WORD*)( m_pBuffer + 34 );
											const DWORD& nKey = *(const DWORD*)( m_pBuffer + 36 );
											strOutput.AppendFormat( L", udp obf.port: %u, tcp obf.port: %u, server key: 0x%08x", nUDPPort, nTCPPort, nKey );
										}
									}
								}
							}
						}
					}
				}
				break;

			case ED2K_S2CG_SEARCHRESULT:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 4 + 2 + 4 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)( m_pBuffer ) );
					const IN_ADDR& nID = *(const IN_ADDR*)( m_pBuffer+ Hashes::Ed2kHash::byteCount );
					const WORD& nPort = *(const WORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount + 4 );
					strOutput.AppendFormat( L"hash: %s, id: %u, port: %u, tags: %s", oMD4.toString(), nID.s_addr, nPort,
						(LPCTSTR)CEDTag::ToString( m_pBuffer + Hashes::Ed2kHash::byteCount + 4 + 2, m_nLength - ( Hashes::Ed2kHash::byteCount + 4 + 2 ) ) );
				}
				break;

			case ED2K_C2SG_GETSOURCES2:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 4 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const DWORD& nSize = *(const DWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					if ( nSize == 0 && m_nLength >= Hashes::Ed2kHash::byteCount + 4 + 8 )
					{
						const QWORD& nSize64 = *(const QWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount + 4 );
						strOutput.Format( L"hash: %s, size: %I64u", oMD4.toString(), nSize64 );
					}
					else
						strOutput.Format( L"hash: %s, size: %u", oMD4.toString(), nSize );
				}
				break;

			case ED2K_C2SG_GETSOURCES:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					strOutput.Format( L"hash: %s", oMD4.toString() );
				}
				break;

			case ED2K_S2CG_FOUNDSOURCES:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 1 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const BYTE& nCount = *(const BYTE*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					strOutput.Format( L"hash: %s, number: %u", oMD4.toString(), nCount );
					if ( nCount && m_nLength >= Hashes::Ed2kHash::byteCount + 1 + 4 + 2 )
					{
						const IN_ADDR& nID = *(const IN_ADDR*)( m_pBuffer + Hashes::Ed2kHash::byteCount + 1 );
						const WORD& nPort = *(const WORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount + 1 + 4 );
						strOutput.AppendFormat( L" -> id: %u, port: %u", nID.s_addr, nPort );
					}
				}
				break;
			}
		}
		else
		{
			switch ( m_nType )
			{
			case ED2K_C2S_LOGINREQUEST:
				if ( m_nLength >= Hashes::Guid::byteCount + 4 + 2 + 4 )
				{
					const Hashes::Guid oGUID( *(const Hashes::Guid::RawStorage*)( m_pBuffer ) );
					const DWORD& nClientID = *(const DWORD*)( m_pBuffer + Hashes::Guid::byteCount );
					const WORD& nPort = *(const WORD*)( m_pBuffer + Hashes::Guid::byteCount + 4 );
					strOutput.Format( L"guid: %s, id: %u, port: %u, tags: %s", oGUID.toString(), nClientID, nPort,
						CEDTag::ToString( m_pBuffer + Hashes::Guid::byteCount + 4 + 2, m_nLength - ( Hashes::Guid::byteCount + 4 + 2 ) ) );
				}
				break;

			case ED2K_S2C_SERVERIDENT:
				if ( m_nLength >= Hashes::Guid::byteCount + 4 + 2 + 4 )
				{
					const Hashes::Guid oGUID( *(const Hashes::Guid::RawStorage*)( m_pBuffer ) );
					const IN_ADDR& nIP = *(const IN_ADDR*)( m_pBuffer + Hashes::Guid::byteCount );
					const WORD& nPort = *(const WORD*)( m_pBuffer + Hashes::Guid::byteCount + 4 );
					strOutput.Format( L"guid: %s, address: %s:%u, tags: %s", oGUID.toString(), (LPCTSTR)CString( inet_ntoa( nIP ) ), nPort,
						CEDTag::ToString( m_pBuffer + Hashes::Guid::byteCount + 4 + 2, m_nLength - ( Hashes::Guid::byteCount + 4 + 2 ) ) );
				}
				break;

			case ED2K_C2S_CALLBACKREQUEST:
				if ( m_nLength >= 4 )
				{
					const DWORD& nID = *(const DWORD*)( m_pBuffer );
					strOutput.Format( L"id: %u", nID );
				}
				break;

			case ED2K_S2C_CALLBACKREQUESTED:
				if ( m_nLength >= 4 + 2 )
				{
					const IN_ADDR& nIP = *(const IN_ADDR*)( m_pBuffer );
					const WORD& nPort = *(const WORD*)( m_pBuffer + 4 );
					strOutput.Format( L"address: %s:%u", (LPCTSTR)CString( inet_ntoa( nIP ) ), nPort );
				}
				break;

			case ED2K_C2S_GETSOURCES:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					strOutput.Format( L"hash: %s", oMD4.toString() );
				}
				break;

			case ED2K_S2C_FOUNDSOURCES:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 1 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const BYTE& nCount = *(const BYTE*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					strOutput.Format( L"hash: %s, number: %u", oMD4.toString(), nCount );
					if ( nCount && m_nLength >= Hashes::Ed2kHash::byteCount + 1 + 4 + 2 )
					{
						const IN_ADDR& nID = *(const IN_ADDR*)( m_pBuffer + Hashes::Ed2kHash::byteCount + 1 );
						const WORD& nPort = *(const WORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount + 1 + 4 );
						strOutput.AppendFormat( L" -> id: %u, port: %u", nID.s_addr, nPort );
					}
				}
				break;

			case ED2K_S2C_IDCHANGE:
				if ( m_nLength >= 4 )
				{
					const DWORD& nClientID = *(const DWORD*)( m_pBuffer );
					strOutput.Format( L"new id: %u", nClientID );
					if ( m_nLength >= 8 )
					{
						const DWORD& nFlags = *(const DWORD*)( m_pBuffer + 4 );
						strOutput.AppendFormat( L", tcp flags: %s", GetED2KServerTCPFlags( nFlags ) );
						if ( m_nLength >= 12 )
						{
							const DWORD& nTCPPort = *(const DWORD*)( m_pBuffer + 8 );
							strOutput.AppendFormat( L", tcp port: %u", nTCPPort );
							if ( m_nLength >= 16 )
							{
								const DWORD& nIP = *(const DWORD*)( m_pBuffer + 12 );
								strOutput.AppendFormat( L", client ip: %u", nIP );
							}
						}
					}
				}
				break;

			case ED2K_S2C_SERVERLIST:
				if ( m_nLength >= 1 )
				{
					const BYTE& nCount = *(const BYTE*)( m_pBuffer );
					strOutput.Format( L"count: %u", nCount );
				}
				break;

			case ED2K_S2C_SERVERMESSAGE:
				if ( m_nLength >= 4 )
				{
					const WORD& nLen = *(const WORD*)( m_pBuffer );
					strOutput.Format( L"\"%s\" (%u bytes)", (LPCTSTR)UTF8Decode( (char*)m_pBuffer + 2, nLen ), nLen );
				}
				break;

			case ED2K_S2C_SEARCHRESULTS:
			case ED2K_C2S_OFFERFILES:
				if ( m_nLength >= 4 )
				{
					const DWORD& nCount = *(DWORD*)( m_pBuffer );
					strOutput.Format( L"number: %u", nCount );
					if ( nCount && m_nLength >= 4 + Hashes::Ed2kHash::byteCount + 4 + 2 + 4 )
					{
						// First hit only
						const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)( m_pBuffer + 4 ) );
						const IN_ADDR& nID = *(const IN_ADDR*)( m_pBuffer + 4 + Hashes::Ed2kHash::byteCount );
						const WORD& nPort = *(const WORD*)( m_pBuffer + 4 + Hashes::Ed2kHash::byteCount + 4 );
						strOutput.AppendFormat( L" -> hash: %s, id: %u, port: %u, tags: %s%s", oMD4.toString(), nID.s_addr, nPort,
							(LPCTSTR)CEDTag::ToString( m_pBuffer + 4 + Hashes::Ed2kHash::byteCount + 4 + 2, m_nLength - ( 4 + Hashes::Ed2kHash::byteCount + 4 + 2 ) ),
							( nCount > 1 ) ? L"..." : L"" );
					}
				}
				break;

			case ED2K_S2C_SERVERSTATUS:
				if ( m_nLength >= 4 + 4 )
				{
					const DWORD& nUsers = *(const DWORD*)( m_pBuffer );
					const DWORD& nFiles = *(const DWORD*)( m_pBuffer + 4 );
					strOutput.Format( L"users: %u, files: %u", nUsers, nFiles );
				}
				break;
			}
		}
	}
	else	// ! bServer
	{
		if ( m_bUDP )
		{
			switch ( m_nType )
			{
			case ED2K_C2C_UDP_REASKFILEPING:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					strOutput.Format( L"hash: %s", oMD4.toString() );
				}
				break;

			case ED2K_C2C_UDP_REASKACK:
				if ( m_nLength >= 2 + 2 )
				{
					const WORD& position = *(const WORD*)( m_pBuffer );
					const WORD& length = *(const WORD*)( m_pBuffer + 2 );
					strOutput.Format( L"position: %u, length: %u", position, length );
				}
				break;
			}
		}
		else if ( m_nEdProtocol == ED2K_PROTOCOL_EDONKEY )
		{
			switch ( m_nType )
			{
			case ED2K_C2C_HELLO:
				if ( m_nLength >= 1 + Hashes::Guid::byteCount + 4 + 2 + 4 )
				{
					const Hashes::Guid oGUID( *(const Hashes::Guid::RawStorage*)(m_pBuffer + 1) );
					const DWORD& nClientID = *(const DWORD*)( m_pBuffer + 1 + Hashes::Guid::byteCount );
					const WORD& nPort = *(const WORD*)( m_pBuffer + 1 + Hashes::Guid::byteCount + 4 );
					strOutput.Format( L"guid: %s, id: %u, port: %u, tags: %s", oGUID.toString(), nClientID, nPort,
						CEDTag::ToString( m_pBuffer + 1 + Hashes::Guid::byteCount + 4 + 2, m_nLength - ( 1 + Hashes::Guid::byteCount + 4 + 2 ) ) );
				}
				break;

			case ED2K_C2C_HELLOANSWER:
				if ( m_nLength >= Hashes::Guid::byteCount + 4 + 2 + 4 + 2 + 4 )
				{
					const Hashes::Guid oGUID( *(const Hashes::Guid::RawStorage*)(m_pBuffer) );
					const DWORD& nClientID = *(const DWORD*)( m_pBuffer + Hashes::Guid::byteCount );
					const WORD& nPort = *(const WORD*)( m_pBuffer + Hashes::Guid::byteCount + 4 );
					const IN_ADDR& nServerIP = *(const IN_ADDR*)( m_pBuffer + m_nLength - 2 - 4 );
					const WORD& nServerPort = *(const WORD*)( m_pBuffer + m_nLength - 2 );
					strOutput.Format( L"guid: %s, id: %u, port: %u, tags: %s, server: %s:%u", oGUID.toString(), nClientID, nPort,
						CEDTag::ToString( m_pBuffer + Hashes::Guid::byteCount + 4 + 2, m_nLength - ( Hashes::Guid::byteCount + 4 + 2 ) ),
						(LPCTSTR)CString( inet_ntoa( nServerIP ) ), nServerPort );
				}
				break;

			case ED2K_C2C_MESSAGE:
			case ED2K_C2C_VIEWSHAREDDIR:
			case ED2K_C2C_VIEWSHAREDDIRANSWER:
				if ( m_nLength >= 2 )
				{
					const WORD& nLen = *(const WORD*)( m_pBuffer );
					if ( m_nLength >= 2u + nLen )
						strOutput.Format( L"\"%s\" (%u bytes)", (LPCTSTR)UTF8Decode( (char*)m_pBuffer + 2, nLen ), nLen );
				}
				break;

			case ED2K_C2C_FILEREQANSWER:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 2 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const WORD& nLen = *(const WORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					if ( m_nLength >= Hashes::Ed2kHash::byteCount + 2u + nLen )
						strOutput.Format( L"hash: %s, name: \"%s\" (%u bytes)", oMD4.toString(), (LPCTSTR)UTF8Decode( (char*)m_pBuffer + Hashes::Ed2kHash::byteCount + 2, nLen ), nLen );
				}
				break;

			case ED2K_C2C_FILENOTFOUND:
			case ED2K_C2C_FILEREQUEST:
			case ED2K_C2C_QUEUEREQUEST:
			case ED2K_C2C_FILESTATUSREQUEST:
			case ED2K_C2C_HASHSETREQUEST:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					strOutput.Format( L"hash: %s", oMD4.toString() );
				}
				break;

			case ED2K_C2C_FILESTATUS:
			case ED2K_C2C_HASHSETANSWER:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 2 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const WORD& nBlocks = *(const WORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					strOutput.Format( L"hash: %s, blocks: %u", oMD4.toString(), nBlocks );
				}
				break;

			case ED2K_C2C_ASKSHAREDFILESANSWER:
			case ED2K_C2C_ASKSHAREDDIRSANSWER:
			case ED2K_C2C_QUEUERANK:
				if ( m_nLength >= 4 )
				{
					const DWORD& nCount = *(const DWORD*)( m_pBuffer );
					strOutput.Format( L"number: %u", nCount );
				}
				break;

			case ED2K_C2C_REQUESTPARTS:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 4 * 6 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const DWORD* offset = (const DWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					strOutput.Format( L"hash: %s, offsets: %u-%u, %u-%u, %u-%u", oMD4.toString(), offset[0], offset[3], offset[1], offset[4], offset[2], offset[5] );
				}
				break;

			case ED2K_C2C_SENDINGPART:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 4 * 2 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const DWORD* offset = (const DWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					strOutput.Format( L"hash: %s, offsets: %u, length: %u bytes", oMD4.toString(), offset[0], offset[1] - offset[0] );
				}
				break;
			}
		}
		else if ( m_nEdProtocol == ED2K_PROTOCOL_EMULE )
		{
			switch ( m_nType )
			{
			case ED2K_C2C_EMULEINFO:
			case ED2K_C2C_EMULEINFOANSWER:
				if ( m_nLength >= 1 + 1 + 4 )
				{
					const BYTE& nVers = *(const BYTE*)( m_pBuffer );
					const BYTE& nProto = *(const BYTE*)( m_pBuffer + 1 );
					strOutput.Format( L"version: %u, protocol: %u, tags: %s", nVers, nProto,
						CEDTag::ToString( m_pBuffer + 1 + 1, m_nLength - ( 1 + 1 ) ) );
				}
				break;

			case ED2K_C2C_QUEUERANKING:
				if ( m_nLength >= 2 + 2 )
				{
					const WORD& position = *(const WORD*)( m_pBuffer );
					const WORD& length = *(const WORD*)( m_pBuffer + 2 );
					strOutput.Format( L"position: %u, length: %u", position, length );
				}
				break;

			case ED2K_C2C_FILEDESC:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 2 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const WORD& nLen = *(const WORD*)( m_pBuffer + 16 );
					if ( m_nLength >= Hashes::Ed2kHash::byteCount + 2 + nLen )
						strOutput.Format( L"hash: %s, desc: \"%s\" (%u bytes)", oMD4.toString(), (LPCTSTR)UTF8Decode( (char*)m_pBuffer + 16 + 2, nLen ), nLen );
				}
				break;

			case ED2K_C2C_REQUESTSOURCES:
			case ED2K_C2C_REQUESTPREVIEW:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					strOutput.Format( L"hash: %s", oMD4.toString() );
				}
				break;

			case ED2K_C2C_REQUESTPARTS_I64:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 8 * 6 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const QWORD* offset = (const QWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					strOutput.Format( L"hash: %s, offsets: %I64u-%I64u, %I64u-%I64u, %I64u-%I64u", oMD4.toString(), offset[0], offset[3], offset[1], offset[4], offset[2], offset[5] );
				}
				break;

			case ED2K_C2C_COMPRESSEDPART:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 4 + 4 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const DWORD& offset = *(const DWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					const DWORD& length = *(const DWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount + 4 );
					strOutput.Format( L"hash: %s, offset: %u, length: %u bytes", oMD4.toString(), offset, length );
				}
				break;

			case ED2K_C2C_COMPRESSEDPART_I64:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 8 + 4 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const QWORD& offset = *(const QWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					const DWORD& length = *(const DWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount + 8 );
					strOutput.Format( L"hash: %s, offset: %I64u, length: %u bytes", oMD4.toString(), offset, length );
				}
				break;

			case ED2K_C2C_SENDINGPART_I64:
				if ( m_nLength >= Hashes::Ed2kHash::byteCount + 8 * 2 )
				{
					const Hashes::Ed2kHash oMD4( *(const Hashes::Ed2kHash::RawStorage*)m_pBuffer );
					const QWORD* offset = (const QWORD*)( m_pBuffer + Hashes::Ed2kHash::byteCount );
					strOutput.Format( L"hash: %s, offsets: %I64u, length: %I64u bytes", oMD4.toString(), offset[0], offset[1] - offset[0] );
				}
				break;
			}
		}
	}

	return strOutput.IsEmpty() ? CPacket::ToASCII() : strOutput;
}

#ifdef _DEBUG
void CEDPacket::Debug(LPCTSTR pszReason) const
{
	if ( m_nType == ED2K_C2C_SENDINGPART ||
		 m_nType == ED2K_C2C_HASHSETANSWER ||
		 m_nType == ED2K_C2C_COMPRESSEDPART )
		return;

	CString strOutput;
	strOutput.Format( L"[ED2K] %s Proto: 0x%x Type: %s", pszReason, int( m_nEdProtocol ), (LPCTSTR)GetType() );
	CPacket::Debug( strOutput );
}
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CEDTag construction

CEDTag::CEDTag()
	: m_nType	( ED2K_TAG_NULL )
	, m_nKey	( 0 )
{
}

CEDTag::CEDTag(const CEDTag& t)
	: m_nType	( t.m_nType )
	, m_sKey	( t.m_sKey )
	, m_nKey	( t.m_nKey )
	, m_sValue	( t.m_sValue )
	, m_nValue	( t.m_nValue )
	, m_oValue	( t.m_oValue )
{
}

CEDTag::CEDTag(BYTE nKey, const Hashes::Ed2kHash& oHash)
	: m_nType	( ED2K_TAG_HASH )
	, m_nKey	( nKey )
	, m_oValue	( oHash )
{
}

CEDTag::CEDTag(BYTE nKey, QWORD nValue)
	: m_nType	( ED2K_TAG_INT )
	, m_nKey 	( nKey )
	, m_nValue	( nValue )
{
}

CEDTag::CEDTag(BYTE nKey, LPCTSTR pszValue)
	: m_nType	( ED2K_TAG_STRING )
	, m_nKey	( nKey )
	, m_sValue	( pszValue )
{
}

CEDTag::CEDTag(LPCTSTR pszKey, QWORD nValue)
	: m_nType	( ED2K_TAG_INT )
	, m_sKey	( pszKey )
	, m_nKey	( 0 )
	, m_nValue	( nValue )
{
}

CEDTag::CEDTag(LPCTSTR pszKey, LPCTSTR pszValue)
	: m_nType	( ED2K_TAG_STRING )
	, m_sKey	( pszKey )
	, m_nKey	( 0 )
	, m_sValue	( pszValue )
{
}

CEDTag& CEDTag::operator=(const CEDTag& t)
{
	m_nType = t.m_nType;
	m_sKey  = t.m_sKey;
	m_nKey  = t.m_nKey;
	m_sValue = t.m_sValue;
	m_nValue = t.m_nValue;
	m_oValue = t.m_oValue;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// CEDTag operations

void CEDTag::Clear()
{
	m_nType		= ED2K_TAG_NULL;
	m_nKey		= 0;
	m_nValue	= 0;
	m_sKey.Empty();
	m_sValue.Empty();
	m_oValue.clear();
}

CString CEDTag::ToString() const
{
	CString strOutput;
	strOutput.Format( L"[0x%02x]{", m_nType );

	if ( m_sKey.IsEmpty() )
	{
		switch ( m_nKey )
		{
		case ED2K_CT_NAME:
			strOutput += L"name";
			break;

		case ED2K_FT_FILESIZE:
			strOutput += L"size";

		case ED2K_FT_DESCRIPTION:
			strOutput += L"info";
			break;

		case ED2K_FT_SOURCES:
			strOutput += L"sources";
			break;

		case ED2K_FT_COMPLETE_SOURCES:
			strOutput += L"complete";
			break;

		case ED2K_CT_VERSION:
			strOutput += L"version";
			break;

		case ED2K_CT_UDPPORTS:
			strOutput += L"ports";
			break;

		case ED2K_CT_FEATUREVERSIONS:
			strOutput += L"features1";
			break;

		case ED2K_CT_MOREFEATUREVERSIONS:
			strOutput += L"features2";
			break;

		case ED2K_CT_SOFTWAREVERSION:
			strOutput += L"client";
			break;

		case ED2K_CT_SERVER_FLAGS:
			strOutput += L"flags";
			break;

		default:
			strOutput.AppendFormat( L"0x%02x", m_nKey );
		}
	}
	else
		strOutput += m_sKey;

	strOutput += L'=';

	switch ( m_nType )
	{
	case ED2K_TAG_HASH:
		strOutput += m_oValue.toString();
		break;

	case ED2K_TAG_STRING:
		strOutput += L'\"' + m_sValue + L'\"';
		break;

	case ED2K_TAG_INT:
		if ( m_nKey == 0xFB )
			strOutput.AppendFormat( L"%u.%u.%u.%u", (BYTE)( m_nValue >> 17 ) & 0x7f, (BYTE)( m_nValue >> 10 ) & 0x7f, (BYTE)( m_nValue >> 7 ) & 0x7, (BYTE)m_nValue & 0x7f );
		else
			strOutput.AppendFormat( L"%I64u", m_nValue );
		break;
	}

	strOutput += L"} ";

	return strOutput;
}

CString CEDTag::ToString(const BYTE* pData, DWORD nLength)
{
	CString strOutput;
	if ( pData && nLength )
	{
		if ( CEDPacket* pTempPacket = CEDPacket::New( (BYTE)0 ) )
		{
			pTempPacket->Write( pData, nLength );
			for ( DWORD nTags = pTempPacket->ReadLongLE(); nTags > 0 && pTempPacket->GetRemaining() > 1; --nTags )
			{
				CEDTag pTag;
				if ( ! pTag.Read( pTempPacket, TRUE ) )
					break;
				strOutput += pTag.ToString();
			}
			pTempPacket->Release();
		}
	}
	return strOutput;
}

//////////////////////////////////////////////////////////////////////
// CEDTag write to packet

void CEDTag::Write(CEDPacket* pPacket, BOOL bUnicode, BOOL bSmallTags)
{
	DWORD nPos = pPacket->m_nLength;

	pPacket->WriteByte( m_nType );

	if ( int nKeyLen = m_sKey.GetLength() )
	{
		pPacket->WriteEDString( m_sKey, bUnicode );
	}
	else
	{
		if ( ! bSmallTags )
			pPacket->WriteShortLE( 1 );
		pPacket->WriteByte( m_nKey );
	}

	if ( m_nType == ED2K_TAG_STRING )
	{
		if ( bSmallTags )	// When supporting small tags
		{
			int nLength;
			if ( bUnicode )
				nLength = pPacket->GetStringLenUTF8( m_sValue );
			else
				nLength = pPacket->GetStringLen( m_sValue );

			if ( nLength <= 16 )
			{
				// We should use a 'short' string tag
				// Correct the packet type
				pPacket->m_pBuffer[nPos] = BYTE( 0x80 | ( ( ED2K_TAG_SHORTSTRING - 1 ) + nLength ) );

				// Write the string
				if ( bUnicode )
					pPacket->WriteStringUTF8( m_sValue, FALSE );
				else
					pPacket->WriteString( m_sValue, FALSE );
			}
			else
			{	// We should use a normal string tag
				// Correct the packet type
				pPacket->m_pBuffer[nPos] = 0x80 | ED2K_TAG_STRING ;

				// Write the string
				pPacket->WriteEDString( m_sValue, bUnicode );
			}
		}
		else
		{
			// Write the string
			pPacket->WriteEDString( m_sValue, bUnicode );
		}
	}
	else if ( m_nType == ED2K_TAG_INT )
	{
		if ( bSmallTags )	// When supporting small tags
		{	// Use a short tag
			if ( m_nValue <= 0xFF )
			{	// Use a 8 bit int
				pPacket->m_pBuffer[nPos] = 0x80 | ED2K_TAG_UINT8;
				// Write a byte
				pPacket->WriteByte( (BYTE)m_nValue );
			}
			else if ( m_nValue <= 0xFFFF )
			{	// Use a 16 bit int
				pPacket->m_pBuffer[nPos] = 0x80 | ED2K_TAG_UINT16;
				// Write a word
				pPacket->WriteShortLE( (WORD)m_nValue );
			}
			else if ( m_nValue <= 0xFFFFFFFF )
			{	// Use a 32 bit int
				pPacket->m_pBuffer[nPos] = 0x80 | ED2K_TAG_INT;
				// Write a DWORD
				pPacket->WriteLongLE( (DWORD)m_nValue );
			}
			else
			{	// Use a 64 bit int
				pPacket->m_pBuffer[nPos] = 0x80 | ED2K_TAG_UINT64;
				// Write a QWORD
				pPacket->WriteInt64( m_nValue );
			}
		}
		else
		{	// Use a normal int
			ASSERT( m_nValue <= 0xFFFFFFFF );
			// Write a DWORD
			pPacket->WriteLongLE( (DWORD)m_nValue );
		}
	}
	else
	{
		ASSERT( FALSE );	// Unsupported tag
	}
}

//////////////////////////////////////////////////////////////////////
// CEDTag read from packet

BOOL CEDTag::Read(CEDPacket* pPacket, BOOL bUnicode)
{
	WORD nLen;

	Clear();

	if ( pPacket->GetRemaining() < 3 ) return FALSE;

	m_nType = pPacket->ReadByte();

	if ( m_nType & 0x80 )
	{
		m_nType &= 0x7F;
		nLen = 1;
	}
	else
	{
		nLen = pPacket->ReadShortLE();
	}

	if ( pPacket->GetRemaining() < nLen ) return FALSE;

	if ( nLen == 1 )
	{
		m_nKey = pPacket->ReadByte();
	}
	else if ( nLen > 1 )
	{
		if ( bUnicode )
			m_sKey = pPacket->ReadStringUTF8( nLen );
		else
			m_sKey = pPacket->ReadStringASCII( nLen );
	}

	switch ( m_nType )
	{
	case ED2K_TAG_HASH:
		if ( pPacket->GetRemaining() < 16 ) return FALSE;
		pPacket->Read( &m_oValue[ 0 ], Hashes::Ed2kHash::byteCount );
		m_oValue.validate();
		break;

	case ED2K_TAG_STRING:
		if ( pPacket->GetRemaining() < 2 ) return FALSE;
		nLen = pPacket->ReadShortLE();
		if ( pPacket->GetRemaining() < nLen ) return FALSE;
		if ( bUnicode )
			m_sValue = pPacket->ReadStringUTF8( nLen );
		else
			m_sValue = pPacket->ReadStringASCII( nLen );
		break;

	case ED2K_TAG_BLOB:
		if ( pPacket->GetRemaining() < 4 ) return FALSE;
		{
			DWORD nLenBlob = pPacket->ReadLongLE();
			if ( pPacket->GetRemaining() < nLenBlob ) return FALSE;
			m_sValue = pPacket->ReadStringASCII( nLenBlob );
		}
		break;

	case ED2K_TAG_INT:
		if ( pPacket->GetRemaining() < 4 ) return FALSE;
		m_nValue = pPacket->ReadLongLE();
		break;

	case ED2K_TAG_FLOAT:
		if ( pPacket->GetRemaining() < 4 ) return FALSE;
		m_nValue = pPacket->ReadLongLE();
		break;

	case ED2K_TAG_UINT16:
		if ( pPacket->GetRemaining() < 2 ) return FALSE;
		m_nValue = pPacket->ReadShortLE();
		m_nType = ED2K_TAG_INT;
		break;

	case ED2K_TAG_UINT8:
		if ( pPacket->GetRemaining() < 1 ) return FALSE;
		m_nValue = pPacket->ReadByte();
		m_nType = ED2K_TAG_INT;
		break;

	case ED2K_TAG_UINT64:
		if ( pPacket->GetRemaining() < 1 ) return FALSE;
		m_nValue = pPacket->ReadInt64();
		m_nType = ED2K_TAG_INT;
		break;

	default:
		if ( m_nType >= ED2K_TAG_SHORTSTRING && m_nType <= ED2K_TAG_SHORTSTRING + 15 )
		{
			nLen = m_nType - ( ED2K_TAG_SHORTSTRING - 1 );
			m_nType = ED2K_TAG_STRING;
			if ( pPacket->GetRemaining() < nLen ) return FALSE;
			if ( bUnicode )
				m_sValue = pPacket->ReadStringUTF8( nLen );
			else
				m_sValue = pPacket->ReadStringASCII( nLen );
		}
		else
		{
			theApp.Message( MSG_DEBUG, L"Unknown ED2K tag type 0x%02x", m_nType );
			return FALSE;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CEDTag read from file

BOOL CEDTag::Read(CFile* pFile)
{
	WORD nLen;

	Clear();

	if ( pFile->Read( &m_nType, sizeof( m_nType ) ) != sizeof( m_nType ) )
		return FALSE;

	if ( m_nType & 0x80 )
	{
		m_nType &= 0x7F;
		nLen = 1;
	}
	else
	{
		if ( pFile->Read( &nLen, sizeof( nLen ) ) != sizeof( nLen ) )
			return FALSE;
	}

	if ( nLen == 1 )
	{
		if ( pFile->Read( &m_nKey, sizeof( m_nKey ) ) != sizeof( m_nKey ) )
			return FALSE;
	}
	else if ( nLen > 1 )
	{
		auto_array< CHAR > psz( new CHAR[ nLen + 1 ] );
		if ( ! psz.get() )
			return FALSE;
		if ( pFile->Read( psz.get(), nLen ) != nLen )
			return FALSE;
		psz[ nLen ] = 0;
		m_sKey = psz.get();
	}

	switch ( m_nType )
	{
	case ED2K_TAG_HASH:
		if ( pFile->Read( &m_oValue[ 0 ], Hashes::Ed2kHash::byteCount ) != Hashes::Ed2kHash::byteCount )
			return FALSE;
		m_oValue.validate();
		break;

	case ED2K_TAG_STRING:
		if ( pFile->Read( &nLen, sizeof( nLen ) ) != sizeof( nLen ) )
			return FALSE;
		{
			auto_array< CHAR > psz( new CHAR[ nLen ] );
			if ( ! psz.get() )
				return FALSE;
			if ( pFile->Read( psz.get(), nLen ) != nLen )
				return FALSE;
			m_sValue = UTF8Decode( psz.get(), nLen );
		}
		break;

	case ED2K_TAG_BLOB:
		{
			DWORD nBlolbLen;
			if ( pFile->Read( &nBlolbLen, sizeof( nBlolbLen ) ) != sizeof( nBlolbLen ) )
				return FALSE;
			auto_array< CHAR > psz( new CHAR[ nBlolbLen ] );
			if ( ! psz.get() )
				return FALSE;
			if ( pFile->Read( psz.get(), nBlolbLen ) != nBlolbLen )
				return FALSE;
			CopyMemory( m_sValue.GetBuffer( ( nBlolbLen + 1 ) / sizeof( TCHAR ) ), psz.get(), nBlolbLen );
			m_sValue.ReleaseBuffer( ( nBlolbLen + 1 ) / sizeof( TCHAR ) );
		}
		break;

	case ED2K_TAG_INT:
	case ED2K_TAG_FLOAT:
		{
			DWORD nValue;
			if ( pFile->Read( &nValue, sizeof( nValue ) ) != sizeof( nValue ) )
				return FALSE;
			m_nValue = nValue;
		}
		break;

	case ED2K_TAG_UINT16:
		{
			WORD nValue;
			if ( pFile->Read( &nValue, sizeof( nValue ) ) != sizeof( nValue ) )
				return FALSE;
			m_nValue = nValue;
			m_nType = ED2K_TAG_INT;
		}
		break;

	case ED2K_TAG_UINT8:
		{
			BYTE nValue;
			if ( pFile->Read( &nValue, sizeof( nValue ) ) != sizeof( nValue ) )
				return FALSE;
			m_nValue = nValue;
			m_nType = ED2K_TAG_INT;
		}
		break;

	case ED2K_TAG_UINT64:
		if ( pFile->Read( &m_nValue, sizeof( m_nValue ) ) != sizeof( m_nValue ) )
			return FALSE;
		m_nType = ED2K_TAG_INT;
		break;

	default:
		if ( m_nType >= ED2K_TAG_SHORTSTRING && m_nType <= ED2K_TAG_SHORTSTRING + 15 )
		{
			// Calculate length of short string
			nLen = m_nType - ( ED2K_TAG_SHORTSTRING - 1 );
			m_nType = ED2K_TAG_STRING;
			auto_array< CHAR > psz( new CHAR[ nLen ] );
			if ( ! psz.get() )
				return FALSE;
			if ( pFile->Read( psz.get(), nLen ) != nLen )
				return FALSE;
			m_sValue = UTF8Decode( psz.get(), nLen );
		}
		else
		{
			theApp.Message( MSG_DEBUG, L"Unknown ED2K tag type 0x%02x", m_nType );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CEDPacket::OnPacket(const SOCKADDR_IN* pHost)
{
	Statistics.Current.eDonkey.Incoming++;

	switch ( m_nEdProtocol )
	{
	case ED2K_PROTOCOL_EDONKEY:
	case ED2K_PROTOCOL_EMULE:
		return EDClients.OnPacket( pHost, this );

	case ED2K_PROTOCOL_KAD:
		return Kademlia.OnPacket( pHost, this );
#ifdef _DEBUG
	case ED2K_PROTOCOL_REVCONNECT:
		// ToDo: Implement RevConnect KAD
		Debug( L"RevConnect KAD not implemented." );
		break;
	default:
		Debug( L"Unknown ED2K/KAD Protocol." );
#endif	// Debug
	}

	return FALSE;
}
