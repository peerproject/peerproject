//
// RelatedSearch.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "PeerProjectFile.h"
#include "Schema.h"

class CXMLElement;
class CMatchFile;
class CLibraryFile;


class CRelatedSearch : public CPeerProjectFile
{
// Construction
public:
	CRelatedSearch(CMatchFile* pFile);
	CRelatedSearch(CLibraryFile* pFile);
	virtual ~CRelatedSearch();

// Attributes
public:
	CSchemaPtr		m_pSchema;
	CXMLElement*	m_pXML;
	BOOL			m_bXML;

// Operations
public:
	BOOL		CanSearchForThis();
	BOOL		RunSearchForThis();
	BOOL		CanSearchForSimilar();
	BOOL		RunSearchForSimilar();
	BOOL		CanSearchForArtist();
	BOOL		RunSearchForArtist();
	BOOL		CanSearchForAlbum();
	BOOL		RunSearchForAlbum();
	BOOL		CanSearchForSeries();
	BOOL		RunSearchForSeries();
protected:
	static CString Tokenise(LPCTSTR psz);
};
