//
// UploadTransferDC.cpp
//
// This file is part of PeerProject (peerproject.org) � 2010
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
#include "PeerProject.h"
#include "DCClient.h"
#include "DCClients.h"
#include "Library.h"
#include "SharedFolder.h"
#include "LibraryFolders.h"
#include "UploadTransfer.h"
#include "UploadTransferDC.h"
#include "Settings.h"
#include "Statistics.h"
#include "Transfers.h"
#include "Uploads.h"
#include "UploadFile.h"
#include "UploadQueue.h"
#include "UploadQueues.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

#define FILE_NOT_AVAILABLE	_T("$Error File Not Available|")
#define UPLOAD_BUSY			_T("$MaxedOut|")
#define UPLOAD_QUEUE		_T("$MaxedOut %i|")


CUploadTransferDC::CUploadTransferDC(CDCClient* pClient)
	: CUploadTransfer	( PROTOCOL_DC )
	, m_pClient			( pClient )
	, m_tRankingCheck	( 0 )
{
	ASSERT( pClient != NULL );
}

CUploadTransferDC::~CUploadTransferDC()
{
	ASSERT( m_pClient == NULL );
}

void CUploadTransferDC::Close(UINT nError)
{
	if ( m_pClient != NULL )
	{
		if ( m_nState >= upsUploading )
		{
			m_pClient->Close();
		}
		m_pClient->OnUploadClose();
		m_pClient = NULL;
	}

	Cleanup();

	CUploadTransfer::Close( nError );
}

void CUploadTransferDC::Cleanup(BOOL bDequeue)
{
	if ( bDequeue )
		UploadQueues.Dequeue( this );

	if ( m_nState == upsUploading )
	{
		if ( m_pBaseFile && m_nLength != SIZE_UNKNOWN )
			m_pBaseFile->AddFragment( m_nOffset, m_nPosition );

		CloseFile();
	}

	ClearRequest();

	m_pBaseFile = NULL;

	m_nState = upsReady;
}

DWORD CUploadTransferDC::GetMeasuredSpeed()
{
	if ( m_pClient == NULL )
		return 0;

	m_pClient->MeasureOut();

	return m_pClient->m_mOutput.nMeasure;
}

void CUploadTransferDC::OnDropped()
{
	if ( m_nState == upsQueued )
		theApp.Message( MSG_INFO, IDS_UPLOAD_QUEUE_DROP, (LPCTSTR)m_sAddress );
	else
		Close( IDS_UPLOAD_DROPPED );
}

BOOL CUploadTransferDC::OnRun()
{
	if ( ! CUploadTransfer::OnRun() )
		return FALSE;

	DWORD tNow = GetTickCount();

	switch ( m_nState )
	{
	case upsReady:
	case upsRequest:
		if ( tNow > m_tRequest + Settings.Connection.TimeoutHandshake )
		{
			Close( IDS_UPLOAD_REQUEST_TIMEOUT );
			return FALSE;
		}
		break;

	case upsQueued:
		if ( tNow > m_tRequest + Settings.DC.DequeueTime )
		{
			Close( IDS_UPLOAD_QUEUE_TIMEOUT );
			return FALSE;
		}
		else if ( tNow > m_tRankingCheck + 5 * 1000 ) // Re-check every 5 seconds
		{
			return CheckRanking();
		}
		break;

	case upsUploading:
	case upsResponse:
	case upsBrowse:
	case upsTigerTree:
		if ( tNow > m_mOutput.tLast + Settings.Connection.TimeoutTraffic )
		{
			Close( IDS_UPLOAD_TRAFFIC_TIMEOUT );
			return FALSE;
		}
		break;

	default:
		;
	}

	return TRUE;
}

BOOL CUploadTransferDC::OnWrite()
{
	ASSUME_LOCK( Transfers.m_pSection );
	ASSERT( m_pClient );

	m_mOutput.tLast = m_pClient->m_mOutput.tLast;

	if ( m_pClient->GetOutputLength() != 0 )
		// There is data in output buffer
		return TRUE;

	if ( m_nState == upsUploading )
	{
		ASSERT( m_nLength != SIZE_UNKNOWN );

		// No file data left to transfer
		if ( m_nPosition >= m_nLength )
		{
			// File completed

			Uploads.SetStable( GetAverageSpeed() );

			m_nState = upsRequest;
			m_tRequest = GetTickCount();

			m_pBaseFile->AddFragment( m_nOffset, m_nLength );

			theApp.Message( MSG_INFO, IDS_UPLOAD_FINISHED,
				(LPCTSTR)m_sName, (LPCTSTR)m_sAddress );
		}
		else
		{
			// Reading next data chunk of file
			QWORD nToRead = min( m_nLength - m_nPosition, 256 * 1024ull );
			QWORD nRead = 0;
			auto_array< BYTE > pBuffer( new BYTE[ nToRead ] );
			if ( ! ReadFile( m_nFileBase + m_nOffset + m_nPosition,
				pBuffer.get(), nToRead, &nRead ) || nToRead != nRead )
			{
				// File error
				return FALSE;
			}
			m_pClient->Write( pBuffer.get(), (DWORD)nRead );

			m_nPosition += nRead;
			m_nUploaded += nRead;

			Statistics.Current.Uploads.Volume += ( nRead / 1024 );
		}
	}
	else if ( m_nState >= upsResponse )
	{
		// Current transfer completed
		m_nState	= ( m_nState == upsPreQueue ) ? upsQueued : upsRequest;
		m_tRequest	= GetTickCount();
	}

	return TRUE;
}

BOOL CUploadTransferDC::OnUpload(const std::string& strType, const std::string& strFilename, QWORD nOffset, QWORD nLength, const std::string& strOptions)
{
	ASSERT( m_pClient );

	if ( m_nState >= upsUploading )
	{
		// Drop unsent data
		CLockedBuffer pOutput( m_pClient->GetOutput() );
		pOutput->Clear();

		m_nState = upsRequest;
	}

	ClearRequest();

	m_sUserAgent = m_pClient->m_sUserAgent;
	m_pHost = m_pClient->m_pHost;
	m_sAddress = m_pClient->m_sAddress;
	UpdateCountry();

	m_pClient->m_mInput.pLimit	= &Settings.Bandwidth.Request;
	m_pClient->m_mOutput.pLimit = &m_nBandwidth;

	m_tRequest = GetTickCount();

	BOOL bZip = ( strOptions.find("ZL1") != std::string::npos );

	if ( strType == "tthl" )
	{
		if ( strFilename.substr( 0, 4 ) != "TTH/" )
		{
			// Invalid hash prefix
			return FALSE;
		}

		Hashes::TigerHash oTiger;
		if ( ! oTiger.fromString( CA2W( strFilename.substr( 4 ).c_str() ) ) )
		{
			// Invalid TigerTree hash encoding
			return FALSE;
		}

		CSingleLock oLock( &Library.m_pSection );
		if ( oLock.Lock( 1000 ) )
		{
			if ( CLibraryFile* pFile = LibraryMaps.LookupFileByTiger( oTiger, TRUE, TRUE ) )
			{
				if ( RequestTigerTree( pFile, nOffset, nLength ) )
					return TRUE;
			}
		}
	}
	else if ( strType == "file" )
	{
		if ( strFilename == "files.xml" || strFilename == "files.xml.bz2" )
		{
			if ( RequestFileList( TRUE, bZip, strFilename, nOffset, nLength ) )
				return TRUE;
		}
		else if ( strFilename.substr( 0, 4 ) == "TTH/" )
		{
			Hashes::TigerHash oTiger;
			if ( ! oTiger.fromString( CA2W( strFilename.substr( 4 ).c_str() ) ) )
			{
				// Invalid TigerTree hash encoding
				return FALSE;
			}

			CSingleLock oLock( &Library.m_pSection );
			if ( oLock.Lock( 1000 ) )
			{
				if ( CLibraryFile* pFile = LibraryMaps.LookupFileByTiger( oTiger, TRUE, TRUE ) )
				{
					if ( RequestFile( pFile, nOffset, nLength ) )
						return TRUE;
				}
			}
		}
	}
	else if ( strType == "list" )
	{
		if ( RequestFileList( FALSE, bZip, strFilename, nOffset, nLength ) )
			return TRUE;
	}
	else
	{
		// Invalid request type
		return FALSE;
	}

	theApp.Message( MSG_ERROR, IDS_UPLOAD_FILENOTFOUND,
		(LPCTSTR)m_sAddress, (LPCTSTR)CA2CT( strFilename.c_str() ) );

	m_pClient->SendCommand( FILE_NOT_AVAILABLE );

	return TRUE;
}

BOOL CUploadTransferDC::CheckRanking()
{
	ASSERT( m_nState == upsQueued );

	m_tRankingCheck = GetTickCount();

	int nPosition = UploadQueues.GetPosition( this, TRUE );
	if ( nPosition < 0 )
	{
		// Invalid queue position, or queue deleted
		Close( IDS_UPLOAD_DROPPED );
		return FALSE;
	}
	else if ( nPosition == 0 )
	{
		// Ready to send
		m_tRankingCheck += 60*1000;

		if ( m_pClient->IsOnline() )
			return SendFile();
		else
			return m_pClient->Connect();
	}

	// Continue waiting in queue
	return TRUE;
}

BOOL CUploadTransferDC::RequestFile(CLibraryFile* pFile, QWORD nOffset, QWORD nLength)
{
	if ( ! RequestComplete( pFile ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	if ( ! UploadQueues.CanUpload( PROTOCOL_DC, pFile, FALSE ) )
	{
		theApp.Message( MSG_ERROR, IDS_UPLOAD_FILENOTFOUND,
			(LPCTSTR)m_sAddress, (LPCTSTR)m_sName );

		m_pClient->SendCommand( FILE_NOT_AVAILABLE );

		return TRUE;
	}

	m_nOffset = nOffset;
	if ( m_nOffset >= m_nSize )
		m_nLength = SIZE_UNKNOWN;
	else
		m_nLength = min( ( ( nLength == SIZE_UNKNOWN ) ? m_nSize : nLength ), m_nSize - m_nOffset );
	m_nPosition = 0;

	if ( m_nLength > m_nSize || m_nOffset + m_nLength > m_nSize )
	{
		theApp.Message( MSG_ERROR, IDS_UPLOAD_BAD_RANGE,
			(LPCTSTR)m_sAddress, (LPCTSTR)m_sName );

		m_pClient->SendCommand( FILE_NOT_AVAILABLE );

		return TRUE;
	}

	AllocateBaseFile();

	if ( m_bStopTransfer )
	{
		m_tRotateTime = 0;
		m_bStopTransfer	= FALSE;

		CUploadQueue* pQueue = m_pQueue;
		if ( pQueue )
			pQueue->Dequeue( this );
	}

	int nPosition = UploadQueues.GetPosition( this, TRUE );
	if ( nPosition < 0 && UploadQueues.Enqueue( this ) )
	{
		nPosition = UploadQueues.GetPosition( this, TRUE );
	}

	if ( nPosition == 0 )
	{
		// Ready to send
		return SendFile();
	}
	else if ( nPosition > 0 )
	{
		// Queued
		theApp.Message( MSG_INFO, IDS_UPLOAD_QUEUED, (LPCTSTR)m_sName,
			(LPCTSTR)m_sAddress, nPosition, m_pQueue->GetQueuedCount(),
			(LPCTSTR)m_pQueue->m_sName );

		CString strQueued;
		strQueued.Format( UPLOAD_QUEUE, nPosition );

		m_pClient->SendCommand( strQueued );

		StartSending( upsPreQueue );

		m_tRankingCheck = GetTickCount();

		return TRUE;
	}
	else
	{
		// Unable to queue anywhere
		UploadQueues.Dequeue( this );
		ASSERT( m_pQueue == NULL );

		theApp.Message( MSG_ERROR, IDS_UPLOAD_BUSY_QUEUE,
			(LPCTSTR)m_sName, (LPCTSTR)m_sAddress, (LPCTSTR)m_sUserAgent );

		m_pClient->SendCommand( UPLOAD_BUSY );

		return TRUE;
	}
}

BOOL CUploadTransferDC::SendFile()
{
	if ( ! IsFileOpen() && ! OpenFile() )
	{
		theApp.Message( MSG_ERROR, IDS_UPLOAD_CANTOPEN,
			(LPCTSTR)m_sName , (LPCTSTR)m_sAddress);

		m_pClient->SendCommand( FILE_NOT_AVAILABLE );

		return FALSE;
	}

	CString sAnswer;
	sAnswer.Format( _T("$ADCSND file TTH/%s %I64u %I64u|"),
		m_oTiger.toString(), m_nOffset, m_nLength );

	m_pClient->SendCommand( sAnswer );

	StartSending( upsUploading );

	if ( m_pBaseFile->m_nRequests++ == 0 )
	{
		theApp.Message( MSG_NOTICE, IDS_UPLOAD_FILE,
			(LPCTSTR)m_sName, (LPCTSTR)m_sAddress );

		ASSERT( m_pBaseFile->m_sPath.GetLength() );
		PostMainWndMessage( WM_NOWUPLOADING, 0, (LPARAM)new CString( m_pBaseFile->m_sPath ) );
	}

	theApp.Message( MSG_INFO, IDS_UPLOAD_CONTENT,
		m_nOffset, m_nOffset + m_nLength - 1, (LPCTSTR)m_sName,
		(LPCTSTR)m_sAddress, (LPCTSTR)m_sUserAgent );

	return TRUE;
}

BOOL CUploadTransferDC::RequestTigerTree(CLibraryFile* pFile, QWORD nOffset, QWORD nLength)
{
	theApp.Message( MSG_INFO, IDS_UPLOAD_TIGER_SEND,
		(LPCTSTR)pFile->m_sName, (LPCTSTR)m_sAddress );

	auto_ptr< CTigerTree > pTigerTree( pFile->GetTigerTree() );
	BYTE* pSerialTree = NULL;
	DWORD nSerialTree = 0;
	if ( ! pTigerTree.get() || ! pTigerTree->ToBytes( &pSerialTree, &nSerialTree ) )
		return FALSE;

	// ToDo: Find out and fix TigerTree hashes placement in output buffer
	nSerialTree = 24; // Root hash only

	if ( nOffset >= nSerialTree )
		nLength = SIZE_UNKNOWN;
	else
		nLength = min( ( ( nLength == SIZE_UNKNOWN ) ? nSerialTree : nLength ), nSerialTree - nOffset );

	if ( nLength > nSerialTree || nOffset + nLength > nSerialTree )
	{
		GlobalFree( pSerialTree );
		return FALSE;
	}

	CString sAnswer;
	sAnswer.Format( _T("$ADCSND tthl TTH/%s %I64u %I64u|"),
		m_oTiger.toString(), nOffset, nLength );

	m_pClient->SendCommand( sAnswer );

	m_pClient->Write( pSerialTree + nOffset, nLength );

	// Start uploading
	m_nOffset = nOffset;
	m_nLength = nLength;
	m_nPosition = 0;
	m_sName = pFile->m_sName;

	StartSending( upsTigerTree );

	GlobalFree( pSerialTree );

	return TRUE;
}

BOOL CUploadTransferDC::RequestFileList(BOOL bFile, BOOL bZip, const std::string& strFilename, QWORD nOffset, QWORD nLength)
{
	theApp.Message( MSG_NOTICE, IDS_UPLOAD_BROWSE,
		(LPCTSTR)m_sAddress, (LPCTSTR)m_sUserAgent );

	BOOL bBZip = bFile && ( strFilename == "files.xml.bz2" );
	CString strFile( bFile ? "/" : strFilename.c_str() );

	// Create library file list
	CBuffer pXML;
	LibraryToFileList( strFile, pXML );

	// TODO: Implement partial request of file list
	nOffset = 0;
	nLength = pXML.m_nLength;

	if ( bBZip )
	{
		// BZip it
		if ( ! pXML.BZip() )
			return FALSE;	// Out of memory

		bZip = FALSE;
		nLength = pXML.m_nLength;
	}

	if ( bZip )
	{
		// Zip it
		if ( ! pXML.Deflate() )
			return FALSE;	// Out of memory
	}

	CString sAnswer;
	sAnswer.Format( _T("$ADCSND %hs %hs %I64u %I64u%hs|"),
		( bFile ? "file" : "list" ), strFilename.c_str(),
		nOffset, nLength, ( bZip ? " ZL1" : "") );

	m_pClient->SendCommand( sAnswer );

	m_pClient->Write( &pXML );

	// Start upload
	m_nOffset = nOffset;
	m_nLength = pXML.m_nLength;
	m_nPosition = 0;
	m_sName = strFile;

	StartSending( upsBrowse );

	return TRUE;
}

void CUploadTransferDC::LibraryToFileList(const CString& strRoot, CBuffer& pXML)
{
	CSingleLock oLock( &Library.m_pSection, TRUE );

	CString strFileListing;
	strFileListing.Format( _T("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\r\n")
		_T("<FileListing Version=\"1\" Base=\"%s\" Generator=\"%s\">\r\n"),
		strRoot, theApp.m_sSmartAgent );
	pXML.Print( strFileListing, CP_UTF8 );

	if ( strRoot == _T("/") )
	{
		// All folders
		for ( POSITION pos = LibraryFolders.GetFolderIterator() ; pos ; )
		{
			FolderToFileList( LibraryFolders.GetNextFolder( pos ), pXML );
		}
	}
	else if ( const CLibraryFolder* pFolder = LibraryFolders.GetFolderByName( strRoot ) )
	{
		// Specified folder
		FolderToFileList( pFolder, pXML );
	}

	pXML.Add( _P("</FileListing>\r\n") );
}

void CUploadTransferDC::FolderToFileList(const CLibraryFolder* pFolder, CBuffer& pXML)
{
	if ( ! pFolder || ! pFolder->IsShared() )
		return;

	CString strFolder;
	strFolder.Format( _T("<Directory Name=\"%s\">\r\n"),
		pFolder->m_sName );
	pXML.Print( strFolder, CP_UTF8 );

	for ( POSITION pos = pFolder->GetFolderIterator() ; pos ; )
	{
		FolderToFileList( pFolder->GetNextFolder( pos ), pXML );
	}

	for ( POSITION pos = pFolder->GetFileIterator() ; pos ; )
	{
		FileToFileList( pFolder->GetNextFile( pos ), pXML );
	}

	pXML.Add( _P("</Directory>\r\n") );
}

void CUploadTransferDC::FileToFileList(const CLibraryFile* pFile, CBuffer& pXML)
{
	if ( ! pFile || ! pFile->IsShared() )
		return;

	CString strFile;
	strFile.Format( _T("<File Name=\"%s\" Size=\"%I64u\" TTH=\"%s\"/>\r\n"),
		pFile->m_sName, pFile->m_nSize, pFile->m_oTiger.toString() );
	pXML.Print( strFile, CP_UTF8 );
}
