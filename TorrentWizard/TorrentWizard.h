//
// TorrentWizard.h
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

#include "Resource.h"
#include "WizardSheet.h"


class CTorrentWizardApp : public CWinApp
{
// Construction
public:
	CTorrentWizardApp();

// Attributes
public:
	CString		m_sPath;
	CString		m_sVersion;
	WORD		m_nVersion[4];
	CFont		m_fntNormal;
	CFont		m_fntBold;
	CFont		m_fntLine;
	CFont		m_fntTiny;

	CString		m_sCommandLineSourceFile;
	CString		m_sCommandLineDestination;
	CString		m_sCommandLineTracker;
	CString		m_sCommandLineComment;

// Operations
protected:
	CWizardSheet*	m_pSheet;

	void		InitEnvironment();
	void		InitResources();

// Overrides
protected:
	//{{AFX_VIRTUAL(CTorrentWizardApp)
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTorrentWizardApp)
	afx_msg void OnHelp();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

extern CTorrentWizardApp theApp;

CString SmartSize(QWORD nVolume);
