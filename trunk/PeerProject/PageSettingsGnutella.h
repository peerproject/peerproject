//
// PageSettingsGnutella.h
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

#if !defined(AFX_PAGESETTINGSGNUTELLA_H__94824865_78DA_468A_BDF0_55E6C4060058__INCLUDED_)
#define AFX_PAGESETTINGSGNUTELLA_H__94824865_78DA_468A_BDF0_55E6C4060058__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CGnutellaSettingsPage : public CSettingsPage
{
// Construction
public:
	CGnutellaSettingsPage();
	virtual ~CGnutellaSettingsPage();

	DECLARE_DYNCREATE(CGnutellaSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CGnutellaSettingsPage)
	enum { IDD = IDD_SETTINGS_GNUTELLA };
	CSpinButtonCtrl	m_wndG2Peers;
	CSpinButtonCtrl	m_wndG2Leafs;
	CSpinButtonCtrl	m_wndG2Hubs;
	CSpinButtonCtrl	m_wndG1Peers;
	CSpinButtonCtrl	m_wndG1Leafs;
	CSpinButtonCtrl	m_wndG1Hubs;
	BOOL	m_bG2Today;
	BOOL	m_bG2Always;
	BOOL	m_bG1Today;
	BOOL	m_bG1Always;
	CComboBox m_wndG1ClientMode;
	int		m_nG1Hubs;
	int		m_nG1Leafs;
	int		m_nG1Peers;
	CComboBox m_wndG2ClientMode;
	int		m_nG2Hubs;
	int		m_nG2Leafs;
	int		m_nG2Peers;
	BOOL	m_bDeflateHub2Hub;
	BOOL	m_bDeflateLeaf2Hub;
	BOOL	m_bDeflateHub2Leaf;
	//}}AFX_DATA
	BOOL	m_bAgent;

// Overrides
public:
	//{{AFX_VIRTUAL(CGnutellaSettingsPage)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGnutellaSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnG2Today();
	afx_msg void OnG1Today();
	afx_msg void OnG2Always();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGESETTINGSGNUTELLA_H__94824865_78DA_468A_BDF0_55E6C4060058__INCLUDED_)
