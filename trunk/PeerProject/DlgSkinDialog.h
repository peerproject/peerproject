//
// DlgSkinDialog.h
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

#if !defined(AFX_DLGSKINDIALOG_H__A87746E3_AE87_49DA_A466_3F8C64364750__INCLUDED_)
#define AFX_DLGSKINDIALOG_H__A87746E3_AE87_49DA_A466_3F8C64364750__INCLUDED_

#pragma once

class CSkinWindow;


class CSkinDialog : public CDialog
{
// Construction
public:
	CSkinDialog(UINT nResID = 0, CWnd* pParent = NULL);

	DECLARE_DYNAMIC(CSkinDialog)

// Dialog Data
public:
	//{{AFX_DATA(CSkinDialog)
	//}}AFX_DATA

// Skin Support
public:
	BOOL	SkinMe(LPCTSTR pszSkin = NULL, UINT nIcon = 0, BOOL bLanguage = TRUE);
	BOOL	SelectCaption(CWnd* pWnd, int nIndex);
	CToolTipCtrl m_wndToolTip;
protected:
	CSkinWindow*	m_pSkin;

// Overrides
public:
	//{{AFX_VIRTUAL(CSkinDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSkinDialog)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg ONNCHITTESTRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGSKINDIALOG_H__A87746E3_AE87_49DA_A466_3F8C64364750__INCLUDED_)
