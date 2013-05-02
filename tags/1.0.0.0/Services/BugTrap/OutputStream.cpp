/*
 * This is a part of the BugTrap package.
 * Copyright (c) 2005-2009 IntelleSoft.
 * All rights reserved.
 *
 * Description: Input stream.
 * Author: Maksim Pyatkovskiy.
 */

#include "StdAfx.h"
#include "OutputStream.h"
#include "InputStream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @param arrBytes - array of bytes.
 * @param nCount - size of the array.
 * @return number of written bytes.
 */
size_t COutputStream::WriteBytes(const unsigned char* arrBytes, size_t nCount)
{
	size_t nNumWritten = 0;
	while (nNumWritten < nCount)
	{
		if (! WriteByte(arrBytes[nNumWritten]))
			break;
		++nNumWritten;
	}
	return nNumWritten;
}

/**
 * @param bValue - byte value to be written.
 * @param nCount - number of bytes to write.
 * @return number of written bytes.
 */
size_t COutputStream::WriteByte(unsigned char bValue, size_t nCount)
{
	size_t nNumWritten = 0;
	while (nNumWritten < nCount)
	{
		if (! WriteByte(bValue))
			break;
		++nNumWritten;
	}
	return nNumWritten;
}

/**
 * @param pInputStream - input stream.
 * @return number of written bytes.
 */
size_t COutputStream::WriteStream(CInputStream* pInputStream)
{
	unsigned char arrBuffer[1024];
	size_t nNumWritten = 0;
	for (;;)
	{
		size_t nNumRead = pInputStream->ReadBytes(arrBuffer, sizeof(arrBuffer));
		if ( nNumRead == 0 )
			return nNumWritten;
		if ( nNumRead == MAXSIZE_T )
			return MAXSIZE_T;
		unsigned char* pBuffer = arrBuffer;
		while (nNumRead > 0)
		{
			size_t nNumWrittenTemp = WriteBytes(pBuffer, nNumRead);
			if ( nNumWrittenTemp != nNumRead )
				return MAXSIZE_T;
			nNumRead -= nNumWrittenTemp;
			nNumWritten += nNumWrittenTemp;
			pBuffer += nNumWrittenTemp;
		}
	}
	/* return nNumWritten; */
}
