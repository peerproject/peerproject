//
// PageTorrentFiles.h
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

#include "PagePropertyAdv.h"
#include "ComboListCtrl.h"


class CTorrentFilesPage : public CPropertyPageAdv
{
public:
	CTorrentFilesPage();
	virtual ~CTorrentFilesPage();

	DECLARE_DYNCREATE(CTorrentFilesPage)

	enum { IDD = IDD_TORRENT_FILES };

protected:
	CComboListCtrl	m_wndFiles;

	// Update interface
	void Update();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnCheckbox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTorrentFiles(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};
