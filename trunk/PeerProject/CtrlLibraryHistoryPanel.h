//
// CtrlLibraryHistoryPanel.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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

#include "SharedFile.h"
#include "CtrlPanel.h"
#include "CtrlLibraryTip.h"

class CLibraryHistoryPanel : public CPanelCtrl
{
	DECLARE_DYNAMIC(CLibraryHistoryPanel)

public:
	CLibraryHistoryPanel();
	virtual ~CLibraryHistoryPanel();

public:
	virtual void Update();

protected:
	class Item
	{
	public:
		inline Item() throw() :
			m_pRecent( NULL ),
			m_nIndex( 0 ),
			m_nIcon16( 0 ) { ZeroMemory( &m_pTime, sizeof( m_pTime ) ); }

		CLibraryRecent*	m_pRecent;
		DWORD			m_nIndex;
		SYSTEMTIME		m_pTime;
		CString			m_sText;
		CString			m_sTime;
		int				m_nIcon16;
		CRect			m_rect;
	};

	CArray< Item* >	m_pList;
	Item*			m_pHover;
	int				m_nColumns;
	CLibraryTipCtrl	m_wndTip;

	void	OnClickFile(DWORD nFile);

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
