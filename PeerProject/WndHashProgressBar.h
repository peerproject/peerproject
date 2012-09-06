//
// WndHashProgressBar.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2002-2008.
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


class CHashProgressBar : public CWnd
{
	DECLARE_DYNCREATE(CHashProgressBar)

public:
	CHashProgressBar();

protected:
	CBitmap		m_bmImage;
	CString		m_sCurrent;				// Name of file currently hashing
	CString		m_sPrevious;			// Name of last file currently hashed
	int			m_nFlash;
//	BYTE		m_nAlpha;

public:
	void		Run();

protected:
	void		Show(int nWidth, BOOL bShow);
//	void		Draw(CDC* pDC);			// Redraw window and calculate width
	void		Update();
	void		OnSkinChange();

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
