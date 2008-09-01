//
// SchemaCache.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
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
#include "PeerProject.h"
#include "Settings.h"
#include "SchemaCache.h"
#include "Schema.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSchemaCache SchemaCache;


//////////////////////////////////////////////////////////////////////
// CSchemaCache construction

CSchemaCache::CSchemaCache()
{
	// experimental values
	m_pURIs.InitHashTable( 61 );
	m_pNames.InitHashTable( 61 );
}

CSchemaCache::~CSchemaCache()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CSchemaCache load

int CSchemaCache::Load()
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	HANDLE hSearch;
	int nCount;

#ifdef _DEBUG
	__int64 nStartTotal = GetMicroCount();
#endif

	Clear();

	strPath.Format( _T("%s\\Schemas\\*.xsd"), (LPCTSTR)Settings.General.Path );
	hSearch = FindFirstFile( strPath, &pFind );
	if ( hSearch == INVALID_HANDLE_VALUE ) return 0;
	nCount = 0;

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
			ToLower( strURI );

			m_pURIs.SetAt( strURI, pSchema );
			
			CString strName( pSchema->m_sSingular );
			ToLower( strName );

			m_pNames.SetAt( strName, pSchema );
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
	TRACE( _T("Schemas load time : %I64i ms\n"),
		( nEndTotal - nStartTotal ) / 1000 );
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
}
