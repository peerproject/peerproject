//
// PageCommandline.h
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

#pragma once

#include "WizardSheet.h"

class CTorrentBuilder;


class CCommandlinePage : public CWizardPage
{
// Construction
public:
	CCommandlinePage();
	virtual ~CCommandlinePage();

	DECLARE_DYNCREATE(CCommandlinePage)

// Dialog Data
public:
	//{{AFX_DATA(CCommandlinePage)
	enum { IDD = IDD_COMMANDLINE_PAGE };
	CButton	m_wndAbort;
	CEdit	m_wndTorrentName;
	CButton	m_wndTorrentOpen;
	CButton	m_wndTorrentSeed;
	CStatic	m_wndSpeedMessage;
	CSliderCtrl	m_wndSpeed;
	CStatic	m_wndSpeedSlow;
	CStatic	m_wndSpeedFast;
	CProgressCtrl	m_wndProgress;
	CStatic	m_wndFileName;
	CStatic	m_wndDone2;
	CStatic	m_wndDone1;
	//}}AFX_DATA

// Attributes
protected:
	CTorrentBuilder*	m_pBuilder;
	CString				m_sDestinationFile;

// Operations
protected:
	void		Start();

// Overrides
public:
	//{{AFX_VIRTUAL(CCommandlinePage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCommandlinePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAbort();
	afx_msg void OnTorrentOpen();
	afx_msg void OnTorrentSeed();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnStnClickedDone2();
};
