//
// WizardConnectionPage.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "ThreadImpl.h"
#include "WizardSheet.h"


class CWizardConnectionPage :
	public CWizardPage,
	public CThreadImpl
{
	DECLARE_DYNCREATE(CWizardConnectionPage)

// Construction
public:
	CWizardConnectionPage();
	virtual ~CWizardConnectionPage();

	enum { IDD = IDD_WIZARD_CONNECTION };

// Dialog Data
protected:
	CComboBox	m_wndType;
	CComboBox	m_wndDownloadSpeed;
	CComboBox	m_wndUploadSpeed;
	CComboBox	m_wndUPnP;
	CEdit		m_wndPort;
	CButton		m_wndRandom;
	CStatic		m_wndStatus;
	CStatic		m_wndTest;
	CProgressCtrl m_wndProgress;

	bool		m_bQueryDiscoveries;
	bool		m_bUpdateServers;
	BOOL		m_bRandom;
	DWORD		m_nPort;
	short		m_nProgressSteps;

	std::map < const DWORD, DWORD > m_mapSpeed;

	CString		SpeedFormat(const double nSpeed) const;

// Operations
protected:
	void		OnRun();

// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnQueryCancel();
	virtual LRESULT OnWizardNext();

	afx_msg void OnSelChangeConnectionType();
	afx_msg void OnChangeConnectionSpeed();
	afx_msg void OnBnClickedRandom();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
