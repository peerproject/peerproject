//
// SharedFile.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "PeerProjectFile.h"
#include "Schema.h"

class CLibraryFolder;
class CSharedSource;
class CXMLElement;
class CQuerySearch;
class CLibraryDownload;
class CTigerTree;
class CED2K;
class CDownload;


class CLibraryFile : public CPeerProjectFile
{
	DECLARE_DYNAMIC(CLibraryFile)

public:
	CLibraryFile(CLibraryFolder* pFolder, LPCTSTR pszName = NULL);
	virtual ~CLibraryFile();

	CLibraryFile*	m_pNextSHA1;
	CLibraryFile*	m_pNextTiger;
	CLibraryFile*	m_pNextED2K;
	CLibraryFile*	m_pNextBTH;
	CLibraryFile*	m_pNextMD5;
	DWORD			m_nScanCookie;
	DWORD			m_nUpdateCookie;
	DWORD			m_nSelectCookie;
	DWORD			m_nListCookie;
	DWORD			m_nIndex;
	CLibraryFolder*	m_pFolder;				// NULL for Ghost files
	FILETIME		m_pTime;
	QWORD			m_nVirtualBase;
	QWORD			m_nVirtualSize;
	TRISTATE		m_bVerify;
	CSchemaPtr		m_pSchema;
	CXMLElement*	m_pMetadata;
	FILETIME		m_pMetadataTime;		// Metadata time
	BOOL			m_bMetadataAuto;		// Metadata is auto-generated
	BOOL			m_bMetadataModified;	// Metadata must be saved
	//int			m_nPeerTag;				// Permissiveness
	int				m_nRating;
	CString			m_sComments;
	CString			m_sShareTags;
	DWORD			m_nUploadsToday;
	DWORD			m_nUploadsTotal;
	BOOL			m_bCachedPreview;
	BOOL			m_bBogus;
	CList< CSharedSource* >		m_pSources;
	// Search helper variables
	mutable DWORD				m_nHitsToday;
	mutable DWORD				m_nHitsTotal;
	mutable DWORD				m_nSearchCookie;
	mutable DWORD				m_nSearchWords;
	mutable DWORD				m_nCollIndex;
	mutable const CLibraryFile*	m_pNextHit;
	int				m_nIcon16;
	BOOL			m_bNewFile;

	CString			GetPath() const;
	CString			GetFolder() const;
	CString 		GetSearchName() const;
	BOOL			CheckFileAttributes(QWORD nSize, BOOL bSharedOnly, BOOL bAvailableOnly) const;
	void			SetShared(bool bShared, bool bOverride = false);
	bool			IsShared(bool bIgnoreOverride = false) const;
	inline BOOL		IsSharedOverride() const { return m_bShared != TRI_UNKNOWN; }
	inline BOOL		IsGhost() const { return m_pFolder == NULL; }
	inline BOOL		IsAvailable() const { return m_pFolder != NULL; }
	BOOL			IsSchemaURI(LPCTSTR pszURI) const;
	BOOL			IsRated() const;		// File rated or commented
	BOOL			IsRatedOnly() const;	// File rated but has no metadata
	//BOOL			IsPeerTagged() const;	// Permissiveness  ToDo: PeerTags
	BOOL			IsHashed() const;
	BOOL			IsNewFile() const;
	BOOL			IsReadable() const;
	BOOL			Rebuild();
	BOOL			Rename(LPCTSTR pszName);
	BOOL			Delete(BOOL bDeleteGhost = FALSE);
	void			UpdateMetadata(CDownload* pDownload);
	BOOL			SetMetadata(CXMLElement*& pXML, BOOL bMerge = FALSE, BOOL bOverwrite = FALSE);
	BOOL			MergeMetadata(CXMLElement*& pXML, BOOL bOverwrite);
	void			ModifyMetadata();		// Mark metadata as modified
	void			ClearMetadata();
	CString			GetMetadataWords() const;
	CTigerTree*		GetTigerTree();
	CED2K*			GetED2K();
	CSharedSource*	AddAlternateSource(LPCTSTR pszURL, FILETIME* tSeen = NULL);
	CSharedSource*	AddAlternateSources(LPCTSTR pszURL);
	CString			GetAlternateSources(CList< CString >* pState, int nMaximum, PROTOCOLID nProtocol);

	inline CString GetNameLC() const
	{
		CString str( m_sName );
		return ToLower( str );
	}

	inline QWORD GetBase() const
	{
		return ( m_nVirtualSize ) ? m_nVirtualBase : 0;
	}

	inline QWORD GetSize() const
	{
		return ( m_nVirtualSize ) ? m_nVirtualSize :
			( ( m_nSize == SIZE_UNKNOWN ) ? 0 : m_nSize );
	}

protected:
	TRISTATE	m_bShared;

	void		Serialize(CArchive& ar, int nVersion);
	BOOL		ThreadScan(CSingleLock& pLock, DWORD nScanCookie, QWORD nSize, FILETIME* pTime/*, LPCTSTR pszMetaData*/);
	void		OnDelete(BOOL bDeleteGhost = FALSE, TRISTATE bCreateGhost = TRI_UNKNOWN);
	void		Ghost();
	BOOL		OnVerifyDownload(
					const Hashes::Sha1ManagedHash& oSHA1,
					const Hashes::TigerManagedHash& oTiger,
					const Hashes::Ed2kManagedHash& oED2K,
					const Hashes::BtManagedHash& oBTH,
					const Hashes::Md5ManagedHash& oMD5,
					LPCTSTR pszSources);

	BEGIN_INTERFACE_PART(LibraryFile, ILibraryFile)
		DECLARE_DISPATCH()
		STDMETHOD(get_Path)(BSTR FAR* psPath);
		STDMETHOD(get_Name)(BSTR FAR* psName);
		STDMETHOD(get_Size)(ULONGLONG FAR* pnSize);
		STDMETHOD(get_URN)(BSTR sURN, BSTR FAR* psURN);
		STDMETHOD(get_Hash)(URN_TYPE nType, ENCODING nBase, BSTR FAR* psURN);
		STDMETHOD(get_URL)(BSTR FAR* psURL);
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get_Folder)(ILibraryFolder FAR* FAR* ppFolder);
		STDMETHOD(get_Shared)(TRISTATE FAR* pnValue);
		STDMETHOD(put_Shared)(TRISTATE nValue);
		STDMETHOD(get_EffectiveShared)(VARIANT_BOOL FAR* pbValue);
		STDMETHOD(get_Index)(LONG FAR* pnIndex);
		STDMETHOD(get_MetadataAuto)(VARIANT_BOOL FAR* pbValue);
		STDMETHOD(get_Metadata)(ISXMLElement FAR* FAR* ppXML);
		STDMETHOD(put_Metadata)(ISXMLElement FAR* pXML);
		STDMETHOD(Execute)();
		STDMETHOD(SmartExecute)();
		STDMETHOD(Delete)();
		STDMETHOD(Rename)(BSTR sNewName);
		STDMETHOD(Copy)(BSTR sNewPath);
		STDMETHOD(Move)(BSTR sNewPath);
		STDMETHOD(MergeMetadata)(ISXMLElement* pXML, VARIANT_BOOL bOverwrite, VARIANT_BOOL* pbValue);
	END_INTERFACE_PART(LibraryFile)

	DECLARE_INTERFACE_MAP()

	friend class CLibrary;
	friend class CLibraryFolder;
	friend class CLibraryMaps;
	friend class CLibraryRecent;
	friend class CDeleteFileDlg;
};

typedef CList< const CLibraryFile* > CFileList;
typedef CMap< DWORD_PTR, DWORD_PTR, CLibraryFile*, CLibraryFile* > CIndexMap;
typedef CMap< CString, const CString&, CLibraryFile*, CLibraryFile* > CFileMap;


class CSharedSource
{
// Construction
public:
	CSharedSource(LPCTSTR pszURL = NULL, FILETIME* pTime = NULL);

// Attributes
public:
	CString		m_sURL;					// The URL
	FILETIME	m_pTime;				// Time last seen

// Operations
public:
	void	Serialize(CArchive& ar, int nVersion);
	void	Freshen(FILETIME* pTime = NULL);
	BOOL	IsExpired(FILETIME& tNow);
};