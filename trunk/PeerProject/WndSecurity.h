//
// WndSecurity.h
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

#if !defined(AFX_WNDSECURITY_H__195AD96B_E718_476E_8307_52239D37E7DC__INCLUDED_)
#define AFX_WNDSECURITY_H__195AD96B_E718_476E_8307_52239D37E7DC__INCLUDED_

#pragma once

#include "WndPanel.h"

class CSecureRule;


class CSecurityWnd : public CPanelWnd
{
public:
	CSecurityWnd();
	virtual ~CSecurityWnd();

	DECLARE_SERIAL(CSecurityWnd)

// Attributes
protected:
	CCoolBarCtrl	m_wndToolBar;
	CListCtrl		m_wndList;
	CImageList		m_gdiImageList;
	CLiveListSizer	m_pSizer;
	DWORD			tLastUpdate;

// Operations
public:
	void			Update(int nColumn = -1, BOOL bSort = TRUE);
	CSecureRule*	GetItem(int nItem);
	virtual void	OnSkinChange();

// Overrides
public:
	//{{AFX_VIRTUAL(CSecurityWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSecurityWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
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

//{{AFX_INSERT_LOCATION}}

#define IDC_RULES		100

#endif // !defined(AFX_WNDSECURITY_H__195AD96B_E718_476E_8307_52239D37E7DC__INCLUDED_)
