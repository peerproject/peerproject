//
// WizardSheet.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "PagePropertyAdv.h"
#include "CoolInterface.h"	// For FixInterface Checkboxes


class CWizardSheet : public CPropertySheetAdv
{
	DECLARE_DYNAMIC(CWizardSheet)

public:
	CWizardSheet(CWnd *pParentWnd = NULL, UINT iSelectPage = 0);

	CRect			m_rcPage;
	CBitmap			m_bmHeader;

	static BOOL 	RunWizard(CWnd* pParent = NULL);

protected:
	virtual BOOL	OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual BOOL	OnInitDialog();

	afx_msg void	OnPaint();
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);

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
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
