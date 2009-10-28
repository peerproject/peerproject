//
// PageSettingsIRC.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2005-2007.
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
// Original Author: peer_l_@hotmail.com
//

#pragma once

#include "WndSettingsPage.h"
#include "CtrlFontCombo.h"

class CIRCSettingsPage : public CSettingsPage
{
// Construction
public:
	CIRCSettingsPage();
	virtual ~CIRCSettingsPage();

	DECLARE_DYNCREATE(CIRCSettingsPage)
// Dialog Data
public:
	enum { IDD = IDD_SETTINGS_IRC };
	CButton	m_wndColorBg;
	CButton	m_wndColorText;
	CButton	m_wndColorTextLocal;
	CButton	m_wndColorUserAction;
	CButton	m_wndColorAction;
	CButton m_wndColorServer;
	CButton	m_wndColorNotice;
	CButton	m_wndColorTopic;
	BOOL	m_bShow;
	BOOL	m_bFloodEnable;
	BOOL	m_bTimestamp;
	CString	m_sNick;
	CString	m_sAlternate;
	CString	m_sServerName;
	CString m_sUserName;
	CString m_sRealName;
	CString	m_sServerPort;
	CString	m_sFloodLimit;
	CString m_sScreenFont;
	CFontCombo m_wndFonts;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	virtual void OnOK();
	virtual BOOL OnApply();

// Implementation
protected:
	afx_msg void OnClickIrcColorBg();
	afx_msg void OnClickIrcColorText();
	afx_msg void OnClickIrcColorTextLocal();
	afx_msg void OnClickIrcColorUserAction();
	afx_msg void OnClickIrcColorAction();
	afx_msg void OnClickIrcColorServer();
	afx_msg void OnClickIrcColorNotice();
	afx_msg void OnClickIrcColorTopic();

	DECLARE_MESSAGE_MAP()
};

#define	ID_COLOR_CHATWINDOW				0
#define ID_COLOR_TEXT					1
#define ID_COLOR_TEXTLOCAL				2
#define	ID_COLOR_CHANNELACTION			3
#define	ID_COLOR_ME						4
#define	ID_COLOR_SERVERMSG				7
#define	ID_COLOR_TOPIC					8
#define	ID_COLOR_NOTICE					9
