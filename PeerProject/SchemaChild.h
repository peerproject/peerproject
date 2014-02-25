//
// SchemaChild.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
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

#pragma once

#include "Schema.h"

class CSchemaChildMap;
class CXMLElement;


class CSchemaChild
{
public:
	CSchemaChild(CSchemaPtr pSchema);
	virtual ~CSchemaChild();

public:
	CSchemaPtr	m_pSchema;
	int			m_nType;
	CString		m_sURI;
public:
	CList< CSchemaChildMap* >	m_pMap;

public:
	BOOL		Load(CXMLElement* pXML);
	void		Clear();
	BOOL		MemberCopy(CXMLElement* pLocal, CXMLElement* pRemote, BOOL bToRemote = FALSE, BOOL bAggressive = FALSE);

};


class CSchemaChildMap
{
public:
	CSchemaChildMap();
	virtual ~CSchemaChildMap();

public:
	BOOL		m_bIdentity;
	CString		m_sLocal;
	CString		m_sRemote;

public:
	BOOL		Load(CXMLElement* pXML);
};
