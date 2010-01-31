//
// CtrlCoolTip.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2008.
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

class CLineGraph;


class CCoolTipCtrl : public CWnd
{
	DECLARE_DYNAMIC(CCoolTipCtrl)

public:
	CCoolTipCtrl();
	virtual ~CCoolTipCtrl();

	virtual BOOL Create(CWnd* pParentWnd, bool* pbEnable = NULL);

	//void Show(T* pContext, HWND hAltWnd = NULL)
	//{
	//	bool bChanged = ( pContext != m_pContext );
	//	m_pContext = pContext;
	//	m_hAltWnd = hAltWnd;
	//	ShowImpl( bChanged );
	//}

	virtual void Hide();

protected:
	bool*	m_pbEnable;
	HWND	m_hAltWnd;
	BOOL	m_bTimer;
	BOOL	m_bVisible;
	DWORD	m_tOpen;
	CPoint	m_pOpen;
	CSize	m_sz;
	static LPCTSTR	m_hClass;

	void	ShowImpl(bool bChanged = false);
	void	CalcSizeHelper();
	void	AddSize(CDC* pDC, LPCTSTR pszText, int nBase = 0);
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

#define TIP_TEXTHEIGHT	14
#define TIP_RULE		11
#define TIP_GAP			5
