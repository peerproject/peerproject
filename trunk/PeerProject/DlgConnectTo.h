//
// DlgConnectTo.h
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

#if !defined(AFX_DLGCONNECTTO_H__08D02B14_977A_4C9C_AEAE_8FBBE0E868B8__INCLUDED_)
#define AFX_DLGCONNECTTO_H__08D02B14_977A_4C9C_AEAE_8FBBE0E868B8__INCLUDED_

#pragma once

#include "DlgSkinDialog.h"


class CConnectToDlg : public CSkinDialog
{
public:
	CConnectToDlg(CWnd* pParent = NULL, BOOL bBrowseHost = FALSE);

	enum { IDD = IDD_CONNECT_TO };
	CButton		m_wndAdvanced;
	CComboBox	m_wndProtocol;
	CButton		m_wndUltrapeer;
	CButton		m_wndPrompt;
	CEdit		m_wndPort;
	CComboBox	m_wndHost;
	CString		m_sHost;
	BOOL		m_bNoUltraPeer;
	int			m_nPort;
	int			m_nProtocol;

protected:
	CImageList	m_pImages;
	BOOL		m_bBrowseHost;

	void		LoadItem(int nItem);
	BOOL		UpdateItems();
	void		SaveItems();

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnCbnSelchangeConnectHost();
	afx_msg void OnCbnSelchangeConnectProtocol();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGCONNECTTO_H__08D02B14_977A_4C9C_AEAE_8FBBE0E868B8__INCLUDED_)
