//
// BENode.cpp
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

#include "StdAfx.h"
#include "BENode.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static bool Decode(UINT nCodePage, LPCSTR szFrom, CString& strTo)
{
	int nLength = MultiByteToWideChar( nCodePage, MB_ERR_INVALID_CHARS, szFrom, -1, NULL, 0 );
	if ( nLength > 0 )
	{
		MultiByteToWideChar( nCodePage, 0, szFrom, -1,
			strTo.GetBuffer( nLength ), nLength );
		strTo.ReleaseBuffer();
		return true;
	}
	return false;
}

UINT CBENode::m_nDefaultCP = CP_ACP;

//////////////////////////////////////////////////////////////////////
// CBENode construction/destruction

CBENode::CBENode()
{
	m_nType		= beNull;
	m_pValue	= NULL;
	m_nValue	= 0;
	m_nSize		= 0;
	m_nPosition	= 0;
}

CBENode::~CBENode()
{
	if ( m_pValue != NULL ) Clear();
}

//////////////////////////////////////////////////////////////////////
// CBENode clear

void CBENode::Clear()
{
	if ( m_pValue != NULL )
	{
		if ( m_nType == beString )
		{
			delete [] (LPSTR)m_pValue;
		}
		else if ( m_nType == beList )
		{
			CBENode** pNode = (CBENode**)m_pValue;
			for ( ; m_nValue-- ; pNode++ ) delete *pNode;
			delete [] (CBENode**)m_pValue;
		}
		else if ( m_nType == beDict )
		{
			CBENode** pNode = (CBENode**)m_pValue;
			for ( ; m_nValue-- ; pNode++ )
			{
				delete *pNode++;
				delete [] (LPBYTE)*pNode;
			}
			delete [] (CBENode**)m_pValue;
		}
	}

	m_nType		= beNull;
	m_pValue	= NULL;
	m_nValue	= 0;
}

//////////////////////////////////////////////////////////////////////
// CBENode add a child node

CBENode* CBENode::Add(const LPBYTE pKey, size_t nKey)
{
	switch ( m_nType )
	{
	case beNull:
		m_nType		= ( pKey != NULL && nKey > 0 ) ? beDict : beList;
		m_pValue	= NULL;
		m_nValue	= 0;
		break;
	case beList:
		ASSERT( pKey == NULL && nKey == 0 );
		break;
	case beDict:
		ASSERT( pKey != NULL && nKey > 0 );
		break;
	default:
		ASSERT( FALSE );
		break;
	}

	auto_ptr< CBENode > pNew( new CBENode );
	CBENode* pNew_ = pNew.get();

	// Overflow check
	ASSERT ( m_nValue <= SIZE_T_MAX );
	size_t nValue = static_cast< size_t >( m_nValue );

	if ( m_nType == beList )
	{
		// Overflow check
		ASSERT( nValue + 1 <= SIZE_T_MAX );
		auto_array< CBENode* > pList( new CBENode*[ nValue + 1 ] );

		if ( m_pValue )
		{
			// Overflow check
			ASSERT( nValue * sizeof( CBENode* ) <= SIZE_T_MAX );
			memcpy( pList.get(), m_pValue, nValue * sizeof( CBENode* ) );

			delete [] (CBENode**)m_pValue;
		}

		pList[ nValue ] = pNew.release();
		m_pValue = pList.release();
		++m_nValue;
	}
	else
	{
		// Overflow check
		ASSERT( nValue * 2 + 2 <= SIZE_T_MAX );
		auto_array< CBENode* > pList( new CBENode*[ nValue * 2 + 2 ] );

		if ( m_pValue )
		{
			// Overflow check
			ASSERT( 2 * nValue * sizeof( CBENode* ) <= SIZE_T_MAX );
			memcpy( pList.get(), m_pValue, 2 * nValue * sizeof( CBENode* ) );

			delete [] (CBENode**)m_pValue;
		}

		auto_array< BYTE > pxKey( new BYTE[ nKey + 1 ] );
		memcpy( pxKey.get(), pKey, nKey );
		pxKey[ nKey ] = 0;

		pList[ nValue * 2 ]		= pNew.release();
		pList[ nValue * 2 + 1 ]	= (CBENode*)pxKey.release();

		m_pValue = pList.release();
		++m_nValue;
	}

	return pNew_;
}

//////////////////////////////////////////////////////////////////////
// CBENode find a child node

CBENode* CBENode::GetNode(LPCSTR pszKey) const
{
	if ( m_nType != beDict ) return NULL;

	CBENode** pNode = (CBENode**)m_pValue;

	for ( DWORD nNode = (DWORD)m_nValue ; nNode ; nNode--, pNode += 2 )
	{
		if ( strcmp( pszKey, (LPCSTR)pNode[1] ) == 0 ) return *pNode;
	}

	return NULL;
}

CBENode* CBENode::GetNode(const LPBYTE pKey, int nKey) const
{
	if ( m_nType != beDict ) return NULL;

	CBENode** pNode = (CBENode**)m_pValue;

	for ( DWORD nNode = (DWORD)m_nValue ; nNode ; nNode--, pNode += 2 )
	{
		if ( memcmp( pKey, (LPBYTE)pNode[1], nKey ) == 0 )
			return *pNode;
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////
// CBENode Extract a string from a node under this one. (Checks both normal and .utf-8)

CString CBENode::GetStringFromSubNode(LPCSTR pszKey, UINT nEncoding, bool& bEncodingError) const
{
	// Open the supplied node + .utf-8
	CBENode* pSubNode = GetNode( CStringA( pszKey ) + ".utf-8" );

	// We found a back-up node
	// If it exists and is a string, try reading it
	CString strValue;
	if ( ( pSubNode ) && ( pSubNode->m_nType == CBENode::beString ) )
		strValue = pSubNode->GetString();	// Assumed to be UTF-8

	// Open the supplied sub-node
	pSubNode = GetNode( pszKey );

	// If it exists and is a string, try reading it
	if ( ( pSubNode ) && ( pSubNode->m_nType == CBENode::beString ) )
	{
		if ( ! IsValid( strValue ) )
		{
			// Read the string using the correct encoding. (UTF-8)
			strValue = pSubNode->GetString();
		}
		else
		{
			// We already have a value - check it's valid
			CString strCheck = pSubNode->GetString();
			if ( strCheck != strValue )
				bEncodingError = true;
		}
	}

	if ( ! IsValid( strValue ) )
	{
		// If we still don't have a valid name, try a decode by forcing the code page.
		pSubNode = GetNode( pszKey );
		if ( ( pSubNode ) && ( pSubNode->m_nType == CBENode::beString ) )
		{
			bEncodingError = true;
			strValue = pSubNode->DecodeString( nEncoding );
		}
	}

	return strValue;
}

// CBENode Extract a string from a list/dictionary

CString CBENode::GetStringFromSubNode(int nItem, UINT nEncoding, bool& bEncodingError) const
{
	CBENode*	pSubNode;
	CString		strValue;

	// Check we are a list / dictionary type
	if ( m_nType != beList && m_nType != beDict ) return strValue;

	// Open the supplied list/dictionary item
	pSubNode = GetNode( nItem );

	// If it exists and is a string, try reading it (using correct UTF-8 encoding).
	if ( pSubNode && ( pSubNode->m_nType == CBENode::beString ) )
		strValue = pSubNode->GetString();	// Read the string

	// If it wasn't valid, try a decode by forcing the code page.
	if ( ! IsValid( strValue ) )
	{
		// If we still don't have a valid name, try a decode by forcing the code page.
		if ( ( pSubNode ) && ( pSubNode->m_nType == CBENode::beString ) )
		{
			bEncodingError = true;
			strValue = pSubNode->DecodeString( nEncoding );
		}
	}

	return strValue;
}

//////////////////////////////////////////////////////////////////////
// CBENode SHA1 computation

CSHA CBENode::GetSHA1() const
{
	ASSERT( this != NULL );

	CBuffer pBuffer;
	Encode( &pBuffer );

	CSHA pSHA;
	pSHA.Add( pBuffer.m_pBuffer, pBuffer.m_nLength );
	pSHA.Finish();
	return pSHA;
}

//////////////////////////////////////////////////////////////////////
// CBENode encoding

void CBENode::Encode(CBuffer* pBuffer) const
{
	CHAR szBuffer[64];
	CString str;

	if ( m_nType == beString )
	{
		pBuffer->Print( szBuffer, sprintf_s( szBuffer, _countof( szBuffer ), "%u:", (DWORD)m_nValue ) );
		pBuffer->Add( m_pValue, (DWORD)m_nValue );
	}
	else if ( m_nType == beInt )
	{
		pBuffer->Print( szBuffer, sprintf_s( szBuffer, _countof( szBuffer ), "i%I64ie", m_nValue ) );
	}
	else if ( m_nType == beList )
	{
		CBENode** pNode = (CBENode**)m_pValue;

		pBuffer->Print( _P("l") );

		for ( DWORD nItem = 0 ; nItem < (DWORD)m_nValue ; nItem++, pNode++ )
		{
			(*pNode)->Encode( pBuffer );
		}

		pBuffer->Print( _P("e") );
	}
	else if ( m_nType == beDict )
	{
		CBENode** pNode = (CBENode**)m_pValue;

		pBuffer->Print( _P("d") );

		for ( DWORD nItem = 0 ; nItem < m_nValue ; nItem++, pNode += 2 )
		{
			LPCSTR pszKey = (LPCSTR)pNode[1];
			size_t nKeyLength = strlen( pszKey );
			pBuffer->Print( szBuffer, sprintf_s( szBuffer, _countof( szBuffer ), "%i:", nKeyLength ) );
			pBuffer->Print( pszKey, nKeyLength );
			(*pNode)->Encode( pBuffer );
		}

		pBuffer->Print( _P("e") );
	}
	else
	{
		ASSERT( FALSE );
	}
}

const CString CBENode::Encode() const
{
	CString sOutput, sTmp;
	switch ( m_nType )
	{
	case beNull:
		sOutput += _T("(null)");
		break;
	case beString:
		{
			sOutput += _T('\"');
			QWORD nLen = min( m_nValue, 100ull );
			for ( QWORD n = 0; n < nLen; n++ )
			{
				sOutput += ( ( ( (LPSTR)m_pValue )[ n ] < ' ' ) ?
					'.' : ( (LPSTR)m_pValue )[ n ] );
			}
			sOutput += _T('\"');
			sTmp.Format( _T("[%I64i]"), m_nValue );
			sOutput += sTmp;
		}
		break;
	case beInt:
		sTmp.Format( _T("%I64i"), m_nValue );
		sOutput += sTmp;
		break;
	case beList:
		sOutput += _T("{ ");
		{
			CBENode** pNode = (CBENode**)m_pValue;
			for (QWORD n = 0 ; n < m_nValue ; n++, pNode++ )
			{
				if ( n )
					sOutput += _T(", ");
				sOutput += (*pNode)->Encode();
			}
		}
		sOutput += _T(" }");
		break;
	case beDict:
		sOutput += _T("{ ");
		{
			CBENode** pNode = (CBENode**)m_pValue;
			for (QWORD n = 0 ; n < m_nValue ; n++, pNode += 2 )
			{
				if ( n )
					sOutput += _T(", ");
				sTmp.Format( _T("\"%s\" = "), CString( (LPCSTR)pNode[ 1 ] ) );
				sOutput += sTmp;
				sOutput += (*pNode)->Encode();
			}
		}
		sOutput += _T(" }");
		break;
	}
	return sOutput;
}

#define INC(x) { pInput += (x); nInput -= (x); }

//////////////////////////////////////////////////////////////////////
// CBENode decoding

CBENode* CBENode::Decode(const CBuffer* pBuffer, DWORD *pnReaden)
{
	try
	{
		auto_ptr< CBENode > pNode( new CBENode() );
		LPBYTE pInput	= pBuffer->m_pBuffer;
		DWORD nInput	= pBuffer->m_nLength;

		// IIS based trackers may insert unneeded EOL at the beginning
		// of the torrent files or scrape responses, due to IIS bug.  Skip it.
		if ( nInput > 1 && pInput[0] == '\r' && pInput[1] == '\n' )
			INC( 2 );

		pNode->Decode( pInput, nInput, nInput );

		if ( pnReaden )
			*pnReaden = pBuffer->m_nLength - nInput;

		return pNode.release();
	}
	catch ( CException* pException )
	{
		pException->Delete();
		return NULL;
	}
}

void CBENode::Decode(LPBYTE& pInput, DWORD& nInput, DWORD nSize)
{
	ASSERT( m_nType == beNull );

	if ( nInput < 1 )
		AfxThrowUserException();

	m_nPosition = nSize - nInput;

	if ( *pInput == 'i' )
	{
		INC( 1 );

		DWORD nSeek = 1;
		for ( ; nSeek < 40 ; nSeek++ )
		{
			if ( nSeek >= nInput )
				AfxThrowUserException();
			if ( pInput[nSeek] == 'e' )
				break;
		}

		if ( nSeek >= 40 ) AfxThrowUserException();

		pInput[nSeek] = 0;
		if ( sscanf_s( (LPCSTR)pInput, "%I64i", &m_nValue ) != 1 )
			AfxThrowUserException();
		pInput[nSeek] = 'e';

		INC( nSeek + 1 );
		m_nType = beInt;
	}
	else if ( *pInput == 'l' )
	{
		m_nType = beList;
		INC( 1 );

		for (;;)
		{
			if ( nInput < 1 )
				AfxThrowUserException();
			if ( *pInput == 'e' )
				break;
			Add()->Decode( pInput, nInput, nSize );
		}

		INC( 1 );
	}
	else if ( *pInput == 'd' )
	{
		m_nType = beDict;
		INC( 1 );

		for (;;)
		{
			if ( nInput < 1 )
				AfxThrowUserException();
			if ( *pInput == 'e' )
				break;

			int nLen = DecodeLen( pInput, nInput );

			if ( nLen )
			{
				LPBYTE pKey = pInput;
				INC( nLen );
				Add( pKey, nLen )->Decode( pInput, nInput, nSize );
			}
		}

		INC( 1 );
	}
	else if ( *pInput >= '0' && *pInput <= '9' )
	{
		m_nType		= beString;
		m_nValue	= DecodeLen( pInput, nInput );
		m_pValue	= new CHAR[ (DWORD)m_nValue + 1 ];
		CopyMemory( m_pValue, pInput, (DWORD)m_nValue );
		((LPBYTE)m_pValue)[ m_nValue ] = 0;

		INC( (DWORD)m_nValue );
	}
	else
	{
		AfxThrowUserException();
	}

	m_nSize = nSize - nInput - m_nPosition;
}

int CBENode::DecodeLen(LPBYTE& pInput, DWORD& nInput)
{
	DWORD nSeek = 1;
	for ( ; nSeek < 32 ; nSeek++ )
	{
		if ( nSeek >= nInput )
			AfxThrowUserException();
		if ( pInput[ nSeek ] == ':' )
			break;
	}

	if ( nSeek >= 32 )
		AfxThrowUserException();
	int nLen = 0;

	pInput[ nSeek ] = 0;
	if ( sscanf_s( (LPCSTR)pInput, "%i", &nLen ) != 1 )
		AfxThrowUserException();
	pInput[ nSeek ] = ':';
	INC( nSeek + 1 );

	if ( nInput < (DWORD)nLen )
		AfxThrowUserException();

	return nLen;
}

CString CBENode::GetString() const
{
	if ( m_nType != beString )
		return CString();

	CString str;
	LPCSTR szValue = (LPCSTR)m_pValue;

	// Decode from UTF-8
	if ( ::Decode( CP_UTF8, szValue, str ) )
		return str;

	// Use as is
	return CString( szValue );
}

CString CBENode::DecodeString(UINT nCodePage) const
{
	if ( m_nType != beString )
		return CString();

	CString str;
	LPCSTR szValue = (LPCSTR)m_pValue;

	// Use the torrent code page (if present)
	if ( nCodePage != CP_ACP )
	{
		if ( ::Decode( nCodePage, szValue, str ) )
			return str;
	}

	// Try the user-specified code page if it's set (previously Settings.BitTorrent.TorrentCodePage)
	if ( m_nDefaultCP != CP_ACP && m_nDefaultCP != nCodePage )
	{
		if ( ::Decode( m_nDefaultCP, szValue, str ) )
			return str;
	}

	// Try OEM
	UINT nOEMCodePage = GetOEMCP();
	if ( nOEMCodePage != nCodePage && nOEMCodePage != m_nDefaultCP )
	{
		if ( ::Decode( nOEMCodePage, szValue, str ) )
			return str;
	}

	// Try ACP. (Should convert anything, but badly)
	if ( ::Decode( CP_ACP, szValue, str ) )
		return str;

	// Use as is
	return CString( szValue );
}
