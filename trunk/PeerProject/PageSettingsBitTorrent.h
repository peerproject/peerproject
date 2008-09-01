//
// PageSettingsBitTorrent.h
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

#if !defined(AFX_PAGESETTINGSBITTORRENT_H__7A9C2316_0CF6_4251_9BD2_C56B9DA5AD89__INCLUDED_)
#define AFX_PAGESETTINGSBITTORRENT_H__7A9C2316_0CF6_4251_9BD2_C56B9DA5AD89__INCLUDED_

#pragma once

#include "WndSettingsPage.h"
#include "CtrlIconButton.h"


class CBitTorrentSettingsPage : public CSettingsPage
{
// Construction
public:
	CBitTorrentSettingsPage();
	virtual ~CBitTorrentSettingsPage();

	DECLARE_DYNCREATE(CBitTorrentSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CBitTorrentSettingsPage)
	enum { IDD = IDD_SETTINGS_BITTORRENT };
	BOOL	m_bTorrentInterface;
	BOOL	m_bEndGame;
	CSpinButtonCtrl	m_wndLinksSpin;
	int		m_nLinks;
	CSpinButtonCtrl	m_wndDownloadsSpin;
	int		m_nDownloads;
	BOOL	m_bAutoClear;
	CEdit	m_wndClearPercentage;
	CSpinButtonCtrl	m_wndClearPercentageSpin;
	int		m_nClearPercentage;
	BOOL	m_bPrefBTSources;
	CIconButtonCtrl	m_wndTorrentPath;
	CString	m_sTorrentPath;
	CString	m_sTracker;
	CIconButtonCtrl	m_wndMakerPath;
	CString	m_sMakerPath;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CBitTorrentSettingsPage)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLibrarySettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTorrentsAutoClear();
	afx_msg void OnTorrentsBrowse();
	afx_msg void OnMakerBrowse();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGESETTINGSBITTORRENT_H__7A9C2316_0CF6_4251_9BD2_C56B9DA5AD89__INCLUDED_)
