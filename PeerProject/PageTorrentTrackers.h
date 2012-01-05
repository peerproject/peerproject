//
// PageTorrentTrackers.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2006.
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
#include "BTTrackerRequest.h"

// Obsolete Scrape Thread
//#include "ThreadImpl.h"
//
//class CDownload;
//class CBENode;


class CTorrentTrackersPage : public CPropertyPageAdv, public CTrackerEvent	/*public CThreadImpl (Obsolete)*/
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
	CBTTrackerRequest*	m_pRequest;
	DWORD			m_nComplete;
	DWORD			m_nIncomplete;

// Obsolete Scrape Thread
//	void			OnRun();
//	BOOL			OnTree(CDownload* pDownload, CBENode* pNode);

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual BOOL	OnInitDialog();
	virtual BOOL	OnApply();
	virtual void	OnTrackerEvent(bool bSuccess, LPCTSTR pszReason, LPCTSTR pszTip, CBTTrackerRequest* pEvent);

	afx_msg void	OnDestroy();
	afx_msg void	OnTorrentRefresh();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg void	OnEnChangeTorrentTracker();
	afx_msg void	OnNMClickTorrentTrackers(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnCbnSelchangeTorrentTrackermode();
	afx_msg void	OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};
