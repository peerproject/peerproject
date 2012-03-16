//
// FileExecutor.cpp
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
#include "FileExecutor.h"
#include "Library.h"
#include "Plugins.h"
#include "Security.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "ShellIcons.h"
#include "XML.h"

#include "WindowManager.h"
#include "WndMain.h"
#include "WndMedia.h"
#include "WndLibrary.h"
#include "DlgTorrentSeed.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

//#define TOO_MANY_FILES_LIMIT	20	// Settings.Library.ManyFilesCount

// Some known media players
static const struct
{
	LPCTSTR szPlayer;
	LPCTSTR	szEnqueue;
}
KnownPlayers[] =
{
	// Windows Media Player
	{ _T("wmplayer.exe"),	_T("/SHELLHLP_V9 Enqueue \"%s\"") },
	// Media Player Classic
	{ _T("mplayerc.exe"),	_T("\"%s\" /add") },
	// MediaPlayerClassic HomeCinema
	{ _T("mpc-hc.exe"), 	_T("\"%s\" /add") },
	// MediaPlayerClassic HomeCinema 64
	{ _T("mpc-hc64.exe"),	_T("\"%s\" /add") },
	// MPlayer
	{ _T("mplayer.exe"),	_T("-enqueue %s") },
	// SMplayer (GUI for MPlayer)
	{ _T("smplayer.exe"),	_T("-add-to-playlist \"%s\"") },
	// VideoLAN
	{ _T("vlc.exe"),		_T("--one-instance --playlist-enqueue \"%s\"") },
	// WinAmp
	{ _T("winamp.exe"),		_T("/ADD \"%s\"") },
	// Light Alloy
	{ _T("la.exe"),			_T("/ADD \"%s\"") },
	// KMPlayer
	{ _T("kmplayer.exe"),	_T("/ADD \"%s\"") },
	// BSPlayer
	{ _T("bsplayer.exe"),	_T("\"%s\" -ADD") },
	// Zoom Player
	{ _T("zplayer.exe"),	_T("\"/Queue:%s\"") },
	// (end)
	{ NULL, NULL }
};

int PathGetArgsIndex(const CString& str)
{
	if ( str.GetAt( 0 ) == _T('\"') )
	{
		// "command" args
		int quote = str.Find( _T('\"'), 1 );
		if ( quote == -1 )
			return -1;	// No closing quote

		return quote + 1;
	}
	// command args
	int i = -1;
	for ( ;; )
	{
		const int slash = str.Find( _T('\\'), i + 1 );
		if ( slash == -1 || GetFileAttributes( str.Mid( 0, slash + 1 ) ) == INVALID_FILE_ATTRIBUTES )
			return str.Find( _T(' '), i + 1 );

		i = slash;
	}
}

CMediaWnd* CFileExecutor::GetMediaWindow(BOOL bFocus)
{
	CMainWnd* pMainWnd = theApp.SafeMainWnd();
	if ( pMainWnd == NULL ) return NULL;
	if ( pMainWnd->IsKindOf( RUNTIME_CLASS(CMainWnd) ) == FALSE ) return NULL;
	return (CMediaWnd*)pMainWnd->m_pWindows.Open( RUNTIME_CLASS(CMediaWnd), FALSE, bFocus );
}

CLibraryWnd* CFileExecutor::GetLibraryWindow()
{
	CMainWnd* pMainWnd = theApp.SafeMainWnd();
	if ( pMainWnd == NULL ) return NULL;
	if ( pMainWnd->IsKindOf( RUNTIME_CLASS(CMainWnd) ) == FALSE ) return NULL;
	return (CLibraryWnd*)pMainWnd->m_pWindows.Open( RUNTIME_CLASS(CLibraryWnd), FALSE, TRUE );
}

void CFileExecutor::DetectFileType(LPCTSTR pszFile, LPCTSTR szType, bool& bVideo, bool& bAudio, bool& bImage)
{
	if ( GetFileAttributes( pszFile ) & FILE_ATTRIBUTE_DIRECTORY )
		return;

	if ( CSchemaPtr pSchema = SchemaCache.GuessByFilename( szType ) )
	{
		if ( pSchema->CheckURI( CSchema::uriAudio ) )
			bAudio = true;
		else if ( pSchema->CheckURI( CSchema::uriVideo ) )
			bVideo = true;
		else if ( pSchema->CheckURI( CSchema::uriImage ) )
			bImage = true;
	}

	// Detect type by MIME "Content Type"
	if ( ! bAudio && ! bVideo && ! bImage )
	{
		CString strMime;
		ShellIcons.Lookup( szType, NULL, NULL, NULL, &strMime );
		if ( ! strMime.IsEmpty() )
		{
			CString strMimeMajor = strMime.SpanExcluding( _T("/") );
			if ( strMimeMajor == _T("video") )
				bVideo = true;
			else if ( strMimeMajor == _T("audio") )
				bAudio = true;
			else if ( strMimeMajor == _T("image") )
				bImage = true;
			else if ( strMime == _T("application/x-shockwave-flash") )
				bVideo = true;
		}
	}

	// Detect type by file schema
	if ( ! bAudio && ! bVideo && ! bImage )
	{
		CQuickLock oLock( Library.m_pSection );
		if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( pszFile ) )
		{
			if ( pFile->IsSchemaURI( CSchema::uriAudio ) )
				bAudio = true;
			else if ( pFile->IsSchemaURI( CSchema::uriVideo ) )
				bVideo = true;
			else if ( pFile->IsSchemaURI( CSchema::uriImage ) )
				bImage = true;
		}
	}
}

CString CFileExecutor::GetCustomPlayer()
{
	for ( string_set::const_iterator i = Settings.MediaPlayer.ServicePath.begin() ;
		i != Settings.MediaPlayer.ServicePath.end() ; ++i )
	{
		CString strPlayer = *i;
		if ( strPlayer.ReverseFind( _T('*') ) == -1 )
			continue;

		// Has Asterisk at end to indicate selected player
		strPlayer.Remove( _T('*') );
		return strPlayer;
	}

	return CString();
}

TRISTATE CFileExecutor::IsSafeExecute(LPCTSTR szExt, LPCTSTR szFile)
{
	const BOOL bSafe = ! szExt || ! *szExt ||
		IsIn( Settings.Library.SafeExecute, szExt + 1 ) ||
		( theApp.m_pfnAssocIsDangerous && ! theApp.m_pfnAssocIsDangerous( szExt ) );

	if ( ! bSafe && szFile )
	{
		CString strPrompt;
		TCHAR szPrettyPath[ 60 ];
		PathCompactPathEx( szPrettyPath, szFile, _countof( szPrettyPath ) - 1, 0 );
		strPrompt.Format( LoadString( IDS_LIBRARY_CONFIRM_EXECUTE ), szPrettyPath );
		switch ( AfxMessageBox( strPrompt,
			MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON2 ) )
		{
		case IDYES:
			return TRI_TRUE;	// Run it
		case IDNO:
			return TRI_FALSE;	// Skip it
		default:
			return TRI_UNKNOWN;	// Cancel file operation
		}
	}

	return bSafe ? TRI_TRUE : TRI_FALSE;
}

TRISTATE CFileExecutor::IsVerified(LPCTSTR szFile)
{
	BOOL bInsecure = FALSE;
	{
		CQuickLock pLock( Library.m_pSection );
		if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( szFile ) )
		{
			bInsecure = pFile->m_bVerify == TRI_FALSE &&
				( ! Settings.Search.AdultFilter || ! AdultFilter.IsChildPornography( szFile ) );
		}
	}

	if ( ! bInsecure )
		return TRI_TRUE;	// Run it

	CString strMessage;
	strMessage.Format( LoadString( IDS_LIBRARY_VERIFY_FAIL ), szFile );
	INT_PTR nResponse = AfxMessageBox( strMessage,
		MB_ICONEXCLAMATION|MB_YESNOCANCEL|MB_DEFBUTTON2 );
	if ( nResponse == IDCANCEL )
		return TRI_UNKNOWN;	// Cancel file operation
	if ( nResponse == IDNO )
		return TRI_FALSE;	// Skip it

	nResponse = AfxMessageBox( LoadString( IDS_LIBRARY_VERIFY_FIX ),
		MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2 );
	if ( nResponse == IDCANCEL )
		return TRI_UNKNOWN;		// Cancel file operation
	if ( nResponse == IDYES )
	{
		// Reset failed verification flag
		CQuickLock pLock( Library.m_pSection );
		if ( CLibraryFile* pFile = LibraryMaps.LookupFileByPath( szFile ) )
		{
			pFile->m_bVerify = TRI_UNKNOWN;
			Library.Update();
		}
	}

	// Run it
	return TRI_TRUE;
}

BOOL CFileExecutor::Execute(LPCTSTR pszFile, LPCTSTR pszExt)
{
	CWaitCursor pCursor;

	TRISTATE bVerified = IsVerified( pszFile );
	if ( bVerified == TRI_UNKNOWN )
		return FALSE;		// Cancel operation
	if ( bVerified == TRI_FALSE )
		return TRUE;		// Skip file

	CString strType;
	if ( ! ( GetFileAttributes( pszFile ) & FILE_ATTRIBUTE_DIRECTORY ) )
		strType = CString( PathFindExtension( pszFile ) ).MakeLower();

	// Handle collections
	if ( strType == _T(".co") ||
		 strType == _T(".collection") ||
		 strType == _T(".emulecollection") )
	{
		if ( CLibraryWnd* pWnd = GetLibraryWindow() )
			pWnd->OnCollection( pszFile );
		return TRUE;		// Skip file
	}

	// Internal list exceptions
	if ( strType == L".met" ||
		( strType == L".bz2" && CString( PathFindFileName( pszFile ) ).Find( L"hublist.xml.bz2" ) >= 0 ) )
	{
		if ( theApp.OpenImport( pszFile ) )
			return TRUE;	// Skip file
	}

	// Open known file types (links)
	if ( theApp.Open( pszFile ) )
		return TRUE;		// Skip file

	// Prepare partials
	bool bPartial = false;
	if ( strType == _T(".partial") && pszExt )
	{
		bPartial = true;
		strType = pszExt;
		strType.MakeLower();
	}

	// Detect type
	bool bVideo = false;
	bool bAudio = false;
	bool bImage = false;
	DetectFileType( pszFile, strType, bVideo, bAudio, bImage );

	// Detect dangerous files
	if ( ! ( bAudio || bVideo || bImage ) )
	{
		TRISTATE bSafe = IsSafeExecute( strType, pszFile );
		if ( bSafe == TRI_UNKNOWN )
			return FALSE;	// Cancel operation
		if ( bSafe == TRI_FALSE )
			return TRUE;	// Skip file
	}

	// Handle video and audio files by internal player
	const bool bShiftKey = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;
	if ( ! bShiftKey && ( bVideo || bAudio ) && Settings.MediaPlayer.EnablePlay && ! strType.IsEmpty() &&
		IsIn( Settings.MediaPlayer.FileTypes, (LPCTSTR)strType + 1 ) )
	{
		if ( CMediaWnd* pWnd = GetMediaWindow( ! bAudio ) )
		{
			pWnd->PlayFile( pszFile );
			return TRUE;
		}
	}

	// Handle video and audio files by external player
	CString strCustomPlayer = GetCustomPlayer();
	if ( ! bShiftKey && ( bVideo || bAudio ) && ! strCustomPlayer.IsEmpty() )
	{
		// Prepare file path for execution
		if ( Settings.MediaPlayer.ShortPaths )
		{
			TCHAR pszShortPath[ MAX_PATH ];
			if ( GetShortPathName( pszFile, pszShortPath, MAX_PATH ) )
				pszFile = pszShortPath;
		}

		HINSTANCE hResult = ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), _T("open"),
			strCustomPlayer, CString( _T('\"') ) + pszFile + _T('\"'), NULL, SW_SHOWNORMAL );
		if ( hResult > (HINSTANCE)32 )
			return TRUE;
	}

	// Handle all by plugins
	if ( ! bShiftKey )
	{
		if ( Plugins.OnExecuteFile( pszFile, bImage ) )
			return TRUE;
	}

	// ToDo: Doesn't work with partial files

	HINSTANCE hResult = ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), NULL,
		pszFile, NULL, NULL, SW_SHOWNORMAL );
	if ( hResult > (HINSTANCE)32 )
		return TRUE;

	return FALSE;
}

BOOL CFileExecutor::Execute(const CStringList& pList)
{
	if ( pList.GetCount() > Settings.Library.ManyFilesCount )	// TOO_MANY_FILES_LIMIT
	{
		CString sMessage;
		sMessage.Format( LoadString( IDS_TOO_MANY_FILES ), pList.GetCount() );
		if ( MsgBox( sMessage, MB_ICONQUESTION | MB_YESNO, 0,
			&Settings.Library.ManyFilesWarning ) != IDYES )
		{
			return FALSE;
		}
	}

	for ( POSITION pos = pList.GetHeadPosition() ; pos ; )
	{
		if ( ! CFileExecutor::Execute( pList.GetNext( pos ) ) )
			return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFileExecutor enqueue

BOOL CFileExecutor::Enqueue(LPCTSTR pszFile, LPCTSTR pszExt)
{
	CWaitCursor pCursor;

	TRISTATE bVerified = IsVerified( pszFile );
	if ( bVerified == TRI_UNKNOWN )
		return FALSE;	// Cancel operation
	if ( bVerified == TRI_FALSE )
		return TRUE;	// Skip file

	CString strType;
	if ( ! ( GetFileAttributes( pszFile ) & FILE_ATTRIBUTE_DIRECTORY ) )
		strType = CString( PathFindExtension( pszFile ) ).MakeLower();

	// Prepare partials
	if ( strType == _T(".partial") && pszExt )
	{
		strType = pszExt;
		strType.MakeLower();
	}

	// Detect type
	bool bVideo = false;
	bool bAudio = false;
	bool bImage = false;
	DetectFileType( pszFile, strType, bVideo, bAudio, bImage );

	// Detect dangerous files
	if ( ! ( bAudio || bVideo || bImage ) )
	{
		TRISTATE bSafe = IsSafeExecute( strType, pszFile );
		if ( bSafe == TRI_UNKNOWN )
			return FALSE;	// Cancel
		if ( bSafe == TRI_FALSE )
			return TRUE;	// Skip file
	}

	// Handle video and audio files by internal player
	const bool bShiftKey = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;
	if ( ! bShiftKey && ( bVideo || bAudio ) && Settings.MediaPlayer.EnableEnqueue &&
		strType.GetLength() > 1 &&
		IsIn( Settings.MediaPlayer.FileTypes, (LPCTSTR)strType + 1 ) )
	{
		if ( CMediaWnd* pWnd = GetMediaWindow( FALSE ) )
		{
			pWnd->EnqueueFile( pszFile );
			return TRUE;
		}
	}

	// Handle all by plugins
	if ( ! bShiftKey && Plugins.OnEnqueueFile( pszFile ) )
		return TRUE;

	// Delay between first and second in row runs
	static DWORD nRunCount = 0;
	static DWORD nLastRun = 0;
	if ( GetTickCount() - nLastRun > 2000 )
		nRunCount = 0;
	if ( ++nRunCount == 2 )
		Sleep( 2000 );
	nLastRun = GetTickCount();

	// Prepare short path
	CString strFile = pszFile;
	if ( Settings.MediaPlayer.ShortPaths )
	{
		TCHAR pszShortPath[ MAX_PATH ];
		if ( GetShortPathName( pszFile, pszShortPath, MAX_PATH ) )
			strFile = pszShortPath;
	}

	// Handle video and audio files by external player
	CString strCustomPlayer = GetCustomPlayer();
	if ( ! bShiftKey && ( bVideo || bAudio ) && ! strCustomPlayer.IsEmpty() )
	{
		// Try Shell "enqueue" verb
		CString strCommand, strParam;
		DWORD nBufferSize = MAX_PATH;
		HRESULT hr = AssocQueryString( ASSOCF_OPEN_BYEXENAME, ASSOCSTR_COMMAND,
			strCustomPlayer, _T("enqueue"),
			strCommand.GetBuffer( MAX_PATH ), &nBufferSize );
		strCommand.ReleaseBuffer();
		int nPos = PathGetArgsIndex( strCommand );
		if ( nPos != -1 )
		{
			strParam = strCommand.Mid( nPos ).Trim();
			strCommand = strCommand.Left( nPos );
		}
		strCommand = strCommand.Trim( _T("\" ") );
		if ( hr == S_OK )
		{
			int nFind = strParam.Find( _T("%1") );
			if ( nFind != -1 )
			{
				strParam.Replace( _T("%1"), strFile );
				HINSTANCE hResult = ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), NULL,
					strCommand, strParam, NULL, SW_SHOWNORMAL );
				if ( hResult > (HINSTANCE)32 )
					return TRUE;
			}
		}

		// Try to create "enqueue" verb from default verb for known players
		CString strExecutable = PathFindFileName( strCustomPlayer );
		for ( int i = 0 ; KnownPlayers[ i ].szPlayer ; ++i )
		{
			if ( strExecutable.CompareNoCase( KnownPlayers[ i ].szPlayer ) == 0 )
			{
				strParam.Format( KnownPlayers[ i ].szEnqueue, strFile );
				break;
			}
		}
		if ( ! strParam.IsEmpty() )
		{
			HINSTANCE hResult = ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), NULL,
				strCustomPlayer, strParam, NULL, SW_SHOWNORMAL );
			if ( hResult > (HINSTANCE)32 )
				return TRUE;
		}
	}

	// Try Shell "enqueue" verb
	HINSTANCE hResult = ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), _T("enqueue"),
		strFile, NULL, NULL, SW_SHOWNORMAL );
	if ( hResult > (HINSTANCE)32 )
		return TRUE;

	// Try to create "enqueue" verb from default verb for known players
	CString strCommand;
	DWORD nBufferSize = MAX_PATH;
	HRESULT hr = AssocQueryString( 0, ASSOCSTR_COMMAND, strType, NULL,
		strCommand.GetBuffer( MAX_PATH ), &nBufferSize );
	strCommand.ReleaseBuffer();
	int nPos = PathGetArgsIndex( strCommand );
	if ( nPos != -1 )
		strCommand = strCommand.Left( nPos );

	strCommand = strCommand.Trim( _T("\" ") );
	if ( hr == S_OK )
	{
		CString strParam, strExecutable = PathFindFileName( strCommand );
		for ( int i = 0 ; KnownPlayers[ i ].szPlayer ; ++i )
		{
			if ( strExecutable.CompareNoCase( KnownPlayers[ i ].szPlayer ) == 0 )
			{
				strParam.Format( KnownPlayers[ i ].szEnqueue, strFile );
				break;
			}
		}
		if ( ! strParam.IsEmpty() )
		{
			hResult = ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), NULL,
				strCommand, strParam, NULL, SW_SHOWNORMAL );
			if ( hResult > (HINSTANCE)32 )
				return TRUE;
		}
	}

	// Try default verb
	hResult = ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), NULL,
		strFile, NULL, NULL, SW_SHOWNORMAL );
	if ( hResult > (HINSTANCE)32 )
		return TRUE;

	return FALSE;
}

BOOL CFileExecutor::Enqueue(const CStringList& pList)
{
	if ( pList.GetCount() > Settings.Library.ManyFilesCount )	// TOO_MANY_FILES_LIMIT
	{
		CString sMessage;
		sMessage.Format( LoadString( IDS_TOO_MANY_FILES ), pList.GetCount() );
		if ( MsgBox( sMessage, MB_ICONQUESTION | MB_YESNO, 0,
			&Settings.Library.ManyFilesWarning ) != IDYES )
		{
			return FALSE;
		}
	}

	for ( POSITION pos = pList.GetHeadPosition() ; pos ; )
	{
		if ( ! CFileExecutor::Enqueue( pList.GetNext( pos ) ) )
			return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFileExecutor show Bitzi ticket
//
// Note: Moved to new WebServices class
//
//BOOL CFileExecutor::ShowBitziTicket(DWORD nIndex)

//////////////////////////////////////////////////////////////////////
// CFileExecutor display a URL

//BOOL CFileExecutor::DisplayURL(LPCTSTR pszURL)
//{
//	ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), _T("open"), pszURL, NULL, NULL, SW_SHOWNORMAL );
//	return TRUE;
//
//#if 0
// ToDo: Why is this here but disabled?
//	DWORD dwFilterFlags = 0;
//	BOOL bSuccess = FALSE;
//	DWORD hInstance = 0;
//
//	UINT uiResult = DdeInitialize( &hInstance, DDECallback, dwFilterFlags, 0 );
//	if ( uiResult != DMLERR_NO_ERROR ) return FALSE;
//
//	HSZ hszService	= DdeCreateStringHandle( hInstance, L"IExplore", CP_WINUNICODE );
//	HSZ hszTopic	= DdeCreateStringHandle( hInstance, L"WWW_OpenURL", CP_WINUNICODE );
//
//	if ( HCONV hConv = DdeConnect( hInstance, hszService, hszTopic, NULL ) )
//	{
//		CString strCommand;
//		strCommand.Format( _T("\"%s\",,0"), pszURL );
//		CT2A pszCommand( (LPCTSTR)strCommand );
//
//		DdeClientTransaction( (LPBYTE)pszCommand, pszCommand,
//			 hConv, 0, 0, XTYP_EXECUTE, 4000, NULL );
//
//		DdeDisconnect( hConv );
//	}
//
//	DdeFreeStringHandle( hInstance, hszTopic );
//	DdeFreeStringHandle( hInstance, hszService );
//
//	DdeUninitialize( hInstance );
//
//	return bSuccess;
//#endif
//}
