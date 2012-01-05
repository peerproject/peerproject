//
// CtrlCoolTip.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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

class CLineGraph;


class CCoolTipCtrl : public CWnd
{
	DECLARE_DYNAMIC(CCoolTipCtrl)

public:
	CCoolTipCtrl();
	virtual ~CCoolTipCtrl();

	virtual BOOL Create(CWnd* pParentWnd, bool* pbEnable = NULL);
	virtual void Hide();

	//void Show(T* pContext, HWND hAltWnd = NULL)
	//{
	//	bool bChanged = ( pContext != m_pContext );
	//	m_pContext = pContext;
	//	m_hAltWnd = hAltWnd;
	//	ShowImpl( bChanged );
	//}

public:
	static LPCTSTR	m_hClass;	// CWnd Style

protected:
	bool*	m_pbEnable;
	BOOL	m_bVisible;
	HWND	m_hAltWnd;
	BOOL	m_bTimer;
	DWORD	m_tOpen;
	CPoint	m_pOpen;
	CSize	m_sz;

	void	ShowImpl(bool bChanged = false);
	void	CalcSizeHelper();
	void	AddSize(CDC* pDC, LPCTSTR pszText, int nBase = 0);
	int		GetSize(CDC* pDC, LPCTSTR pszText) const;
	void	GetPaintRect(RECT* pRect);
	void	DrawText(CDC* pDC, POINT* pPoint, LPCTSTR pszText, int nBase);
	void	DrawText(CDC* pDC, POINT* pPoint, LPCTSTR pszText, SIZE* pTextMaxSize = NULL);
	void	DrawRule(CDC* pDC, POINT* pPoint, BOOL bPos = FALSE);
	BOOL	WindowFromPointBelongsToOwner(const CPoint& point);
	CLineGraph*	CreateLineGraph();

	virtual BOOL OnPrepare();
	virtual void OnCalcSize(CDC* pDC);
	virtual void OnShow();
	virtual void OnHide();
	virtual void OnPaint(CDC* pDC);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

#ifndef WS_EX_LAYERED
  #define WS_EX_LAYERED	0x80000
  #define LWA_ALPHA		0x02
#endif


//#define TIP_TIMER_GRAPH 72	// 30s display (Settings.Interface.RefreshRateGraph)
#define TIP_TIMER_TEXT	400		// (Settings.Interface.RefreshRateText ?)
#define TIP_OFFSET_X	0
#define TIP_OFFSET_Y	24
#define TIP_MARGIN		7

#define TIP_GRAPHHEIGHT	40
#define TIP_BARHEIGHT	14
#define TIP_TEXTHEIGHT	14
#define TIP_RULE		11
#define TIP_GAP			5
