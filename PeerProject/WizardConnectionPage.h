//
// WizardConnectionPage.h
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

#include "ThreadImpl.h"
#include "WizardSheet.h"


class CWizardConnectionPage :
	public CWizardPage,
	public CThreadImpl
{
// Construction
public:
	CWizardConnectionPage();
	virtual ~CWizardConnectionPage();

	DECLARE_DYNCREATE(CWizardConnectionPage)

// Dialog Data
public:
	//{{AFX_DATA(CWizardConnectionPage)
	enum { IDD = IDD_WIZARD_CONNECTION };
	CComboBox	m_wndType;
	CComboBox	m_wndDownloadSpeed;
	CComboBox	m_wndUploadSpeed;
	CComboBox	m_wndUPnP;
	CEdit		m_wndPort;
	CButton		m_wndRandom;
	CStatic		m_wndStatus;
	CProgressCtrl m_wndProgress;
	//}}AFX_DATA

// Operations
protected:
	void		OnRun();

// Overrides
public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnQueryCancel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	//{{AFX_MSG(CWizardConnectionPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeConnectionType();
	afx_msg void OnChangeConnectionSpeed();
	afx_msg void OnSelChangeUPnP();
	afx_msg void OnBnClickedRandom();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool	m_bQueryDiscoveries;
	bool	m_bUpdateDonkeyServers;
	bool	m_bUPnPForward;
	BOOL	m_bRandom;
	DWORD	m_nPort;
	short	m_nProgressSteps;
};
