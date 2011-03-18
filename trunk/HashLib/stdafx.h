//
// stdafx.h
//

#pragma once

// For /Wall
#pragma warning(disable:4668)
#pragma warning(disable:4820)
#pragma warning(disable:4548)
#pragma warning(disable:4710)	// 'inline' function not inlined
#pragma warning(disable:4986)	// operator new[]/delete[] previous declaration

#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#define _WIN32_IE 0x0500

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define BOOST_USE_WINDOWS_H

#include <windows.h>
#include <algorithm>

// Define HASHLIB_USE_ASM for win32 assembler use (several times faster)
