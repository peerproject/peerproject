//
// Class.cpp : Implementation of CClass
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2007.
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

#include "StdAfx.h"
#include "Class.h"

class ATL_NO_VTABLE CZipHandler
{
public:
	inline CZipHandler( LPCTSTR szFilename ) throw()
	{
		hArchive = unzOpen( CW2A( szFilename ) );
		if ( ! hArchive )
		{
			TCHAR szFileShort[ MAX_PATH ];
			if ( GetShortPathName( szFilename, szFileShort, MAX_PATH ) )
				hArchive = unzOpen( CW2A( szFileShort ) );
		}
	}

	inline ~CZipHandler() throw()
	{
		if ( hArchive )
		{
			unzClose( hArchive );
			hArchive = NULL;
		}
	}

	inline operator unzFile() const throw()
	{
		return hArchive;
	}

protected:
	unzFile hArchive;
};

STDMETHODIMP CSkinScan::Process (
	/* [in] */ HANDLE /* hFile */,
	/* [in] */ BSTR sFile,
	/* [in] */ ISXMLElement* pXML)
{
	if ( ! pXML )
		return E_POINTER;

	CZipHandler pFile( sFile );
	if ( ! pFile )
		return E_FAIL;

	unz_global_info pDir = {};
	if ( unzGetGlobalInfo( pFile, &pDir ) != UNZ_OK )
	{
		// Bad format. Call CLibraryBuilder::SubmitCorrupted()
		return E_UNEXPECTED;
	}

	for ( UINT nFile = 0; nFile < pDir.number_entry; nFile++ )
	{
		unz_file_info pInfo = {};
		CHAR szFile[ MAX_PATH ];

		if ( nFile && unzGoToNextFile( pFile ) != UNZ_OK )
		{
			// Bad format. Call CLibraryBuilder::SubmitCorrupted()
			return E_UNEXPECTED;
		}

		if ( unzGetCurrentFileInfo( pFile, &pInfo, szFile,
			MAX_PATH, NULL, 0, NULL, 0 ) != UNZ_OK )
		{
			// Bad format. Call CLibraryBuilder::SubmitCorrupted()
			return E_UNEXPECTED;
		}

		if ( lstrcmpiA( szFile + lstrlenA( szFile ) - 4, ".xml" ) == 0 )
		{
			if ( unzOpenCurrentFile( pFile ) != UNZ_OK )
			{
				// Bad format. Call CLibraryBuilder::SubmitCorrupted()
				return E_UNEXPECTED;
			}

			LPSTR pszXML = new CHAR[ pInfo.uncompressed_size + 1 ];
			if ( ! pszXML )
			{
				unzCloseCurrentFile( pFile );
				return E_OUTOFMEMORY;
			}
			ZeroMemory( pszXML, pInfo.uncompressed_size + 1 );

			if ( unzReadCurrentFile( pFile, pszXML, pInfo.uncompressed_size ) < 0 )
			{
				delete [] pszXML;
				unzCloseCurrentFile( pFile );
				// Bad format. Call CLibraryBuilder::SubmitCorrupted()
				return E_UNEXPECTED;
			}

			pszXML[ pInfo.uncompressed_size ] = 0;

			if ( ScanFile( pszXML, pXML ) )
			{
				delete [] pszXML;
				unzCloseCurrentFile( pFile );
				return S_OK;
			}

			delete [] pszXML;
			unzCloseCurrentFile( pFile );
		}
	}

	return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
// 
// ScanFile() is a helper function which accepts an XML string, decodes it,
// checks if it is a PeerProject skin file, and copies <manifest> metadata to the output.
// 
// pszXML  : The XML string
// pOutput : An empty XML element to build metadata in
// 

BOOL CSkinScan::ScanFile(LPCSTR pszXML, ISXMLElement* pOutput)
{
	// Put the XML string in a BSTR
	BOOL bBOMPresent = FALSE;
	if ( lstrlenA( pszXML ) > 3 && (UCHAR)pszXML[0] == 0xEF && 
		(UCHAR)pszXML[1] == 0xBB && (UCHAR)pszXML[2] == 0xBF )
	{
		bBOMPresent = TRUE;
		pszXML += 3;
	}
	int nLength = MultiByteToWideChar(CP_UTF8, 0, pszXML, lstrlenA(pszXML), NULL, 0);
	WCHAR* pszUnicode = new WCHAR[ nLength + 1 ];
	MultiByteToWideChar(CP_UTF8, 0, pszXML, lstrlenA(pszXML), pszUnicode, nLength);
	pszUnicode[ nLength ] = 0;
	CComBSTR sUnicode( pszUnicode );
	delete [] pszUnicode;
	if ( bBOMPresent ) pszXML -= 3;

	// Use the FromString() method in ISXMLElement to decode an XML document from the XML string.
	// Output is in "pFile".
	CComPtr< ISXMLElement > pFile;
	if ( FAILED( pOutput->FromString( sUnicode, &pFile ) ) || pFile == NULL )
	{
		return FALSE;
	}

	// Test if the root element of the document is called "skin"
	VARIANT_BOOL bNamed = VARIANT_FALSE;
	pFile->IsNamed( CComBSTR( _T("skin") ), &bNamed );
	if ( ! bNamed )
	{
		pFile->Delete();
		return FALSE;
	}

	// Get the Elements collection from the XML document
	CComPtr< ISXMLElements > pElements;
	if ( FAILED( pFile->get_Elements( &pElements ) ) || pElements == NULL )
	{
		pFile->Delete();
		return FALSE;
	}

	// Find the <manifest> element
	CComPtr< ISXMLElement > pManifest;
	if ( FAILED( pElements->get_ByName( CComBSTR( _T("manifest") ), &pManifest ) ) || pManifest == NULL )
	{
		pFile->Delete();
		return FALSE;
	}

	// Add the plural <PeerProjectSkins> element
	CComPtr< ISXMLElement > pPlural;
	{
		CComPtr< ISXMLElements > pElements;
		if ( FAILED( pOutput->get_Elements( &pElements ) ) || pElements == NULL )
		{
			pFile->Delete();
			return FALSE;
		}
		pElements->Create( CComBSTR( _T("PeerProjectSkins") ), &pPlural );
	}

	// Add xsi:noNamespaceSchemaLocation="http://schemas.peerproject.org/Skin.xsd"
	// to the Attributes collection of <PeerProjectSkins>
	{
		CComPtr< ISXMLAttributes > pAttrs;
		pPlural->get_Attributes( &pAttrs );
		pAttrs->Add( CComBSTR( _T("xsi:noNamespaceSchemaLocation") ),
			CComBSTR( _T("http://schemas.peerproject.org/Skin.xsd") ) );
	}

	// Change <manifest> to <PeerProjectSkin>
	pManifest->put_Name( CComBSTR( _T("PeerProjectSkin") ) );

	// Detach <manifest> from the file document and add it to the output XML document
	pManifest->Detach();
	{
		CComPtr< ISXMLElements > pElements;
		pPlural->get_Elements( &pElements );
		pElements->Attach( pManifest );
	}

	pFile->Delete();

	return TRUE;
}
