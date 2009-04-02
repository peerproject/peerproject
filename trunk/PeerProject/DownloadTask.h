//
// DownloadTask.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#include "BTInfo.h"
#include "HttpRequest.h"
#include "PeerProjectThread.h"


class CDownload;

class CDownloadTask : public CRazaThread
{
	DECLARE_DYNAMIC(CDownloadTask)

public:
	enum dtask
	{
		dtaskAllocate,
		dtaskCopySimple,
		dtaskCopyTorrent, 
		dtaskPreviewRequest,
		dtaskCheckHash,
		dtaskMergeFile
	};

	CDownloadTask(CDownload* pDownload, dtask nTask, LPCTSTR szParam1 = NULL);
	virtual ~CDownloadTask();

	void			Abort();
	BOOL			WasAborted();
	static CString	SafeFilename(LPCTSTR pszName);
	CBuffer*		IsPreviewAnswerValid();

	dtask		m_nTask;
	BOOL		m_bSuccess;
	CString		m_sName;
	CString		m_sFilename;
	CString		m_sPath;
	CHttpRequest m_pRequest;
	DWORD		m_dwFileError;

private:
	CDownload*	m_pDownload;
	QWORD		m_nSize;
	CBTInfo		m_pTorrent;
	CString		m_sMergeFilename;

	void	Construct(CDownload* pDownload);

	POSITION	m_posTorrentFile;	// Torrent file list current position
	CEvent*		m_pEvent;

	void	RunAllocate();
	void	RunCopySimple();
	void	RunCopyTorrent();
	void	RunMerge();
	BOOL	CopyFile(HANDLE hSource, LPCTSTR pszTarget, QWORD nLength);
	void	CreatePathForFile(const CString& strBase, const CString& strPath);
	BOOL	MakeBatchTorrent();
	BOOL	CopyFileToBatch(HANDLE hSource, QWORD nOffset, QWORD nLength, LPCTSTR pszPath);

	static DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize,
		LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize,
		LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber,
		DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile,
		LPVOID lpData);

	virtual int Run();

	DECLARE_MESSAGE_MAP()
};
