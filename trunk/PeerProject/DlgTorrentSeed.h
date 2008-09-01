//
// DlgTorrentSeed.h
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

#pragma once

#include "DlgSkinDialog.h"
#include "BTInfo.h"


class CTorrentSeedDlg :
	public CSkinDialog,
	public CThreadImpl
{
// Construction
public:
	CTorrentSeedDlg(LPCTSTR pszTorrent, BOOL bForceSeed = FALSE, CWnd* pParent = NULL);
	virtual ~CTorrentSeedDlg();

	DECLARE_DYNAMIC(CTorrentSeedDlg)
	enum { IDD = IDD_TORRENT_SEED };

// Dialog Data
protected:
	CProgressCtrl	m_wndProgress;
	CButton			m_wndDownload;
	CButton			m_wndSeed;
protected:
	BOOL			m_bCancel;
	CString			m_sTorrent;
	BOOL			m_bForceSeed;
	CString			m_sTarget;
	CBTInfo			m_pInfo;
	DWORD			m_nBlockNumber;
	DWORD			m_nBlockLength;
protected:
	CString			m_sMessage;
	QWORD			m_nVolume;
	QWORD			m_nTotal;
	int				m_nScaled;
	int				m_nOldScaled;

// Implementation
protected:
	void		OnRun();
	void		RunSingleFile();
	void		RunMultiFile();
	BOOL		CheckFiles();
	HANDLE		CreateTarget();
	BOOL		CreateDownload();

// Message Map
protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnDownload();
	afx_msg void OnSeed();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
