//
// SharedFolder.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#include "LibraryFolders.h"

class CLibraryList;
class CXMLElement;


class CLibraryFolder : public CComObject
{
	DECLARE_DYNAMIC(CLibraryFolder)

// Construction
public:
	CLibraryFolder(CLibraryFolder* pParent, LPCTSTR pszPath = NULL);
	virtual ~CLibraryFolder();

// Attributes
public:
	DWORD			m_nUpdateCookie;
	DWORD			m_nSelectCookie;
	CLibraryFolder*	m_pParent;
	CString			m_sName;
	CString			m_sPath;
	BOOL			m_bExpanded;
	DWORD			m_nFiles;
	QWORD			m_nVolume;

protected:
	DWORD			m_nScanCookie;
	CString			m_sNameLC;
	TRISTATE		m_bShared;
	CMap< CString, const CString&, CLibraryFile*, CLibraryFile* >		m_pFiles;
	CMap< CString, const CString&, CLibraryFolder*, CLibraryFolder* >	m_pFolders;
	HANDLE			m_hMonitor;
	BOOL			m_bForceScan;		// TRUE - next scan forced (root folder only)
	BOOL			m_bOffline;			// TRUE - folder absent (root folder only)
	Hashes::Guid	m_oGUID;

// Operations
public:
	CXMLElement*	CreateXML(CXMLElement* pRoot, BOOL bSharedOnly, XmlType nType) const;
	POSITION		GetFolderIterator() const;
	CLibraryFolder*	GetNextFolder(POSITION& pos) const;
	CLibraryFolder*	GetFolderByName(LPCTSTR pszName) const;
	CLibraryFolder*	GetFolderByPath(LPCTSTR pszPath) const;
	BOOL			CheckFolder(CLibraryFolder* pFolder, BOOL bRecursive = FALSE) const;
	DWORD			GetFolderCount() const;
	CLibraryFile*	AddFile(LPCTSTR szName, BOOL& bNew);		// Add new or get existing file
	BOOL			OnFileDelete(CLibraryFile* pRemovingFile);	// Remove existing file recursively.  TRUE if file found and removed, FALSE when not found
	POSITION		GetFileIterator() const;
	CLibraryFile*	GetNextFile(POSITION& pos) const;
	CLibraryFile*	GetFile(LPCTSTR pszName) const;
	DWORD			GetFileCount() const;
	DWORD			GetFileList(CLibraryList* pList, BOOL bRecursive) const;
	DWORD			GetSharedCount(BOOL bRecursive = TRUE) const;
	CString			GetRelativeName() const;

	void			Scan();
	BOOL			IsShared() const;
	// Set to TRUE if shared, to FALSE if not, and leave unchanged if unknown
	void			GetShared(BOOL& bShared) const;
	void			SetShared(TRISTATE bShared);
	BOOL			IsOffline() const;
	BOOL			SetOffline();
	BOOL			SetOnline();
	void			Serialize(CArchive& ar, int nVersion);
	BOOL			ThreadScan(DWORD nScanCookie = 0);
	// Manage filesystem change notification. Returns TRUE if changes detected.
	BOOL			IsChanged();
	void			OnDelete(TRISTATE bCreateGhost = TRI_UNKNOWN);
	void			OnFileRename(CLibraryFile* pFile);
	void			Maintain(BOOL bAdd);

protected:
	// Disable change notification monitor
	void			CloseMonitor();
	void			Clear();
	void			PathToName();
	bool			operator==(const CLibraryFolder& val) const;
	void			RenewGUID();

// Automation
protected:
	BEGIN_INTERFACE_PART(LibraryFolder, ILibraryFolder)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get_Parent)(ILibraryFolder FAR* FAR* ppFolder);
		STDMETHOD(get_Path)(BSTR FAR* psPath);
		STDMETHOD(get_Name)(BSTR FAR* psPath);
		STDMETHOD(get_Shared)(TRISTATE FAR* pnValue);
		STDMETHOD(put_Shared)(TRISTATE nValue);
		STDMETHOD(get_EffectiveShared)(VARIANT_BOOL FAR* pbValue);
		STDMETHOD(get_Folders)(ILibraryFolders FAR* FAR* ppFolders);
		STDMETHOD(get_Files)(ILibraryFiles FAR* FAR* ppFiles);
	END_INTERFACE_PART(LibraryFolder)
	BEGIN_INTERFACE_PART(LibraryFolders, ILibraryFolders)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get__NewEnum)(IUnknown FAR* FAR* ppEnum);
		STDMETHOD(get_Item)(VARIANT vIndex, ILibraryFolder FAR* FAR* ppFolder);
		STDMETHOD(get_Count)(LONG FAR* pnCount);
	END_INTERFACE_PART(LibraryFolders)
	BEGIN_INTERFACE_PART(LibraryFiles, ILibraryFiles)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get__NewEnum)(IUnknown FAR* FAR* ppEnum);
		STDMETHOD(get_Item)(VARIANT vIndex, ILibraryFile FAR* FAR* ppFile);
		STDMETHOD(get_Count)(LONG FAR* pnCount);
	END_INTERFACE_PART(LibraryFiles)

	DECLARE_INTERFACE_MAP()
};
