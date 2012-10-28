//
// EDPartImporter.cpp
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
#include "EDPartImporter.h"
#include "EDPacket.h"
#include "Transfers.h"
#include "Download.h"
#include "Downloads.h"
#include "DownloadGroups.h"
#include "DownloadTask.h"
#include "FragmentedFile.h"
#include "CtrlText.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CEDPartImporter, CAppThread)

BEGIN_MESSAGE_MAP(CEDPartImporter, CAppThread)
	//{{AFX_MSG_MAP(CEDPartImporter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEDPartImporter construction

CEDPartImporter::CEDPartImporter()
	: m_pTextCtrl ( NULL )
{
	m_bAutoDelete = FALSE;
}

CEDPartImporter::~CEDPartImporter()
{
}

/////////////////////////////////////////////////////////////////////////////
// CEDPartImporter operations

void CEDPartImporter::AddFolder(LPCTSTR pszFolder)
{
	if ( m_pTextCtrl != NULL ) return;
	m_pFolders.AddTail( pszFolder );
}

void CEDPartImporter::Start(CEdit* pCtrl)
{
	ASSERT( pCtrl != NULL );
	m_pTextCtrl = pCtrl;
	CreateThread( "ED Part Importer" );
}

void CEDPartImporter::Stop()
{
	if ( m_pTextCtrl == NULL ) return;
	m_pTextCtrl = NULL;
	WaitForSingleObject( m_hThread, INFINITE );
}

BOOL CEDPartImporter::IsRunning()
{
	if ( m_hThread == NULL ) return FALSE;
	DWORD nCode = 0;
	if ( ! GetExitCodeThread( m_hThread, &nCode ) ) return FALSE;
	return nCode == STILL_ACTIVE;
}

/////////////////////////////////////////////////////////////////////////////
// CEDPartImporter run

int CEDPartImporter::Run()
{
	BOOL bCOM = SUCCEEDED( OleInitialize( NULL ) );

	Message( IDS_ED2K_EPI_START );
	m_nCount = 0;

	CreateDirectory( Settings.Downloads.IncompletePath );

	for ( POSITION pos = m_pFolders.GetHeadPosition() ; pos && m_pTextCtrl != NULL ; )
	{
		ImportFolder( m_pFolders.GetNext( pos ) );
	}

	Message( IDS_ED2K_EPI_FINISHED, m_nCount );

	if ( m_nCount )
		Downloads.Save();

	if ( bCOM )
		OleUninitialize();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CEDPartImporter import a folder

void CEDPartImporter::ImportFolder(LPCTSTR pszPath)
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	HANDLE hSearch;

	Message( IDS_ED2K_EPI_FOLDER, pszPath );

	strPath.Format( _T("%s\\*.part.met"), pszPath );
	hSearch = FindFirstFile( strPath, &pFind );
	if ( hSearch == INVALID_HANDLE_VALUE ) return;

	do
	{
		if ( m_pTextCtrl == NULL ) break;

		if ( pFind.cFileName[0] == '.' ) continue;
		if ( ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) continue;

		strPath = pFind.cFileName;
		int nPos = strPath.Find( _T(".part.met") );
		if ( nPos < 1 ) continue;
		strPath = strPath.Left( nPos );

		if ( ImportFile( pszPath, strPath ) )
			m_nCount++;
		else
			Message( IDS_ED2K_EPI_FILE_FAILED );
	}
	while ( FindNextFile( hSearch, &pFind ) );

	FindClose( hSearch );
}

/////////////////////////////////////////////////////////////////////////////
// CEDPartImporter import file

BOOL CEDPartImporter::ImportFile(LPCTSTR pszPath, LPCTSTR pszFile)
{
	Message( IDS_ED2K_EPI_FILE_START, pszFile );

	CString strPath;
	strPath.Format( _T("%s\\%s.part.met"), pszPath, pszFile );

	CFile pFile;
	if ( ! pFile.Open( strPath, CFile::modeRead ) )
	{
		Message( IDS_ED2K_EPI_CANT_OPEN_PART, (LPCTSTR)strPath );
		return FALSE;
	}

	BYTE nMagic;
	if ( pFile.Read( &nMagic, 1 ) != 1 )
		return FALSE;

	if ( nMagic != 0xE0 )
		return FALSE;

	LONG nDate;
	if ( pFile.Read( &nDate, 4 ) != 4 )
		return FALSE;

	Hashes::Ed2kHash oED2K;
	if ( pFile.Read( &*oED2K.begin(), oED2K.byteCount ) != oED2K.byteCount )
		return FALSE;
	oED2K.validate();

	WORD nParts;
	if ( pFile.Read( &nParts, 2 ) != 2 )
		return FALSE;

	{
		CQuickLock oTransfersLock( Transfers.m_pSection );

		if ( Downloads.FindByED2K( oED2K ) )
		{
			Message( IDS_ED2K_EPI_ALREADY );
			return FALSE;
		}
	}

	CED2K pED2K;
	if ( nParts == 0 )
	{
		pED2K.FromRoot( &oED2K[ 0 ] );
	}
	else if ( nParts > 0 )
	{
		UINT len = sizeof( CMD4::Digest ) * nParts;
		auto_array< CMD4::Digest > pHashset( new CMD4::Digest[ nParts ] );
		if ( pFile.Read( pHashset.get(), len ) != len )
			return FALSE;

		BOOL bSuccess = pED2K.FromBytes( (BYTE*)pHashset.get(), len );
		if ( ! bSuccess )
			return FALSE;

		Hashes::Ed2kHash pCheck;
		pED2K.GetRoot( &pCheck[ 0 ] );
		pCheck.validate();
		if ( validAndUnequal( pCheck, oED2K ) )
			return FALSE;
	}

	if ( ! pED2K.IsAvailable() )
		return FALSE;

	DWORD nCount;
	if ( pFile.Read( &nCount, 4 ) != 4 )
		return FALSE;
	if ( nCount > 2048 )
		return FALSE;

	CMap< int, int, QWORD, QWORD > pGapStart, pGapStop;
	CArray< int > pGapIndex;
	BOOL bPaused = FALSE;
	CString strName, strPartName;
	QWORD nSize = 0;

	while ( nCount-- )
	{
		CEDTag pTag;
		if ( ! pTag.Read( &pFile ) )
			return FALSE;

		if ( pTag.Check( ED2K_FT_FILENAME, ED2K_TAG_STRING ) )
		{
			strName = pTag.m_sValue;
		}
		else if ( pTag.Check( ED2K_FT_PARTFILENAME, ED2K_TAG_STRING ) )
		{
			strPartName = pTag.m_sValue;
		}
		else if ( pTag.Check( ED2K_FT_FILESIZE, ED2K_TAG_INT ) )
		{
			nSize = pTag.m_nValue;
		}
		else if ( pTag.Check( ED2K_FT_STATUS, ED2K_TAG_INT ) )
		{
			bPaused = pTag.m_nValue != 0;
		}
		else if ( pTag.m_nType == ED2K_TAG_INT && pTag.m_sKey.GetLength() > 1 )
		{
			if ( pTag.m_sKey.GetAt( 0 ) == 0x09 )		// Start of gap
			{
				int nPart = 0;
				_stscanf( (LPCTSTR)pTag.m_sKey + 1, _T("%i"), &nPart );
				pGapStart.SetAt( nPart, pTag.m_nValue );
				pGapIndex.Add( nPart );
			}
			else if ( pTag.m_sKey.GetAt( 0 ) == 0x0A )	// End of gap
			{
				int nPart = 0;
				_stscanf( (LPCTSTR)pTag.m_sKey + 1, _T("%i"), &nPart );
				pGapStop.SetAt( nPart, pTag.m_nValue );
			}
		}

		if ( m_pTextCtrl == NULL )
			return FALSE;
	}

	if ( strName.IsEmpty() || nSize == SIZE_UNKNOWN || nSize == 0 || pGapStart.IsEmpty() )
		return FALSE;

	// Test gap list
	Fragments::List oGaps( nSize );
	for ( int nGap = 0 ; nGap < pGapIndex.GetSize() ; nGap++ )
	{
		int nPart = pGapIndex.GetAt( nGap );
		QWORD nStart, nStop;
		if ( ! pGapStart.Lookup( nPart, nStart ) )
			return FALSE;
		if ( nStart >= nSize )
			return FALSE;
		if ( ! pGapStop.Lookup( nPart, nStop ) )
			return FALSE;
		if ( nStop > nSize || nStop <= nStart )
			return FALSE;

		oGaps.insert( Fragments::Fragment( nStart, nStop ) );
	}

	Message( IDS_ED2K_EPI_DETECTED, strName, Settings.SmartVolume( nSize ) );

	if ( ! Downloads.IsSpaceAvailable( nSize, Downloads.dlPathIncomplete ) )
	{
		Message( IDS_ED2K_EPI_DISK_SPACE );
		return FALSE;
	}

	if ( strPartName.IsEmpty() )
		strPath.Format( _T("%s\\%s.part"), pszPath, pszFile );
	else
		strPath.Format( _T("%s\\%s"), pszPath, (LPCTSTR)strPartName );

	CFile pData;
	if ( ! pData.Open( strPath, CFile::modeRead ) )
		return FALSE;

	CFileStatus pStatus;
	if ( ! pData.GetStatus( pStatus ) )
		return FALSE;

	pData.Close();

	struct tm ptmTemp = {};
	if ( nDate > mktime( pStatus.m_mtime.GetLocalTm( &ptmTemp ) ) )
	{
		Message( IDS_ED2K_EPI_FILE_OLD );
		return FALSE;
	}

	Message( IDS_ED2K_EPI_COPY_FINISHED );

	CQuickLock oTransfersLock( Transfers.m_pSection );

	CDownload* pDownload = Downloads.Add();
	if ( ! pDownload ) return FALSE;

	pDownload->m_sName			= strName;
	pDownload->m_nSize			= nSize;
	pDownload->m_oED2K			= oED2K;
	pDownload->m_bED2KTrusted	= true;		// .part use trusted hashes
	pDownload->Pause();

	BYTE* pHashset = NULL;
	DWORD nHashset = 0;
	if ( pED2K.ToBytes( &pHashset, &nHashset ) )
	{
		pDownload->SetHashset( pHashset, nHashset );
		GlobalFree( pHashset );
	}

	pDownload->Save();

	DownloadGroups.Link( pDownload );

	Message( IDS_ED2K_EPI_COPY_START, (LPCTSTR)strPath, (LPCTSTR)pDownload->m_sPath );

// ToDo: Alt?
//	CList < CString > oFiles;
//	oFiles.AddHead( strPath );
//	CDownloadTask::MergeFile( pDownload, &oFiles, FALSE, &oGaps );

	CDownloadTask::MergeFile( pDownload, strPath, FALSE, &oGaps );

	if ( ! bPaused )
		pDownload->Resume();

	Message( IDS_ED2K_EPI_FILE_CREATED,
		Settings.SmartVolume( pDownload->GetVolumeRemaining() ) );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEDPartImporter message

void CEDPartImporter::Message(UINT nMessageID, ...)
{
	CEdit* pCtrl = m_pTextCtrl;
	if ( pCtrl == NULL ) return;

	const DWORD nBufferLength = 2048;
	auto_array< TCHAR > szBuffer( new TCHAR[ nBufferLength ] );
	ZeroMemory( szBuffer.get(), nBufferLength * sizeof( TCHAR ) );
	CString strFormat;
	va_list pArgs;

	LoadString( strFormat, nMessageID );
	va_start( pArgs, nMessageID );
	_vsntprintf_s( szBuffer.get(), nBufferLength, nBufferLength - 8, strFormat, pArgs );
	_tcscat( szBuffer.get(), _T("\r\n") );
	va_end( pArgs );

	int nLen = pCtrl->GetWindowTextLength();
	pCtrl->SetSel( nLen, nLen );
	pCtrl->ReplaceSel( szBuffer.get() );
	nLen += static_cast< int >( _tcslen( szBuffer.get() ) );
	pCtrl->SetSel( nLen, nLen );
}
