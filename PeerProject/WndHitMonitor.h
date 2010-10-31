//
// WndHitMonitor.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "WndBaseMatch.h"


class CHitMonitorWnd : public CBaseMatchWnd
{
// Construction
public:
	CHitMonitorWnd();
	virtual ~CHitMonitorWnd();

	DECLARE_SERIAL(CHitMonitorWnd)

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
