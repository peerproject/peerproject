//
// WndDiscovery.h
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

#include "WndPanel.h"
#include "LiveList.h"

#define IDC_SERVICES	100

class CDiscoveryService;


class CDiscoveryWnd : public CPanelWnd
{
	DECLARE_SERIAL(CDiscoveryWnd)

// Construction
public:
	CDiscoveryWnd();
	//virtual ~CDiscoveryWnd();

// Attributes
public:
	CLiveListCtrl	m_wndList;
	CLiveListSizer	m_pSizer;
	CCoolBarCtrl	m_wndToolBar;
	CImageList		m_gdiImageList;
	BOOL			m_bShowGnutella;
	BOOL			m_bShowWebCache;
	BOOL			m_bShowServerList;
	BOOL			m_bShowBlocked;

// Operations
public:
	CDiscoveryService* GetItem(int nItem);
	void			Update();

// Overrides
protected:
	//{{AFX_VIRTUAL(CDiscoveryWnd)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnSkinChange();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDiscoveryWnd)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDblClkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSortList(NMHDR* pNotifyStruct, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDiscoveryAdd();
	afx_msg void OnUpdateDiscoveryRemove(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryRemove();
	afx_msg void OnUpdateDiscoveryEdit(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryEdit();
	afx_msg void OnUpdateDiscoveryGnutella(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryGnutella();
	afx_msg void OnUpdateDiscoveryWebcache(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryWebcache();
	afx_msg void OnUpdateDiscoveryServerList(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryServerList();
	afx_msg void OnUpdateDiscoveryBlocked(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryBlocked();
	afx_msg void OnUpdateDiscoveryQuery(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryQuery();
	afx_msg void OnUpdateDiscoveryAdvertise(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryAdvertise();
	afx_msg void OnUpdateDiscoveryBrowse(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryBrowse();
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
