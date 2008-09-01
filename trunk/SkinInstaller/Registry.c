// 
// Registry.c
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

static int CreateHKCRKey(LPCTSTR lpSubKey, LPTSTR lpClass, LPTSTR lpData);
static int DeleteHKCRKey(LPCTSTR lpSubKey, LPTSTR lpClass);

// EXPORT BEGIN
void CreateSkinKeys() {
	TCHAR filename[MAX_PATH];
	TCHAR fullpath[MAX_PATH];
	TCHAR imagpath[MAX_PATH];

	GetModuleFileName(NULL,filename,sizeof(filename));
	_snwprintf(fullpath, sizeof(fullpath), L"\"%s\" \"%%1\"", filename);
	_snwprintf(imagpath, sizeof(imagpath), L"%s,0", filename);
	CreateHKCRKey(L".psk", L"", L"PeerProject.SkinFile");
	CreateHKCRKey(L".sks", L"", L"PeerProject.SkinFile");
	CreateHKCRKey(L"PeerProject.SkinFile", L"", L"PeerProject Skin File");
	CreateHKCRKey(L"PeerProject.SkinFile\\DefaultIcon", L"", imagpath);
	CreateHKCRKey(L"PeerProject.SkinFile\\shell", L"", L"open");
	CreateHKCRKey(L"PeerProject.SkinFile\\shell\\open\\command", L"", fullpath);
	CreateHKCRKey(L"PeerProject.SkinFile\\shell\\skininstall", L"", L"Install PeerProject Skin");
	CreateHKCRKey(L"PeerProject.SkinFile\\shell\\skininstall\\command", L"", fullpath);
}

void DeleteSkinKeys() {
	DeleteHKCRKey(L".psk", L"");
	DeleteHKCRKey(L".sks", L"");
	DeleteHKCRKey(L"PeerProject.SkinFile\\shell\\open\\command", L"");
	DeleteHKCRKey(L"PeerProject.SkinFile\\shell\\open", L"");
	DeleteHKCRKey(L"PeerProject.SkinFile\\shell\\skininstall\\command", L"");
	DeleteHKCRKey(L"PeerProject.SkinFile\\shell\\skininstall", L"");
	DeleteHKCRKey(L"PeerProject.SkinFile\\shell", L"");
	DeleteHKCRKey(L"PeerProject.SkinFile\\DefaultIcon", L"");
	DeleteHKCRKey(L"PeerProject.SkinFile", L"");
}
//EXPORT END

static int CreateHKCRKey(LPCTSTR lpSubKey, LPTSTR lpClass, LPTSTR lpData) {
    HKEY keyHandle;
    long rtn =0;
    DWORD lpdw;
    DWORD aLen;

    aLen = (DWORD)wcslen(lpData) * sizeof(TCHAR) + 1;
    rtn = RegCreateKeyEx(HKEY_CLASSES_ROOT,lpSubKey,0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,0,&keyHandle,&lpdw);
    if(rtn == ERROR_SUCCESS) {
		RegSetValueEx(keyHandle,lpClass,0,REG_SZ,(LPBYTE)lpData,aLen);
        rtn = RegCloseKey(keyHandle);
    }
    return rtn;
}

static int DeleteHKCRKey(LPCTSTR lpSubKey, LPTSTR lpClass) {
    HKEY keyHandle;
    long rtn =0;
    DWORD lpdw;

    rtn = RegCreateKeyEx(HKEY_CLASSES_ROOT,lpSubKey,0,NULL,REG_OPTION_NON_VOLATILE, KEY_READ,0,&keyHandle,&lpdw);
    if(rtn == ERROR_SUCCESS) {
		RegDeleteKey(keyHandle,lpClass);
        rtn = RegCloseKey(keyHandle);
    }
    return rtn;
}
