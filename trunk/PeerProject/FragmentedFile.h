//
// FragmentedFile.h
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

#include "PeerProjectFile.h"
#include "FileFragments.hpp"
#include "TransferFile.h"

class CPeerProjectFile;
class CBTInfo;
class CDownload;


class CFragmentedFile : public CObject
{
	DECLARE_DYNCREATE( CFragmentedFile )

public:
	CFragmentedFile();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Multifile Download priority settings
	// Not used. (Sets removed PageTorrentFiles combobox order)
	// Note: Changing this breaks torrents at startup!
	enum { prUnwanted, prLow, prNormal, prHigh };

protected:
	virtual ~CFragmentedFile();

	class CVirtualFilePart : public CPeerProjectFile
	{
	public:
		CVirtualFilePart();
		CVirtualFilePart(const CVirtualFilePart& p);
		CVirtualFilePart& operator=(const CVirtualFilePart& p);

		void Release();

		inline bool operator ==(LPCTSTR pszFile) const
		{
			return ! m_sPath.CompareNoCase( pszFile );
		}

		inline bool operator <(const CVirtualFilePart& p) const
		{
			return ( m_nOffset < p.m_nOffset );
		}

		CTransferFile*	m_pFile;	// Opened file handler
		QWORD			m_nOffset;	// File offset (0 - for first/single file)
		BOOL			m_bWrite;	// File opened for write
		int				m_nPriority;// Download priority (NotWanted, Low, Normal or High)
	};

	typedef std::vector< CVirtualFilePart > CVirtualFile;

	struct Less : public std::binary_function< CVirtualFilePart, CVirtualFilePart, bool >
	{
		inline bool operator()(const CVirtualFilePart& _Left, const CVirtualFilePart& _Right) const
		{
			return _Left < _Right;
		}
	};

	struct Greater : public std::binary_function< CVirtualFilePart, QWORD, bool >
	{
		inline bool operator()(const CVirtualFilePart& _Left, QWORD _Right) const
		{
			return _Left.m_nOffset > _Right;
		}
	};

	struct Flusher : public std::unary_function< CVirtualFilePart, void >
	{
		inline void operator()(const CVirtualFilePart& p) const
		{
			if ( p.m_pFile )
			{
				FlushFileBuffers( p.m_pFile->GetHandle() );
			}
		}
	};

	struct Releaser : public std::unary_function< CVirtualFilePart, void >
	{
		inline void operator()(CVirtualFilePart& p) const
		{
			if ( p.m_pFile )
			{
				p.m_pFile->Release();
				p.m_pFile = NULL;
			}
		}
	};

	struct Completer : public std::unary_function< CVirtualFilePart, void >
	{
		inline void operator()(const CVirtualFilePart& p) const
		{
			if ( p.m_pFile )
			{
				HANDLE hFile = p.m_pFile->GetHandle( TRUE );
				if ( hFile != INVALID_HANDLE_VALUE )
				{
					LARGE_INTEGER nLength;
					nLength.QuadPart = p.m_nSize;
					SetFilePointerEx( hFile, nLength, NULL, FILE_BEGIN );
					SetEndOfFile( hFile );
				}
			}
		}
	};

	struct EnsureWriter : public std::unary_function< CVirtualFilePart, bool >
	{
		inline bool operator()(const CVirtualFilePart& p) const
		{
			return ! p.m_pFile || p.m_pFile->EnsureWrite();
		}
	};

	mutable CCriticalSection	m_pSection;
	CVirtualFile				m_oFile;
	QWORD						m_nUnflushed;
	Fragments::List				m_oFList;
	DWORD						m_nFileError;
	volatile LONG				m_dwRef;
	const CDownload*			m_pDownload;	// Reference download object (optional)

	BOOL	VirtualRead(QWORD nOffset, char* pBuffer, QWORD nBuffer, QWORD* pnRead);
	BOOL	VirtualWrite(QWORD nOffset, const char* pBuffer, QWORD nBuffer, QWORD* pnWritten);

	// Get completed size of defined range (in bytes)
	QWORD GetCompleted(QWORD nOffset, QWORD nLength) const;

public:
	void	SetDownload(const CDownload* pDownload);
	// Open file from disk
	BOOL	Open(LPCTSTR pszFile, QWORD nOffset = 0, QWORD nLength = SIZE_UNKNOWN,
			BOOL bWrite = FALSE, LPCTSTR pszName = NULL, int nPriority = prNormal );
	// Open file from disk or create file inside incomplete folder from library by hash
	BOOL	Open(const CPeerProjectFile& oSHFile, BOOL bWrite);
	// Open file from disk or create file inside incomplete folder file(s) from .torrent
	BOOL	Open(const CBTInfo& oInfo, BOOL bWrite, CString& sErrorMessage);
	ULONG	AddRef();
	ULONG	Release();
	BOOL	Flush();
	void	Close();
	BOOL	MakeComplete();
	void	Serialize(CArchive& ar, int nVersion);
	BOOL	EnsureWrite();
	// Delete file(s)
	void	Delete();
	// Move file to destination. Returns 0 on success or file error number.
	DWORD	Move(DWORD nIndex, LPCTSTR pszDestination, LPPROGRESS_ROUTINE lpProgressRoutine = NULL, LPVOID lpData = NULL);
	BOOL	Write(QWORD nOffset, LPCVOID pData, QWORD nLength, QWORD* pnWritten = NULL);
	BOOL	Read(QWORD nOffset, LPVOID pData, QWORD nLength, QWORD* pnRead = NULL);
	QWORD	InvalidateRange(QWORD nOffset, QWORD nLength);
	// Check if specified file handled
	BOOL	FindByPath(const CString& sPath) const;

	// Get amount of files
	inline DWORD GetCount() const
	{
		return (DWORD)m_oFile.size();
	}

	// Get subfile
	inline const CPeerProjectFile* GetAt(DWORD nIndex) const
	{
		CQuickLock oLock( m_pSection );

		return nIndex < m_oFile.size() ?
			static_cast< const CPeerProjectFile* >( &m_oFile[ nIndex ] ) : NULL;
	}

	// Are all of subfiles open?
	BOOL IsOpen() const;

	// Get subfile offset
	QWORD GetOffset(DWORD nIndex) const;

	// Get subfile length
	QWORD GetLength(DWORD nIndex) const;

	// Get subfile path
	CString GetPath(DWORD nIndex) const;

	// Set subfile path
	void SetPath(DWORD nIndex, LPCTSTR szPath);

	// Select subfile (with user interaction)
	int SelectFile(CSingleLock* pLock = NULL) const;

	// Get subfile original name
	CString GetName(DWORD nIndex) const;

	// Set subfile original name
	void SetName(DWORD nIndex, LPCTSTR szName);

	// Get subfile priority
	int GetPriority(DWORD nIndex) const;

	// Set subfile priority
	void SetPriority(DWORD nIndex, int nPriority);

	// Get last file/disk error
	inline DWORD GetFileError() const
	{
		return m_nFileError;
	}

	// Is file has size?
	inline BOOL IsValid() const
	{
		return m_oFList.limit() > 0;
	}

	// Returns file download progress ( < 0 - unknown or 0..100% )
	float GetProgress(DWORD nIndex) const;

	// Get total size of whole file (in bytes)
	inline QWORD GetTotal() const
	{
		CQuickLock oLock( m_pSection );

		return m_oFList.limit();
	}

	inline QWORD GetRemaining() const
	{
		CQuickLock oLock( m_pSection );

		return ( ( m_oFList.limit() == 0 ) ||
			( m_oFList.limit() == SIZE_UNKNOWN && m_oFList.length_sum() ) ) ?
			SIZE_UNKNOWN : m_oFList.length_sum();
	}

	// Get list of all fragments which must be downloaded
	Fragments::List GetFullFragmentList() const;

	// Get list of empty fragments
	inline Fragments::List GetEmptyFragmentList() const
	{
		CQuickLock oLock( m_pSection );

		return m_oFList;
	}

	// Get list of empty fragments we really want to download
	Fragments::List GetWantedFragmentList() const;

	inline BOOL IsPositionRemaining(QWORD nOffset) const
	{
		CQuickLock oLock( m_pSection );

		return m_oFList.has_position( nOffset );
	}

	inline BOOL DoesRangeOverlap(QWORD nOffset, QWORD nLength) const
	{
		CQuickLock oLock( m_pSection );

		return m_oFList.overlaps( Fragments::Fragment( nOffset, nOffset + nLength ) );
	}

	inline QWORD GetRangeOverlap(QWORD nOffset, QWORD nLength) const
	{
		CQuickLock oLock( m_pSection );

		return m_oFList.overlapping_sum( Fragments::Fragment( nOffset, nOffset + nLength ) );
	}

	// Get completed size of whole file (in bytes)
	inline QWORD GetCompleted() const
	{
		CQuickLock oLock( m_pSection );

		return m_oFList.missing();
	}

	// Get completed size of subfile (in bytes)
	QWORD GetCompleted(DWORD nIndex) const;

	// Is download complete?
	inline bool IsComplete() const
	{
		CQuickLock oLock( m_pSection );

		return IsValid() && GetWantedFragmentList().empty();
	}

//	inline QWORD GetEmptyFragmentCount() const
//	{
//		CQuickLock oLock( m_pSection );
//
//		return m_oFList.size();
//	}

//	inline BOOL IsFlushNeeded() const
//	{
//		CQuickLock oLock( m_pSection );
//
//		return ( m_pFile != NULL ) && ( m_nUnflushed > 0 );
//	}
};

// For auto_ptr< CFragmentedFile >
template<>
inline void boost::checked_delete< CFragmentedFile >(CFragmentedFile* x)
{
    if ( x )
		x->Release();
}
