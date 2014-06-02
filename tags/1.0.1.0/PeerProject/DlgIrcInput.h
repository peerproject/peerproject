//
// DlgIrcInput.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
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

class CIrcInputDlg : public CSkinDialog
{
public:
	CIrcInputDlg(CWnd* pParent = NULL, int m_nCaptionIndex = 0, BOOL m_bKickOnly = FALSE);
	virtual ~CIrcInputDlg();

	enum { IDD = IDD_IRC_INPUTBOX };

public:
	int			m_nCaptionIndex;
	BOOL		m_bKickOnly;
	CButton		m_wndPrompt;
	CEdit		m_wndAnswer;
	CString 	m_sAnswer;

	void OnOK();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();
};
