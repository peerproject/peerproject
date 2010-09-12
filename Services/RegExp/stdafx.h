//
// stdafx.h
//
// Source file for standard system include files,
// or project files that are used frequently but changed infrequently
//

#pragma once

#include "TargetVer.h"

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#pragma warning(disable:4820)	// X bytes padding added after data member 'Y'
#pragma warning(disable:4365)	// Conversion from '' to '', signed/unsigned mismatch
#pragma warning(disable:4548)	// Expression before comma has no effect; expected expression with side-effect
#pragma warning(disable:4555)	// Expression has no effect; expected expression with side-effect
#pragma warning(disable:4571)	// Catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
#pragma warning(disable:4619)	// #pragma warning : there is no warning number
#pragma warning(disable:4625)	// Copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable:4668)	// Is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning(disable:4686)	// Possible change in behavior, change in UDT return calling convention

#include <atlconv.h>
#include <tchar.h>

#include "RegExpr2.h"

using namespace ATL;
