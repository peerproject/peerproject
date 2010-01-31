//
// DlgNewSearch.h
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
#include "CtrlSchema.h"
#include "CtrlSchemaCombo.h"

class CQuerySearch;


class CNewSearchDlg : public CSkinDialog
{
public:
	CNewSearchDlg(CWnd* pParent = NULL, CQuerySearch* pSearch = NULL,
		BOOL bLocal = FALSE, BOOL bAgain = FALSE);

	enum { IDD = IDD_NEW_SEARCH };

	CQuerySearchPtr GetSearch() const
	{
		return m_pSearch;
	}

protected:
	CButton			m_wndOK;
	CButton			m_wndCancel;
	CSchemaCombo	m_wndSchemas;
	CComboBox		m_wndSearch;

// Attributes
private:
	CSchemaCtrl		m_wndSchema;
	BOOL			m_bLocal;
	BOOL			m_bAgain;
	CQuerySearchPtr m_pSearch;

// Implementation
protected:
	//{{AFX_MSG(CNewSearchDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSelChangeSchemas();
	afx_msg void OnCloseUpSchemas();
	afx_msg void OnChangeSearch();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define IDC_METADATA		100
