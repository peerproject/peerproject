//
// DlgFolderScan.h
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


class CFolderScanDlg : public CSkinDialog
{
// Construction
public:
	CFolderScanDlg(CWnd* pParent = NULL);
	virtual ~CFolderScanDlg();

// Dialog Data
public:
	//{{AFX_DATA(CFolderScanDlg)
	enum { IDD = IDD_FOLDER_SCAN };
	CStatic	m_wndVolume;
	CStatic	m_wndFiles;
	CStatic	m_wndFile;
	//}}AFX_DATA

// Operations
public:
	static void Update(LPCTSTR pszName, DWORD nVolume);//, BOOL bLock);
	void	InstanceUpdate(LPCTSTR pszName, DWORD nVolume);

// Data
protected:
	static	CFolderScanDlg*	m_pDialog;
	DWORD	m_nCookie;
	DWORD	m_nFiles;
	DWORD	m_nVolume;
	DWORD	m_tLastUpdate;
	BOOL	m_bActive;

// Overrides
public:
	//{{AFX_VIRTUAL(CFolderScanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFolderScanDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};