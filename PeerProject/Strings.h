//
// Strings.h
//
// This file is part of PeerProject (peerproject.org) © 2010-2012
// Portions copyright Shareaza Development Team, 2010.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include <set>

// Produce 2 comma-seperated arguments: string itself, and string length (without null terminator)	_PT("Text") = L"Text",4
#define _P(x) (x),((sizeof(x))/sizeof((x)[0])-1)
#define _PT(x) ( _T(x) ), ( (sizeof(_T(x)))/sizeof((_T(x))[0]) - 1 )

#define IsSpace(ch)	((ch) == _T(' ') || (ch) == _T('\t') || (ch) == _T('\r') || (ch) == _T('\n'))

class CLowerCaseTable
{
public:
	explicit CLowerCaseTable();
	TCHAR operator()(TCHAR cLookup) const;
	CString& operator()(CString& strSource) const;

	// Convert string to lower case and do character substitutes:
	// ".", "_", "+" to " " and "[", "{" to "(" and "]", "}" to ")"
	CString& Clean(CString& strSource) const;

private:
	TCHAR cTable[ 65536 ];

	CLowerCaseTable(const CLowerCaseTable&);
	CLowerCaseTable& operator=(const CLowerCaseTable&);
};

extern const CLowerCaseTable ToLower;


typedef enum
{
	sNone = 0,
	sNumeric = 1,
	sRegular = 2,
	sKanji = 4,
	sHiragana = 8,
	sKatakana = 16
} ScriptType;

bool IsCharacter(WCHAR nChar);
bool IsHiragana(WCHAR nChar);
bool IsKatakana(WCHAR nChar);
bool IsKanji(WCHAR nChar);
bool IsWord(LPCTSTR pszString, size_t nStart, size_t nLength);
void IsType(LPCTSTR pszString, size_t nStart, size_t nLength, bool& bWord, bool& bDigit, bool& bMix);

// Encode Unicode text to UTF-8 text
CStringA UTF8Encode(__in const CStringW& strInput);
CStringA UTF8Encode(__in_bcount(nInput) LPCWSTR psInput, __in int nInput);

// Decode UTF-8 text to Unicode text
CStringW UTF8Decode(__in const CStringA& strInput);
CStringW UTF8Decode(__in_bcount(nInput) LPCSTR psInput, __in int nInput);

// Encode "hello world" into "hello%20world"
CString URLEncode(LPCTSTR pszInput);

// Decode "hello%20world" back to "hello world"
CString URLDecode(LPCTSTR pszInput);
CString URLDecode(__in const CStringA& strInput);
CString URLDecode(__in_bcount(nInput) LPCSTR psInput, __in int nInput);

// Decodes properly encoded URLs
CString URLDecodeANSI(LPCTSTR pszInput);

// Decodes URLs with extended characters
CString URLDecodeUnicode(LPCTSTR pszInput);

// Case independent string search
LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszSubString);
LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszSubString, size_t nlen);

// Convert string to integer (64-bit, decimal only, with sign, no spaces allowed). Returns false on error.
bool atoin(__in_bcount(nLen) const char* pszString, __in size_t nLen, __int64& nNum);

// Split string using delimiter to string array
void Split(const CString& strSource, TCHAR cDelimiter, CStringArray& pAddIt, BOOL bAddFirstEmpty = FALSE);

// StartsWith("Hello world", _PT("hello")) is true
BOOL StartsWith(const CString& strInput, LPCTSTR pszText, size_t nLen = 0);

// Load all text from file (Unicode-compatible)
CString LoadFile(LPCTSTR pszPath);

// Replaces a substring with another (case-insensitive)
BOOL ReplaceNoCase(CString& sInStr, LPCTSTR pszOldStr, LPCTSTR pszNewStr);

// IsValidIP("1.2.3.4:0000") is true
//BOOL IsValidIP(const CString& sInput);

// "1.2.3.4:0000" to ~0x11223344, 0 for invalid	(Alt ~0x44332211)
DWORD IPStringToDWORD(LPCTSTR pszIP, BOOL bReverse = TRUE);

// Returns "a.a.a.a:port"
CString HostToString(const SOCKADDR_IN* pHost);

// Format long paths if needed "\\?\"
LPCTSTR SafePath(const CString& sPath);
BOOL MakeSafePath(CString& sPath);

// Function is used to split a phrase in Asian languages to separate keywords
// to ease keyword matching, allowing user to type as in the natural language.
// Spacebar key is not a convenient way to separate keywords with IME,
// and user may not know how application is keywording their files.
//
// The function splits katakana, Hiragana and CJK phrases out of the input string.
// ToDo: "minus" words and quoted phrases for Asian languages may not work correctly in all cases.
CString MakeKeywords(const CString& strPhrase, bool bExpression = true);

typedef std::pair< LPCTSTR, size_t > WordEntry;

struct CompareWordEntries : public std::binary_function< WordEntry, WordEntry, bool >
{
	bool operator()(const WordEntry& lhs, const WordEntry& rhs) const
	{
		int cmp = _tcsnicmp( lhs.first, rhs.first, min( lhs.second, rhs.second ) );
		return ( cmp < 0 || cmp == 0 && lhs.second < rhs.second );
	}
};

typedef std::set< WordEntry, CompareWordEntries > WordTable;

void BuildWordTable(LPCTSTR pszWord, WordTable& oWords, WordTable& oNegWords);

// Skip slashes starting from nAdd position
LPCTSTR SkipSlashes(LPCTSTR pszURL, int nAdd = 0);

// Replace all symbols with code less than space by underscore symbols
void SafeString(CString& strInput);

// Escape unsafe symbols
CString Escape(const CString& strValue);

// Unescape unsafe symbols
CString Unescape(LPCTSTR pszXML, int nLength = -1);
