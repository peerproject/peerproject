//
// Buffer.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once


class CBuffer
{
// Construction
public:
	CBuffer(DWORD* pLimit = NULL);
	virtual ~CBuffer();

// Attributes
public:
	CBuffer*	m_pNext;
	BYTE*		m_pBuffer;
	DWORD		m_nLength;
	DWORD		m_nBuffer;
	
// Operations
public:
	LPVOID	Allocate(DWORD nLength);
	void	Add(const void* pData, DWORD nLength);
	void	Insert(DWORD nOffset, const void* pData, DWORD nLength);
	void	Remove(DWORD nLength);
	void	Clear();
	void	Print(LPCSTR pszText);
	DWORD	Append(CBuffer* pBuffer, DWORD nLength = 0xFFFFFFFF);
	BOOL	ReadLine(CString& strLine, BOOL bPeek = FALSE);

// Extras
public:
#ifdef _WINSOCKAPI_
	DWORD	Receive(SOCKET hSocket);
	DWORD	Send(SOCKET hSocket);
#endif
#ifdef _DEFLATE_
	BOOL	Deflate(BOOL bIfSmaller = FALSE);
	BOOL	Inflate(DWORD nSuggest = 0);
#endif

};

