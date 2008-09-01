//
// Uploads.h
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

class CBuffer;
class CConnection;
class CUploadTransfer;


class CUploads  
{
// Construction
public:
	CUploads();
	virtual ~CUploads();
	
// Attributes
public:
	DWORD		m_nCount;			// Active count
	DWORD		m_nBandwidth;		// Total speed
	DWORD		m_nTorrentSpeed;	// BitTorrent clamp
public:
	BOOL		m_bStable;			// Stable flag
	DWORD		m_nBestSpeed;		// Best speed
protected:
	CList< CUploadTransfer* >	m_pList;

// Operations
public:
	void		Clear(BOOL bMessage = TRUE);
	DWORD		GetCount(CUploadTransfer* pExcept, int nState = -1) const;
	DWORD		GetTorrentCount(int nState) const;
public:
	BOOL		AllowMoreTo(IN_ADDR* pAddress) const;
	BOOL		CanUploadFileTo(IN_ADDR* pAddress, const Hashes::Sha1Hash& oSHA1) const;
	BOOL		EnforcePerHostLimit(CUploadTransfer* pUpload, BOOL bRequest = FALSE);
public:
	void		SetStable(DWORD nSpeed);
	DWORD		GetBandwidth() const;
	void		OnRun();
	BOOL		OnAccept(CConnection* pConnection);
	bool		OnRename(const CString& strSource, LPCTSTR pszTarget = (LPCTSTR)1, bool bRemoving = false);
public:
	void		Add(CUploadTransfer* pUpload);
	void		Remove(CUploadTransfer* pUpload);

// List Access
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}
	
	inline CUploadTransfer* GetNext(POSITION& pos) const
	{
		return m_pList.GetNext( pos );
	}
	
	inline BOOL Check(CUploadTransfer* pUpload) const
	{
		return m_pList.Find( pUpload ) != NULL;
	}
	
	inline INT_PTR GetTransferCount() const
	{
		return GetCount( NULL, -2 );
	}

	inline DWORD GetTorrentTransferCount() const
	{
		return GetTorrentCount( -2 );
	}

	inline DWORD GetTorrentUploadCount() const
	{
		return GetTorrentCount( -3 );
	}
	
};

extern CUploads Uploads;
