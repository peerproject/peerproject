//
// FileExecutor.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#pragma once

class CMediaWnd;
class CLibraryWnd;


class CFileExecutor
{
public:
	// Is file extension safe to execute?  TRI_TRUE -safe, TRI_FALSE -dangerous, TRI_UNKNOWN -dangerous and cancel
	static TRISTATE IsSafeExecute(LPCTSTR szExt, LPCTSTR szFile = NULL);
	static TRISTATE	IsVerified(LPCTSTR szFile);
	static BOOL		Execute(LPCTSTR pszFile, LPCTSTR pszExt = NULL);
	static BOOL		Execute(const CStringList& pList);
	static BOOL		Enqueue(LPCTSTR pszFiles, LPCTSTR pszExt = NULL);
	static BOOL		Enqueue(const CStringList& pList);
//	static BOOL		ShowBitziTicket(DWORD nFile);	// Moved to WebServices
//	static BOOL		DisplayURL(LPCTSTR pszURL);

protected:
	// Open Media Player window
	static CMediaWnd*	GetMediaWindow(BOOL bFocus);

	// Open Library window
	static CLibraryWnd*	GetLibraryWindow();

	// Is file a video, audio or image file?
	static void DetectFileType(LPCTSTR pszFile, LPCTSTR szType, bool& bVideo, bool& bAudio, bool& bImage);

	// Extract custom mediaplayers from settings
	static CString GetCustomPlayer();
};
