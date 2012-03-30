//
// WndSearchMonitor.h
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

class CLiveItem;


class CSearchMonitorWnd : public CPanelWnd
{
	DECLARE_SERIAL(CSearchMonitorWnd)

// Construction
public:
	CSearchMonitorWnd();
//	virtual ~CSearchMonitorWnd();


// Attributes
protected:
	CListCtrl		m_wndList;
	CImageList		m_gdiImageList;
	CLiveListSizer	m_pSizer;
	BOOL			m_bPaused;

	CList< CLiveItem* >	m_pQueue;
	CCriticalSection	m_pSection;

// Overrides
public:
	//{{AFX_VIRTUAL(CSearchMonitorWnd)
	virtual void	OnQuerySearch(const CQuerySearch* pSearch);
	virtual void	OnSkinChange();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSearchMonitorWnd)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateSearchMonitorPause(CCmdUI* pCmdUI);
	afx_msg void OnSearchMonitorPause();
	afx_msg void OnSearchMonitorClear();
	afx_msg void OnUpdateSearchMonitorSearch(CCmdUI* pCmdUI);
	afx_msg void OnSearchMonitorSearch();
	afx_msg void OnUpdateSecurityBan(CCmdUI* pCmdUI);
	afx_msg void OnSecurityBan();
	afx_msg void OnUpdateBrowseLaunch(CCmdUI* pCmdUI);
	afx_msg void OnBrowseLaunch();
	afx_msg void OnDblClkList(NMHDR* pNotifyStruct, LRESULT *pResult);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define IDC_SEARCHES	100
