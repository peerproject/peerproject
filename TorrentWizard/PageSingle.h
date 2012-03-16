//
// PageSingle.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008,2012
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once

#include "WizardSheet.h"


class CSinglePage : public CWizardPage
{
	DECLARE_DYNCREATE(CSinglePage)

// Construction
public:
	CSinglePage();
	//virtual ~CSinglePage();

	enum { IDD = IDD_SINGLE_PAGE };

// Dialog Data
public:
	//{{AFX_DATA(CSinglePage)
	CString	m_sFileName;
	CString	m_sFileSize;
	//CString	m_sMagnet;
	//}}AFX_DATA

protected:
	void Update();

// Overrides
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSinglePage)
	virtual void OnReset();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSinglePage)
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBrowseFile();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
