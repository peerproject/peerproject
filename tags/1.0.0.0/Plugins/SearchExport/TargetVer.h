
#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, etc. that has the necessary features to run your application.
// The macros work by enabling all features available on platform versions up to the version specified.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#ifndef WINVER
#define WINVER 0x0500		// Specifies minimum required platform is Windows 2000.
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500	// Specifies minimum required platform is Windows 2000.
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0500	// Change this to target Windows Me or later.
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0501	// Specifies minimum required platform is Internet Explorer 5.0
#endif
