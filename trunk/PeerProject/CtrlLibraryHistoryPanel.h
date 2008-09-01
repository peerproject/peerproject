//
// CtrlLibraryHistoryPanel.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#if !defined(AFX_CTRLLIBRARYHISTORYPANEL_H__FF136FD8_4F7F_4BE6_94B8_9B751169D10A__INCLUDED_)
#define AFX_CTRLLIBRARYHISTORYPANEL_H__FF136FD8_4F7F_4BE6_94B8_9B751169D10A__INCLUDED_

#pragma once

#include "CtrlLibraryPanel.h"


class CLibraryHistoryPanel : public CLibraryPanel
{
// Construction
public:
	CLibraryHistoryPanel();
	virtual ~CLibraryHistoryPanel();

// Attributes
protected:
	class Item
	{
	public:
		inline Item() throw() :
			m_pRecent( NULL ),
			m_nIndex( 0 ),
			m_nIcon16( 0 ) { ZeroMemory( &m_pTime, sizeof ( m_pTime ) ); }

		CLibraryRecent*	m_pRecent;
		DWORD			m_nIndex;
		SYSTEMTIME		m_pTime;
		CString			m_sText;
		CString			m_sTime;
		int				m_nIcon16;
		CRect			m_rect;
	};
protected:
	CArray< Item* > m_pList;
	Item*		m_pHover;
	int			m_nColumns;
	int			m_nScrollWheelLines;

// Operations
public:
	virtual BOOL CheckAvailable(CLibraryTreeItem* pFolders, CLibraryList* pObjects);
	virtual void Update();
protected:
	void	OnClickFile(DWORD nFile);

// Overrides
public:
	//{{AFX_VIRTUAL(CLibraryHistoryPanel)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLibraryHistoryPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CTRLLIBRARYHISTORYPANEL_H__FF136FD8_4F7F_4BE6_94B8_9B751169D10A__INCLUDED_)
