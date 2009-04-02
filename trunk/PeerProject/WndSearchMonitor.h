//
// WndSearchMonitor.h
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

#if !defined(AFX_WNDSEARCHMONITOR_H__0485D2F4_A299_45B8_80D0_6FCADDAA2E44__INCLUDED_)
#define AFX_WNDSEARCHMONITOR_H__0485D2F4_A299_45B8_80D0_6FCADDAA2E44__INCLUDED_

#pragma once

#include "WndPanel.h"

class CLiveItem;

class CSearchMonitorWnd : public CPanelWnd
{
// Construction
public:
	CSearchMonitorWnd();
	virtual ~CSearchMonitorWnd();

	DECLARE_SERIAL(CSearchMonitorWnd)

// Attributes
protected:
	CListCtrl		m_wndList;
	CImageList		m_gdiImageList;
	CLiveListSizer	m_pSizer;
	BOOL			m_bPaused;
protected:
	CList< CLiveItem* >			m_pQueue;
	CCriticalSection	m_pSection;

// Operations
public:
	virtual void	OnQuerySearch(CQuerySearch* pSearch);
	virtual void	OnSkinChange();

// Overrides
public:
	//{{AFX_VIRTUAL(CSearchMonitorWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSearchMonitorWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#define IDC_SEARCHES	100

#endif // !defined(AFX_WNDSEARCHMONITOR_H__0485D2F4_A299_45B8_80D0_6FCADDAA2E44__INCLUDED_)
