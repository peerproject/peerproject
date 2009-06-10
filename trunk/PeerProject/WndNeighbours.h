//
// WndNeighbours.h
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

#if !defined(AFX_WNDNEIGHBOURS_H__C3F579F1_50F6_40F8_9A98_2A14ED05A9CC__INCLUDED_)
#define AFX_WNDNEIGHBOURS_H__C3F579F1_50F6_40F8_9A98_2A14ED05A9CC__INCLUDED_

#pragma once

#include "WndPanel.h"
#include "CtrlTipList.h"
#include "CtrlNeighbourTip.h"

class CNeighbour;


class CNeighboursWnd : public CPanelWnd
{
// Construction
public:
	CNeighboursWnd();

	DECLARE_SERIAL(CNeighboursWnd)

// Attributes
protected:
	CCoolBarCtrl		m_wndToolBar;
	CTipListCtrl		m_wndList;
	CNeighbourTipCtrl	m_wndTip;
	CImageList			m_gdiImageList;
	CLiveListSizer		m_pSizer;
	DWORD				m_tLastUpdate;
private:
	int					m_nProtocolRev; // Reversed index of start image in RTL mode

// Operations
public:
	void			Update();
	CNeighbour*		GetItem(int nItem);
	void			OpenPacketWnd(BOOL bIncoming, BOOL bOutgoing);
	void			DrawEmptyMessage(CDC* pDC);
	virtual void	OnSkinChange();

// Overrides
public:
	//{{AFX_VIRTUAL(CNeighboursWnd)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNeighboursWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSortList(NMHDR* pNotifyStruct, LRESULT *pResult);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateNeighboursDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnNeighboursDisconnect();
	afx_msg void OnUpdateNeighboursViewAll(CCmdUI* pCmdUI);
	afx_msg void OnNeighboursViewAll();
	afx_msg void OnUpdateNeighboursViewIncoming(CCmdUI* pCmdUI);
	afx_msg void OnNeighboursViewIncoming();
	afx_msg void OnUpdateNeighboursViewOutgoing(CCmdUI* pCmdUI);
	afx_msg void OnNeighboursViewOutgoing();
	afx_msg void OnDestroy();
	afx_msg void OnUpdateNeighboursChat(CCmdUI* pCmdUI);
	afx_msg void OnNeighboursChat();
	afx_msg void OnUpdateSecurityBan(CCmdUI* pCmdUI);
	afx_msg void OnSecurityBan();
	afx_msg void OnUpdateBrowseLaunch(CCmdUI* pCmdUI);
	afx_msg void OnBrowseLaunch();
	afx_msg void OnUpdateNeighboursCopy(CCmdUI* pCmdUI);
	afx_msg void OnNeighboursCopy();
	afx_msg void OnNeighboursSettings();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define IDC_NEIGHBOURS	100

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_WNDNEIGHBOURS_H__C3F579F1_50F6_40F8_9A98_2A14ED05A9CC__INCLUDED_)