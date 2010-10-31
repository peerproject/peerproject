//
// PageSettingsBandwidth.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "WndSettingsPage.h"


class CBandwidthSettingsPage : public CSettingsPage
{
// Construction
public:
	CBandwidthSettingsPage();
	virtual ~CBandwidthSettingsPage();

	DECLARE_DYNCREATE(CBandwidthSettingsPage)

	enum { IDD = IDD_SETTINGS_BANDWIDTH };

// Dialog Data
public:
	CStatic	m_wndHeadUP2;
	CStatic	m_wndHeadUP1;
	CStatic	m_wndHeadTransmit;
	CStatic	m_wndHeadReceive;
	CString	m_sUPInLimit;
	CString	m_sUPInMax;
	CString	m_sUPInTotal;
	CString	m_sUPOutLimit;
	CString	m_sUPOutMax;
	CString	m_sUPOutTotal;
	CString	m_sDLTotal;
	CString	m_sInTotal;
	CString	m_sOutTotal;
	CString	m_sPInTotal;
	CString	m_sPOutLimit;
	CString	m_sPOutMax;
	CString	m_sPOutTotal;
	CString	m_sULTotal;
	CString	m_sPInLimit;
	CString	m_sPInMax;
	CString	m_sLInLimit;
	CString	m_sLInMax;
	CString	m_sLInTotal;
	CString	m_sLOutLimit;
	CString	m_sLOutMax;
	CString	m_sLOutTotal;
	CString	m_sUDPTotal;

// Operations
public:
	void	Calculate(BOOL bForeward);
	void	Calculate(CString& strLimit, int nCount, CString& strCount, CString& strTotal, BOOL bForeward);
	DWORD	ToSpeed(CString& str);
	CString	ToString(DWORD nSpeed, BOOL bUnlimited = TRUE, BOOL bUnit = FALSE);
	DWORD	AddString(CString& str);
	void	SwapBytesBits(CString& str);

// Attributes
protected:
	BOOL	m_bActive;
	BOOL	m_bBytes;

// Overrides
public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
