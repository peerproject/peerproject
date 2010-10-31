//
// DlgTorrentSeed.h
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

#include "DlgSkinDialog.h"
#include "BTInfo.h"


class CTorrentSeedDlg :
	public CSkinDialog,
	public CThreadImpl
{
	DECLARE_DYNAMIC(CTorrentSeedDlg)

public:
	CTorrentSeedDlg(LPCTSTR pszTorrent, BOOL bForceSeed = FALSE, CWnd* pParent = NULL);
	BOOL			LoadTorrent(CString strPath);

	enum { IDD = IDD_TORRENT_SEED };

protected:
	CProgressCtrl	m_wndProgress;
	CButton			m_wndDownload;
	CButton			m_wndSeed;
	BOOL			m_bCancel;
	CString			m_sTorrent;
	BOOL			m_bForceSeed;
	CBTInfo			m_pInfo;
	CString			m_sMessage;
	int				m_nScaled;
	int				m_nOldScaled;

	void		OnRun();
	BOOL		CreateDownload();

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg void OnDownload();
	afx_msg void OnSeed();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};
