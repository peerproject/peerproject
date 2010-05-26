//
// DlgScheduleItem.h
//
// This file is part of PeerProject (peerproject.org) � 2010
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
#include "afxdtctl.h"
#include "afxwin.h"

class CScheduleTask;

//////////////////////////////////////////////////////////////////////
//
// CScheduleTaskDlg class: Schedule Item Dialog
//

class CScheduleTaskDlg : public CSkinDialog
{
// Construction
public:
	CScheduleTaskDlg(CWnd* pParent = NULL, CScheduleTask* pSchTask = NULL);
	virtual ~CScheduleTaskDlg();

// Dialog Data
public:
	//{{AFX_DATA(CScheduleTaskDlg)
	enum { IDD = IDD_SCHEDULE_TASK };

	//}}AFX_DATA

	CScheduleTask		*m_pScheduleTask;
	bool				m_bSpecificDays;
	unsigned int		m_nAction;
	unsigned int		m_nDays;
	CString				m_sDescription;
	CTime				m_tDateAndTime;
	bool				m_bActive;
	bool				m_bNew;
	BOOL				m_bToggleBandwidth;
	BOOL				m_bLimitedNetworks;
	int					m_nLimit;
	int					m_nLimitDown;
	int					m_nLimitUp;

// Overrides
public:
	//{{AFX_VIRTUAL(CScheduleTaskDlg)

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CScheduleTaskDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedOnlyonce();
	afx_msg void OnBnClickedToggleBandwidth();
	afx_msg void OnDtnDatetimechangeDate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEveryday();
	afx_msg void OnBnClickedActive();
	afx_msg void OnCbnSelchangeEventtype();
	afx_msg void OnBnClickedButtonAllDays();
	//}}AFX_MSG
	void EnableDaysOfWeek(bool bEnable);
	DECLARE_MESSAGE_MAP()
public:
	CEdit				m_wndLimitedEdit;
	CComboBox			m_wndTypeSel;
	CEdit				m_wndLimitedEditDown;
	CEdit				m_wndLimitedEditUp;
	CButton				m_wndLimitedCheck;
	CButton				m_wndLimitedCheckTgl;
	CStatic				m_wndLimitedStatic;
	CStatic				m_wndLimitedStaticDown;
	CStatic				m_wndLimitedStaticUp;
	CButton				m_wndActiveCheck;
	CDateTimeCtrl		m_wndDate;
	CDateTimeCtrl		m_wndTime;
	CSpinButtonCtrl		m_wndSpin;
	CSpinButtonCtrl		m_wndSpinDown;
	CSpinButtonCtrl		m_wndSpinUp;
	CButton				m_wndRadioOnce;
	CButton				m_wndRadioEveryDay;
	CButton				m_wndChkDaySun;
	CButton				m_wndChkDayMon;
	CButton				m_wndChkDayTues;
	CButton				m_wndChkDayWed;
	CButton				m_wndChkDayThu;
	CButton				m_wndChkDayFri;
	CButton				m_wndChkDaySat;
	CStatic				m_wndGrpBoxDayOfWeek;
	CButton				m_wndBtnAllDays;
};

//{{AFX_INSERT_LOCATION}}
