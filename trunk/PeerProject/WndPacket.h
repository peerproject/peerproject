//
// WndPacket.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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
#include "G1Packet.h"
#include "G2Packet.h"
#include "EDPacket.h"
#include "DCPacket.h"
#include "BTPacket.h"

class CLiveItem;
class CNeighbour;
class CPacket;
class CCoolMenu;

const int	nTypeG1Size = G1_PACKTYPE_MAX;
const int	nTypeG2Size = 22;

class CPacketWnd : public CPanelWnd
{
	DECLARE_SERIAL(CPacketWnd)

public:
	CPacketWnd(CChildWnd* pOwner = NULL);
	virtual ~CPacketWnd();

public:
	void SmartDump(const CPacket* pPacket, const SOCKADDR_IN* pAddress, BOOL bUDP, BOOL bOutgoing, DWORD_PTR nNeighbourUnique = 0);
	virtual void OnSkinChange();

	CChildWnd*			m_pOwner;
	DWORD_PTR			m_nInputFilter;
	DWORD_PTR			m_nOutputFilter;
	BOOL				m_bPaused;

protected:
	BOOL				m_bTypeG1[nTypeG1Size];
	BOOL				m_bTypeG2[nTypeG2Size];
	BOOL				m_bTypeED;
	CListCtrl			m_wndList;
//	CImageList			m_gdiImageList;
	CLiveListSizer		m_pSizer;
	CFont				m_pFont;
	CCoolMenu*			m_pCoolMenu;
	CList< CLiveItem* >	m_pQueue;
	CCriticalSection	m_pSection;
	static G2_PACKET	m_nG2[nTypeG2Size];

	void AddNeighbour(CMenu* pMenus, int nGroup, UINT nID, DWORD_PTR nTarget, LPCTSTR pszText);

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnUpdateSystemClear(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateBlocker(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

#define IDC_PACKETS		100
