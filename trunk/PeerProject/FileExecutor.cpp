//
// FileExecutor.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "PeerProject.h"
#include "Settings.h"
#include "FileExecutor.h"
#include "Plugins.h"
#include "Skin.h"
#include "XML.h"
#include "Schema.h"
#include "SchemaCache.h"
#include "ShellIcons.h"
#include "Library.h"
#include "SharedFile.h"
#include "Connection.h"
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
	// VideoLAN
	{ _T("vlc.exe"),		_T("--one-instance --playlist-enqueue \"%s\"") },
	// WinAmp
	{ _T("winamp.exe"),		_T("/ADD \"%s\"") },
	// Light Alloy
	{ _T("la.exe"),			_T("/ADD \"%s\"") },
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
		// "command"args
		int quote = str.Find( _T('\"'), 1 );
		if ( quote == -1 )
			return -1;	// No closing quote
		else
			return quote + 1;
	}
	// command args
	int i = -1;
	for ( ;; )
	{
		int slash = str.Find( _T('\\'), i + 1 );
		if ( slash == -1 ||
			GetFileAttributes( str.Mid( 0, slash + 1 ) ) == INVALID_FILE_ATTRIBUTES )
		{
			return str.Find( _T(' '), i + 1 );
		}
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

	CSchemaPtr pSchema;
	if ( ( pSchema = SchemaCache.Get( CSchema::uriAudio ) ) != NULL && pSchema->FilterType( szType ) )
		bAudio = true;
	else if ( ( pSchema = SchemaCache.Get( CSchema::uriVideo ) ) != NULL && pSchema->FilterType( szType ) )
		bVideo = true;
	else if ( ( pSchema = SchemaCache.Get( CSchema::uriImage ) ) != NULL && pSchema->FilterType( szType ) )
		bImage = true;

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
		CLibraryFile* pFile = LibraryMaps.LookupFileByPath( pszFile );
		if ( pFile )
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

int CFileExecutor::FillServices(CString strServicePaths[])
{
	int nCount = 0;
	int nSelected = 3;

	for( string_set::const_reverse_iterator i = Settings.MediaPlayer.ServicePath.rbegin() ; i != Settings.MediaPlayer.ServicePath.rend() ; ++i )
	{
		strServicePaths[nCount] = *i;

		int nAstrix= strServicePaths[nCount].ReverseFind( '*' );

		if( nAstrix == strServicePaths[nCount].GetLength() - 1 )	// Has * at the end so is Selected player
			nSelected = nCount;

		strServicePaths[nCount].Remove( '*' );
		nCount++;
	}

	return nSelected;
}

TRISTATE CFileExecutor::IsSafeExecute(LPCTSTR szExt, LPCTSTR szFile)
{
	BOOL bSafe = ! szExt || IsIn( Settings.Library.SafeExecute, szExt + 1 ) ||
		( theApp.m_pfnAssocIsDangerous && ! theApp.m_pfnAssocIsDangerous( szExt ) );

	if ( ! bSafe && szFile )
	{
		CString strFormat, strPrompt;
		Skin.LoadString( strFormat, IDS_LIBRARY_CONFIRM_EXECUTE );
		strPrompt.Format( strFormat, szFile );
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

BOOL CFileExecutor::Execute(LPCTSTR pszFile, BOOL bSkipSecurityCheck, LPCTSTR pszExt)
{
	CString strServicePaths[3];
	int nSelServiceIndex = FillServices(strServicePaths);

	CWaitCursor pCursor;

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
		return TRUE;	// Skip file
	}

	// Open known file types (links)
	if ( theApp.Open( pszFile ) )
		return TRUE;	// Skip file

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
	if ( ! ( bAudio || bVideo || bImage ) && ! bSkipSecurityCheck )
	{
		TRISTATE bSafe = IsSafeExecute( strType, pszFile );
		if ( bSafe == TRI_UNKNOWN )
			return FALSE;
		else if ( bSafe == TRI_FALSE )
			return TRUE;
	}

	// Handle video and audio files by internal player
	bool bShiftKey = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;
	if ( ! bShiftKey && ( bVideo || bAudio ) && Settings.MediaPlayer.EnablePlay &&
		strType.GetLength() > 1 &&
		IsIn( Settings.MediaPlayer.FileTypes, (LPCTSTR)strType + 1 ) )
	{
		if ( CMediaWnd* pWnd = GetMediaWindow( ! bAudio ) )
		{
			pWnd->PlayFile( pszFile );
			return TRUE;
		}
	}

	// Prepare file path for execution
	CString strFile = CString( _T('\"') ) + pszFile + CString( _T('\"') );
	if ( Settings.MediaPlayer.ShortPaths )
	{
		TCHAR pszShortPath[ MAX_PATH ];
		if ( GetShortPathName( pszFile, pszShortPath, MAX_PATH ) )
			strFile = pszShortPath;
	}

	// Handle video and audio files by external player
	if ( ! bShiftKey && ( bVideo || bAudio ) && nSelServiceIndex < 3 )
	{
		if ( ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), _T("open"),
			strServicePaths[nSelServiceIndex], strFile, NULL,
			SW_SHOWNORMAL ) > (HINSTANCE)SE_ERR_DLLNOTFOUND )
			return TRUE;
	}

	// Handle all by plugins
	if ( ! bShiftKey )
	{
		if ( Plugins.OnExecuteFile( pszFile, bImage ) )
			return TRUE;
	}

	// ToDo: Doesn't work with partial files

	ShellExecute( AfxGetMainWnd()->GetSafeHwnd(),
		NULL, strFile, NULL, NULL, SW_SHOWNORMAL );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFileExecutor enqueue

BOOL CFileExecutor::Enqueue(LPCTSTR pszFile, BOOL /*bSkipSecurityCheck*/, LPCTSTR pszExt)
{
	CString strServicePaths[3];
	int nSelServiceIndex = FillServices(strServicePaths);

	CWaitCursor pCursor;

	// Handle all by plugins
	if ( Plugins.OnEnqueueFile( pszFile ) )
		return TRUE;

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

	// Handle video and audio files by internal player
	bool bShiftKey = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;
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
	if ( ! bShiftKey && ( bVideo || bAudio ) && nSelServiceIndex < 3 )
	{
		// Try Shell "enqueue" verb
		CString strCommand, strParam;
		DWORD nBufferSize = MAX_PATH;
		HRESULT hr = AssocQueryString( ASSOCF_OPEN_BYEXENAME, ASSOCSTR_COMMAND,
			strServicePaths[nSelServiceIndex], _T("enqueue"),
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
		CString strExecutable = PathFindFileName( strServicePaths[nSelServiceIndex] );
		for ( int i = 0; KnownPlayers[ i ].szPlayer; ++i )
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
				strServicePaths[nSelServiceIndex], strParam, NULL, SW_SHOWNORMAL );
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
		for ( int i = 0; KnownPlayers[ i ].szPlayer; ++i )
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

//////////////////////////////////////////////////////////////////////
// CFileExecutor show Bitzi ticket

BOOL CFileExecutor::ShowBitziTicket(DWORD nIndex)
{
	CString str;

	if ( ! Settings.WebServices.BitziOkay )
	{
		Skin.LoadString( str, IDS_LIBRARY_BITZI_MESSAGE );
		if ( AfxMessageBox( str, MB_ICONQUESTION|MB_YESNO ) != IDYES ) return FALSE;
		Settings.WebServices.BitziOkay = TRUE;
		Settings.Save();
	}

	CSingleLock pLock( &Library.m_pSection, TRUE );

	CLibraryFile* pFile = Library.LookupFile( nIndex );
	if ( pFile == NULL ) return FALSE;

	if ( ! pFile->m_oSHA1 || ! pFile->m_oTiger || ! pFile->m_oED2K )
	{
		CString strFormat;
		Skin.LoadString( strFormat, IDS_LIBRARY_BITZI_HASHED );
		str.Format( strFormat, (LPCTSTR)pFile->m_sName );
		pLock.Unlock();
		AfxMessageBox( str, MB_ICONINFORMATION );
		return FALSE;
	}

	CString strURL = Settings.WebServices.BitziWebView;
	CFile hFile;

	if ( hFile.Open( pFile->GetPath(), CFile::modeRead|CFile::shareDenyNone ) )
	{
		strURL = Settings.WebServices.BitziWebSubmit;

		if ( hFile.GetLength() > 0 )
		{
			static LPCTSTR pszHex = _T("0123456789ABCDEF");
			BYTE nBuffer[20];
			int nPeek = hFile.Read( nBuffer, 20 );
			hFile.Close();
			str.Empty();

			for ( int nByte = 0 ; nByte < nPeek ; nByte++ )
			{
				str += pszHex[ (BYTE)nBuffer[ nByte ] >> 4 ];
				str += pszHex[ (BYTE)nBuffer[ nByte ] & 15 ];
			}

			strURL.Replace( _T("(FIRST20)"), str );
		}
		else
			strURL.Replace( _T("(FIRST20)"), _T("0") );
	}
	else
		strURL.Replace( _T("(URN)"), pFile->m_oSHA1.toString() + _T(".") + pFile->m_oTiger.toString() );

	CString strName = pFile->m_sName;
	LPCTSTR pszExt = _tcsrchr( strName, '.' );
	int nExtLen = pszExt ? static_cast< int >( _tcslen( pszExt ) - 1 ) : 0;
	CString strExt = strName.Right( nExtLen );
	strExt.Trim().MakeUpper();

	strURL.Replace( _T("(NAME)"), URLEncode( strName ) );
	strURL.Replace( _T("(SHA1)"), pFile->m_oSHA1.toString() );
	strURL.Replace( _T("(TTH)"), pFile->m_oTiger.toString() );
	strURL.Replace( _T("(ED2K)"), pFile->m_oED2K.toString() );
	strURL.Replace( _T("(AGENT)"), URLEncode( Settings.SmartAgent() ) );

	str.Format( _T("%I64i"), pFile->GetSize() );
	strURL.Replace( _T("(SIZE)"), str );


	CString strINFO = _T("&tag.tiger.tree=") + pFile->m_oTiger.toString();
	if ( pFile->m_oMD5 )
		strINFO += _T("&tag.md5.md5=") + pFile->m_oMD5.toString();
	if ( ! pFile->m_sComments.Trim().IsEmpty() )
		strINFO += _T("&tag.subjective.comment=") + URLEncode( pFile->m_sComments );

	if ( pFile->m_pMetadata != NULL && pFile->m_pSchema != NULL )
	{
		CXMLElement* pMetadata = pFile->m_pMetadata;
		int nTemp, nMP3orOGGorWAVTag = 0, nImageTag = 0;
		CString strDescription, strTitle, strMP3orOGGorWAVTag, strImageTag;

		for ( POSITION pos = pMetadata->GetAttributeIterator() ; pos ; )
		{
			CString strReplace;
			CXMLNode* pNode = pMetadata->GetNextAttribute( pos );
			str = pNode->GetName();
			strReplace = pNode->GetValue();

			if ( str == "link" )
			{
				strINFO += _T("&tag.url.url=") + URLEncode( strReplace );
			}
			else if ( pFile->m_pSchema->CheckURI( CSchema::uriAudio ) )
			{
				if ( str == "description" )
					strINFO += _T("&tag.objective.description=") + URLEncode( strReplace.Trim() );
				else if ( str == "title" )
					strINFO += _T("&tag.audiotrack.title=") + URLEncode( strReplace.Trim() );
				else if ( str == "artist" )
					strINFO += _T("&tag.audiotrack.artist=") + URLEncode( strReplace.Trim() );
				else if ( str == "album" )
					strINFO += _T("&tag.audiotrack.album=") + URLEncode( strReplace.Trim() );
				else if ( str == "track" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.audiotrack.tracknumber=") + strReplace;
				}
				else if ( str == "year" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.audiotrack.year=") + strReplace;
				}
				// ToDo: Read WAV information in FileExecutor.cpp, bitzi submit is already ready
				else if ( strExt == "MP3" || strExt == "OGG" || strExt == "WAV" )
				{
					if ( str == "bitrate" )
					{
						if ( strExt == "MP3" )
						{
							strMP3orOGGorWAVTag += _T("&tag.mp3.vbr=");

							if( _tcsstr( strReplace, _T("~") ) )
								strMP3orOGGorWAVTag += _T("y");
							else
								strMP3orOGGorWAVTag += _T("n");
						}

						nTemp = _ttoi( strReplace );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == "MP3" )
							strMP3orOGGorWAVTag += _T("&tag.mp3.bitrate=");
						else if ( strExt == "OGG" )
							strMP3orOGGorWAVTag += _T("&tag.vorbis.bitrate=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
						{
							strMP3orOGGorWAVTag += strReplace;
							nMP3orOGGorWAVTag++;
						}
					}
					// ToDo: Read sampleSize of WAV in FileExecutor.cpp, bitzi submit is already ready
					else if ( str == "sampleSize" )
					{
						nTemp = _ttoi( strReplace );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == "WAV" )
						{
							strMP3orOGGorWAVTag += _T("&tag.wav.samplesize=") + strReplace;
							nMP3orOGGorWAVTag++;
						}
					}
					else if ( str == "seconds" )
					{
						nTemp = (int)( _wtof( strReplace ) * 1000 );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == "MP3" )
							strMP3orOGGorWAVTag += _T("&tag.mp3.duration=");
						else if ( strExt == "OGG" )
							strMP3orOGGorWAVTag += _T("&tag.vorbis.duration=");
						else if ( strExt == "WAV" )
							strMP3orOGGorWAVTag += _T("&tag.wav.duration=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
						{
							strMP3orOGGorWAVTag += strReplace;
							nMP3orOGGorWAVTag++;
						}
					}
					else if ( str == "sampleRate" )
					{
						nTemp = _ttoi( strReplace );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == "MP3" )
							strMP3orOGGorWAVTag += _T("&tag.mp3.samplerate=");
						else if ( strExt == "OGG" )
							strMP3orOGGorWAVTag += _T("&tag.vorbis.samplerate=");
						else if ( strExt == "WAV" )
							strMP3orOGGorWAVTag += _T("&tag.wav.samplerate=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
						{
							strMP3orOGGorWAVTag += strReplace;
							nMP3orOGGorWAVTag++;
						}
					}
					else if ( str == "channels" )
					{
						nTemp = _ttoi( strReplace );
						strReplace.Format( _T("%d"), nTemp );

						if ( strExt == "OGG" )
							strMP3orOGGorWAVTag += _T("&tag.vorbis.channels=");
						else if ( strExt == "WAV" )
							strMP3orOGGorWAVTag += _T("&tag.wav.channels=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
						{
							strMP3orOGGorWAVTag += strReplace;
							nMP3orOGGorWAVTag++;
						}
					}
					else if ( str == "soundType" )
					{
						if ( strExt == "MP3" )
							if ( ( strReplace == "Stereo" ) || ( strReplace == "Joint Stereo" ) )
								strMP3orOGGorWAVTag += _T("&tag.mp3.stereo=y");
							else if ( ( strReplace == "Dual Channel" ) || ( strReplace == "Single Channel" ) )
								strMP3orOGGorWAVTag += _T("&tag.mp3.stereo=n");
							else
								strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
							nMP3orOGGorWAVTag++;
					}
					else if ( str == "encoder" )
					{
						if ( strExt == "MP3" )
							strMP3orOGGorWAVTag += _T("&tag.mp3.encoder=");
						else if ( strExt == "OGG" )
							strMP3orOGGorWAVTag += _T("&tag.vorbis.encoder=");
						else
							strReplace.Empty();

						if ( ! strReplace.IsEmpty() )
							strMP3orOGGorWAVTag += URLEncode( strReplace );
					}
				}
			}
			else if ( pFile->m_pSchema->CheckURI( CSchema::uriImage ) )
			{
				if ( str == "description" )
					strINFO += _T("&tag.objective.description=") + URLEncode( strReplace.Trim() );
				else if ( str == "width" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strImageTag += _T("&tag.image.width=") + strReplace;
					nImageTag++;
				}
				else if ( str == "height" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strImageTag += _T("&tag.image.height=") + strReplace;
					nImageTag++;
				}
				else if ( str == "colors" )
				{
					if ( strReplace == "2" ) strReplace = "1";
					else if ( strReplace == "16" ) strReplace = "4";
					else if ( strReplace == "256" || strReplace == "Greyscale" ) strReplace = "8";
					else if ( strReplace == "64K" ) strReplace = "16";
					else if ( strReplace == "16.7M" ) strReplace = "24";
					else strReplace = "";

					if ( ! strReplace.IsEmpty() )
					{
						strImageTag += _T("&tag.image.bpp=") + strReplace;
						nImageTag++;
					}
				}
			}
			else if ( pFile->m_pSchema->CheckURI( CSchema::uriVideo ) )
			{
				if ( str == "realdescription" )
					strINFO += _T("&tag.objective.description=") + URLEncode( strReplace.Trim() );
				else if ( str == "width" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.width=") + strReplace;
				}
				else if ( str == "height" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.height=") + strReplace;
				}
				else if ( str == "frameRate" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.fps=") + strReplace;
				}
				else if ( str == "minutes" )
				{
					nTemp = (int)( _wtof( strReplace ) * 60 * 1000 );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.duration=") + strReplace;
				}
				// ToDo: Read video's bitrate in FileExecutor.cpp, bitzi submit is already ready
				else if ( str == "bitrate" )
				{
					nTemp = _ttoi( strReplace );
					strReplace.Format( _T("%d"), nTemp );

					strINFO += _T("&tag.video.bitrate=") + strReplace;
				}
				else if ( str == "codec" )
				{
					strReplace.MakeUpper();
					strINFO += _T("&tag.video.codec=") + URLEncode( strReplace );
				}
			}
			else if ( pFile->m_pSchema->CheckURI( CSchema::uriApplication ) )
			{
				if ( str == "fileDescription" )
					strDescription = URLEncode( strReplace.Trim() );
				else if ( str == "title" )
					strTitle = URLEncode( strReplace.Trim() );
			}
			else
			{
				if ( str == "description" )
					strINFO += _T("&tag.objective.description=") + URLEncode( strReplace.Trim() );
			}
		}

		if ( nMP3orOGGorWAVTag == 4 )
			strINFO += strMP3orOGGorWAVTag;

		if ( nImageTag == 3 )
		{
			strINFO += strImageTag;

			if ( strExt == "BMP" || strExt == "GIF" || strExt == "PNG" )
				strINFO += _T("&tag.image.format=") + strExt;
			else if ( strExt == "JPG" || strExt == "JPEG" || strExt == "JPE" || strExt == "JFIF" )
				strINFO += _T("&tag.image.format=JPEG");
		}

		if ( ! strDescription.IsEmpty() )
			strINFO += _T("&tag.objective.description=") + strDescription;
		else if ( ! strTitle.IsEmpty() )
			strINFO += _T("&tag.objective.description=") + strTitle;
	}

	if ( strExt == "AVI" )
		strINFO += _T("&tag.video.format=AVI");
	else if ( strExt == "DIVX" || strExt == "DIV" || strExt == "TIX" )
		strINFO += _T("&tag.video.format=DivX");
	else if ( strExt == "XVID" )
		strINFO += _T("&tag.video.format=XviD");
	else if ( strExt == "MKV" )
		strINFO += _T("&tag.video.format=Matroska");
	else if ( strExt == "MOV" || strExt == "QT" )
		strINFO += _T("&tag.video.format=QuickTime");
	else if ( strExt == "RM" || strExt == "RMVB" || strExt == "RAM" || strExt == "RPM" || strExt == "RV" )
		strINFO += _T("&tag.video.format=Real");
	else if ( strExt == "MPG" || strExt == "MPEG" || strExt == "MPE" )
		strINFO += _T("&tag.video.format=MPEG");
	else if ( strExt == "M1V" )
		strINFO += _T("&tag.video.format=MPEG-1");
	else if ( strExt == "MPV2" || strExt == "MP2" || strExt == "M2V" )
		strINFO += _T("&tag.video.format=MPEG-2");
	else if ( strExt == "MP4" || strExt == "M4V" )
		strINFO += _T("&tag.video.format=MPEG-4");
	else if ( strExt == "WM" || strExt == "WMV" || strExt == "WMD" || strExt == "ASF" )
		strINFO += _T("&tag.video.format=Windows Media");
	else if ( strExt == "OGM" )
		strINFO += _T("&tag.video.format=Ogg Media File");
	else if ( strExt == "VP6" )
		strINFO += _T("&tag.video.format=VP6");
	else if ( strExt == "VOB" )
		strINFO += _T("&tag.video.format=DVD");
	else if ( strExt == "IVF" )
		strINFO += _T("&tag.video.format=Indeo Video");
	else if ( pFile->m_pSchema != NULL && pFile->m_pSchema->CheckURI( CSchema::uriVideo ) && ! strExt.IsEmpty() )
		strINFO += _T("&tag.video.format=") + URLEncode( strExt );

	strURL.Replace( _T("&(INFO)"), strINFO );

	pLock.Unlock();

	DisplayURL( strURL );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFileExecutor display a URL

BOOL CFileExecutor::DisplayURL(LPCTSTR pszURL)
{
	ShellExecute( AfxGetMainWnd()->GetSafeHwnd(), _T("open"), pszURL, NULL, NULL, SW_SHOWNORMAL );
	return TRUE;

#if 0
	DWORD dwFilterFlags = 0;
	BOOL bSuccess = FALSE;
	DWORD hInstance = 0;

	UINT uiResult = DdeInitialize( &hInstance, DDECallback, dwFilterFlags, 0 );
	if ( uiResult != DMLERR_NO_ERROR ) return FALSE;

	HSZ hszService	= DdeCreateStringHandle( hInstance, L"IExplore", CP_WINUNICODE );
	HSZ hszTopic	= DdeCreateStringHandle( hInstance, L"WWW_OpenURL", CP_WINUNICODE );

	if ( HCONV hConv = DdeConnect( hInstance, hszService, hszTopic, NULL ) )
	{
		CString strCommand;

		strCommand.Format( _T("\"%s\",,0"), pszURL );
		CT2A pszCommand( (LPCTSTR)strCommand );

		DdeClientTransaction( (LPBYTE)pszCommand, pszCommand,
			 hConv, 0, 0, XTYP_EXECUTE, 4000, NULL );

		DdeDisconnect( hConv );
	}

	DdeFreeStringHandle( hInstance, hszTopic );
	DdeFreeStringHandle( hInstance, hszService );

	DdeUninitialize( hInstance );

	return bSuccess;
#endif
}
