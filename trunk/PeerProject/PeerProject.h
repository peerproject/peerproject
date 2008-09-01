//
// PeerProject.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#include "Resource.h"
#include "ComObject.h"
#include "Buffer.h"
#include <wincrypt.h>

class CUPnPFinder;
class CMainWnd;
class CSplashDlg;
class CFontManager;

class CPeerProjectCommandLineInfo : public CCommandLineInfo
{
public:
	CPeerProjectCommandLineInfo();

	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	BOOL m_bHelp;
	BOOL m_bTray;
	BOOL m_bNoSplash;
	BOOL m_bNoAlphaWarning;
	INT  m_nGUIMode;

private:
	CPeerProjectCommandLineInfo(const CPeerProjectCommandLineInfo&);
	CPeerProjectCommandLineInfo& operator=(const CPeerProjectCommandLineInfo&);
};

class CPeerProjectApp : public CWinApp
{
public:
	CPeerProjectApp();

	HANDLE				m_pMutex;
	CMutex				m_pSection;
	WORD				m_nVersion[4];
	BYTE				m_pBTVersion[4];			// PExx
	CString				m_sVersion;					// x.x.x.x
	CString				m_sSmartAgent;				// PeerProject x.x.x.x
	CString				m_sBuildDate;
	CString				m_strBinaryPath;			// PeerProject.exe path
	CFont				m_gdiFont;
	CFont				m_gdiFontBold;
	CFont				m_gdiFontLine;
	CWnd*				m_pSafeWnd;
	volatile bool		m_bLive;
	BOOL				m_bInteractive;
	bool				m_bIsServer;				// Is OS a Server version
	bool				m_bIsWin2000;				// Is OS Windows 2000
	bool				m_bIsVistaOrNewer;			// Is OS Vista or newer
	bool				m_bLimitedConnections;		// Networking is limited (XP SP2)
	DWORD				m_nWindowsVersion;			// Windows version
	DWORD				m_nWindowsVersionMinor;		// Windows minor version
	QWORD				m_nPhysicalMemory;			// Physical RAM installed
	int					m_nLogicalProcessors;		// Multi-core, Multi-CPUs, or HT modules
	BOOL				m_bMenuWasVisible;			// For the menus in media player window
	int					m_nDefaultFontSize;			// The basic font size. (11)
	CString				m_sDefaultFont;				// Main font. (Tahoma)
	CString				m_sPacketDumpFont;			// Packet Window. (Lucida Console)
	CString				m_sSystemLogFont;			// System Window. (Courier New)
	boost::scoped_ptr< CUPnPFinder > m_pUPnPFinder;
	TRISTATE			m_bUPnPPortsForwarded;		// UPnP values are assigned when the discovery is complete
	TRISTATE			m_bUPnPDeviceConnected;		// or when the service notifies
	DWORD				m_nUPnPExternalAddress;
	DWORD				m_nLastInput;				// Time of last input event (in secs)
	HHOOK				m_hHookKbd;
	HHOOK				m_hHookMouse;

	// Cryptography Context handle
	HCRYPTPROV			m_hCryptProv;

	// For themes functions
	HINSTANCE	m_hTheme;
	HRESULT		(WINAPI *m_pfnSetWindowTheme)(HWND, LPCWSTR, LPCWSTR);
	BOOL		(WINAPI *m_pfnIsThemeActive)(VOID);
	HANDLE		(WINAPI *m_pfnOpenThemeData)(HWND, LPCWSTR);
	HRESULT		(WINAPI *m_pfnCloseThemeData)(HANDLE);
	HRESULT		(WINAPI *m_pfnDrawThemeBackground)(HANDLE, HDC, int, int, const RECT*, const RECT*);
	HRESULT		(WINAPI *m_pfnEnableThemeDialogTexture)(HWND, DWORD);
//	HRESULT		(WINAPI *m_pfnDrawThemeParentBackground)(HWND, HDC, RECT*);
//	HRESULT		(WINAPI *m_pfnGetThemeBackgroundContentRect)(HANDLE, HDC, int, int, const RECT*, RECT*);
//	HRESULT		(WINAPI *m_pfnGetThemeSysFont)(HANDLE, int, LOGFONT);
//	HRESULT		(WINAPI *m_pfnDrawThemeText)(HANDLE, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const RECT*);

	// Shell functions
	HINSTANCE	m_hShlWapi;
	BOOL		(WINAPI *m_pfnAssocIsDangerous)(LPCWSTR);

	// GeoIP - IP to Country lookup
	HINSTANCE	m_hGeoIP;
	GeoIP*		m_pGeoIP;
	GeoIP_country_code_by_addrFunc	m_pfnGeoIP_country_code_by_addr;
	GeoIP_country_name_by_addrFunc	m_pfnGeoIP_country_name_by_addr;

	HINSTANCE	m_hLibGFL;

	HINSTANCE			CustomLoadLibrary(LPCTSTR);
	CMainWnd*			SafeMainWnd() const;
	bool				IsLogDisabled(WORD nType) const;
	void				Message(WORD nType, UINT nID, ...) const;
	void				Message(WORD nType, LPCTSTR pszFormat, ...) const;
	BOOL				InternalURI(LPCTSTR pszURI);
	void				PrintMessage(WORD nType, const CString& strLog) const;
	void				LogMessage(LPCTSTR pszLog) const;
	void				SplashStep(LPCTSTR pszMessage = NULL, int nMax = 0, bool bClosing = false);

	CString				GetCountryCode(IN_ADDR pAddress) const;
	CString				GetCountryName(IN_ADDR pAddress) const;

//	CFontManager*		m_pFontManager;

	// Open file or url. Returns NULL always.
	virtual CDocument*	OpenDocumentFile(LPCTSTR lpszFileName);
	// Open file or url (generic function)
	static BOOL			Open(LPCTSTR lpszFileName, BOOL bDoIt);
	// Open .lnk file
	static BOOL			OpenShellShortcut(LPCTSTR lpszFileName, BOOL bDoIt);
	// Open .url file
	static BOOL			OpenInternetShortcut(LPCTSTR lpszFileName, BOOL bDoIt);
	// Open .torrent file
	static BOOL			OpenTorrent(LPCTSTR lpszFileName, BOOL bDoIt);
	// Open .co, .collection, or .emulecollection file
	static BOOL			OpenCollection(LPCTSTR lpszFileName, BOOL bDoIt);
	// Open url
	static BOOL			OpenURL(LPCTSTR lpszFileName, BOOL bDoIt, BOOL bSilent = FALSE);

protected:
	CSplashDlg*					m_dlgSplash;
	mutable CCriticalSection	m_csMessage;
	CPeerProjectCommandLineInfo	m_ocmdInfo;

	virtual BOOL		InitInstance();
	virtual int			ExitInstance();
	virtual void		WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

	void				GetVersionNumber();
	void				InitResources();

	DECLARE_MESSAGE_MAP()
};

extern CPeerProjectApp theApp;

//
// Utility Functions
//

// Post message to main window in safe way
BOOL PostMainWndMessage(UINT Msg, WPARAM wParam = NULL, LPARAM lParam = NULL);

CRuntimeClass* AfxClassForName(LPCTSTR pszClass);

BOOL LoadString(CString& str, UINT nID);
LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszSubString);
LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszPattern, size_t plen);
void Split(const CString& strSource, TCHAR cDelimiter, CStringArray& pAddIt, BOOL bAddFirstEmpty = FALSE);
BOOL LoadSourcesString(CString& str, DWORD num, bool bFraction=false);

DWORD	TimeFromString(LPCTSTR psz);
CString	TimeToString(time_t tVal);
BOOL	TimeFromString(LPCTSTR psz, FILETIME* pTime);
CString	TimeToString(FILETIME* pTime);

void	RecalcDropWidth(CComboBox* pWnd);
// Load 16x16, 32x32, 48x48 icons from .ico, .exe, .dll files
BOOL LoadIcon(LPCTSTR szFilename, HICON* phSmallIcon, HICON* phLargeIcon, HICON* phHugeIcon);
// Load and add icon to CImageList, mirrored if needed
int		AddIcon(UINT nIcon, CImageList& gdiImageList);
// Add icon to CImageList, mirrored if needed
int		AddIcon(HICON hIcon, CImageList& gdiImageList);
// Create mirrored icon. Returns:
// mirrored icon (original destroyed if needed) if succeed or original icon otherwise
HICON	CreateMirroredIcon(HICON hIconOrig, BOOL bDestroyOriginal = TRUE);
HBITMAP	CreateMirroredBitmap(HBITMAP hbmOrig);

LRESULT CALLBACK KbdHook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam);

CString GetFolderPath( int nFolder );
CString GetWindowsFolder();
CString GetProgramFilesFolder();
CString GetDocumentsFolder();
CString GetAppDataFolder();
CString GetLocalAppDataFolder();

// Create directory. If one or more of the intermediate folders do not exist, they are created as well.
BOOL CreateDirectory(LPCTSTR szPath);

// Loads RT_HTML or RT_GZIP resource as string
CString LoadHTML(HINSTANCE hInstance, UINT nResourceID);

// Loads well-known resource for HTTP-uploading
bool ResourceRequest(const CString& strPath, CBuffer& pResponse, CString& sHeader);

// Mark file as downloaded from Internet (using NTFS stream)
bool MarkFileAsDownload(const CString& sFilename);

// Load GUID from NTFS stream of file
bool LoadGUID(const CString& sFilename, Hashes::Guid& oGUID);

// Save GUID to NTFS stream of file
bool SaveGUID(const CString& sFilename, const Hashes::Guid& oGUID);

// Resolve shell shortcut (.lnk file)
CString ResolveShortcut(LPCTSTR lpszFileName);

// Get Win32 API error description
CString GetErrorString(DWORD dwError = GetLastError());

// Displays a dialog box enabling the user to select a Shell folder
CString BrowseForFolder(UINT nTitle, LPCTSTR szInitialPath = NULL, HWND hWnd = NULL);
CString BrowseForFolder(LPCTSTR szTitle, LPCTSTR szInitialPath = NULL, HWND hWnd = NULL);

typedef enum
{
	sNone = 0,
	sNumeric = 1,
	sRegular = 2,
	sKanji = 4,
	sHiragana = 8,
	sKatakana = 16
} ScriptType;

struct CompareNums
{
	inline bool operator()(WORD lhs, WORD rhs) const
	{
		return lhs > rhs;
	}
};

inline bool IsCharacter(TCHAR nChar)
{
	WORD nCharType = 0;

	if ( GetStringTypeExW( LOCALE_NEUTRAL, CT_CTYPE3, &nChar, 1, &nCharType ) )
		return ( ( nCharType & C3_ALPHA ) == C3_ALPHA ||
				 ( ( nCharType & C3_KATAKANA ) == C3_KATAKANA ||
				   ( nCharType & C3_HIRAGANA ) == C3_HIRAGANA ) &&
				   !( ( nCharType & C3_SYMBOL ) == C3_SYMBOL )  ||
				 ( nCharType & C3_IDEOGRAPH ) == C3_IDEOGRAPH ||
				 _istdigit( nChar ) );

	return false;
}

inline bool IsHiragana(TCHAR nChar)
{
	WORD nCharType = 0;

	if ( GetStringTypeExW( LOCALE_NEUTRAL, CT_CTYPE3, &nChar, 1, &nCharType ) )
		return ( ( nCharType & C3_HIRAGANA ) == C3_HIRAGANA );
	return false;
}

inline bool IsKatakana(TCHAR nChar)
{
	WORD nCharType = 0;

	if ( GetStringTypeExW( LOCALE_NEUTRAL, CT_CTYPE3, &nChar, 1, &nCharType ) )
		return ( ( nCharType & C3_KATAKANA ) == C3_KATAKANA );
	return false;
}

inline bool IsKanji(TCHAR nChar)
{
	WORD nCharType = 0;

	if ( GetStringTypeExW( LOCALE_NEUTRAL, CT_CTYPE3, &nChar, 1, &nCharType ) )
		return ( ( nCharType & C3_IDEOGRAPH ) == C3_IDEOGRAPH );
	return false;
}

inline bool IsWord(LPCTSTR pszString, size_t nStart, size_t nLength)
{
	for ( pszString += nStart ; *pszString && nLength ; pszString++, nLength-- )
	{
		if ( _istdigit( *pszString ) ) return false;
	}
	return true;
}

inline bool IsHasDigit(LPCTSTR pszString, size_t nStart, size_t nLength)
{
	for ( pszString += nStart ; *pszString && nLength ; pszString++, nLength-- )
	{
		if ( _istdigit( *pszString ) ) return true;
	}
	return false;
}

inline bool IsNumeric(LPCTSTR pszString, size_t nStart, size_t nLength)
{
	bool bDigit = true;
	for ( pszString += nStart ; *pszString && nLength ; pszString++, nLength-- )
	{
		if ( !_istdigit( *pszString ) ) bDigit = false;
	}
	return bDigit;
}

inline void IsType(LPCTSTR pszString, size_t nStart, size_t nLength, bool& bWord, bool& bDigit, bool& bMix)
{
	bWord = false;
	bDigit = false;
	for ( pszString += nStart ; *pszString && nLength ; pszString++, nLength-- )
	{
		if ( _istdigit( *pszString ) ) bDigit = true;
		else if ( IsCharacter( *pszString ) ) bWord = true;
	}

	bMix = bWord && bDigit;
	if ( bMix )
	{
		bWord = false;
		bDigit = false;
	}
}

// Use with whole numbers only
template <typename T>
inline T GetRandomNum(const T& min, const T& max)
{
	if ( theApp.m_hCryptProv != 0 )
	{
		T nRandom = 0;
		if ( CryptGenRandom( theApp.m_hCryptProv, sizeof( T ), (BYTE*)&nRandom ) )
			return min + (double)nRandom / ( (double)( static_cast< T >( -1 ) ) / ( max - min + 1 ) + 1 );
	}

	// Fallback to non-secure method
	return min + (double)rand() / ( (double)RAND_MAX / ( max - min + 1 ) + 1 );
}

template <>
inline __int8 GetRandomNum<__int8>(const __int8& min, const __int8& max)
{
	return (__int8)GetRandomNum<unsigned __int8>( min, max );
}

template <>
inline __int16 GetRandomNum<__int16>(const __int16& min, const __int16& max)
{
	return (__int16)GetRandomNum<unsigned __int16>( min, max );
}

template <>
inline __int32 GetRandomNum<__int32>(const __int32& min, const __int32& max)
{
	return (__int32)GetRandomNum<unsigned __int32>( min, max );
}

template <>
inline __int64 GetRandomNum<__int64>(const __int64& min, const __int64& max)
{
	return (__int64)GetRandomNum<unsigned __int64>( min, max );
}

// Log severity (log level)
#define MSG_SEVERITY_MASK		0x00ff
#define MSG_ERROR				0x0000
#define MSG_WARNING				0x0001
#define MSG_NOTICE				0x0002
#define MSG_INFO				0x0003
#define MSG_DEBUG				0x0004

// Log facility
#define MSG_FACILITY_MASK		0xff00
#define MSG_FACILITY_DEFAULT	0x0000
#define MSG_FACILITY_SEARCH		0x0100
#define MSG_FACILITY_INCOMING	0x0200
#define MSG_FACILITY_OUTGOING	0x0300

#define WM_WINSOCK		(WM_APP+101)
#define WM_VERSIONCHECK	(WM_APP+102)
#define WM_OPENCHAT		(WM_APP+103)
#define WM_TRAY			(WM_APP+104)
#define WM_URL			(WM_APP+105)
#define WM_SKINCHANGED	(WM_APP+106)
#define WM_COLLECTION	(WM_APP+107)
#define WM_OPENSEARCH	(WM_APP+108)
#define WM_LOG			(WM_APP+109)
#define WM_LIBRARYSEARCH (WM_APP+110)
#define WM_PLAYFILE		(WM_APP+111)
#define WM_ENQUEUEFILE	(WM_APP+112)
#define WM_SETALPHA		(WM_APP+113)
#define WM_METADATA		(WM_APP+114)
#define WM_SANITY_CHECK	(WM_APP+115)	// Run allsystem check against banned hosts

#define WM_AFX_SETMESSAGESTRING 0x0362
#define WM_AFX_POPMESSAGESTRING 0x0375
#define WM_IDLEUPDATECMDUI		0x0363

#define ID_PLUGIN_FIRST	27000
#define ID_PLUGIN_LAST	27999


#define THUMB_STORE_SIZE	128


// Client's name
#define CLIENT_NAME			"PeerProject"


// Network ID stuff

// 4 Character vendor code (used on G1, G2)
// BEAR, LIME, RAZA, RAZB, RZCB, etc
#define VENDOR_CODE			"PEER"

// ed2k client ID number.
// 0x2c (44) = Proposed PeerProject ID
// 0 = eMule, 1 = cDonkey, 4 = old Shareaza alpha/beta/mod/fork, 0x28 (40) = Shareaza, 0xcb (203) = ShareazaPlus RazaCB, etc
#define ED2K_CLIENT_ID			40

// 2 Character BT peer-id code
// PE = PeerProject, SZ = Shareaza, S~ = old Shareaza alpha/beta , CB = ShareazaPlus-RazaCB, AZ = Azureus, etc.
#define BT_ID1				'P'
#define BT_ID2				'E'

#define WEB_SITE			"http://PeerProject.org"
#define WEB_SITE_T			_T( WEB_SITE )


// Drag-n-drop stuff

#define MAX_DRAG_SIZE		256
#define MAX_DRAG_SIZE_2		(MAX_DRAG_SIZE/2)
#define DRAG_COLOR_KEY		(RGB(245,248,252))	// Light-blue
#define DRAG_HOVER_TIME		900 				// Dragging mouse press button after X ms

extern const LPCTSTR RT_BMP;
extern const LPCTSTR RT_JPEG;
extern const LPCTSTR RT_PNG;
extern const LPCTSTR RT_GZIP;