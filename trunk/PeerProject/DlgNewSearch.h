//
// DlgNewSearch.h
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

#if !defined(AFX_DLGNEWSEARCH_H__6120C263_1A9E_4C29_8D73_7B9FF200AF5C__INCLUDED_)
#define AFX_DLGNEWSEARCH_H__6120C263_1A9E_4C29_8D73_7B9FF200AF5C__INCLUDED_

#pragma once

#include "DlgSkinDialog.h"
#include "CtrlSchema.h"
#include "CtrlSchemaCombo.h"

class CQuerySearch;


class CNewSearchDlg : public CSkinDialog
{
// Construction
public:
	CNewSearchDlg(CWnd* pParent = NULL,
			auto_ptr< CQuerySearch > pSearch = auto_ptr< CQuerySearch >(),
			BOOL bLocal = FALSE, BOOL bAgain = FALSE);

// Dialog Data
public:
	//{{AFX_DATA(CNewSearchDlg)
	enum { IDD = IDD_NEW_SEARCH };
	CButton	m_wndCancel;
	CButton	m_wndOK;
	CSchemaCombo	m_wndSchemas;
	CComboBox		m_wndSearch;
	//}}AFX_DATA

// Attributes
private:
	CSchemaCtrl		m_wndSchema;
	BOOL			m_bLocal;
	BOOL			m_bAgain;
	auto_ptr< CQuerySearch > m_pSearch;

// Operations
public:
	auto_ptr< CQuerySearch > GetSearch() { return m_pSearch; }

// Overrides
public:
	//{{AFX_VIRTUAL(CNewSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNewSearchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnOK();
	afx_msg void OnSelChangeSchemas();
	afx_msg void OnCloseUpSchemas();
	afx_msg void OnChangeSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#define IDC_METADATA		100

#endif // !defined(AFX_DLGNEWSEARCH_H__6120C263_1A9E_4C29_8D73_7B9FF200AF5C__INCLUDED_)
