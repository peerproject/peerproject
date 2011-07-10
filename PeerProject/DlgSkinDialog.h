//
// DlgSkinDialog.h
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

class CSkinWindow;


class CSkinDialog : public CDialog
{
	DECLARE_DYNAMIC(CSkinDialog)

public:
	CSkinDialog(UINT nResID = 0, CWnd* pParent = NULL, BOOL bAutoBanner = TRUE);

	virtual BOOL	SkinMe(LPCTSTR pszSkin = NULL, UINT nIcon = 0, BOOL bLanguage = TRUE);
	virtual BOOL	SelectCaption(CWnd* pWnd, int nIndex);

protected:
	CSkinWindow*	m_pSkin;
	CStatic			m_oBanner;		// Banner to add (id=IDC_BANNER, bitmap=IDB_BANNER)
	BOOL			m_bAutoBanner;	// Add banner to top of dialog (default = yes)

	void EnableBanner(BOOL bEnable = TRUE);

	//int GetBannerHeight() const	// Using Skin.m_nBanner instead
	//{
	//	if ( CStatic* pBanner = (CStatic*)GetDlgItem( IDC_BANNER ) )
	//	{
	//		BITMAP bm = {};
	//		GetObject( pBanner->GetBitmap(), sizeof( BITMAP ), &bm );
	//		return bm.bmHeight;
	//	}
	//	return 0;
	//}

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
