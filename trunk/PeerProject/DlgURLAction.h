//
// DlgURLAction.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

class CPeerProjectURL;


class CURLActionDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CURLActionDlg)

public:
	CURLActionDlg(CPeerProjectURL* pURL);
	virtual ~CURLActionDlg();

	enum { IDD = IDD_URL_ACTION };

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
	CPeerProjectURL* m_pURL;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void PostNcDestroy();

	afx_msg void OnUrlDownload();
	afx_msg void OnUrlSearch();

	DECLARE_MESSAGE_MAP()
};
