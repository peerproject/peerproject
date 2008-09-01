//
// StdAfx.h
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

//! \file       StdAfx.h
//! \brief      Standard header for prcompiled header feature.
//!
//! Includes MFC header files. Contains several global definitions.

#pragma once

//
// Configuration
//

#if 1

#if _MSC_VER >= 1400	// VS 2005 & VS 2008

// Warnings that are normally ON by default
#pragma warning ( disable : 4350 )	// (Level 1)	behavior change: 'member1' called instead of 'member2'
#pragma warning ( disable : 4351 )	// (Level 1)	new behavior: elements of array 'array' will be default initialized

#pragma warning ( disable : 4244 )	// (Level 2)	'argument' : conversion from 'type1' to 'type2', possible loss of data

#pragma warning ( disable : 4347 )	// (Level 4)	behavior change: 'function template' is called instead of 'function'
#pragma warning ( disable : 4512 )	// (Level 4)	'class' : assignment operator could not be generated

// Warnings that are normally OFF by default (enabled by using /Wall)
#pragma warning ( disable : 4264 )	// (Level 1)	'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
#pragma warning ( disable : 4555 )	// (Level 1)	expression has no effect; expected expression with side-effect
#pragma warning ( disable : 4711 )	// (Level 1)	function 'function' selected for inline expansion
#pragma warning ( disable : 4548 )	// (Level 1)	expression before comma has no effect; expected expression with side-effect

#pragma warning ( disable : 4191 )	// (Level 3)	'operator/operation' : unsafe conversion from 'type of expression' to 'type required'
#pragma warning ( disable : 4640 )	// (Level 3)	'instance' : construction of local static object is not thread-safe
#pragma warning ( disable : 4738 )	// (Level 3)	storing 32-bit float result in memory, possible loss of performance

#pragma warning ( disable : 4061 )	// (Level 4)	enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label
#pragma warning ( disable : 4263 )	// (Level 4)	'function' : member function does not override any base class virtual member function
#pragma warning ( disable : 4266 )	// (Level 4)	'function' : no override available for virtual member function from base 'type'; function is hidden
#pragma warning ( disable : 4365 )	// (Level 4)	'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
#pragma warning ( disable : 4514 )	// (Level 4)	'function' : unreferenced inline function has been removed
#pragma warning ( disable : 4571 )	// (Level 4)	Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
#pragma warning ( disable : 4625 )	// (Level 4)	'derived class' : copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning ( disable : 4626 )	// (Level 4)	'derived class' : assignment operator could not be generated because a base class assignment operator is inaccessible
#pragma warning ( disable : 4710 )	// (Level 4)	'function' : function not inlined
#pragma warning ( disable : 4820 )	// (Level 4)	'bytes' bytes padding added after construct 'member_name'

#define _SCL_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

// For detecting Memory Leaks
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#else	// VS 2003

// 64bit related - need to be fixed
#pragma warning ( disable : 4302 4311 4312 )
// general - fix where feasable then move to useless
#pragma warning ( disable : 4061 4127 4191 4244 4263 4264 4265 4296 4529 4548 4555 4640 4668 4686 4946 )
// copy/asignment-related
#pragma warning ( disable : 4511 4512 4625 4626 )
// behaviour change - check for regression
#pragma warning ( disable : 4347 )
// padding
#pragma warning ( disable : 4820 )
// useless
#pragma warning ( disable : 4217 4514 4619 4702 4710 4711 )

#endif

#endif

// Target features available from Windows XP onwards.
//	To show features that need guards for Windows 2000 compatability use:
//	#define NTDDI_VERSION	NTDDI_WIN2K
//	#define _WIN32_WINNT	0x0500
#define NTDDI_VERSION	NTDDI_WINXP	// Minimum build target
#define _WIN32_WINNT	0x0501		// Windows XP, 2003, Vista, 2008
#include <sdkddkver.h>				// Setup versioning for windows SDK/DDK

// Add defines missed/messed up when microsoft converted to NTDDI macros
#define WINXP			0x05010000		// rpcdce.h, rpcdcep.h
#define NTDDI_XP		0x05010000		// ipexport.h, iphlpapi.h
#define NTDDI_WXP		0x05010000		// rpcasync.h
#define NTDDI_XPSP1		0				// 0x05010100	// ipmib.h (leave as 0 due to broken struct)
#define NTDDI_XPSP2		0x05010200		// shellapi.h
#define NTDDI_WIN2K3	0				// 0x05020000	// docobj.h (leave as 0 due to broken enum)
#define NTDDI_WINLH		0x06000000		// objidl.h
#define NTDDK_VERSION	NTDDI_VERSION	// winioctl.h

#define VC_EXTRALEAN
#define _ATL_NO_COM_SUPPORT

#pragma warning( push, 0 )			// Suppress Microsoft warnings

//
// MFC
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxtempl.h>		// MFC templates
#include <afxmt.h>			// MFC threads
#include <afxole.h>			// MFC OLE
#include <afxocc.h>			// MDC OCC
#include <afxhtml.h>		// MFC HTML
#include <afxpriv.h>
#include <../src/mfc/afximpl.h>

//
// WIN32
//

#include <winsock2.h>		// Windows sockets V2
#include <wininet.h>		// Internet
#include <ddeml.h>			// DDE
#include <math.h>			// Math
#include <winsvc.h>			// Services (excluded by VC_EXTRALEAN)
#include <shlwapi.h>		// Windows Shell API
#include <exdispid.h>		// Internet Explorer DISPIDs
#include <mmsystem.h>		// Multimedia
#include <winioctl.h>		// Sparse files support
#include <atltime.h>		// Time classes
#include <atlenc.h>			// Base64Encode, UUEncode etc.

// If this header is not found, you'll need to install the Windows XP SP2 Platform SDK (or later)
// from http://www.microsoft.com/msdownload/platformsdk/sdkupdate/

#include <netfw.h>
#include <upnp.h>
#include <natupnp.h>
#include <iphlpapi.h>
#include <MsiQuery.h>
#include <MsiDefs.h>
#include <Powrprof.h>		// Power policy applicator

#if _MSC_VER < 1400 || _MSC_VER >= 1500
#include <intrin.h>
#endif

//
// STL
//

#include <vector>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <functional>
#include <algorithm>
#include <memory>
#include <iterator>
#include <limits>
#include <new>

//
// Boost
//

#ifndef _WIN64
	#define BOOST_BIND_ENABLE_STDCALL 1
	#define BOOST_MEM_FN_ENABLE_STDCALL 1
#endif

#include <boost/cstdint.hpp>
#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/type_traits.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/array.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/checked_delete.hpp>

//
// Standard headers
//

#include "../zlib/zlib.h"

#include "RegExp/regexpr2.h"

#include "MinMax.hpp"

#if _MSC_VER >= 1500				// Work-around for VC9 where a (pop) is
	#pragma warning( pop )			// ifdef'd out in stdio.h
#endif

#pragma warning( pop )				// Restore warnings

#include "augment/augment.hpp"
using augment::implicit_cast;
using augment::auto_ptr;
using augment::auto_array;
using augment::com_ptr;
using augment::IUnknownImplementation;

#include "Utility.hpp"
#include "Hashes.hpp"

#undef IDC_HAND		// Defined in Windows.h->WinUser.h and in Resource.h

#include "Resource.h"

#include "PeerProjectOM.h"

//
// Smaller type check fix (/RTCc)
//

#ifdef _DEBUG
	#undef GetRValue
	#define GetRValue(rgb)      ((BYTE)( (rgb)        & 0xff))
	#undef GetGValue
	#define GetGValue(rgb)      ((BYTE)(((rgb) >>  8) & 0xff))
	#undef GetBValue
	#define GetBValue(rgb)      ((BYTE)(((rgb) >> 16) & 0xff))
#endif

//
// Missing constants
//

#define BIF_NEWDIALOGSTYLE	0x0040
#define OFN_ENABLESIZING	0x00800000

// MFC changed resulttype of CWnd::OnNcHitTest method
#if _MSC_VER <= 1310
typedef UINT ONNCHITTESTRESULT;
// broken standard auto_ptr fix
#pragma warning ( disable : 4239 )
#else
typedef LRESULT ONNCHITTESTRESULT;
#endif

//
// 64-bit type
//

typedef unsigned __int64 QWORD;

#define	MAKEDWORD(a,b)	((DWORD) (((a)) | ((DWORD) ((b))) << 16))
#define	MAKEQWORD(a,b)	((QWORD) (((a)) | ((QWORD) ((b))) << 32))

inline CArchive& AFXAPI operator<<(CArchive& ar, const TRISTATE& n)
{
	int tmp = static_cast< int >( n );
	return ar << tmp;
}

inline CArchive& AFXAPI operator>>(CArchive& ar, TRISTATE& n)
{
	int tmp;
	ar >> tmp;
	n = static_cast< TRISTATE >( tmp );
	return ar;
}

#pragma pack( push, 1 )

typedef struct _ICONDIRENTRY
{
	BYTE        bWidth;          // Width, in pixels, of the image
	BYTE        bHeight;         // Height, in pixels, of the image
	BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
	BYTE        bReserved;       // Reserved ( must be 0)
	WORD        wPlanes;         // Color Planes
	WORD        wBitCount;       // Bits per pixel
	DWORD       dwBytesInRes;    // How many bytes in this resource?
	DWORD       dwImageOffset;   // Where in the file is this image?
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct _GRPICONDIRENTRY
{
	BYTE   bWidth;               // Width, in pixels, of the image
	BYTE   bHeight;              // Height, in pixels, of the image
	BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE   bReserved;            // Reserved
	WORD   wPlanes;              // Color Planes
	WORD   wBitCount;            // Bits per pixel
	DWORD  dwBytesInRes;         // how many bytes in this resource?
	WORD   nID;                  // the ID
} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;

typedef struct _ICONDIR
{
	WORD           idReserved;   // Reserved (must be 0)
	WORD           idType;       // Resource Type (1 for icons)
	WORD           idCount;      // How many images?
//	ICONDIRENTRY   idEntries[];  // An entry for each image (idCount of 'em)
} ICONDIR, *LPICONDIR;

#pragma pack( pop )

// Copied from GeoIP.h
typedef struct GeoIPTag {
  FILE *GeoIPDatabase;
  char *file_path;
	unsigned char *cache;
	unsigned char *index_cache;
	unsigned int *databaseSegments;
	char databaseType;
	time_t mtime;
	int flags;
	char record_length;
	int record_iter; /* used in GeoIP_next_record */
} GeoIP;

typedef enum {
	GEOIP_STANDARD = 0,
	GEOIP_MEMORY_CACHE = 1,
	GEOIP_CHECK_CACHE = 2,
	GEOIP_INDEX_CACHE = 4,
} GeoIPOptions;

typedef GeoIP* (*GeoIP_newFunc)(int);
typedef const char * (*GeoIP_country_code_by_addrFunc) (GeoIP*, const char *);
typedef const char * (*GeoIP_country_name_by_addrFunc) (GeoIP*, const char *);

const uint64 SIZE_UNKNOWN = ~0ull;

//
// Protocol IDs
//

enum PROTOCOLID
{
	PROTOCOL_ANY  = -1,
	PROTOCOL_NULL = 0,
	PROTOCOL_G1   = 1,
	PROTOCOL_G2   = 2,
	PROTOCOL_ED2K = 3,
	PROTOCOL_HTTP = 4,
	PROTOCOL_FTP  = 5,
	PROTOCOL_BT   = 6,
	PROTOCOL_KAD   = 7
};

struct ProtocolCmdIDMapEntry
{
	BYTE	protocol;
	DWORD	commandID;
};

const ProtocolCmdIDMapEntry protocolCmdMap[] =
{
	{ PROTOCOL_NULL, ID_NETWORK_NULL },
	{ PROTOCOL_G1, ID_NETWORK_G1 },
	{ PROTOCOL_G2, ID_NETWORK_G2 },
	{ PROTOCOL_ED2K, ID_NETWORK_ED2K },
	{ PROTOCOL_HTTP, ID_NETWORK_HTTP },
	{ PROTOCOL_FTP, ID_NETWORK_FTP },
	{ PROTOCOL_BT, ID_NETWORK_BT },
	{ PROTOCOL_KAD, ID_NETWORK_KAD }
};

inline PROTOCOLID& operator++(PROTOCOLID& arg)
{
	ASSERT( arg < PROTOCOL_KAD );
	arg = PROTOCOLID( arg + 1 );
	return arg;
}
inline PROTOCOLID& operator--(PROTOCOLID& arg)
{
	ASSERT( arg > PROTOCOL_ANY );
	arg = PROTOCOLID( arg - 1 );
	return arg;
}
inline CArchive& operator<<(CArchive& ar, const PROTOCOLID& rhs)
{
	int value = rhs;
	return ar << value;
};
inline CArchive& operator>>(CArchive& ar, PROTOCOLID& rhs)
{
	int value;
	ar >> value;
	if ( !( value >= PROTOCOL_ANY && value <= PROTOCOL_KAD ) )
		AfxThrowUserException();
	rhs = value >= PROTOCOL_ANY && value <= PROTOCOL_KAD
		? PROTOCOLID( value )
		: PROTOCOL_NULL;
	return ar;
};


class CQuickLock
{
public:
	explicit CQuickLock(CSyncObject& oMutex) : m_oMutex( oMutex ) { oMutex.Lock(); }
	~CQuickLock() { m_oMutex.Unlock(); }
private:
	CSyncObject& m_oMutex;
	CQuickLock(const CQuickLock&);
	CQuickLock& operator=(const CQuickLock&);
	static void* operator new(std::size_t);
	static void* operator new[](std::size_t);
	static void operator delete(void*);
	static void operator delete[](void*);
	CQuickLock* operator&() const;
};

template< class T >
class CGuarded
{
public:
	explicit CGuarded() : m_oSection(), m_oValue() { }
	explicit CGuarded(const CGuarded& other) : m_oSection(), m_oValue( other ) { }
	CGuarded(const T& oValue) : m_oSection(), m_oValue( oValue ) { }
	CGuarded& operator=(const T& oValue)
	{
		CQuickLock oLock( m_oSection );
		m_oValue = oValue;
		return *this;
	}
	operator T() const
	{
		CQuickLock oLock( m_oSection );
		return m_oValue;
	}
private:
	mutable CCriticalSection m_oSection;
	T m_oValue;
	CGuarded* operator&() const; // too unsafe
};

class CLowerCaseTable
{
public:
	explicit CLowerCaseTable();
	inline const TCHAR& operator()(const TCHAR cLookup) const { return cTable[ cLookup ]; }
	const CString& operator()(CString& strSource) const;
	inline const TCHAR& operator[](const TCHAR cLookup) const { return ( *this )( cLookup ); }
private:
	TCHAR cTable[ 65537 ];
};

extern const CLowerCaseTable ToLower;

#ifdef _DEBUG
	#define VERIFY_FILE_ACCESS(h,f) \
	{ \
		if ( ( h ) == INVALID_HANDLE_VALUE ) \
		{ \
			DWORD err = GetLastError(); \
			theApp.Message( MSG_DEBUG, _T("File error \"%s\": %s (0x%08x)"), \
				LPCTSTR( f ), LPCTSTR( GetErrorString( err ) ), err ); \
		} \
	}
#else
	#define VERIFY_FILE_ACCESS(h,f) ((void)0);
#endif

template<>
struct std::less< CLSID > : public std::binary_function< CLSID, CLSID, bool >
{
	inline bool operator()(const CLSID& _Left, const CLSID& _Right) const throw()
	{
		return _Left.Data1 < _Right.Data1 || ( _Left.Data1 == _Right.Data1 &&
			( _Left.Data2 < _Right.Data2 || ( _Left.Data2 == _Right.Data2 &&
			( _Left.Data3 < _Right.Data3 || ( _Left.Data3 == _Right.Data3 &&
			( memcmp( _Left.Data4, _Right.Data4, 8 ) < 0 ) ) ) ) ) );
	}
};

template<>
struct std::less< CString > : public std::binary_function< CString, CString, bool>
{
	inline bool operator()(const CString& _Left, const CString& _Right) const throw()
	{
		return ( _Left.CompareNoCase( _Right ) < 0 );
	}
};

typedef std::set < CString > string_set;

#define IsIn(x,y) ((x.find((y)))!=(x.end()))

inline UINT ReadArchive(CArchive& ar, void* lpBuf, const UINT nMax)
{
	UINT nReaded = ar.Read( lpBuf, nMax );
	if ( nReaded != nMax )
		AfxThrowArchiveException( CArchiveException::endOfFile );
	return nReaded;
}

// Produces two arguments divided by comma, where first argument is a string itself
// and second argument is a string length without null terminator
#define _P(x)	(x),((sizeof(x))/sizeof((x)[0])-1)
#define _PT(x)	_P(_T(x))

// The GetMicroCount function retrieves the number of microseconds that have elapsed
// since the application was started.
__int64 GetMicroCount();

// Produces the best hash table size for CMap::InitHashTable use
UINT GetBestHashTableSize(UINT nCount);

// Encode and decode URL text, and see if a string starts with a tag
CString URLEncode(LPCTSTR pszInput);                   // Encode "hello world" into "hello%20world"
CString URLDecode(LPCTSTR pszInput);                   // Decode "hello%20world" back to "hello world"
CString URLDecodeANSI(LPCTSTR pszInput);               // Decodes properly encoded URLs
CString URLDecodeUnicode(LPCTSTR pszInput);            // Decodes URLs with extended characters

// StartsWith("hello world", "hello") is true
inline BOOL StartsWith(const CString& sInput, LPCTSTR pszText, const int len)
{
	return ( sInput.GetLength() >= len ) && ! _tcsnicmp( (LPCTSTR)sInput, pszText, len );
}

// Compute average of values collected by specified time
template< class T, DWORD dwMilliseconds >
class CTimeAverage : boost::noncopyable
{
public:
	inline T operator()(T Val)
	{
		// Add new value
		DWORD dwNow = GetTickCount();
		m_Data.push_back( CAveragePair( Val, dwNow ) );

		// Remove outdated values
		while ( m_Data.size() > 1 )
		{
			if ( dwNow - (*(++m_Data.begin())).second < dwMilliseconds )
				break;
			m_Data.pop_front();
		}

		// Calculate average
		T sum = 0;
		for ( CAverageList::const_iterator i = m_Data.begin(); i != m_Data.end(); ++i )
			sum += (*i).first;
		return sum / m_Data.size();
	}

protected:
	typedef std::pair< T, DWORD > CAveragePair;
	typedef std::list< CAveragePair > CAverageList;
	CAverageList m_Data;
};

template< class T >
inline void SafeRelease(CComPtr< T >& pObj) throw()
{
	__try
	{
		pObj.Release();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		pObj.Detach();
	}
}

inline bool IsFileNewerThan(LPCTSTR pszFile, const QWORD nMilliseconds)
{
	WIN32_FILE_ATTRIBUTE_DATA fd = {};
	if ( ! GetFileAttributesEx( pszFile, GetFileExInfoStandard, &fd ) )
		return false;

	FILETIME ftNow = {};
	GetSystemTimeAsFileTime( &ftNow );

	if ( ( MAKEQWORD( ftNow.dwLowDateTime, ftNow.dwHighDateTime ) -
		10000ull * nMilliseconds ) > MAKEQWORD( fd.ftLastWriteTime.dwLowDateTime,
		fd.ftLastWriteTime.dwHighDateTime ) )
		return false;

	return true;
}
