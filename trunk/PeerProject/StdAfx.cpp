//
// StdAfx.cpp
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

#include "StdAfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

__int64 GetMicroCount()
{
	static __int64 Freq = 0;
	static __int64 FirstCount = 0;
	if ( Freq < 0 )
		return GetTickCount() * 1000;

	if ( Freq == 0 )
	{
		if ( ! QueryPerformanceFrequency( (LARGE_INTEGER*)&Freq ) )
		{
			Freq = -1;
			return GetMicroCount();
		}
		QueryPerformanceCounter( (LARGE_INTEGER*)&FirstCount );
	}
	__int64 Count = 0;
	QueryPerformanceCounter( (LARGE_INTEGER*)&Count );
	return ( 1000000 * ( Count - FirstCount ) ) / Freq;
}

class InitGetMicroCount
{
public:
	inline InitGetMicroCount() throw() { GetMicroCount(); }
};

InitGetMicroCount initGetMicroCount;

static const UINT primes[] =
{
	11,			13,			17,			19,			23,			29,
	31,			61,			127,		251,		347,		509,
	631,		761,		887,		1021,		1531,		2039,
	3067,		4093,		5119,		6143,		7159,		8191,
	9209,		10223,		11261,		12227,		13309,		14327,
	16381,		20479,		24571,		28669,		32749,		49139,
	65521,		98299,		131071,		196597,		262139,		327673
};

UINT GetBestHashTableSize(UINT nCount)
{
	const UINT* last  = primes + ( sizeof( primes ) / sizeof( primes[ 0 ] ) - 1 );
	const UINT value  = ( nCount + nCount / 5 );
	return * std::lower_bound( primes, last, value, std::less< UINT >() );	// + 20%
}

CStringA UTF8Encode(__in const CStringW& strInput)
{
	return UTF8Encode( strInput, strInput.GetLength() );
}

CStringA UTF8Encode(__in_bcount(nInput) LPCWSTR psInput, __in int nInput)
{
	CStringA strUTF8;
	int nUTF8 = ::WideCharToMultiByte( CP_UTF8, 0, psInput, nInput,
		strUTF8.GetBuffer( nInput * 4 + 1 ), nInput * 4 + 1, NULL, NULL );

	if ( nUTF8 == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER )
	{
		nUTF8 = ::WideCharToMultiByte( CP_UTF8, 0, psInput, nInput,
			NULL, 0, NULL, NULL );

		nUTF8 = ::WideCharToMultiByte( CP_UTF8, 0, psInput, nInput,
			strUTF8.GetBuffer( nUTF8 ), nUTF8, NULL, NULL );
	}
	strUTF8.ReleaseBuffer( nUTF8 );

	return strUTF8;
}

CStringW UTF8Decode(__in const CStringA& strInput)
{
	return UTF8Decode( strInput, strInput.GetLength() );
}

CStringW UTF8Decode(__in_bcount(nInput) LPCSTR psInput, __in int nInput)
{
	CStringW strWide;
	int nWide = ::MultiByteToWideChar( CP_UTF8, 0, psInput, nInput,
		strWide.GetBuffer( nInput + 1 ), nInput + 1 );

	if ( nWide == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER )
	{
		nWide = ::MultiByteToWideChar( CP_UTF8, 0, psInput, nInput,
			NULL, 0 );

		nWide = ::MultiByteToWideChar( CP_UTF8, 0, psInput, nInput,
			strWide.GetBuffer( nWide ), nWide );
	}
	strWide.ReleaseBuffer( nWide );

	return strWide;
}

// Encodes unsafe characters in a string: "hello world" to "hello%20world"
// Takes text and returns a string
CString URLEncode(LPCTSTR pszInputT)
{
	// Setup two strings, one with all the hexidecimal digits, the other with all the characters to find and encode
	static LPCTSTR pszHex	= _T("0123456789ABCDEF");	// A string with all the hexidecimal digits
	static LPCSTR pszUnsafe	= "<>\"#%{}|\\^~[]+?&@=:,";	// A string with all the characters unsafe for a URL

	CString strOutput;

	// If the input character pointer points to null or points to the null terminator, just return the blank output string
	if ( pszInputT == NULL || *pszInputT == 0 ) return strOutput;

	// Map the wide character string to a new character set
	int nUTF8 = WideCharToMultiByte(
		CP_UTF8,	// Translate using UTF-8, the default encoding for Unicode
		0,			// Must be 0 for UTF-8 encoding
		pszInputT,	// Points to the wide character string to be converted
		-1,			// The string is null terminated
		NULL,		// We just want to find out how long the buffer for the output string needs to be
		0,
		NULL,		// Both must be NULL for UTF-8 encoding
		NULL );

	// If the converted text would take less than 2 bytes, which is 1 character, just return blank
	if ( nUTF8 < 2 ) return strOutput;

	// Make a new array of CHARs which is nUTF8 bytes long
	LPSTR pszUTF8 = new CHAR[ static_cast< UINT>( nUTF8 ) ];

	// Call WideCharToMultiByte again, this time it has the output buffer and will actually do the conversion
	WideCharToMultiByte( CP_UTF8, 0, pszInputT, -1, pszUTF8, nUTF8, NULL, NULL );

	// Set the null terminator in pszUTF8 to right where you think it should be, and point a new character pointer at it
	pszUTF8[ nUTF8 - 1 ] = 0;
	LPCSTR pszInput = pszUTF8;

	// Get the character buffer inside the output string, specifying how much larger to make it
	LPTSTR pszOutput = strOutput.GetBuffer( static_cast< int >( ( nUTF8 - 1 ) * 3 + 1 ) ); // Times 3 in case every character gets encoded

	// Loop for each character of input text
	for ( ; *pszInput ; pszInput++ )
	{
		// If the character code is 32 or less, more than 127, or in the unsafe list
		if ( *pszInput <= 32 || *pszInput > 127 || strchr( pszUnsafe, *pszInput ) != NULL )
		{
			// Write a three letter code for it like %20 in the output text
			*pszOutput++ = _T('%');
			*pszOutput++ = pszHex[ ( *pszInput >> 4 ) & 0x0F ];
			*pszOutput++ = pszHex[ *pszInput & 0x0F ];
		}
		else	// The character doesn't need to be encoded
		{
			// Just copy it across
			*pszOutput++ = (TCHAR)*pszInput;
		}
	}

	// Null terminate the output text, and then close our direct manipulation of the string
	*pszOutput = 0;
	strOutput.ReleaseBuffer(); // This closes the string so Windows can again start managing its memory for us

	// Free the memory we allocated with the new keyword above
	delete [] pszUTF8;

	// Return the URL-encoded, %20-filled text
	return strOutput;
}

// Decodes unsafe characters in a string: "hello%20world" to "hello world"
// Takes text and returns a string
CString URLDecode(LPCTSTR pszInput)
{
	LPCTSTR pszLoop = pszInput;
	// Check each character of input text
	for ( ; *pszLoop ; pszLoop++ )
	{
		// This URI is not properly encoded, and has unicode characters in it. URL-decode only
		if ( *pszLoop > 255 )
			return URLDecodeUnicode( pszInput );
	}

	// This is a correctly formatted URI, which must be url-decoded, then UTF-8 decoded.
	return URLDecodeANSI( pszInput );
}

// Decodes a properly formatted URI, then UTF-8 decodes it
CString URLDecodeANSI(LPCTSTR pszInput)
{
	// Setup local variables useful for the conversion
	TCHAR szHex[3] = { 0, 0, 0 };	// A 3 character long array filled with 3 null terminators
	CString strOutput;				// The output string, which starts out blank
	int nHex;						// The hex code of the character we found

	// Allocate a new CHAR array big enough to hold the input characters and a null terminator
	LPSTR pszBytes = new CHAR[ _tcslen( pszInput ) + 1 ];

	// Point the output string pointer at this array
	LPSTR pszOutput = pszBytes;

	// Loop for each character of input text
	for ( ; *pszInput ; pszInput++ )
	{
		if ( *pszInput == '%' )			// We hit a %, which might be the start of something like %20
		{
			// Copy characters like "20" into szHex, making sure neither are null
			if ( ( szHex[0] = pszInput[1] ) == 0 ) break;
			if ( ( szHex[1] = pszInput[2] ) == 0 ) break;

			// Read the text like "20" as a number, and store it in nHex
			if ( _stscanf( szHex, _T("%x"), &nHex ) != 1 ) break;
			if ( nHex < 1 ) break; // Make sure the number isn't 0 or negative

			// That number is the code of a character, copy it into the output string
			*pszOutput++ = CHAR( nHex ); // And then move the output pointer to the next spot

			// Move the input pointer past the two characters of the "20"
			pszInput += 2;
		}
		else if ( *pszInput == '+' )	// We hit a +, which is shorthand for space
		{
			// Add a space to the output text, and move the pointer forward
			*pszOutput++ = ' ';
		}
		else	// The input pointer is just on a normal character
		{
			// Copy it across
			*pszOutput++ = (CHAR)*pszInput;
		}
	}

	// Cap off the output text with a null terminator
	*pszOutput = 0;

	// Copy the text from pszBytes into strOutput, converting it into Unicode
	int nLength = MultiByteToWideChar( CP_UTF8, 0, pszBytes, -1, NULL, 0 );
	MultiByteToWideChar( CP_UTF8, 0, pszBytes, -1, strOutput.GetBuffer( nLength ), nLength );

	// Close the output string, we are done editing its buffer directly
	strOutput.ReleaseBuffer();

	// Free the memory we allocated above
	delete [] pszBytes;

	return strOutput;
}

// Decodes encoded characters in a unicode string
CString URLDecodeUnicode(LPCTSTR pszInput)
{
	// Setup local variables useful for the conversion
	TCHAR szHex[3] = { 0, 0, 0 };	// A 3 character long array filled with 3 null terminators
	CString strOutput;				// The output string, which starts out blank
	int nHex;						// The hex code of the character we found

	// Allocate a new CHAR array big enough to hold the input characters and a null terminator
	LPTSTR pszBytes = strOutput.GetBuffer( static_cast< int >( _tcslen( pszInput ) ) );

	// Point the output string pointer at this array
	LPTSTR pszOutput = pszBytes;

	// Loop for each character of input text
	for ( ; *pszInput ; pszInput++ )
	{
		if ( *pszInput == '%' ) 		// We hit a %, which might be the start of something like %20
		{
			// Copy characters like "20" into szHex, making sure neither are null
			if ( ( szHex[0] = pszInput[1] ) == 0 ) break;
			if ( ( szHex[1] = pszInput[2] ) == 0 ) break;

			// Read the text like "20" as a number, and store it in nHex
			if ( _stscanf( szHex, _T("%x"), &nHex ) != 1 ) break;
			if ( nHex < 1 ) break;	// Make sure the number isn't 0 or negative

			// That number is the code of a character, copy it into the output string
			*pszOutput++ = WCHAR( nHex ); // And then move the output pointer to the next spot

			// Move the input pointer past the two characters of the "20"
			pszInput += 2;
		}
		else if ( *pszInput == '+' )	// We hit a +, which is shorthand for space
		{
			// Add a space to the output text, and move the pointer forward
			*pszOutput++ = ' ';
		}
		else	// The input pointer is just on a normal character
		{
			// Copy it across
			*pszOutput++ = (TCHAR)*pszInput;
		}
	}

	// Close and return the string
	*pszOutput = 0;            // End the output text with a null terminator
	strOutput.ReleaseBuffer(); // Release direct access to the buffer of the CString object

	return strOutput;
}
