//
// WndSettingsPage.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#pragma once

class CSettingsSheet;


class CSettingsPage : public CDialog
{
	DECLARE_DYNAMIC(CSettingsPage)

public:
	CSettingsPage(UINT nIDTemplate, LPCTSTR pszCaption = NULL);
	virtual ~CSettingsPage();

	CString			m_sCaption;
	BOOL			m_bGroup;

	BOOL			Create(CRect& rcPage, CWnd* pSheetWnd);
	BOOL			LoadDefaultCaption();
	CSettingsPage*	GetPage(CRuntimeClass* pClass) const;

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

protected:
	CToolTipCtrl	m_wndToolTip;

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

// CEditPath
// Same functionality as CEdit has but with ability to
// run specified file or folder on mouse double click.

class CEditPath : public CEdit
{
	DECLARE_DYNCREATE(CEditPath)

protected:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
