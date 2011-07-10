//
// WndSecurity.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2007.
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

class CSecureRule;


class CSecurityWnd : public CPanelWnd
{
	DECLARE_SERIAL(CSecurityWnd)

public:
	CSecurityWnd();
	virtual ~CSecurityWnd();

// Attributes
protected:
	CCoolBarCtrl	m_wndToolBar;
	CListCtrl		m_wndList;
	CLiveListSizer	m_pSizer;
	CImageList		m_gdiImageList;
	DWORD			m_tLastUpdate;

// Operations
protected:
	CSecureRule* GetItem(int nItem);
	void		 Update(int nColumn = -1, BOOL bSort = TRUE);

// Overrides
protected:
	//{{AFX_VIRTUAL(CSecurityWnd)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnSkinChange();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSecurityWnd)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSortList(NMHDR* pNotifyStruct, LRESULT *pResult);
	afx_msg void OnUpdateSecurityEdit(CCmdUI* pCmdUI);
	afx_msg void OnSecurityEdit();
	afx_msg void OnUpdateSecurityReset(CCmdUI* pCmdUI);
	afx_msg void OnSecurityReset();
	afx_msg void OnUpdateSecurityRemove(CCmdUI* pCmdUI);
	afx_msg void OnSecurityRemove();
	afx_msg void OnSecurityAdd();
	afx_msg void OnUpdateSecurityPolicyAccept(CCmdUI* pCmdUI);
	afx_msg void OnSecurityPolicyAccept();
	afx_msg void OnUpdateSecurityPolicyDeny(CCmdUI* pCmdUI);
	afx_msg void OnSecurityPolicyDeny();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateSecurityMoveUp(CCmdUI* pCmdUI);
	afx_msg void OnSecurityMoveUp();
	afx_msg void OnUpdateSecurityMoveDown(CCmdUI* pCmdUI);
	afx_msg void OnSecurityMoveDown();
	afx_msg void OnUpdateSecurityExport(CCmdUI* pCmdUI);
	afx_msg void OnSecurityExport();
	afx_msg void OnSecurityImport();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define IDC_RULES		100
