// 
// Skin.h
// 
// This file is part of PeerProject (peerproject.org) © 2008
// 
// Portions of this page have been previously released into the public domain.  
// You are free to redistribute and modify it without any restrictions
// with the exception of the following notice:
// 
// The Zlib library is Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler.
// The Unzip library is Copyright (C) 1998-2003 Gilles Vollant.

#ifndef SKIN_H
#define SKIN_H

#include <stddef.h>
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <unzip.h>
#include <errno.h>
#include <tchar.h>
#include "Resource.h"

#define SKIN_RAZA_HWND	L"PeerProjectMainWnd"
#define SKIN_SKIN_TITLE	L"PeerProject Skin Installer"
#define SKIN_LANG_TITLE L"PeerProject Language Installer"
#define VERSION         L"1.0.0"

// Globals
extern int   skinType;
extern TCHAR* szName;
extern TCHAR* szVersion;
extern TCHAR* szAuthor;
extern TCHAR* szUpdates;
extern TCHAR* szXML;
extern TCHAR prefix[MAX_PATH];
extern BOOL bRunningOnNT;

// Extract.c
void ExtractSkinFile(LPCTSTR pszFile);
int GetInstallDirectory();
int GetSkinFileCount(LPTSTR pszFile);
int ValidateSkin(LPTSTR pszFile, HWND hwndDlg);
int ExtractSkin(LPTSTR pszFile, HWND hwndDlg);
LPCTSTR GetUnicodeString(char* pszString);

// Registry.c
void CreateSkinKeys();
void DeleteSkinKeys();

// Utils.c
void LoadManifestInfo(char *buf);
int SetSkinAsDefault();
int MakeDirectory(LPCTSTR newdir);

// Window.c
INT_PTR CALLBACK ExtractProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
