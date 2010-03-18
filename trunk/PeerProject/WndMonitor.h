//
// WndMonitor.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "CtrlCoolBar.h"

class CSkinWindow;
class CGraphItem;
class CMonitorBarCtrl;


class CRemoteWnd : public CWnd
{
// Construction
public:
	CRemoteWnd();
	virtual ~CRemoteWnd();

	DECLARE_DYNAMIC(CRemoteWnd)

// Command Button Class
protected:
	class CmdButton : public CCmdUI
	{
	public:
		CString		m_sName;
		CRect		m_rc;
		BOOL		m_bVisible;
		BOOL		m_bEnabled;
		BOOL		m_bChecked;
		BOOL		m_bChanged;

	public:
		CmdButton(LPCTSTR pszName);
		virtual ~CmdButton() {}

		BOOL HitTest(const CPoint& point, BOOL bAll = FALSE) const;
		void Paint(CDC* pdcWindow, CRect& rcWindow, CSkinWindow* pSkin, CmdButton* pHover, CmdButton* pDown);

		virtual void Show(BOOL bShow = TRUE);
		virtual void Enable(BOOL bOn = TRUE);
		virtual void SetCheck(int nCheck = 1);
		virtual void Execute(CFrameWnd* pTarget);
	};

// Operations
public:
	BOOL		Create(CMonitorBarCtrl* pMonitor);
	BOOL		IsVisible();
	void		OnSkinChange();

	inline void	RemoveSkin()
	{
		m_pSkin = NULL;
	}

protected:
	CmdButton*	HitTestButtons(const CPoint& ptIn, BOOL bAll = FALSE) const;
	void		UpdateCmdButtons();
	void		PaintHistory(CDC* pDC, CGraphItem* pTxItem, CGraphItem* pRxItem, DWORD nMaximum);
	void		PaintFlow(CDC* pDC, BOOL* pbDest, CRect* prcDest, BOOL* pbSrc, CRect* prcSrc, CGraphItem* pItem, DWORD nMaximum);
	void		PaintScaler(CDC* pDC);
	void		PaintMedia(CDC* pDC);
	void		PaintStatus(CDC* pDC);
	void		TrackScaler();
	void		TrackSeek();
	void		TrackVol();

// Attributes
protected:
	static LPCTSTR		m_hClass;
	CMonitorBarCtrl*	m_pMonitor;

	CSkinWindow*		m_pSkin;
	CList<CmdButton*>	m_pButtons;
	CmdButton*			m_pCmdHover;
	CmdButton*			m_pCmdDown;
	CString				m_sStatus;
	BOOL				m_bStatus;
	int					m_nTimer;

	CRect				m_rcBackground;
	BOOL				m_bsStatusText;
	CRect				m_rcsStatusText;

	BOOL				m_bsHistoryDest;
	CRect				m_rcsHistoryDest;
	BOOL				m_bsHistoryTx[2];
	CRect				m_rcsHistoryTx[2];
	BOOL				m_bsHistoryRx[2];
	CRect				m_rcsHistoryRx[2];
	BOOL				m_bsFlowTxDest;
	CRect				m_rcsFlowTxDest;
	BOOL				m_bsFlowTxSrc[2];
	CRect				m_rcsFlowTxSrc[2];
	BOOL				m_bsFlowRxDest;
	CRect				m_rcsFlowRxDest;
	BOOL				m_bsFlowRxSrc[2];
	CRect				m_rcsFlowRxSrc[2];
	BOOL				m_bsScalerTrack;
	CRect				m_rcsScalerTrack;
	BOOL				m_bsScalerTab;
	CRect				m_rcsScalerTab;
	BOOL				m_bsScalerBar;
	CRect				m_rcsScalerBar;
	BOOL				m_bScaler;
	CRect				m_rcScalerTab;

	BOOL				m_bsMediaSeekTrack;
	CRect				m_rcsMediaSeekTrack;
	BOOL				m_bsMediaSeekTab;
	CRect				m_rcsMediaSeekTab;
	BOOL				m_bsMediaSeekBar;
	CRect				m_rcsMediaSeekBar;
	BOOL				m_bsMediaVolTrack;
	CRect				m_rcsMediaVolTrack;
	BOOL				m_bsMediaVolTab;
	CRect				m_rcsMediaVolTab;
	BOOL				m_bsMediaVolBar;
	CRect				m_rcsMediaVolBar;
	BOOL				m_bMediaSeek;
	float				m_nMediaSeek;
	CRect				m_rcMediaSeekTab;
	BOOL				m_bMediaVol;
	float				m_nMediaVol;
	CRect				m_rcMediaVolTab;
	BOOL				m_bsMediaStates[2][3];
	CRect				m_rcsMediaStates[2][3];

// Message Map
protected:
	DECLARE_MESSAGE_MAP()
 	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
};
