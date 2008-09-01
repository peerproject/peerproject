//
// PageSettingsScheduler.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#if !defined(AFX_PAGESETTINGSSCHEDULER_H__INCLUDED_)
#define AFX_PAGESETTINGSSCHEDULER_H__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CSchedulerSettingsPage : public CSettingsPage
{
// Construction
public:
	CSchedulerSettingsPage();
	virtual ~CSchedulerSettingsPage();

	DECLARE_DYNCREATE(CSchedulerSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CSchedulerSettingsPage)
	enum { IDD = IDD_SETTINGS_SCHEDULER };
	BOOL	m_bSchedulerEnable;
	CSpinButtonCtrl	m_wndLimitedSpin;
	int		m_nLimited;
	BOOL	m_bLimitedNetworks;
	CStatic	m_wndDisplay;
	//}}AFX_DATA

// Painting Data
protected:
	BYTE			m_pSchedule[7][24];		// 2D Array representing the schedule

	CImageList		m_pTimeSlices;			// Images for the three states a time slice can be in
	CBitmap			m_bmHeader;				// Header graphic (times)

	BYTE			m_nHoverDay;			// The day of the week (0-6) or all days of the week (7)
	BYTE			m_nHoverHour;			// The hour of the day (0-23) or all hours of the day (24)
	BYTE			m_nDownDay;
	BYTE			m_nDownHour;

	CString			m_sDayName[8];			// List of day names and "Everyday"

// Overrides
public:
	//{{AFX_VIRTUAL(CSchedulerSettingsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSchedulerSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ToggleTimeBlocks(BYTE nDirection);
	void InvalidateSchedulerRect();
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGESETTINGSSCHEDULER_H__INCLUDED_)
