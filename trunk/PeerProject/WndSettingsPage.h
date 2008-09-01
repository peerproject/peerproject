//
// WndSettingsPage.h
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

#if !defined(AFX_WNDSETTINGSPAGE_H__E0931D5D_AEFE_437E_9C5B_F1E0147EFD70__INCLUDED_)
#define AFX_WNDSETTINGSPAGE_H__E0931D5D_AEFE_437E_9C5B_F1E0147EFD70__INCLUDED_

#pragma once

class CSettingsSheet;


class CSettingsPage : public CDialog
{
// Construction
public:
	CSettingsPage(UINT nIDTemplate, LPCTSTR pszCaption = NULL);
	virtual ~CSettingsPage();

	DECLARE_DYNAMIC(CSettingsPage)

// Attributes
public:
	CString	m_sCaption;
	BOOL	m_bGroup;
	CToolTipCtrl m_wndToolTip;

// Operations
public:
	BOOL	Create(CRect& rcPage, CWnd* pSheetWnd);
	BOOL	LoadDefaultCaption();

	CSettingsPage* GetPage(CRuntimeClass* pClass) const;

	inline CSettingsSheet* GetSheet() const
	{
		return (CSettingsSheet*)GetParent();
	}

	inline LPCTSTR GetTemplateName() const
	{
		return m_lpszTemplateName;
	}

// Events
public:
	virtual void SetModified(BOOL bChanged = TRUE);
	virtual BOOL OnApply();
	virtual void OnReset();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

// Standard Dialog Functionality
public:
	//{{AFX_DATA(CSettingsPage)
	//}}AFX_DATA
	//{{AFX_VIRTUAL(CSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CSettingsPage)
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_WNDSETTINGSPAGE_H__E0931D5D_AEFE_437E_9C5B_F1E0147EFD70__INCLUDED_)
