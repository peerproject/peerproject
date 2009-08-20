//
// PageTracker.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008
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

#if !defined(AFX_PAGETRACKER_H__98B08251_6D7C_4028_943D_5E483220F28D__INCLUDED_)
#define AFX_PAGETRACKER_H__98B08251_6D7C_4028_943D_5E483220F28D__INCLUDED_

#pragma once

#include "WizardSheet.h"


class CTrackerPage : public CWizardPage
{
// Construction
public:
	CTrackerPage();
	virtual ~CTrackerPage();

	DECLARE_DYNCREATE(CTrackerPage)

// Dialog Data
public:
	//{{AFX_DATA(CTrackerPage)
	enum { IDD = IDD_TRACKER_PAGE };
	CComboBox	m_wndTracker;
	CComboBox	m_wndTracker2;
	CString	m_sTracker;
	CString	m_sTracker2;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CTrackerPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTrackerPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClearTrackers();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGETRACKER_H__98B08251_6D7C_4028_943D_5E483220F28D__INCLUDED_)
