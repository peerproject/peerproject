//
// FileExecutor.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#pragma once

class CMediaWnd;
class CLibraryWnd;


class CFileExecutor
{
public:
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
};
