//
// PeerProject.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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


class CMainWnd;
class CDatabase;
class CSplashDlg;
class CFontManager;
class CUPnPFinder;


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


class __declspec(novtable) CLogMessage
{
public:
	CLogMessage(WORD nType, const CString& strLog) :
		m_strLog( strLog ),
		m_nType( nType )
	{
	}
	CString m_strLog;
	WORD	m_nType;
};

typedef CList< CLogMessage* > CLogMessageList;


class CPeerProjectApp : public CWinApp
{
public:
	CPeerProjectApp();

	HANDLE				m_pMutex;
	CMutex				m_pSection;
	WORD				m_nVersion[4];
	BYTE				m_pBTVersion[4];			// PExx
	CString				m_sVersion;					// x.x.x.x
	CString				m_sVersionLong;				// x.x.x.x 32/64-bit (date rXXXX) Debug
	CString				m_sSmartAgent;				// PeerProject x.x.x.x
	CString				m_sBuildDate;
	CString				m_strBinaryPath;			// PeerProject.exe path
	BYTE				m_nFontQuality;				// ClearType DEFAULT_QUALITY
	CFont				m_gdiFont;
	CFont				m_gdiFontBold;
	CFont				m_gdiFontLine;
	CWnd*				m_pSafeWnd;
	volatile LONG		m_bBusy;					// PeerProject is busy
	volatile bool		m_bInteractive;				// PeerProject begins initialization
	volatile bool		m_bLive;					// PeerProject fully initialized
	volatile bool		m_bClosing;					// PeerProject begins closing
	bool				m_bIsServer;				// Is OS a Server version
	bool				m_bIsWin2000;				// Is OS Windows 2000
	bool				m_bIsVistaOrNewer;			// Is OS Vista/7 or newer
	bool				m_bLimitedConnections;		// Networking is limited (XP SP2)
	BOOL				m_bMenuWasVisible;			// For the menus in media player window
	DWORD				m_nWindowsVersion;			// Windows version
	DWORD				m_nWindowsVersionMinor;		// Windows minor version
	QWORD				m_nPhysicalMemory;			// Physical RAM installed
	CAutoPtr< CUPnPFinder > m_pUPnPFinder;
	TRISTATE			m_bUPnPPortsForwarded;		// UPnP values are assigned when the discovery is complete
	TRISTATE			m_bUPnPDeviceConnected;		// or when the service notifies
	IN_ADDR				m_nUPnPExternalAddress;		// UPnP current external address
	DWORD				m_nLastInput;				// Time of last input event (in secs)
	HHOOK				m_hHookKbd;
	HHOOK				m_hHookMouse;
	UINT				m_nMouseWheel;				// System-defined number of lines to move with mouse wheel

	SYSTEM_INFO			m_SysInfo;					// System Information (CPU cores, etc.)

	// Cryptography Context handle
	HCRYPTPROV			m_hCryptProv;

	// For themes functions
	HINSTANCE			m_hTheme;
	HRESULT		(WINAPI *m_pfnSetWindowTheme)(HWND, LPCWSTR, LPCWSTR);
	BOOL		(WINAPI *m_pfnIsThemeActive)(VOID);
	HANDLE		(WINAPI *m_pfnOpenThemeData)(HWND, LPCWSTR);
	HRESULT		(WINAPI *m_pfnCloseThemeData)(HANDLE);
	HRESULT		(WINAPI *m_pfnDrawThemeBackground)(HANDLE, HDC, int, int, const RECT*, const RECT*);
//	HRESULT		(WINAPI *m_pfnEnableThemeDialogTexture)(HWND, DWORD);
//	HRESULT		(WINAPI *m_pfnDrawThemeParentBackground)(HWND, HDC, RECT*);
//	HRESULT		(WINAPI *m_pfnGetThemeBackgroundContentRect)(HANDLE, HDC, int, int, const RECT*, RECT*);
//	HRESULT		(WINAPI *m_pfnDrawThemeText)(HANDLE, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const RECT*);
	HRESULT		(WINAPI *m_pfnGetThemeSysFont)(HTHEME, int, __out LOGFONTW*);

	// Kernel functions
	HRESULT		(WINAPI *m_pRegisterApplicationRestart)( __in_opt PCWSTR pwzCommandline, __in DWORD dwFlags );

	// Shell functions
	BOOL		(WINAPI *m_pfnAssocIsDangerous)(LPCWSTR);
	HINSTANCE	m_hShlWapi;
	HINSTANCE	m_hShell32;
	HRESULT		(WINAPI *m_pfnSHGetFolderPathW)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath);
	HRESULT		(WINAPI *m_pfnSHGetKnownFolderPath)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);

	// GeoIP - IP to Country lookup
	HINSTANCE	m_hGeoIP;
	GeoIP*		m_pGeoIP;
	typedef GeoIP* (*GeoIP_newFunc)(int);
	typedef void (*GeoIP_deleteFunc)(GeoIP* gi);
	typedef const char * (*GeoIP_country_code_by_ipnumFunc) (GeoIP* gi, unsigned long ipnum);
	typedef const char * (*GeoIP_country_name_by_ipnumFunc) (GeoIP* gi, unsigned long ipnum);
	GeoIP_country_code_by_ipnumFunc	m_pfnGeoIP_country_code_by_ipnum;
	GeoIP_country_name_by_ipnumFunc	m_pfnGeoIP_country_name_by_ipnum;
	GeoIP_deleteFunc	m_pfnGeoIP_delete;

	HINSTANCE			m_hLibGFL;

	HINSTANCE			CustomLoadLibrary(LPCTSTR);
	CMainWnd*			SafeMainWnd() const;
	BOOL				InternalURI(LPCTSTR pszURI);

	// Logging functions
	CLogMessageList		m_oMessages;	// Log temporary storage
	CCriticalSection	m_csMessage;	// m_oMessages guard
	bool				IsLogDisabled(WORD nType) const;
	void				Message(WORD nType, UINT nID, ...);
	void				Message(WORD nType, LPCTSTR pszFormat, ...);
	void				PrintMessage(WORD nType, const CString& strLog);
	void				LogMessage(const CString& strLog);
	void				SplashStep(LPCTSTR pszMessage = NULL, int nMax = 0, bool bClosing = false);
	void				ShowStartupText();

	CString 			GetCountryCode(IN_ADDR pAddress) const;
	CString 			GetCountryName(IN_ADDR pAddress) const;

	//	CFontManager*	m_pFontManager;

	// Open file or url. Returns NULL always.
	virtual CDocument*	OpenDocumentFile(LPCTSTR lpszFileName);
	// Open file or url (generic function)
	static BOOL			Open(LPCTSTR lpszFileName);
	// Open .lnk file
	static BOOL			OpenShellShortcut(LPCTSTR lpszFileName);
	// Open .url file
	static BOOL			OpenInternetShortcut(LPCTSTR lpszFileName);
	// Open .torrent file
	static BOOL			OpenTorrent(LPCTSTR lpszFileName);
	// Open .co, .collection, or .emulecollection file
	static BOOL			OpenCollection(LPCTSTR lpszFileName);
	// Open .met or .dat file
	static BOOL			OpenMET(LPCTSTR lpszFileName);
	// Open url
	static BOOL			OpenURL(LPCTSTR lpszFileName, BOOL bSilent = FALSE);

	CString				GetWindowsFolder() const;
	CString				GetProgramFilesFolder() const;
	CString				GetDocumentsFolder() const;
	CString				GetDownloadsFolder() const;
	CString				GetAppDataFolder() const;
	CString				GetLocalAppDataFolder() const;

	CDatabase*			GetDatabase(bool bGeneral = false) const;	// Get SQLite thumbnail database handler, must be freed by "delete" operator.

	// pszTarget: 0 = delete file, 1 = release file.
	void				OnRename(LPCTSTR strSource, LPCTSTR pszTarget = (LPCTSTR)1);

protected:
	CSplashDlg*					m_dlgSplash;
	CPeerProjectCommandLineInfo	m_ocmdInfo;

	virtual BOOL		InitInstance();
	virtual int			ExitInstance();
	virtual void		WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

	void				GetVersionNumber();
	void				InitResources();

	void				LoadCountry();		// Load the GeoIP library for mapping IPs to countries
	void				FreeCountry();		// Free GeoIP resources

	DECLARE_MESSAGE_MAP()

private:
	CPeerProjectApp(const CPeerProjectApp&);
	CPeerProjectApp& operator=(const CPeerProjectApp&);
};

extern CPeerProjectApp theApp;

//
// Utility Functions
//

// Post message to main window in safe way
BOOL PostMainWndMessage(UINT Msg, WPARAM wParam = NULL, LPARAM lParam = NULL);

CRuntimeClass* AfxClassForName(LPCTSTR pszClass);

BOOL LoadString(CString& str, UINT nID);
CString LoadString(UINT nID);
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
BOOL LoadIcon(LPCTSTR szFilename, HICON* phSmallIcon, HICON* phLargeIcon, HICON* phHugeIcon, int nIcon = 0);
// Load 16x16 icon from module pointed by its CLSID
//HICON LoadCLSIDIcon(LPCTSTR szCLSID);
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

// Generate safe file name for file system (bPath == true - allow path i.e. "\" symbol)
CString SafeFilename(CString strName, bool bPath = false);

// Create directory. If one or more of the intermediate folders do not exist, they are created as well.
BOOL CreateDirectory(LPCTSTR szPath);

// Delete file to Recycle Bin, etc.
BOOL DeleteFileEx(LPCTSTR szFileName, BOOL bShared, BOOL bToRecycleBin, BOOL bEnableDelayed);
void DeleteFiles(CStringList& pList);	// User confirmation
void PurgeDeletes();					// Delete postponed file

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

// Do message loop
void SafeMessageLoop();

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
	bool operator()(WORD lhs, WORD rhs) const
	{
		return lhs > rhs;
	}
};

bool IsCharacter(WCHAR nChar);
bool IsHiragana(WCHAR nChar);
bool IsKatakana(WCHAR nChar);
bool IsKanji(WCHAR nChar);
bool IsWord(LPCTSTR pszString, size_t nStart, size_t nLength);
void IsType(LPCTSTR pszString, size_t nStart, size_t nLength, bool& bWord, bool& bDigit, bool& bMix);

class CLowerCaseTable
{
public:
	explicit CLowerCaseTable();
	TCHAR operator()(TCHAR cLookup) const;
	CString& operator()(CString& strSource) const;
private:
	TCHAR cTable[ 65536 ];
};

extern const CLowerCaseTable ToLower;

// Use with whole numbers only
template <typename T>
inline T GetRandomNum(const T& min, const T& max)
{
	if ( theApp.m_hCryptProv != 0 )
	{
		T nRandom = 0;
		if ( CryptGenRandom( theApp.m_hCryptProv, sizeof( T ), (BYTE*)&nRandom ) )
			return static_cast< T >( (double)nRandom  * ( (double)max - (double)min + 1 ) / ( (double)static_cast< T >( -1 ) + 1 ) + min );
	}

	// Fallback to non-secure method
	return static_cast< T >( (double)rand() * ( max - min + 1 ) / ( (double)RAND_MAX + 1 ) + min );
}

template <>
__int8 GetRandomNum<__int8>(const __int8& min, const __int8& max);

template <>
__int16 GetRandomNum<__int16>(const __int16& min, const __int16& max);

template <>
__int32 GetRandomNum<__int32>(const __int32& min, const __int32& max);

template <>
__int64 GetRandomNum<__int64>(const __int64& min, const __int64& max);


// Log severity (log level)
#define MSG_SEVERITY_MASK		0x000f
#define MSG_ERROR				0x0000
#define MSG_WARNING				0x0001
#define MSG_NOTICE				0x0002
#define MSG_INFO				0x0003
#define MSG_DEBUG				0x0004
#define MSG_TRAY				0x0010			// Show message in system tray popup

// Log facility
#define MSG_FACILITY_MASK		0xff00
#define MSG_FACILITY_DEFAULT	0x0000
#define MSG_FACILITY_SEARCH		0x0100
#define MSG_FACILITY_INCOMING	0x0200
#define MSG_FACILITY_OUTGOING	0x0300


// Event Messages
#define WM_WINSOCK				(WM_APP+101)	// Winsock messages proxy to Network object (Used by WSAAsyncGetHostByName() function)
#define WM_VERSIONCHECK			(WM_APP+102)	// Version check (WAPARM: VERSION_CHECK nCode, LPARAM: unused)
#define WM_OPENCHAT				(WM_APP+103)	// Open chat window (WAPARM: CChatSession* pChat, LPARAM: unused)
#define WM_TRAY					(WM_APP+104)	// Tray icon notification (WPARAM: unused, LPARAM: uMouseMessage)
#define WM_URL					(WM_APP+105)	// Open URL (WPARAM: CShareazaURL* pURL, LPARAM: unused)
#define WM_SKINCHANGED			(WM_APP+106)	// Skin change (WPARAM: unused, LPARAM: unused)
#define WM_COLLECTION			(WM_APP+107)	// Open collection file (WPARAM: unused, LPARAM: LPTSTR szFilename)
#define WM_OPENSEARCH			(WM_APP+108)	// Open new search (WPARAM: CQuerySearch* pSearch, LPARAM: unused)
#define WM_LIBRARYSEARCH		(WM_APP+110)	// Start file library search (WPARAM: LPTSTR pszSearch, LPARAM: unused)
#define WM_PLAYFILE				(WM_APP+111)	// Play file by media system (WPARAM: unused, LPARAM: CString* pFilename)
#define WM_ENQUEUEFILE			(WM_APP+112)	// Enqueue file to media system (WPARAM: unused, LPARAM: CString* pFilename)
#define WM_SETALPHA				(WM_APP+113)	// Increase/decrease main window transparency (WPARAM: 0 - to decrease or 1 - to increase, LPARAM: unused)
#define WM_METADATA				(WM_APP+114)	// Set/clear library metapanel data & status message (WPARAM: CMetaPanel* pPanelData, LPARAM: LPCTSTR pszMessage)
#define WM_SANITY_CHECK			(WM_APP+115)	// Run allsystem check against banned hosts (WPARAM: unused, LPARAM: unused)
#define WM_NOWUPLOADING			(WM_APP+117)	// New upload notification (WPARAM: unused, LPARAM: CString* pFilename)
#define WM_TORRENT				(WM_APP+118)	// Open torrent file ( WPARAM: LPTSTR szFilename, LPARAM: unused )

#define ID_PLUGIN_FIRST			27000
#define ID_PLUGIN_LAST			27999

#define HTTP_HEADER_MAX_LINE	(256 * 1024)	// Maximum allowed size of single HTTP-header line (256 Kb)

// Drag-n-drop stuff
#define MAX_DRAG_SIZE			256
#define MAX_DRAG_SIZE_2			(MAX_DRAG_SIZE/2)
#define DRAG_COLOR_KEY			(RGB(245,248,252)) // Light-blue	ToDo: Skinning?
#define DRAG_HOVER_TIME			900 			// Dragging mouse button press after X ms

// Set Default Sizes in Pixels					// Skinnable Options:
//#define PANEL_WIDTH			200				// Skin.m_nSidebarWidth
//#define TOOLBAR_HEIGHT		28				// Skin.m_nToolbarHeight
#define THUMB_STORE_SIZE		128


// Network ID's:

// Client's name
#define CLIENT_NAME				_T("PeerProject")

// 4 Character vendor code (G1,G2)
// PEER, RAZA, RAZB, BEAR, LIME
#define VENDOR_CODE				"PEER"

// ed2k client ID number.
// 80 (0x50) = Proposed PeerProject ID
// 0 = eMule, 1 = cDonkey, 4 = old Shareaza, 40 (0x28) = Shareaza
#define ED2K_CLIENT_ID			80

// 2 Character BT peer-id code: -PE1000-
// PE = PeerProject, SZ = Shareaza, S~ = old Shareaza
#define BT_ID1					'P'
#define BT_ID2					'E'

// Locations
#define WEB_SITE				_T("http://PeerProject.org/")
#define UPDATE_URL				_T("http://peerproject.sourceforge.net/update")
#define REGISTRY_KEY			_T("Software\\PeerProject\\PeerProject")


extern const LPCTSTR RT_BMP;
extern const LPCTSTR RT_PNG;
extern const LPCTSTR RT_JPEG;
extern const LPCTSTR RT_GZIP;
