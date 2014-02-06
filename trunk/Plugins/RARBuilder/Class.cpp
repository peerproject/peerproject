//
// Class.cpp : Implementation of CClass (Rar)
//
// This file is part of PeerProject (peerproject.org) © 2008-2014
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

#define MAX_SIZE_FILES		128
#define MAX_SIZE_FOLDERS	128
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

STDMETHODIMP CRARBuilder::Process(
	/*[in]*/ BSTR sFile,
	/*[in]*/ ISXMLElement* pXML)
{
	if ( ! pXML )
		return E_POINTER;

	if ( ! fnRAROpenArchiveEx || ! fnRARCloseArchive || ! fnRARReadHeaderEx || ! fnRARProcessFileW )
		return E_NOTIMPL;		// Unrar.dll not loaded

	CComPtr <ISXMLElements> pISXMLRootElements;
	HRESULT hr = pXML->get_Elements( &pISXMLRootElements );
	if ( FAILED( hr ) ) return hr;
	CComPtr <ISXMLElement> pXMLRootElement;
	hr = pISXMLRootElements->Create( CComBSTR( L"archives" ), &pXMLRootElement );
	if ( FAILED( hr ) ) return hr;
	CComPtr <ISXMLAttributes> pISXMLRootAttributes;
	hr = pXMLRootElement->get_Attributes( &pISXMLRootAttributes );
	if ( FAILED( hr ) ) return hr;
	pISXMLRootAttributes->Add( CComBSTR( L"xmlns:xsi" ),
		CComBSTR( L"http://www.w3.org/2001/XMLSchema-instance" ) );
	pISXMLRootAttributes->Add( CComBSTR( L"xsi:noNamespaceSchemaLocation" ),
		CComBSTR( L"http://www.shareaza.com/schemas/archive.xsd" ) );

	CComPtr <ISXMLElements> pISXMLElements;
	hr = pXMLRootElement->get_Elements( &pISXMLElements );
	if ( FAILED( hr ) ) return hr;
	CComPtr <ISXMLElement> pXMLElement;
	hr = pISXMLElements->Create( CComBSTR( L"archive" ), &pXMLElement );
	if ( FAILED( hr ) ) return hr;
	CComPtr <ISXMLAttributes> pISXMLAttributes;
	hr = pXMLElement->get_Attributes( &pISXMLAttributes );
	if ( FAILED( hr ) ) return hr;

	CString strFiles;				// Plain list of archive files
	CString strFolders;				// Plain list of archive folders
	CString strComment;				// Archive comments
	bool bEncrypted = false;		// Archive itself or selective files are encrypted
	bool bMoreFiles = false;		// More files than listed in strFiles
	bool bMoreFolders = false;		// More folders than listed in strFolders
	ULONGLONG nUnpackedSize = 0;	// Total size of unpacked files
	int nFileCount = 0;				// Total number of contained files

	char szCmtBuf[ MAX_SIZE_COMMENTS ] = {};
	RAROpenArchiveDataEx oad = {};
	oad.ArcNameW = sFile;
	oad.CmtBuf = szCmtBuf;
	oad.CmtBufSize = sizeof( szCmtBuf );
	oad.OpenMode = RAR_OM_LIST;

	CRarHandler oArchive( &oad );
	CAtlMap< CString, CString > oFolderList;

	switch( oad.OpenResult )
	{
	// Success
	case ERAR_SUCCESS:	// 0
		switch( oad.CmtState )
		{
		case ERAR_SUCCESS:	// 0	// Comments not present
			break;

		case 1:						// Comments read completely
		case ERAR_SMALL_BUF:		// Buffer too small, comments not completely read
			szCmtBuf[ MAX_SIZE_COMMENTS - 1 ] = _T('\0');
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

		if ( ( oad.Flags & RAR_HEAD_ENCRYPTED ) )
			bEncrypted = true;		// Block headers are encrypted

		// List all files
		for( int nResult = ERAR_SUCCESS ; nResult == ERAR_SUCCESS ; )
		{
			RARHeaderDataEx hd = {};
			nResult = fnRARReadHeaderEx( oArchive, &hd );
			switch ( nResult )
			{
			case ERAR_SUCCESS:					// Success
			{
				if ( ( hd.Flags & RAR_FILE_ENCRYPTED ) )
					bEncrypted = true;	// File encrypted with password

				CString strName( hd.FileNameW );
				bool bFolder = false;

				// Get folder names from paths
				for ( int i = 0 ; ; )
				{
					CString strPart = strName.Tokenize( _T("\\"), i );
					if ( strPart.IsEmpty() )
						break;
					if ( i + 1 >= strName.GetLength() )
						break;	// Last part

					CString strPartLower = strPart;
					strPartLower.MakeLower();
					oFolderList.SetAt( strPartLower, strPart );
				}

				int nBackSlashPos = strName.ReverseFind( _T('\\') );
				if ( nBackSlashPos == strName.GetLength() - 1 )
				{
					bFolder = true;
					strName = strName.Left( nBackSlashPos );
					nBackSlashPos = strName.ReverseFind( _T('\\') );
				}
				if ( nBackSlashPos >= 0 )
					strName = strName.Mid( nBackSlashPos + 1 );

				if ( bFolder || ( hd.Flags & RAR_FILE_DIRECTORY ) == RAR_FILE_DIRECTORY )
				{
					CString strNameLower = strName;
					strNameLower.MakeLower();
					oFolderList.SetAt( strNameLower, strName );
				}
				else	// File
				{
					nUnpackedSize += hd.UnpSize;
					nFileCount++;

					if ( strFiles.GetLength() + strName.GetLength() > MAX_SIZE_FILES - 5 )
					{
						bMoreFiles = true;
						break;
					}

					if ( ! strFiles.IsEmpty() )
						strFiles += _T(", ");
					strFiles += strName;
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

			if ( nResult != ERAR_SUCCESS )
				break;

			nResult = fnRARProcessFileW( oArchive, RAR_SKIP, NULL, NULL );
			switch ( nResult )
			{
			case ERAR_SUCCESS:	// 0	// Success
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

		for ( POSITION pos = oFolderList.GetStartPosition() ; pos ; )
		{
			CString strName = oFolderList.GetNextValue( pos );

			if ( strFolders.GetLength() + strName.GetLength() > MAX_SIZE_FOLDERS - 5 )
			{
				bMoreFolders = true;
				continue;
			}

			if ( ! strFolders.IsEmpty() )
				strFolders += _T(", ");
			strFolders += strName;
		}

		if ( bMoreFiles )
			strFiles += _T(", ...");

		if ( bMoreFolders )
			strFolders += _T(", ...");

		if ( ! strFiles.IsEmpty() )
			pISXMLAttributes->Add( CComBSTR( L"files" ), CComBSTR( strFiles ) );

		if ( ! strFolders.IsEmpty() )
			pISXMLAttributes->Add( CComBSTR( L"folders" ), CComBSTR( strFolders ) );

		if ( ! strComment.IsEmpty() )
			pISXMLAttributes->Add( CComBSTR( L"comments" ), CComBSTR( strComment ) );

		if ( bEncrypted )
			pISXMLAttributes->Add( CComBSTR( L"encrypted" ), CComBSTR( L"true" ) );

		if ( nUnpackedSize )
		{
			CString strUnpackedSize;
			strUnpackedSize.Format( _T("%I64u"), nUnpackedSize );
			pISXMLAttributes->Add( CComBSTR( L"unpackedsize" ), CComBSTR( strUnpackedSize ) );
		}

		if ( nFileCount > 0 )
		{
			CString strFileCount;
			strFileCount.Format( _T("%i"), nFileCount );
			pISXMLAttributes->Add( CComBSTR( L"filecount" ), CComBSTR( strFileCount ) );
		}

		// Special case .CBR - Common filename metadata
		{
			CString strName( sFile );
			if ( strName.Right( 4 ) == _T(".cbr") )
			{
				strName.MakeLower();
				if ( strName.Find( _T("minutemen") ) > 0 )
					pISXMLAttributes->Add( CComBSTR( L"releasegroup" ), CComBSTR( L"Minutemen" ) );
				else if ( strName.Find( _T("dcp") ) > 0 )
					pISXMLAttributes->Add( CComBSTR( L"releasegroup" ), CComBSTR( L"DCP" ) );
				else if ( strName.Find( _T("cps") ) > 0 )
					pISXMLAttributes->Add( CComBSTR( L"releasegroup" ), CComBSTR( L"CPS" ) );
			}
			if ( strName.Find( _T("20") ) > 4 || strName.Find( _T("19") ) > 4 )
			{
				CString strYear;
				for ( int i = 2022; i > 1940; i-- )
				{
					strYear.Format( _T("%i"), i );
					if ( strName.Find( strYear ) > 4 )
					{
						pISXMLAttributes->Add( CComBSTR( L"year" ), CComBSTR( strYear ) );
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
