//
// PageTorrentTrackers.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
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

#include "ThreadImpl.h"
#include "PagePropertyAdv.h"
#include "HttpRequest.h"

class CTorrentTrackersPage :
	public CPropertyPageAdv,
	public CThreadImpl
{
public:
	CTorrentTrackersPage();
	virtual ~CTorrentTrackersPage();

	DECLARE_DYNCREATE(CTorrentTrackersPage)

	enum { IDD = IDD_TORRENT_TRACKERS };

protected:
	CString			m_sTracker;
	CButton			m_wndRefresh;
	CEdit			m_wndComplete;
	CEdit			m_wndIncomplete;
	CComboBox		m_wndTrackerMode;
	CListCtrl		m_wndTrackers;
	CHttpRequest	m_pRequest;
	int				m_nComplete;
	int				m_nIncomplete;
	CDownload*		m_pDownload;

	void			OnRun();
	BOOL			OnTree(CBENode* pNode);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL	OnInitDialog();
	virtual BOOL OnApply();

	afx_msg void	OnTorrentRefresh();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg void	OnDestroy();
	afx_msg void OnEnChangeTorrentTracker();
	afx_msg void OnNMClickTorrentTrackers(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeTorrentTrackermode();

	DECLARE_MESSAGE_MAP()
};
