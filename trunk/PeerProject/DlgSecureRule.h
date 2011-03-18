//
// DlgSecureRule.h
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

#include "DlgSkinDialog.h"

class CSecureRule;


class CSecureRuleDlg : public CSkinDialog
{
// Construction
public:
	CSecureRuleDlg(CWnd* pParent = NULL, CSecureRule* pRule = NULL);
	virtual ~CSecureRuleDlg();

	enum { IDD = IDD_SECURE_RULE };

// Dialog Data
public:
	//{{AFX_DATA(CSecureRuleDlg)
	CEdit	m_wndContent;
	CButton	m_wndGroupContent;
	CButton	m_wndGroupNetwork;
	CEdit	m_wndExpireM;
	CEdit	m_wndExpireH;
	CEdit	m_wndExpireD;
	CEdit	m_wndMask4;
	CEdit	m_wndMask3;
	CEdit	m_wndMask2;
	CEdit	m_wndMask1;
	CEdit	m_wndIP4;
	CEdit	m_wndIP3;
	CEdit	m_wndIP2;
	CEdit	m_wndIP1;
	int		m_nExpireD;
	int		m_nExpireH;
	int		m_nExpireM;
	int		m_nAction;
	int		m_nExpire;
	CString	m_sComment;
	CString	m_sContent;
	int		m_nType;	// Dropdown Box Select
	int		m_nMatch;	// Radio Button Select
	//}}AFX_DATA

	CSecureRule*	m_pRule;
	BOOL			m_bNew;

	void	ShowGroup(CWnd* pWnd, BOOL bShow);

// Overrides
public:
	//{{AFX_VIRTUAL(CSecureRuleDlg)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSecureRuleDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelChangeRuleExpire();
	afx_msg void OnSelChangeRuleType();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
