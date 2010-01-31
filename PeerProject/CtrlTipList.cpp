//
// CtrlTipList.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "CtrlTipList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTipListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CTipListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CTipListCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTipListCtrl construction

CTipListCtrl::CTipListCtrl()
	: m_pTip( NULL )
{
}

CTipListCtrl::~CTipListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTipListCtrl operations

void CTipListCtrl::SetTip(CNeighbourTipCtrl* pTip)
{
	m_pTip = pTip;
}

/////////////////////////////////////////////////////////////////////////////
// CTipListCtrl message handlers

void CTipListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CListCtrl::OnMouseMove( nFlags, point );

	if ( m_pTip )
	{
		int nHit = HitTest( point );

		if ( nHit >= 0 )
			m_pTip->Show( GetItemData( nHit ) );
		else
			m_pTip->Hide();
	}
}

void CTipListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( m_pTip ) m_pTip->Hide();
	CListCtrl::OnLButtonDown(nFlags, point);
}

void CTipListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if ( m_pTip ) m_pTip->Hide();
	CListCtrl::OnRButtonDown(nFlags, point);
}

void CTipListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( m_pTip ) m_pTip->Hide();
	CListCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
}
