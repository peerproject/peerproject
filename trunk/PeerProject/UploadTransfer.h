//
// UploadTransfer.h
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

#if !defined(AFX_UPLOADTRANSFER_H__16B2B6A4_C939_4018_81B6_B1C4460B3CD0__INCLUDED_)
#define AFX_UPLOADTRANSFER_H__16B2B6A4_C939_4018_81B6_B1C4460B3CD0__INCLUDED_

#pragma once

#include "Transfer.h"
#include "PeerProjectFile.h"

class CUploadQueue;
class CUploadFile;
class CTransferFile;
class CLibraryFile;
class CDownload;

#define ULA_SLOTS	16


class CUploadTransfer : public CTransfer, public CPeerProjectFile
{
// Construction
public:
	CUploadTransfer(PROTOCOLID nProtocol);
	virtual ~CUploadTransfer();

// Attributes
public:
	int				m_nState;		// Common state code
	CUploadQueue*	m_pQueue;		// Queue reference
	CUploadFile*	m_pBaseFile;	// Reference file
	CTransferFile*	m_pDiskFile;	// Disk file
	DWORD			m_nBandwidth;	// Bandwidth
	CString			m_sNick;		// User Nick
	DWORD			m_nUserRating;	// Has the downloader uploaded anything?
	BOOL			m_bClientExtended;// Does the user support extended (G2) functions? (In practice, this means can we use G2 chat, browse, etc...)
public:
	CString			m_sFileName;	// Name of requested file
	CString			m_sFilePath;	// Path of requested file
	QWORD			m_nFileBase;	// Base offset in requested file
	QWORD			m_nFileSize;	// Size of requested file
	BOOL			m_bFilePartial;	// Partial file flag
	CString			m_sFileTags;	// File sharing tags
public:
	BOOL			m_bLive;		// Live connection tag
	DWORD			m_nRequests;	// Request count
	QWORD			m_nUploaded;	// Bytes uploaded
	DWORD			m_tContent;		// Send start timestamp
	QWORD			m_nOffset;		// Fragment offset
	QWORD			m_nLength;		// Fragment length
	QWORD			m_nPosition;	// Send position
protected:
	BOOL			m_bStopTransfer;	// Should this transfer stop? (to allow queue rotation, etc)
	DWORD			m_tRotateTime;
	DWORD			m_tAverageTime;
	int				m_nAveragePos;
	DWORD			m_nAverageRate[ULA_SLOTS];
	DWORD			m_nMaxRate;			// Maximum average speed we got
	DWORD			m_tRatingTime;		// When rating was last calculated
	
// Operations
public:
	virtual void	Remove(BOOL bMessage = TRUE);
	virtual void	Close(BOOL bMessage = FALSE);
	virtual BOOL	Promote();
	virtual BOOL	OnRename(LPCTSTR pszSource, LPCTSTR pszTarget);
public:
	virtual float	GetProgress() const;
	virtual DWORD	GetAverageSpeed();
	virtual DWORD	GetMeasuredSpeed();
	virtual DWORD	GetMaxSpeed() const;
	virtual void	SetSpeedLimit(DWORD nLimit);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnRead();
	virtual BOOL	OnWrite();
	virtual void	OnQueueKick() {};
protected:
	void		LongTermAverage(DWORD tNow);
	void		RotatingQueue(DWORD tNow);
	void		CalculateRating(DWORD tNow);
	void		ClearHashes();
	BOOL		HashesFromURN(LPCTSTR pszURN);
	void		ClearRequest();
	BOOL		RequestComplete(CLibraryFile* pFile);
	BOOL		RequestPartial(CDownload* pFile);
	void		StartSending(int nState);
	void		AllocateBaseFile();
	void		CloseFile();

};

enum UserRating
{
	urNull, urCredit, urSharing, urNew, urNotSharing
	// 1 = Uploaded more to us than we have to them (We 'owe' them upload)
	// 2 = Known Sharer (We've given them more than they have given us)
	// 3 = New user ()
	// 4 = Known user who has not uploaded (May simply not have anything we want)
};


enum UploadState
{
	upsNull, upsReady, upsConnecting,
	upsRequest, upsHeaders, upsQueued,
	upsUploading, upsResponse,
	upsBrowse, upsTigerTree, upsMetadata, upsPreview, upsPreQueue
};

#endif // !defined(AFX_UPLOADTRANSFER_H__16B2B6A4_C939_4018_81B6_B1C4460B3CD0__INCLUDED_)
