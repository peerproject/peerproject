//
// LibraryFolders.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
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

class CLibraryFolder;
class CAlbumFolder;
class CLibraryFile;
class CCollectionFile;
class CXMLElement;

enum XmlType
{
	xmlDefault,		// Default
	xmlDC			// DC++ file listing
};


class CLibraryFolders : public CComObject
{
	DECLARE_DYNAMIC(CLibraryFolders)

// Construction
public:
	CLibraryFolders();
	virtual ~CLibraryFolders();

// Attributes
protected:
	CList< CLibraryFolder* > m_pFolders;
	CAlbumFolder*	m_pAlbumRoot;

// Physical Folder Operations
public:
	CXMLElement*	CreateXML(LPCTSTR szRoot, BOOL bSharedOnly, XmlType nType) const;
	POSITION		GetFolderIterator() const;
	CLibraryFolder*	GetNextFolder(POSITION& pos) const;
	INT_PTR			GetFolderCount() const { return m_pFolders.GetCount(); }
	CLibraryFolder*	GetFolder(LPCTSTR pszPath) const;
	BOOL			CheckFolder(CLibraryFolder* pFolder, BOOL bRecursive = FALSE) const;
	CLibraryFolder*	GetFolderByName(LPCTSTR pszName) const;
	CLibraryFolder*	AddFolder(LPCTSTR pszPath);
	CLibraryFolder*	AddFolder(LPCTSTR pszPath, BOOL bShared);
	bool			AddSharedFolder(CListCtrl& oList);
	BOOL			RemoveFolder(CLibraryFolder* pFolder);
	CLibraryFolder*	IsFolderShared(const CString& strPath) const;
	CLibraryFolder*	IsSubFolderShared(const CString& strPath) const;
	static bool		IsShareable(const CString& strPath);
	void			Maintain();

// Virtual Album Operations
	BOOL			CheckAlbum(CAlbumFolder* pFolder) const;
	CAlbumFolder* 	CreateAlbumTree();
	CAlbumFolder*	GetAlbumRoot() const;
	CAlbumFolder*	GetAlbumTarget(LPCTSTR pszSchemaURI, LPCTSTR pszMember, LPCTSTR pszValue) const;
	CAlbumFolder*	GetCollection(const Hashes::Sha1Hash& oSHA1);
	BOOL			MountCollection(const Hashes::Sha1Hash& oSHA1, CCollectionFile* pCollection);
	BOOL			OnFileDelete(CLibraryFile* pFile, BOOL bDeleteGhost = FALSE);	// Remove file from all albums and folders
	void			ClearGhosts();				// Remove all ghost files
	DWORD			GetGhostCount() const;		// Get total amount of ghost files


// Core
protected:
	void			Clear();
	void			Serialize(CArchive& ar, int nVersion);
	BOOL			ThreadScan(const BOOL bForce = FALSE);

// COM
protected:
	BEGIN_INTERFACE_PART(LibraryFolders, ILibraryFolders)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get__NewEnum)(IUnknown FAR* FAR* ppEnum);
		STDMETHOD(get_Item)(VARIANT vIndex, ILibraryFolder FAR* FAR* ppFolder);
		STDMETHOD(get_Count)(LONG FAR* pnCount);
	END_INTERFACE_PART(LibraryFolders)

	DECLARE_INTERFACE_MAP()

	friend class CLibrary;
};

extern CLibraryFolders LibraryFolders;
