//
// BENode.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) � 2008-2010
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

#include "StdAfx.h"
#include "..\TorrentWizard.h"
#include "..\Buffer.h"
#include "BENode.h"
#include "SHA1.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CBENode construction/destruction

CBENode::CBENode()
{
	m_nType		= beNull;
	m_pValue	= NULL;
	m_nValue	= 0;
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

	CBENode* pNew = new CBENode();

	if ( m_nType == beList )
	{
		CBENode** pList = new CBENode*[ (DWORD)m_nValue + 1 ];

		if ( m_pValue != NULL )
		{
			CopyMemory( pList, m_pValue, 4 * (DWORD)m_nValue );
			delete [] (CBENode**)m_pValue;
		}

		pList[ m_nValue++ ] = pNew;
		m_pValue = pList;
	}
	else
	{
		CBENode** pList = new CBENode*[ (DWORD)m_nValue * 2 + 2 ];

		if ( m_pValue != NULL )
		{
			CopyMemory( pList, m_pValue, 8 * (DWORD)m_nValue );
			delete [] (CBENode**)m_pValue;
		}

		BYTE* pxKey = new BYTE[ nKey + 1 ];
		CopyMemory( pxKey, pKey, nKey );
		pxKey[ nKey ] = 0;

		pList[ m_nValue * 2 ]		= pNew;
		pList[ m_nValue * 2 + 1 ]	= (CBENode*)pxKey;

		m_pValue = pList;
		m_nValue ++;

		qsort( m_pValue, (size_t)m_nValue, sizeof(*pList) * 2, SortDict );
	}

	return pNew;
}

int __cdecl CBENode::SortDict(const void * pA, const void * pB)
{
	LPCSTR* pszA = (LPCSTR*)pA + 1;
	LPCSTR* pszB = (LPCSTR*)pB + 1;

	return strcmp( *pszA, *pszB );
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
		if ( memcmp( pKey, (LPBYTE)pNode[1], nKey ) == 0 ) return *pNode;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CBENode SHA1 computation

CHashSHA1 CBENode::GetSHA1() const
{
	ASSERT( this != NULL );

	CBuffer pBuffer;
	Encode( &pBuffer );

	CSHA1 pDigest;
	pDigest.Add( pBuffer.m_pBuffer, pBuffer.m_nLength );
	pDigest.Finish();
	return pDigest;
}

//////////////////////////////////////////////////////////////////////
// CBENode encoding

void CBENode::Encode(CBuffer* pBuffer) const
{
	CHAR szBuffer[128];

	ASSERT( this != NULL );
	ASSERT( pBuffer != NULL );
	CString str;

	if ( m_nType == beString )
	{
		sprintf( szBuffer, "%u:", (DWORD)m_nValue );
		pBuffer->Print( szBuffer );
		pBuffer->Add( m_pValue, (DWORD)m_nValue );
	}
	else if ( m_nType == beInt )
	{
		sprintf( szBuffer, "i%I64ie", m_nValue );
		pBuffer->Print( szBuffer );
	}
	else if ( m_nType == beList )
	{
		CBENode** pNode = (CBENode**)m_pValue;

		pBuffer->Print( "l" );

		for ( DWORD nItem = 0 ; nItem < (DWORD)m_nValue ; nItem++, pNode++ )
		{
			(*pNode)->Encode( pBuffer );
		}

		pBuffer->Print( "e" );
	}
	else if ( m_nType == beDict )
	{
		CBENode** pNode = (CBENode**)m_pValue;

		pBuffer->Print( "d" );

		for ( DWORD nItem = 0 ; nItem < m_nValue ; nItem++, pNode += 2 )
		{
			LPCSTR pszKey = (LPCSTR)pNode[1];
			size_t nKeyLength = strlen( pszKey );
			sprintf( szBuffer, "%i:", nKeyLength );
			pBuffer->Print( szBuffer );
			pBuffer->Print( pszKey );
			(*pNode)->Encode( pBuffer );
		}

		pBuffer->Print( "e" );
	}
	else
	{
		ASSERT( FALSE );
	}
}
