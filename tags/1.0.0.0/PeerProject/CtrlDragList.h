//
// CtrlDragList.h
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

class CDragListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CDragListCtrl)

// Construction
public:
	CDragListCtrl();
	virtual ~CDragListCtrl();

// Attributes
protected:
	CImageList*	m_pDragImage;
	int			m_nDragDrop;
	BOOL		m_bCreateDragImage;

// Overrides
public:
	//{{AFX_VIRTUAL(CDragListCtrl)
	virtual void	OnDragDrop(int nDrop);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDragListCtrl)
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define LVN_DRAGDROP	(LVN_FIRST+1)
