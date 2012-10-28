//
// DownloadTask.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "DownloadTask.h"
#include "DownloadGroups.h"
#include "Download.h"
#include "Downloads.h"
#include "Transfers.h"
#include "Uploads.h"
#include "Library.h"
#include "LibraryFolders.h"
#include "LibraryMaps.h"
#include "SharedFile.h"
#include "FragmentedFile.h"
#include "HttpRequest.h"
#include "BTInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

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
	, m_fProgress		( 0 )
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

CString CDownloadTask::GetRequest() const
{
	return m_sURL;
}

float CDownloadTask::GetProgress() const
{
	return m_fProgress;
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

	pTask->m_sURL = szURL;
	pTask->m_pRequest = new CHttpRequest();
	pTask->m_pRequest->SetURL( pTask->m_sURL );
	pTask->m_pRequest->AddHeader( _T("Accept"), _T("image/jpeg") );
	pTask->m_pRequest->LimitContentLength( Settings.Search.MaxPreviewLength );

	pTask->CreateThread( "Download Task : Preview" );	// RunPreviewRequest()
}

void CDownloadTask::MergeFile(CDownload* pDownload, CList< CString >* pFiles, BOOL bValidation, const Fragments::List* pGaps)
{
	CDownloadTask* pTask = new CDownloadTask( pDownload, dtaskMergeFile );
	if ( ! pTask ) return;		// Out of memory

	pTask->m_oMergeFiles.AddTail( pFiles );

	if ( pGaps )
		pTask->m_oMergeGaps = *pGaps;
	pTask->m_bMergeValidation = bValidation;

	pTask->CreateThread( "Download Task : Merge" );		// RunMerge()
}

void CDownloadTask::MergeFile(CDownload* pDownload, LPCTSTR szPath, BOOL bValidation, const Fragments::List* pGaps)
{
	CDownloadTask* pTask = new CDownloadTask( pDownload, dtaskMergeFile );
	if ( ! pTask ) return;		// Out of memory

	pTask->m_oMergeFiles.AddTail( (CString)szPath );

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
//	if ( theApp.m_bIsVistaOrNewer )
//		::SetThreadPriority( GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN );	// Too aggressive?

	BOOL bCOM = SUCCEEDED( OleInitialize( NULL ) );

	switch ( m_nTask )
	{
	case dtaskCopy:
		RunCopy();
		break;
	case dtaskMergeFile:
		RunMerge();
		break;
	case dtaskPreviewRequest:
		RunPreviewRequest();
	//	break;
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
	m_fProgress  = 0;
	m_nFileError = m_pDownload->MoveFile( m_sDestination, CopyProgressRoutine, this );
	m_fProgress  = 100.0f;

	m_bSuccess = ( m_nFileError == ERROR_SUCCESS );
}

DWORD CALLBACK CDownloadTask::CopyProgressRoutine(LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER /*StreamSize*/,
	LARGE_INTEGER /*StreamBytesTransferred*/, DWORD /*dwStreamNumber*/,
	DWORD /*dwCallbackReason*/, HANDLE /*hSourceFile*/, HANDLE /*hDestinationFile*/,
	LPVOID lpData)
{
	CDownloadTask* pThis = (CDownloadTask*)lpData;

	if ( TotalFileSize.QuadPart )
		pThis->m_fProgress = (float)( TotalBytesTransferred.QuadPart * 100 ) / (float)TotalFileSize.QuadPart;
	//else
	//	pThis->m_fProgress = 100.0f;

	return ( pThis->m_pEvent == NULL ) ? PROGRESS_CONTINUE : PROGRESS_CANCEL;
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask copy file (Obsolete)

//BOOL CDownloadTask::CopyFile(HANDLE hSource, LPCTSTR pszTarget, QWORD nLength)
//{
//	HANDLE hTarget = CreateFile( SafePath( pszTarget ), GENERIC_WRITE,
//		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL );
//	m_nFileError = GetLastError();
//	VERIFY_FILE_ACCESS( hTarget, pszTarget )
//	if ( hTarget == INVALID_HANDLE_VALUE ) return FALSE;
//
//	BYTE* pBuffer = new BYTE[ BUFFER_SIZE ];
//
//	while ( nLength )
//	{
//		DWORD nBuffer	= (DWORD)min( nLength, BUFFER_SIZE );
//		DWORD nSuccess	= 0;
//		DWORD tStart	= GetTickCount();
//
//		if ( ! ReadFile( hSource, pBuffer, nBuffer, &nBuffer, NULL ) || ! nBuffer ||
//			 ! WriteFile( hTarget, pBuffer, nBuffer, &nSuccess, NULL ) || nSuccess != nBuffer )
//		{
//			m_nFileError = GetLastError();
//			break;
//		}
//
//		nLength -= nBuffer;
//
//		if ( m_pEvent != NULL ) break;
//		tStart = ( GetTickCount() - tStart ) / 2;
//		Sleep( min( tStart, 50ul ) );
//		if ( m_pEvent != NULL ) break;
//	}
//
//	delete [] pBuffer;
//
//	CloseHandle( hTarget );
//	if ( nLength > 0 )
//		DeleteFileEx( pszTarget, TRUE, FALSE, TRUE );
//
//	return ( nLength == 0 );
//}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask merge local file

void CDownloadTask::RunMerge()
{
	m_fProgress = 0;
	const float fIncrement = 100.0f / m_oMergeFiles.GetCount();

	for ( POSITION pos = m_oMergeFiles.GetHeadPosition() ; pos ; )
	{
	//	if ( m_pDownload->IsCompleted() || m_pDownload->IsMoving() ) break;

		RunMergeFile( m_pDownload, m_oMergeFiles.GetNext( pos ), m_bMergeValidation, m_oMergeGaps, fIncrement );
	}

	m_fProgress = 100.0f;
	m_bSuccess = true;
}

void CDownloadTask::RunMergeFile(CDownload* pDownload, LPCTSTR szFilename, BOOL bMergeValidation, const Fragments::List& oMissedGaps, float fProgress)
{
//	HANDLE hSource = CreateFile( szFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );

	QWORD nSourceSize = 0;		// qwSourceLength
	QWORD nSourceOffset = 0;	// qwSourceOffset

	CAtlFile oSource;
	oSource.Create( szFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, OPEN_EXISTING );
	VERIFY_FILE_ACCESS( oSource, szFilename )
	if ( ! oSource )
	{
		// Source file open error
		theApp.Message( MSG_ERROR, IDS_DOWNLOAD_FILE_OPEN_ERROR, szFilename );
		return;
	}

	oSource.GetSize( nSourceSize );
	if ( ! nSourceSize )
		return;		// Empty source file

	CSingleLock pLock( &Transfers.m_pSection, TRUE );

	if ( ! Downloads.Check( pDownload ) ||
		  pDownload->IsCompleted() ||
		  pDownload->IsMoving() )
		return;		// Moot download almost completed

	if ( m_bMergeValidation &&
		! pDownload->IsTorrent() &&
		 pDownload->NeedTigerTree() &&
		 pDownload->NeedHashset() )
	{
	//	pLock.Unlock();
	//	MsgBox( IDS_DOWNLOAD_EDIT_COMPLETE_NOHASH, MB_ICONEXCLAMATION );
		DEBUG_ONLY( theApp.Message( MSG_DEBUG, IDS_DOWNLOAD_EDIT_COMPLETE_NOHASH ) );
		return;		// No hashsets
	}

	if ( ! pDownload->PrepareFile() )
		return;		// Destination file open error

	Fragments::List oList( pDownload->GetEmptyFragmentList() );
	if ( ! oMissedGaps.empty() )
	{
		Fragments::List::const_iterator pItr = oMissedGaps.begin();
		const Fragments::List::const_iterator pEnd = oMissedGaps.end();
		for ( ; pItr != pEnd ; ++pItr )
			oList.erase( *pItr );
	}

	if ( ! oList.size() )
		return;		// No available fragments

	// Determine offset if needed
	if ( pDownload->IsMultiFileTorrent() )
	{
		QWORD nOffset = 0;		// qwOffset
		BOOL bFound = FALSE;
		CBTInfo::CBTFile* pFile;
		CString strTargetName;
		const CString strSourceName = PathFindFileName( szFilename );

		if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )	// Forced selection dialog
		{
			int nIndex = pDownload->SelectFile( &pLock );
			if ( nIndex == 0 )
			{
				bFound = TRUE;
			}
			else if ( nIndex > 0 )
			{
				CBTInfo::CBTFile* pSelectFile = pDownload->m_pTorrent.m_pFiles.GetAt( pDownload->m_pTorrent.m_pFiles.FindIndex( nIndex ) );
				for ( POSITION pos = pDownload->m_pTorrent.m_pFiles.GetHeadPosition() ; pos ; )
				{
					pFile = pDownload->m_pTorrent.m_pFiles.GetNext( pos );
					if ( pFile->m_sPath == pSelectFile->m_sPath )
					{
						DEBUG_ONLY( theApp.Message( MSG_DEBUG, _T("Merge Selected File: ") + pFile->m_sPath ) );
						nSourceOffset = nOffset;
						bFound = TRUE;	// Avoid checks below
						break;
					}

					nOffset += pFile->m_nSize;
				}
			}
		}

		if ( ! bFound )		// No forced match, try filename
		{
			for ( POSITION pos = pDownload->m_pTorrent.m_pFiles.GetHeadPosition() ; pos ; )
			{
				pFile = pDownload->m_pTorrent.m_pFiles.GetNext( pos );
				strTargetName = PathFindFileName( pFile->m_sPath );

				if ( strTargetName.CompareNoCase( strSourceName ) == 0 )
				{
					DEBUG_ONLY( theApp.Message( MSG_DEBUG, _T("Merge Filename: ") + pFile->m_sPath ) );
					nSourceOffset = nOffset;
					bFound = TRUE;	// Avoid fallback check below
					break;
				}

				nOffset += pFile->m_nSize;
			}
		}

		if ( ! bFound )		// No filename match, try exact size
		{
			nOffset = 0;
		//	const CString strExt = PathFindExtension( strSourceName );
			for ( POSITION pos = pDownload->m_pTorrent.m_pFiles.GetHeadPosition() ; pos ; )
			{
				pFile = pDownload->m_pTorrent.m_pFiles.GetNext( pos );

				if ( pFile->m_nSize == nSourceSize )	// && strExt == PathFindExtension( pFile->m_sPath )
				{
					DEBUG_ONLY( theApp.Message( MSG_DEBUG, _T("Merge Filesize Fallback") ) );
					nSourceOffset = nOffset;
				//	bFound = TRUE;
					break;
				}

				nOffset += pFile->m_nSize;
			}
		}
	}

	pLock.Unlock();

	const float fIncrement = fProgress / oList.size();

	const DWORD nBufferLength = 256 * 1024;		// Was 65536?

	// Read missing file fragments from selected file
	auto_array< BYTE > Buf( new BYTE [nBufferLength] );
	Fragments::List::const_iterator pItr = oList.begin();
	const Fragments::List::const_iterator pEnd = oList.end();
	for ( ; ! m_pEvent && pItr != pEnd ; ++pItr )
	{
		m_fProgress += fIncrement;		// Update tooltip

		QWORD qwLength = pItr->end() - pItr->begin();
		QWORD qwOffset = pItr->begin();

		// Check for overlapped fragments
		if ( qwOffset + qwLength <= nSourceOffset ||
			 nSourceOffset + nSourceSize <= qwOffset )
			 continue;	 // No overlaps

		// Calculate overlapped range end offset
		QWORD qwEnd = min( qwOffset + qwLength, nSourceOffset + nSourceSize );
		// Calculate overlapped range start offset
		qwOffset = max( qwOffset, nSourceOffset );
		// Calculate overlapped range length
		qwLength = qwEnd - qwOffset;
		// Calculate file offset if any
		QWORD qwFileOffset = ( qwOffset > nSourceOffset ) ? qwOffset - nSourceOffset : 0;
		if ( FAILED( oSource.Seek( qwFileOffset, FILE_BEGIN ) ) )
			continue;

		DWORD dwToRead;
		while ( ( dwToRead = (DWORD)min( qwLength, (QWORD)nBufferLength ) ) != 0 && ! m_pEvent )
		{
			DWORD dwReaded = 0;
			if ( SUCCEEDED( oSource.Read( Buf.get(), dwToRead, dwReaded ) ) && dwReaded )
			{
				pLock.Lock();

				if ( ! Downloads.Check( pDownload ) || pDownload->IsCompleted() || pDownload->IsMoving() )
					return;

				pDownload->SubmitData( qwOffset, Buf.get(), (QWORD)dwReaded );
				pLock.Unlock();
				qwOffset += (QWORD)dwReaded;
				qwLength -= (QWORD)dwReaded;
			}
			else
			{
				// File error or end of file. Non-Fatal
				break;
			}
		}

		pLock.Lock();

		if ( ! Downloads.Check( pDownload ) || pDownload->IsCompleted() || pDownload->IsMoving() )
			return;

		if ( bMergeValidation )
			pDownload->RunValidation();

		pDownload->SetModified();

		pLock.Unlock();
	}

//	m_bSuccess = true;
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
//		if ( (DWORD)cChar > 128 || IsCharacter( cChar ) || _tcschr( pszValid, cChar ) != NULL )
//			continue;
//
//		strName.SetAt( nChar, '_' );
//	}
//
//	LPCTSTR pszExt = _tcsrchr( strName, '.' );
//	if ( pszExt )
//	{
//		if ( _tcsicmp( pszExt, _T(".pd") ) == 0 || _tcsicmp( pszExt, _T(".sd") ) == 0 )
//			strName += _T("x");
//	}
//
//	// Maximum filepath length is:
//	// <Windows limit = 256 - 1> - <length of path to download directory> - <length of hash = 39(tiger)> - <space = 1> - <length of ".pd.sav" = 7>
//	const int nMaxFilenameLength = 208 - Settings.Downloads.IncompletePath.GetLength();
//	if ( strName.GetLength() > nMaxFilenameLength )
//	{
//		int nExtLen = pszExt ? static_cast< int >( _tcslen( pszExt ) ) : 0;
//		strName = strName.Left( nMaxFilenameLength - nExtLen ) + strName.Right( nExtLen );
//	}
//
//	return strName;
//}

/////////////////////////////////////////////////////////////////////////////
// CDownloadTask path creator (Obsolete)

//void CDownloadTask::CreatePathForFile(const CString& strBase, const CString& strPath)
//{
//	CString strFolder = strBase + _T('\\') + strPath;
//	CreateDirectory( strFolder.Left( strFolder.ReverseFind( _T('\\') ) ) );
//}

CBuffer* CDownloadTask::IsPreviewAnswerValid() const
{
	if ( m_nTask != dtaskPreviewRequest || ! m_pRequest->IsFinished() )
		return NULL;

	m_pRequest->GetStatusCode();

	if ( m_pRequest->GetStatusSuccess() == FALSE )
	{
		theApp.Message( MSG_DEBUG, L"Preview failed: HTTP status code %i", m_pRequest->GetStatusCode() );
		return NULL;
	}

	CString strURN = m_pRequest->GetHeader( L"X-Previewed-URN" );

	if ( ! strURN.IsEmpty() )
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
		if ( ! bValid )
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

	return m_pRequest->GetResponseBuffer();
}
