//
// PageTorrentFiles.h
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

	void Update();	// Refresh interface

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnCheckbox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSortColumn(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnNMDblclkTorrentFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};
