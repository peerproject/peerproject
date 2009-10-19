//
// WizardSheet.h
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

#include "PagePropertyAdv.h"


class CWizardSheet : public CPropertySheetAdv
{
	DECLARE_DYNAMIC(CWizardSheet)

public:
	CWizardSheet(CWnd *pParentWnd = NULL, UINT iSelectPage = 0);

	CRect			m_rcPage;
	CBitmap			m_bmHeader;

	static BOOL	RunWizard(CWnd* pParent = NULL);

protected:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);

	DECLARE_MESSAGE_MAP()
};


class CWizardPage : public CPropertyPageAdv
{
	DECLARE_DYNCREATE(CWizardPage)

public:
	CWizardPage(UINT nID = 0);

	CWizardSheet*	GetSheet();
	void			SetWizardButtons(DWORD dwFlags);
	void			StaticReplace(LPCTSTR pszSearch, LPCTSTR pszReplace);
	BOOL			IsConnectionCapable();

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
