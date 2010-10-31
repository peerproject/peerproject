//
// Free implementation of the MD4 hash algorithm
// MD4C.C - RSA Data Security, Inc., MD4 message-digest algorithm
//

//	Copyright (C) 1990-2, RSA Data Security, Inc. All rights reserved.

//	License to copy and use this software is granted provided that it
//	is identified as the "RSA Data Security, Inc. MD4 Message-Digest
//	Algorithm" in all material mentioning or referencing this software
//	or this function.

//	License is also granted to make and use derivative works provided
//	that such works are identified as "derived from the RSA Data
//	Security, Inc. MD4 Message-Digest Algorithm" in all material
//	mentioning or referencing the derived work.
//
//	RSA Data Security, Inc. makes no representations concerning either
//	the merchantability of this software or the suitability of this
//	software for any particular purpose. It is provided "as is"
//	without express or implied warranty of any kind.

//	These notices must be retained in any copies of any part of this
//	documentation and/or software.


#include "StdAfx.h"
#include "MD4.h"

BYTE CMD4::MD4_PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};