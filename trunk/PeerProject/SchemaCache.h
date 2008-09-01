//
// SchemaCache.h
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

#include "Schema.h"


class CSchemaCache
{
// Construction
public:
	CSchemaCache();
	virtual ~CSchemaCache();

// Attributes
protected:
	CMap< CString, const CString&, CSchema*, CSchema* > m_pURIs;
	CMap< CString, const CString&, CSchema*, CSchema* >	m_pNames;

// Operations
public:
	BOOL		Load();
	void		Clear();

// Inlines
public:
	POSITION GetIterator() const
	{
		return m_pURIs.GetStartPosition();
	}
	
	CSchema* GetNext(POSITION& pos) const
	{
		CSchema* pSchema = NULL;
		CString strURI;
		m_pURIs.GetNextAssoc( pos, strURI, pSchema );
		return pSchema;
	}
	
	CSchema* Get(LPCTSTR pszURI) const
	{
		if ( ! pszURI || ! *pszURI ) return NULL;
		CString strURI( pszURI );
		ToLower( strURI );

		CSchema* pSchema = NULL;
		return ( m_pURIs.Lookup( strURI, pSchema ) ) ? pSchema : NULL;
	}
	
	CSchema* Guess(LPCTSTR pszName) const
	{
		if ( ! pszName || ! *pszName ) return NULL;
		CString strName( pszName );
		ToLower( strName );

		CSchema* pSchema = NULL;

		// A quick hack for Limewire documents schema
		// ToDo: Remove it when the full schema mapping is ready
		if ( strName == L"document" )
			return m_pNames.Lookup( L"wordprocessing", pSchema ) ? pSchema : NULL;

		return m_pNames.Lookup( strName, pSchema ) ? pSchema : NULL;
	}

};

extern CSchemaCache	SchemaCache;

// Compare two schema URIs with schema mapping
inline bool CheckURI(const CString& strURI1, LPCTSTR szURI2)
{
	if ( strURI1.CompareNoCase( szURI2 ) == 0 )
		return true;
	CSchema* pSchema1 = SchemaCache.Get( strURI1 );
	if ( pSchema1 && pSchema1->CheckURI( szURI2 ) )
		return true;
	CSchema* pSchema2 = SchemaCache.Get( szURI2 );
	if ( pSchema2 && pSchema2->CheckURI( strURI1 ) )
		return true;
	return false;
}
