//
// SchemaCache.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2008.
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

#include "Schema.h"


class CSchemaCache
{
// Construction
public:
	CSchemaCache();
	virtual ~CSchemaCache();

// Operations
public:
	int		Load();
	void	Clear();

// Inlines
public:
	POSITION GetIterator() const
	{
		return m_pURIs.GetStartPosition();
	}

	CSchemaPtr GetNext(POSITION& pos) const
	{
		CSchemaPtr pSchema = NULL;
		CString strURI;
		m_pURIs.GetNextAssoc( pos, strURI, pSchema );
		return pSchema;
	}

	CSchemaPtr Get(LPCTSTR pszURI) const
	{
		if ( ! pszURI || ! *pszURI ) return NULL;
		CString strURI( pszURI );
		strURI.MakeLower();

		CSchemaPtr pSchema = NULL;
		return ( m_pURIs.Lookup( strURI, pSchema ) ) ? pSchema : NULL;
	}

	CSchemaPtr Guess(LPCTSTR pszName) const
	{
		if ( ! pszName || ! *pszName ) return NULL;
		CString strName( pszName );
		strName.MakeLower();

		CSchemaPtr pSchema = NULL;

		// A quick hack for Limewire documents schema
		// ToDo: Remove it when the full schema mapping is verified
		//if ( strName == L"document" )
		//	return m_pNames.Lookup( L"wordprocessing", pSchema ) ? pSchema : NULL;

		return m_pNames.Lookup( strName, pSchema ) ? pSchema : NULL;
	}

	inline BOOL IsFilter(const CString& sType) const
	{
		return ( m_pTypeFilters.PLookup( sType ) != NULL );
	}

	// Decode metadata and Schema from text or XML deflated or plain
	CXMLElement* Decode(BYTE* pszData, DWORD nLength, CSchemaPtr& pSchema);
	static CXMLElement* AutoDetectSchema(LPCTSTR pszInfo);
	static CXMLElement* AutoDetectAudio(LPCTSTR pszInfo);

private:
	typedef CMap< CString, const CString&, CSchemaPtr, CSchemaPtr > CSchemaMap;
	CSchemaMap m_pURIs;
	CSchemaMap m_pNames;
	CSchemaMap m_pTypeFilters;	// Combined "file type":"schema"

	CSchemaCache(const CSchemaCache&);
	CSchemaCache& operator=(const CSchemaCache&);
};

extern CSchemaCache	SchemaCache;

// Compare two schema URIs with schema mapping
inline bool CheckURI(const CString& strURI1, LPCTSTR szURI2)
{
	if ( strURI1.CompareNoCase( szURI2 ) == 0 )
		return true;
	CSchemaPtr pSchema1 = SchemaCache.Get( strURI1 );
	if ( pSchema1 && pSchema1->CheckURI( szURI2 ) )
		return true;
	CSchemaPtr pSchema2 = SchemaCache.Get( szURI2 );
	if ( pSchema2 && pSchema2->CheckURI( strURI1 ) )
		return true;
	return false;
}
