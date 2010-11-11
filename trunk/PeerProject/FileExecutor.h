//
// FileExecutor.h
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

#pragma once

class CMediaWnd;
class CLibraryWnd;


class CFileExecutor
{
public:
	// Is file extension safe to execute?
	// Returns: TRI_TRUE - safe, TRI_FALSE - dangerous, TRI_UNKNOWN - dangerous and cancel
	static TRISTATE IsSafeExecute(LPCTSTR szExt, LPCTSTR szFile = NULL);
	static BOOL		Execute(LPCTSTR pszFile, BOOL bSkipSecurityCheck = FALSE, LPCTSTR pszExt = NULL);
	static BOOL		Enqueue(LPCTSTR pszFiles, BOOL bSkipSecurityCheck = FALSE, LPCTSTR pszExt = NULL);
	static BOOL		ShowBitziTicket(DWORD nFile);
	static BOOL		DisplayURL(LPCTSTR pszURL);

protected:
	// Open Media Player window
	static CMediaWnd*	GetMediaWindow(BOOL bFocus);

	// Open Library window
	static CLibraryWnd*	GetLibraryWindow();

	// Is file a video, audio or image file?
	static void DetectFileType(LPCTSTR pszFile, LPCTSTR szType, bool& bVideo, bool& bAudio, bool& bImage);

	//Extracts custom players form settings
	static int FillServices(CString sServicePaths[]);
};