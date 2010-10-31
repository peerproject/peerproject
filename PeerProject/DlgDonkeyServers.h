//
// DlgDonkeyServers.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "HttpRequest.h"
#include "DlgSkinDialog.h"


class CDonkeyServersDlg : public CSkinDialog
{
public:
	CDonkeyServersDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_DONKEY_SERVERS };

protected:
	CEdit			m_wndURL;
	CButton			m_wndOK;
	CProgressCtrl	m_wndProgress;
	CHttpRequest	m_pRequest;
	CString			m_sURL;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnChangeURL();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};
