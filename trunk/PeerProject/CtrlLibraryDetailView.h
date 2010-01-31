//
// CtrlLibraryDetailView.h
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

#pragma once

#include "CtrlLibraryFileView.h"
#include "Schema.h"

class CLibraryFile;


class CLibraryDetailView : public CLibraryFileView
{
	DECLARE_DYNCREATE(CLibraryDetailView)

public:
	CLibraryDetailView(UINT nCommandID = ID_LIBRARY_VIEW_DETAIL);

protected:
	virtual void			Update();
	virtual BOOL			Select(DWORD nObject);
	virtual void			CacheSelection();
	virtual DWORD_PTR		HitTestIndex(const CPoint& point) const;
	virtual HBITMAP			CreateDragImage(const CPoint& ptMouse, CPoint& ptOffset);
	virtual void			OnSkinChange();
	void	SetViewSchema(CSchemaPtr pSchema, CList< CSchemaMember* >* pColumns, BOOL bSave, BOOL bUpdate);
	void	CacheItem(int nItem);
	void	SortItems(int nColumn = -1);

protected:
	UINT		m_nStyle;
	CSchemaPtr	m_pSchema;
	CList< CSchemaMember* >	m_pColumns;
	CCoolMenu*	m_pCoolMenu;
	BOOL		m_bCreateDragImage;

	struct LDVITEM
	{
		DWORD			nIndex;
		DWORD			nCookie;
		DWORD			nState;
		int				nIcon;
		CArray< CString >*	pText;
	};

	LDVITEM*	m_pList;
	DWORD		m_nList;
	DWORD		m_nBuffer;
	DWORD		m_nListCookie;
	int			m_nSortColumn;
	BOOL		m_bSortFlip;

	static int ListCompare(LPCVOID pA, LPCVOID pB);
	static CLibraryDetailView* m_pThis;

	virtual BOOL Create(CWnd* pParentWnd);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnUpdateLibraryRename(CCmdUI* pCmdUI);
	afx_msg void OnLibraryRename();
	afx_msg void OnLibraryColumns();
	afx_msg void OnUpdateLibraryColumns(CCmdUI* pCmdUI);
	afx_msg void OnCacheHint(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnGetDispInfoW(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnGetDispInfoA(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnEndLabelEditW(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnEndLabelEditA(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnItemRangeChanged(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnFindItemW(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnFindItemA(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnDblClk(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnUpdateBlocker(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};


class CLibraryListView : public CLibraryDetailView
{
public:
	CLibraryListView() : CLibraryDetailView( ID_LIBRARY_VIEW_LIST ) {}
	DECLARE_DYNCREATE(CLibraryListView);
};


class CLibraryIconView : public CLibraryDetailView
{
public:
	CLibraryIconView() : CLibraryDetailView( ID_LIBRARY_VIEW_ICON ) {}
	DECLARE_DYNCREATE(CLibraryIconView);
};


#define LDVI_SELECTED	0x01
#define LDVI_PRIVATE	0x02
#define LDVI_UNSCANNED	0x04
#define LDVI_UNSAFE		0x08
