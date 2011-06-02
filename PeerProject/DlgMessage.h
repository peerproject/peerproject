//
// DlgMessage.h
//
// This file is part of PeerProject (peerproject.org) � 2009-2010
// Portions copyright Shareaza Development Team, 2009.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
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

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	DECLARE_MESSAGE_MAP()
};