//
// CtrlIRCPanel.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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
// Original Author: peer_l_@hotmail.com
//

#pragma once

#include "CtrlTaskPanel.h"
#include "CtrlCoolBar.h"
#include "CtrlIconButton.h"

#define	ID_COLOR_LISTTEXT				6
#define IDC_IRC_USERS					121
#define IDC_IRC_CHANNELS				122
#define IDC_IRC_DBLCLKCHANNELS			200
#define IDC_IRC_DBLCLKUSERS				201
#define	IDC_IRC_MENUUSERS				202
#define IDC_IRC_ADDCHANNEL				203
#define IDC_IRC_REMOVECHANNEL			204

#define	WM_REMOVECHANNEL				20933
#define	WM_ADDCHANNEL					20934

class CIRCChannelsBox : public CTaskBox
{
	DECLARE_DYNAMIC(CIRCChannelsBox)

public:
	CIRCChannelsBox();
	virtual ~CIRCChannelsBox();

public:
	CListCtrl		m_wndChanList;
	CIconButtonCtrl	m_wndAddChannel;
	CIconButtonCtrl	m_wndRemoveChannel;
	HCURSOR			m_hHand;
	CDC				m_dcBuffer;
	HBITMAP			m_hBuffer;
	CBitmap			m_bmWatermark;
	CString			m_sPassedChannel;

	void	OnSkinChange();

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnAddChannel();
	afx_msg void OnRemoveChannel();
	afx_msg void OnChansDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

class CIRCUsersBox : public CTaskBox
{
	DECLARE_DYNAMIC(CIRCUsersBox)

public:
	CIRCUsersBox();
	virtual ~CIRCUsersBox();

public:
	CListBox		m_wndUserList;
	CDC				m_dcBuffer;
	HBITMAP			m_hBuffer;
	CBitmap			m_bmWatermark;

	void	OnSkinChange();
	void	UpdateCaptionCount();

protected:
	int		HitTest(const CPoint& pt) const;

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnUsersDoubleClick();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int  OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct);

	DECLARE_MESSAGE_MAP()
};

class CIRCPanel : public CTaskPanel
{
	DECLARE_DYNAMIC(CIRCPanel)

public:
	CIRCPanel();
	virtual ~CIRCPanel();

public:
	CIRCUsersBox	m_boxUsers;
	CIRCChannelsBox	m_boxChans;

	void	OnSkinChange();
	virtual BOOL Create(CWnd* pParentWnd);

protected:
	CFont	m_pFont;

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

typedef struct
{
	NMHDR	hdr;
} IRC_PANELEVENT;
