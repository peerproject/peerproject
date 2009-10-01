//
// Skin.c
//
// This file is part of PeerProject (peerproject.org) © 2008
//
// Portions of this page have been previously released into the public domain.
// You are free to redistribute and modify it without any restrictions
// with the exception of the following notice:
//
// The Zlib library is Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler.
// The Unzip library is Copyright (C) 1998-2003 Gilles Vollant.

#include "Skin.h"

// globals
int   skinType;
TCHAR* szName;
TCHAR* szPath;
TCHAR* szVersion;
TCHAR* szAuthor;
TCHAR* szUpdates;
TCHAR* szXML;
TCHAR prefix[MAX_PATH];

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR cmdParam, int cmdShow) {
	InitCommonControls();

	// globals
	skinType  = 0;
	szName    = NULL;
	szPath    = NULL;
	szVersion = NULL;
	szAuthor  = NULL;
	szUpdates = NULL;
	szXML     = NULL;

	if (wcslen(cmdParam)==0) MessageBox(NULL,L"PeerProject Skin Installer " VERSION L"\n\nDouble-click on a PeerProject Skin File (.psk) to use this tool.",L"PeerProject Skin Installer",MB_OK | MB_ICONINFORMATION);
	else if (!wcscmp(cmdParam, L"/install") || !wcscmp(cmdParam, L"/installsilent")) CreateSkinKeys();
	else if (!wcscmp(cmdParam, L"/uninstall") || !wcscmp(cmdParam, L"/uninstallsilent")) DeleteSkinKeys();
	else ExtractSkinFile(cmdParam);

	// free up memory from globals
	if (szName) free(szName);
	if (szPath) free(szPath);
	if (szVersion) free(szVersion);
	if (szAuthor) free(szAuthor);
	if (szUpdates) free(szUpdates);
	if (szXML) free(szXML);
	return 0;
}
