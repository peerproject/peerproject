//
// DlgHelp.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "RichViewCtrl.h"
#include "RichDocument.h"


class CHelpDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CHelpDlg)

public:
	CHelpDlg(LPCTSTR pszName, CWnd* pParent = NULL);

	enum { IDD = IDD_HELP };

	static BOOL Show(LPCTSTR pszName, CWnd* pParent = NULL);

// Attributes
protected:
	CRichViewCtrl	m_wndView;
	CRichDocument	m_pDocument;
	CString			m_sDocument;

// Overrides
protected:
	//{{AFX_VIRTUAL(CHelpDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHelpDlg)
	afx_msg void OnClickView(NMHDR* pNotify, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define IDC_HELP_VIEW	100
