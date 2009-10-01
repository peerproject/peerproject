//
// stdafx.h
//


#pragma once

// For /Wall
#pragma warning(disable:4668)
#pragma warning(disable:4820)
#pragma warning(disable:4548)

#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#define _WIN32_IE 0x0500

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define BOOST_USE_WINDOWS_H

#include <windows.h>
#include <algorithm>

// define HASHLIB_USE_ASM for assembler use (several times faster)
