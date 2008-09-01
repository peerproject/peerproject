//
// DlgIrcInput.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2005.
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

#pragma once

#include "DlgSkinDialog.h"
// CIrcInputDlg dialog

class CIrcInputDlg : public CSkinDialog
{
public:
	CIrcInputDlg(CWnd* pParent = NULL, int m_nCaptionIndex = 0, BOOL m_bKickOnly = FALSE);
	virtual ~CIrcInputDlg();

// Dialog Data
	enum { IDD = IDD_IRC_INPUTBOX };
	int			m_nCaptionIndex;
	BOOL		m_bKickOnly;
	CButton		m_wndPrompt;
	CEdit		m_wndAnswer;

// Overrides
protected:
	//{{AFX_VIRTUAL(CIrcInputDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CIrcInputDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

public:
	void OnOK();
	CString	m_sAnswer;
};
