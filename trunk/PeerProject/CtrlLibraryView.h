//
// CtrlLibraryView.h
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

#if !defined(AFX_CTRLLIBRARYVIEW_H__9FA28C5B_A6A3_4616_9407_4661EA1C7B8A__INCLUDED_)
#define AFX_CTRLLIBRARYVIEW_H__9FA28C5B_A6A3_4616_9407_4661EA1C7B8A__INCLUDED_

#pragma once

#include "PeerProjectDataSource.h"

class CAlbumFolder;
class CLibraryFrame;
class CLibraryTreeItem;
class CLibraryTipCtrl;


class CLibraryView : public CWnd
{
// Construction
public:
	CLibraryView();
	virtual ~CLibraryView();

	DECLARE_DYNAMIC(CLibraryView)

// Attributes
public:
	UINT				m_nCommandID;
	LPCTSTR				m_pszToolBar;
	BOOL				m_bAvailable;
	BOOL				m_bGhostFolder;
	CLibraryList		m_pSelection;
	CLibraryListItem	m_oDropItem;

// Operations
public:
	virtual BOOL				CheckAvailable(CLibraryTreeItem* pSel);
	virtual void				GetHeaderContent(int& nImage, CString& strTitle);
	virtual void				Update();
	virtual BOOL				Select(DWORD nObject);
	virtual void				CacheSelection();
	virtual CLibraryListItem	DropHitTest(const CPoint& point);
	virtual CLibraryListItem	GetFolder() const;
	virtual void				StartDragging(const CPoint& ptMouse);
	virtual HBITMAP				CreateDragImage(const CPoint& ptMouse, CPoint& ptMiddle);

protected:
	void				PostUpdate();
	CLibraryFrame*		GetFrame() const;
	CLibraryTipCtrl*	GetToolTip() const;
	DWORD				GetFolderCookie() const;
	CLibraryTreeItem*	GetFolderSelection() const;
	CAlbumFolder*		GetSelectedAlbum(CLibraryTreeItem* pSel = NULL) const;

protected:
	BOOL	SelAdd(CLibraryListItem oObject, BOOL bNotify = TRUE);
	BOOL	SelRemove(CLibraryListItem oObject, BOOL bNotify = TRUE);
	BOOL	SelClear(BOOL bNotify = TRUE);
	INT_PTR	GetSelectedCount() const;

// Overrides
public:
	//{{AFX_VIRTUAL(CLibraryView)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLibraryView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DROP()
};

//{{AFX_INSERT_LOCATION}}

#define IDC_LIBRARY_VIEW	132

#endif // !defined(AFX_CTRLLIBRARYVIEW_H__9FA28C5B_A6A3_4616_9407_4661EA1C7B8A__INCLUDED_)
