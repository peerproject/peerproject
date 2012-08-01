//
// Download.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "Settings.h"
#include "PeerProject.h"
#include "Download.h"
#include "Downloads.h"
#include "DownloadGroups.h"
#include "DownloadSource.h"
#include "DownloadTask.h"
#include "DownloadTransfer.h"
#include "BTTrackerRequest.h"
#include "FileExecutor.h"
#include "FragmentedFile.h"
#include "Library.h"
#include "LibraryBuilder.h"
#include "LibraryHistory.h"
#include "Network.h"
#include "SharedFile.h"
#include "Statistics.h"
#include "Transfers.h"
#include "Uploads.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename


//////////////////////////////////////////////////////////////////////
// CDownload construction

CDownload::CDownload()
	: m_nSerID		( Downloads.GetFreeSID() )
	, m_bExpanded	( Settings.Downloads.AutoExpand )
	, m_bSelected	( FALSE )
	, m_tCompleted	( 0 )
	, m_nRunCookie	( 0 )
	, m_nGroupCookie( 0 )

	, m_bTempPaused	( FALSE )
	, m_bPaused		( FALSE )
	, m_bBoosted	( FALSE )
	, m_bClearing	( FALSE )
	, m_bDownloading( false )
	, m_bComplete	( false )
	, m_bShared		( Settings.Uploads.SharePartials )
	, m_tSaved		( 0 )
	, m_tBegan		( 0 )
{
}

CDownload::~CDownload()
{
	AbortTask();
	DownloadGroups.Unlink( this );
}

//////////////////////////////////////////////////////////////////////
// CDownload control : pause

void CDownload::Pause(BOOL bRealPause)
{
	if ( m_bPaused || m_bComplete && ! IsSeeding() )
		return;

	theApp.Message( MSG_NOTICE, IDS_DOWNLOAD_PAUSED, GetDisplayName() );

	StopTrying();
	m_bTempPaused = TRUE;

	if ( bRealPause )
		m_bPaused = TRUE;

	SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownload control : resume

void CDownload::Resume()
{
	if ( IsCompleted() && ! IsSeeding() )
		return;

	if ( ! IsPaused() )
	{
		StartTrying();
		return;
	}

	theApp.Message( MSG_NOTICE, IDS_DOWNLOAD_RESUMED, (LPCTSTR)GetDisplayName() );

	if ( IsFileOpen() )
	{
		for ( POSITION posSource = GetIterator() ; posSource ; )
		{
			CDownloadSource* pSource = GetNext( posSource );

			pSource->OnResume();
		}
	}

	m_bPaused				= FALSE;
	m_bTempPaused			= FALSE;
	m_bTorrentTrackerError	= FALSE;
	m_tReceived				= GetTickCount();

	// Try again
	ClearFileError();

	// Obsolete: Remove this
	//if ( IsTorrent() )
	//{
	//	if ( Downloads.GetTryingCount( true ) < Settings.BitTorrent.DownloadTorrents )
	//		SetStartTimer();
	//}
	//else
	//{
	//	if ( Downloads.GetTryingCount() < ( Settings.Downloads.MaxFiles + Settings.Downloads.MaxFileSearches ) )
	//		SetStartTimer();
	//}

	SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownload control : remove

// ToDo: Stop freezing GUI during many cleared downloads/seeds

void CDownload::Remove()
{
	AbortTask();
	StopTrying();
	CloseTorrent();
	CloseTransfers();

	if ( IsTrying() )
		Downloads.StopTrying( IsTorrent() );

	theApp.Message( MSG_NOTICE, IDS_DOWNLOAD_REMOVE, (LPCTSTR)GetDisplayName() );

	IsCompleted() ? CloseFile() : DeleteFile();

	DeletePreviews();

	if ( ! m_sPath.IsEmpty() )
	{
		DeleteFileEx( m_sPath + _T(".png"), FALSE, FALSE, TRUE );
		DeleteFileEx( m_sPath + _T(".sav"), FALSE, FALSE, TRUE );
		DeleteFileEx( m_sPath, FALSE, FALSE, TRUE );
		m_sPath.Empty();
	}

	Downloads.Remove( this );
}

//////////////////////////////////////////////////////////////////////
// CDownload control : boost toggle

void CDownload::Boost(BOOL bBoost)
{
	if ( ! IsFileOpen() || m_bBoosted == bBoost ) return;

	theApp.Message( MSG_NOTICE, IDS_DOWNLOAD_BOOST, (LPCTSTR)GetDisplayName() );

	for ( CDownloadTransfer* pTransfer = GetFirstTransfer() ; pTransfer ; pTransfer = pTransfer->m_pDlNext )
	{
		pTransfer->Boost( bBoost );
	}

	m_bBoosted = bBoost;
	SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownload control : sharing

void CDownload::Share(BOOL bShared)
{
	m_bShared = bShared;
	SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownload control : Stop trying

void CDownload::StopTrying()
{
	if ( ! IsTrying() || ( IsCompleted() && ! IsSeeding() ) )
		return;

	Downloads.StopTrying( IsTorrent() );

	m_tBegan = 0;
	m_bDownloading = false;

	// If m_bTorrentRequested = TRUE, sends Stop
	// CloseTorrent() additionally closes uploads
	if ( IsTorrent() )
		CloseTorrent();

	CloseTransfers();
	CloseFile();
	StopSearch();
	SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownload control : StartTrying

void CDownload::StartTrying()
{
	ASSERT( ! IsCompleted() || IsSeeding() );
	ASSERT( ! IsPaused() );
	if ( IsTrying() || IsPaused() || ( IsCompleted() && ! IsSeeding() ) )
		return;

	if ( ! Network.IsConnected() && ! Network.Connect( TRUE ) )
		return;

	Downloads.StartTrying( IsTorrent() );
	m_tBegan = GetTickCount();
}

//////////////////////////////////////////////////////////////////////
// CDownload control : GetStartTimer

DWORD CDownload::GetStartTimer() const
{
	return m_tBegan;
}

//////////////////////////////////////////////////////////////////////
// CDownload state checks

bool CDownload::IsStarted() const
{
	return GetVolumeComplete() > 0;
}

bool CDownload::IsPaused(bool bRealState /*= false*/) const
{
	if ( bRealState )
		return m_bPaused != 0;

	return m_bTempPaused != 0;
}

// Is the download receiving data?
bool CDownload::IsDownloading() const
{
	return m_bDownloading;
}

bool CDownload::IsCompleted() const
{
	return m_bComplete;
}

bool CDownload::IsBoosted() const
{
	return ( m_bBoosted != 0 );
}

// Is the download currently trying to download?
bool CDownload::IsTrying() const
{
	return ( m_tBegan != 0 );
}

bool CDownload::IsShared() const
{
	return m_bShared ||
		( Settings.Gnutella1.Enabled && m_oSHA1 ) ||
		( Settings.Gnutella2.Enabled && HasHash() ) ||
		( Settings.eDonkey.Enabled && m_oED2K ) ||
		( Settings.DC.Enabled && m_oTiger ) ||
		( Settings.BitTorrent.Enabled && IsTorrent() && ( IsSeeding() || IsStarted() ) );
}

int CDownload::GetClientStatus() const
{
	return IsCompleted() ? -1 : (int)GetEffectiveSourceCount();
}

CString CDownload::GetDownloadStatus() const
{
	CString strText;

	if ( m_bClearing )	// Briefly marked for removal
	{
		LoadString( strText, IDS_STATUS_CLEARING );
	}
	else if ( IsPaused() )
	{
		if ( GetFileError() == ERROR_SUCCESS || IsSeeding() )
			LoadString( strText, IDS_STATUS_PAUSED );
		else
			LoadString( strText, IsMoving() ? IDS_STATUS_CANTMOVE : IDS_STATUS_FILEERROR );
	}
	else if ( IsCompleted() )
	{
		if ( IsSeeding() )
			LoadString( strText, m_bTorrentTrackerError ? IDS_STATUS_TRACKERDOWN : IDS_STATUS_SEEDING );
		else
			LoadString( strText, IDS_STATUS_COMPLETED );
	}
	else if ( IsMoving() )
	{
		LoadString( strText, IDS_STATUS_MOVING );
	}
	else if ( IsStarted() && GetProgress() == 100.0f )
	{
		LoadString( strText, IDS_STATUS_VERIFYING );
	}
	else if ( ! IsTrying() )
	{
		LoadString( strText, IDS_STATUS_QUEUED );
	}
	else if ( IsDownloading() )
	{
		const DWORD nTime = GetTimeRemaining();

		if ( nTime == 0xFFFFFFFF )
		{
			LoadString( strText, IDS_STATUS_ACTIVE );	// IDS_STATUS_DOWNLOADING
		}
		else
		{
			if ( nTime > 86400 )
				strText.Format( _T("%u:%.2u:%.2u:%.2u"), nTime / 86400, ( nTime / 3600 ) % 24, ( nTime / 60 ) % 60, nTime % 60 );
			else
				strText.Format( _T("%u:%.2u:%.2u"), nTime / 3600, ( nTime / 60 ) % 60, nTime % 60 );
		}
	}
	else if ( GetEffectiveSourceCount() > 0 )
	{
		LoadString( strText, IDS_STATUS_PENDING );
	}
	else if ( IsTorrent() )
	{
		if ( GetTaskType() == dtaskAllocate )
			LoadString( strText, IDS_STATUS_CREATING );
		else if ( m_bTorrentTrackerError )
			LoadString( strText, IDS_STATUS_TRACKERDOWN );
		else
			LoadString( strText, IDS_STATUS_TORRENT );
	}
	else // Inactive
	{
		LoadString( strText, IDS_STATUS_QUEUED );
	}

	return strText;
}

CString CDownload::GetDownloadSources() const
{
	const int nTotalSources = GetSourceCount();
	const int nSources = GetEffectiveSourceCount();

	CString strText;
	if ( IsCompleted() )
	{
		if ( m_bVerify == TRI_TRUE )
			LoadString( strText, IDS_STATUS_VERIFIED );
		else if ( m_bVerify == TRI_FALSE )
			LoadString( strText, IDS_STATUS_UNVERIFIED );
	}
	else if ( nTotalSources == 0 )
	{
		LoadString( strText, IDS_STATUS_NOSOURCES );
	}
	else if ( nSources == nTotalSources )
	{
		CString strSource;
		LoadSourcesString( strSource, nSources );
		strText.Format( _T("(%i %s)"), nSources, (LPCTSTR)strSource );
	}
	else
	{
		CString strSource;
		LoadSourcesString( strSource, nTotalSources, true );
		strText.Format( _T("(%i/%i %s)"), nSources, nTotalSources, (LPCTSTR)strSource );
	}
	return strText;
}

//////////////////////////////////////////////////////////////////////
// CDownload run handler

void CDownload::OnRun()
{
	// Set the currently downloading state
	// (Used to optimize display in Ctrl/Wnd functions)
	m_bDownloading = false;

	const DWORD tNow = GetTickCount();

	if ( ! IsPaused() )
	{
		if ( GetFileError() != ERROR_SUCCESS )
		{
			// File or disk errors
			Pause( FALSE );
		}
		else if ( IsMoving() )
		{
			if ( ! IsCompleted() && ! IsTasking() )
				OnDownloaded();
		}
		else if ( IsTrying() || IsSeeding() )
		{
			// Dead Download Check: if download appears dead, give up and allow another to start.
			// Incomplete, and trying for at least 3 hours:
			if ( ! IsCompleted() && tNow > GetStartTimer() + ( 3 * 60 * 60 * 1000 ) )
			{
				const DWORD tHoursToTry = min( ( GetEffectiveSourceCount() + 49u ) / 50u, 9lu ) + Settings.Downloads.StarveGiveUp;

				// No new data for 5-14 hours
				if ( tNow > m_tReceived + ( tHoursToTry * 60 * 60 * 1000 ) )
				{
					if ( IsTorrent() )
					{
						// Are there other torrents that should start?
						if ( Downloads.GetTryingCount( TRUE ) >= Settings.BitTorrent.DownloadTorrents )
						{
							StopTrying();		// Give up for now, try again later
							return;
						}
					}
					else	// Regular download
					{
						// Are there other downloads that should try?
						if ( Downloads.GetTryingCount() >= ( Settings.Downloads.MaxFiles + Settings.Downloads.MaxFileSearches ) )
						{
							StopTrying();		// Give up for now, try again later
							return;
						}
					}
				}
			}
			// End Dead Download Check

			// Run the download
			if ( ! IsTorrent() || RunTorrent( tNow ) )
			{
				RunSearch( tNow );
				RunValidation();

				if ( IsSeeding() )
				{
					// Mark as collapsed to get correct heights when dragging files
					if ( ! Settings.General.DebugBTSources && m_bExpanded )
						m_bExpanded = FALSE;
				}
				else // if ( ! IsMoving() )
				{
					if ( IsComplete() && IsFileOpen() )
					{
						if ( IsFullyVerified() )
							OnDownloaded();
					}
					else if ( CheckTorrentRatio() )
					{
						if ( ! Network.IsConnected() )
						{
							StopTrying();
							return;
						}

						StartTransfersIfNeeded( tNow );
					}
				}
			} // if ( RunTorrent( tNow ) )

			// Calculate current downloading state
			if ( HasActiveTransfers() )
				m_bDownloading = true;
		}
		else if ( ! IsCompleted() && m_bVerify != TRI_TRUE )
		{
			// This is pending download
		//	// If this download isn't trying to download, see if it can try
		//	if ( IsDownloading() )
		//		SetStartTimer();	// This download was probably started by a push/etc

			if ( Network.IsConnected() )
			{
				// Have extra regular downloads 'trying' so when a new slot is ready,
				// a download has sources and is ready to go.
				if ( Downloads.GetTryingCount() < ( Settings.Downloads.MaxFiles + Settings.Downloads.MaxFileSearches ) )
				{
					if ( ! IsTorrent() ||
						Downloads.GetTryingCount( true ) < Settings.BitTorrent.DownloadTorrents )
					{
						// Torrents only try when 'ready to go'. (Reduce tracker load)
						Resume();
					}
				}
			}
		//	else
		//		ASSERT( ! IsTrying() );
		}
	}

	// Don't save Downloads with many sources too often, since it's slow
	if ( tNow >= m_tSaved + ( GetCount() > 20 ? 5 * Settings.Downloads.SaveInterval : Settings.Downloads.SaveInterval ) )
	{
		if ( IsModified() )
		{
			FlushFile();
			if ( Save() )
				m_tSaved = tNow;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CDownload download complete handler

void CDownload::OnDownloaded()
{
	ASSERT( m_bComplete == false );

	theApp.Message( MSG_NOTICE, IDS_DOWNLOAD_COMPLETED, GetDisplayName() );
	m_tCompleted = GetTickCount();
	m_bDownloading = false;

	CloseTransfers();

	// AppendMetadata();

	if ( GetTaskType() == dtaskMergeFile || GetTaskType() == dtaskPreviewRequest )
		AbortTask();

	LibraryBuilder.m_bBusy = true;	// ToDo: Remove this?

	CDownloadTask::Copy( this );

	Statistics.Current.Downloads.Files++;

	SetModified();
}

//////////////////////////////////////////////////////////////////////
// CDownload task completion

void CDownload::OnTaskComplete(const CDownloadTask* pTask)
{
	SetTask( NULL );

	// Check if task was aborted
	if ( pTask->WasAborted() )
		return;

	if ( pTask->GetTaskType() == dtaskPreviewRequest )
	{
		OnPreviewRequestComplete( pTask );
	}
	else if ( pTask->GetTaskType() == dtaskCopy )
	{
		LibraryBuilder.m_bBusy = false;

		if ( ! pTask->HasSucceeded() )
			SetFileError( pTask->GetFileError() );
		else
			OnMoved();
	}
}

//////////////////////////////////////////////////////////////////////
// CDownload moved handler

void CDownload::OnMoved()
{
	// Just completed torrent
	if ( IsTorrent() && IsFullyVerified() )
	{
		// Set FALSE to prevent sending 'stop' announce to tracker
		m_bTorrentRequested = FALSE;
		StopTrying();

		// Send 'completed' announce to tracker
		SendCompleted();

		// This torrent is now seeding
		m_bSeeding = TRUE;
		m_bVerify = TRI_TRUE;
		m_bTorrentStarted = TRUE;
		m_bTorrentRequested = TRUE;
	}
	else if ( IsTorrent() )	// Something wrong, since we moved the torrent ?
	{
		// Explicitly set flag to send stop announce to tracker
		m_bTorrentRequested = TRUE;
		StopTrying();
	}
	else	// Not torrent?
	{
		StopTrying();
	}

	ClearSources();

	ASSERT( ! m_sPath.IsEmpty() );
	DeleteFileEx( m_sPath + _T(".png"), FALSE, FALSE, TRUE );
	DeleteFileEx( m_sPath + _T(".sav"), FALSE, FALSE, TRUE );
	DeleteFileEx( m_sPath, FALSE, FALSE, TRUE );
	m_sPath.Empty();

	// Download finalized, tracker notified, set flags that we completed
	m_bComplete  = true;
	m_tCompleted = GetTickCount();
}

//////////////////////////////////////////////////////////////////////
// CDownload load and save

BOOL CDownload::Load(LPCTSTR pszName)
{
	ASSERT( m_sPath.IsEmpty() );
	m_sPath = pszName;
	GetSafePath( m_sPath );

	BOOL bSuccess = FALSE;
	CFile pFile;
	if ( pFile.Open( m_sPath, CFile::modeRead ) )	// .pd file
	{
		TRY
		{
			CArchive ar( &pFile, CArchive::load, 32768 );	// 32 KB buffer
			Serialize( ar, 0 );
			bSuccess = TRUE;
		}
		CATCH( CFileException, pException )
		{
			if ( pException->m_cause == CFileException::fileNotFound )
				return FALSE;	// Subfile missing
		}
		AND_CATCH_ALL( pException )
		{
			theApp.Message( MSG_ERROR, IDS_DOWNLOAD_FILE_OPEN_ERROR, m_sPath );
		}
		END_CATCH_ALL

		pFile.Close();
	}

	if ( ! bSuccess && pFile.Open( m_sPath + _T(".sav"), CFile::modeRead ) )
	{
		TRY
		{
			CArchive ar( &pFile, CArchive::load, 32768 );	// 32 KB buffer
			Serialize( ar, 0 );
			bSuccess = TRUE;
		}
		CATCH( CFileException, pException )
		{
			if ( pException->m_cause == CFileException::fileNotFound )
				return FALSE;	// Subfile missing
		}
		AND_CATCH_ALL( pException )
		{
			theApp.Message( MSG_ERROR, IDS_DOWNLOAD_FILE_OPEN_ERROR, m_sPath + _T(".sav") );
		}
		END_CATCH_ALL

		pFile.Close();

		if ( bSuccess )
			Save();
	}

	m_bGotPreview = GetFileAttributes( m_sPath + _T(".png") ) != INVALID_FILE_ATTRIBUTES;
	m_nSaveCookie = m_nCookie;

	return bSuccess;
}

BOOL CDownload::Save(BOOL bFlush)
{
	CSingleLock pTransfersLock( &Transfers.m_pSection, TRUE );

	if ( ! m_sPath.IsEmpty() && m_sPath.Right( 3 ) != L".pd" )		// Remove imported .sd files, .pd will be created below
		DeleteFileEx( LPCTSTR( m_sPath ), FALSE, FALSE, FALSE );

	if ( m_sPath.IsEmpty() || m_sPath.Right( 3 ) != L".pd" )		// From incomplete folder or .sd imports
		m_sPath = SafePath( Settings.Downloads.IncompletePath + _T("\\") + GetFilename() + _T(".pd") );

	// Escape Windows path length limit with \\?\ if needed?  (SafePath() above)
	//const CString strPath = ( m_sPath.GetLength() > ( MAX_PATH - 4 ) ) ? ( _T("\\\\?\\")  + m_sPath ) : m_sPath;

	m_nSaveCookie = m_nCookie;
	m_tSaved = GetTickCount();

	if ( m_bComplete && ! m_bSeeding )
		return TRUE;

	if ( m_bSeeding && ! Settings.BitTorrent.AutoSeed )
		return TRUE;

	DeleteFileEx( LPCTSTR( m_sPath + _T(".sav") ), FALSE, FALSE, FALSE );

	CFile pFile;
	if ( ! pFile.Open( LPCTSTR( m_sPath + _T(".sav") ),		// Create temp .pd.sav
		CFile::modeReadWrite|CFile::modeCreate|CFile::osWriteThrough ) )
		return FALSE;

	CHAR szID[3] = { 0, 0, 0 };
	try
	{
		CArchive ar( &pFile, CArchive::store, 32768 );	// 32 KB buffer
		try
		{
			Serialize( ar, 0 );
			ar.Close();
		}
		catch ( CException* pException )
		{
			ar.Abort();
			pFile.Abort();
			theApp.Message( MSG_ERROR, _T("Serialize Error: %s"), GetFilename() );
			pException->Delete();
			return FALSE;
		}

		if ( Settings.Downloads.FlushPD || bFlush ) 	// Always true (Why advanced setting?)
			pFile.Flush();

		pFile.SeekToBegin();
		pFile.Read( szID, 3 );
		pFile.Close();
	}
	catch ( CException* pException )
	{
		pFile.Abort();
		pException->Delete();
		return FALSE;
	}

	// .pd file should now start with characters PD:  (Legacy SDL in Shareaza .sd)
	if ( szID[0] == 'P' && szID[1] == 'D' && szID[2] == ':' )
	{
		// Rename temp .pd.sav file to .pd
		if ( ::MoveFileEx( LPCTSTR( m_sPath + _T(".sav") ), LPCTSTR( m_sPath ),
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH ) )
			return TRUE;	// Done

		ASSERT( FALSE );
	}
	else // Bad file header?
	{
		DeleteFileEx( LPCTSTR( m_sPath + _T(".sav") ), FALSE, FALSE, FALSE );
		ASSERT( FALSE );
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CDownload serialize

void CDownload::Serialize(CArchive& ar, int nVersion)	// DOWNLOAD_SER_VERSION
{
	ASSERT( ! m_bComplete || m_bSeeding );

	if ( m_bSeeding && ! Settings.BitTorrent.AutoSeed )
		return;

	if ( nVersion < 2 ) // NULL
	{
		nVersion = DOWNLOAD_SER_VERSION;

		if ( ar.IsStoring() )
		{
			ar.Write( "PD:", 3 );
			ar << nVersion;
		}
		else // Loading ?
		{
			CHAR szID[3];
			ReadArchive( ar, szID, 3 );
			if ( strncmp( szID, "PD:", 3 ) != 0 )
				if ( strncmp( szID, "SDL", 3 ) != 0 )	// Shareaza import or pre r60?
					AfxThrowUserException();
			ar >> nVersion;
			if ( nVersion < 1 || nVersion > DOWNLOAD_SER_VERSION )
				AfxThrowUserException();
		}
	}
//	else if ( nVersion < 11 && ar.IsLoading() ) 		// Very old Shareaza, for reference
//	{
//		SerializeOld( ar, nVersion );
//		return;
//	}

	CDownloadWithExtras::Serialize( ar, nVersion );		// Any Previews/Reviews

	if ( ar.IsStoring() )
	{
		ar << m_bExpanded;
		ar << m_bPaused;
		ar << m_bBoosted;
		ar << m_bShared;
		ar << m_nSerID;
	}
	else // Loading
	{
		ar >> m_bExpanded;
		ar >> m_bPaused;
		m_bTempPaused = m_bPaused;
		ar >> m_bBoosted;
		ar >> m_bShared;
		ar >> m_nSerID;

		DownloadGroups.Link( this );

	//	if ( nVersion == 32 )	// ShareazaPlus = 38?
	//	{
	//		// Compatibility for CB Branch.
	//		if ( ! ar.IsBufferEmpty() )
	//		{
	//			CString sSearchKeyword;
	//			ar >> sSearchKeyword;
	//		}
	//	}
	}
}

//void CDownload::SerializeOld(CArchive& ar, int nVersion)	// DOWNLOAD_SER_VERSION < 11
//{
//	// nVersion < 11 (Very old Shareaza)
//
//	ASSERT( ar.IsLoading() );
//
//	ar >> m_sPath;
//	m_sPath += _T(".sd");
//	ar >> m_sName;
//
//	DWORD nSize;
//	ar >> nSize;
//	m_nSize = nSize;
//
//	Hashes::Sha1Hash oSHA1;
//	SerializeIn( ar, oSHA1, nVersion );
//	m_oSHA1 = oSHA1;
//	m_bSHA1Trusted = true;
//
//	ar >> m_bPaused;
//	ar >> m_bExpanded;
//	if ( nVersion >= 6 ) ar >> m_bBoosted;
//
//	CDownloadWithFile::SerializeFile( ar, nVersion );
//
//	for ( DWORD_PTR nSources = ar.ReadCount() ; nSources ; nSources-- )
//	{
//		CDownloadSource* pSource = new CDownloadSource( this );
//		pSource->Serialize( ar, nVersion );
//		AddSourceInternal( pSource );
//	}
//
//	if ( nVersion >= 3 && ar.ReadCount() )
//	{
//		auto_ptr< CXMLElement > pXML( new CXMLElement() );
//		pXML->Serialize( ar );
//		MergeMetadata( pXML.get() );
//	}
//}

void CDownload::ForceComplete()
{
	m_bPaused = FALSE;
	m_bTempPaused = FALSE;
	m_bVerify = TRI_FALSE;
	MakeComplete();
	StopTrying();
	Share( FALSE );
	OnDownloaded();
}

BOOL CDownload::Launch(int nIndex, CSingleLock* pLock, BOOL bForceOriginal)
{
	if ( nIndex < 0 && IsCompleted() && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0 )
	{
		// Shift key opens file externally, folder on multifile torrents
		const CString strPath = IsMultiFileTorrent() ?
			GetPath( 0 ).Left( GetPath( 0 ).ReverseFind( '\\' ) + 1 ) : GetPath( 0 );
		ShellExecute( AfxGetMainWnd()->GetSafeHwnd(),
			_T("open"), strPath, NULL, NULL, SW_SHOWNORMAL );
		return TRUE;
	}

	if ( nIndex < 0 )
		nIndex = SelectFile( pLock );
	if ( nIndex < 0 || ! Downloads.Check( this ) )
		return FALSE;

	BOOL bResult = TRUE;
	const CString strPath = GetPath( nIndex );
	const CString strName = GetName( nIndex );
	const CString strExt = strName.Mid( strName.ReverseFind( '.' ) );

	if ( IsCompleted() )
	{
		// Run complete file

		if ( m_bVerify == TRI_FALSE )
		{
			if ( pLock ) pLock->Unlock();

			CString strMessage;
			strMessage.Format( LoadString( IDS_LIBRARY_VERIFY_FAIL ), (LPCTSTR)strName );
			INT_PTR nResponse = AfxMessageBox( strMessage, MB_ICONEXCLAMATION | MB_YESNOCANCEL | MB_DEFBUTTON2 );

			if ( pLock ) pLock->Lock();

			if ( nResponse == IDCANCEL )
				return FALSE;
			if ( nResponse == IDNO )
				return TRUE;
		}

		if ( pLock ) pLock->Unlock();

		bResult = CFileExecutor::Execute( strPath, strExt );

		if ( pLock ) pLock->Lock();
	}
	else if ( CanPreview( nIndex ) )
	{
		if ( ! bForceOriginal )
		{
			// Previewing...
			if ( pLock ) pLock->Unlock();

			TRISTATE bSafe = CFileExecutor::IsSafeExecute( strExt, strName );

			if ( pLock ) pLock->Lock();

			if ( bSafe == TRI_UNKNOWN )
				return FALSE;
			if ( bSafe == TRI_FALSE )
				return TRUE;

			if ( ! Downloads.Check( this ) )
				return TRUE;

			if ( PreviewFile( nIndex, pLock ) )
				return TRUE;
		}

		// Run file as is
		if ( pLock ) pLock->Unlock();

		bResult = CFileExecutor::Execute( strPath, strExt );

		if ( pLock ) pLock->Lock();
	}

	return bResult;
}

BOOL CDownload::Enqueue(int nIndex, CSingleLock* pLock)
{
	if ( nIndex < 0 )
		nIndex = SelectFile( pLock );
	if ( nIndex < 0 || ! Downloads.Check( this ) )
		return TRUE;

	BOOL bResult = TRUE;
	if ( IsStarted() )
	{
		if ( pLock ) pLock->Unlock();

		const CString strPath = GetPath( nIndex );
		const CString strName = GetName( nIndex );
		const CString strExt = strName.Mid( strName.ReverseFind( '.' ) );

		bResult = CFileExecutor::Enqueue( strPath, strExt );

		if ( pLock ) pLock->Lock();
	}

	return bResult;
}
