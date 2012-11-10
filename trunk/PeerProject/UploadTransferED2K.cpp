//
// UploadTransferED2K.cpp
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
#include "UploadTransferED2K.h"
#include "UploadFile.h"
#include "UploadQueue.h"
#include "UploadQueues.h"
#include "Statistics.h"
#include "Datagrams.h"
#include "EDClient.h"
#include "EDPacket.h"

#include "Buffer.h"
#include "Library.h"
#include "Download.h"
#include "Downloads.h"
#include "SharedFile.h"
#include "TransferFile.h"
#include "FragmentedFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug


//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K construction

CUploadTransferED2K::CUploadTransferED2K(CEDClient* pClient)
	: CUploadTransfer( PROTOCOL_ED2K )
	, m_pClient			( pClient )
	, m_nRanking		( 0 )
	, m_tRankingSent	( 0 )
	, m_tRankingCheck	( 0 )
	, m_tLastRun		( 0 )
{
	ASSERT( pClient != NULL );

	m_pHost				= pClient->m_pHost;
	m_sAddress			= pClient->m_sAddress;
	UpdateCountry();

	m_sUserAgent		= pClient->m_sUserAgent;
	m_bClientExtended	= pClient->m_bClientExtended;
	m_sRemoteNick		= pClient->m_sNick;
	m_nState			= upsReady;

	m_pClient->m_mOutput.pLimit = &m_nBandwidth;
}

CUploadTransferED2K::~CUploadTransferED2K()
{
	ASSERT( m_pClient == NULL );
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K request

BOOL CUploadTransferED2K::Request(const Hashes::Ed2kHash& oED2K)
{
	ASSERT( oED2K );
	BOOL bSame = validAndEqual( m_oED2K, oED2K );

	Cleanup( ! bSame );

	CSingleLock oLock( &Library.m_pSection );
	BOOL bLocked = oLock.Lock( 1000 );
	if ( CLibraryFile* pFile = ( bLocked ? LibraryMaps.LookupFileByED2K( oED2K, TRUE, TRUE ) : NULL ) )
	{
		// Send comments if necessary
		if ( m_pClient ) m_pClient->SendCommentsPacket( pFile->m_nRating, pFile->m_sComments );

		RequestComplete( pFile );
		oLock.Unlock();
	}
	else
	{
		if ( bLocked )
			oLock.Unlock();

		if ( CDownload* pFile = Downloads.FindByED2K( oED2K, TRUE ) )
		{
			RequestPartial( pFile );
		}
		else
		{
			UploadQueues.Dequeue( this );

			theApp.Message( MSG_ERROR, IDS_UPLOAD_FILENOTFOUND, (LPCTSTR)m_sAddress, (LPCTSTR)oED2K.toUrn() );

			CEDPacket* pReply = CEDPacket::New( ED2K_C2C_FILENOTFOUND );
			pReply->Write( oED2K );
			Send( pReply );

			Close();
			return FALSE;
		}
	}

	if ( UploadQueues.GetPosition( this, FALSE ) < 0 && ! UploadQueues.Enqueue( this ) )
	{
		theApp.Message( MSG_ERROR, IDS_UPLOAD_BUSY_QUEUE, (LPCTSTR)m_sName, (LPCTSTR)m_sAddress, _T("ED2K") );

		CEDPacket* pReply = CEDPacket::New( ED2K_C2C_FILENOTFOUND );
		pReply->Write( oED2K );
		Send( pReply );

		Close();
		return FALSE;
	}

	AllocateBaseFile();

	theApp.Message( MSG_NOTICE, IDS_UPLOAD_FILE, (LPCTSTR)m_sName, (LPCTSTR)m_sAddress );

	m_nRanking = -1;
	return CheckRanking();
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K close

void CUploadTransferED2K::Close(UINT nError)
{
	if ( m_nState == upsNull )
	{
		if ( m_pClient != NULL ) m_pClient->OnUploadClose();
		m_pClient = NULL;
		return;
	}

	if ( m_pBaseFile != NULL && m_pClient->IsOnline() )
	{
		if ( m_nState == upsUploading || m_nState == upsQueued )
			Send( CEDPacket::New( ED2K_C2C_FINISHUPLOAD ) );

		CEDPacket* pPacket = CEDPacket::New( ED2K_C2C_FILENOTFOUND );
		pPacket->Write( m_oED2K );
		Send( pPacket );
	}

	Cleanup();

	ASSERT( m_pClient != NULL );
	m_pClient->OnUploadClose();
	m_pClient = NULL;

	CUploadTransfer::Close( nError );
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K run

BOOL CUploadTransferED2K::OnRun()
{
	return OnRunEx( GetTickCount() );
}

BOOL CUploadTransferED2K::OnRunEx(DWORD tNow)
{
	// Limit per-source packet rate
	if ( tNow < m_tLastRun + Settings.eDonkey.SourceThrottle ) return FALSE;
	m_tLastRun = tNow;

	if ( m_nState == upsQueued )
	{
		if ( m_pClient->IsOnline() == FALSE && tNow > m_tRequest &&
			 tNow >= m_tRequest + Settings.eDonkey.DequeueTime * 1000 )
		{
			Close( IDS_UPLOAD_QUEUE_TIMEOUT );
			return FALSE;
		}

		DWORD nCheckThrottle;	// Throttle for how often ED2K clients have queue rank checked
		if ( m_nRanking <= 2 )
			nCheckThrottle = 2 * 1000;
		else if ( m_nRanking < 10 )
			nCheckThrottle = 15 * 1000;
		else if ( m_nRanking < 50 )
			nCheckThrottle = 1 * 60 * 1000;
		else if ( m_nRanking < 200 )
			nCheckThrottle = 4 * 60 * 1000;
		else // Over 200 at 8 minutes
			nCheckThrottle = 8 * 60 * 1000;

		if ( tNow > m_tRankingCheck && tNow >= m_tRankingCheck + nCheckThrottle )
		{
			// Check the queue rank. Start upload or send rank update if required.
			if ( ! CheckRanking() )
				return FALSE;
		}
	}
	else if ( m_nState == upsUploading )
	{
		if ( ! ServeRequests() )
			return FALSE;

		if ( tNow > m_pClient->m_mOutput.tLast &&	// Is this necessary?
			 tNow > m_pClient->m_mOutput.tLast + Settings.Connection.TimeoutTraffic * 3 )
		{
			Close( IDS_UPLOAD_TRAFFIC_TIMEOUT );
			return FALSE;
		}
	}
	else if ( m_nState == upsReady || m_nState == upsRequest )
	{
		if ( tNow > m_tRequest && tNow > m_tRequest + Settings.Connection.TimeoutHandshake )
		{
			Close( IDS_UPLOAD_REQUEST_TIMEOUT );
			return FALSE;
		}
	}
	else if ( m_nState == upsConnecting )
	{
		if ( tNow > m_tRequest && tNow > m_tRequest + Settings.Connection.TimeoutConnect + Settings.Connection.TimeoutHandshake + 10000 )
		{
			Close( IDS_UPLOAD_DROPPED );
			return FALSE;
		}
	}

	return CUploadTransfer::OnRun();
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K connection establishment

BOOL CUploadTransferED2K::OnConnected()
{
	if ( m_nState != upsConnecting ) return TRUE;

	m_tRequest = GetTickCount();
	m_nRanking = -1;

	m_pClient->m_mOutput.pLimit = &m_nBandwidth;

	return CheckRanking();
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K connection lost

void CUploadTransferED2K::OnDropped()
{
	if ( m_nState == upsQueued )
	{
		theApp.Message( MSG_INFO, IDS_UPLOAD_QUEUE_DROP, (LPCTSTR)m_sAddress );

		m_tRequest = GetTickCount();
		m_oRequested.clear();
		m_oServed.clear();
	}
	else
	{
		Close( IDS_UPLOAD_DROPPED );
	}
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K queue drop hook

void CUploadTransferED2K::OnQueueKick()
{
	m_nRanking = -1;

	if ( m_nState == upsRequest || m_nState == upsUploading )
	{
		if ( UploadQueues.GetPosition( this, TRUE ) == 0 ) return;

		if ( m_pBaseFile != NULL && m_pClient->IsOnline() )
			Send( CEDPacket::New( ED2K_C2C_FINISHUPLOAD ) );

		Cleanup( FALSE );
	}
	else
	{
		CheckRanking();
	}
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K speed measurement

DWORD CUploadTransferED2K::GetMeasuredSpeed()
{
	// Return if there is no client
	if ( m_pClient == NULL ) return 0;

	// Calculate Output
	m_pClient->MeasureOut();

	// Return calculated speed
	return m_pClient->m_mOutput.nMeasure;
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K queue release

BOOL CUploadTransferED2K::OnQueueRelease(CEDPacket* /*pPacket*/)
{
	Cleanup();
	Close( IDS_UPLOAD_DROPPED );
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K part request

BOOL CUploadTransferED2K::OnRequestParts(CEDPacket* pPacket)
{
	if ( pPacket->GetRemaining() < Hashes::Ed2kHash::byteCount + 4 * 3 * 2 )
	{
		theApp.Message( MSG_ERROR, IDS_ED2K_CLIENT_BAD_PACKET, (LPCTSTR)m_sAddress, pPacket->m_nType );
		Close();
		return FALSE;
	}

	Hashes::Ed2kHash oED2K;
	pPacket->Read( oED2K );

	if ( validAndUnequal( oED2K, m_oED2K ) )
	{
		if ( ! Request( oED2K ) ) return FALSE;
	}

	if ( m_nState != upsQueued && m_nState != upsRequest && m_nState != upsUploading )
	{
		theApp.Message( MSG_ERROR, IDS_ED2K_CLIENT_BAD_PACKET, (LPCTSTR)m_sAddress, pPacket->m_nType );
		Close();
		return FALSE;
	}

	DWORD nOffset[2][3];
	pPacket->Read( nOffset, sizeof(DWORD) * 2 * 3 );

	for ( int nRequest = 0 ; nRequest < 3 ; nRequest++ )
	{
		if ( nOffset[1][nRequest] <= m_nSize )
		{
			// Valid (or null) request, Add non-null ranges to the list
			if ( nOffset[0][nRequest] < nOffset[1][nRequest] )
				AddRequest( nOffset[0][nRequest], nOffset[1][nRequest] - nOffset[0][nRequest] );
		}
		else
		{
			// Invalid request- had an impossible range.
			theApp.Message( MSG_ERROR, IDS_UPLOAD_BAD_RANGE, (LPCTSTR)m_sAddress, (LPCTSTR)m_sName );

			// They probably have an incorrent hash associated with a file.
			// Calling close now will send "file not found" to stop them re-asking, then close the connection.
			Close();
			return FALSE;
		}
	}

	ServeRequests();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K cleanup

void CUploadTransferED2K::Cleanup(BOOL bDequeue)
{
	if ( bDequeue )
		UploadQueues.Dequeue( this );

	if ( m_nState == upsUploading )
	{
		ASSERT( m_pBaseFile != NULL );
		if ( m_nLength < SIZE_UNKNOWN )
			m_pBaseFile->AddFragment( m_nOffset, m_nPosition );

		CloseFile();
	}

	ClearRequest();

	m_oRequested.clear();

	m_oServed.clear();

	m_pBaseFile	= NULL;
	m_nState	= upsReady;
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K send

void CUploadTransferED2K::Send(CEDPacket* pPacket, BOOL bRelease)
{
	ASSERT( m_nState != upsNull );
	ASSERT( m_pClient != NULL );
	m_pClient->Send( pPacket, bRelease );
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K request a fragment

void CUploadTransferED2K::AddRequest(QWORD nOffset, QWORD nLength)
{
	ASSERT( m_pBaseFile != NULL );

	Fragments::Fragment oRequest( nOffset, nOffset + nLength );

	if ( std::find( m_oRequested.begin(), m_oRequested.end(), oRequest ) == m_oRequested.end() )
		m_oRequested.push_back( oRequest );
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K serve requests

BOOL CUploadTransferED2K::ServeRequests()
{
	if ( m_nState != upsUploading && m_nState != upsRequest )
		return TRUE;

	if ( ! m_pClient || ! m_pClient->IsOutputExist() ) return TRUE;
	if ( m_pClient->GetOutputLength() > 512000ul ) return TRUE;		// 500 KB (Was Settings.eDonkey.RequestSize) ToDo: 250000ul Lugdunum max?

	ASSERT( m_pBaseFile != NULL );

	if ( m_nLength == SIZE_UNKNOWN )
	{
		// Check has just finished
		if ( m_bStopTransfer )
		{
			m_tRotateTime = 0;
			m_bStopTransfer	= FALSE;

			if ( CUploadQueue* pQueue = m_pQueue )
			{
				pQueue->Dequeue( this );
				pQueue->Enqueue( this, TRUE, FALSE );
			}

			int nQpos = UploadQueues.GetPosition( this, TRUE );
			if ( nQpos != 0 )
			{
				if ( m_pBaseFile != NULL && m_pClient->IsOnline() )
					Send( CEDPacket::New( ED2K_C2C_FINISHUPLOAD ) );

				if ( nQpos > 0 )	// If we aren't uploading any more (the queue wasn't empty)
				{
					// Set state to queued, and reset ranking to send a queue ranking packet.
					m_tRequest = GetTickCount();
					m_nState = upsQueued;
					m_nRanking = -1;
				}

				return TRUE;
			}
		}

		ASSERT( m_nState == upsRequest || m_nState == upsUploading );
		ASSERT( m_pBaseFile != NULL );

		if ( ! IsFileOpen() && ! OpenFile() )
		{
			theApp.Message( MSG_ERROR, IDS_UPLOAD_CANTOPEN, (LPCTSTR)m_sName, (LPCTSTR)m_sAddress );

			CEDPacket* pReply = CEDPacket::New( ED2K_C2C_FILENOTFOUND );
			pReply->Write( m_oED2K );
			Send( pReply );

			Cleanup();
			Close();
			return FALSE;
		}

		ASSERT( ! m_pBaseFile->m_sPath.IsEmpty() );
		PostMainWndMessage( WM_NOWUPLOADING, 0, (LPARAM)new CString( m_pBaseFile->m_sPath ) );

		if ( ! StartNextRequest() )
			return FALSE;
	}

	if ( m_nLength != SIZE_UNKNOWN )
	{
		if ( DispatchNextChunk() )
		{
			CheckFinishedRequest();

			if ( ! Settings.eDonkey.Enabled && Settings.Connection.RequireForTransfers )
			{
				Send( CEDPacket::New( ED2K_C2C_FINISHUPLOAD ) );
				Cleanup();
				Close();
				return FALSE;
			}
		}
		else
		{
			Cleanup();
			Close();
			return FALSE;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K start the next request

BOOL CUploadTransferED2K::StartNextRequest()
{
	ASSERT( m_nState == upsUploading || m_nState == upsRequest );
	ASSERT( IsFileOpen() );

	while ( ! m_oRequested.empty() && m_nLength == SIZE_UNKNOWN )
	{
		if ( std::find( m_oServed.begin(), m_oServed.end(), *m_oRequested.begin() ) == m_oServed.end()
			// This should be redundant (Camper)
			&& m_oRequested.begin()->begin() < m_nSize
			&& m_oRequested.begin()->end() <= m_nSize )
		{
			m_nOffset = m_oRequested.begin()->begin();
			m_nLength = m_oRequested.begin()->size();
			m_nPosition = 0;
		}
		m_oRequested.pop_front();
	}

	if ( m_nLength < SIZE_UNKNOWN )
	{
		if ( ! Settings.eDonkey.Enabled && Settings.Connection.RequireForTransfers )
		{
			Send( CEDPacket::New( ED2K_C2C_FILENOTFOUND ) );
			Cleanup();
			Close();
			return FALSE;
		}

		m_nState	= upsUploading;
		m_tContent	= m_pClient->m_mOutput.tLast = GetTickCount();

		theApp.Message( MSG_INFO, IDS_UPLOAD_CONTENT, m_nOffset, m_nOffset + m_nLength - 1, (LPCTSTR)m_sName, (LPCTSTR)m_sAddress, (LPCTSTR)m_sUserAgent );

		return TRUE;
	}
	else
	{
		Send( CEDPacket::New( ED2K_C2C_FINISHUPLOAD ) );
		Cleanup();
		Close( IDS_UPLOAD_DROPPED );
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K chunk dispatch

BOOL CUploadTransferED2K::DispatchNextChunk()
{
	ASSERT( m_nState == upsUploading );

	if ( ! IsFileOpen() )
		return FALSE;

	ASSERT( m_nLength < SIZE_UNKNOWN );
	ASSERT( m_nPosition < m_nLength );

	QWORD nPacket = m_nLength - m_nPosition;
	if ( nPacket > 1024000 ) nPacket = 1024000;		// 1000 KB

	while ( nPacket )
	{
		QWORD nChunk = min( nPacket, (QWORD)Settings.eDonkey.FrameSize );		// 10 KB limit for eMule (~512 others)
		const QWORD nOffset =	m_nOffset + m_nPosition;
		const bool bI64Offset =	( nOffset & 0xffffffff00000000 ) ||
								( ( nOffset + nChunk ) & 0xffffffff00000000 );

#if 0
// ToDo: Why is this here but unused?
//		// Use packet form
//		if ( bI64Offset )
//		{
//			CEDPacket* pPacket = CEDPacket::New( ED2K_C2C_SENDINGPART_I64, ED2K_PROTOCOL_EMULE );
//			if ( ! pPacket )
//				return FALSE;	// Out of memory
//
//			pPacket->Write( m_oED2K );
//			pPacket->WriteLongLE( nOffset & 0x00000000ffffffff );
//			pPacket->WriteLongLE( ( nOffset & 0xffffffff00000000 ) >> 32);
//			pPacket->WriteLongLE( ( nOffset + nChunk ) & 0x00000000ffffffff );
//			pPacket->WriteLongLE( ( ( nOffset + nChunk ) & 0xffffffff00000000 ) >> 32);
//
//			// SetFilePointer( hFile, m_nFileBase + nOffset, NULL, FILE_BEGIN );
//			// ReadFile( hFile, pPacket->WriteGetPointer( nChunk ), nChunk, &nChunk, NULL );
//
//			if ( ! ReadFile( m_nFileBase + nOffset, pPacket->GetWritePointer( nChunk ), nChunk, &nChunk ) || nChunk == 0 )
//			{
//				pPacket->Release();
//				return FALSE;
//			}
//
//			pPacket->m_nLength = sizeof(MD4) + 16 + nChunk;
//
//			Send( pPacket );
//		}
//		else
//		{
//			CEDPacket* pPacket = CEDPacket::New( ED2K_C2C_SENDINGPART );
//			pPacket->Write( m_oED2K );
//			pPacket->WriteLongLE( nOffset );
//			pPacket->WriteLongLE( nOffset + nChunk );
//
//			// SetFilePointer( hFile, m_nFileBase + nOffset, NULL, FILE_BEGIN );
//			// ReadFile( hFile, pPacket->WriteGetPointer( nChunk ), nChunk, &nChunk, NULL );
//
//			if ( ! ReadFile( m_nFileBase + nOffset, pPacket->GetWritePointer( nChunk ), nChunk, &nChunk ) || nChunk == 0 )
//			{
//				pPacket->Release();
//				return FALSE;
//			}
//
//			pPacket->m_nLength = sizeof(MD4) + 8 + nChunk;
//
//			Send( pPacket );
//		}
//
#else
		// Raw write
		CBuffer pBuffer;
		if ( bI64Offset )
		{
			if ( ! pBuffer.EnsureBuffer( sizeof(ED2K_PART_HEADER_I64) + nChunk ) )
				return FALSE;	// Out of memory

			ED2K_PART_HEADER_I64* pHeader = (ED2K_PART_HEADER_I64*)( pBuffer.m_pBuffer + pBuffer.m_nLength );

			// SetFilePointer( hFile, m_nFileBase + nOffset, NULL, FILE_BEGIN );
			// ReadFile( hFile, &pHeader[1], nChunk, &nChunk, NULL );

			if ( ! ReadFile( m_nFileBase + nOffset, &pHeader[1], nChunk, &nChunk ) || nChunk == 0 )
				return FALSE;	// File error

			pHeader->nProtocol	= ED2K_PROTOCOL_EMULE;
			pHeader->nType		= ED2K_C2C_SENDINGPART_I64;
			pHeader->nLength	= (DWORD)( 1 + m_oED2K.byteCount + 16 + nChunk );
			CopyMemory( &*pHeader->pMD4.begin(), &*m_oED2K.begin(), m_oED2K.byteCount );
			pHeader->nOffset1	= nOffset;
			pHeader->nOffset2	= nOffset + nChunk;

			pBuffer.m_nLength += (DWORD)( sizeof(ED2K_PART_HEADER_I64) + nChunk );
		}
		else
		{
			if ( ! pBuffer.EnsureBuffer( sizeof(ED2K_PART_HEADER) + nChunk ) )
				return FALSE;	// Out of memory

			ED2K_PART_HEADER* pHeader = (ED2K_PART_HEADER*)( pBuffer.m_pBuffer + pBuffer.m_nLength );

			// SetFilePointer( hFile, m_nFileBase + nOffset, NULL, FILE_BEGIN );
			// ReadFile( hFile, &pHeader[1], nChunk, &nChunk, NULL );

			if ( ! ReadFile( m_nFileBase + nOffset, &pHeader[1], nChunk, &nChunk ) || nChunk == 0 )
				return FALSE;	// File error

			pHeader->nProtocol	= ED2K_PROTOCOL_EDONKEY;
			pHeader->nType		= ED2K_C2C_SENDINGPART;
			pHeader->nLength	= (DWORD)( 1 + m_oED2K.byteCount + 8 + nChunk );
			CopyMemory( &*pHeader->pMD4.begin(), &*m_oED2K.begin(), m_oED2K.byteCount );
			pHeader->nOffset1	= (DWORD)nOffset;
			pHeader->nOffset2	= (DWORD)( nOffset + nChunk );

			pBuffer.m_nLength += (DWORD)( sizeof(ED2K_PART_HEADER) + nChunk );
		}

		m_pClient->Write( &pBuffer );

#endif // 0

		nPacket -= nChunk;

		m_nPosition += nChunk;
		m_nUploaded += nChunk;

		Statistics.Current.Uploads.Volume += ( nChunk / 1024 );
	}

	m_pClient->Send( NULL );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K request

BOOL CUploadTransferED2K::CheckFinishedRequest()
{
	ASSERT( m_nState == upsUploading );

	if ( m_nPosition < m_nLength &&
		( Settings.eDonkey.Enabled ||
		! Settings.Connection.RequireForTransfers ) )
		return FALSE;

	theApp.Message( MSG_INFO, IDS_UPLOAD_FINISHED, (LPCTSTR)m_sName, (LPCTSTR)m_sAddress );

	m_oServed.push_back( Fragments::Fragment( m_nOffset, m_nOffset + m_nLength ) );
	m_pBaseFile->AddFragment( m_nOffset, m_nLength );
	m_nLength = SIZE_UNKNOWN;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K ranking update

BOOL CUploadTransferED2K::CheckRanking()
{
	int nPosition = UploadQueues.GetPosition( this, TRUE );

	if ( nPosition < 0 )
	{
		// Invalid queue position, or queue deleted. Drop client and exit.
		Cleanup();
		Close( IDS_UPLOAD_DROPPED );
		return FALSE;
	}

	const DWORD tNow = GetTickCount();

	// Update 'ranking checked' timer
	m_tRankingCheck = tNow;

	// If queue ranking hasn't changed, don't bother sending an update
	// Note: if a rank was requested by the remote client, then m_nRanking will be set to -1.
	if ( m_nRanking == nPosition ) return TRUE;

	if ( nPosition == 0 )
	{
		// Ready to start uploading

		if ( m_pClient->IsOnline() )
		{
			if ( m_nState != upsUploading )
			{
				m_nState = upsRequest;
				Send( CEDPacket::New( ED2K_C2C_STARTUPLOAD ) );
			}
		}
		else
		{
			m_nState = upsConnecting;
			m_pClient->Connect();
		}

		// Update the 'request sent' timer
		m_tRequest = m_tRankingCheck;

		// Update the 'ranking sent' variables
		m_nRanking = nPosition;
		m_tRankingSent = tNow;
	}
	else if ( m_pClient->IsOnline() )
	{
		// Upload is queued

		// Check if we should send a ranking packet- If we have not sent one in a while, or one was requested
		if ( tNow > m_tRankingSent + Settings.eDonkey.QueueRankThrottle || m_nRanking == -1 )
		{
			// Send a queue rank packet
			CSingleLock pLock( &UploadQueues.m_pSection, TRUE );

			if ( UploadQueues.Check( m_pQueue ) )
				theApp.Message( MSG_INFO, IDS_UPLOAD_QUEUED, (LPCTSTR)m_sName, (LPCTSTR)m_sAddress, nPosition, m_pQueue->GetQueuedCount(), (LPCTSTR)m_pQueue->m_sName );

			pLock.Unlock();

			m_nState = upsQueued;

			// eMule queue ranking
			if ( m_pClient->m_bEmule )
			{
				CEDPacket* pPacket = CEDPacket::New( ED2K_C2C_QUEUERANKING, ED2K_PROTOCOL_EMULE );
				pPacket->WriteShortLE( WORD( nPosition ) );
				pPacket->WriteShortLE( 0 );
				pPacket->WriteLongLE( 0 );
				pPacket->WriteLongLE( 0 );
				Send( pPacket );
			}
			else	// Older eDonkey style
			{
				CEDPacket* pPacket = CEDPacket::New( ED2K_C2C_QUEUERANK );
				pPacket->WriteLongLE( nPosition );
				Send( pPacket );
			}

			// Update the 'ranking sent' variables
			m_nRanking = nPosition;
			m_tRankingSent = tNow;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K reask

BOOL CUploadTransferED2K::OnReask()
{
	if ( m_nState != upsQueued ) return FALSE;

	int nPosition = UploadQueues.GetPosition( this, TRUE );
	if ( nPosition < 0 ) return FALSE;

	CEDPacket* pPacket = CEDPacket::New( ED2K_C2C_UDP_REASKACK, ED2K_PROTOCOL_EMULE );
	pPacket->WriteShortLE( WORD( nPosition ) );
	Datagrams.Send( &m_pClient->m_pHost.sin_addr, m_pClient->m_nUDP, pPacket );

	m_tRequest = GetTickCount();

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CUploadTransferED2K 64bit Large file support

BOOL CUploadTransferED2K::OnRequestParts64(CEDPacket* pPacket)
{
	if ( pPacket->GetRemaining() < Hashes::Ed2kHash::byteCount + 4 * 3 * 2 )
	{
		theApp.Message( MSG_ERROR, IDS_ED2K_CLIENT_BAD_PACKET, (LPCTSTR)m_sAddress, pPacket->m_nType );
		Close();
		return FALSE;
	}

	Hashes::Ed2kHash oED2K;
	pPacket->Read( oED2K );

	if ( validAndUnequal( oED2K, m_oED2K ) )
	{
		if ( ! Request( oED2K ) ) return FALSE;
	}

	if ( m_nState != upsQueued && m_nState != upsRequest && m_nState != upsUploading )
	{
		theApp.Message( MSG_ERROR, IDS_ED2K_CLIENT_BAD_PACKET, (LPCTSTR)m_sAddress, pPacket->m_nType );
		Close();
		return FALSE;
	}

	QWORD nOffset[2][3];
	pPacket->Read( nOffset, sizeof(QWORD) * 2 * 3 );

	for ( int nRequest = 0 ; nRequest < 3 ; nRequest++ )
	{
		if ( nOffset[1][nRequest] <= m_nSize )
		{
			// Valid (or null) request, Add non-null ranges to the list
			if ( nOffset[0][nRequest] < nOffset[1][nRequest] )
				AddRequest( nOffset[0][nRequest], nOffset[1][nRequest] - nOffset[0][nRequest] );
		}
		else
		{
			// Invalid request- had an impossible range.
			theApp.Message( MSG_ERROR, IDS_UPLOAD_BAD_RANGE, (LPCTSTR)m_sAddress, (LPCTSTR)m_sName );
			// They probably have an incorrent hash associated with a file.
			// Calling close now will send "file not found" to stop them re-asking, then close the connection.
			Close();
			return FALSE;
		}
	}

	ServeRequests();

	return TRUE;
}
