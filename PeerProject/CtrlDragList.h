//
// CtrlDragList.h
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

#if !defined(AFX_CTRLDRAGLIST_H__34DF1A23_88C6_4F76_91DF_575F07A37F2E__INCLUDED_)
#define AFX_CTRLDRAGLIST_H__34DF1A23_88C6_4F76_91DF_575F07A37F2E__INCLUDED_

#pragma once


class CDragListCtrl : public CListCtrl
{
// Construction
public:
	CDragListCtrl();
	virtual ~CDragListCtrl();

	DECLARE_DYNAMIC(CDragListCtrl)

// Attributes
protected:
	CImageList*	m_pDragImage;
	int			m_nDragDrop;
	BOOL		m_bCreateDragImage;

// Operations
public:
	virtual void	OnDragDrop(int nDrop);

// Overrides
public:
	//{{AFX_VIRTUAL(CDragListCtrl)
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

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CTRLDRAGLIST_H__34DF1A23_88C6_4F76_91DF_575F07A37F2E__INCLUDED_)
