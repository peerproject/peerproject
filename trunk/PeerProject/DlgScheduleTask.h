//
// DlgScheduleItem.h
//
// This file is part of PeerProject (peerproject.org) © 2010
// Portions Copyright Shareaza Development Team, 2010.
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

#include "DlgSkinDialog.h"
#include <afxdtctl.h>		// MFC date & time controls

class CScheduleTask;

//////////////////////////////////////////////////////////////////////
// CScheduleTaskDlg class: Schedule Item Dialog
//

class CScheduleTaskDlg : public CSkinDialog
{
// Construction
public:
	CScheduleTaskDlg(CWnd* pParent = NULL, CScheduleTask* pSchTask = NULL);
	virtual ~CScheduleTaskDlg();

	enum { IDD = IDD_SCHEDULE_TASK };

public:
	CScheduleTask		*m_pScheduleTask;
	unsigned int		m_nAction;
	unsigned int		m_nDays;
	CString				m_sDescription;
	CTime				m_tDateAndTime;
	bool				m_bSpecificDays;
	bool				m_bActive;
	bool				m_bNew;
	BOOL				m_bLimitedNetworks;
	int					m_nLimitDown;
	int					m_nLimitUp;

	CComboBox			m_wndTypeSel;
	CButton				m_wndLimitedCheck;
	CEdit				m_wndLimitedEditDown;
	CEdit				m_wndLimitedEditUp;
	CStatic				m_wndLimitedStaticDown;
	CStatic				m_wndLimitedStaticUp;
	CSpinButtonCtrl		m_wndSpinDown;
	CSpinButtonCtrl		m_wndSpinUp;
	CDateTimeCtrl		m_wndDate;
	CDateTimeCtrl		m_wndTime;
	CButton				m_wndActiveCheck;
	CButton				m_wndRadioOnce;
	CButton				m_wndRadioEveryDay;
	CButton				m_wndChkDayMon;
	CButton				m_wndChkDayTues;
	CButton				m_wndChkDayWed;
	CButton				m_wndChkDayThu;
	CButton				m_wndChkDayFri;
	CButton				m_wndChkDaySat;
	CButton				m_wndChkDaySun;
	CButton				m_wndBtnAllDays;

protected:
	void EnableDaysOfWeek(bool bEnable);

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnBnClickedOnlyonce();
	afx_msg void OnDtnDatetimechangeDate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEveryday();
	afx_msg void OnCbnSelchangeEventType();
	afx_msg void OnBnClickedButtonAllDays();

	DECLARE_MESSAGE_MAP()
};
