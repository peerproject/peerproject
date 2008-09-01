//
// DlgExistingFile.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#if !defined(AFX_DLGEXISTINGFILE_H__B1B3EEF6_4A7B_4C0C_B705_BBF49EC7B1F0__INCLUDED_)
#define AFX_DLGEXISTINGFILE_H__B1B3EEF6_4A7B_4C0C_B705_BBF49EC7B1F0__INCLUDED_

#pragma once

#include "DlgSkinDialog.h"

class CLibraryFile;


class CExistingFileDlg : public CSkinDialog
{
// Construction
public:
	CExistingFileDlg(CLibraryFile* pFile, CWnd* pParent = NULL, bool bDuplicateSearch = false);
	DECLARE_DYNAMIC(CExistingFileDlg)

// Dialog Data
public:
	//{{AFX_DATA(CExistingFileDlg)
	enum { IDD = IDD_EXISTING_FILE };
	CButton	m_wndOK;
	CStatic	m_wndName;
	CString	m_sName;
	CString	m_sURN;
	int		m_nAction;
	CStatic m_wndComments;
	CStatic m_wndMessageAvailable;
	CStatic m_wndMessageDeleted;
	CStatic m_wndMessageDuplicates;
	CButton m_wndLocate;
	CButton m_wndDownload;
	CButton m_wndDontDownload;
	CString m_sComments;
	//}}AFX_DATA
	TRISTATE m_bAvailable;

// Overrides
public:
	//{{AFX_VIRTUAL(CExistingFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CExistingFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	afx_msg void OnAction0();
	afx_msg void OnAction1();
	afx_msg void OnAction2();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGEXISTINGFILE_H__B1B3EEF6_4A7B_4C0C_B705_BBF49EC7B1F0__INCLUDED_)
