//
// SchemaChild.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#pragma once

#include "Schema.h"

class CSchemaChildMap;
class CXMLElement;


class CSchemaChild
{
// Construction
public:
	CSchemaChild(CSchemaPtr pSchema);
	virtual ~CSchemaChild();

// Attributes
public:
	CSchemaPtr	m_pSchema;
	int			m_nType;
	CString		m_sURI;
public:
	CList< CSchemaChildMap* >	m_pMap;

// Operations
public:
	BOOL		Load(CXMLElement* pXML);
	void		Clear();
	BOOL		MemberCopy(CXMLElement* pLocal, CXMLElement* pRemote, BOOL bToRemote = FALSE, BOOL bAggressive = FALSE);

};


class CSchemaChildMap
{
// Construction
public:
	CSchemaChildMap();
	virtual ~CSchemaChildMap();

// Attributes
public:
	BOOL		m_bIdentity;
	CString		m_sLocal;
	CString		m_sRemote;

// Operations
public:
	BOOL		Load(CXMLElement* pXML);
};
