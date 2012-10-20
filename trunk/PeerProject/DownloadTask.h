//
// DownloadTask.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
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

protected:
	CDownloadTask(CDownload* pDownload, dtask nTask);
	virtual ~CDownloadTask();

public:
	static void			Copy(CDownload* pDownload);
	static void			PreviewRequest(CDownload* pDownload, LPCTSTR szURL);
	static void			MergeFile(CDownload* pDownload, CList< CString >* pFiles, BOOL bValidation = TRUE, const Fragments::List* pGaps = NULL);
	static void			MergeFile(CDownload* pDownload, LPCTSTR szPath, BOOL bValidation = TRUE, const Fragments::List* pGaps = NULL);

	void				Abort();
	bool				WasAborted() const;
	bool				HasSucceeded() const;
	DWORD				GetFileError() const;
	dtask				GetTaskType() const;
	CString				GetRequest() const;
	float				GetProgress() const;	// Get progress of current operation (0-100%)
	CBuffer*			IsPreviewAnswerValid() const;

protected:
	dtask				m_nTask;
	CHttpRequest*		m_pRequest;
//	CString				m_sURL;				// Request URL
	bool				m_bSuccess;
	CString				m_sFilename;
	CString				m_sDestination;
	DWORD				m_nFileError;
	QWORD				m_nSize;
	CString				m_sURL;
	CDownload*			m_pDownload;
	CList< CString >	m_oMergeFiles;		// Source filename(s)
	Fragments::List		m_oMergeGaps;		// Missed ranges in source file
	BOOL				m_bMergeValidation;	// Run validation after merging
	POSITION			m_posTorrentFile;	// Torrent file list current position
	float				m_fProgress;		// Progress of current operation (0-100%)
	CEvent*				m_pEvent;

	static DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize,
		LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize,
		LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber,
		DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile,
		LPVOID lpData);

	void				Construct(CDownload* pDownload);
	void				RunPreviewRequest();
//	void				RunAllocate();
	void				RunCopy();
	void				RunMerge();
	void				RunMergeFile(CDownload* pDownload, LPCTSTR szFilename, BOOL bMergeValidation, const Fragments::List& oMissedGaps, float fProgress = 100.0f);
//	void				CreatePathForFile(const CString& strBase, const CString& strPath);
//	BOOL				CopyFile(HANDLE hSource, LPCTSTR pszTarget, QWORD nLength);
	BOOL				CopyFileToBatch(HANDLE hSource, QWORD nOffset, QWORD nLength, LPCTSTR pszPath);
	BOOL				MakeBatchTorrent();

	virtual int			Run();

	DECLARE_MESSAGE_MAP()
};
