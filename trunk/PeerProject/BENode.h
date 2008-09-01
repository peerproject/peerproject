//
// BENode.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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
	
	enum { beNull, beString, beInt, beList, beDict };
	
// Operations
public:
	void		Clear();
	CBENode*	Add(const LPBYTE pKey, size_t nKey);
	CBENode*	GetNode(LPCSTR pszKey) const;
	CBENode*	GetNode(const LPBYTE pKey, int nKey) const;
    void		GetBth(Hashes::BtHash& oBTH) const;
	CString		GetStringFromSubNode(LPCSTR pszKey, UINT nEncoding, bool& pEncodingError);
	CString		GetStringFromSubNode(int nItem, UINT nEncoding, bool& pEncodingError);
	void		Encode(CBuffer* pBuffer) const;
public:
	static CBENode*	Decode(CBuffer* pBuffer);
private:
	void		Decode(LPBYTE& pInput, DWORD& nInput);
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
	
	inline CString GetString() const
	{
		CString str;
		if ( m_nType != beString ) return str;
		str = (LPCSTR)m_pValue;

		int nLength = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)m_pValue, -1, NULL, 0 );
		if ( nLength > 0 )
			MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)m_pValue, -1, str.GetBuffer( nLength ), nLength );
		else
		{
			// Bad encoding
			str.ReleaseBuffer();
			str = _T("#ERROR#");
			return str;
		}
		str.ReleaseBuffer();

		return str;
	}

	// If a torrent is badly encoded, you can try forcing a code page.
	inline CString DecodeString(UINT nCodePage) const
	{
		CString str;
		if ( m_nType != beString ) return str;
		str = (LPCSTR)m_pValue;
		int nSource = str.GetLength();
		if ( nSource == 0 ) return str;
		int nLength = 0;

		// Use the torrent code page (if present)
		if ( nCodePage != 0 ) 
		{
			nLength = MultiByteToWideChar( nCodePage, MB_ERR_INVALID_CHARS,
				(LPCSTR)m_pValue, -1 , NULL, 0 );
		}

		if ( nLength > 0 )
			MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pValue, -1 , str.GetBuffer( nLength ), nLength );
		else
		{		
			// Try the user-specified code page if it's set, else use the system code page
			if ( Settings.BitTorrent.TorrentCodePage != 0 )
				nCodePage = Settings.BitTorrent.TorrentCodePage;
			else
				nCodePage = GetOEMCP();

			nLength = MultiByteToWideChar( nCodePage, MB_ERR_INVALID_CHARS,
				(LPCSTR)m_pValue, nSource, NULL, 0 );

			if ( nLength > 0 )
				MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pValue, nSource, str.GetBuffer( nLength ), nLength );
			else
			{
				// Try ACP. (Should convert anything, but badly)
				nCodePage = CP_ACP;
				nLength = MultiByteToWideChar( nCodePage, MB_ERR_INVALID_CHARS,
					(LPCSTR)m_pValue, nSource, NULL, 0 );
				if ( nLength > 0 )
					MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pValue, nSource, str.GetBuffer( nLength ), nLength );
				else
				{
					// Nothing more we can do
					str = _T("#ERROR#");
					return str;
				}
			}
		}
		str.ReleaseBuffer();

		return str;
	}

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
		SetString( (LPCSTR)pszASCII, strlen(pszASCII), TRUE );
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