//
// DownloadTask.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "HttpRequest.h"
#include "PeerProjectThread.h"


class CDownload;

class CDownloadTask : public CRazaThread
{
public:
	enum dtask
	{
//		dtaskAllocate,
		dtaskCopy,
		dtaskPreviewRequest,
		dtaskCheckHash,
		dtaskMergeFile
	};

// Construction
public:
	CDownloadTask(CDownload* pDownload, dtask nTask, LPCTSTR szParam1 = NULL);
	virtual ~CDownloadTask();

	DECLARE_DYNAMIC(CDownloadTask)

// Attributes
public:
	dtask			m_nTask;
	CHttpRequest	m_pRequest;
private:
	bool			m_bSuccess;
	CString			m_sFilename;
	CString			m_sDestination;
	DWORD			m_nFileError;
	CDownload*		m_pDownload;
	QWORD			m_nSize;
	CString			m_sMergeFilename;
	POSITION		m_posTorrentFile;	// Torrent file list current position
	CEvent*			m_pEvent;

// Statics
public:
	static CString	SafeFilename(LPCTSTR pszName);
	static DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize,
		LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize,
		LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber,
		DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile,
		LPVOID lpData);

// Operations
public:
	bool		HasSucceeded() const;
	void		Abort();
	bool		WasAborted() const;
	DWORD		GetFileError() const;
	CBuffer*	IsPreviewAnswerValid();
private:
	void		Construct(CDownload* pDownload);
//	void		RunAllocate();
	void		RunCopy();
	void		RunPreviewRequest();
	void		RunMerge();
	BOOL		CopyFile(HANDLE hSource, LPCTSTR pszTarget, QWORD nLength);
	void		CreatePathForFile(const CString& strBase, const CString& strPath);
	BOOL		MakeBatchTorrent();
	BOOL		CopyFileToBatch(HANDLE hSource, QWORD nOffset, QWORD nLength, LPCTSTR pszPath);

// Overrides
protected:
	virtual int Run();

	DECLARE_MESSAGE_MAP()
};
