//
// PeerProjectFile.cpp
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
#include "PeerProject.h"
#include "PeerProjectFile.h"
#include "Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

inline bool validAndEqual(QWORD nLeft, QWORD nRight)
{
	return ( nLeft != SIZE_UNKNOWN && nRight != SIZE_UNKNOWN && nLeft == nRight );
}

inline bool validAndUnequal(QWORD nLeft, QWORD nRight)
{
	return ( nLeft != SIZE_UNKNOWN && nRight != SIZE_UNKNOWN && nLeft != nRight );
}

inline bool validAndEqual(const CString& sLeft, const CString& sRight)
{
	return ( sLeft.GetLength() && sRight.GetLength() && ! sLeft.CompareNoCase( sRight ) );
}

inline bool validAndUnequal(const CString& sLeft, const CString& sRight)
{
	return ( sLeft.GetLength() && sRight.GetLength() && sLeft.CompareNoCase( sRight ) );
}

// CPeerProjectFile

IMPLEMENT_DYNAMIC(CPeerProjectFile, CComObject)

BEGIN_INTERFACE_MAP(CPeerProjectFile, CComObject)
	INTERFACE_PART(CPeerProjectFile, IID_IPeerProjectFile, PeerProjectFile)
END_INTERFACE_MAP()

CPeerProjectFile::CPeerProjectFile()
	: m_nSize	( SIZE_UNKNOWN )
{
	EnableDispatch( IID_IPeerProjectFile );
}

CPeerProjectFile::CPeerProjectFile(const CPeerProjectFile& pFile)
	: m_sName	( pFile.m_sName )
	, m_nSize	( pFile.m_nSize )
	, m_oSHA1	( pFile.m_oSHA1 )
	, m_oTiger	( pFile.m_oTiger )
	, m_oED2K	( pFile.m_oED2K )
	, m_oBTH	( pFile.m_oBTH )
	, m_oMD5	( pFile.m_oMD5 )
	, m_sPath	( pFile.m_sPath )
	, m_sURL	( pFile.m_sURL )
{
	EnableDispatch( IID_IPeerProjectFile );
}

CPeerProjectFile& CPeerProjectFile::operator=(const CPeerProjectFile& pFile)
{
	m_sName = pFile.m_sName;
	m_nSize = pFile.m_nSize;
	m_oSHA1 = pFile.m_oSHA1;
	m_oTiger = pFile.m_oTiger;
	m_oED2K = pFile.m_oED2K;
	m_oBTH  = pFile.m_oBTH;
	m_oMD5  = pFile.m_oMD5;
	m_sPath = pFile.m_sPath;
	m_sURL  = pFile.m_sURL;
	return *this;
}

bool CPeerProjectFile::operator==(const CPeerProjectFile& pFile) const
{
	if ( this == &pFile )
		return true;	// Same object

	if ( validAndUnequal( m_nSize,  pFile.m_nSize  ) ||
		 validAndUnequal( m_oSHA1,  pFile.m_oSHA1  ) ||
		 validAndUnequal( m_oTiger, pFile.m_oTiger ) ||
		 validAndUnequal( m_oED2K,  pFile.m_oED2K  ) ||
		 validAndUnequal( m_oMD5,   pFile.m_oMD5   ) )
	{
		return false;	// Different sizes or hashes (excluding BitTorrent)
	}

	if ( validAndEqual( m_oSHA1,  pFile.m_oSHA1  ) ||
		 validAndEqual( m_oTiger, pFile.m_oTiger ) ||
		 validAndEqual( m_oED2K,  pFile.m_oED2K  ) ||
		 validAndEqual( m_oMD5,   pFile.m_oMD5   ) )
	{
		return true;	// Same hash (excluding BitTorrent)
	}

	if ( validAndEqual( m_oBTH,  pFile.m_oBTH  ) &&
		 validAndEqual( m_sName, pFile.m_sName ) )
	{
		return true;	// Same name and BitTorrent hash
	}

	return false;		// Insufficient data
}

bool CPeerProjectFile::operator!=(const CPeerProjectFile& pFile) const
{
	if ( this == &pFile )
		return false;	// Same object

	if ( validAndUnequal( m_nSize,  pFile.m_nSize  ) ||
		 validAndUnequal( m_oSHA1,  pFile.m_oSHA1  ) ||
		 validAndUnequal( m_oTiger, pFile.m_oTiger ) ||
		 validAndUnequal( m_oED2K,  pFile.m_oED2K  ) ||
		 validAndUnequal( m_oMD5,   pFile.m_oMD5   ) )
	{
		return true;	// Different sizes or hashes (excluding BitTorrent)
	}

	return false;		// Insufficient data
}

CString CPeerProjectFile::GetURL(const IN_ADDR& nAddress, WORD nPort) const
{
	CString strURL;
	if ( m_oSHA1 )
	{
		strURL.Format( _T("http://%s:%i/uri-res/N2R?%s"),
			(LPCTSTR)CString( inet_ntoa( nAddress ) ), nPort, (LPCTSTR)m_oSHA1.toUrn() );
	}
	else if ( m_oTiger )
	{
		strURL.Format( _T("http://%s:%i/uri-res/N2R?%s"),
			(LPCTSTR)CString( inet_ntoa( nAddress ) ), nPort, (LPCTSTR)m_oTiger.toUrn() );
	}
	else if ( m_oED2K )
	{
		strURL.Format( _T("http://%s:%i/uri-res/N2R?%s"),
			(LPCTSTR)CString( inet_ntoa( nAddress ) ), nPort, (LPCTSTR)m_oED2K.toUrn() );
	}
	else if ( m_oMD5 )
	{
		strURL.Format( _T("http://%s:%i/uri-res/N2R?%s"),
			(LPCTSTR)CString( inet_ntoa( nAddress ) ), nPort, (LPCTSTR)m_oMD5.toUrn() );
	}
	else if ( m_oBTH )
	{
		strURL.Format( _T("http://%s:%i/uri-res/N2R?%s"),
			(LPCTSTR)CString( inet_ntoa( nAddress ) ), nPort, (LPCTSTR)m_oBTH.toUrn() );
	}
	return strURL;
}

CString CPeerProjectFile::GetBitprint() const
{
	if ( m_oSHA1 || m_oTiger )
		return GetURN();

	return CString();
}

CString CPeerProjectFile::GetURN() const
{
	if ( m_oSHA1 && m_oTiger )
		return _T("urn:bitprint:") + m_oSHA1.toString() + _T('.') + m_oTiger.toString();
	if ( m_oSHA1 )
		return m_oSHA1.toUrn();
	if ( m_oTiger )
		return m_oTiger.toUrn();
	if ( m_oED2K )
		return m_oED2K.toUrn();
	if ( m_oBTH )
		return m_oBTH.toUrn();
	if ( m_oMD5 )
		return m_oMD5.toUrn();

	return CString();
}

CString CPeerProjectFile::GetFilename() const
{
	CString strFilename;
	if ( m_oTiger )
		strFilename = _T("ttr_")  + m_oTiger.toString();
	else if ( m_oSHA1 )
		strFilename = _T("sha1_") + m_oSHA1.toString();
	else if ( m_oED2K )
		strFilename = _T("ed2k_") + m_oED2K.toString();
	else if ( m_oBTH )
		strFilename = _T("btih_") + m_oBTH.toString();
	else if ( m_oMD5 )
		strFilename = _T("md5_")  + m_oMD5.toString();
	else if ( m_sName.GetLength() > 1 )			// Note intentional obfuscation. Need to avoid conflicts but random numbers can leave orphan partials, different .pd and .partial
		strFilename.Format( _T("file_%u%s"), m_sName, CTime::GetCurrentTime().Format( _T("%M%S") ).Mid( 1, 2 ) );
	else
		strFilename.Format( _T("file_%.2i%.2i%.2i"), GetRandomNum( 0, 99 ), GetRandomNum( 0, 99 ), GetRandomNum( 0, 99 ) );
	return strFilename;
}

bool CPeerProjectFile::SplitStringToURLs(LPCTSTR pszURLs, CMapStringToFILETIME& oUrls) const
{
	CString strURLs( pszURLs );

	// Fix buggy URLs
	strURLs.Replace( _T("Zhttp://"), _T("Z, http://") );
	strURLs.Replace( _T("Z%2C http://"), _T("Z, http://") );

	// Temporary replace quoted commas
	bool bQuote = false;
	for ( int nScan = 0 ; nScan < strURLs.GetLength() ; nScan++ )
	{
		if ( strURLs[ nScan ] == '\"' )
		{
			bQuote = ! bQuote;
			strURLs.SetAt( nScan, ' ' );
		}
		else if ( strURLs[ nScan ] == ',' && bQuote )
		{
			strURLs.SetAt( nScan, '\x1f' );
		}
	}

	int nStart = 0;
	for ( ;; )
	{
		CString strURL = strURLs.Tokenize( _T(","), nStart );
		if ( strURL.IsEmpty() )
			break;
		strURL.Replace( '\x1f', ',' );	// Restore quoted commas
		strURL.Trim();

		// Get time
		FILETIME tSeen = { 0, 0 };
		int nPos = strURL.ReverseFind( ' ' );
		if ( nPos > 8 && TimeFromString( strURL.Mid( nPos + 1 ).TrimLeft(), &tSeen ) )
			strURL = strURL.Left( nPos ).TrimRight();

		// Convert short "h.o.s.t:port" to full source URL
		nPos = strURL.Find( ':' );
		if ( nPos > 6 && strURL.GetLength() > nPos + 1 &&
			strURL.GetAt( nPos + 1 ) != '/' )
		{
			int nPort;
			if ( _stscanf( strURL.Mid( nPos + 1 ), _T("%i"), &nPort ) != 1 )
				nPort = 0;
			DWORD nAddress = inet_addr( CT2CA( strURL.Left( nPos ) ) );
			if ( nPort > 0 && nPort <= USHRT_MAX && nAddress != INADDR_NONE &&
				! Network.IsFirewalledAddress( (IN_ADDR*)&nAddress, TRUE ) &&
				! Network.IsReserved( (IN_ADDR*)&nAddress ) )
			{
				strURL = GetURL( *(IN_ADDR*)&nAddress, static_cast< WORD >( nPort ) );
			}
			else
			{
				strURL.Empty();
			}
		}

		if ( ! strURL.IsEmpty() )
		{
			strURL.Replace( _T("%2C"), _T(",") );
			oUrls.SetAt( strURL, tSeen );
		}
	}

	return ! oUrls.IsEmpty();
}

//////////////////////////////////////////////////////////////////////
// CPeerProjectFile Automation

IMPLEMENT_DISPATCH(CPeerProjectFile, PeerProjectFile)

STDMETHODIMP CPeerProjectFile::XPeerProjectFile::get_Path(BSTR FAR* psPath)
{
	METHOD_PROLOGUE( CPeerProjectFile, PeerProjectFile )
	*psPath = CComBSTR( pThis->m_sPath ).Detach();
	return S_OK;
}

STDMETHODIMP CPeerProjectFile::XPeerProjectFile::get_Name(BSTR FAR* psName)
{
	METHOD_PROLOGUE( CPeerProjectFile, PeerProjectFile )
	*psName = CComBSTR( pThis->m_sName ).Detach();
	return S_OK;
}

STDMETHODIMP CPeerProjectFile::XPeerProjectFile::get_Size(ULONGLONG FAR* pnSize)
{
	METHOD_PROLOGUE( CPeerProjectFile, PeerProjectFile )
	*pnSize = pThis->m_nSize;
	return S_OK;
}

STDMETHODIMP CPeerProjectFile::XPeerProjectFile::get_URN(BSTR sURN, BSTR FAR* psURN)
{
	METHOD_PROLOGUE( CPeerProjectFile, PeerProjectFile )

	CString strURN = sURN ? sURN : _T("");
	CComBSTR bstrURN;

	if ( strURN.IsEmpty() )
	{
		bstrURN = pThis->GetURN();
	}
	else if ( strURN.CompareNoCase( _T("urn:bitprint") ) == 0 )
	{
		if ( pThis->m_oSHA1 && pThis->m_oTiger ) bstrURN = _T("urn:bitprint:") + pThis->m_oSHA1.toString() + _T('.') + pThis->m_oTiger.toString();
	}
	else if ( strURN.CompareNoCase( _T("urn:sha1") ) == 0 )
	{
		if ( pThis->m_oSHA1 ) bstrURN = pThis->m_oSHA1.toUrn();
	}
	else if ( strURN.CompareNoCase( _T("urn:tree:tiger/") ) == 0 )
	{
		if ( pThis->m_oTiger ) bstrURN = pThis->m_oTiger.toUrn();
	}
	else if ( strURN.CompareNoCase( _T("urn:md5") ) == 0 )
	{
		if ( pThis->m_oMD5 ) bstrURN = pThis->m_oMD5.toUrn();
	}
	else if ( strURN.CompareNoCase( _T("urn:ed2k") ) == 0 )
	{
		if ( pThis->m_oED2K ) bstrURN = pThis->m_oED2K.toUrn();
	}
	else if ( strURN.CompareNoCase( _T("urn:btih") ) == 0 )
	{
		if ( pThis->m_oBTH ) bstrURN = pThis->m_oBTH.toUrn();
	}

	*psURN = bstrURN.Detach();

	return S_OK;
}

STDMETHODIMP CPeerProjectFile::XPeerProjectFile::get_Hash(URN_TYPE nType, ENCODING nEncoding, BSTR FAR* psURN)
{
	METHOD_PROLOGUE( CPeerProjectFile, PeerProjectFile )

	CComBSTR bstrURN;

	switch ( nType )
	{
	case URN_SHA1:
		if ( pThis->m_oSHA1 )
		{
			switch ( nEncoding )
			{
			case ENCODING_GUID:
				bstrURN = pThis->m_oSHA1.toString< Hashes::guidEncoding >();
				break;
			case ENCODING_BASE16:
				bstrURN = pThis->m_oSHA1.toString< Hashes::base16Encoding >();
				break;
			case ENCODING_BASE32:
				bstrURN = pThis->m_oSHA1.toString< Hashes::base32Encoding >();
				break;
			//default:
			//	;
			}
		}
		break;

	case URN_TIGER:
		if ( pThis->m_oTiger )
		{
			switch ( nEncoding )
			{
			case ENCODING_GUID:
				bstrURN = pThis->m_oTiger.toString< Hashes::guidEncoding >();
				break;
			case ENCODING_BASE16:
				bstrURN = pThis->m_oTiger.toString< Hashes::base16Encoding >();
				break;
			case ENCODING_BASE32:
				bstrURN = pThis->m_oTiger.toString< Hashes::base32Encoding >();
				break;
			//default:
			//	;
			}
		}
		break;

	case URN_ED2K:
		if ( pThis->m_oED2K )
		{
			switch ( nEncoding )
			{
			case ENCODING_GUID:
				bstrURN = pThis->m_oED2K.toString< Hashes::guidEncoding >();
				break;
			case ENCODING_BASE16:
				bstrURN = pThis->m_oED2K.toString< Hashes::base16Encoding >();
				break;
			case ENCODING_BASE32:
				bstrURN = pThis->m_oED2K.toString< Hashes::base32Encoding >();
				break;
			//default:
			//	;
			}
		}
		break;

	case URN_MD5:
		if ( pThis->m_oMD5 )
		{
			switch ( nEncoding )
			{
			case ENCODING_GUID:
				bstrURN = pThis->m_oMD5.toString< Hashes::guidEncoding >();
				break;
			case ENCODING_BASE16:
				bstrURN = pThis->m_oMD5.toString< Hashes::base16Encoding >();
				break;
			case ENCODING_BASE32:
				bstrURN = pThis->m_oMD5.toString< Hashes::base32Encoding >();
				break;
			//default:
			//	;
			}
		}
		break;

	case URN_BTIH:
		if ( pThis->m_oBTH )
		{
			switch ( nEncoding )
			{
			case ENCODING_GUID:
				bstrURN = pThis->m_oBTH.toString< Hashes::guidEncoding >();
				break;
			case ENCODING_BASE16:
				bstrURN = pThis->m_oBTH.toString< Hashes::base16Encoding >();
				break;
			case ENCODING_BASE32:
				bstrURN = pThis->m_oBTH.toString< Hashes::base32Encoding >();
				break;
			//default:
			//	;
			}
		}
		break;

	//default:
	//	;
	}

	*psURN = bstrURN.Detach();

	return S_OK;
}

STDMETHODIMP CPeerProjectFile::XPeerProjectFile::get_URL(BSTR FAR* psURL)
{
	METHOD_PROLOGUE( CPeerProjectFile, PeerProjectFile )
	*psURL = CComBSTR( pThis->m_sURL ).Detach();
	return S_OK;
}
