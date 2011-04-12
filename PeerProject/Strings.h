//
// Strings.h
//
// This file is part of PeerProject (peerproject.org) © 2010-2011
// Portions copyright Shareaza Development Team, 2010.
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

#pragma once

class CLowerCaseTable
{
public:
	explicit CLowerCaseTable();
	TCHAR operator()(TCHAR cLookup) const;
	CString& operator()(CString& strSource) const;

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

// Convert string to integer (64-bit, decimal only, no sign, no spaces allowed). Returns -1 on error.
__int64 atoin(__in_bcount(nLen) const char* pszString, __in size_t nLen);

// Split string using delimiter to string array
void Split(const CString& strSource, TCHAR cDelimiter, CStringArray& pAddIt, BOOL bAddFirstEmpty = FALSE);

// StartsWith("Hello world", "hello") is true
BOOL StartsWith(const CString& sInput, LPCTSTR pszText, size_t nLen = 0);
