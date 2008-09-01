//
// CtrlIconButton.h
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

#if !defined(AFX_CTRLICONBUTTON_H__5E629D93_681A_4631_BD16_166C2E025871__INCLUDED_)
#define AFX_CTRLICONBUTTON_H__5E629D93_681A_4631_BD16_166C2E025871__INCLUDED_

#pragma once

class CIconButtonCtrl : public CWnd
{
	DECLARE_DYNAMIC(CIconButtonCtrl)

public:
	CIconButtonCtrl();

	void	SetText(LPCTSTR pszText);
	void	SetIcon(HICON hIcon, BOOL bMirrored = FALSE);
	void	SetCoolIcon(UINT nIconID, BOOL bMirrored = FALSE);
	void	SetIcon(UINT nIconID, BOOL bMirrored = FALSE);
	void	SetHandCursor(BOOL bCursor);

	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nControlID, DWORD dwStyle = 0);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CImageList	m_pImageList;
	BOOL		m_bCapture;
	BOOL		m_bDown;
	BOOL		m_bCursor;

	BOOL	RemoveStyle();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLICONBUTTON_H__5E629D93_681A_4631_BD16_166C2E025871__INCLUDED_)
