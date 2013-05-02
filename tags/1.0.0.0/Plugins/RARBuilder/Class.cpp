//
// Class.cpp : Implementation of CClass (Rar)
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

#define MAX_SIZE_FILES		80
#define MAX_SIZE_FOLDERS	80
#define MAX_SIZE_COMMENTS	256

class ATL_NO_VTABLE CRarHandler
{
public:
	inline CRarHandler( RAROpenArchiveDataEx* oad ) throw()
	{
		hArchive = fnRAROpenArchiveEx( oad );
	}

	inline ~CRarHandler() throw()
	{
		if ( hArchive )
		{
			fnRARCloseArchive( hArchive );
			hArchive = NULL;
		}
	}

	inline operator HANDLE() const throw()
	{
		return hArchive;
	}

protected:
	HANDLE hArchive;
};

STDMETHODIMP CRARBuilder::Process(/*[in]*/ BSTR sFile, /*[in]*/ ISXMLElement* pXML)
{
	if ( ! pXML )
		return E_POINTER;

	if ( ! fnRAROpenArchiveEx || ! fnRARCloseArchive || ! fnRARReadHeaderEx || ! fnRARProcessFileW )
		return E_NOTIMPL;		// Unrar.dll not loaded

	CComPtr <ISXMLElements> pISXMLRootElements;
	HRESULT hr = pXML->get_Elements( &pISXMLRootElements );
	if ( FAILED( hr ) ) return hr;
	CComPtr <ISXMLElement> pXMLRootElement;
	hr = pISXMLRootElements->Create( CComBSTR( "archives" ), &pXMLRootElement );
	if ( FAILED( hr ) ) return hr;
	CComPtr <ISXMLAttributes> pISXMLRootAttributes;
	hr = pXMLRootElement->get_Attributes( &pISXMLRootAttributes );
	if ( FAILED( hr ) ) return hr;
	pISXMLRootAttributes->Add( CComBSTR( "xmlns:xsi" ),
		CComBSTR( "http://www.w3.org/2001/XMLSchema-instance" ) );
	pISXMLRootAttributes->Add( CComBSTR( "xsi:noNamespaceSchemaLocation" ),
		CComBSTR( "http://www.shareaza.com/schemas/archive.xsd" ) );

	CComPtr <ISXMLElements> pISXMLElements;
	hr = pXMLRootElement->get_Elements( &pISXMLElements );
	if ( FAILED( hr ) ) return hr;
	CComPtr <ISXMLElement> pXMLElement;
	hr = pISXMLElements->Create( CComBSTR( "archive" ), &pXMLElement );
	if ( FAILED( hr ) ) return hr;
	CComPtr <ISXMLAttributes> pISXMLAttributes;
	hr = pXMLElement->get_Attributes( &pISXMLAttributes );
	if ( FAILED( hr ) ) return hr;

	CString strFiles;				// Plain list of archive files
	bool bMoreFiles = false;		// More files than listed in sFiles
	CString strFolders;				// Plain list of archive folders
	bool bMoreFolders = false;		// More folders than listed in sFolders
	CString strComment;				// Archive comments
	bool bEncrypted = false;		// Archive itself or selective files are encrypted
	ULONGLONG nUnpackedSize = 0;	// Total size of unpacked files
	int nFileCount = 0;				// Total number of contained files

	char szCmtBuf[ MAX_SIZE_COMMENTS ] = {};
	RAROpenArchiveDataEx oad = {};
	oad.ArcNameW = sFile;
	oad.CmtBuf = szCmtBuf;
	oad.CmtBufSize = sizeof( szCmtBuf );
	oad.OpenMode = RAR_OM_LIST;

	CRarHandler oArchive( &oad );

	switch( oad.OpenResult )
	{
	// Success
	case 0:
		switch( oad.CmtState )
		{
		case 0: 					// Comments not present
			break;

		case 1:						// Comments read completely
		case ERAR_SMALL_BUF:		// Buffer too small, comments not completely read
			szCmtBuf[ MAX_SIZE_COMMENTS - 1 ] = '\0';
			strComment = szCmtBuf;
			strComment.Replace( _T('\r'), _T(' ') );
			strComment.Replace( _T('\n'), _T(' ') );
			strComment.Replace( _T("  "), _T(" ") );
			break;

		case ERAR_NO_MEMORY:		// Not enough memory to extract comments
			return E_OUTOFMEMORY;

		case ERAR_BAD_DATA:			// Broken comment
		case ERAR_UNKNOWN_FORMAT:	// Unknown comment format
			return E_UNEXPECTED;	// Bad format. Call CLibraryBuilder::SubmitCorrupted()

		default:					// Other errors
			return E_FAIL;
		}

		if ( ( oad.Flags & 0x0080 ) )
			bEncrypted = true;		// Block headers are encrypted

		// List all files
		for( int nResult = 0; nResult == 0; )
		{
			RARHeaderDataEx hd = {};
			nResult = fnRARReadHeaderEx( oArchive, &hd );
			switch ( nResult )
			{
			case 0:					// Success
			{
				CString strName( hd.FileNameW );
				int nBackSlashPos = strName.ReverseFind( _T('\\') );
				if ( nBackSlashPos >= 0 )
					strName = strName.Mid( nBackSlashPos + 1 );

				if ( ( hd.Flags & 0xe0 ) == 0xe0 )	// Folder
				{
					if ( strFolders.GetLength() + strName.GetLength() <= MAX_SIZE_FOLDERS - 5 )
					{
						if ( ! strFolders.IsEmpty() )
							strFolders += _T(", ");
						strFolders += strName;
					}
					else
						bMoreFolders = true;
				}
				else	// File
				{
					nFileCount++;

					if ( strFiles.GetLength() + strName.GetLength() <= MAX_SIZE_FILES - 5 )
					{
						if ( ! strFiles.IsEmpty() )
							strFiles += _T(", ");
						strFiles += strName;
					}
					else
						bMoreFiles = true;

					nUnpackedSize += hd.UnpSize;

					if ( ( hd.Flags & 0x04 ) )
						bEncrypted = true;	// File encrypted with password
				}
				break;
			}

			case ERAR_END_ARCHIVE:		// End of archive
				break;

			case ERAR_BAD_DATA:			// File header broken
				return E_UNEXPECTED;	// Bad format. Call CLibraryBuilder::SubmitCorrupted()

			case ERAR_UNKNOWN:			// Decryption errors (wrong password)
				break;

			default:					// Other errors
				return E_FAIL;
			}

			if ( nResult != 0 )
				break;

			nResult = fnRARProcessFileW( oArchive, RAR_SKIP, NULL, NULL );
			switch ( nResult )
			{
			case 0:						// Success
				break;

			case ERAR_BAD_DATA:			// File CRC error
			case ERAR_BAD_ARCHIVE:		// Volume is not valid RAR archive
			case ERAR_UNKNOWN_FORMAT:	// Unknown archive format
				return E_UNEXPECTED;	// Bad format. Call CLibraryBuilder::SubmitCorrupted()

			case ERAR_EOPEN:			// Volume open error (volume missing)
				break;

			case ERAR_ECREATE:			// File create error
			case ERAR_ECLOSE:			// File close error
			case ERAR_EREAD:			// Read error
			case ERAR_EWRITE:			// Write error
			default:					// Other errors
				return E_FAIL;
			}
		}

		if ( ! strFiles.IsEmpty() )
		{
			if ( bMoreFiles )
				strFiles += _T(", ...");
			pISXMLAttributes->Add( CComBSTR( "files" ), CComBSTR( strFiles ) );
		}

		if ( ! strFolders.IsEmpty() )
		{
			if ( bMoreFolders )
				strFolders += _T(", ...");
			pISXMLAttributes->Add( CComBSTR( "folders" ), CComBSTR( strFolders ) );
		}

		if ( ! strComment.IsEmpty() )
			pISXMLAttributes->Add( CComBSTR( "comments" ), CComBSTR( strComment ) );

		if ( bEncrypted )
			pISXMLAttributes->Add( CComBSTR( "encrypted" ), CComBSTR( "true" ) );

		if ( nUnpackedSize )
		{
			CString strUnpackedSize;
			strUnpackedSize.Format( _T("%I64u"), nUnpackedSize );
			pISXMLAttributes->Add( CComBSTR( "unpackedsize" ), CComBSTR( strUnpackedSize ) );
		}

		if ( nFileCount > 0 )
		{
			CString strFileCount;
			strFileCount.Format( _T("%i"), nFileCount );
			pISXMLAttributes->Add( CComBSTR( "filecount" ), CComBSTR( strFileCount ) );
		}

		// Special case .CBR - Common filename metadata
		{
			CString strName( sFile );
			if ( strName.Right( 4 ) == _T(".cbr") )
			{
				strName.MakeLower();
				if ( strName.Find( _T("minutemen") ) > 0 )
					pISXMLAttributes->Add( CComBSTR( "releasegroup" ), CComBSTR( L"Minutemen" ) );
				else if ( strName.Find( _T("dcp") ) > 0 )
					pISXMLAttributes->Add( CComBSTR( "releasegroup" ), CComBSTR( L"DCP" ) );
				else if ( strName.Find( _T("cps") ) > 0 )
					pISXMLAttributes->Add( CComBSTR( "releasegroup" ), CComBSTR( L"CPS" ) );
			}
			if ( strName.Find( _T("20") ) > 4 || strName.Find( _T("19") ) > 4 )
			{
				CString strYear;
				for ( int i = 2022; i > 1940; i-- )
				{
					strYear.Format( _T("%i"), i );
					if ( strName.Find( strYear ) > 4 )
					{
						pISXMLAttributes->Add( CComBSTR( "year" ), CComBSTR( strYear ) );
						break;
					}
				}
			}
		}

		return S_OK;

	case ERAR_NO_MEMORY:		// Not enough memory to initialize data structures
		return E_OUTOFMEMORY;

	case ERAR_BAD_DATA: 		// Archive header broken
	case ERAR_BAD_ARCHIVE:		// File is not valid RAR archive
	case ERAR_UNKNOWN_FORMAT:	// Unknown encryption used for archive headers
		return E_UNEXPECTED;	// Bad format. Call CLibraryBuilder::SubmitCorrupted()

	case ERAR_EOPEN:			// File open error
	default:					// Other errors
		return E_FAIL;
	}
}
