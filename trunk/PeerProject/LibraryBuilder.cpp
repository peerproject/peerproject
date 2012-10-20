//
// LibraryBuilder.cpp
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
#include "LibraryBuilder.h"
#include "LibraryHistory.h"
#include "Library.h"
#include "SharedFile.h"
#include "HashDatabase.h"
#include "ThumbCache.h"
#include "Downloads.h"
#include "Transfers.h" // Locks
#include "Security.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "XML.h"
#include "ID3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

CLibraryBuilder LibraryBuilder;

CFileHash::CFileHash(QWORD nFileSize)
{
	m_pTiger.BeginFile( Settings.Library.TigerHeight, nFileSize );
	m_pED2K.BeginFile( nFileSize );
}

void CFileHash::Add(const void* pBuffer, DWORD nBlock)
{
	m_pSHA1.Add( pBuffer, nBlock );
	m_pMD5.Add( pBuffer, nBlock );
	m_pTiger.AddToFile( pBuffer, nBlock );
	m_pED2K.AddToFile( pBuffer, nBlock );
}

void CFileHash::Finish()
{
	m_pSHA1.Finish();
	m_pMD5.Finish();
	m_pTiger.FinishFile();
	m_pED2K.FinishFile();
}

void CFileHash::CopyTo(CLibraryFile* pFile) const
{
	m_pSHA1.GetHash( &pFile->m_oSHA1[ 0 ] );
	pFile->m_oSHA1.validate();
	m_pMD5.GetHash( &pFile->m_oMD5[ 0 ] );
	pFile->m_oMD5.validate();
	m_pTiger.GetRoot( &pFile->m_oTiger[ 0 ] );
	pFile->m_oTiger.validate();
	m_pED2K.GetRoot( &pFile->m_oED2K[ 0 ] );
	pFile->m_oED2K.validate();

	LibraryHashDB.StoreTiger( pFile->m_nIndex, const_cast< CTigerTree* >( &m_pTiger ) );
	LibraryHashDB.StoreED2K( pFile->m_nIndex, const_cast< CED2K* >( &m_pED2K ) );
}


//////////////////////////////////////////////////////////////////////
// CLibraryBuilder construction

CLibraryBuilder::CLibraryBuilder()
	: m_bPriority	( false )
	, m_nReaded 	( 0 )
	, m_nElapsed	( 0 )
	, m_nProgress	( 0 )
	, m_bSkip		( false )
	, m_bBusy		( false )
{
	QueryPerformanceFrequency( &m_nFreq );
	QueryPerformanceCounter( &m_nLastCall );
}

CLibraryBuilder::~CLibraryBuilder()
{
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder add and remove

bool CLibraryBuilder::Add(CLibraryFile* pFile)
{
	ASSERT( pFile->m_nIndex );

	if ( pFile->IsReadable() && ! m_bBusy )
	{
		CQuickLock pLock( m_pSection );

		// Check queue
		if ( std::find( m_pFiles.begin(), m_pFiles.end(), pFile->m_nIndex ) == m_pFiles.end() )
		{
			// Check current file
			if ( m_sPath.CompareNoCase( pFile->GetPath() ) )
			{
				m_pFiles.push_back( pFile->m_nIndex );

				BeginThread( "LibraryBuilder", m_bPriority ?
					THREAD_PRIORITY_BELOW_NORMAL : THREAD_PRIORITY_IDLE );

				return true;
			}
		}
	}
	return false;
}

void CLibraryBuilder::Remove(DWORD nIndex)
{
	if ( ! nIndex ) return;

	CQuickLock oLock( m_pSection );

	CFileInfoList::iterator i = std::find( m_pFiles.begin(), m_pFiles.end(), nIndex );
	if ( i != m_pFiles.end() )
		m_pFiles.erase( i );
}

void CLibraryBuilder::Remove(CLibraryFile* pFile)
{
	// Remove file from queue
	Remove( pFile->m_nIndex );

	// Remove currently hashing file
	if ( ! GetCurrent().CompareNoCase( pFile->GetPath() ) && ! m_bSkip )
	{
		m_bSkip = true;

		while ( m_bSkip )
			Sleep( 100 );
	}
}

void CLibraryBuilder::Remove(LPCTSTR szPath)
{
	ASSERT( szPath );

	DWORD nIndex = 0;
	if ( GetRemaining() )
	{
		CQuickLock oLibraryLock( Library.m_pSection );
		if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( szPath ) )
			nIndex = pFile->m_nIndex;
	}
	if ( nIndex )
		Remove( nIndex );

	// Remove currently hashing file
	if ( ! GetCurrent().CompareNoCase( szPath ) && ! m_bSkip )
	{
		m_bSkip = true;

		while ( m_bSkip )
			Sleep( 100 );
	}
}

CString CLibraryBuilder::GetCurrent() const
{
	CQuickLock oLock( m_pSection );
	return m_sPath;
}

size_t CLibraryBuilder::GetRemaining() const
{
	CQuickLock oLock( m_pSection );
	return m_pFiles.size();
}

float CLibraryBuilder::GetProgress() const
{
	CQuickLock oLock( m_pSection );
	return m_nProgress;
}

void CLibraryBuilder::RequestPriority(LPCTSTR pszPath)
{
	ASSERT( pszPath );

	DWORD nIndex = 0;
	{
		CQuickLock oLibraryLock( Library.m_pSection );
		CLibraryFile* pFile = LibraryMaps.LookupFileByPath( pszPath );
		if ( pFile )
			nIndex = pFile->m_nIndex;
	}
	if ( nIndex )
	{
		CQuickLock oLock( m_pSection );
		CFileInfoList::iterator i = std::find( m_pFiles.begin(), m_pFiles.end(), nIndex );
		if ( i != m_pFiles.end() )
		{
			m_pFiles.erase( i );

			m_pFiles.push_front( nIndex );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder rotate queue

void CLibraryBuilder::Skip(DWORD nIndex)
{
	ASSERT( nIndex );

	CQuickLock oLock( m_pSection );
	CFileInfoList::iterator i = std::find( m_pFiles.begin(), m_pFiles.end(), nIndex );
	if ( i != m_pFiles.end() )
	{
		CFileInfo fi( *i );

		m_pFiles.erase( i );

		FILETIME ftCurrentTime;
		GetSystemTimeAsFileTime( &ftCurrentTime );
		fi.nNextAccessTime = MAKEQWORD( ftCurrentTime.dwLowDateTime,
			ftCurrentTime.dwHighDateTime ) + 50000000;	// + 5 sec

		m_pFiles.push_back( fi );
	}
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder get best file to hash

DWORD CLibraryBuilder::GetNextFileToHash(CString& sPath)
{
	DWORD nIndex = 0;
	sPath.Empty();

	CSingleLock oLock( &m_pSection );
	if ( oLock.Lock( 100 ) )
	{
		if ( m_pFiles.empty() )
		{
			// No files left
			Exit();
		}
		else
		{
			// Get next candidate
			FILETIME ftCurrentTime;
			GetSystemTimeAsFileTime( &ftCurrentTime );
			QWORD nCurrentTime = MAKEQWORD( ftCurrentTime.dwLowDateTime, ftCurrentTime.dwHighDateTime );
			for ( CFileInfoList::iterator i = m_pFiles.begin() ; i != m_pFiles.end() ; i++ )
			{
				if ( (*i).nNextAccessTime < nCurrentTime )
				{
					nIndex = (*i).nIndex;
					break;
				}
			}
		}
		oLock.Unlock();
	}

	if ( nIndex )
	{
		CSingleLock oLibraryLock( &Library.m_pSection );
		if ( oLibraryLock.Lock( 100 ) )
		{
			CLibraryFile* pFile = LibraryMaps.LookupFile( nIndex );
			if ( pFile )
				sPath = pFile->GetPath();

			oLibraryLock.Unlock();

			if ( ! pFile )
			{
				// Unknown file
				Remove( nIndex );
				nIndex = 0;
			}
		}
		else	// Library locked
			nIndex = 0;

		if ( nIndex )
		{
			WIN32_FILE_ATTRIBUTE_DATA wfad;
			if ( GetFileAttributesEx( sPath, GetFileExInfoStandard, &wfad ) )
			{
				int nSlash = sPath.ReverseFind( _T('\\') );
				if ( CLibrary::IsBadFile( sPath.Mid( nSlash + 1 ), sPath.Left( nSlash ), wfad.dwFileAttributes ) )
				{
					// Remove bad file
					Remove( nIndex );
					nIndex = 0;
				}
			}
			else
			{
				DWORD err = GetLastError();
				if ( err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND )
					Remove( nIndex );	// Remove if error is fatal
				else
					Skip( nIndex );		// Ignore if error is not fatal (ex. access violation)

				nIndex = 0;
			}
		}
	}

	return nIndex;
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder thread control

void CLibraryBuilder::StopThread()
{
	Exit();
	Wakeup();
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder priority control

void CLibraryBuilder::BoostPriority(bool bPriority)
{
	CQuickLock pLock( m_pSection );

	if ( m_bPriority == bPriority )
		return;

	m_bPriority = bPriority;

	SetThreadPriority( m_bPriority ? THREAD_PRIORITY_BELOW_NORMAL : THREAD_PRIORITY_IDLE );
}

bool CLibraryBuilder::GetBoostPriority() const
{
	CQuickLock pLock( m_pSection );

	return m_bPriority;
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder thread run (threaded)

void CLibraryBuilder::OnRun()
{
	int nAttempts = 0;

	while ( IsThreadEnabled() )
	{
		if ( Settings.Library.HighPriorityHash )
			Sleep( 20 );
		else
			Sleep( 100 );	// Max 10 files per second

		// Delay thread load at startup
		if ( ! theApp.m_bLive )
		{
			Sleep( 0 );
			continue;
		}

		CString sPath;
		const DWORD nIndex = GetNextFileToHash( sPath );
		if ( ! nIndex )
			continue;

		{
			CQuickLock pLock( m_pSection );
			m_sPath = sPath;
		}

		HANDLE hFile = CreateFile( SafePath( sPath ), GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_DELETE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		VERIFY_FILE_ACCESS( hFile, sPath )
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			theApp.Message( MSG_DEBUG, _T("Hashing: %s"), (LPCTSTR)sPath );

			// ToDo: We need MD5 hash of the audio file without tags
			if ( HashFile( sPath, hFile ) )
			{
				nAttempts = 0;
				SetFilePointer( hFile, 0, NULL, FILE_BEGIN );

				try
				{
					ExtractMetadata( nIndex, sPath, hFile );
				}
				catch ( CException* pException )
				{
					pException->Delete();
				}

				ExtractPluginMetadata( nIndex, sPath );

				CThumbCache::Cache( sPath );

				// Done
				Remove( nIndex );
			}
			else if ( ++nAttempts > 4 || m_bSkip )
			{
				Remove( nIndex );
				nAttempts = 0;
			}
			else
			{
				Skip( nIndex );
			}

			CloseHandle( hFile );
		}
		else
		{
			DWORD err = GetLastError();
			if ( err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND )
			{
				Remove( nIndex );	// Fatal error
			}
			else if ( ++nAttempts > 4 || m_bSkip )
			{
				Remove( nIndex );
				nAttempts = 0;
			}
			else
			{
				Skip( nIndex );
			}
		}

		{
			CQuickLock pLock( m_pSection );
			m_sPath.Empty();
			m_nProgress = 0;
			m_bSkip = false;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder file hashing (threaded)

#define MAX_HASH_BUFFER_SIZE	1024ul*256ul	// 256 Kb

bool CLibraryBuilder::HashFile(LPCTSTR szPath, HANDLE hFile)
{
	DWORD nSizeHigh	= 0;
	DWORD nSizeLow	= GetFileSize( hFile, &nSizeHigh );
	QWORD nFileSize	= (QWORD)nSizeLow | ( (QWORD)nSizeHigh << 32 );
	QWORD nFileBase	= 0;

	bool bVirtual = false;

	if ( Settings.Library.VirtualFiles )
		bVirtual = DetectVirtualFile( szPath, hFile, nFileBase, nFileSize );

	nSizeLow  = (DWORD)( nFileBase & 0xFFFFFFFF );
	nSizeHigh = (DWORD)( nFileBase >> 32 );
	SetFilePointer( hFile, nSizeLow, (PLONG)&nSizeHigh, FILE_BEGIN );

	auto_ptr< CFileHash > pFileHash( new CFileHash( nFileSize ) );
	if ( ! pFileHash.get() )
		return false;	// Out of memory

	// Reset statistics if passed more than 10 seconds
	LARGE_INTEGER count1;
	QueryPerformanceCounter( &count1 );
	if ( ( ( ( count1.QuadPart - m_nLastCall.QuadPart ) * 1000000ull ) / m_nFreq.QuadPart ) > 10000 )
	{
		m_nLastCall.QuadPart = count1.QuadPart;
		m_nElapsed = 0;
		m_nReaded = 0;
	}

	if ( theApp.m_bIsVistaOrNewer )
		::SetThreadPriority( GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN );

	void* pBuffer = VirtualAlloc( NULL, MAX_HASH_BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE );
	DWORD nBlock;
	QWORD nLength = nFileSize;
	while ( nLength )
	{
		nBlock = (DWORD)min( nLength, (QWORD)MAX_HASH_BUFFER_SIZE );

		{
			CQuickLock pLock( m_pSection );

			// Calculate % done (nResult = 0.00 -> 100.00)
			m_nProgress = ( ( nFileSize - nLength ) * 100.00f ) / nFileSize;
		}

		if ( ! IsThreadEnabled() || m_bSkip )
			break;

		// Exit loop on read error
		if ( !::ReadFile( hFile, pBuffer, nBlock, &nBlock, NULL ) )
			break;

		QueryPerformanceCounter( &count1 );
		m_nElapsed += ( ( ( count1.QuadPart - m_nLastCall.QuadPart ) * 1000000ull ) / m_nFreq.QuadPart );	// mks
		m_nLastCall.QuadPart = count1.QuadPart;
		m_nReaded += nBlock;

		if ( m_nElapsed > 0 && m_nReaded > 0 )
		{
			// Calculation of compensation delay
			QWORD nSpeed = ( m_nReaded * 1000000ull ) / m_nElapsed;	// B/s
			QWORD nMaxSpeed = 1024 * 1024 * ( m_bPriority ?
				Settings.Library.HighPriorityHashing :
				Settings.Library.LowPriorityHashing );				// B/s
			if ( nMaxSpeed && nSpeed > nMaxSpeed )
			{
				DWORD nDelay = (DWORD) ( ( ( ( nSpeed * m_nElapsed ) / nMaxSpeed ) -
					m_nElapsed ) / 1000ull );	// ms
				if ( nDelay > 1000 )
					nDelay = 1000;	// 1 s
				else if ( nDelay < 1 )
					nDelay = 1;		// 1 ms

				// Compensation
				Sleep( nDelay );
			}

			m_nElapsed = 0;	// mks
			m_nReaded = 0;
		}

		// Exit loop on EOF
		if ( ! nBlock )
			break;

		pFileHash->Add( pBuffer, nBlock );

		nLength -= nBlock;
	}

	if ( theApp.m_bIsVistaOrNewer )
		::SetThreadPriority( GetCurrentThread(), THREAD_MODE_BACKGROUND_END );

	{
		CQuickLock pLock( m_pSection );
		m_nProgress = 100ul;
	}

	VirtualFree( pBuffer, 0, MEM_RELEASE );

	if ( nLength )
		return false;

	pFileHash->Finish();

	CSingleLock oLibraryLock( &Library.m_pSection, FALSE );
	while ( ! oLibraryLock.Lock( 100 ) )
	{
		if ( m_bSkip )
			return false;
	}

	m_bSkip = true;

	CLibraryFile* pFile = LibraryMaps.LookupFileByPath( szPath );
	if ( ! pFile )
		return false;

	Library.RemoveFile( pFile );

	pFile->m_bNewFile		= TRUE;
	pFile->m_nVirtualBase	= bVirtual ? nFileBase : 0;
	pFile->m_nVirtualSize	= bVirtual ? nFileSize : 0;

	pFileHash->CopyTo( pFile );

	LibraryMaps.CullDeletedFiles( pFile );

	BOOL bHistory = FALSE;

	// Get associated download, if any
	CSingleLock oTransfersLock( &Transfers.m_pSection );
	if ( oTransfersLock.Lock( 2000 ) )
	{
		if ( const CDownload* pDownload = Downloads.FindByPath( szPath ) )
		{
			pFile->UpdateMetadata( pDownload );
			LibraryHistory.Add( szPath, pDownload );
			bHistory = TRUE;

			if ( Security.IsDenied( (CPeerProjectFile*)pDownload ) )
			{
				pFile->m_bVerify = TRI_FALSE;
				pFile->SetShared( false );
			}
		}

		oTransfersLock.Unlock();
	}

	// Child pornography check
	if ( Settings.Search.AdultFilter && pFile->IsShared() &&
		( AdultFilter.IsChildPornography( pFile->GetSearchName() ) ||
		  AdultFilter.IsChildPornography( pFile->GetMetadataWords() ) ) )
	{
		pFile->m_bVerify = TRI_FALSE;
		pFile->SetShared( false );
	}

	Library.AddFile( pFile );

	oLibraryLock.Unlock();

	if ( ! bHistory )
		LibraryHistory.Add( szPath );

	Library.Update();

	theApp.Message( MSG_DEBUG, _T("Hashing completed: %s"), szPath );

	return true;
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder metadata submission (threaded)

int CLibraryBuilder::SubmitMetadata(DWORD nIndex, LPCTSTR pszSchemaURI, CXMLElement* pXML)
{
	CSchemaPtr pSchema = SchemaCache.Get( pszSchemaURI );

	if ( pSchema == NULL )
	{
		delete pXML;
		return 0;
	}

	// Validate schema
	auto_ptr< CXMLElement > pBase( pSchema->Instantiate( true ) );
	pBase->AddElement( pXML );
	if ( ! pSchema->Validate( pBase.get(), true ) )
		return 0;
	pXML->Detach();

	int nAttributeCount = pXML->GetAttributeCount();

	CQuickLock oLibraryLock( Library.m_pSection );
	if ( CLibraryFile* pFile = Library.LookupFile( nIndex ) )
	{
		BOOL bMetadataAuto = pFile->m_bMetadataAuto;
		if ( pFile->MergeMetadata( pXML, TRUE ) )
		{
			if ( bMetadataAuto )
				pFile->m_bMetadataAuto = TRUE;
			Library.Update();
		}
	}

	delete pXML;

	return nAttributeCount;
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder bogus/corrupted state submission (threaded)

bool CLibraryBuilder::SubmitCorrupted(DWORD nIndex)
{
	CQuickLock oLibraryLock( Library.m_pSection );
	if ( CLibraryFile* pFile = Library.LookupFile( nIndex ) )
	{
		pFile->m_bBogus = TRUE;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
// CLibraryBuilder virtual file detection (threaded)

bool CLibraryBuilder::DetectVirtualFile(LPCTSTR szPath, HANDLE hFile, QWORD& nOffset, QWORD& nLength)
{
	bool bVirtual = false;

	if ( _tcsistr( szPath, _T(".mp3") ) != NULL )
	{
		bVirtual |= DetectVirtualID3v2( hFile, nOffset, nLength );
		bVirtual |= DetectVirtualID3v1( hFile, nOffset, nLength );
	//	bVirtual |= DetectVirtualLyrics( hFile, nOffset, nLength );
	//	bVirtual |= DetectVirtualAPEHeader( hFile, nOffset, nLength );
	//	bVirtual |= DetectVirtualAPEFooter( hFile, nOffset, nLength );
	//	bVirtual |= DetectVirtualLAME( hFile, nOffset, nLength );
	}

	return bVirtual;
}

bool CLibraryBuilder::DetectVirtualID3v1(HANDLE hFile, QWORD& nOffset, QWORD& nLength)
{
	ID3V1 pInfo = { 0 };
	DWORD nRead;

	if ( nLength <= 128 )
		return false;

	LONG nPosLow	= (LONG)( ( nOffset + nLength - 128 ) & 0xFFFFFFFF );
	LONG nPosHigh	= (LONG)( ( nOffset + nLength - 128 ) >> 32 );
	SetFilePointer( hFile, nPosLow, &nPosHigh, FILE_BEGIN );

	if ( ! ReadFile( hFile, &pInfo, sizeof(pInfo), &nRead, NULL ) )
		return false;
	if ( nRead != sizeof(pInfo) )
		return false;
	if ( memcmp( pInfo.szTag, ID3V1_TAG, 3 ) )
		return false;

	nLength -= 128;

	return true;
}

bool CLibraryBuilder::DetectVirtualID3v2(HANDLE hFile, QWORD& nOffset, QWORD& nLength)
{
	ID3V2_HEADER pHeader = { 0 };
	DWORD nRead;

	LONG nPosLow	= (LONG)( ( nOffset ) & 0xFFFFFFFF );
	LONG nPosHigh	= (LONG)( ( nOffset ) >> 32 );
	SetFilePointer( hFile, nPosLow, &nPosHigh, FILE_BEGIN );

	if ( ! ReadFile( hFile, &pHeader, sizeof(pHeader), &nRead, NULL ) )
		return false;
	if ( nRead != sizeof(pHeader) )
		return false;

	if ( strncmp( pHeader.szTag, ID3V2_TAG, 3 ) )
		return false;
	if ( pHeader.nMajorVersion < 2 || pHeader.nMajorVersion > 4 )
		return false;
	if ( pHeader.nFlags & ~ID3V2_KNOWNMASK )
		return false;
	if ( pHeader.nFlags & ID3V2_UNSYNCHRONISED )
		return false;

	DWORD nTagSize = swapEndianess( pHeader.nSize );
	ID3_DESYNC_SIZE( nTagSize );

	if ( pHeader.nFlags & ID3V2_FOOTERPRESENT )
		nTagSize += 10;
	nTagSize += sizeof(pHeader);

	if ( nLength <= nTagSize )
		return false;

	nOffset += nTagSize;
	nLength -= nTagSize;

	// Remove leading zeroes
	nPosLow  = (LONG)( ( nOffset ) & 0xFFFFFFFF );
	nPosHigh = (LONG)( ( nOffset ) >> 32 );
	SetFilePointer( hFile, nPosLow, &nPosHigh, FILE_BEGIN );
	CHAR szByte;
	while ( ReadFile( hFile, &szByte, 1, &nRead, NULL ) && nRead == 1 && szByte == '\0' )
	{
		nOffset++;
		nLength--;
	}
	return true;
}

bool CLibraryBuilder::DetectVirtualAPEHeader(HANDLE hFile, QWORD& nOffset, QWORD& nLength)
{
	APE_HEADER pHeader = {};
	DWORD nRead;

	LONG nPosLow  = (LONG)( ( nOffset ) & 0xFFFFFFFF );
	LONG nPosHigh = (LONG)( ( nOffset ) >> 32 );
	SetFilePointer( hFile, nPosLow, &nPosHigh, FILE_BEGIN );

	if ( ! ReadFile( hFile, &pHeader, sizeof(pHeader), &nRead, NULL ) )
		return false;
	if ( nRead != sizeof(pHeader) )
		return false;

	const char szMAC[ 4 ] = "MAC";
	if ( memcmp( pHeader.cID, szMAC, 3 ) )
		return false;

	DWORD nTagSize = 0;
	if ( pHeader.nVersion >= APE2_VERSION )
	{
		APE_HEADER_NEW pNewHeader = {};
		SetFilePointer( hFile, nPosLow, &nPosHigh, FILE_BEGIN );
		if ( ! ReadFile( hFile, &pNewHeader, sizeof(pNewHeader), &nRead, NULL ) )
			return false;
		if ( nRead != sizeof(pNewHeader) )
			return false;
		nTagSize = pNewHeader.nHeaderBytes + sizeof(pNewHeader);
	}
	else
		nTagSize = pHeader.nHeaderBytes + sizeof(pHeader);

	if ( nLength <= nTagSize )
		return false;

	nOffset += nTagSize;
	nLength -= nTagSize;

	return true;
}

bool CLibraryBuilder::DetectVirtualAPEFooter(HANDLE hFile, QWORD& nOffset, QWORD& nLength)
{
	APE_TAG_FOOTER pFooter = { 0 };
	if ( nLength < sizeof(pFooter) )
		return false;

	DWORD nRead;
	LONG nPosLow  = (LONG)( ( nOffset + nLength - sizeof(pFooter) ) & 0xFFFFFFFF );
	LONG nPosHigh = (LONG)( ( nOffset + nLength - sizeof(pFooter) ) >> 32 );

	SetFilePointer( hFile, nPosLow, &nPosHigh, FILE_BEGIN );
	if ( ! ReadFile( hFile, &pFooter, sizeof(pFooter), &nRead, NULL ) )
		return false;
	if ( nRead != sizeof(pFooter) )
		return false;

	const char szAPE[ 9 ] = "APETAGEX";
	if ( memcmp( pFooter.cID, szAPE, 8 ) )
		return false;
	if ( pFooter.nSize + sizeof(pFooter) > nLength )
		return false;

	nLength -= sizeof(pFooter) + pFooter.nSize;

	return true;
}

bool CLibraryBuilder::DetectVirtualLyrics(HANDLE hFile, QWORD& nOffset, QWORD& nLength)
{
	typedef struct Lyrics3v2
	{
		CHAR nSize[6];
		struct LyricsTag
		{
			CHAR szID[6];
			CHAR szVersion[3];
		} Tag;
	} LYRICS3_2;

	if ( nLength < 15 )
		return false;

	LYRICS3_2 pFooter = { 0 };
	DWORD nRead;

	LONG nPosLow  = (LONG)( ( nOffset + nLength - sizeof(pFooter) ) & 0xFFFFFFFF );
	LONG nPosHigh = (LONG)( ( nOffset + nLength - sizeof(pFooter) ) >> 32 );
	SetFilePointer( hFile, nPosLow, &nPosHigh, FILE_BEGIN );

	if ( ! ReadFile( hFile, &pFooter, sizeof(pFooter), &nRead, NULL ) )
		return false;
	if ( nRead != sizeof(pFooter) )
		return false;

	const char cLyrics[ 7 ] = "LYRICS";
	const char cVersion[ 4 ] = "200";	// version 2.00
	const char cEnd[ 4 ] = "END";		// version 1.00

	if ( memcmp( pFooter.Tag.szID, cLyrics, 6 ) )
		return false;

	CString strLength( pFooter.nSize, 6 );
	QWORD nSize = 0;
	if ( memcmp( pFooter.Tag.szVersion, cVersion, 3 ) == 0 &&
		 _stscanf( strLength.TrimLeft('0'), L"%I64u", &nSize ) == 1 )
	{
		if ( nSize + sizeof(pFooter) > nLength )
			return false;
		nLength -= nSize + sizeof(pFooter);
		return true;
	}
	else if ( memcmp( pFooter.Tag.szVersion, cEnd, 3 ) )
	{
		// ToDo: Find "LYRICSBEGIN" reading backwards and count the length manually
		return false;
	}

	return false;
}

bool CLibraryBuilder::DetectVirtualLAME(HANDLE hFile, QWORD& nOffset, QWORD& nLength)
{
	BYTE nFrameHeader[4] = { 0 };
	DWORD nRead;

	LONG nPosLow	= (LONG)( ( nOffset ) & 0xFFFFFFFF );
	LONG nPosHigh	= (LONG)( ( nOffset ) >> 32 );
	SetFilePointer( hFile, nPosLow, &nPosHigh, FILE_BEGIN );

	if ( ! ReadFile( hFile, nFrameHeader, sizeof(nFrameHeader), &nRead, NULL ) )
		return false;
	if ( nRead != sizeof(nFrameHeader) )
		return false;

	const int bitrate_table[ 3 ][ 16 ] = {
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, -1 },		// MPEG 2
		{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, -1 },	// MPEG 1
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, -1, -1, -1, -1, -1, -1, -1 }			// MPEG 2.5
	};

	const int samplerate_table[ 3 ][ 4 ] = {
		{ 22050, 24000, 16000, -1 },	// MPEG 2
		{ 44100, 48000, 32000, -1 },	// MPEG 1
		{ 11025, 12000, 8000, -1 }		// MPEG 2.5
	};

	// Get MPEG header data
	int nId = ( nFrameHeader[1] >> 3 ) & 1;
	int nSampleRateIndex = ( nFrameHeader[2] >> 2 ) & 3;
	int nMode = ( nFrameHeader[3] >> 6 ) & 3;
	int nBitrate = ( nFrameHeader[2] >> 4 ) & 0xf;
	nBitrate = bitrate_table[ nId ][ nBitrate ];

	int nSampleRate = 0;
	// Check for FFE syncword
	if ( ( nFrameHeader[1] >> 4 ) == 0xE )
		nSampleRate = samplerate_table[ 2 ][ nSampleRateIndex ];
	else
		nSampleRate = samplerate_table[ nId ][ nSampleRateIndex ];
	int nFrameSize = ( ( nId + 1 ) * 72000 * nBitrate ) / nSampleRate;
	if ( nFrameSize > nLength )
		return false;

	int nVbrHeaderOffset = GetVbrHeaderOffset( nId, nMode );
	LARGE_INTEGER nNewOffset = { 0 };
	nNewOffset.HighPart = (LONG)( ( nOffset + nVbrHeaderOffset ) >> 32 );
	nNewOffset.LowPart  = (LONG)( ( nOffset + nVbrHeaderOffset ) & 0xFFFFFFFF );
	nNewOffset.LowPart  = SetFilePointer( hFile, nNewOffset.LowPart, &(nNewOffset.HighPart), FILE_BEGIN );

	if ( nNewOffset.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
		return false;

	LAME_FRAME pFrame = { 0 };
	const LAME_FRAME pEmtyRef = { 0 };
	bool bChanged = false;

	if ( ! ReadFile( hFile, &pFrame, sizeof(pFrame), &nRead, NULL ) || nRead != sizeof(pFrame) )
		return false;
	if ( memcmp( &pFrame, &pEmtyRef, sizeof(pFrame) ) == 0 )	// All zeros, strip them off
	{
		bChanged = true;
		nLength -= nVbrHeaderOffset + sizeof(pFrame);
		nOffset = nNewOffset.QuadPart + sizeof(pFrame);
		nNewOffset.LowPart = (LONG)( ( nOffset ) & 0xFFFFFFFF );
		nNewOffset.HighPart = (LONG)( ( nOffset ) >> 32 );
		SetFilePointer( hFile, nNewOffset.LowPart, &(nNewOffset.HighPart), FILE_BEGIN );
		CHAR szByte;
		while ( ReadFile( hFile, &szByte, 1, &nRead, NULL ) && nRead == 1 && szByte == '\0' )
		{
			nOffset++;
			nLength--;
		}
	}
	else if ( memcmp( &pFrame, "Xing", 4 ) == 0 )
	{
		bChanged = true;
		nOffset += nFrameSize;
		nLength -= nFrameSize;
	}
	else if ( memcmp( pFrame.ClassID, "LAME", 4 ) == 0 )	// LAME encoder
	{
		bChanged = true;
		DWORD nMusicLength = swapEndianess( pFrame.MusicLength ) - nFrameSize;	// Minus the first frame
		nOffset += nFrameSize;

		if ( nFrameSize + nMusicLength > nLength )
			return false;
		nLength = nMusicLength;
	}

	nFrameSize++;

	char szTrail = '\0';

	// Strip off silence and incomplete frames from the end (hackish way)
	for ( ; nFrameSize > 0 ; )
	{
		nNewOffset.LowPart = (LONG)( ( nOffset + nLength - nFrameSize ) & 0xFFFFFFFF );
		nNewOffset.HighPart = (LONG)( ( nOffset + nLength - nFrameSize ) >> 32 );
		nNewOffset.LowPart = SetFilePointer( hFile, nNewOffset.LowPart, &(nNewOffset.HighPart), FILE_BEGIN );
		if ( nNewOffset.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
			break;

		WORD nTestBytes = 0;
		if ( ! ReadFile( hFile, &nTestBytes, sizeof(nTestBytes), &nRead, NULL ) || nRead != sizeof(WORD) )
			break;
		if ( memcmp( &nTestBytes, nFrameHeader, 2 ) )	// Doesn't match the start of the first frame
		{
			nFrameSize--;	// Shorten frame size from the end until it becomes so small to fit header
			continue;
		}

		nFrameHeader[ 0 ] = LOBYTE( nTestBytes );
		nFrameHeader[ 1 ] = HIBYTE( nTestBytes );
		if ( ! ReadFile( hFile, &nTestBytes, sizeof(nTestBytes), &nRead, NULL ) || nRead != sizeof(WORD) )
			break;
		nFrameHeader[ 2 ] = LOBYTE( nTestBytes );
		nFrameHeader[ 3 ] = HIBYTE( nTestBytes );

		// Get MPEG header data
		nId = ( nFrameHeader[1] >> 3 ) & 1;
		nMode = ( nFrameHeader[3] >> 6 ) & 3;
		nVbrHeaderOffset = GetVbrHeaderOffset( nId, nMode );
		QWORD nCurrOffset = nNewOffset.QuadPart + nVbrHeaderOffset;
		nNewOffset.LowPart = (LONG)( ( nCurrOffset ) & 0xFFFFFFFF );
		nNewOffset.HighPart = (LONG)( ( nCurrOffset ) >> 32 );
		nNewOffset.LowPart = SetFilePointer( hFile, nNewOffset.LowPart, &(nNewOffset.HighPart), FILE_BEGIN );

		if ( nNewOffset.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
			break;

		int nLen = sizeof( pFrame );
		ZeroMemory( &pFrame, nLen );
		if ( ! ReadFile( hFile, &pFrame, min( nLen, nFrameSize - nVbrHeaderOffset ), &nRead, NULL ) )
			break;

		nLen--;
		char* pszChars = (char*)&pFrame;
		while ( nLen && pszChars[ nLen-- ] == pFrame.ClassID[ 0 ] );

		if ( nLen == 0 )	// All bytes equal
		{
			bChanged = true;
			nLength -= nFrameSize;
			szTrail = pFrame.ClassID[ 0 ];
		}
		else
			break;
	}

	// Remove trailing bytes
	for ( ;; )
	{
		nNewOffset.LowPart = (LONG)( ( nOffset + nLength - 1 ) & 0xFFFFFFFF );
		nNewOffset.HighPart = (LONG)( ( nOffset + nLength - 1 ) >> 32 );
		nNewOffset.LowPart = SetFilePointer( hFile, nNewOffset.LowPart, &(nNewOffset.HighPart), FILE_BEGIN );
		if ( nNewOffset.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
			break;
		CHAR szByte;
		if ( ! ReadFile( hFile, &szByte, 1, &nRead, NULL ) || nRead != 1 || szByte != szTrail )
			break;

		bChanged = true;
		nLength--;
	}

	// Last LAME ID is one byte shorter (Bug? verify beta versions)
	nNewOffset.LowPart = (LONG)( ( nOffset + nLength - 8 ) & 0xFFFFFFFF );
	nNewOffset.HighPart = (LONG)( ( nOffset + nLength - 8 ) >> 32 );
	nNewOffset.LowPart = SetFilePointer( hFile, nNewOffset.LowPart, &(nNewOffset.HighPart), FILE_BEGIN );
	if ( nNewOffset.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
		return bChanged;

	if ( ! ReadFile( hFile, pFrame.ClassID, 9, &nRead, NULL ) || nRead != 9 )
		return bChanged;

	if ( memcmp( pFrame.ClassID, "LAME", 4 ) == 0 )
	{
		bChanged = true;
		nLength -= 8;
	}

	return bChanged;
}

bool CLibraryBuilder::RefreshMetadata(const CString& sPath)
{
	CWaitCursor wc;
	DWORD nIndex;

	{
		CQuickLock oLibraryLock( Library.m_pSection );
		CLibraryFile* pFile = LibraryMaps.LookupFileByPath( sPath );
		if ( ! pFile )
			return false;
		nIndex = pFile->m_nIndex;
		pFile->m_bMetadataAuto = TRUE;
	}

	theApp.Message( MSG_DEBUG, _T("Refreshing: %s"), (LPCTSTR)sPath );

	bool bResult = false;
	HANDLE hFile = CreateFile( SafePath( sPath ), GENERIC_READ,
		 FILE_SHARE_READ | FILE_SHARE_DELETE, NULL,
		 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	VERIFY_FILE_ACCESS( hFile, sPath )
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		SetFilePointer( hFile, 0, NULL, FILE_BEGIN );

		try
		{
			bResult |= ExtractMetadata( nIndex, sPath, hFile );
		}
		catch ( CException* pException )
		{
			pException->Delete();
		}

		CloseHandle( hFile );
	}

	bResult |= ExtractPluginMetadata( nIndex, sPath );

	return bResult;
}
