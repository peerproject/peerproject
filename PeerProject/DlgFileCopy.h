//
// DlgFileCopy.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "DlgSkinDialog.h"
#include "CtrlSharedFolder.h"
#include "ThreadImpl.h"

class CLibraryFile;


class CFileCopyDlg :
	public CSkinDialog,
	public CThreadImpl
{
public:
	CFileCopyDlg(CWnd* pParent = NULL, BOOL bMove = FALSE);

	enum { IDD = IDD_FILE_COPY };

public:
	CString				m_sTarget;

protected:
	CStatic				m_wndMove;
	CStatic				m_wndCopy;
	CStatic				m_wndFileName;
	CProgressCtrl		m_wndFileProg;
	CButton				m_wndCancel;
	CButton				m_wndOK;
	CProgressCtrl		m_wndProgress;
	CStatic				m_wndPlaceholder;
	BOOL				m_bMove;
	CList< DWORD >		m_pFiles;
	CLibraryFolderCtrl	m_wndTree;
	DWORD				m_nCookie;
	BOOL				m_bCancel;
	int					m_nFileProg;

public:
	void		AddFile(CLibraryFile* pFile);

protected:
	void		StartOperation();
	void		StopOperation();
	void		OnRun();
	bool		ProcessFile(const CString& strName, const CString& strPath);
	bool		CheckTarget(const CString& pszTarget);
	bool		ProcessMove(const CString& strSource, const CString& strTarget);
	bool		ProcessCopy(const CString& pszSource, const CString& pszTarget);

	static DWORD WINAPI	CopyCallback(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber, DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile, LPVOID lpData);

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

#define IDC_FOLDERS	100
