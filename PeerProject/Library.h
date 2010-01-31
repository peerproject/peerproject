//
// Library.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "ComObject.h"
#include "ThreadImpl.h"
#include "SharedFile.h"

class CQueryHit;
class CQuerySearch;
class CLibraryFolder;
class CAlbumFolder;

#define LIBRARY_SER_VERSION		1000	//29
// nVersion History:
// 27 - Changed CLibraryFile metadata saving order (ryo-oh-ki)
// 28 - Added CLibraryMaps m_pIndexMap, m_pNameMap and m_pPathMap counts (ryo-oh-ki)
// 29 - Added CLibraryDictionary serialize (ryo-oh-ki)
// 1000 - (PeerProject 1.0) (29)

class CLibrary :
	public CComObject,
	public CThreadImpl
{
// Construction
public:
	CLibrary();
	virtual ~CLibrary();

	DECLARE_DYNAMIC(CLibrary)

// Attributes
public:
	CMutexEx		m_pSection;

protected:
	int				m_nFileSwitch;			// Library next save slot number
	volatile LONG	m_nUpdateCookie;		// Library cookie (ms)
	volatile LONG	m_nScanCookie;			// Used by CLibraryFolder::ThreadScan()
	volatile DWORD	m_nScanCount;			// Library scan counter
	volatile DWORD	m_nScanTime;			// Last library scan time (ms)
	volatile LONG	m_nForcedUpdate;		// Forced update request
	volatile LONG	m_nSaveCookie;			// Library last save cookie (ms)
	volatile DWORD	m_nSaveTime;			// Library last save time (ms)

// Sync Operations
public:
	inline DWORD GetCookie() const
	{
		return m_nUpdateCookie;
	}

	inline DWORD GetScanCookie()
	{
		return (DWORD)InterlockedIncrement( &m_nScanCookie );
	}

	inline DWORD GetScanCount() const
	{
		return m_nScanCount;
	}

	// Mark library as modified:
	// bForce = false -Library has internal changes so it must be saved
	// bForce = true -Library also has disk changes so it must be rescanned
	inline void Update(bool bForce = false)
	{
		InterlockedExchange( &m_nUpdateCookie, (LONG)GetTickCount() );

		if ( bForce )
			InterlockedExchange( &m_nForcedUpdate, TRUE );
	}

// File and Folder Operations
public:
	CAlbumFolder*	GetAlbumRoot();
	CLibraryFile*	LookupFile(DWORD_PTR nIndex, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;
	void			AddFile(CLibraryFile* pFile);
	void			RemoveFile(CLibraryFile* pFile);
	void			CheckDuplicates(LPCTSTR pszMD5Hash);

protected:
	void			CheckDuplicates(CLibraryFile* pFile, bool bForce = false);

// General Operations
public:
	// Update library files alternate sources
	bool			OnQueryHits(const CQueryHit* pHits);
	CFileList*		Search(const CQuerySearch* pSearch, int nMaximum = 0, bool bLocal = false, bool bAvailableOnly = false);
	void			Clear();
	BOOL			Load();
	BOOL			Save();
	inline void		StopThread()
	{
		CloseThread();
	}
	static BOOL		IsBadFile(LPCTSTR szFilenameOnly, LPCTSTR szPathOnly = NULL, DWORD dwFileAttributes = 0);

protected:
	void			OnRun();
	void			Serialize(CArchive& ar);
	BOOL			ThreadScan();
	BOOL			SafeReadTime(CFile& pFile, FILETIME* pFileTime) throw();
	BOOL			SafeSerialize(CArchive& ar) throw();

// Automation
protected:
	BEGIN_INTERFACE_PART(Library, ILibrary)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get_Folders)(ILibraryFolders FAR* FAR* ppFolders);
		STDMETHOD(get_Albums)(IUnknown FAR* FAR* ppAlbums);
		STDMETHOD(get_Files)(ILibraryFiles FAR* FAR* ppFiles);
		STDMETHOD(FindByName)(BSTR sName, ILibraryFile FAR* FAR* ppFile);
		STDMETHOD(FindByPath)(BSTR sPath, ILibraryFile FAR* FAR* ppFile);
		STDMETHOD(FindByURN)(BSTR sURN, ILibraryFile FAR* FAR* ppFile);
		STDMETHOD(FindByIndex)(LONG nIndex, ILibraryFile FAR* FAR* ppFile);
	END_INTERFACE_PART(Library)

	DECLARE_INTERFACE_MAP()
};

extern CLibrary Library;

#include "LibraryList.h"
#include "LibraryMaps.h"
