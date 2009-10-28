//
// DlgFileProperties.h
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
#include "CtrlSchema.h"
#include "CtrlSchemaCombo.h"

class CFilePropertiesDlg : public CSkinDialog
{
public:
	CFilePropertiesDlg(CWnd* pParent = NULL, DWORD nIndex = 0);

	DECLARE_DYNAMIC(CFilePropertiesDlg)

	CStatic	m_wndHash;
	CStatic	m_wndIcon;
	CButton	m_wndCancel;
	CButton	m_wndOK;
	CSchemaCombo	m_wndSchemas;
	CString	m_sName;
	CString	m_sSize;
	CString	m_sType;
	CString	m_sPath;
	CString	m_sIndex;
	CString	m_sSHA1;
	CString	m_sTiger;

	void	Update();

protected:
	CSchemaCtrl	m_wndSchema;
	DWORD		m_nIndex;
	BOOL		m_bHexHash;
	int			m_nWidth;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSelChangeSchemas();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnCloseUpSchemas();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

#define IDC_METADATA		100
