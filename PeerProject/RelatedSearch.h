//
// RelatedSearch.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "PeerProjectFile.h"

class CSchema;
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
	CSchema*		m_pSchema;
	CXMLElement*	m_pXML;
	BOOL			m_bXML;

// Operations
public:
	BOOL		CanSearchForThis();
	BOOL		RunSearchForThis();
public:
	BOOL		CanSearchForSimilar();
	BOOL		RunSearchForSimilar();
public:
	BOOL		CanSearchForArtist();
	BOOL		RunSearchForArtist();
public:
	BOOL		CanSearchForAlbum();
	BOOL		RunSearchForAlbum();
public:
	BOOL		CanSearchForSeries();
	BOOL		RunSearchForSeries();
protected:
	static CString Tokenise(LPCTSTR psz);
};
