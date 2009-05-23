//
// DlgURLAction.h
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

#pragma once

#include "DlgSkinDialog.h"

class CPeerProjectURL;


class CURLActionDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CURLActionDlg)

public:
	CURLActionDlg(CWnd* pParent = NULL, CPeerProjectURL* pURL = NULL, BOOL bMultiple = FALSE);
	virtual ~CURLActionDlg();

	enum { IDD = IDD_URL_ACTION };

	void	AddURL(CPeerProjectURL* pURL);

protected:
	CStatic	m_wndMessage4;
	CStatic	m_wndMessage3;
	CButton	m_wndNewWindow;
	CButton	m_wndCancel;
	CStatic	m_wndMessage2;
	CStatic	m_wndMessage1;
	CButton	m_wndSearch;
	CButton	m_wndDownload;
	CString	m_sNameTitle;
	CString	m_sNameValue;
	CString	m_sHashTitle;
	CString	m_sHashValue;
	BOOL	m_bNewWindow;
	BOOL	m_bAlwaysOpen;
	CList< CPeerProjectURL* >	m_pURLs;
	BOOL	m_bMultiple;

	void	Update();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnUrlDownload();
	afx_msg void OnUrlSearch();

	DECLARE_MESSAGE_MAP()
};
