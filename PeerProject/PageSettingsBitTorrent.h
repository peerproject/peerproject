//
// PageSettingsBitTorrent.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#include "WndSettingsPage.h"
#include "CtrlIconButton.h"


class CBitTorrentSettingsPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CBitTorrentSettingsPage)

public:
	CBitTorrentSettingsPage();
	virtual ~CBitTorrentSettingsPage();

	enum { IDD = IDD_SETTINGS_BITTORRENT };

public:
	virtual void OnOK();
	virtual BOOL OnSetActive();

protected:
	BOOL			m_bEnableDHT;
	BOOL			m_bEndGame;
	BOOL			m_bPrefBTSources;
	int				m_nLinks;
	CSpinButtonCtrl	m_wndLinksSpin;
	CSpinButtonCtrl	m_wndDownloadsSpin;
	int				m_nDownloads;
	BOOL			m_bAutoClear;
	CEdit			m_wndClearPercentage;
	CSpinButtonCtrl	m_wndClearPercentageSpin;
	int				m_nClearPercentage;
	CIconButtonCtrl	m_wndTorrentPath;
	CString			m_sTorrentPath;
	CString			m_sTracker;
	CIconButtonCtrl	m_wndMakerPath;
	CString			m_sMakerPath;
	CEditPath		m_wndTorrentFolder;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnTorrentsAutoClear();
	afx_msg void OnTorrentsBrowse();
	afx_msg void OnMakerBrowse();

	DECLARE_MESSAGE_MAP()
};
