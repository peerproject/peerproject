//
// Schema.h
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

#include "SchemaMember.h"

class CSchema;
class CSchemaMember;
class CSchemaChild;
class CXMLElement;
class CSchemaBitzi;

class CSchema  
{
// Construction
public:
	CSchema();
	virtual ~CSchema();
	
// Attributes
public:
	int			m_nType;
	CString		m_sTitle;
	CString		m_sPlural;
	CString		m_sSingular;
	int			m_nAvailability;
	BOOL		m_bPrivate;
	CString		m_sDonkeyType;
public:
	CList< CSchemaMember* >	m_pMembers;
	CList< CString >	m_pExtends;
	CList< CSchemaChild* >	m_pContains;
	CString		m_sDefaultColumns;
	CString		m_sTypeFilter;
	CList< CSchemaBitzi* >	m_pBitziMap;
	CString		m_sBitziTest;
	CString		m_sLibraryView;
	CString		m_sHeaderTitle;
	CString		m_sHeaderSubtitle;
	CString		m_sTileLine1;
	CString		m_sTileLine2;
public:
	CString		m_sIcon;
	int			m_nIcon16;
	int			m_nIcon32;
	int			m_nIcon48;
	
	enum { stFile, stFolder };
	enum { saDefault, saAdvanced, saSystem, saMax };

protected:
	CString		m_sURI;
	
// Operations
public:
	POSITION		GetMemberIterator() const;
	CSchemaMember*	GetNextMember(POSITION& pos) const;
	CSchemaMember*	GetMember(LPCTSTR pszName) const;
	INT_PTR			GetMemberCount() const;
	CString			GetFirstMemberName() const;
	void			Clear();
	BOOL			Load(LPCTSTR pszName);
	CSchemaChild*	GetContained(LPCTSTR pszURI) const;
	CString			GetContainedURI(int nType) const;
	CXMLElement*	Instantiate(BOOL bNamespace = FALSE) const;
	BOOL			Validate(CXMLElement* pXML, BOOL bFix);
	CString			GetIndexedWords(CXMLElement* pXML) const;
	CString			GetVisibleWords(CXMLElement* pXML) const;
	void			ResolveTokens(CString& str, CXMLElement* pXML) const;
protected:
	BOOL			LoadSchema(LPCTSTR pszFile);
	BOOL			LoadPrimary(CXMLElement* pRoot, CXMLElement* pType);
	CXMLElement*	GetType(CXMLElement* pRoot, LPCTSTR pszName);
	BOOL			LoadDescriptor(LPCTSTR pszFile);
	void			LoadDescriptorTitles(CXMLElement* pElement);
	void			LoadDescriptorIcons(CXMLElement* pElement);
	void			LoadDescriptorMembers(CXMLElement* pElement);
	void			LoadDescriptorTypeFilter(CXMLElement* pElement);
	void			LoadDescriptorExtends(CXMLElement* pElement);
	void			LoadDescriptorContains(CXMLElement* pElement);
	void			LoadDescriptorBitziImport(CXMLElement* pElement);
	void			LoadDescriptorHeaderContent(CXMLElement* pElement);
	void			LoadDescriptorViewContent(CXMLElement* pElement);
	BOOL			LoadIcon();

// Inlines
public:
	inline CString GetURI() const
	{
		return m_sURI;
	}

	inline bool Equals(CSchema* pSchema) const
	{
		return ( pSchema && ( ( this == pSchema ) || CheckURI( pSchema->m_sURI ) ) );
	}

	inline bool CheckURI(LPCTSTR pszURI) const
	{
		if ( ! pszURI ) return false;
		if ( m_sURI.CompareNoCase( pszURI ) == 0 ) return true;
		for ( POSITION pos = m_pExtends.GetHeadPosition() ; pos ; )
		{
			CString strURI = m_pExtends.GetNext( pos );
			if ( strURI.CompareNoCase( pszURI ) == 0 ) return true;
		}
		return false;
	}

	inline bool FilterType(LPCTSTR pszFile, bool bDefault = false) const
	{
		if ( m_sTypeFilter.IsEmpty() ) return bDefault;

		LPCTSTR pszExt = _tcsrchr( pszFile, '.' );
		if ( pszExt == NULL ) return false;

		CString strExt = _T("|");
		strExt += pszExt;
		strExt += '|';
		ToLower( strExt );

		return m_sTypeFilter.Find( strExt ) >= 0;
	}


// Common Schemas
public:
	static LPCTSTR	uriApplication;
	static LPCTSTR	uriAudio;
	static LPCTSTR	uriArchive;
	static LPCTSTR	uriBook;
	static LPCTSTR	uriImage;
	static LPCTSTR	uriVideo;
	static LPCTSTR	uriROM;
	static LPCTSTR	uriDocument;
	static LPCTSTR	uriSpreadsheet;
	static LPCTSTR	uriPresentation;
	static LPCTSTR	uriCollection;
	static LPCTSTR	uriLibrary;
	static LPCTSTR	uriFolder;
	static LPCTSTR	uriCollectionsFolder;
	static LPCTSTR	uriFavouritesFolder;
	static LPCTSTR	uriSearchFolder;
	static LPCTSTR	uriAllFiles;
	static LPCTSTR	uriApplicationRoot;
	static LPCTSTR	uriApplicationAll;
	static LPCTSTR	uriArchiveRoot;
	static LPCTSTR	uriArchiveAll;
	static LPCTSTR	uriBookRoot;
	static LPCTSTR	uriBookAll;
	static LPCTSTR	uriImageRoot;
	static LPCTSTR	uriImageAll;
	static LPCTSTR	uriImageAlbum;
	static LPCTSTR	uriMusicRoot;
	static LPCTSTR	uriMusicAll;
	static LPCTSTR	uriMusicAlbumCollection;
	static LPCTSTR	uriMusicArtistCollection;
	static LPCTSTR	uriMusicGenreCollection;
	static LPCTSTR	uriMusicAlbum;
	static LPCTSTR	uriMusicArtist;
	static LPCTSTR	uriMusicGenre;
	static LPCTSTR	uriVideoRoot;
	static LPCTSTR	uriVideoAll;
	static LPCTSTR	uriVideoSeriesCollection;
	static LPCTSTR	uriVideoSeries;
	static LPCTSTR	uriVideoFilmCollection;
	static LPCTSTR	uriVideoFilm;
	static LPCTSTR	uriVideoMusicCollection;
	static LPCTSTR	uriDocumentRoot;
	static LPCTSTR	uriDocumentAll;
	static LPCTSTR	uriGhostFolder;
	static LPCTSTR	uriComments;
	static LPCTSTR	uriBitTorrent;

	friend class CSchemaMember;

private:
	CSchema(const CSchema&);
	CSchema& operator=(const CSchema&);
};


class CSchemaBitzi
{
public:
	CString		m_sFrom;
	CString		m_sTo;
	double		m_nFactor;
public:
	BOOL		Load(CXMLElement* pXML);
};
