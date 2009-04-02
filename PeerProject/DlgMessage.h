//
// DlgMessage.h
//
// This file is part of PeerProject (peerproject.org) © 2009
// Portions Copyright Shareaza Development Team, 2009.
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

class CSkinDialog;


// CMessageDlg dialog

class CMessageDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_MESSAGE };

	DWORD	m_nType;		// Message box type
	CString	m_sText;		// Message box text
	DWORD*	m_pnDefault;	// Message box variable

	virtual INT_PTR DoModal();

protected:
	CStatic m_Icon;
	CStatic m_pText;
	CStatic m_pSplit;
	CButton m_pDefault;
	CButton m_pButton1;
	CButton m_pButton2;
	CButton m_pButton3;
	BOOL	m_bRemember;	// Remember my selection next time

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	DECLARE_MESSAGE_MAP()
};
