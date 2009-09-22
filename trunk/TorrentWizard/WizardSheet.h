//
// WizardSheet.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008
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

#if !defined(AFX_WIZARDSHEET_H__E2500C10_98B2_48B0_9165_975E5444679A__INCLUDED_)
#define AFX_WIZARDSHEET_H__E2500C10_98B2_48B0_9165_975E5444679A__INCLUDED_

#pragma once

class CWizardPage;


class CWizardSheet : public CPropertySheet
{
// Construction
public:
	CWizardSheet(CWnd *pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CWizardSheet();

// Attributes
public:
	CRect			m_rcPage;
	CBitmap			m_bmHeader;
	
// Operations
public:
	static BOOL	Run(CWnd* pParent = NULL);
public:
	CWizardPage*	GetPage(CRuntimeClass* pClass);
	void			DoReset();

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardSheet)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	//{{AFX_MSG(CWizardSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonUp( UINT nHitTest, CPoint point );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


class CWizardPage : public CPropertyPage
{
// Construction
public:
	CWizardPage(UINT nID = 0);
	virtual ~CWizardPage();

	DECLARE_DYNCREATE(CWizardPage)

// Attributes
public:
	COLORREF	m_crWhite;
	CBrush		m_brWhite;

// Operations
public:
	CWizardSheet*	GetSheet();
	CWizardPage*	GetPage(CRuntimeClass* pClass);
	void			SetWizardButtons(DWORD dwFlags);
	void			StaticReplace(LPCTSTR pszSearch, LPCTSTR pszReplace);

// Implementation
protected:
	//{{AFX_MSG(CWizardPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#define GET_PAGE(gpClass, gpVar)	gpClass * gpVar = ( gpClass * )GetPage( RUNTIME_CLASS( gpClass ) )

#endif // !defined(AFX_WIZARDSHEET_H__E2500C10_98B2_48B0_9165_975E5444679A__INCLUDED_)
