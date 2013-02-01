//
// SchemaCache.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "SchemaCache.h"
#include "Schema.h"
#include "XML.h"
#include "Zlib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

CSchemaCache SchemaCache;


//////////////////////////////////////////////////////////////////////
// CSchemaCache construction

CSchemaCache::CSchemaCache()
{
	// Experimental values
	m_pURIs.InitHashTable( 61 );
	m_pNames.InitHashTable( 61 );
	m_pTypeFilters.InitHashTable( 1021 );
}

CSchemaCache::~CSchemaCache()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CSchemaCache load

int CSchemaCache::Load()
{
#ifdef _DEBUG
	__int64 nStartTotal = GetMicroCount();
#endif

	Clear();

	CString strPath;
	strPath.Format( _T("%s\\Schemas\\*.xsd"), (LPCTSTR)Settings.General.Path );

	WIN32_FIND_DATA pFind = {};
	HANDLE hSearch = FindFirstFile( strPath, &pFind );
	if ( hSearch == INVALID_HANDLE_VALUE )
		return 0;	// Missing schemas

	int nCount = 0;
	do
	{
#ifdef _DEBUG
		__int64 nStart = GetMicroCount();
#endif
		strPath.Format( _T("%s\\Schemas\\%s"), (LPCTSTR)Settings.General.Path, pFind.cFileName );

		CSchema* pSchema = new CSchema();
		if ( pSchema && pSchema->Load( strPath ) )
		{
			CString strURI( pSchema->GetURI() );
			strURI.MakeLower();

			m_pURIs.SetAt( strURI, pSchema );

			CString strName( pSchema->m_sSingular );
			strName.MakeLower();

			m_pNames.SetAt( strName, pSchema );

			for ( POSITION pos = pSchema->GetFilterIterator() ; pos ; )
			{
				CString strType;
				BOOL bResult;
				pSchema->GetNextFilter( pos, strType, bResult );
				if ( bResult )
					m_pTypeFilters.SetAt( strType, pSchema );
			}

			nCount++;
		}
		else
		{
			delete pSchema;
			pSchema = NULL;
		}

#ifdef _DEBUG
		__int64 nEnd = GetMicroCount();
		TRACE( _T("Schema \"%s\" load time : %I64i ms : %s\n"), strPath,
			( nEnd - nStart ) / 1000, pSchema ? _T("SUCCESS") : _T("FAILED") );
#endif
	}
	while ( FindNextFile( hSearch, &pFind ) );

	FindClose( hSearch );

#ifdef _DEBUG
	__int64 nEndTotal = GetMicroCount();
	TRACE( _T("Schemas load time : %I64i ms. Found %d types.\n"),
		( nEndTotal - nStartTotal ) / 1000, m_pTypeFilters.GetCount() );
#endif

	return nCount;
}

//////////////////////////////////////////////////////////////////////
// CSchemaCache clear

void CSchemaCache::Clear()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		delete GetNext( pos );
	}

	m_pURIs.RemoveAll();
	m_pNames.RemoveAll();
	m_pTypeFilters.RemoveAll();
}

bool CSchemaCache::Normalize(CSchemaPtr& pSchema, CXMLElement*& pXML) const
{
	pSchema = NULL;

	if ( pXML )
	{
		pSchema = SchemaCache.Get( pXML->GetAttributeValue( CXMLAttribute::schemaName ) );
		if ( ! pSchema )
		{
			// Schemas do not match by URN, get first element to compare
			// with names map of schemas (which are singulars)
			if ( CXMLElement* pElement = pXML->GetFirstElement() )
			{
				pSchema = Guess( pElement->GetName() );
				if ( pSchema )
				{
					// Strip envelope
					pElement->Detach();
					pXML->Delete();
					pXML = pElement;
				}
			}

			if ( ! pSchema )	// Has no plural envelope
				pSchema = Guess( pXML->GetName() );
		}
	}

	return ( pSchema != NULL );
}

CString CSchemaCache::GetFilter(LPCTSTR pszURI) const
{
	if ( CSchemaPtr pSchema = Get( pszURI ) )
	{
		LPCTSTR pszURIType;
		if ( pszURI == CSchema::uriAllFiles )
			pszURIType = NULL;
		else if ( pszURI == CSchema::uriApplicationAll )
			pszURIType = CSchema::uriApplication;
		else if ( pszURI == CSchema::uriVideoAll )
			pszURIType = CSchema::uriVideo;
		else if ( pszURI == CSchema::uriMusicAll )
			pszURIType = CSchema::uriAudio;
		else if ( pszURI == CSchema::uriImageAll )
			pszURIType = CSchema::uriImage;
		else if ( pszURI == CSchema::uriArchiveAll )
			pszURIType = CSchema::uriArchive;
		else if ( pszURI == CSchema::uriDocumentAll )
			pszURIType = CSchema::uriDocument;
		else
		{
			ASSERT( FALSE );
			return CString();
		}

		CString strTypes;
		if ( CSchemaPtr pSchemaType = Get( pszURIType ) )
		{
			for ( POSITION pos = pSchemaType->GetFilterIterator() ; pos ; )
			{
				CString strType;
				BOOL bResult;
				pSchemaType->GetNextFilter( pos, strType, bResult );
				if ( bResult )
				{
					if ( strTypes.IsEmpty() )
						strTypes += _T("|*.") + strType;
					else
						strTypes += _T(";*.") + strType;
				}
			}
		}

		if ( strTypes.IsEmpty() )
			return pSchema->m_sHeaderTitle + _T("|*.*|");

		return pSchema->m_sHeaderTitle + strTypes + _T("|");
	}

	return CString();
}
