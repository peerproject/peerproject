//
// DownloadTask.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#include "StdAfx.h"
#include "PeerProject.h"
#include "Settings.h"
#include "Download.h"
#include "Downloads.h"
#include "DownloadTask.h"
#include "DownloadGroups.h"
#include "Transfers.h"
#include "Uploads.h"
#include "Library.h"
#include "LibraryFolders.h"
#include "LibraryMaps.h"
#include "SharedFile.h"
#include "HttpRequest.h"
#include "FragmentedFile.h"
#include "BTInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDownloadTask, CAppThread)

BEGIN_MESSAGE_MAP(CDownloadTask, CAppThread)
END_MESSAGE_MAP()

const DWORD BUFFER_SIZE = 2 * 1024 * 1024u;


/////////////////////////////////////////////////////////////////////////////
// CDownloadTask construction

CDownloadTask::CDownloadTask(CDownload* pDownload, dtask nTask)
	: m_nTask			( nTask )
	, m_pRequest		( NULL )
	, m_bSuccess		( false )
	, m_sFilename		( pDownload->m_sPath )
	, m_sDestination	( DownloadGroups.GetCompletedPath( pDownload ).TrimRight( _T("\\") ) )
	, m_nFileError		( NO_ERROR )
	, m_pDownload		( pDownload )
	, m_nSize			( pDownload->m_nSize )
	, m_oMergeGaps		( pDownload->m_nSize )
	, m_bMergeValidation( FALSE )
	, m_posTorrentFile	( NULL )
	, m_pEvent			( NULL )
{
	ASSERT( ! pDownload->IsTasking() );
	pDownload->SetTask( this );

	if ( pDownload->IsTorrent() )
		m_posTorrentFile = pDownload->m_pTorrent.m_pFiles.GetHeadPosition();

	m_bAutoDelete = TRUE;
}

CDownloadTask::~CDownloadTask()
{
	BOOL bCOM = SUCCEEDED( OleInitialize( NULL ) );

	Transfers.m_pSection.Lock();

	if ( Downloads.Check( m_pDownload ) )
		m_pDownload->OnTaskComplete( this );

	CEvent* pEvent = m_pEvent;
	Transfers.m_pSection.Unlock();
	if ( pEvent )
		pEvent->SetEvent();

	delete m_pRequest;

	if ( bCOM )
		OleUninitialize();
}

bool CDownloadTask::HasSucceeded() const
{
	return m_bSuccess;
}

DWORD CDownloadTask::GetFileError() const
{
	return m_nFileError;
}

dtask CDownloadTask::GetTaskType() const
{
	return m_nTask;
}

const CHttpRequest* CDownloadTask::GetRequest() const
{
	return m_pRequest;
}


/////////////////////////////////////////////////////////////////////////////
// CDownloadTask operations construction

void CDownloadTask::Copy(CDownload* pDownload)
{
	CDownloadTask* pTask = new CDownloadTask( pDownload, dtaskCopy );
	if ( ! pTask ) return;		// Out of memory

	pTask->CreateThread( "Download Task : Copy" );		// RunCopy()
}

void CDownloadTask::PreviewRequest(CDownload* pDownload, LPCTSTR szURL)
{
	CDownloadTask* pTask = new CDownloadTask( pDownload, dtaskPreviewRequest );
	if ( ! pTask ) return;		// Out of memory

	pTask->m_pRequest = new CHttpRequest();
	pTask->m_pRequest->SetURL( szURL );
	pTask->m_pRequest->AddHeader( _T("Accept"), _T("image/jpeg") );
	pTask->m_pRequest->LimitContentLength( Settings.Search.MaxPreviewLength );

	pTask->CreateThread( "Download Task : Preview" );	// RunPreviewRequest()
}

void CDownloadTask::MergeFile(CDownload* pDownload, LPCTSTR szPath, BOOL bValidation, const Fragments::List* pGaps)
{
	CDownloadTask* pTask = new CDownloadTask( pDownload, dtaskMergeFile );
	if ( ! pTask ) return;		// Out of memory

	pTask->m_sMergeFilename = szPath;
	if ( pGaps )
		pTask->m_oMergeGaps = *pGaps;
	pTask->m_bMergeValidation = bValidation;

	pTask->CreateThread( "Download Task : Merge" );		// RunMerge()
}


/////////////////////////////////////////////////////////////////////////////
// CDownloadTask aborting

void CDownloadTask::Abort()
{
	CEvent* pEvent = m_pEvent = new CEvent();
	Transfers.m_pSection.Unlock();
	WaitForSingleObject( *pEvent, INFINITE );
	Transfers.m_pSection.Lock();
	delete pEvent;
}

bool CDownloadTask::WasAborted() const
{
	return m_pEvent != NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask run

int CDownloadTask::Run()
{
	BOOL bCOM = SUCCEEDED( OleInitialize( NULL ) );

	switch ( m_nTask )
	{
	case dtaskCopy:
		RunCopy();
		break;

	case dtaskPreviewRequest:
		RunPreviewRequest();
		break;

	case dtaskMergeFile:
		RunMerge();
		break;

	default:
		;
	}

	if ( bCOM )
		OleUninitialize();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask allocate

//void CDownloadTask::RunAllocate()
//{
//	HANDLE hFile = CreateFile( m_sFilename, GENERIC_WRITE,
//		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
//		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL );
//	VERIFY_FILE_ACCESS( hFile, m_sFilename )
//	if ( hFile == INVALID_HANDLE_VALUE ) return;
//
//	if ( GetFileSize( hFile, NULL ) != 0 )
//	{
//		CloseHandle( hFile );
//		return;
//	}
//	if ( Settings.Downloads.SparseThreshold > 0 &&
//		 m_nSize != SIZE_UNKNOWN &&
//		 m_nSize >= Settings.Downloads.SparseThreshold * 1024 )
//	{
//		DWORD dwOut = 0;
//		if ( ! DeviceIoControl( hFile, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dwOut, NULL ) )
//		{
//			DWORD nError = GetLastError();
//			theApp.Message( MSG_ERROR, _T("Unable to set sparse file: \"%s\", Win32 error %x."), m_sFilename, nError );
//		}
//	}
//	if ( m_nSize > 0 && m_nSize != SIZE_UNKNOWN )
//	{
//		DWORD nOffsetLow	= (DWORD)( ( m_nSize - 1 ) & 0x00000000FFFFFFFF );
//		DWORD nOffsetHigh	= (DWORD)( ( ( m_nSize - 1 ) & 0xFFFFFFFF00000000 ) >> 32 );
//		SetFilePointer( hFile, nOffsetLow, (PLONG)&nOffsetHigh, FILE_BEGIN );
//
//		BYTE bZero = 0;
//		DWORD nSize = 0;
//		WriteFile( hFile, &bZero, 1, &nSize, NULL );
//	}
//	CloseHandle( hFile );
//	m_bSuccess = TRUE;
//}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask simple copy

void CDownloadTask::RunCopy()
{
	m_nFileError = m_pDownload->MoveFile( m_sDestination, CopyProgressRoutine, this );
	m_bSuccess = ( m_nFileError == ERROR_SUCCESS );
}

DWORD CALLBACK CDownloadTask::CopyProgressRoutine(LARGE_INTEGER /*TotalFileSize*/,
	LARGE_INTEGER /*TotalBytesTransferred*/, LARGE_INTEGER /*StreamSize*/,
	LARGE_INTEGER /*StreamBytesTransferred*/, DWORD /*dwStreamNumber*/,
	DWORD /*dwCallbackReason*/, HANDLE /*hSourceFile*/, HANDLE /*hDestinationFile*/,
	LPVOID lpData)
{
	CDownloadTask* pThis = (CDownloadTask*)lpData;

	// ToDo: Implement notification dialog (?)

	return ( pThis->m_pEvent == NULL ) ? PROGRESS_CONTINUE : PROGRESS_CANCEL;
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask copy file

BOOL CDownloadTask::CopyFile(HANDLE hSource, LPCTSTR pszTarget, QWORD nLength)
{
	HANDLE hTarget = CreateFile( pszTarget, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	m_nFileError = GetLastError();
	VERIFY_FILE_ACCESS( hTarget, pszTarget )
	if ( hTarget == INVALID_HANDLE_VALUE ) return FALSE;

	BYTE* pBuffer = new BYTE[ BUFFER_SIZE ];

	while ( nLength )
	{
		DWORD nBuffer	= (DWORD)min( nLength, BUFFER_SIZE );
		DWORD nSuccess	= 0;
		DWORD tStart	= GetTickCount();

		if ( !ReadFile( hSource, pBuffer, nBuffer, &nBuffer, NULL )
			|| !nBuffer
			|| !WriteFile( hTarget, pBuffer, nBuffer, &nSuccess, NULL )
			|| nSuccess != nBuffer )
		{
			m_nFileError = GetLastError();
			break;
		}

		nLength -= nBuffer;

		if ( m_pEvent != NULL ) break;
		tStart = ( GetTickCount() - tStart ) / 2;
		Sleep( min( tStart, 50ul ) );
		if ( m_pEvent != NULL ) break;
	}

	delete [] pBuffer;

	CloseHandle( hTarget );
	if ( nLength > 0 )
		DeleteFileEx( pszTarget, TRUE, FALSE, TRUE );

	return ( nLength == 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask merge local file

void CDownloadTask::RunMerge()
{
	HANDLE hSource = CreateFile( m_sMergeFilename, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	VERIFY_FILE_ACCESS( hSource, m_sMergeFilename )
	if ( hSource == INVALID_HANDLE_VALUE )
	{
		// Source file open error
		theApp.Message( MSG_ERROR, IDS_DOWNLOAD_FILE_OPEN_ERROR, m_sMergeFilename );
		m_bSuccess = true;
		return;
	}

	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	if ( ! Downloads.Check( m_pDownload ) ||
		  m_pDownload->IsCompleted() ||
		  m_pDownload->IsMoving() )
	{
		// Download almost completed
		CloseHandle( hSource );
		m_bSuccess = true;
		return;
	}

	if ( m_bMergeValidation && ! m_pDownload->IsTorrent() &&
		 m_pDownload->NeedTigerTree() && m_pDownload->NeedHashset() )
	{
		// No hashsets
		pLock.Unlock();
		CString strMessage;
		LoadString( strMessage, IDS_DOWNLOAD_EDIT_COMPLETE_NOHASH );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		CloseHandle( hSource );
		m_bSuccess = true;
		return;
	}

	if ( ! m_pDownload->PrepareFile() )
	{
		// Destination file open error
		CloseHandle( hSource );
		m_bSuccess = true;
		return;
	}

	Fragments::List oList( m_pDownload->GetEmptyFragmentList() );
	if ( ! m_oMergeGaps.empty() )
	{
		for ( Fragments::List::const_iterator i = m_oMergeGaps.begin();
			i != m_oMergeGaps.end(); ++i )
			oList.erase( *i );
	}
	if ( ! oList.size() )
	{
		// No available fragments
		CloseHandle( hSource );
		m_bSuccess = true;
		return;
	}

	QWORD qwSourceOffset = 0;
	DWORD dwSourceSizeHigh = 0;
	DWORD dwSourceSizeLow = GetFileSize( hSource, &dwSourceSizeHigh );
	QWORD qwSourceLength = (QWORD)dwSourceSizeLow + ( (QWORD)dwSourceSizeHigh << 32 );

	// Multifile Torrent Selection
	if ( m_pDownload->IsTorrent() && ! m_pDownload->IsSingleFileTorrent() )
	{
		QWORD qwOffset = 0;
		CString strFilename = PathFindFileName( m_sMergeFilename );
		if ( strFilename.GetLength() > 3 )
		{
			CString strLocalname;
			for ( POSITION pos = m_pDownload->m_pTorrent.m_pFiles.GetHeadPosition() ; pos ; )
			{
				CBTInfo::CBTFile* pFile = m_pDownload->m_pTorrent.m_pFiles.GetNext( pos );

				strLocalname = pFile->m_sPath;
				int nSlash = strLocalname.Find( _T('\\') ) + 1;
				if ( nSlash > 0 )
				{
					strLocalname = strLocalname.Mid( nSlash );
					// Check second level folder
					nSlash = strLocalname.Find( _T('\\') ) + 1;
					if ( nSlash > 0 )
						strLocalname = strLocalname.Mid( nSlash );
				}

				if ( strFilename.CompareNoCase( strLocalname ) == 0 )
				{
					//AfxMessageBox( _T("Found Filename") );
					qwSourceOffset = qwOffset;
					qwOffset = 1;	// Avoid fallback check below
					break;
				}

				qwOffset += pFile->m_nSize;
			}
		}

		if ( qwOffset != 1 )	// No filename match, try exact size
		{
			qwOffset = 0;
			QWORD nMergeSize = GetFileSize( m_sMergeFilename );
			if ( nMergeSize > 4 )
			{
				for ( POSITION pos = m_pDownload->m_pTorrent.m_pFiles.GetHeadPosition() ; pos ; )
				{
					CBTInfo::CBTFile* pFile = m_pDownload->m_pTorrent.m_pFiles.GetNext( pos );

					if ( pFile->m_nSize == nMergeSize )
					{
						//AfxMessageBox( _T("Found Size") );
						qwSourceOffset = qwOffset;
						break;
					}

					qwOffset += pFile->m_nSize;
				}
			}
		}
	}

	pLock.Unlock();

	const int nBufferLength = 65536;

	// Read missing file fragments from selected file
	auto_array< BYTE > Buf( new BYTE [nBufferLength] );
	for ( Fragments::List::const_iterator pFragment = oList.begin();
		pFragment != oList.end() && m_pEvent == NULL; ++pFragment )
	{
		QWORD qwLength = pFragment->end() - pFragment->begin();
		QWORD qwOffset = pFragment->begin();

		if ( qwOffset + qwLength <= qwSourceOffset ||
			 qwSourceOffset + qwSourceLength <= qwOffset )
			 continue;	 // No overlapped fragments

		// Calculate overlapped range end offset
		QWORD qwEnd = min( qwOffset + qwLength, qwSourceOffset + qwSourceLength );
		// Calculate overlapped range start offset
		qwOffset = max( qwOffset, qwSourceOffset );
		// Calculate overlapped range length
		qwLength = qwEnd - qwOffset;
		// Calculate file offset if any
		QWORD qwFileOffset = ( qwOffset > qwSourceOffset ) ? qwOffset - qwSourceOffset : 0;

		LONG nFileOffsetHigh = (LONG)( qwFileOffset >> 32 );
		LONG nFileOffsetLow = (LONG)( qwFileOffset & 0xFFFFFFFF );
		SetFilePointer( hSource, nFileOffsetLow, &nFileOffsetHigh, FILE_BEGIN );
		if ( GetLastError() != NO_ERROR )
			continue;

		DWORD dwToRead;
		while ( ( dwToRead = (DWORD)min( qwLength, (QWORD)nBufferLength ) ) != 0 &&
			m_pEvent == NULL )
		{
			DWORD dwReaded = 0;
			if ( ReadFile( hSource, Buf.get(), dwToRead, &dwReaded, NULL ) &&
				dwReaded != 0 )
			{
				pLock.Lock();
				m_pDownload->SubmitData( qwOffset, Buf.get(), (QWORD) dwReaded );
				pLock.Unlock();
				qwOffset += (QWORD) dwReaded;
				qwLength -= (QWORD) dwReaded;
			}
			else
			{
				// File error or end of file. Non-Fatal
				break;
			}
		}

		if ( m_bMergeValidation )
			m_pDownload->RunValidation();

		m_pDownload->SetModified();
	}

	CloseHandle( hSource );
	m_bSuccess = true;
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask preview request

void CDownloadTask::RunPreviewRequest()
{
	m_pRequest->Execute( FALSE );	// Without threading
}


/////////////////////////////////////////////////////////////////////////////
// CDownloadTask filename processor (Obsolete. For reference or delete)

//CString CDownloadTask::SafeFilename(LPCTSTR pszName)
//{
//	static LPCTSTR pszValid = _T(" `~!@#$%^&()-_=+[]{}';.,");
//	CString strName = pszName;
//
//	for ( int nChar = 0 ; nChar < strName.GetLength() ; nChar++ )
//	{
//		TCHAR cChar = strName.GetAt( nChar );
//
//		if ( (DWORD)cChar > 128 )
//			continue;
//		else if ( IsCharacter( cChar ) )
//			continue;
//		else if ( _tcschr( pszValid, cChar ) != NULL )
//			continue;
//
//		strName.SetAt( nChar, '_' );
//	}
//
//	LPCTSTR pszExt = _tcsrchr( strName, '.' );
//	if ( pszExt )
//	{
//		if ( _tcsicmp( pszExt, _T(".pd") ) == 0 )
//			strName += _T("x");
//		else if ( _tcsicmp( pszExt, _T(".sd") ) == 0 )
//			strName += _T("x");
//	}
//
//	// Maximum filepath length is:
//	// <Windows limit = 256 - 1> - <length of path to download directory> - <length of hash = 39(tiger)> - <space = 1> - <length of ".pd.sav" = 7>
//	int nMaxFilenameLength = 208 - Settings.Downloads.IncompletePath.GetLength();
//	if ( strName.GetLength() > nMaxFilenameLength )
//	{
//		int nExtLen = pszExt ? static_cast< int >( _tcslen( pszExt ) ) : 0;
//		strName = strName.Left( nMaxFilenameLength - nExtLen ) + strName.Right( nExtLen );
//	}
//
//	return strName;
//}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask path creator

void CDownloadTask::CreatePathForFile(const CString& strBase, const CString& strPath)
{
	CString strFolder = strBase + _T('\\') + strPath;
	CreateDirectory( strFolder.Left( strFolder.ReverseFind( _T('\\') ) ) );
}

CBuffer* CDownloadTask::IsPreviewAnswerValid() const
{
	if ( m_nTask != dtaskPreviewRequest || !m_pRequest->IsFinished() )
		return NULL;

	m_pRequest->GetStatusCode();

	if ( m_pRequest->GetStatusSuccess() == FALSE )
	{
		theApp.Message( MSG_DEBUG, L"Preview failed: HTTP status code %i",
			m_pRequest->GetStatusCode() );
		return NULL;
	}

	CString strURN = m_pRequest->GetHeader( L"X-Previewed-URN" );

	if ( strURN.GetLength() )
	{
		Hashes::Sha1Hash oSHA1;
		bool bValid = true;

		if ( m_pDownload )
		{
			if ( oSHA1.fromUrn( strURN ) && validAndUnequal( oSHA1, m_pDownload->m_oSHA1 ) )
				bValid = false;
		}
		else
		{
			CSingleLock oLock( &Library.m_pSection, TRUE );
			CLibraryFile* pFile = LibraryMaps.LookupFileBySHA1( oSHA1 );
			if ( pFile == NULL )
				bValid = false;
			oLock.Unlock();
		}
		if ( !bValid )
		{
			theApp.Message( MSG_DEBUG, L"Preview failed: wrong URN." );
			return NULL;
		}
	}

	CString strMIME = m_pRequest->GetHeader( L"Content-Type" );

	if ( strMIME.CompareNoCase( L"image/jpeg" ) != 0 )
	{
		theApp.Message( MSG_DEBUG, L"Preview failed: unacceptable content type." );
		return NULL;
	}

	CBuffer* pBuffer = m_pRequest->GetResponseBuffer();
	return pBuffer;
}
