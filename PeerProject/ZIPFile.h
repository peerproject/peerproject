//
// ZIPFile.h
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

class CBuffer;


class CZIPFile
{
// Construction
public:
	CZIPFile(HANDLE hAttach = INVALID_HANDLE_VALUE);
	~CZIPFile();

// File Class
public:
	class File
	{
	private:
		friend class CZIPFile;
		inline File() {};
		CZIPFile*	m_pZIP;
	public:
		CBuffer*	Decompress();
		BOOL		Extract(LPCTSTR pszFile);
	public:
		CString		m_sName;
		QWORD		m_nSize;
	protected:
		QWORD		m_nLocalOffset;
		QWORD		m_nCompressedSize;
		int			m_nCompression;
		BOOL		PrepareToDecompress(LPVOID pStream);
	};

// Attributes
protected:
	BOOL	m_bAttach;
	HANDLE	m_hFile;
	File*	m_pFile;
	int		m_nFile;

// Operations
public:
	BOOL	Open(LPCTSTR pszFile);
	BOOL	Attach(HANDLE hFile);
	BOOL	IsOpen() const;
	void	Close();
public:
	int		GetCount() const;
	File*	GetFile(int nFile) const;
	File*	GetFile(LPCTSTR pszFile, BOOL bPartial = FALSE) const;
protected:
	BOOL	LocateCentralDirectory();
	BOOL	ParseCentralDirectory(BYTE* pDirectory, DWORD nDirectory);
	BOOL	SeekToFile(File* pFile);

};
