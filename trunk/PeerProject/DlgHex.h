//
// DlgHex.h
//
// This file is part of PeerProject (peerproject.org) © 2014
// Portions copyright Shareaza Development Team, 2014.
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
// Debug-only

#pragma once

#include "DlgSkinDialog.h"
#include "Buffer.h"


class CHexDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CHexDlg)

public:
	CHexDlg(CWnd* pParent = NULL);
	virtual ~CHexDlg();

	enum { IDD = IDD_DEBUG_HEX };

	CBuffer* GetData() { return &m_pBuffer; }

protected:
	CString m_sHex;
	CBuffer m_pBuffer;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
