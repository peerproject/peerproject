//
// BENode.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) � 2008, 2014
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once

class CBuffer;
#ifdef _PORTABLE
class CHashSHA1;
#endif

class CBENode
{
// Construction
public:
	CBENode();
	~CBENode();

// Attributes
public:
	int			m_nType;
	LPVOID		m_pValue;
	QWORD		m_nValue;

	static UINT	m_nDefaultCP;	// User codepage for string decoding

	enum { beNull, beString, beInt, beList, beDict };

// Operations
public:
	void		Clear();
	void		Encode(CBuffer* pBuffer) const;
	CBENode*	Add(const LPBYTE pKey, size_t nKey);
	CBENode*	GetNode(LPCSTR pszKey) const;
	CBENode*	GetNode(const LPBYTE pKey, int nKey) const;
#ifdef _PORTABLE
	CHashSHA1	GetSHA1() const;
#else
	CSHA		GetSHA1() const;
#endif
	static int __cdecl SortDict(const void * pA, const void * pB);

// Inline
public:
	inline bool IsType(int nType) const
	{
		if ( this == NULL ) return false;
		return m_nType == nType;
	}

	inline QWORD GetInt() const
	{
		if ( m_nType != beInt ) return 0;
		return m_nValue;
	}

	inline void SetInt(QWORD nValue)
	{
		Clear();
		m_nType  = beInt;
		m_nValue = nValue;
	}

	CString GetString() const;

	// Check if a string is a valid path/file name.
	inline BOOL IsValid(LPCTSTR psz) const
	{
		if ( _tcsclen( psz ) == 0 ) return FALSE;
		if ( _tcschr( psz, '?' ) != NULL ) return FALSE;
		if ( _tcsicmp( psz , L"#ERROR#" ) == 0 ) return FALSE;

		return TRUE;
	}

	inline void SetString(LPCSTR psz)
	{
		SetString( psz, strlen(psz), TRUE );
	}

	inline void SetString(LPCWSTR psz)
	{
		int nLength = WideCharToMultiByte( CP_UTF8, 0, psz, -1, NULL, 0, NULL, NULL );
		if ( nLength > 0 )
		{
			CStringA str;
			WideCharToMultiByte( CP_UTF8, 0, psz, -1, str.GetBuffer( nLength ), nLength, NULL, NULL );
			str.ReleaseBuffer();
			SetString( str, str.GetLength(), TRUE );
		}
	}

	inline void SetString(LPCVOID pString, size_t nLength, BOOL bNull = FALSE)
	{
		Clear();
		m_nType  = beString;
		m_nValue = (QWORD)nLength;
		m_pValue = new BYTE[ nLength + ( bNull ? 1 : 0 ) ];
		CopyMemory( m_pValue, pString, nLength + ( bNull ? 1 : 0 ) );
	}

	inline CBENode* Add(LPCSTR pszKey = NULL)
	{
		return Add( (LPBYTE)pszKey, pszKey != NULL ? strlen( pszKey ) : 0 );
	}

	inline int GetCount() const
	{
		if ( m_nType != beList && m_nType != beDict ) return 0;
		return (int)m_nValue;
	}

	inline CBENode* GetNode(int nItem) const
	{
		if ( m_nType != beList && m_nType != beDict ) return NULL;
		if ( m_nType == beDict ) nItem *= 2;
		if ( nItem < 0 || nItem >= (int)m_nValue ) return NULL;
		return *( (CBENode**)m_pValue + nItem );
	}
};
