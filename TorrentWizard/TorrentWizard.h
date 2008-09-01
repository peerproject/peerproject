//
// TorrentWizard.h
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

#if !defined(AFX_TORRENTWIZARD_H__5756891D_0476_49ED_A2B2_4889DE191BA3__INCLUDED_)
#define AFX_TORRENTWIZARD_H__5756891D_0476_49ED_A2B2_4889DE191BA3__INCLUDED_

#pragma once

#include "Resource.h"

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
	BOOL		m_bNT;
	CFont		m_fntNormal;
	CFont		m_fntBold;
	CFont		m_fntLine;
	CFont		m_fntTiny;

	BOOL		m_bCommandLine;
	CString		m_sCommandLineSourceFile;
	CString		m_sCommandLineDestination;
	CString		m_sCommandLineTracker;
	
// Operations
protected:
	void		InitEnvironment();
	void		InitResources();
	
// Overrides
public:
	//{{AFX_VIRTUAL(CTorrentWizardApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTorrentWizardApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

extern CTorrentWizardApp theApp;

CString SmartSize(QWORD nVolume);

#endif // !defined(AFX_TORRENTWIZARD_H__5756891D_0476_49ED_A2B2_4889DE191BA3__INCLUDED_)
