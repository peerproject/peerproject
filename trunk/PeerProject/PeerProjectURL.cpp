//
// PeerProjectURL.cpp
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

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "PeerProjectURL.h"
#include "Transfer.h"
#include "QuerySearch.h"
#include "DiscoveryServices.h"
#include "Network.h"
#include "BTInfo.h"
#include "Skin.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

#define REG_NUMBER(x) REG_DWORD,((LPBYTE)&(x)),(sizeof(DWORD))
#define REG_STRING(x) REG_SZ,((LPBYTE)(LPCTSTR)(x)),((DWORD)(sizeof(TCHAR)*(_tcslen((LPCTSTR)(x))+1)))

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL construction

CPeerProjectURL::CPeerProjectURL(LPCTSTR pszURL)
	: m_nProtocol		( PROTOCOL_NULL )
	, m_nAction			( uriNull )
	, m_pTorrent		( )
	, m_pAddress		( )
	, m_nPort			( 0 )
	, m_pServerAddress	( )
	, m_nServerPort		( 0 )
	, m_bSize			( FALSE )
{
	if ( pszURL != NULL ) Parse( pszURL );
}

CPeerProjectURL::CPeerProjectURL(CBTInfo* pTorrent)
	: CPeerProjectFile( static_cast< const CPeerProjectFile& >( *pTorrent ) )
	, m_nProtocol		( PROTOCOL_NULL )
	, m_nAction			( uriDownload )
	, m_pTorrent		( pTorrent )
	, m_pAddress		( )
	, m_nPort			( 0 )
	, m_pServerAddress	( )
	, m_nServerPort		( 0 )
	, m_bSize			( TRUE )
{
}

CPeerProjectURL::CPeerProjectURL(const CPeerProjectURL& pURL)
	: CPeerProjectFile( static_cast< const CPeerProjectFile& >( pURL ) )
	, m_nProtocol		( pURL.m_nProtocol )
	, m_nAction			( pURL.m_nAction )
	, m_pTorrent		( pURL.m_pTorrent )
	, m_sAddress		( pURL.m_sAddress )
	, m_pAddress		( pURL.m_pAddress )
	, m_nPort			( pURL.m_nPort )
	, m_pServerAddress	( pURL.m_pServerAddress )
	, m_nServerPort		( pURL.m_nServerPort )
	, m_bSize			( pURL.m_bSize )
	, m_oBTC			( pURL.m_oBTC )
	, m_sLogin			( pURL.m_sLogin )
	, m_sPassword		( pURL.m_sPassword )
{
}

CPeerProjectURL::~CPeerProjectURL()
{
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL clear

void CPeerProjectURL::Clear()
{
	// CPeerProjectFile
	m_nSize	= SIZE_UNKNOWN;
	m_sName.Empty();
	m_oSHA1.clear();
	m_oTiger.clear();
	m_oMD5.clear();
	m_oED2K.clear();
	m_oBTH.clear();
	m_sURL.Empty();
	m_sPath.Empty();

	// CPeerProjectURL
	m_nProtocol				= PROTOCOL_NULL;
	m_nAction				= uriNull;
	m_pTorrent.Free();
	m_sAddress.Empty();
	m_pAddress.s_addr		= 0;
	m_nPort					= 0;	// protocolPorts[ PROTOCOL_G2 ]
	m_pServerAddress.s_addr = 0;
	m_nServerPort			= 0;
	m_bSize					= FALSE;
	m_sLogin.Empty();
	m_sPassword.Empty();
	m_oBTC.clear();
}

//////////////////////////////////////////////////////////////////////
// Parse URL list

BOOL CPeerProjectURL::Parse(const CString& sText, CList< CString >& pURLs, BOOL bResolve)
{
	pURLs.RemoveAll();

	// Split text to reverse string list
	CList< CString > oReverse;

	int curPos = 0;
	CString strPart;
	while ( ( strPart = sText.Tokenize( _T("\n"), curPos ) ).GetLength() )
	{
		oReverse.AddHead( strPart.Trim( _T("\r\n\t >< ") ) );		// Second space is #160
	}

	CString strBuf;
	for ( POSITION pos = oReverse.GetHeadPosition() ; pos ; )
	{
		CString strLine( oReverse.GetNext( pos ) );
		if ( strLine.IsEmpty() )
		{
			// Empty strings breaks URL
			strBuf.Empty();
		}
		else
		{
			// Append new line to current URL and parse
			strBuf.Insert( 0, strLine );
			if ( Parse( strBuf, bResolve ) )
			{
				// OK, new URL found
				pURLs.AddTail( strBuf );
				strBuf.Empty();
			}
		}
	}

	return ! pURLs.IsEmpty();
}

//////////////////////////////////////////////////////////////////////
// Parse single URL

BOOL CPeerProjectURL::Parse(LPCTSTR pszURL, BOOL bResolve)
{
	// Parse "good" URL, and retry "bad" URL
	return ( ParseRoot( pszURL, bResolve ) || ParseRoot( URLDecode( pszURL ), bResolve ) );
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL root parser

BOOL CPeerProjectURL::ParseRoot(LPCTSTR pszURL, BOOL bResolve)
{
	CString strRoot( pszURL );
	const int nRoot = strRoot.Find( _T(":") ) + 1;
	if ( nRoot < 3 ) return FALSE;
	strRoot = strRoot.Left( nRoot ).MakeLower();

	//CString strPart = SkipSlashes( pszURL, nRoot );
	//if ( strPart.GetLength() < 4 ) return FALSE;

	SwitchMap( Root )
	{
		Root[ L"http:" ] 	= 'h';
		Root[ L"https:" ]	= 'h';
		Root[ L"ftp:" ]		= 'f';
		Root[ L"btc:" ]		= 'b';
		Root[ L"magnet:" ]	= 'm';
		Root[ L"ed2k:" ]	= 'e';
		Root[ L"ed2kftp:" ]	= 'k';
		Root[ L"peerproject:" ] = 'g';
		Root[ L"shareaza:" ] = 'g';
		Root[ L"gnutella:" ] = 'g';
		Root[ L"gnet:" ] 	= 'g';
		Root[ L"g2:" ]		= '2';
		Root[ L"gwc:" ]		= 'u';
		Root[ L"uhc:" ]		= 'u';
		Root[ L"ukhl:" ]	= 'u';
		Root[ L"gnutella1:" ] = 'u';
		Root[ L"gnutella2:" ] = 'u';
		Root[ L"mp2p:" ]	= 'p';
		Root[ L"adc:" ] 	= 'd';
		Root[ L"dchub:" ]	= 'd';
		Root[ L"dcfile:" ]	= 'c';
		Root[ L"foxy:" ]	= 'x';
		Root[ L"irc:" ]		= 'i';
	}

	switch ( Root[ strRoot ] )
	{
	case 'h':	// http:// https://
		return ParseHTTP( pszURL, bResolve );
	case 'f':	// ftp://
		return ParseFTP( pszURL, bResolve );
	case 'm':	// magnet:?
		if ( _tcsnicmp( pszURL, _T("magnet:?"), 8 ) == 0 )
		{
			pszURL += 8;
			return ParseMagnet( pszURL );
		}
		return ParsePeerProject( SkipSlashes( pszURL, nRoot ) );
	case 'g':	// peerproject: shareaza: gnutella: gnet:
		return ParsePeerProject( SkipSlashes( pszURL, nRoot ) );
	case '2':	// g2:
		pszURL = SkipSlashes( pszURL, nRoot );
		if ( _tcsnicmp( pszURL, _T("//"), 2 ) == 0 )	// Check malformed assumption
			pszURL = SkipSlashes( pszURL, 2 );
		if ( _tcsnicmp( pszURL, _T("browse:"), 7 ) == 0 ||
			 _tcsnicmp( pszURL, _T("host:"), 5 ) == 0 ||
			 _tcsnicmp( pszURL, _T("chat:"), 5 ) == 0 ||
			 _tcsnicmp( pszURL, _T("search:"), 7 ) == 0 ||
			 ! ParseMagnet( pszURL ) )
			return ParsePeerProject( pszURL );
		return true;	// Magnet succeeded (?)
	case 'u':	// gwc: uhc: ukhl: gnutella1:host: gnutella2:host:
		return ParsePeerProject( pszURL );
	case 'k':	// ed2kftp://
		return ParseED2KFTP( pszURL, bResolve );
	case 'e':	// ed2k:
		SkipSlashes( pszURL, nRoot );
		return ParseDonkey( pszURL );
	case 'b':	// btc://
		return ParseBTC( pszURL, bResolve );
	case 'p':	// mp2p:
		SkipSlashes( pszURL, nRoot );
		return ParsePiolet( pszURL );
	case 'd':	// dchub://1.2.3.4:411	(adc:// ?)
		return ParseDCHub( pszURL, bResolve );
	case 'c':	// dcfile:// (Deprecated?)
		return ParseDCFile( pszURL, FALSE );
	case 'x':	// foxy://download?
		pszURL = SkipSlashes( pszURL, nRoot );
		if ( _tcsnicmp( pszURL, _T("download?"), 9 ) == 0 )		// Original
			return ParseMagnet( pszURL + 9 );
		if ( _tcsnicmp( pszURL, _T("download/?"), 10 ) == 0 )	// "Fixed" by IE
			return ParseMagnet( pszURL + 10 );
		//return FALSE;		// Other?
	case 'i':	// irc://irc.server:port/channel?key
	//	SkipSlashes( pszURL, nRoot );
		return FALSE;	// ToDo: IRC link support
	default:
		if ( IPStringToDWORD( CString( pszURL ).Trim( _T(" \t\r\n/") ), FALSE ) )
			return ParsePeerProject( pszURL );
		//return FALSE;		// Unknown? See http://en.wikipedia.org/wiki/URI_scheme
	}

// Obsolete method, for reference and deletion
//	if ( ! _tcsnicmp( pszURL, _T("http://"), 7 ) ||
//		 ! _tcsnicmp( pszURL, _T("https://"), 8 ) )
//		return ParseHTTP( pszURL, bResolve );
//	if ( ! _tcsnicmp( pszURL, _T("ftp://"), 6 ) )
//		return ParseFTP( pszURL, bResolve );
//	if ( ! _tcsnicmp( pszURL, _T("ed2kftp://"), 10 ) )
//		return ParseED2KFTP( pszURL, bResolve );
//	if ( ! _tcsnicmp( pszURL, _T("btc://"), 6 ) )
//		return ParseBTC( pszURL, bResolve );
//
//	if ( _tcsnicmp( pszURL, _T("magnet:?"), 8 ) == 0 )
//	{
//		pszURL += 8;
//		return ParseMagnet( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("magnet:"), 7 ) == 0 )
//	{
//		pszURL += 7;
//		return ParsePeerProject( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("ed2k:"), 5 ) == 0 )
//	{
//		SkipSlashes( pszURL, 5 );
//		return ParseDonkey( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("shareaza:"), 9 ) == 0 ||
//			  _tcsnicmp( pszURL, _T("gnutella:"), 9 ) == 0 )
//	{
//		SkipSlashes( pszURL, 9 );
//		return ParsePeerProject( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("peerproject:"), 12 ) == 0 )	// APP_LENGTH LETTERCOUNT
//	{
//		SkipSlashes( pszURL, 12 );
//		return ParsePeerProject( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("gwc:"), 4 ) == 0 )
//	{
//		CString strTemp;
//		strTemp.Format( _T("peerproject:%s"), pszURL );
//		pszURL = strTemp;
//		SkipSlashes( pszURL, 12 );
//		return ParsePeerProject( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("uhc:"), 4 ) == 0 ||
//			  _tcsnicmp( pszURL, _T("ukhl:"), 5 ) == 0 ||
//			  _tcsnicmp( pszURL, _T("gnutella1:"), 10 ) == 0 ||
//			  _tcsnicmp( pszURL, _T("gnutella2:"), 10 ) == 0 )
//	{
//		return ParsePeerProject( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("g2:"), 3 ) == 0 )
//	{
//		SkipSlashes( pszURL, 3 );
//		if ( _tcsnicmp( pszURL, _T("browse:"), 7 ) == 0 || ! ParseMagnet( pszURL ) )
//			return ParsePeerProject( pszURL );
//		return TRUE;
//	}
//	else if ( _tcsnicmp( pszURL, _T("gnet:"), 5 ) == 0 )
//	{
//		SkipSlashes( pszURL, 5 );
//		return ParsePeerProject( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("mp2p:"), 5 ) == 0 )
//	{
//		SkipSlashes( pszURL, 5 );
//		return ParsePiolet( pszURL );
//	}
//	else if ( _tcsnicmp( pszURL, _T("foxy:"), 5 ) == 0 )			// Foxy
//	{
//		pszURL += 5;
//		if ( ! _tcsnicmp( pszURL, _T("//download?"), 11 ) )			// Original
//		{
//			pszURL += 11;
//			return ParseMagnet( pszURL );
//		}
//		else if ( ! _tcsnicmp( pszURL, _T("//download/?"), 12 ) )	// "Fixed" by IE
//		{
//			pszURL += 12;
//			return ParseMagnet( pszURL );
//		}
//	}
//	else if ( ! _tcsnicmp( pszURL, _T("dchub://"), 8 ) )			// dchub://1.2.3.4:411
//	{
//		SkipSlashes( pszURL, 8 );
//		m_nPort = protocolPorts[ PROTOCOL_DC ];
//		m_nProtocol	= PROTOCOL_DC;
//		return ParsePeerProjectHost( pszURL, FALSE );
//	}
//	else if ( ! _tcsnicmp( pszURL, _T("dcfile://"), 8 ) )
//	{
//		return ParseDC( pszURL, FALSE );
//	}

	Clear();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL HTTP

BOOL CPeerProjectURL::ParseHTTP(LPCTSTR pszURL, BOOL bResolve)
{
	CString strURL = pszURL;
	if ( ! _tcsnicmp( pszURL, _T("http://"), 7 ) )
		strURL = pszURL + 7;
	else if ( ! _tcsncmp( pszURL, _T("https://"), 8 ) )		// HTTPS unsupported, but try such links
		strURL = pszURL + 8;
	//else
	//	return FALSE;

	Clear();

	const int nSlash = strURL.Find( _T('/') );
	if ( nSlash >= 0 )
	{
		m_sAddress	= strURL.Left( nSlash );
		m_sPath 	= strURL.Mid( nSlash );
	}
	else
	{
		m_sAddress	= strURL;
		m_sPath 	= _T("/");
	}

	// Detect mistaken IP:port
	if ( m_sPath == _T("/") && m_sAddress.Find( _T(':') ) > 8 && IPStringToDWORD( m_sAddress, FALSE ) )
		return ParsePeerProject( m_sAddress );

	const int nAt = m_sAddress.Find( _T('@') );
	if ( nAt >= 0 ) m_sAddress = m_sAddress.Mid( nAt + 1 );

	if ( m_sAddress.IsEmpty() ) return FALSE;

	if ( _tcsnicmp( m_sPath, _T("/uri-res/N2R?"), 13 ) == 0 )
	{
		strURL = m_sPath.Mid( 13 );
		if ( m_oSHA1.fromUrn( strURL ) ) ;
		else if ( m_oTiger.fromUrn( strURL ) ) ;
		else if ( m_oED2K.fromUrn( strURL ) ) ;
		else if ( m_oBTH.fromUrn( strURL ) ) ;
		else if ( m_oBTH.fromUrn< Hashes::base16Encoding >( strURL ) ) ;
		else if ( m_oMD5.fromUrn( strURL ) ) ;
		else return FALSE;

		m_nAction = uriSource;
	}
	else
	{
		m_nAction = uriDownload;

		CString strName( URLDecode( m_sPath.Mid( m_sPath.ReverseFind( '/' ) + 1 ).SpanExcluding( _T("?") ) ) );
		if ( ! strName.IsEmpty() )
			m_sName = strName;
	}

	SOCKADDR_IN saHost;

	const BOOL bResult = Network.Resolve( m_sAddress, INTERNET_DEFAULT_HTTP_PORT, &saHost, bResolve );

	m_pAddress	= saHost.sin_addr;
	m_nPort		= htons( saHost.sin_port );

	m_sURL		= pszURL;
	m_nProtocol	= PROTOCOL_HTTP;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL FTP

BOOL CPeerProjectURL::ParseFTP(LPCTSTR pszURL, BOOL bResolve)
{
	// URI format:	ftp://[user[:password]@]host[:port][/path]

	//if ( _tcsncmp( pszURL, _T("ftp://"), 6 ) != 0 ) return FALSE;

	Clear();

	CString strURL ( pszURL + 6 );

	const int nSlash = strURL.Find( _T('/') );
	if ( nSlash >= 0 )
	{
		m_sAddress	= strURL.Left( nSlash );
		m_sPath		= strURL.Mid( nSlash );
	}
	else
	{
		m_sAddress = strURL;
		m_sPath = _T("/");
	}

	const int nAt = m_sAddress.Find( _T('@') );
	if ( nAt >= 0 )
	{
		m_sLogin = m_sAddress.Left( nAt );
		m_sAddress = m_sAddress.Mid( nAt + 1 );

		const int nColon = m_sLogin.Find( _T(':') );
		if ( nColon >= 0 )
		{
			m_sPassword = m_sLogin.Mid( nColon + 1 );
			m_sLogin = m_sLogin.Left( nColon );
		}
	}
	else
	{
		m_sLogin = _T("anonymous");
		m_sPassword = _T("guest@peerproject.org");
	}

	if ( m_sAddress.IsEmpty() || m_sLogin.IsEmpty() )
		return FALSE;

	// Add fix set name
	const int nPos = m_sPath.ReverseFind( '/' );
	if ( m_sName.IsEmpty() && nPos >= 0 )
	{
		const CString sName( URLDecode( m_sPath.Mid( nPos + 1 ).SpanExcluding( _T("?") ) ) );
		if ( ! sName.IsEmpty() )
			m_sName = sName;
	}

	SOCKADDR_IN saHost;

	BOOL bResult = Network.Resolve( m_sAddress, INTERNET_DEFAULT_FTP_PORT, &saHost, bResolve );

	m_pAddress	= saHost.sin_addr;
	m_nPort		= htons( saHost.sin_port );

	m_sURL		= pszURL;
	m_nProtocol	= PROTOCOL_FTP;
	m_nAction	= uriDownload;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL ED2KFTP

BOOL CPeerProjectURL::ParseED2KFTP(LPCTSTR pszURL, BOOL bResolve)
{
	//if ( _tcsnicmp( pszURL, _T("ed2kftp://"), 10 ) != 0 ) return FALSE;

	Clear();

	CString strURL = pszURL + 10;
	BOOL bPush = FALSE;

	int nSlash = strURL.Find( _T('/') );
	if ( nSlash < 7 ) return FALSE;

	m_sAddress	= strURL.Left( nSlash );
	strURL		= strURL.Mid( nSlash + 1 );

	nSlash = strURL.Find( _T('/') );
	if ( nSlash != 32 ) return FALSE;

	CString strHash	= strURL.Left( 32 );
	strURL			= strURL.Mid( 33 );

	if ( ! m_oED2K.fromString( strHash ) ) return FALSE;

	m_bSize = _stscanf( strURL, _T("%I64i"), &m_nSize ) == 1;
	if ( ! m_bSize ) return FALSE;

	nSlash = m_sAddress.Find( _T('@') );

	if ( nSlash > 0 )
	{
		strHash = m_sAddress.Left( nSlash );
		m_sAddress = m_sAddress.Mid( nSlash + 1 );
		if ( _stscanf( strHash, _T("%lu"), &m_pAddress.S_un.S_addr ) != 1 ) return FALSE;
		bPush = TRUE;
	}

	SOCKADDR_IN saHost;
	BOOL bResult = Network.Resolve( m_sAddress, protocolPorts[ PROTOCOL_ED2K ], &saHost, bResolve );

	if ( bPush )
	{
		m_pServerAddress	= saHost.sin_addr;
		m_nServerPort		= htons( saHost.sin_port );
		m_nPort				= 0;
	}
	else
	{
		m_pAddress	= saHost.sin_addr;
		m_nPort		= htons( saHost.sin_port );
	}

	m_sURL		= pszURL;
	m_nProtocol	= PROTOCOL_ED2K;
	m_nAction	= uriDownload;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL DC

BOOL CPeerProjectURL::ParseDCHub(LPCTSTR pszURL, BOOL bResolve)
{
	Clear();

	// dchub://[login@]address:port/[filepath]	-Can be regular path or "files.xml.bz2" or "TTH:tiger_hash/size/"

	if ( pszURL[0] == _T('a') )
		pszURL = SkipSlashes( pszURL, 6 );	// "adc://"
	else
		pszURL = SkipSlashes( pszURL, 8 );	// "dchub://"

	CString strURL = pszURL;

	int nSlash = strURL.Find( _T('/') );

	// Short version (hub address only)
	if ( nSlash == -1 || nSlash == strURL.GetLength() - 1 )		// || strURL.IsEmpty()
	{
		m_sAddress.Empty();
		m_nPort = protocolPorts[ PROTOCOL_DC ];
		if ( ! ParsePeerProjectHost( pszURL ) )
			return FALSE;
		m_nProtocol = PROTOCOL_DC;
		return TRUE;
	}

	// Full version (file URL)

	m_sAddress	= strURL.Left( nSlash );
	strURL		= strURL.Mid( nSlash + 1 ).TrimLeft( _T("/") );

	int nAt = m_sAddress.Find( _T('@') );
	if ( nAt > 0 )
	{
		m_sLogin = URLDecode( m_sAddress.Left( nAt ) );
		m_sAddress = m_sAddress.Mid( nAt + 1 );
	}

	int nHash = strURL.Find( _T("TTH:") );
	if ( nHash != -1 )
	{
		CString strHash = strURL.Mid( nHash + 4, 39 );
		strURL = strURL.Mid( nHash + 4 + 39 );

		if ( ! m_oTiger.fromString( strHash ) )
			return FALSE;

		if ( _stscanf( strURL, _T("/%I64i"), &m_nSize ) == 1 )
			m_bSize = TRUE;
	}
	else
	{
		m_sName = URLDecode( strURL );
	}

	SOCKADDR_IN saHost = {};
	BOOL bResult = Network.Resolve( m_sAddress, protocolPorts[ PROTOCOL_DC ], &saHost, bResolve );

	m_pServerAddress	= saHost.sin_addr;
	m_nServerPort		= htons( saHost.sin_port );
	m_sURL.Format( _T("dchub://%s"), pszURL );
	m_nProtocol			= PROTOCOL_DC;
	m_nAction			= uriDownload;

	return bResult;
}

BOOL CPeerProjectURL::ParseDCFile(LPCTSTR pszURL, BOOL bResolve)
{
	Clear();

	// dcfile://address:port/login/TTH:tiger_hash/size/	(Deprecated?)

	//if ( _tcsnicmp( pszURL, _T("dcfile://"), 6 ) != 0 ) return FALSE;

	CString strURL = pszURL + 9;		// "dcfile://"

	int nSlash = strURL.Find( _T('/') );
	if ( nSlash < 7 ) return FALSE;

	m_sAddress	= strURL.Left( nSlash );
	strURL		= strURL.Mid( nSlash + 1 );

	nSlash = strURL.Find( _T('/') );
	if ( nSlash < 3 ) return FALSE;

	m_sLogin	= URLDecode( strURL.Left( nSlash ) );
	strURL		= strURL.Mid( nSlash + 1 );

	nSlash = strURL.Find( _T('/') );
	if ( nSlash != 4 + 39 ) return FALSE;

	CString strHash	= strURL.Left( nSlash );
	strURL			= strURL.Mid( nSlash + 1 );
	strURL.TrimRight( _T("//") );

	if ( strHash.Left( 4 ) != _T("TTH:") ) return FALSE;

	if ( ! m_oTiger.fromString( strHash.Mid( 4 ) ) ) return FALSE;

	m_bSize = _stscanf( strURL, _T("%I64i"), &m_nSize ) == 1;
	if ( ! m_bSize ) return FALSE;

	SOCKADDR_IN saHost = {};
	BOOL bResult = Network.Resolve( m_sAddress, protocolPorts[ PROTOCOL_DC ], &saHost, bResolve );

	m_pServerAddress	= saHost.sin_addr;
	m_nServerPort		= htons( saHost.sin_port );
	m_sURL				= pszURL;
	m_nProtocol			= PROTOCOL_DC;
	m_nAction			= uriDownload;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL BTC

BOOL CPeerProjectURL::ParseBTC(LPCTSTR pszURL, BOOL bResolve)
{
	//if ( _tcsnicmp( pszURL, _T("btc://"), 6 ) != 0 ) return FALSE;

	Clear();

	CString strURL = pszURL + 6;

	int nSlash = strURL.Find( _T('/') );
	if ( nSlash < 7 ) return FALSE;

	m_sAddress	= strURL.Left( nSlash );
	strURL		= strURL.Mid( nSlash + 1 );

	nSlash = strURL.Find( _T('/') );
	m_oBTC.clear();

	if ( nSlash < 0 ) return FALSE;

	if ( nSlash == 32 )
	{
		CString strGUID	= strURL.Left( 32 );
		m_oBTC.fromString( strGUID );
	}

	strURL = strURL.Mid( nSlash + 1 );

	if ( ! m_oBTH.fromString( strURL ) ) return FALSE;

	SOCKADDR_IN saHost;
	BOOL bResult = Network.Resolve( m_sAddress, protocolPorts[ PROTOCOL_BT ], &saHost, bResolve );

	m_pAddress	= saHost.sin_addr;
	m_nPort		= htons( saHost.sin_port );

	m_sURL		= pszURL;
	m_nProtocol	= PROTOCOL_BT;
	m_nAction	= uriDownload;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse "magnet:" URLs

BOOL CPeerProjectURL::ParseMagnet(LPCTSTR pszURL)
{
	Clear();

	while ( *pszURL == _T('?') )
		++pszURL;	// "magnet:?"

	CString strURL( pszURL );
	CAutoPtr< CBTInfo > pTorrent( new CBTInfo() );		// Was CBTInfo* pTorrent = new CBTInfo();

	// http://en.wikipedia.org/wiki/Magnet_URI_scheme

	for ( strURL += '&' ; ! strURL.IsEmpty() ; )
	{
		const CString strPart = strURL.SpanExcluding( _T("&") );
		strURL = strURL.Mid( strPart.GetLength() + 1 );

		const int nEquals = strPart.Find( '=' );
		if ( nEquals < 0 ) continue;

		CString strKey   = URLDecode( strPart.Left( nEquals ) );
		CString strValue = URLDecode( strPart.Mid( nEquals + 1 ) );

		SafeString( strKey );
		SafeString( strValue );

		if ( strKey.IsEmpty() || strValue.IsEmpty() ) continue;

		if ( _tcsicmp( strKey, _T("xt") ) == 0 ||
			 _tcsicmp( strKey, _T("xs") ) == 0 ||
			 _tcsicmp( strKey, _T("as") ) == 0 ||
			 _tcsicmp( strKey, _T("mt") ) == 0 ||
			 _tcsnicmp( strKey, _T("tr"), 2 ) == 0 ||	// &tr{any}
			 _tcsnicmp( strKey, _T("xt"), 2 ) == 0 )	// &xt.1
		{
			if ( StartsWith( strValue, _PT("urn:") ) ||
				 StartsWith( strValue, _PT("sha1:") ) ||
				 StartsWith( strValue, _PT("bitprint:") ) ||
				 StartsWith( strValue, _PT("btih:") ) ||
				 StartsWith( strValue, _PT("ed2k:") ) ||
				 StartsWith( strValue, _PT("md5:") ) ||
				 StartsWith( strValue, _PT("tree:tiger") ) )		// tree:tiger: tree:tiger/: tree:tiger/1024:
			{
				if ( ! m_oSHA1 ) m_oSHA1.fromUrn( strValue );
				if ( ! m_oTiger ) m_oTiger.fromUrn( strValue );
				if ( ! m_oED2K ) m_oED2K.fromUrn( strValue );
				if ( ! m_oMD5 ) m_oMD5.fromUrn( strValue );
				if ( ! m_oBTH ) m_oBTH.fromUrn( strValue );
				if ( ! m_oBTH ) m_oBTH.fromUrn< Hashes::base16Encoding >( strValue );
			}
			else if ( StartsWith( strValue, _PT("http://") ) ||
					  StartsWith( strValue, _PT("https://") ) ||
					  StartsWith( strValue, _PT("http%3A//") ) ||
					  StartsWith( strValue, _PT("udp://") ) ||
					  StartsWith( strValue, _PT("udp%3A//") ) ||
					  StartsWith( strValue, _PT("ftp://") ) ||
					  StartsWith( strValue, _PT("ftp%3A//") ) ||
					  StartsWith( strValue, _PT("dchub://") ) ||
					  StartsWith( strValue, _PT("dchub%3A//") ) )
			{
				strValue.Replace( _T(" "), _T("%20") );
				strValue.Replace( _T("%3A//"), _T("://") );

				if ( _tcsnicmp( strKey, _T("xt"), 2 ) == 0 )		// Compatibility hack: "&xt.{any}="
				{
					CString strURL = _T("@") + strValue;

					if ( ! m_sURL.IsEmpty() )
						m_sURL = strURL + _T(", ") + m_sURL;
					else
						m_sURL = strURL;
				}
				else if ( _tcsnicmp( strKey, _T("tr"), 2 ) == 0 )	// Compatibility hack: "&tr{any}="
				{
					pTorrent->SetTracker( strValue );
				}
				else
				{
					if ( ! m_sURL.IsEmpty() )
						m_sURL += _T(", ");
					m_sURL += strValue;
				}
			}
		}
		else if ( _tcsicmp( strKey, _T("dn") ) == 0 )
		{
			m_sName = strValue;
		}
		else if ( _tcsicmp( strKey, _T("kt") ) == 0 )
		{
			m_sName = strValue;
			m_oSHA1.clear();
			m_oTiger.clear();
			m_oED2K.clear();
			m_oMD5.clear();
			m_oBTH.clear();
		}
		else if ( _tcsicmp( strKey, _T("xl") ) == 0 ||
			_tcsicmp( strKey, _T("sz") ) == 0 ||	// Non-standard
			_tcsicmp( strKey, _T("fs") ) == 0 )		// Foxy
		{
			QWORD nSize;
			if ( ! m_bSize && ( _stscanf( strValue, _T("%I64i"), &nSize ) == 1 ) && nSize > 0 )
			{
				m_nSize = nSize;
				m_bSize = TRUE;
			}
		}
		else if ( _tcsicmp( strKey, _T("bn") ) == 0 )
		{
			pTorrent->SetNode( strValue );
		}
		//else ToDo: Note unknown key?
	}

	if ( m_sName.GetLength() > 3 )
		m_sName = URLDecode( (LPCTSTR)m_sName );

	if ( m_oBTH && ! m_pTorrent )
	{
		pTorrent->SetTrackerMode( pTorrent->GetTrackerCount() == 1 ?
			CBTInfo::tSingle : CBTInfo::tMultiFinding );

		m_pTorrent = pTorrent;
		m_pTorrent->m_nSize		= m_nSize;
		m_pTorrent->m_sName		= m_sName;
		m_pTorrent->m_oSHA1		= m_oSHA1;
		m_pTorrent->m_oTiger	= m_oTiger;
		m_pTorrent->m_oED2K		= m_oED2K;
		m_pTorrent->m_oMD5		= m_oMD5;
		m_pTorrent->m_oBTH		= m_oBTH;
	}

	delete pTorrent;

	if ( HasHash() || ! m_sURL.IsEmpty() )
	{
		m_nAction = uriDownload;
		return TRUE;
	}

	if ( ! m_sName.IsEmpty() )
	{
		m_nAction = uriSearch;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse "peerproject:" URLs

BOOL CPeerProjectURL::ParsePeerProject(LPCTSTR pszURL)
{
	Clear();

	int nIP[4];
	if ( _stscanf( pszURL, _T("%i.%i.%i.%i"), &nIP[0], &nIP[1], &nIP[2], &nIP[3] ) == 4 )
		return ParsePeerProjectHost( pszURL, FALSE );

	if ( ! _tcsnicmp( pszURL, _T("host:"), 5 ) || ! _tcsnicmp( pszURL, _T("node:"), 5 ) )
		return ParsePeerProjectHost( pszURL + 5, FALSE );
	if ( ! _tcsnicmp( pszURL, _T("hub:"), 4 ) )
		return ParsePeerProjectHost( pszURL + 4, FALSE );
	if ( ! _tcsnicmp( pszURL, _T("server:"), 7 ) )
		return ParsePeerProjectHost( pszURL + 7, FALSE );
	if ( ! _tcsnicmp( pszURL, _T("browse:"), 7 ) )
		return ParsePeerProjectHost( pszURL + 7, TRUE );
	if ( ! _tcsnicmp( pszURL, _T("chat:"), 5 ) )
		return ParsePeerProjectHost( pszURL + 5, TRUE );
	if ( ! _tcsnicmp( pszURL, _T("http://"), 7 ) )
		return ParsePeerProjectHost( pszURL + 7, TRUE );
	if ( ! _tcsnicmp( pszURL, _T("gwc:"), 4 ) )
		return ParseDiscovery( pszURL + 4, CDiscoveryService::dsWebCache );
	if ( ! _tcsnicmp( pszURL, _T("meturl:"), 7 ) )
		return ParseDiscovery( pszURL + 7, CDiscoveryService::dsServerList );
	if ( ! _tcsnicmp( pszURL, _T("url:"), 4 ) )
		return Parse( pszURL + 4 );
	if ( ! _tcsnicmp( pszURL, _T("uhc:"), 4 ) ||
		 ! _tcsnicmp( pszURL, _T("ukhl:"), 5 ) ||
		 ! _tcsnicmp( pszURL, _T("gnutella1:host:"), 15 ) ||
		 ! _tcsnicmp( pszURL, _T("gnutella2:host:"), 15 ) )
		return ParseDiscovery( pszURL, CDiscoveryService::dsGnutella );
	if ( _tcsnicmp( pszURL, _T("btnode:"), 7 ) == 0 )
	{
		m_nPort = protocolPorts[ PROTOCOL_BT ];
		if ( ParsePeerProjectHost( SkipSlashes( pszURL, 7 ) ) )
		{
			m_sAddress.Format( _T("%s:%u"), m_sName, m_nPort );
			m_nProtocol = PROTOCOL_BT;
			m_nAction = uriHost;
			return TRUE;
		}
		return FALSE;
	}
	if ( _tcsnicmp( pszURL, _T("search:"), 7 ) == 0 )
	{
		// ToDo: Formalize search parameters (Handle size/schema/etc?)
		m_sName = URLDecode( pszURL + 7 );
		m_nAction = uriSearch;
		m_oSHA1.clear();
		m_oTiger.clear();
		m_oED2K.clear();
		m_oMD5.clear();
		m_oBTH.clear();
		return TRUE;
	}
	if ( _tcsnicmp( pszURL, _T("command:"), 8 ) == 0 )
	{
		m_sName = pszURL + 8;
		if ( m_sName.IsEmpty() )
			return FALSE;
		m_nAction = uriCommand;
		return TRUE;
	}

	return ParsePeerProjectFile( pszURL );
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse PeerProject host URL

BOOL CPeerProjectURL::ParsePeerProjectHost(LPCTSTR pszURL, BOOL bBrowse)
{
	m_nAction = bBrowse ? uriBrowse : uriHost;

	m_sName = pszURL;
	m_sName = m_sName.SpanExcluding( _T("/\\") );
	m_nPort = protocolPorts[ PROTOCOL_G2 ];

	int nPos = m_sName.Find( ':' );
	if ( nPos >= 0 )
	{
		_stscanf( m_sName.Mid( nPos + 1 ), _T("%i"), &m_nPort );
		m_sName = m_sName.Left( nPos );
	}

	int nAt = m_sName.Find( _T('@') );
	if ( nAt >= 0 )
	{
		if ( nAt > 0 )
			m_sLogin = URLDecode( m_sName.Left( nAt ) );
		m_sName = m_sName.Mid( nAt + 1 );
	}

	m_sName.Trim();

	return m_sName.GetLength();
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse PeerProject file URL

BOOL CPeerProjectURL::ParsePeerProjectFile(LPCTSTR pszURL)
{
	CString strURL( pszURL );

	for ( strURL += '/' ; ! strURL.IsEmpty() ; )
	{
		CString strPart = strURL.SpanExcluding( _T("/|") );
		strURL = strURL.Mid( strPart.GetLength() + 1 );

		strPart.Trim();
		if ( strPart.IsEmpty() ) continue;

		if ( StartsWith( strPart, _PT("urn:") ) ||
			 StartsWith( strPart, _PT("sha1:") ) ||
			 StartsWith( strPart, _PT("bitprint:") ) ||
			 StartsWith( strPart, _PT("btih:") ) ||
			 StartsWith( strPart, _PT("ed2k:") ) ||
			 StartsWith( strPart, _PT("md5:") ) ||
			 StartsWith( strPart, _PT("tree:tiger") ) )		// tree:tiger: tree:tiger/: tree:tiger/1024:
		{
			if ( ! m_oSHA1 ) m_oSHA1.fromUrn( strPart );
			if ( ! m_oTiger ) m_oTiger.fromUrn( strPart );
			if ( ! m_oMD5 ) m_oMD5.fromUrn( strPart );
			if ( ! m_oED2K ) m_oED2K.fromUrn( strPart );
			if ( ! m_oBTH ) m_oBTH.fromUrn( strPart );
			if ( ! m_oBTH ) m_oBTH.fromUrn< Hashes::base16Encoding >( strPart );
		}
		else if ( _tcsnicmp( strPart, _T("source:"), 7 ) == 0 )
		{
			CString strSource = URLDecode( strPart.Mid( 7 ) );
			SafeString( strSource );

			if ( ! m_sURL.IsEmpty() ) m_sURL += ',';
			m_sURL += _T("http://");
			m_sURL += URLEncode( strSource );
			m_sURL += _T("/(^name^)");
		}
		else if ( _tcsnicmp( strPart, _T("name:"), 5 ) == 0 ||
				  _tcsnicmp( strPart, _T("file:"), 5 ) == 0 )
		{
			m_sName = URLDecode( strPart.Mid( 5 ) );
			SafeString( m_sName );
		}
		else if ( _tcschr( strPart, ':' ) == NULL )
		{
			m_sName = URLDecode( strPart );
			SafeString( m_sName );
		}
	}

	if ( ! m_sURL.IsEmpty() )
	{
		if ( ! m_sName.IsEmpty() )
		{
			m_sURL.Replace( _T("(^name^)"), URLEncode( m_sName ) );
			m_sURL.Replace( _T("\\"), _T("/") );
		}
		else
		{
			m_sURL.Empty();
		}
	}

	if ( HasHash() || ! m_sURL.IsEmpty() )
	{
		m_nAction = uriDownload;
		return TRUE;
	}
	if ( ! m_sName.IsEmpty() )
	{
		m_nAction = uriSearch;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse "ed2k:" URLs

BOOL CPeerProjectURL::ParseDonkey(LPCTSTR pszURL)
{
	Clear();

	if ( ! _tcsnicmp( pszURL, _T("|file|"), 6 ) )
		return ParseDonkeyFile( pszURL + 6 );
	if ( ! _tcsnicmp( pszURL, _T("|server|"), 8 ) )
		return ParseDonkeyServer( pszURL + 8 );
	if ( ! _tcsnicmp( pszURL, _T("|meturl|"), 8 ) )
		return ParseDiscovery( pszURL + 8, CDiscoveryService::dsServerList );
	if ( ! _tcsnicmp( pszURL, _T("|serverlist|"), 12 ) )
		return ParseDiscovery( pszURL + 12, CDiscoveryService::dsServerList );
	if ( ! _tcsnicmp( pszURL, _T("|search|"), 8 ) )
	{
		// ed2k://|search|text_to_find|/
		CString strURL( pszURL + 8 );

		const int nSep = strURL.Find( _T('|') );
		if ( nSep <= 0 ) return FALSE;

		m_sName = URLDecode( strURL.Mid( 0, nSep ) ).Trim();
		if ( m_sName.IsEmpty() ) return FALSE;

		m_nAction = uriSearch;

		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse eDonkey2000 file URL
//
// ed2k://|file|Shareaza_2.1.0.0.exe|3304944|A63D221505E99043B7E7308C67F81986|h=XY5VGKFVGJFYWMOAR5XS44YCEPXSL2JZ|/|sources,1.2.3.4:5555|/

BOOL CPeerProjectURL::ParseDonkeyFile(LPCTSTR pszURL)
{
	CString strURL( pszURL ), strPart;
	int nSep;

	// Name
	nSep = strURL.Find( '|' );
	if ( nSep < 0 ) return FALSE;
	strPart	= strURL.Left( nSep );
	strURL	= strURL.Mid( nSep + 1 );

	m_sName = URLDecode( strPart );
	SafeString( m_sName );
	if ( m_sName.IsEmpty() ) return FALSE;

	// Size
	nSep = strURL.Find( '|' );
	if ( nSep < 0 ) return FALSE;
	strPart	= strURL.Left( nSep );
	strURL	= strURL.Mid( nSep + 1 );

	if ( _stscanf( strPart, _T("%I64i"), &m_nSize ) != 1 ) return FALSE;
	m_bSize = TRUE;

	// Hash
	nSep = strURL.Find( '|' );
	if ( nSep < 0 ) return FALSE;
	strPart	= strURL.Left( nSep );
	strURL	= strURL.Mid( nSep + 1 );

	m_oED2K.fromString( strPart );

	// URL is valid
	m_nAction = uriDownload;

	// AICH hash (h), HTTP source (s) and/or hash set (p)
	nSep = strURL.Find( '|' );
	if ( nSep < 0 ) return TRUE;
	strPart	= strURL.Left( nSep );
	strURL	= strURL.Mid( nSep + 1 );
	while ( strPart != _T("/") )
	{
		if ( _tcsncmp( strPart, _T("h="), 2 ) == 0 )
		{
			// AICH hash
			//theApp.Message(MSG_INFO, _T("AICH") );
			strPart = strPart.Mid( 2 );
		}
		else if ( _tcsncmp( strPart, _T("s="), 2 ) == 0 )
		{
			// HTTP source
			//theApp.Message(MSG_INFO, _T("HTTP") );
			strPart = strPart.Mid( 2 );

			if ( ! m_sURL.IsEmpty() ) m_sURL += _T(", ");
			SafeString( strPart );
			m_sURL += strPart;
		}
		else if ( _tcsncmp( strPart, _T("p="), 2 ) == 0 )
		{
			// Hash set
			//theApp.Message(MSG_INFO, _T("hash set") );
			strPart = strPart.Mid( 2 );
		}

		// Read in next chunk
		nSep = strURL.Find( '|' );
		if ( nSep < 0 ) return TRUE;
		strPart	= strURL.Left( nSep );
		strURL	= strURL.Mid( nSep + 1 );
	}

	while ( strURL.GetLength() > 8 )
	{
		// Source (Starts with |/|sources,
		nSep = strURL.Find( ',' );
		if ( nSep < 0 ) return TRUE;
		strPart	= strURL.Left( nSep );
		strURL	= strURL.Mid( nSep + 1 );

		if ( _tcsncmp( strPart, _T("sources"), 7 ) != 0 ) return TRUE;

		nSep = strURL.Find( '|' );
		if ( nSep < 0 ) return TRUE;
		strPart	= strURL.Left( nSep );
		strURL	= strURL.Mid( nSep + 1 );

		// Now we have the source in x.x.x.x:port format.
		CString strEDFTP;
		strEDFTP.Format( _T("ed2kftp://%s/%s/%I64u/"), strPart, (LPCTSTR)m_oED2K.toString(), m_nSize );
		SafeString( strEDFTP );
		if ( ! m_sURL.IsEmpty() ) m_sURL += _T(", ");
		m_sURL += strEDFTP;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse eDonkey2000 server URL
//
// ed2k://|server|1.2.3.4|4661|/

BOOL CPeerProjectURL::ParseDonkeyServer(LPCTSTR pszURL)
{
	LPCTSTR pszPort = _tcschr( pszURL, '|' );
	if ( pszPort == NULL ) return FALSE;

	if ( _stscanf( pszPort + 1, _T("%i"), &m_nPort ) != 1 ) return FALSE;

	m_sName = pszURL;
	m_sName = m_sName.Left( static_cast< int >( pszPort - pszURL ) );

	m_sName.Trim();
	if ( m_sName.IsEmpty() ) return FALSE;

	m_sAddress.Format( _T("%s:%u"), m_sName, m_nPort );

	m_nProtocol = PROTOCOL_ED2K;
	m_nAction	= uriHost;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse "mp2p:" URLs

BOOL CPeerProjectURL::ParsePiolet(LPCTSTR pszURL)
{
	Clear();

	if ( _tcsnicmp( pszURL, _T("file|"), 5 ) == 0 )
		return ParsePioletFile( pszURL + 5 );
	if ( _tcsnicmp( pszURL, _T("|file|"), 6 ) == 0 )
		return ParsePioletFile( pszURL + 6 );

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse Piolet file URL
//
// mp2p://file|Shareaza1600.exe|789544|3fb626ed1a9f4cb9921107f510148370/

BOOL CPeerProjectURL::ParsePioletFile(LPCTSTR pszURL)
{
	CString strURL( pszURL ), strPart;
	int nSep;

	nSep = strURL.Find( '|' );
	if ( nSep < 0 ) return FALSE;
	strPart	= strURL.Left( nSep );
	strURL	= strURL.Mid( nSep + 1 );

	m_sName = URLDecode( strPart );
	SafeString( m_sName );
	if ( m_sName.IsEmpty() ) return FALSE;

	nSep = strURL.Find( '|' );
	if ( nSep < 0 ) return FALSE;
	strPart	= strURL.Left( nSep );
	strURL	= strURL.Mid( nSep + 1 );

	if ( _stscanf( strPart, _T("%I64i"), &m_nSize ) != 1 ) return FALSE;
	m_bSize = TRUE;

	strPart = strURL.SpanExcluding( _T(" |/") );
	m_oSHA1.fromString( strPart );

	m_nAction = uriDownload;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectURL parse discovery service URL

BOOL CPeerProjectURL::ParseDiscovery(LPCTSTR pszURL, int nType)
{
	if ( _tcsncmp( pszURL, _T("http://"), 7 ) != 0 &&
		 _tcsncmp( pszURL, _T("https://"), 8 ) != 0 &&
		 _tcsncmp( pszURL, _T("uhc:"), 4 ) != 0 &&
		 _tcsncmp( pszURL, _T("ukhl:"), 5 ) != 0 &&
		 _tcsncmp( pszURL, _T("gnutella1:host:"), 15 ) != 0 &&
		 _tcsncmp( pszURL, _T("gnutella2:host:"), 15 ) != 0 &&
		 _tcsncmp( pszURL, _T("g2:host:"), 8 ) != 0 )
		return FALSE;

	CString strURL, strNets, strTemp( pszURL );
	m_nProtocol = PROTOCOL_NULL;

	int nPos = strTemp.Find( '?' );

	strURL = nPos < 0 ? strTemp : strTemp.Left( nPos );
	strNets = strTemp.Mid( nPos + 1 );

	if ( ! _tcsnicmp( strNets, _T("nets="), 5 ) )
	{
		BOOL bG1 = FALSE, bG2 = FALSE;

		if ( _tcsistr( strNets, (LPCTSTR)_T("gnutella2") ) )
		{
			bG2 = TRUE;
			strNets.Replace( _T("gnutella2"), _T("") );
		}

		if ( _tcsistr( strNets, (LPCTSTR)_T("gnutella") ) )
			bG1 = TRUE;

		if ( bG1 && bG2 )
			;	// Do nothing
		else if ( bG2 )
			m_nProtocol = PROTOCOL_G2;
		else if ( bG1 && Settings.Discovery.EnableG1GWC )
			m_nProtocol = PROTOCOL_G1;
		else
			return FALSE;
	}

	nPos = strURL.Find( '|' );
	if ( nPos >= 0 )
		strURL = strURL.Left( nPos );

	m_nAction	= uriDiscovery;
	m_sURL		= strURL;
	m_nSize		= nType;

	return TRUE;
}

// Note SkipSlashes/SafeString moved to Strings

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectURL query constructor

CQuerySearchPtr CPeerProjectURL::ToQuery() const
{
	if ( m_nAction != uriDownload && m_nAction != uriSearch )
		return CQuerySearchPtr();

	CQuerySearchPtr pSearch = new CQuerySearch();

	if ( ! m_sName.IsEmpty() )
		pSearch->m_sSearch = m_sName;

	if ( m_oSHA1 )
		pSearch->m_oSHA1 = m_oSHA1;

	if ( m_oTiger )
		pSearch->m_oTiger = m_oTiger;

	if ( m_oED2K )
		pSearch->m_oED2K = m_oED2K;

	if ( m_oBTH )
		pSearch->m_oBTH = m_oBTH;

	if ( m_oMD5 )
		pSearch->m_oMD5 = m_oMD5;

	return pSearch;
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectURL shell registration

void CPeerProjectURL::Register(BOOL bRegister, BOOL bOnStartup)
{
	if ( bRegister )
	{
		RegisterShellType( NULL, _T("peerproject"), _T("URL:PeerProject P2P"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterMagnetHandler( _T("PeerProject"), _T("PeerProject P2P"), _T("PeerProject can automatically search and download selected content on peer-to-peer networks."), _T("PeerProject"), IDR_MAINFRAME );
	}
	else
	{
		UnregisterShellType( _T("peerproject") );
		UnregisterShellType( _T("Applications\\PeerProject.exe") );		// CLIENT_NAME
	}

	if ( CRegistry::GetString( _T("Software\\Shareaza\\Shareaza"), _T("Path") ).IsEmpty() )
	{
		if ( bRegister )
			RegisterShellType( NULL, _T("shareaza"), _T("URL:Shareaza P2P"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		else
			UnregisterShellType( _T("shareaza") );
	}

	if ( bRegister && Settings.Web.Magnet )
		RegisterShellType( NULL, _T("magnet"), _T("URL:Magnet Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
	else
		UnregisterShellType( _T("magnet") );

	if ( bRegister && Settings.Web.Gnutella )
	{
		RegisterShellType( NULL, _T("gnutella"), _T("URL:Gnutella Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("gnutella1"), _T("URL:Gnutella1 Bootstrap"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("gnutella2"), _T("URL:Gnutella2 Bootstrap"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("gwc"), _T("URL:GWC Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("g2"), _T("URL:G2 Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("gnet"), _T("URL:Gnutella Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("uhc"), _T("URL:Gnutella1 UDP Host Cache"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("ukhl"), _T("URL:Gnutella2 UDP known Hub Cache"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
	}
	else
	{
		UnregisterShellType( _T("gnutella") );
		UnregisterShellType( _T("gnutella1") );
		UnregisterShellType( _T("gnutella2") );
		UnregisterShellType( _T("gwc") );
		UnregisterShellType( _T("g2") );
		UnregisterShellType( _T("gnet") );
		UnregisterShellType( _T("uhc") );
		UnregisterShellType( _T("ukhl") );
	}

	if ( bRegister && Settings.Web.ED2K )
		RegisterShellType( NULL, _T("ed2k"), _T("URL:eDonkey2000 Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
	else
		UnregisterShellType( _T("ed2k") );

	if ( bRegister && Settings.Web.DC )
	{
		// ToDo: Support "adc:" hubs
		RegisterShellType( NULL, _T("adc"),    _T("URL:DirectConnect Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("dchub"),  _T("URL:DirectConnect Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
		RegisterShellType( NULL, _T("dcfile"), _T("URL:DirectConnect Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
	}
	else
	{
		UnregisterShellType( _T("adc") );
		UnregisterShellType( _T("dchub") );
		UnregisterShellType( _T("dcfile") );
	}

	if ( bRegister && Settings.Web.Piolet )
		RegisterShellType( NULL, _T("mp2p"), _T("URL:Piolet Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
	else
		UnregisterShellType( _T("mp2p") );

	if ( bRegister && Settings.Web.Foxy )
		RegisterShellType( NULL, _T("foxy"), _T("URL:Foxy Protocol"), NULL, _T("PeerProject"), _T("URL"), IDR_MAINFRAME );
	else
		UnregisterShellType( _T("foxy") );

	if ( ! Settings.Live.FirstRun || ! bOnStartup )
	{
		if ( bRegister && Settings.Web.Torrent )
		{
			RegisterShellType( NULL, _T("BitTorrent"), _T("Torrent File"), _T(".torrent"),
				_T("PeerProject"), _T("PEERFORMAT"), IDR_MAINFRAME );
			RegisterShellType( _T("Applications\\PeerProject.exe"), NULL, _T("Torrent File"), _T(".torrent"),
				_T("PeerProject"), _T("PEERFORMAT"), IDR_MAINFRAME );
		}
		else
		{
			UnregisterShellType( _T("BitTorrent") );
			UnregisterShellType( _T("Applications\\PeerProject.exe") );
		}

		// ToDo: .metalink files
	}

	if ( bRegister )
	{
		RegisterShellType( NULL, _T("PeerProject.Collection"), _T("PeerProject Collection File"),
			_T(".co"), _T("PeerProject"), _T("PEERFORMAT"), IDI_COLLECTION );
		RegisterShellType( _T("Applications\\PeerProject.exe"), NULL, _T("PeerProject Collection File"),
			_T(".co"), _T("PeerProject"), _T("PEERFORMAT"), IDI_COLLECTION );

		RegisterShellType( NULL, _T("PeerProject.Collection"), _T("PeerProject Collection File"),
			_T(".collection"), _T("PeerProject"), _T("PEERFORMAT"), IDI_COLLECTION );
		RegisterShellType( _T("Applications\\PeerProject.exe"), NULL, _T("PeerProject Collection File"),
			_T(".collection"), _T("PeerProject"), _T("PEERFORMAT"), IDI_COLLECTION );

		RegisterShellType( NULL, _T("eMule.Collection"), _T("eMule Collection File"),
			_T(".emulecollection"), _T("PeerProject"), _T("PEERFORMAT"), IDI_COLLECTION );
		RegisterShellType( _T("Applications\\PeerProject.exe"), NULL, _T("eMule Collection File"),
			_T(".emulecollection"), _T("PeerProject"), _T("PEERFORMAT"), IDI_COLLECTION );
	}
	else
	{
		UnregisterShellType( _T(".co") );
		UnregisterShellType( _T(".collection") );
		UnregisterShellType( _T(".emulecollection") );
		UnregisterShellType( _T("eMule.Collection") );
		UnregisterShellType( _T("PeerProject.Collection") );	// CLIENT_NAME
	}

	if ( ! bOnStartup )
		SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectURL shell registration helper

BOOL CPeerProjectURL::RegisterShellType(LPCTSTR pszRoot, LPCTSTR pszProtocol, LPCTSTR pszName, LPCTSTR pszType, LPCTSTR pszApplication, LPCTSTR pszTopic, UINT nIDIcon, BOOL bOverwrite)
{
	HKEY hRootKey = AfxGetPerUserRegistration() ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;
	LPCTSTR szRootKey = _T("Software\\Classes");

	HKEY hKey, hSub1, hSub2, hSub3, hSub4;
	CString strValue;
	DWORD nDisposition;

	CString strSubKey = szRootKey;
	if ( pszRoot )
	{
		if ( ! strSubKey.IsEmpty() )
			strSubKey += _T("\\");
		strSubKey += pszRoot;
	}
	if ( pszProtocol )
	{
		if ( ! strSubKey.IsEmpty() )
			strSubKey += _T("\\");
		strSubKey += pszProtocol;
	}

	if ( RegCreateKeyEx( hRootKey, (LPCTSTR)strSubKey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &nDisposition ) != ERROR_SUCCESS )
		return FALSE;

	if ( nDisposition == REG_OPENED_EXISTING_KEY && ! bOverwrite )
	{
		RegCloseKey( hKey );
		return FALSE;
	}

	const BOOL bProtocol = _tcsncmp( pszName, _T("URL:"), 4 ) == 0;
	const BOOL bApplication = pszRoot && _tcsicmp( pszRoot, _T("Applications\\PeerProject.exe") ) == 0;		// CLIENT_NAME

	// Register protocol to "Default Programs"
	if ( bProtocol )
	{
		CString strUrlAssociations = _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\");
		strUrlAssociations += pszProtocol;
		SHSetValue( hRootKey, strUrlAssociations + _T("\\UserChoice"), _T("Progid"), REG_STRING( CLIENT_NAME ) );
	}

	if ( ! bApplication )
	{
		RegSetValueEx( hKey, NULL, 0, REG_SZ, (LPBYTE)pszName,
			static_cast< DWORD >( sizeof(TCHAR) * ( _tcslen( pszName ) + 1 ) ) );

		if ( bProtocol )
			RegSetValueEx( hKey, _T("URL Protocol"), 0, REG_SZ, (LPBYTE)(LPCTSTR)_T(""), sizeof(TCHAR) );

		if ( RegCreateKey( hKey, _T("DefaultIcon"), &hSub1 ) == ERROR_SUCCESS )
		{
			strValue = Skin.GetImagePath( nIDIcon );
			RegSetValueEx( hSub1, NULL, 0, REG_SZ,
				(LPBYTE)(LPCTSTR)strValue, sizeof(TCHAR) * ( strValue.GetLength() + 1 ) );
			RegCloseKey( hSub1 );
		}
	}
	else if ( pszType != NULL )
	{
	//	HKEY hKeySupported;
	//	if ( RegCreateKey( hKey, _T("SupportedTypes"), &hKeySupported ) == ERROR_SUCCESS )
	//	{
	//		RegSetValueEx( hKeySupported, pszType, 0, REG_SZ, NULL, 0 );
	//		RegCloseKey( hKeySupported );
	//	}

		RegSetValueEx( hKey, _T("FriendlyAppName"), 0, REG_STRING( CLIENT_NAME ) );
	}

	if ( RegCreateKey( hKey, _T("shell"), &hSub1 ) == ERROR_SUCCESS )
	{
		if ( RegCreateKey( hSub1, _T("open"), &hSub2 ) == ERROR_SUCCESS )
		{
			if ( RegCreateKey( hSub2, _T("command"), &hSub3 ) == ERROR_SUCCESS )
			{
				strValue.Format( _T("\"%s\" \"%%%c\""), theApp.m_strBinaryPath, bProtocol ? 'L' : '1' );
				RegSetValueEx( hSub3, NULL, 0, REG_SZ, (LPBYTE)(LPCTSTR)strValue, sizeof(TCHAR) * ( strValue.GetLength() + 1 ) );
				RegCloseKey( hSub3 );
			}

			if ( RegCreateKey( hSub2, _T("ddeexec"), &hSub3 ) == ERROR_SUCCESS )
			{
				RegSetValueEx( hSub3, NULL, 0, REG_SZ, (LPBYTE)_T("%1"), sizeof(_T("%1")) );

				if ( RegCreateKey( hSub3, _T("Application"), &hSub4 ) == ERROR_SUCCESS )
				{
					RegSetValueEx( hSub4, NULL, 0, REG_SZ, (LPBYTE)pszApplication,
						static_cast< DWORD >( sizeof(TCHAR) * ( _tcslen( pszApplication ) + 1 ) ) );
					RegCloseKey( hSub4 );
				}

				if ( RegCreateKey( hSub3, _T("Topic"), &hSub4 ) == ERROR_SUCCESS )
				{
					RegSetValueEx( hSub4, NULL, 0, REG_SZ, (LPBYTE)pszTopic,
						static_cast< DWORD >( sizeof(TCHAR) * ( _tcslen( pszTopic ) + 1 ) ) );
					RegCloseKey( hSub4 );
				}

				if ( RegCreateKey( hSub3, _T("IfExec"), &hSub4 ) == ERROR_SUCCESS )
				{
					RegSetValueEx( hSub4, NULL, 0, REG_STRING( _T("*") ) );
					RegCloseKey( hSub4 );
				}

				RegSetValueEx( hSub3, _T("WindowClassName"), 0, REG_STRING( CLIENT_HWND ) );	// "PeerProjectMainWnd"

				RegCloseKey( hSub3 );
			}

			RegCloseKey( hSub2 );
		}

		RegCloseKey( hSub1 );
	}

	if ( ! bApplication )
	{
		if ( pszType && *pszType == _T('.') )
		{
			DWORD dwData = /* FTA_OpenIsSafe */ 0x00010000;	// FILETYPEATTRIBUTEFLAGS
			RegSetValueEx( hKey, _T("EditFlags"), 0, REG_NUMBER( dwData ) );
			RegSetValueEx( hKey, _T("AppUserModelID"), 0, REG_STRING( CLIENT_NAME ) );
		}
		else if ( bProtocol )
		{
			DWORD dwData = /* FTA_Show */ 0x00000002;		// FILETYPEATTRIBUTEFLAGS
			RegSetValueEx( hKey, _T("EditFlags"), 0, REG_NUMBER( dwData ) );
			RegSetValueEx( hKey, _T("AppUserModelID"), 0, REG_STRING( CLIENT_NAME ) );
		}
	}

	RegCloseKey( hKey );

	if ( pszType && pszProtocol )
	{
		strSubKey = szRootKey;
		if ( pszRoot )
		{
			if ( ! strSubKey.IsEmpty() )
				strSubKey += _T("\\");
			strSubKey += pszRoot;
		}
		if ( ! strSubKey.IsEmpty() )
			strSubKey += _T("\\");
		strSubKey += pszType;

		if ( RegCreateKeyEx( hRootKey, (LPCTSTR)strSubKey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &nDisposition ) == ERROR_SUCCESS )
		{
			RegSetValueEx( hKey, NULL, 0, REG_SZ, (LPBYTE)pszProtocol,
				static_cast< DWORD >( sizeof(TCHAR) * ( _tcslen( pszProtocol ) + 1 ) ) );

		//	if ( RegCreateKey( hKey, _T("OpenWithProgids"), &hSub1 ) == ERROR_SUCCESS )
		//	{
		//		RegSetValueEx( hSub1, pszProtocol, 0, REG_NONE, NULL, 0 );
		//		RegCloseKey( hSub1 );
		//	}

			RegCloseKey( hKey );
		}
	}

	return TRUE;
}

BOOL CPeerProjectURL::UnregisterShellType(LPCTSTR pszRoot)
{
	HKEY hKey, hRootKey = AfxGetPerUserRegistration() ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;
	LPCTSTR szRootKey = _T("Software\\Classes");

	BOOL bRegisteredUser = FALSE;

	CString strSubKey = szRootKey, strOldKey;
	if ( pszRoot )
	{
		if ( ! strSubKey.IsEmpty() )
			strSubKey += _T("\\");
		strSubKey += pszRoot;
	}

	// Obsolete method:
	//if ( ! bRegisteredUser )
	//{
	//	CString strSubKey;
	//	strSubKey.Format( _T("Software\\Classes\\%s"), pszProtocol );
	//	CString strPath = CRegistry::GetString( _T("shell\\open\\command"), NULL, NULL, strSubKey );
	//	if ( _tcsistr( strPath, theApp.m_strBinaryPath ) != NULL ||
	//		CRegistry::GetString( _T("shell\\open\\ddeexec\\Application"), NULL, NULL, strSubKey ) == _T("PeerProject") )
	//	{
	//		CRegistry::DeleteKey( HKEY_CURRENT_USER, (LPCTSTR)strSubKey );
	//	}
	//}

	// Delete protocol from "Default Programs"
	if ( pszRoot && *pszRoot != _T('.') )
	{
		CString strProgID;
		CString strUrlAssociations = _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\");
		strUrlAssociations += pszRoot;
		DWORD nType, nSize = MAX_PATH * sizeof( TCHAR );
		LRESULT ret = SHGetValue( hRootKey, strUrlAssociations + _T("\\UserChoice"), _T("Progid"), &nType, strProgID.GetBuffer( MAX_PATH ), &nSize );
		strProgID.ReleaseBuffer();
		if ( ret == ERROR_SUCCESS && strProgID.CompareNoCase( CLIENT_NAME ) == 0 )
			SHDeleteKey( hRootKey, strUrlAssociations );
	}
	else if ( pszRoot && *pszRoot == _T('.') )
	{
		// Get real key for file extension
		if ( RegOpenKeyEx( hRootKey, strSubKey, 0, KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS )
		{
			CString strPath;
			DWORD dwType;
			DWORD dwSize = MAX_PATH * sizeof( TCHAR );
			RegQueryValueEx( hKey, NULL, NULL, &dwType, (LPBYTE)strPath.GetBuffer( MAX_PATH ), &dwSize );
			strPath.ReleaseBuffer();
			if ( ! strPath.IsEmpty() )
			{
				strOldKey = strSubKey;
				strSubKey = szRootKey;
				if ( ! strSubKey.IsEmpty() )
					strSubKey += _T("\\");
				strSubKey += strPath;
			}
			RegCloseKey( hKey );
		}
	}

	if ( RegOpenKeyEx( hRootKey, strSubKey + _T("\\shell\\open\\command"), 0, KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS )
	{
		CString strPath;
		DWORD dwType, dwSize = MAX_PATH * sizeof( TCHAR );
		RegQueryValueEx( hKey, NULL, NULL, &dwType, (LPBYTE)strPath.GetBuffer( MAX_PATH ), &dwSize );
		strPath.ReleaseBuffer();
		if ( _tcsistr( strPath, theApp.m_strBinaryPath ) != NULL )
			bRegisteredUser = TRUE;

		RegCloseKey( hKey );
	}

	if ( ! bRegisteredUser )
	{
		if ( RegOpenKeyEx( hRootKey, strSubKey + _T("\\shell\\open\\ddeexec\\Application"), 0, KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS )
		{
			CString strPath;
			DWORD dwType, dwSize = MAX_PATH * sizeof( TCHAR );
			RegQueryValueEx( hKey, NULL, NULL, &dwType, (LPBYTE)strPath.GetBuffer( MAX_PATH ), &dwSize );
			strPath.ReleaseBuffer();
			if ( _tcsistr( strPath, CLIENT_NAME ) != NULL )
				bRegisteredUser = TRUE;

			RegCloseKey( hKey );
		}
	}

	if ( bRegisteredUser )
		SHDeleteKey( hRootKey, strOldKey.IsEmpty() ? (LPCTSTR)strSubKey : (LPCTSTR)strOldKey );

	return bRegisteredUser;
}

// Note recursive delete moved to CRegistry::DeleteKey(HKEY, LPCTSTR), SHDeleteKey is used

/////////////////////////////////////////////////////////////////////////////
// CPeerProjectURL magnet registration helper

BOOL CPeerProjectURL::RegisterMagnetHandler(LPCTSTR pszID, LPCTSTR pszName, LPCTSTR pszDescription, LPCTSTR pszApplication, UINT nIDIcon)
{
	HKEY hSoftware, hMagnetRoot, hHandlers, hHandler;
	DWORD dwDisposition;
	LONG lResult;

	lResult = RegOpenKeyEx( HKEY_CURRENT_USER, _T("Software"),
		0, KEY_ALL_ACCESS, &hSoftware );

	if ( lResult != ERROR_SUCCESS ) return FALSE;

	lResult = RegCreateKeyEx( hSoftware, _T("Magnet"),
		0, NULL, 0, KEY_ALL_ACCESS, NULL, &hMagnetRoot, &dwDisposition );

	if ( lResult != ERROR_SUCCESS )
	{
		RegCloseKey( hSoftware );
		return FALSE;
	}

	lResult = RegCreateKeyEx( hMagnetRoot, _T("Handlers"),
		0, NULL, 0, KEY_ALL_ACCESS, NULL, &hHandlers, &dwDisposition );

	if ( lResult != ERROR_SUCCESS )
	{
		RegCloseKey( hMagnetRoot );
		RegCloseKey( hSoftware );
		return FALSE;
	}

	lResult = RegCreateKeyEx( hHandlers, pszID,
		0, NULL, 0, KEY_ALL_ACCESS, NULL, &hHandler, &dwDisposition );

	if ( lResult != ERROR_SUCCESS )
	{
		RegCloseKey( hHandler );
		RegCloseKey( hMagnetRoot );
		RegCloseKey( hSoftware );
		return FALSE;
	}

	CString strCommand;
	CString strIcon( Skin.GetImagePath( nIDIcon ) );
	strCommand.Format( _T("\"%s\" \"%%URL\""), theApp.m_strBinaryPath );

	RegSetValueEx( hHandler, _T(""), 0, REG_SZ,
		(LPBYTE)pszName, static_cast< DWORD >( sizeof(TCHAR) * ( _tcslen( pszName ) + 1 ) ) );

	RegSetValueEx( hHandler, _T("Description"), 0, REG_SZ,
		(LPBYTE)pszDescription, static_cast< DWORD >( sizeof(TCHAR) * ( _tcslen( pszDescription ) + 1 ) ) );

	RegSetValueEx( hHandler, _T("DefaultIcon"), 0, REG_SZ,
		(LPBYTE)(LPCTSTR)strIcon, sizeof(TCHAR) * ( strIcon.GetLength() + 1 ) );

	RegSetValueEx( hHandler, _T("ShellExecute"), 0, REG_SZ,
		(LPBYTE)(LPCTSTR)strCommand, sizeof(TCHAR) * ( strCommand.GetLength() + 1 ) );

	RegSetValueEx( hHandler, _T("DdeApplication"), 0, REG_SZ,
		(LPBYTE)pszApplication, static_cast< DWORD >( sizeof(TCHAR) * ( _tcslen( pszApplication ) + 1 ) ) );

	RegSetValueEx( hHandler, _T("DdeTopic"), 0, REG_SZ,
		(LPBYTE)_T("URL"), sizeof(TCHAR) * 4 );

	RegCloseKey( hHandler );
	RegCloseKey( hHandlers );
	RegCloseKey( hMagnetRoot );
	RegCloseKey( hSoftware );

	return TRUE;
}
