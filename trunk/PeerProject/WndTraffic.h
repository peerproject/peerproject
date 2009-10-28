//
// WndTraffic.h
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

#include "WndChild.h"

class CGraphBase;


class CTrafficWnd : public CChildWnd
{
// Construction
public:
	CTrafficWnd(DWORD nUnique = 0);
	virtual ~CTrafficWnd();

	DECLARE_SERIAL(CTrafficWnd)

// Attributes
public:
	DWORD		m_nUnique;
	CString		m_sName;
	CGraphBase*	m_pGraph;

// Operations
protected:
	void	FindFreeUnique();
	BOOL	Serialize(BOOL bSave);
	void	SetUpdateRate();
	void	UpdateCaption();

// Overrides
public:
	//{{AFX_VIRTUAL(CTrafficWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTrafficWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateTrafficGrid(CCmdUI* pCmdUI);
	afx_msg void OnTrafficGrid();
	afx_msg void OnUpdateTrafficAxis(CCmdUI* pCmdUI);
	afx_msg void OnTrafficAxis();
	afx_msg void OnUpdateTrafficLegend(CCmdUI* pCmdUI);
	afx_msg void OnTrafficLegend();
	afx_msg void OnTrafficSetup();
	afx_msg void OnTrafficClear();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTrafficWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
