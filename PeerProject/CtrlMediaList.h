//
// CtrlMediaList.h
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

#include "CtrlLibraryTip.h"


class CMediaListCtrl : public CListCtrl
{
// Construction
public:
	CMediaListCtrl();
	virtual ~CMediaListCtrl();

	DECLARE_DYNAMIC(CMediaListCtrl)

// Attributes
protected:
	CImageList*	m_pDragImage;
	int			m_nDragDrop;
	BOOL		m_bCreateDragImage;
protected:
	CLibraryTipCtrl	m_wndTip;

// Operations
protected:
	int		Add(LPCTSTR pszPath, int nItem = -1);
	void	Remove(int nItem);
public:
	BOOL	Open(LPCTSTR pszFile);
	BOOL	Enqueue(LPCTSTR pszFile, BOOL bStart = TRUE);
	int		RecursiveEnqueue(LPCTSTR pszPath);
	void	Remove(LPCTSTR pszFile);
	BOOL	LoadTextList(LPCTSTR pszFile);
	int		GetCount();
	void	Clear();
	int		GetCurrent();
	void	SetCurrent(int nCurrent);
	int		GetNext(BOOL bSet = TRUE);
	void	Reset(BOOL bNext = TRUE);
	CString	GetPath(int nItem);
	void	OnSkinChange();
private:
	BOOL	AreSelectedFilesInLibrary();
	void	ShowFilePropertiesDlg(int nPage = 0 );
// Overrides
public:
	//{{AFX_VIRTUAL(CMediaListCtrl)
	public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMediaListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnMediaAdd();
	afx_msg void OnUpdateMediaRate(CCmdUI* pCmdUI);
	afx_msg void OnMediaRate();
	afx_msg void OnUpdateMediaProperties(CCmdUI* pCmdUI);
	afx_msg void OnMediaProperties();
	afx_msg void OnUpdateMediaRemove(CCmdUI* pCmdUI);
	afx_msg void OnMediaRemove();
	afx_msg void OnUpdateMediaClear(CCmdUI* pCmdUI);
	afx_msg void OnMediaClear();
	afx_msg void OnUpdateMediaSelect(CCmdUI* pCmdUI);
	afx_msg void OnMediaSelect();
	afx_msg void OnUpdateMediaSave(CCmdUI* pCmdUI);
	afx_msg void OnMediaSave();
	afx_msg void OnMediaOpen();
	afx_msg void OnUpdateMediaPrevious(CCmdUI* pCmdUI);
	afx_msg void OnMediaPrevious();
	afx_msg void OnUpdateMediaNext(CCmdUI* pCmdUI);
	afx_msg void OnMediaNext();
	afx_msg void OnUpdateMediaRepeat(CCmdUI* pCmdUI);
	afx_msg void OnMediaRepeat();
	afx_msg void OnUpdateMediaRandom(CCmdUI* pCmdUI);
	afx_msg void OnMediaRandom();
	afx_msg void OnUpdateMediaCollection(CCmdUI* pCmdUI);
	afx_msg void OnMediaCollection();
	afx_msg void OnMediaAddFolder();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define MLN_NEWCURRENT	101