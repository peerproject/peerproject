//
// RegExp.h : Main header file
//

#pragma once

#ifdef REGEXP_EXPORTS
#define REGEXP_API __declspec(dllexport)
#else
#define REGEXP_API __declspec(dllimport)
#endif

namespace RegExp
{

// Returns TRUE if szContent matches szRegExp regular expression (case insensitive)
REGEXP_API BOOL Match(LPCTSTR szRegExp, LPCTSTR szContent);

// Splits szContent according szRegExp regular expression (case insensitive)
// Returns number of string in function allocated pszResult (array of strings)
// pszResult must be freed by GlobalFree() function
REGEXP_API size_t Split(LPCTSTR szRegExp, LPCTSTR szContent, LPTSTR* pszResult);

};
