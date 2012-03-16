//
// Plugin.cpp : Implementation of CPlugin for SearchExport
//
// This file is part of PeerProject (peerproject.org) © 2009-2012
// Portions Previously Copyright Nikolay Raspopov, 2009.
//
// PeerProject is free software; you can redistribute it
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

#include "StdAfx.h"
#include "Plugin.h"

static const LPCWSTR SearchExport_CHECK	= L"&Export Results...";

// Insert menu item, if no item present
void InsertCommand(ISMenu* pMenu, int nPos, UINT nID, LPCWSTR szItem)
{
	LONG nCount;
	if ( SUCCEEDED( pMenu->get_Count( &nCount ) ) )
	{
		for ( int i = 0 ; i < (int)nCount ; ++i )
		{
			CComPtr< ISMenu > pItem;
			LONG nItemID;	// Note: -1 - submenu, 0 - separator
			if ( SUCCEEDED( pMenu->get_Item( CComVariant( i ), &pItem ) ) && pItem &&
				 SUCCEEDED( pItem->get_CommandID( &nItemID ) ) && (UINT)nItemID == nID )
				return;		// Already in place
		}
	}

	// Insert new
	pMenu->InsertCommand( nPos, nID, CComBSTR( szItem ), NULL );
}

#define KiloBytes	( 1024 )
#define MegaBytes	( KiloBytes * 1024 )
#define GigaBytes	( MegaBytes * 1024 )
#define KiloFloat	( 1024.0f )
#define MegaFloat	( KiloFloat * 1024.0f )
#define GigaFloat	( MegaFloat * 1024.0f )
#define TeraFloat	( GigaFloat * 1024.0f )
#define PetaFloat	( TeraFloat * 1024.0f )
#define ExaFloat	( PetaFloat * 1024.0f )

// Returns a nicely formatted string displaying a given volume
CString SmartVolume(ULONGLONG nVolume)
{
	CString strVolume;

	if ( nVolume < KiloBytes )					// Bytes
		strVolume.Format( _T("%I64u B"), nVolume );
	else if ( nVolume < 10 * KiloBytes )		// 1..10 KiloBytes
		strVolume.Format( _T("%.2f KB"), nVolume / KiloFloat );
	else if ( nVolume < MegaBytes )				// 10..1024 KiloBytes
		strVolume.Format( _T("%I64u KB"), nVolume / KiloBytes );
	else if ( nVolume < 100 * MegaBytes )		// 1..100 MegaBytes
		strVolume.Format( _T("%.2f MB"), nVolume / MegaFloat );
	else if ( nVolume < GigaBytes )				// 100..1024 MegaBytes
		strVolume.Format( _T("%.1f MB"), nVolume / MegaFloat );
	else if ( nVolume < TeraFloat )				// GigaBytes
		strVolume.Format( _T("%.2f GB"), nVolume / GigaFloat );
	else if ( nVolume < PetaFloat )				// Tera
		strVolume.Format( _T("%.2f TB"), nVolume / TeraFloat );
	else if ( nVolume < ExaFloat )				// Peta
		strVolume.Format( _T("%.2f PB"), nVolume / PetaFloat );
	else
		strVolume.Format( _T("%.2f EB"), nVolume / ExaFloat );

	return strVolume;
}

// Encodes unsafe characters in a string ("hello world" to "hello%20world")
CString URLEncode(LPCTSTR pszInputT)
{
	static LPCTSTR pszHex	= _T("0123456789ABCDEF");
	static LPCSTR pszUnsafe	= "<>\"#%{}|\\^~[]+?&@=:,";

	CString strOutput;

	if ( pszInputT == NULL || *pszInputT == 0 )
		return strOutput;

	int nUTF8 = WideCharToMultiByte( CP_UTF8, 0, pszInputT, -1, NULL, 0, NULL, NULL );
	if ( nUTF8 < 2 )
		return strOutput;

	CAutoVectorPtr< CHAR > pszUTF8( new CHAR[ nUTF8 ] );
	if ( ! pszUTF8 )
		return strOutput;

	WideCharToMultiByte( CP_UTF8, 0, pszInputT, -1, pszUTF8, nUTF8, NULL, NULL );
	pszUTF8[ nUTF8 - 1 ] = 0;

	LPCSTR pszInput = pszUTF8;
	LPTSTR pszOutput = strOutput.GetBuffer( ( nUTF8 - 1 ) * 3 + 1 );
	for ( ; *pszInput ; pszInput++ )
	{
		if ( *pszInput <= 32 || *pszInput > 127 || strchr( pszUnsafe, *pszInput ) != NULL )
		{
			*pszOutput++ = _T('%');
			*pszOutput++ = pszHex[ ( *pszInput >> 4 ) & 0x0F ];
			*pszOutput++ = pszHex[ *pszInput & 0x0F ];
		}
		else
			*pszOutput++ = (TCHAR)*pszInput;
	}
	*pszOutput = 0;
	strOutput.ReleaseBuffer();

	return strOutput;
}

// Class for creating .MHT-files
class CMultipartFile
{
public:
	CMultipartFile(LPCTSTR szRoot = _T(""))
		: m_sRoot( szRoot )
	{
	}

	void Add(LPCTSTR szSrc, LPCTSTR szURI)
	{
		CT2CA szSrcA( szSrc );
		int nSrcLen = lstrlenA( (LPCSTR)szSrcA );
		int nDstLen = Base64EncodeGetRequiredLength( nSrcLen, 0 );
		CStringA sDest;
		Base64Encode( (const BYTE*)(LPCSTR)szSrcA, nSrcLen,
			sDest.GetBuffer( nDstLen ), &nDstLen, 0 );
		sDest.ReleaseBuffer( nDstLen );
		m_Parts.AddTail( sDest );
		m_URIs.AddTail( szURI );
	}

	void AddFile(LPCTSTR szFilename)
	{
		CAtlFile fileIn;
		ULONGLONG nSrcLen;
		if ( SUCCEEDED( fileIn.Create( szFilename, GENERIC_READ, 0, OPEN_EXISTING ) ) &&
			 SUCCEEDED( fileIn.GetSize( nSrcLen ) ) )
		{
			CAutoVectorPtr< BYTE > szSrc( new BYTE[ (DWORD)nSrcLen ] );
			if ( szSrc )
			{
				if ( SUCCEEDED( fileIn.Read( szSrc, (DWORD)nSrcLen ) ) )
				{
					int nDstLen = Base64EncodeGetRequiredLength( (DWORD)nSrcLen, 0 );
					CStringA sDest;
					Base64Encode( szSrc, (DWORD)nSrcLen,
						sDest.GetBuffer( nDstLen ), &nDstLen, 0 );
					sDest.ReleaseBuffer( nDstLen );
					m_Parts.AddTail( sDest );
					m_URIs.AddTail( PathFindFileName ( szFilename ) );
				}
			}
		}
	}

	void AddFolder(LPCTSTR szFolder)
	{
		CString sFolder( szFolder );
		sFolder.TrimRight( _T('\\') ) += _T("\\");

		WIN32_FIND_DATA wfa = {};
		HANDLE hFind = FindFirstFile ( sFolder + _T("*.*"), &wfa );
		if ( hFind != INVALID_HANDLE_VALUE )
		{
			do
			{
				if ( ! ( wfa.dwFileAttributes & ( FILE_ATTRIBUTE_DIRECTORY |
					FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM ) ) )
				{
					AddFile( sFolder + wfa.cFileName );
				}
			}
			while ( FindNextFile( hFind, &wfa ) );
		}
	}

	BOOL Save(LPCTSTR szFilename) const
	{
		const CHAR szBoundary[] = { "----=_NextPart_000_0000_01C24B8C.A6928500" };

		// Header
		CString strHeader;
		strHeader.Format(
			_T("From: <Saved by PeerProject>\r\n")
			_T("Subject: Search Results\r\n")
			_T("MIME-Version: 1.0\r\n")
			_T("Content-Type: multipart/related;\r\n")
			_T("\ttype=\"text/html\";\r\n")
			_T("\tboundary=\"%hs\"\r\n")
			_T("\r\n")
			_T("This is a multi-part message in MIME format.\r\n")
			_T("\r\n"),
			szBoundary );

		CAtlFile fileOut;
		if ( SUCCEEDED( fileOut.Create( szFilename, GENERIC_WRITE, 0, CREATE_ALWAYS ) ) &&
			 SUCCEEDED( fileOut.Write( (LPCSTR)CT2A( strHeader ), strHeader.GetLength() ) ) )
		{
			// Content
			for ( POSITION pos1 = m_Parts.GetHeadPosition(),
				pos2 = m_URIs.GetHeadPosition(); pos1 && pos2; )
			{
				const CStringA& sPart = m_Parts.GetNext( pos1 );
				const CString& sURI = m_URIs.GetNext( pos2 );

				TCHAR szType[ 64 ] = { _T("application/octet-stream") };
				DWORD dwType = REG_SZ, dwTypeSize = sizeof( szType );
				SHGetValue( HKEY_CLASSES_ROOT, PathFindExtension( sURI ),
					_T("Content Type"), &dwType, szType, &dwTypeSize );

				strHeader.Format (
					_T("--%hs\r\n")
					_T("Content-Type: %s\r\n")
					_T("Content-Transfer-Encoding: base64\r\n")
					_T("Content-Location: %s%s\r\n")
					_T("\r\n"),
					szBoundary, szType, m_sRoot, sURI );

				fileOut.Write( (LPCSTR)CT2A( strHeader ), strHeader.GetLength() );
				fileOut.Write( sPart, sPart.GetLength() );
				fileOut.Write( "\r\n\r\n", 4 );
			}

			// Footer
			fileOut.Write( "--", 2 );
			fileOut.Write( szBoundary, sizeof( szBoundary ) - 1 );
			fileOut.Write( "--\r\n", 4 );

			return TRUE;
		}
		return FALSE;
	}

protected:
	CString					m_sRoot;	// Root URL (fake)
	CAtlList< CStringA >	m_Parts;	// Base64 encoded parts
	CAtlList< CString >		m_URIs;		// Part's filenames
};

HRESULT CPlugin::Export(IGenericView* pGenericView, LONG nCount)
{
	HRESULT hr;

	CComPtr< ISettings > pSettings;
	hr = m_pApplication->get_Settings( &pSettings );
	if ( FAILED( hr ) )
		return hr;

	// Get template directory path
	CComVariant var( CComBSTR( _T("General.Path") ) );
	hr = pSettings->GetValue( &var );
	ATLASSERT( SUCCEEDED( hr ) && var.vt == VT_BSTR );
	CString sPath( var.bstrVal );
	sPath += _T("\\Templates\\");

	// Get main window handle
	HWND hWnd = NULL;
	m_pUserInterface->get_MainWindowHwnd( &hWnd );

	// Choose file to save
	TCHAR szFilename[ MAX_PATH ] = {};
	OPENFILENAME ofn = {
		sizeof( OPENFILENAME ), hWnd, NULL,
		_T("MHT Files (*.mht)\0*.mht\0\0"),
		NULL, 0, 1, szFilename, MAX_PATH, NULL, 0, NULL, NULL,
		OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR |
		OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN, 0, 0, _T("mht")
	};
	if ( ! GetSaveFileName( &ofn ) )
		return S_OK;

	// Create index file
	CString sHTML =
		_T("<html>\r\n")
		_T("<head>\r\n")
		_T("\t<link rel=\"stylesheet\" media=\"all\" type=\"text/css\" href=\"SearchExport.css\">\r\n")
		_T("</head>\r\n")
		_T("<body>\r\n")
		_T("\t<div class=\"pg\">\r\n")
		_T("\t\t<a href=\"http://peerproject.org/\"><div class=\"hd\"></div></a>\r\n");

	CAtlMap< CComBSTR, bool > oSHA1Map, oTigerMap, oED2KMap, oMD5Map;
	int n = 1;
	for ( LONG i = 0; i < nCount; ++i )
	{
		CComVariant pItem;
		hr = pGenericView->get_Item( CComVariant( i ), &pItem );
		if ( FAILED( hr ) || pItem.vt != VT_DISPATCH )
			continue;

		CComQIPtr< IPeerProjectFile > pPeerProjectFile( pItem.pdispVal );
		if ( ! pPeerProjectFile )
			continue;

		CComBSTR bstrSHA1, bstrTiger, bstrED2K, bstrMD5;
		pPeerProjectFile->get_Hash( URN_SHA1,  ENCODING_BASE32, &bstrSHA1 );
		pPeerProjectFile->get_Hash( URN_TIGER, ENCODING_BASE32, &bstrTiger );
		pPeerProjectFile->get_Hash( URN_ED2K,  ENCODING_BASE16, &bstrED2K );
		pPeerProjectFile->get_Hash( URN_MD5,   ENCODING_BASE16, &bstrMD5 );

		// Remove dups
		bool bFake;
		if ( bstrSHA1.Length() && oSHA1Map.Lookup( bstrSHA1, bFake ) )
			continue;
		if ( bstrTiger.Length() && oTigerMap.Lookup( bstrTiger, bFake ) )
			continue;
		if ( bstrED2K.Length() && oED2KMap.Lookup( bstrED2K, bFake ) )
			continue;
		if ( bstrMD5.Length() && oMD5Map.Lookup( bstrMD5, bFake ) )
			continue;

		if ( bstrSHA1.Length() )
			oSHA1Map.SetAt( bstrSHA1, true );
		if ( bstrTiger.Length() )
			oTigerMap.SetAt( bstrTiger, true );
		if ( bstrED2K.Length() )
			oED2KMap.SetAt( bstrED2K, true );
		if ( bstrMD5.Length() )
			oMD5Map.SetAt( bstrMD5, true );

		CString str;
		str.Format( _T("\t\t<div class=\"f%d\">")
			_T("<span class=\"f_\">%d</span>")
			_T("<span class=\"fn\"><a href=\"magnet:?"),
			( n & 1 ), n );
		sHTML += str;

		n++;

		if ( bstrSHA1.Length() && bstrTiger.Length() )
			sHTML = sHTML + _T("xt=urn:bitprint:") + bstrSHA1 + _T(".") + bstrTiger + _T("&amp;");
		else if ( bstrSHA1.Length() )
			sHTML = sHTML + _T("xt=urn:sha1:") + bstrSHA1 + _T("&amp;");
		else if ( bstrTiger.Length() )
			sHTML = sHTML + _T("xt=urn:tree:tiger/:") + bstrTiger + _T("&amp;");

		if ( bstrED2K.Length() )
			sHTML = sHTML + _T("xt=urn:ed2k:") + bstrED2K + _T("&amp;");
		if ( bstrMD5.Length() )
			sHTML = sHTML + _T("xt=urn:md5:") + bstrMD5 + _T("&amp;");

		CComBSTR bstrName;
		hr = pPeerProjectFile->get_Name( &bstrName );
		ATLASSERT( SUCCEEDED( hr ) );

		ULONGLONG nSize = 0;
		hr = pPeerProjectFile->get_Size( &nSize );
		ATLASSERT( SUCCEEDED( hr ) );

		str.Format( _T("dn=%s&amp;xl=%I64u\">%s</a></span>")
			_T("<span class=\"fs\">%s</span></div>\r\n"),
			URLEncode( bstrName ), nSize, bstrName, SmartVolume( nSize ) );
		sHTML += str;
	}

	//Date String
	TCHAR szBuffer[12];
	SYSTEMTIME time;
	GetSystemTime(&time);
	GetDateFormat( LOCALE_USER_DEFAULT, DATE_SHORTDATE,
		&time, NULL, szBuffer, _countof( szBuffer ) );
	CString sDate = (LPCTSTR)szBuffer;

	sHTML +=
		_T("\t\t<div class=\"ft\">")
		+ sDate +
		_T("</div>\r\n")
		_T("\t</div>\r\n")
		_T("</body>\r\n")
		_T("</html>\r\n");

	// Save .mht-file
	CMultipartFile fileExport;
	fileExport.Add( sHTML, _T("index.html") );
	fileExport.AddFolder( sPath );
	if ( fileExport.Save( ofn.lpstrFile ) )	// Show result
		ShellExecute ( hWnd, NULL, ofn.lpstrFile, NULL, NULL, SW_SHOWNORMAL);

	return S_OK;
}

// IGeneralPlugin

STDMETHODIMP CPlugin::SetApplication(
	/*[in]*/ IApplication __RPC_FAR *pApplication)
{
	if ( ! pApplication )
		return E_POINTER;

	m_pApplication = pApplication;
	return m_pApplication->get_UserInterface( &m_pUserInterface );
}

STDMETHODIMP CPlugin::QueryCapabilities(
	/*[in]*/ DWORD __RPC_FAR *pnCaps)
{
	if ( ! pnCaps )
		return E_POINTER;

	return S_OK;
}

STDMETHODIMP CPlugin::Configure()
{
	return S_OK;
}

STDMETHODIMP CPlugin::OnSkinChanged()
{
	// Recreate lost menu items
	return InsertCommands();
}

// ICommandPlugin

STDMETHODIMP CPlugin::RegisterCommands()
{
	if ( ! m_pUserInterface )
		return E_UNEXPECTED;

	HRESULT hr = m_pUserInterface->RegisterCommand( CComBSTR( L"PluginID_SearchExport_Check" ),
		LoadIcon( _AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE( IDI_ICON ) ), &m_nCmdCheck );
	if ( SUCCEEDED( hr ) )
		return S_OK;

	return E_FAIL;
}

STDMETHODIMP CPlugin::InsertCommands()
{
	if ( ! m_pUserInterface )
		return E_UNEXPECTED;

	CComPtr< ISMenu > pSearchMenu;
	if ( SUCCEEDED( m_pUserInterface->GetMenu( CComBSTR( L"CSearchWnd" ),
		VARIANT_FALSE, &pSearchMenu ) ) && pSearchMenu )
	{
		InsertCommand( pSearchMenu, 11, m_nCmdCheck, SearchExport_CHECK );
	}

	return E_FAIL;
}

STDMETHODIMP CPlugin::OnUpdate(
    /*[in]*/ UINT nCommandID,
    /*[out][in]*/ TRISTATE __RPC_FAR *pbVisible,
    /*[out][in]*/ TRISTATE __RPC_FAR *pbEnabled,
    /*[out][in]*/ TRISTATE __RPC_FAR *pbChecked)
{
	if ( ! pbVisible || ! pbEnabled || ! pbChecked )
		return E_POINTER;

	if ( ! m_pUserInterface )
		return E_UNEXPECTED;

	if ( nCommandID == m_nCmdCheck )
	{
		*pbEnabled = TRI_FALSE;
		*pbVisible = TRI_TRUE;
		*pbChecked = TRI_UNKNOWN;

		CComPtr< IGenericView > pGenericView;
		HRESULT hr = m_pUserInterface->get_ActiveView( &pGenericView );
		if ( SUCCEEDED( hr ) && pGenericView )
		{
			LONG nCount = 0;
			hr = pGenericView->get_Count( &nCount );
			if ( SUCCEEDED( hr ) && nCount )
				*pbEnabled = TRI_TRUE;
		}
		return S_OK;
	}

	return S_FALSE;
}

STDMETHODIMP CPlugin::OnCommand(
	/*[in]*/ UINT nCommandID)
{
	if ( ! m_pUserInterface )
		return E_UNEXPECTED;

	if ( nCommandID == m_nCmdCheck )
	{
		CComPtr< IGenericView > pGenericView;
		HRESULT hr = m_pUserInterface->get_ActiveView( &pGenericView );
		if ( SUCCEEDED( hr ) && pGenericView )
		{
			LONG nCount = 0;
			hr = pGenericView->get_Count( &nCount );
			if ( SUCCEEDED( hr ) && nCount )
				return Export( pGenericView, nCount );
		}
	}

	return S_FALSE;
}
