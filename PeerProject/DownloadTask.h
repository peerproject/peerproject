//
// DownloadTask.h
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

#include "PeerProjectThread.h"
#include "FileFragments.hpp"

class CDownload;
class CHttpRequest;


enum dtask
{
	dtaskNone = 0,		// No task
	dtaskAllocate,		// Allocating...
	dtaskCopy,			// Moving...
	dtaskMergeFile,		// Merging...
	dtaskPreviewRequest	// Previewing...
};


class CDownloadTask : public CAppThread
{
	DECLARE_DYNAMIC(CDownloadTask)

public:
	static void			Copy(CDownload* pDownload);
	static void			PreviewRequest(CDownload* pDownload, LPCTSTR szURL);
	static void			MergeFile(CDownload* pDownload, LPCTSTR szPath,
						BOOL bValidation = TRUE, const Fragments::List* pGaps = NULL);

	void				Abort();
	bool				WasAborted() const;
	bool				HasSucceeded() const;
	DWORD				GetFileError() const;
	dtask				GetTaskType() const;
	const CHttpRequest*	GetRequest() const;
	CBuffer*			IsPreviewAnswerValid() const;

protected:
	CDownloadTask(CDownload* pDownload, dtask nTask);
	virtual ~CDownloadTask();

	dtask				m_nTask;
	CHttpRequest*		m_pRequest;
	bool				m_bSuccess;
	CString				m_sFilename;
	CString				m_sDestination;
	DWORD				m_nFileError;
	CDownload*			m_pDownload;
	QWORD				m_nSize;
	CString				m_sMergeFilename;	// Source filename
	Fragments::List		m_oMergeGaps;		// Missed ranges in source file
	BOOL				m_bMergeValidation;	// Run validation after merging
	POSITION			m_posTorrentFile;	// Torrent file list current position
	CEvent*				m_pEvent;

	static DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize,
		LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize,
		LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber,
		DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile,
		LPVOID lpData);

	void				Construct(CDownload* pDownload);
	void				RunCopy();
	void				RunMerge();
//	void				RunAllocate();
	void				RunPreviewRequest();
	void				CreatePathForFile(const CString& strBase, const CString& strPath);
	BOOL				CopyFile(HANDLE hSource, LPCTSTR pszTarget, QWORD nLength);
	BOOL				CopyFileToBatch(HANDLE hSource, QWORD nOffset, QWORD nLength, LPCTSTR pszPath);
	BOOL				MakeBatchTorrent();

	virtual int			Run();

	DECLARE_MESSAGE_MAP()
};
