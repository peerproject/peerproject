//
// StdAfx.h
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

//! \file	StdAfx.h
//! \brief	Standard header for prcompiled header feature.
//!
//! Includes MFC header files. Contains several global definitions.

#pragma once

//
// Configuration
//

#if 1

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

#endif	//1

// Target features available from Windows Vista onwards.
// To find features that need guards for Windows 2000/XP temporarily use:
//	#define NTDDI_VERSION	NTDDI_WIN2K
//	#define _WIN32_WINNT	0x0500
#define NTDDI_VERSION	NTDDI_LONGHORN	// Minimum build target Vista
#define _WIN32_WINNT	0x0600			// Vista, 2008
#include <sdkddkver.h>					// Setup versioning for Windows SDK

// Add defines missed/messed up when Microsoft converted to NTDDI macros
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
#define BOOST_USE_WINDOWS_H

#pragma warning( push, 0 )	// Suppress Microsoft warnings

//
// MFC
//

#include <afxwin.h>			// MFC core and standard components
#include <afxext.h>			// MFC extensions
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
#include <wincrypt.h>		// Cryptographic API
#include <ddeml.h>			// DDE
#include <math.h>			// Math
#include <winsvc.h>			// Services (excluded by VC_EXTRALEAN)
#include <shlwapi.h>		// Windows Shell API
#include <exdispid.h>		// Internet Explorer DISPIDs
#include <mmsystem.h>		// Multimedia
#include <winioctl.h>		// Sparse files support
#include <atltime.h>		// Time classes
#include <atlenc.h>			// Base64Encode, UUEncode etc.
#include <atlfile.h>		// Thin file classes

// If header is not found, install Windows SDK from microsoft.com
// ( Vista SDK (6.0) or later -previously XP Platform SDK )

#include <netfw.h>
#include <upnp.h>
#include <natupnp.h>
#include <iphlpapi.h>
#include <MsiQuery.h>
#include <MsiDefs.h>
#include <Powrprof.h>		// Power policy applicator

// Intrinsics  (Workaround for Microsoft double declaration with Visual Studio 2005)
#if defined(_MSC_VER) && (_MSC_VER < 1500)
	#define _interlockedbittestandset _ms_set
	#define _interlockedbittestandreset _ms_reset
	#define _interlockedbittestandset64 _ms_set64
	#define _interlockedbittestandreset64 _ms_reset64
	#include <intrin.h>
	#undef _interlockedbittestandset
	#undef _interlockedbittestandreset
	#undef _interlockedbittestandset64
	#undef _interlockedbittestandreset64
#else
	#include <intrin.h>
#endif


//
// STL
//

#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <new>
#include <queue>
#include <deque>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

//#include <memory>		// ?


//
// TR1 (std::tr1::)
//
// ToDo: See Shareaza r8451 for tr1 implementation

//#if defined(_MSC_VER) && (_MSC_FULL_VER > 150030000)	// VS2008 SP1 for tr1
//  #include <type_traits>	// In MinMax.hpp
  //#include <array>
  //#include <memory>
  //#include <regex>
//#else	// Boost fallback
// #include <Boost/tr1/type_traits.hpp>
  //#include <Boost/tr1/array.hpp>
  //#include <Boost/tr1/memory.hpp>
  //#include <Boost/tr1/utility.hpp>
//#endif


//
// Boost
//

#ifndef _WIN64
	#define BOOST_BIND_ENABLE_STDCALL 1
	#define BOOST_MEM_FN_ENABLE_STDCALL 1
#endif

// BOOST_STATIC_ASSERT(false) for compile-time checks below VS2010
#include <Boost/static_assert.hpp>

#include <Boost/bind.hpp>
#include <Boost/shared_ptr.hpp>
#include <Boost/ptr_container/ptr_list.hpp>	// For noncopyable

//#include <Boost/array.hpp>			// In Hashes/HashDescriptors.hpp
//#include <Boost/type_traits.hpp>		// In MinMax.hpp
//#include <Boost/checked_delete.hpp>	// In Augment/auto_ptr.hpp
//#include <Boost/cstdint.hpp>			// ?
//#include <Boost/utility.hpp>			// ?
//#include <Boost/bind/placeholders.hpp>


//
// Standard headers
//

#include <zlib/zlib.h>

#include <RegExp/RegExpr2.h>

#include "MinMax.hpp"

#if _MSC_VER >= 1500 && _MSC_VER < 1600		// Work-around for VC9 (VS2005) where
	#pragma warning( pop )					// a (pop) is ifdef'd out in stdio.h
#endif

#pragma warning( pop )						// Restore warnings

#include <Augment/Augment.hpp>
using augment::implicit_cast;
using augment::auto_ptr;
using augment::auto_array;
using augment::IUnknownImplementation;

#include "../HashLib/HashLib.h"

// GeoIP (geolite.maxmind.com)
#include "../Services/GeoIP/GeoIP.h"

// BugTrap (www.intellesoft.net)
#ifdef _DEBUG
	#include "../Services/BugTrap/BugTrap.h"
#endif


//typedef CString StringType;		// Previousy for <Hashes>

// Obsolete for custom StringType:
//! \brief Hash function needed for CMap with const CString& as ARG_KEY.
//template<> AFX_INLINE UINT AFXAPI HashKey(const CString& key)
//{
//	UINT nHash = 0;
//	LPCTSTR pKey = key.GetString();
//	while ( *pKey )
//		nHash = ( nHash << 5 ) + nHash + *pKey++;
//	return nHash;
//}
//! \brief Hash function needed for CMap with CString& as ARG_KEY.
//template<> AFX_INLINE UINT AFXAPI HashKey(CString& key)
//{
//	return HashKey< const CString& >( key );
//}
//! \brief Hash function needed for CMap with CString as ARG_KEY.
//template<> AFX_INLINE UINT AFXAPI HashKey(CString key)
//{
//	return HashKey< const CString& >( key );
//}
//! \brief Hash function needed for CMap with DWORD_PTR as ARG_KEY.

//! While the default hash function could be used, this one does not generate (false) 64 bit warnings.
//! \brief Hash function needed for CMap with const CString& as ARG_KEY.
template<> AFX_INLINE UINT AFXAPI HashKey(const CString& key)
{
	return HashKey<LPCTSTR>( key );
}

template<> AFX_INLINE UINT AFXAPI HashKey(DWORD_PTR key)
{
	return static_cast< UINT >( key >> 4 );
}

template<> AFX_INLINE BOOL AFXAPI CompareElements(const IN_ADDR* pElement1, const IN_ADDR* pElement2)
{
	return pElement1->s_addr == pElement2->s_addr;
}

template<> AFX_INLINE UINT AFXAPI HashKey(const IN_ADDR& key)
{
	return key.s_addr;
}

#include "Hashes.hpp"

#undef IDC_HAND		// Defined in Windows.h->WinUser.h (then in Resource.h)

#include "Resource.h"

#include "PeerProjectOM.h"

//
// Smaller type check fix (/RTCc)
//

#ifdef _DEBUG
	#undef GetRValue
	#define GetRValue(rgb)	((BYTE)( (rgb)        & 0xff))
	#undef GetGValue
	#define GetGValue(rgb)	((BYTE)(((rgb) >>  8) & 0xff))
	#undef GetBValue
	#define GetBValue(rgb)	((BYTE)(((rgb) >> 16) & 0xff))
#endif

//
// Missing constants
//

#ifndef BIF_NEWDIALOGSTYLE
	#define BIF_NEWDIALOGSTYLE	0x00000040
#endif
#ifndef OFN_ENABLESIZING
	#define OFN_ENABLESIZING	0x00800000
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
	BYTE	bWidth; 		// Width, in pixels, of the image
	BYTE	bHeight;		// Height, in pixels, of the image
	BYTE	bColorCount;	// Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;		// Reserved ( must be 0)
	WORD	wPlanes;		// Color Planes
	WORD	wBitCount;		// Bits per pixel
	DWORD	dwBytesInRes;	// How many bytes in this resource?
	DWORD	dwImageOffset;	// Where in the file is this image?
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct _GRPICONDIRENTRY
{
	BYTE   bWidth;			// Width, in pixels, of the image
	BYTE   bHeight;			// Height, in pixels, of the image
	BYTE   bColorCount;		// Number of colors in image (0 if >=8bpp)
	BYTE   bReserved;		// Reserved
	WORD   wPlanes;			// Color Planes
	WORD   wBitCount;		// Bits per pixel
	DWORD  dwBytesInRes;	// how many bytes in this resource?
	WORD   nID; 			// the ID
} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;

typedef struct _ICONDIR
{
	WORD	idReserved;		// Reserved (must be 0)
	WORD	idType;			// Resource Type (1 for icons)
	WORD	idCount;		// How many images?
//	ICONDIRENTRY   idEntries[];	// An entry for each image (idCount of 'em)
} ICONDIR, *LPICONDIR;

#pragma pack( pop )


// GEOIP Obsolete, from #included GeoIP.h
//typedef struct GeoIPTag {
//	FILE *GeoIPDatabase;
//	char *file_path;
//	unsigned char *cache;
//	unsigned char *index_cache;
//	unsigned int *databaseSegments;
//	char databaseType;
//	time_t mtime;
//	int flags;
//	char record_length;
//	int record_iter; /* for GeoIP_next_record */
//} GeoIP;

//typedef enum {
//	GEOIP_STANDARD = 0,
//	GEOIP_MEMORY_CACHE = 1,
//	GEOIP_CHECK_CACHE = 2,
//	GEOIP_INDEX_CACHE = 4,
//} GeoIPOptions;

//typedef GeoIP* (*GeoIP_newFunc)(int);
//typedef const char * (*GeoIP_country_code_by_addrFunc) (GeoIP*, const char *);
//typedef const char * (*GeoIP_country_name_by_addrFunc) (GeoIP*, const char *);


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
	PROTOCOL_KAD  = 7
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
	CGuarded* operator&() const; // Too unsafe
};


typedef boost::shared_ptr< CCriticalSection > CCriticalSectionPtr;

template< typename T, typename L >
class CLocked
{
public:
	CLocked(const CLocked& pGB) :
		m_oValue( pGB.m_oValue ),
		m_oLock( pGB.m_oLock )
	{
		m_oLock->Lock();
	}

	CLocked(T oValue, L oLock) :
		m_oValue( oValue ),
		m_oLock( oLock )
	{
		m_oLock->Lock();
	}

	~CLocked()
	{
		m_oLock->Unlock();
	}

	operator T() const throw()
	{
		return m_oValue;
	}

	T operator->() const throw()
	{
		return m_oValue;
	}

private:
	T	m_oValue;
	L	m_oLock;

	CLocked* operator&() const;
	CLocked& operator=(const CLocked&);
};

#ifdef _DEBUG

	// Assume we already entered to this lock
	#define ASSUME_LOCK(lock) \
	if ( (lock).m_nEnterCount < 1 || (lock).m_nThreadId != (LONG)GetCurrentThreadId() ) { \
		static char BUF[1024] = {}; \
		lstrcpyA(BUF,THIS_FILE); \
		lstrcatA(BUF,"\n\nThis code must be protected by " #lock "!"); \
		if ( ::AfxAssertFailedLine(BUF, __LINE__) ) AfxDebugBreak(); }

	// Assume we already entered to this lock only once
	#define ASSUME_SINGLE_LOCK(lock) \
	if ( (lock).m_nEnterCount != 1 || (lock).m_nThreadId != (LONG)GetCurrentThreadId() ) { \
		static char BUF[1024] = {}; \
		lstrcpyA(BUF,THIS_FILE); \
		lstrcatA(BUF,"\n\nThis code must be protected by " #lock "!"); \
		if ( ::AfxAssertFailedLine(BUF, __LINE__) ) AfxDebugBreak(); }

	class CMutexEx : public CMutex
	{
	public:
		CMutexEx(BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL)
			: CMutex( bInitiallyOwn, lpszName, lpsaAttribute )
			, m_nThreadId( 0 )
			, m_nEnterCount( 0 )
		{
		}

		virtual BOOL Lock(DWORD dwTimeout = INFINITE)
		{
			if ( CMutex::Lock( dwTimeout ) )
			{
				InterlockedIncrement( &m_nEnterCount );
				InterlockedCompareExchange( &m_nThreadId, (LONG)GetCurrentThreadId(), 0 );
				return TRUE;
			}
			else
				return FALSE;
		}

		virtual BOOL Unlock()
		{
			if ( m_nThreadId && InterlockedDecrement( &m_nEnterCount ) == 0 )
				InterlockedExchange( &m_nThreadId, 0 );
			return CMutex::Unlock();
		}

		volatile LONG m_nThreadId;		// Owner thread
		volatile LONG m_nEnterCount;	// Re-enter counter
	};

#else	// No DEBUG

	#define ASSUME_LOCK(lock) ((void)0)
	#define ASSUME_SINGLE_LOCK(lock) ((void)0)
	typedef CMutex CMutexEx;

#endif	// _DEBUG

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

// Encode Unicode text to UTF-8 text
CStringA UTF8Encode(__in const CStringW& strInput);
CStringA UTF8Encode(__in_bcount(nInput) LPCWSTR psInput, __in int nInput);

// Decode UTF-8 text to Unicode text
CStringW UTF8Decode(__in const CStringA& strInput);
CStringW UTF8Decode(__in_bcount(nInput) LPCSTR psInput, __in int nInput);

// Encode and decode URL text, and see if a string starts with a tag
CString URLEncode(LPCTSTR pszInput);			// Encode "hello world" into "hello%20world"
CString URLDecode(LPCTSTR pszInput);			// Decode "hello%20world" back to "hello world"
CString URLDecodeANSI(LPCTSTR pszInput);		// Decodes properly encoded URLs
CString URLDecodeUnicode(LPCTSTR pszInput);		// Decodes URLs with extended characters

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
		return sum / (T)m_Data.size();
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

inline QWORD GetFileSize(LPCTSTR pszFile)
{
	WIN32_FILE_ATTRIBUTE_DATA fd = {};
	if ( GetFileAttributesEx( pszFile, GetFileExInfoStandard, &fd ) )
		return MAKEQWORD( fd.nFileSizeLow, fd.nFileSizeHigh );
	else
		return SIZE_UNKNOWN;
}

// Powered version of AfxMessageBox()
// nType				| *pnDefault
// MB_OK				| 0 - ask, 1 - IDOK
// MB_OKCANCEL			| 0 - ask, 1 - IDOK, 2 - IDCANCEL
// MB_ABORTRETRYIGNORE	| 0 - ask, 1 - IDABORT, 2 - IDRETRY, 3 - IDIGNORE
// MB_YESNOCANCEL		| 0 - ask, 1 - IDNO, 2 - IDYES, 3 - IDCANCEL
// MB_YESNO				| 0 - ask, 1 - IDNO, 2 - IDYES
// MB_RETRYCANCEL		| 0 - ask, 1 - IDRETRY, 2 - IDCANCEL
// MB_CANCELTRYCONTINUE	| 0 - ask, 1 - IDCANCEL, 2 - IDTRYAGAIN, 3 - IDCONTINUE
INT_PTR MsgBox(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0, DWORD* pnDefault = NULL);
INT_PTR MsgBox(UINT nIDPrompt, UINT nType = MB_OK, UINT nIDHelp = 0, DWORD* pnDefault = NULL);
#undef AfxMessageBox
#define AfxMessageBox MsgBox
