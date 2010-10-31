//
// Uploads.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

class CBuffer;
class CConnection;
class CUploadTransfer;
class CPeerProjectFile;


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

	BOOL		AllowMoreTo(IN_ADDR* pAddress) const;
	BOOL		CanUploadFileTo(IN_ADDR* pAddress, const CPeerProjectFile* pFile) const;
	BOOL		EnforcePerHostLimit(CUploadTransfer* pUpload, BOOL bRequest = FALSE);

	void		SetStable(DWORD nSpeed);
	DWORD		GetBandwidth() const;
	void		OnRun();
	BOOL		OnAccept(CConnection* pConnection);
	void		OnRename(LPCTSTR pszSource, LPCTSTR pszTarget);	// pszTarget: 0 = delete file, 1 = release file

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
