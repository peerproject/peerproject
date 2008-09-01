//
// DlgDonkeyImport.h
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

#if !defined(AFX_DLGDONKEYIMPORT_H__4B7375BD_FA9B_4BEA_BC0E_2989145B240A__INCLUDED_)
#define AFX_DLGDONKEYIMPORT_H__4B7375BD_FA9B_4BEA_BC0E_2989145B240A__INCLUDED_

#pragma once

#include "DlgSkinDialog.h"
#include "EDPartImporter.h"


class CDonkeyImportDlg : public CSkinDialog
{
// Construction
public:
	CDonkeyImportDlg(CWnd* pParent = NULL);

// Dialog Data
public:
	//{{AFX_DATA(CDonkeyImportDlg)
	enum { IDD = IDD_DONKEY_IMPORT };
	CButton	m_wndClose;
	CButton	m_wndCancel;
	CButton	m_wndImport;
	CEdit	m_wndLog;
	//}}AFX_DATA

	CEDPartImporter	m_pImporter;
	CString			m_sCancel;

// Overrides
public:
	//{{AFX_VIRTUAL(CDonkeyImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDonkeyImportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnImport();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDONKEYIMPORT_H__4B7375BD_FA9B_4BEA_BC0E_2989145B240A__INCLUDED_)
