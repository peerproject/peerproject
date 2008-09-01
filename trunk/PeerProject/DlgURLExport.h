//
// DlgURLExport.h
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

#if !defined(AFX_DLGURLEXPORT_H__1B69A614_F171_4EE4_925E_FEDBBAF13A0D__INCLUDED_)
#define AFX_DLGURLEXPORT_H__1B69A614_F171_4EE4_925E_FEDBBAF13A0D__INCLUDED_

#pragma once

#include "DlgSkinDialog.h"
#include "PeerProjectFile.h"

class CURLExportDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CURLExportDlg)

public:
	CURLExportDlg(CWnd* pParent = NULL);

public:
	enum { IDD = IDD_URL_EXPORT };
	CButton			m_wndSave;
	CButton			m_wndCopy;
	CProgressCtrl	m_wndProgress;
	CComboBox		m_wndToken;
	CComboBox		m_wndPreset;
	CEdit			m_wndFormat;
	CStatic			m_wndMessage;
	CString			m_sFormat;

	void		Add(const CPeerProjectFile* pFile);

protected:
	CList< const CPeerProjectFile* >	m_pFiles;

	void		MakeURL(const CPeerProjectFile* pFile, CString& strLine);

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseUpUrlToken();
	afx_msg void OnSelChangeUrlPreset();
	afx_msg void OnKillFocusUrlPreset();
	afx_msg void OnSave();
	afx_msg void OnCopy();

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_DLGURLEXPORT_H__1B69A614_F171_4EE4_925E_FEDBBAF13A0D__INCLUDED_)
