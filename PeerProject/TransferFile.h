//
// TransferFile.h
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

#if !defined(AFX_TRANSFERFILE_H__FF7BC368_5878_4BCF_A2AD_055B0355AC3A__INCLUDED_)
#define AFX_TRANSFERFILE_H__FF7BC368_5878_4BCF_A2AD_055B0355AC3A__INCLUDED_

#pragma once

class CTransferFile;

class CTransferFiles
{
// Construction
public:
	CTransferFiles();
	virtual ~CTransferFiles();

// Attributes
public:
	CCriticalSection	m_pSection;
	CMap< CString, const CString&, CTransferFile*, CTransferFile* > m_pMap;
	CList< CTransferFile* > m_pDeferred;

// Operations
public:
	CTransferFile*	Open(LPCTSTR pszFile, BOOL bWrite, BOOL bCreate);
	void			Close();
	void			CommitDeferred();
protected:
	void			QueueDeferred(CTransferFile* pFile);
	void			Remove(CTransferFile* pFile);

	friend class CTransferFile;
};

#define	DEFERRED_MAX	8

class CTransferFile
{
// Construction
public:
	CTransferFile(LPCTSTR pszPath);
	virtual ~CTransferFile();

// Deferred Write Structure
protected:
	class DefWrite
	{
	public:
		QWORD	m_nOffset;
		DWORD	m_nLength;
		BYTE*	m_pBuffer;
	};

// Attributes
protected:
	CString		m_sPath;
	HANDLE		m_hFile;
	BOOL		m_bWrite;
	DWORD		m_nReference;
protected:
	DefWrite	m_pDeferred[DEFERRED_MAX];
	int			m_nDeferred;

// Operations
public:
	void		AddRef();
	void		Release(BOOL bWrite);
	HANDLE		GetHandle(BOOL bWrite = FALSE);
	BOOL		IsOpen();
	BOOL		Read(QWORD nOffset, LPVOID pBuffer, QWORD nBuffer, QWORD* pnRead);
	BOOL		Write(QWORD nOffset, LPCVOID pBuffer, QWORD nBuffer, QWORD* pnWritten);
protected:
	BOOL		Open(BOOL bWrite, BOOL bCreate);
	BOOL		EnsureWrite();
	BOOL		CloseWrite();
	void		DeferredWrite(BOOL bOffline = FALSE);

	friend class CTransferFiles;

};


extern CTransferFiles TransferFiles;

#endif // !defined(AFX_TRANSFERFILE_H__FF7BC368_5878_4BCF_A2AD_055B0355AC3A__INCLUDED_)
