//
// WndSystem.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "WndPanel.h"
#include "CtrlText.h"


class CSystemWnd : public CPanelWnd
{
public:
	CSystemWnd();

	DECLARE_SERIAL(CSystemWnd)

protected:
	CTextCtrl	m_wndText;

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSystemClear();
	afx_msg void OnSystemCopy();
	afx_msg void OnDestroy();
	afx_msg void OnUpdateSystemVerboseError(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseError();
	afx_msg void OnUpdateSystemVerboseWarning(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseWarning();
	afx_msg void OnUpdateSystemVerboseNotice(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseNotice();
	afx_msg void OnUpdateSystemVerboseInfo(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseInfo();
	afx_msg void OnUpdateSystemVerboseDebug(CCmdUI* pCmdUI);
	afx_msg void OnSystemVerboseDebug();
	afx_msg void OnUpdateSystemTimestamp(CCmdUI* pCmdUI);
	afx_msg void OnSystemTimestamp();
	afx_msg void OnSystemTest();

	DECLARE_MESSAGE_MAP()
};
