//
// WndHitMonitor.h
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

#include "WndBaseMatch.h"


class CHitMonitorWnd : public CBaseMatchWnd
{
	DECLARE_SERIAL(CHitMonitorWnd)

// Construction
public:
	CHitMonitorWnd();
	virtual ~CHitMonitorWnd();

// Operations
public:
	virtual BOOL	OnQueryHits(const CQueryHit* pHits);
	virtual void	OnSkinChange();

// Overrides
public:
	//{{AFX_VIRTUAL(CHitMonitorWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHitMonitorWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnHitMonitorClear();
	afx_msg void OnUpdateHitMonitorPause(CCmdUI* pCmdUI);
	afx_msg void OnHitMonitorPause();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
