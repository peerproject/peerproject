//
// DlgIrcInput.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2005.
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

#include "DlgSkinDialog.h"
// CIrcInputDlg dialog

class CIrcInputDlg : public CSkinDialog
{
public:
	CIrcInputDlg(CWnd* pParent = NULL, int m_nCaptionIndex = 0, BOOL m_bKickOnly = FALSE);
	virtual ~CIrcInputDlg();

	enum { IDD = IDD_IRC_INPUTBOX };

// Dialog Data
public:
	int			m_nCaptionIndex;
	BOOL		m_bKickOnly;
	CButton		m_wndPrompt;
	CEdit		m_wndAnswer;
	CString 	m_sAnswer;

	void OnOK();

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
};
