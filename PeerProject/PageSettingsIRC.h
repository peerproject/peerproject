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
// Author: peer_l_@hotmail.com
//

#if !defined(AFX_PAGESETTINGSIRC__INCLUDED_)
#define AFX_PAGESETTINGSIRC__INCLUDED_

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
	CButton m_wndColorServer;
	CButton	m_wndColorTopic;
	CButton	m_wndColorAction;
	CButton	m_wndColorNotice;
	CButton	m_wndColorBg;
	CButton	m_wndColorText;
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnDrawItem(int /* nIDCtl */, LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	virtual void OnOK();
	virtual BOOL OnApply();

// Implementation
protected:
	afx_msg void OnBnClickedIrcColorServer();
	afx_msg void OnBnClickedIrcColorTopic();
	afx_msg void OnBnClickedIrcColorAction();
	afx_msg void OnBnClickedIrcColorNotice();
	afx_msg void OnBnClickedIrcColorBg();
	afx_msg void OnBnClickedIrcColorText();

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PAGESETTINGSIRC__INCLUDED_)
 
#define	ID_COLOR_SERVERMSG				2
#define	ID_COLOR_TOPIC					3
#define	ID_COLOR_CHANNELACTION			4
#define	ID_COLOR_NOTICE					5
#define	ID_COLOR_CHATBACK				7
#define ID_COLOR_TEXT					8
