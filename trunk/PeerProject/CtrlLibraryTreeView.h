//
// CtrlLibraryTreeView.h
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

#pragma once

#include "CtrlTipFolder.h"
#include "CtrlTipAlbum.h"
#include "PeerProjectDataSource.h"

class CLibraryTreeItem;
class CCoolTipCtrl;
class CLibraryFolder;
class CAlbumFolder;

class CLibraryTreeView : public CWnd
{
// Construction
public:
	CLibraryTreeView();
	virtual ~CLibraryTreeView();

	DECLARE_DYNAMIC(CLibraryTreeView)

// Attributes
private:
	CLibraryTreeItem*	m_pRoot;
	size_t				m_nTotal;
	int					m_nVisible;
	int					m_nScroll;
	int					m_nSelected;
	CLibraryTreeItem*	m_pSelFirst;
	CLibraryTreeItem*	m_pSelLast;
	CLibraryTreeItem*	m_pFocus;
	BOOL				m_bDrag;
	CPoint				m_ptDrag;
	CLibraryTreeItem*	m_pDropItem;
	DWORD				m_nCleanCookie;
	CCoolTipCtrl*		m_pTip;
	BOOL				m_bVirtual;
	CFolderTipCtrl		m_wndFolderTip;
	CAlbumTipCtrl		m_wndAlbumTip;

// Operations
public:
	void				SetToolTip(CCoolTipCtrl* pTip);
	BOOL				Expand(CLibraryTreeItem* pItem, TRISTATE bExpand = TRI_TRUE, BOOL bInvalidate = TRUE);
	BOOL				Select(CLibraryTreeItem* pItem, TRISTATE bSelect = TRI_TRUE, BOOL bInvalidate = TRUE);
	BOOL				SelectAll(CLibraryTreeItem* pParent = NULL, BOOL bInvalidate = TRUE);
	BOOL				DeselectAll(CLibraryTreeItem* pExcept = NULL, CLibraryTreeItem* pParent = NULL, BOOL bInvalidate = TRUE);
	BOOL				Highlight(CLibraryTreeItem* pItem);
	int					GetSelectedCount() const;
	CLibraryTreeItem*	GetFirstSelected() const;
	CLibraryTreeItem*	GetLastSelected() const;
	CLibraryTreeItem*	HitTest(const POINT& point, RECT* pRect = NULL) const;
	BOOL				GetRect(CLibraryTreeItem* pItem, RECT* pRect);
	CLibraryTreeItem*	GetFolderItem(void* pSearch, CLibraryTreeItem* pParent = NULL);
	void				SetVirtual(BOOL bVirtual);
	void				Update(DWORD nSelectCookie);
	BOOL				SelectFolder(LPVOID pSearch);

protected:
	void				Clear();
	void				UpdateScroll();
	void				ScrollBy(int nDelta);
	void				ScrollTo(int nPosition);
	void				Paint(CDC& dc, CRect& rcClient, CPoint& pt, CLibraryTreeItem* pItem);
	CLibraryTreeItem*	HitTest(CRect& rcClient, CPoint& pt, CLibraryTreeItem* pItem, const POINT& point, RECT* pRect) const;
	BOOL				GetRect(CPoint& pt, CLibraryTreeItem* pItem, CLibraryTreeItem* pFind, RECT* pRect);
	BOOL				CleanItems(CLibraryTreeItem* pItem, DWORD nCookie, BOOL bVisible);
	BOOL				CollapseRecursive(CLibraryTreeItem* pItem);
	void				NotifySelection();
	virtual HBITMAP		CreateDragImage(const CPoint& ptMouse, CPoint& ptMiddle);
	void				StartDragging(CPoint& ptMouse);
	void				PostUpdate();
	void				UpdatePhysical(DWORD nSelectCookie);
	void				UpdateVirtual(DWORD nSelectCookie);
	BOOL				Update(CLibraryFolder* pFolder, CLibraryTreeItem* pItem, CLibraryTreeItem* pParent, BOOL bVisible, BOOL bShared, DWORD nCleanCookie, DWORD nSelectCookie, BOOL bRecurse);
	BOOL				Update(CAlbumFolder* pFolder, CLibraryTreeItem* pItem, CLibraryTreeItem* pParent, BOOL bVisible, DWORD nCleanCookie, DWORD nSelectCookie);

// Overrides
public:
	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);


// Implementation
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateLibraryParent(CCmdUI* pCmdUI);
	afx_msg void OnLibraryParent();
	afx_msg void OnUpdateLibraryExplore(CCmdUI* pCmdUI);
	afx_msg void OnLibraryExplore();
	afx_msg void OnUpdateLibraryScan(CCmdUI* pCmdUI);
	afx_msg void OnLibraryScan();
	afx_msg void OnUpdateLibraryShared(CCmdUI* pCmdUI);
	afx_msg void OnLibraryShared();
	afx_msg void OnLibraryAdd();
	afx_msg void OnUpdateLibraryRemove(CCmdUI* pCmdUI);
	afx_msg void OnLibraryRemove();
	afx_msg void OnUpdateLibraryFolderProperties(CCmdUI* pCmdUI);
	afx_msg void OnLibraryFolderProperties();
	afx_msg void OnUpdateLibraryFolderNew(CCmdUI* pCmdUI);
	afx_msg void OnLibraryFolderNew();
	afx_msg void OnUpdateLibraryFolderDelete(CCmdUI* pCmdUI);
	afx_msg void OnLibraryFolderDelete();
	afx_msg void OnUpdateLibraryFolderMetadata(CCmdUI* pCmdUI);
	afx_msg void OnLibraryFolderMetadata();
	afx_msg void OnUpdateLibraryFolderEnqueue(CCmdUI* pCmdUI);
	afx_msg void OnLibraryFolderEnqueue();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateLibraryFolderFileProperties(CCmdUI* pCmdUI);
	afx_msg void OnLibraryFolderFileProperties();
	afx_msg void OnUpdateLibraryRebuild(CCmdUI* pCmdUI);
	afx_msg void OnLibraryRebuild();
	afx_msg void OnUpdateLibraryExportCollection(CCmdUI *pCmdUI);
	afx_msg void OnLibraryExportCollection();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
	DECLARE_DROP()
};


class CLibraryTreeItem : public CObject
{
public:
	CLibraryTreeItem(CLibraryTreeItem* pParent = NULL, const CString& name = CString());

protected:
	CLibraryTreeItem* const m_pParent;
	typedef boost::ptr_list< CLibraryTreeItem > Container;
	Container m_oList;

public:
	typedef Container::iterator iterator;
	typedef Container::const_iterator const_iterator;
	typedef Container::reverse_iterator reverse_iterator;
	typedef Container::const_reverse_iterator const_reverse_iterator;

	CLibraryTreeItem*       parent()       { return m_pParent; }
	const CLibraryTreeItem* parent() const { return m_pParent; }

	iterator               begin()        { return m_oList.begin(); }
	const_iterator         begin()  const { return m_oList.begin(); }
	iterator               end()          { return m_oList.end(); }
	const_iterator         end()    const { return m_oList.end(); }
	reverse_iterator       rbegin()       { return m_oList.rbegin(); }
	const_reverse_iterator rbegin() const { return m_oList.rbegin(); }
	reverse_iterator       rend()         { return m_oList.rend(); }
	const_reverse_iterator rend()   const { return m_oList.rend(); }

	size_t size() const { return m_oList.size(); }
	size_t treeSize() const
	{
		size_t result = size();
		for ( const_iterator i = begin(); i != end(); ++i )
		{
			result += i->treeSize();
		}
		return result;
	}
	bool empty() const { return m_oList.empty(); }
	void clear() { m_oList.clear(); }
	CLibraryTreeItem* addItem(const CString& name);
	iterator erase(iterator item) { return m_oList.erase( item ); }

public:
	CLibraryTreeItem*	m_pSelPrev;
	CLibraryTreeItem*	m_pSelNext;
	DWORD				m_nCleanCookie;
	BOOL				m_bExpanded;
	BOOL				m_bSelected;
	BOOL				m_bContract1;
	BOOL				m_bContract2;
	CLibraryFolder*		m_pPhysical;
	CAlbumFolder*		m_pVirtual;
	DWORD				m_nCookie;
	CString				m_sText;
	BOOL				m_bBold;
	BOOL				m_bShared;
	BOOL				m_bCollection;
	int					m_nIcon16;

	BOOL				IsVisible() const;
	void				Paint(CDC& dc, CRect& rc, BOOL bTarget, COLORREF crBack = CLR_NONE) const;
	int					GetFileList(CLibraryList* pList, BOOL bRecursive = FALSE) const;
};

#define IDC_LIBRARY_TREE	131
#define LTN_SELCHANGED		101
