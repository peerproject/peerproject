//
// DlgConnectTo.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "DlgSkinDialog.h"


class CConnectToDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CConnectToDlg)

public:
	enum { IDD = IDD_CONNECT_TO };
	enum Type { Connect = 0, Browse = 1, Chat = 2 };

	CConnectToDlg(CWnd* pParent = NULL, Type nType = Connect);

	CString		m_sHost;
	CComboBox	m_wndHost;
	CComboBox	m_wndProtocol;
//	CButton		m_wndAdvanced;
	CButton		m_wndUltrapeer;
	CButton		m_wndPrompt;
	CEdit		m_wndPort;
	int			m_nPort;
	BOOL		m_bNoUltraPeer;
	PROTOCOLID	m_nProtocol;

protected:
	CImageList	m_gdiProtocols;
	Type		m_nType;

	void		LoadItem(int nItem);
	BOOL		UpdateItems();
	void		SaveItems();
	BOOL		UpdateData(BOOL bSaveAndValidate = TRUE);

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
