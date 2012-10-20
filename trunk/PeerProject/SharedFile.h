//
// SharedFile.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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
#include "LibraryFolders.h"
#include "Schema.h"

class CLibraryFolder;
class CXMLElement;
class CSharedSource;
class CQuerySearch;
class CLibraryRecent;
class CLibraryDownload;
class CDownload;
class CTigerTree;
class CED2K;


class CLibraryFile : public CPeerProjectFile
{
	DECLARE_DYNAMIC(CLibraryFile)

public:
	CLibraryFile(CLibraryFolder* pFolder, LPCTSTR pszName = NULL);
	virtual ~CLibraryFile();

public:
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
	CLibraryFile*	m_pNextHit;
	DWORD			m_nHitsToday;
	DWORD			m_nHitsTotal;
	DWORD			m_nSearchCookie;
	DWORD			m_nSearchWords;
	DWORD			m_nCollIndex;
	int				m_nIcon16;
	BOOL			m_bNewFile;

	CString			GetPath() const;
	CString			GetFolder() const;
	CString 		GetSearchName() const;
	const CLibraryFolder* GetFolderPtr() const;
	CXMLElement*	CreateXML(CXMLElement* pRoot, BOOL bSharedOnly, XmlType nType) const;

	bool			IsPrivateTorrent() const;
	DWORD			GetCreationTime();				// Get network wide file creation time (seconds, as time())
	BOOL			SetCreationTime(DWORD tTime);	// Set network wide file creation time (seconds, as time())
	BOOL			CheckFileAttributes(QWORD nSize, BOOL bSharedOnly, BOOL bAvailableOnly) const;
	void			SetShared(bool bShared, bool bOverride = false);
	bool			IsShared(bool bIgnoreOverride = false) const;
	inline BOOL		IsSharedOverride() const { return m_bShared != TRI_UNKNOWN; }
	inline BOOL		IsGhost() const { return m_pFolder == NULL; }
	inline BOOL		IsAvailable() const { return m_pFolder != NULL; }
	BOOL			IsSchemaURI(LPCTSTR pszURI) const;
	BOOL			IsRated() const;		// File rated or commented
	BOOL			IsRatedOnly() const;	// File rated but has no metadata
//	BOOL			IsPeerTagged() const;	// Permissiveness  ToDo: PeerTags
	BOOL			IsHashed() const;		// File fully hashed
	BOOL			IsNewFile() const;
	BOOL			IsReadable() const;
	BOOL			Rebuild();
	BOOL			Rename(LPCTSTR pszName);
	BOOL			Delete(BOOL bDeleteGhost = FALSE);
	void			UpdateMetadata(const CDownload* pDownload);
	BOOL			SetMetadata(CXMLElement*& pXML, BOOL bMerge = FALSE, BOOL bOverwrite = FALSE);
	BOOL			MergeMetadata(CXMLElement*& pXML, BOOL bOverwrite);
	BOOL			MergeMetadata(const CXMLElement* pXML);
	void			ModifyMetadata();		// Mark metadata as modified
	void			ClearMetadata();
	CString			GetMetadataWords() const;
	CTigerTree*		GetTigerTree();
	CED2K*			GetED2K();
	CSharedSource*	AddAlternateSource(LPCTSTR pszURL, FILETIME* tSeen = NULL);
	CSharedSource*	AddAlternateSources(LPCTSTR pszURL);
	CString			GetAlternateSources(CList< CString >* pState, int nMaximum, PROTOCOLID nProtocol);
	BOOL			OnVerifyDownload(const CLibraryRecent* pRecent);

	// Adds file data to string array using template. Supported template variables:
	// $meta:name$		- file name
	// $meta:comments$	- file comments
	// $meta:hours$		- hours as decimal from file metadata "minutes" or "seconds" field
	// $meta:minutes$	- minutes as decimal from file metadata "minutes" or "seconds" field
	// $meta:seconds$	- seconds as  decimal from file metadata "minutes" or "seconds" field
	// $meta:time$		- time as string "hours:minutes:seconds" from file metadata "minutes" or "seconds" field
	// $meta:track$		- track as decimal
	// $meta:*$			- other file metadata fields as is
	// $meta:sizebytes$	- file size in bytes
	// $meta:size$		- file size in KB or MB
	// $meta:sha1$		- file SHA1 hash
	// $meta:gnutella$	- file SHA1 link (gnutella://)
	// $meta:tiger$		- file Tiger hash
	// $meta:bitprint$	- file SHA1.Tiger hash
	// $meta:ed2khash$	- file ED2K hash
	// $meta:ed2k$		- file ED2K link (ed2k://|file|)
	// $meta:md5$		- file MD5 hash
	// $meta:btih$		- file BitTorrnet info hash
	// $meta:magnet$	- file magnet-link
	// $meta:number$	- file number in string array
	// Unknown variables will be replaced by "N/A" string.
	BOOL			PrepareDoc(LPCTSTR pszTemplate, CArray< CString >& oDocs);

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
	TRISTATE		m_bShared;
	CLibraryFolder*	m_pFolder;			// NULL for Ghost files
	DWORD			m_tCreateTime;		// Cached network wide file creation time (seconds, as time())

	void			Serialize(CArchive& ar, int nVersion);
	BOOL			ThreadScan(CSingleLock& pLock, DWORD nScanCookie, QWORD nSize, FILETIME* pTime/*, LPCTSTR pszMetaData*/);
	void			OnDelete(BOOL bDeleteGhost = FALSE, TRISTATE bCreateGhost = TRI_UNKNOWN);
	void			Ghost();

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
	friend class CDeleteFileDlg;
};

typedef CList< CLibraryFile* > CFileList;
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
