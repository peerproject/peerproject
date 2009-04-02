//
// TransferFile.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

class CTransferFile;

class CTransferFiles
{
public:
	CTransferFiles();
	virtual ~CTransferFiles();

	typedef CMap< CString, const CString&, CTransferFile*, CTransferFile* > CTransferFileMap;
	typedef CList< CTransferFile* > CTransferFileList;

	CTransferFile*		Open(LPCTSTR pszFile, BOOL bWrite, BOOL bCreate);
	void				CommitDeferred();

protected:
	CCriticalSection	m_pSection;
	CTransferFileMap	m_pMap;
	CTransferFileList	m_pDeferred;

	void				Close();
	void				QueueDeferred(CTransferFile* pFile);
	void				Remove(CTransferFile* pFile);

	friend class CTransferFile;
};

#define	DEFERRED_MAX	8

class CTransferFile
{
public:
	CTransferFile(LPCTSTR pszPath);

	ULONG		AddRef();
	ULONG		Release();
	HANDLE		GetHandle(BOOL bWrite = FALSE);
	QWORD		GetSize() const;
	BOOL		Read(QWORD nOffset, LPVOID pBuffer, QWORD nBuffer, QWORD* pnRead);
	BOOL		Write(QWORD nOffset, LPCVOID pBuffer, QWORD nBuffer, QWORD* pnWritten);
	BOOL		EnsureWrite();

	inline BOOL	IsOpen() const
	{
		return ( m_hFile != INVALID_HANDLE_VALUE );
	}

	inline BOOL	IsExists() const
	{
		return m_bExists;
	}

protected:
	virtual ~CTransferFile();

	// Deferred Write Structure
	class DefWrite
	{
	public:
		QWORD	m_nOffset;
		DWORD	m_nLength;
		BYTE*	m_pBuffer;
	};

	CString		m_sPath;
	HANDLE		m_hFile;
	BOOL		m_bExists;			// File exists before open
	BOOL		m_bWrite;			// File opened for write operations
	volatile LONG m_nRefCount;
	DefWrite	m_pDeferred[DEFERRED_MAX];
	int			m_nDeferred;

	BOOL		Open(BOOL bWrite, BOOL bCreate);
	BOOL		CloseWrite();
	void		DeferredWrite(BOOL bOffline = FALSE);

	friend class CTransferFiles;
};

extern CTransferFiles TransferFiles;
