//
// BENode.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
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
	QWORD		m_nSize;
	QWORD		m_nPosition;

	static UINT	m_nDefaultCP;	// User codepage for string decoding

	enum { beNull, beString, beInt, beList, beDict };

// Operations
public:
	void		Clear();
	CBENode*	Add(const LPBYTE pKey, size_t nKey);
	CBENode*	GetNode(LPCSTR pszKey) const;
	CBENode*	GetNode(const LPBYTE pKey, int nKey) const;
	CString		GetStringFromSubNode(LPCSTR pszKey, UINT nEncoding, bool& pEncodingError) const;
	CString		GetStringFromSubNode(int nItem, UINT nEncoding, bool& pEncodingError) const;
	CSHA		GetSHA1() const;
	void		Encode(CBuffer* pBuffer) const;

	static CBENode*	Decode(const CBuffer* pBuffer, DWORD *pnReaden = NULL );
private:
	void		Decode(LPBYTE& pInput, DWORD& nInput, DWORD nSize);
	static int	DecodeLen(LPBYTE& pInput, DWORD& nInput);

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
		m_nType		= beInt;
		m_nValue	= nValue;
	}

	CString GetString() const;

	// If a torrent is badly encoded, you can try forcing a code page.
	// Trying codepages: nCodePage, m_nDefaultCP, OEM, ANSI, as-is
	CString DecodeString(UINT nCodePage) const;

	// Check if a string is a valid path/file name.
	inline BOOL IsValid(LPCTSTR psz) const
	{
		if ( _tcsclen( psz ) == 0 ) return FALSE;
		if ( _tcschr( psz, '?' ) != NULL ) return FALSE;
		if ( _tcsicmp( psz , _T("#ERROR#") ) == 0 ) return FALSE;

		return TRUE;
	}

	inline void SetString(LPCSTR psz)
	{
		SetString( psz, strlen(psz), TRUE );
	}

	inline void SetString(LPCWSTR psz)
	{
		CW2A pszASCII( psz );
		SetString( (LPCSTR)pszASCII, strlen( (LPCSTR)pszASCII ), TRUE );
	}

	inline void SetString(LPCVOID pString, size_t nLength, BOOL bNull = FALSE)
	{
		Clear();
		m_nType		= beString;
		m_nValue	= (QWORD)nLength;
		m_pValue	= new BYTE[ nLength + ( bNull ? 1 : 0 ) ];
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

	// Encode node to human readable string
	const CString Encode() const;
};
