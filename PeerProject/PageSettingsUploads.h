//
// PageSettingsUploads.h
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

#include "WndSettingsPage.h"
#include "CtrlDragList.h"


class CUploadsSettingsPage : public CSettingsPage
{
// Construction
public:
	CUploadsSettingsPage();
	virtual ~CUploadsSettingsPage();

	DECLARE_DYNCREATE(CUploadsSettingsPage)

// Dialog Data
public:
	enum { IDD = IDD_SETTINGS_UPLOADS };

	CSpinButtonCtrl	m_wndMaxPerHost;
	CComboBox		m_wndAgentList;
	CButton			m_wndAgentAdd;
	CButton			m_wndAgentRemove;
	CComboBox		m_wndBandwidthLimit;
	CDragListCtrl	m_wndQueues;
	CButton			m_wndQueueDelete;
	CButton			m_wndQueueEdit;

	BOOL			m_bHubUnshare;
	BOOL			m_bSharePartials;
	BOOL			m_bSharePreviews;
	BOOL			m_bThrottleMode;
	BOOL			m_bFairUseMode;
	DWORD			m_nMaxPerHost;
	CString			m_sBandwidthLimit;


//
public:
	BOOL			m_bQueuesChanged;		//Have the queues been changed? (Rebuild hash table)
	void			UpdateQueues();

// Overrides
public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	bool IsLimited(CString& strText) const;
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeAgentList();
	afx_msg void OnEditChangeAgentList();
	afx_msg void OnAgentAdd();
	afx_msg void OnAgentRemove();
	afx_msg void OnItemChangedQueues(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnQueueNew();
	afx_msg void OnQueueEdit();
	afx_msg void OnQueueDelete();
	afx_msg void OnDblClkQueues(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnQueueDrop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
