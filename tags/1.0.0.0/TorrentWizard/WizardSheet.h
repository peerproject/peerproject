//
// WizardSheet.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008-2012
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once

class CWizardPage;


class CWizardSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CWizardSheet)

// Construction
public:
	CWizardSheet(CWnd *pParentWnd = NULL, UINT iSelectPage = 0);
	//virtual ~CWizardSheet();

// Attributes
public:
	CRect			m_rcPage;
	CBitmap			m_bmHeader;
	int 			m_nBannerHeight;

// Operations
public:
	CWizardPage*	GetPage(CRuntimeClass* pClass);
	void			DoReset();

// Overrides
protected:
	//{{AFX_VIRTUAL(CWizardSheet)
	virtual BOOL	OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual BOOL	OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardSheet)
	afx_msg void	OnPaint();
//	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL	OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnNcLButtonUp( UINT nHitTest, CPoint point );
	afx_msg void	OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CWizardPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWizardPage)

// Construction
public:
	CWizardPage(UINT nID = 0);
	//virtual ~CWizardPage();

// Attributes
public:
	CBrush			m_brPageColor;	// PAGE_COLOR (Was m_brWhite)

// Operations
public:
	CWizardSheet*	GetSheet();
	CWizardPage*	GetPage(CRuntimeClass* pClass);
	void			SetWizardButtons(DWORD dwFlags);
	void			StaticReplace(LPCTSTR pszSearch, LPCTSTR pszReplace);
	void			Next();

// Implementation
protected:
	//{{AFX_MSG(CWizardPage)
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnPressButton(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define GET_PAGE(gpClass, gpVar)	gpClass * gpVar = ( gpClass * )GetPage( RUNTIME_CLASS( gpClass ) )

#define WM_PRESSBUTTON (WM_APP + 100)
