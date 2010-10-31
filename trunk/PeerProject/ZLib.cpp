//
// ZLib.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

// CZLib makes it easier to use the zlib compression library
// http://sourceforge.net/apps/mediawiki/shareaza/index.php?title=Developers.Code.CZLib

#include "StdAfx.h"
#include "PeerProject.h"
#include "ZLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

//////////////////////////////////////////////////////////////////////
// CZLib compression

// Takes a pointer to memory and how many bytes are there
// Compresses the memory into a new buffer this function allocates
// Returns a pointer to the new buffer, and writes its size under pnOutput
auto_array< BYTE > CZLib::Compress(LPCVOID pInput, DWORD nInput, DWORD* pnOutput, DWORD nSuggest)
{
	// If we were given nSuggest, use it as the output buffer size, otherwise call compressBound to set it
	*pnOutput = nSuggest ? nSuggest : compressBound( nInput ); // compressBound just uses math to guess, it doesn't look at the data

	// Allocate a new buffer of pnOutput bytes
	auto_array< BYTE > pBuffer( new BYTE[ *pnOutput ] );

	// Compress the data at pInput into pBuffer, putting how many bytes it wrote under pnOutput
	int nRes = compress(		// Compress data from one buffer to another, returns Z_OK 0 false if it works
		pBuffer.get(),			// The output buffer where ZLib can write compressed data
		pnOutput,				// Reads how much space it has there, writes how much space it used
		(const BYTE *)pInput,	// The source buffer with data to compress
		nInput );				// The number of bytes there
	if ( nRes != Z_OK )
	{
		// The compress function reported error
		ASSERT( Z_BUF_ERROR != nRes );	// TODO
		return auto_array< BYTE >();
	}

	// The pBuffer buffer is too big, make a new one exactly the right size, copy the data, delete the first, and return the second
	auto_array< BYTE > pOutput( new BYTE[ *pnOutput ] );	// Allocate a new buffer exact size to hold the bytes compress wrote
	memcpy( pOutput.get(), pBuffer.get(), *pnOutput );	// Copy the compressed bytes from the old buffer to the new one
	return pOutput;										// Return new buffer
}

//////////////////////////////////////////////////////////////////////
// CZLib decompression

// Takes a pointer to compressed input bytes, and how many are there
// Decompresses the memory into a new buffer this function allocates
// Returns a pointer to the new buffer, and writes its size under pnOutput
auto_array< BYTE > CZLib::Decompress(LPCVOID pInput, DWORD nInput, DWORD* pnOutput)
{
	BYTE* pBuffer;

	// Guess how big the data will be decompressed, use nSuggest, or just guess it will be 6 times as big
	for ( DWORD nSuggest = max( nInput * 6ul, 1024ul ); ; nSuggest *= 2 )
	{
		pBuffer = new BYTE[ nSuggest ];
		if ( ! pBuffer )
		{
			// Out of memory
			*pnOutput = 0;
			return auto_array< BYTE >();
		}

		*pnOutput = nSuggest;

		// Uncompress the data from pInput into pBuffer, writing how big it is now in pnOutput
		int nRes = uncompress(		// Uncompress data
			pBuffer,				// Destination buffer where uncompress can write uncompressed data
			pnOutput,				// Reads how much space it has there, and writes how much space it used
			(const BYTE *)pInput,	// Source buffer of compressed data
			nInput ); 				// Number of bytes there

		if ( Z_OK == nRes )
			break;

		delete [] pBuffer;

		if ( Z_BUF_ERROR != nRes )
		{
			// Uncompress function returned an error, delete allocated buffer and return the error
			*pnOutput = 0;
			return auto_array< BYTE >();
		}
	}

	// The pBuffer buffer is bigger than necessary, move its bytes into one perfectly sized, and return it
	auto_array< BYTE > pOutput( new BYTE[ *pnOutput ] );	// Make a new buffer exactly the right size
	memcpy( pOutput.get(), pBuffer, *pnOutput );	// Copy the data from the one that's too big

	delete [] pBuffer;

	return pOutput;									// Return a pointer to the perfectly sized one
}
