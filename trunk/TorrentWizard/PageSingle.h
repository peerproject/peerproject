//
// PageSingle.h
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

#if !defined(AFX_PAGESINGLE_H__48CC1296_B18B_489B_B046_389F70A940B5__INCLUDED_)
#define AFX_PAGESINGLE_H__48CC1296_B18B_489B_B046_389F70A940B5__INCLUDED_

#pragma once

#include "WizardSheet.h"


class CSinglePage : public CWizardPage
{
// Construction
public:
	CSinglePage();
	virtual ~CSinglePage();

	DECLARE_DYNCREATE(CSinglePage)
	
// Dialog Data
public:
	//{{AFX_DATA(CSinglePage)
	enum { IDD = IDD_SINGLE_PAGE };
	CString	m_sFileName;
	CString	m_sFileSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSinglePage)
	public:
	virtual void OnReset();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSinglePage)
	afx_msg void OnBrowseFile();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESINGLE_H__48CC1296_B18B_489B_B046_389F70A940B5__INCLUDED_)
