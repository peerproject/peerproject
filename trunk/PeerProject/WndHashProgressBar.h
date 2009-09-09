//
// WndHashProgressBar.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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


class CHashProgressBar : public CWnd
{
	DECLARE_DYNCREATE(CHashProgressBar)

public:
	CHashProgressBar();

protected:
	CWnd*		m_pParent;
	COLORREF	m_crText;
	COLORREF	m_crBorder;
	COLORREF	m_crFill;
	CBitmap		m_bmImage;
	HICON		m_hIcon;
	int			m_nFlash;
	CString		m_sCurrent;				// Name of file currently hashing
	CString		m_sPrevious;			// Name of last file currently hashed

public:
	void	Create(CWnd* pParent);
	void	Run();
	void	Update();
	void	OnSkinChange();

protected:
	void	Show(int nWidth, BOOL bShow);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
