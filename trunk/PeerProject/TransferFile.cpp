//
// TransferFile.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
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
#include "TransferFile.h"
#include "Download.h"
#include "Downloads.h"
#include "Transfers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CTransferFiles TransferFiles;


//////////////////////////////////////////////////////////////////////
// CTransferFiles construction

CTransferFiles::CTransferFiles()
{
}

CTransferFiles::~CTransferFiles()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
// CTransferFiles open a file

CTransferFile* CTransferFiles::Open(LPCTSTR pszFile, BOOL bWrite, BOOL bCreate)
{
	CSingleLock pLock( &m_pSection, TRUE );
	CTransferFile* pFile = NULL;

	if ( m_pMap.Lookup( pszFile, pFile ) )
	{
		if ( bWrite && ! pFile->EnsureWrite() ) return NULL;
	}
	else
	{
		pFile = new CTransferFile( pszFile );

		if ( ! pFile->Open( bWrite, bCreate ) )
		{
			delete pFile;
			return NULL;
		}

		m_pMap.SetAt( pFile->m_sPath, pFile );
	}

	pFile->AddRef();

	return pFile;
}

//////////////////////////////////////////////////////////////////////
// CTransferFiles close all files

void CTransferFiles::Close()
{
	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION pos = m_pMap.GetStartPosition() ; pos ; )
	{
		CTransferFile* pFile;
		CString strPath;

		m_pMap.GetNextAssoc( pos, strPath, pFile );
		delete pFile;
	}

	m_pMap.RemoveAll();
	m_pDeferred.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CTransferFiles commit deferred writes

void CTransferFiles::CommitDeferred()
{
	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION pos = m_pDeferred.GetHeadPosition() ; pos ; )
	{
		CTransferFile* pFile = m_pDeferred.GetNext( pos );
		pFile->DeferredWrite( TRUE );
	}

	m_pDeferred.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CTransferFiles queue for deferred write

void CTransferFiles::QueueDeferred(CTransferFile* pFile)
{
	if ( NULL == m_pDeferred.Find( pFile ) ) m_pDeferred.AddTail( pFile );
}

//////////////////////////////////////////////////////////////////////
// CTransferFiles remove a single file

void CTransferFiles::Remove(CTransferFile* pFile)
{
	m_pMap.RemoveKey( pFile->m_sPath );
	if ( POSITION pos = m_pDeferred.Find( pFile ) ) m_pDeferred.RemoveAt( pos );
}


//////////////////////////////////////////////////////////////////////
// CTransferFile construction

CTransferFile::CTransferFile(LPCTSTR pszPath)
{
	m_sPath				= pszPath;
	m_hFile				= INVALID_HANDLE_VALUE;
	m_nReference		= 0;
	m_bWrite			= FALSE;
	m_nDeferred			= 0;
}

CTransferFile::~CTransferFile()
{
	if ( m_hFile != INVALID_HANDLE_VALUE )
	{
		DeferredWrite();
		CloseHandle( m_hFile );
	}
}

//////////////////////////////////////////////////////////////////////
// CTransferFile reference counts

void CTransferFile::AddRef()
{
	CSingleLock pLock( &TransferFiles.m_pSection, TRUE );
	m_nReference++;
}

void CTransferFile::Release(BOOL bWrite)
{
	CSingleLock pLock( &TransferFiles.m_pSection, TRUE );

	if ( ! --m_nReference )
	{
		TransferFiles.Remove( this );
		delete this;
		return;
	}

	if ( m_bWrite && bWrite ) CloseWrite();
}

//////////////////////////////////////////////////////////////////////
// CTransferFile handle

HANDLE CTransferFile::GetHandle(BOOL bWrite)
{
	CSingleLock pLock( &TransferFiles.m_pSection, TRUE );

	if ( bWrite && ! m_bWrite ) return INVALID_HANDLE_VALUE;
	if ( m_nDeferred > 0 ) DeferredWrite();

	return m_hFile;
}

BOOL CTransferFile::IsOpen()
{
	return m_hFile != INVALID_HANDLE_VALUE;
}

//////////////////////////////////////////////////////////////////////
// CTransferFile open

BOOL CTransferFile::Open(BOOL bWrite, BOOL bCreate)
{
	if ( m_hFile != INVALID_HANDLE_VALUE ) return FALSE;

	// Don't touch moving files
	{
		CQuickLock pLock( Transfers.m_pSection );
		if ( CDownload* pDownload = Downloads.FindByPath( m_sPath ) )
			if ( pDownload->IsMoving() && pDownload->IsTasking() )
				return FALSE;
	}

	m_hFile = CreateFile( m_sPath, GENERIC_READ | ( bWrite ? GENERIC_WRITE : 0 ),
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, ( bCreate ? CREATE_ALWAYS : OPEN_EXISTING ), FILE_ATTRIBUTE_NORMAL, NULL );
	VERIFY_FILE_ACCESS( m_hFile, m_sPath )
	if ( m_hFile != INVALID_HANDLE_VALUE ) m_bWrite = bWrite;

	return m_hFile != INVALID_HANDLE_VALUE;
}

//////////////////////////////////////////////////////////////////////
// CTransferFile write access management

BOOL CTransferFile::EnsureWrite()
{
	if ( m_hFile == INVALID_HANDLE_VALUE ) return FALSE;
	if ( m_bWrite ) return TRUE;

	CloseHandle( m_hFile );
	m_hFile = INVALID_HANDLE_VALUE;

	if ( Open( TRUE, FALSE ) ) return TRUE;

	Open( FALSE, FALSE );

	return FALSE;
}

BOOL CTransferFile::CloseWrite()
{
	if ( m_hFile == INVALID_HANDLE_VALUE ) return FALSE;
	if ( ! m_bWrite ) return TRUE;

	DeferredWrite();

	CloseHandle( m_hFile );
	m_hFile = INVALID_HANDLE_VALUE;

	return Open( FALSE, FALSE );
}

//////////////////////////////////////////////////////////////////////
// CTransferFile read

BOOL CTransferFile::Read(QWORD nOffset, LPVOID pBuffer, QWORD nBuffer, QWORD* pnRead)
{
	CSingleLock pLock( &TransferFiles.m_pSection, TRUE );

	*pnRead = 0;
	if ( m_hFile == INVALID_HANDLE_VALUE ) return FALSE;
	if ( m_nDeferred > 0 ) DeferredWrite();

	DWORD nOffsetLow	= (DWORD)( nOffset & 0x00000000FFFFFFFF );
	DWORD nOffsetHigh	= (DWORD)( ( nOffset & 0xFFFFFFFF00000000 ) >> 32 );
	SetFilePointer( m_hFile, nOffsetLow, (PLONG)&nOffsetHigh, FILE_BEGIN );

	return ReadFile( m_hFile, pBuffer, (DWORD)nBuffer, (DWORD*)pnRead, NULL );
}

//////////////////////////////////////////////////////////////////////
// CTransferFile write (with deferred extension)

#define DEFERRED_THRESHOLD		(20*1024*1024)

BOOL CTransferFile::Write(QWORD nOffset, LPCVOID pBuffer, QWORD nBuffer, QWORD* pnWritten)
{
	CSingleLock pLock( &TransferFiles.m_pSection, TRUE );

	*pnWritten = 0;
	if ( m_hFile == INVALID_HANDLE_VALUE ) return FALSE;
	if ( ! m_bWrite ) return FALSE;

	if ( nOffset > DEFERRED_THRESHOLD )
	{
		DWORD nSizeHigh = 0;
		QWORD nSize = (QWORD)GetFileSize( m_hFile, &nSizeHigh );
		nSize |= ( (QWORD)nSizeHigh << 32 );

		if ( nOffset > nSize && nOffset - nSize > DEFERRED_THRESHOLD )
		{
			TransferFiles.QueueDeferred( this );

			if ( m_nDeferred >= DEFERRED_MAX ) DeferredWrite();

			DefWrite* pWrite = &m_pDeferred[ m_nDeferred++ ];

			pWrite->m_nOffset	= nOffset;
			pWrite->m_nLength	= (DWORD)nBuffer;
			pWrite->m_pBuffer	= new BYTE[ (DWORD)nBuffer ];
			CopyMemory( pWrite->m_pBuffer, pBuffer, (DWORD)nBuffer );
			*pnWritten = nBuffer;

			return TRUE;
		}
	}

	DWORD nOffsetLow	= (DWORD)( nOffset & 0x00000000FFFFFFFF );
	DWORD nOffsetHigh	= (DWORD)( ( nOffset & 0xFFFFFFFF00000000 ) >> 32 );
	SetFilePointer( m_hFile, nOffsetLow, (PLONG)&nOffsetHigh, FILE_BEGIN );

	return WriteFile( m_hFile, pBuffer, (DWORD)nBuffer, (LPDWORD)pnWritten, NULL );
}

//////////////////////////////////////////////////////////////////////
// CTransferFile deferred writes

void CTransferFile::DeferredWrite(BOOL /*bOffline*/)
{
	if ( m_nDeferred == 0 ) return;
	if ( m_hFile == INVALID_HANDLE_VALUE ) return;
	if ( ! m_bWrite ) return;

	DefWrite* pWrite = m_pDeferred;

	for ( int nDeferred = 0 ; nDeferred < m_nDeferred ; nDeferred++, pWrite++ )
	{
		DWORD nOffsetLow	= (DWORD)( pWrite->m_nOffset & 0x00000000FFFFFFFF );
		DWORD nOffsetHigh	= (DWORD)( ( pWrite->m_nOffset & 0xFFFFFFFF00000000 ) >> 32 );
		SetFilePointer( m_hFile, nOffsetLow, (PLONG)&nOffsetHigh, FILE_BEGIN );

		DWORD nWritten = 0;
		WriteFile( m_hFile, pWrite->m_pBuffer, pWrite->m_nLength, &nWritten, NULL );

		delete [] pWrite->m_pBuffer;
	}

	m_nDeferred = 0;
}
