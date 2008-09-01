//
// DlgFileCopy.h
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

#include "DlgSkinDialog.h"
#include "CtrlSharedFolder.h"

class CLibraryFile;


class CFileCopyDlg :
	public CSkinDialog,
	public CThreadImpl
{
public:
	CFileCopyDlg(CWnd* pParent = NULL, BOOL bMove = FALSE);

	enum { IDD = IDD_FILE_COPY };

	CString				m_sTarget;

	void		AddFile(CLibraryFile* pFile);

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

protected:
	void		StartOperation();
	void		StopOperation();
	void		OnRun();
	bool		ProcessFile(const CString& strName, const CString& strPath);
	bool		CheckTarget(const CString& pszTarget);
	bool		ProcessMove(const CString& strSource, const CString& strTarget);
	bool		ProcessCopy(const CString& pszSource, const CString& pszTarget);

	static DWORD WINAPI	CopyCallback(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber, DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile, LPVOID lpData);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

#define IDC_FOLDERS	100
