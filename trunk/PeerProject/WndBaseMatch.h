//
// WndBaseMatch.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#include "WndPanel.h"
#include "CtrlMatch.h"
#include "AutocompleteEdit.h"

class CMatchList;
class CCoolMenu;


class CBaseMatchWnd : public CPanelWnd
{
// Construction
public:
	CBaseMatchWnd();
	virtual ~CBaseMatchWnd();

	DECLARE_DYNCREATE(CBaseMatchWnd)

// Attributes
public:
	CMatchCtrl		m_wndList;
	CCoolBarCtrl	m_wndToolBar;
	CAutocompleteEdit	m_wndFilter;

protected:
	CMatchList*		m_pMatches;
	CCoolMenu*		m_pCoolMenu;
	BOOL			m_bContextMenu;
	DWORD			m_tContextMenu;
	BOOL			m_bPaused;
	BOOL			m_bUpdate;
	BOOL			m_bBMWActive;
	DWORD			m_nCacheFiles;
	DWORD			m_tModify;			// Last modify time (0 if not modified)

// Operations
public:
	void			Serialize(CArchive& ar);

	inline BOOL		IsPaused() const
	{
		return m_bPaused;
	}

	inline void		SetModified()
	{
		if ( ! m_tModify )
			m_tModify = static_cast< DWORD >( time( NULL ) );
	}

	virtual BOOL	OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void	SanityCheck();
	virtual void	UpdateMessages(BOOL bActive = TRUE);
	virtual HRESULT	GetGenericView(IGenericView** ppView);

// Implementation
protected:
	virtual void	OnSkinChange();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnUpdateSearchDownload(CCmdUI* pCmdUI);
	afx_msg void OnSearchDownload();
	afx_msg void OnUpdateSearchDownloadNow(CCmdUI* pCmdUI);
	afx_msg void OnSearchDownloadNow();
	afx_msg void OnUpdateSearchURI(CCmdUI* pCmdUI);
	afx_msg void OnSearchURI();
	afx_msg void OnUpdateSearchChat(CCmdUI* pCmdUI);
	afx_msg void OnSearchChat();
	afx_msg void OnUpdateSearchFilter(CCmdUI* pCmdUI);
	afx_msg void OnSearchFilter();
	afx_msg void OnUpdateSearchFilterRemove(CCmdUI* pCmdUI);
	afx_msg void OnSearchFilterRemove();
	afx_msg void OnSearchColumns();
	afx_msg void OnUpdateLibraryBitziWeb(CCmdUI* pCmdUI);
	afx_msg void OnLibraryBitziWeb();
	afx_msg void OnUpdateSecurityBan(CCmdUI* pCmdUI);
	afx_msg void OnSecurityBan();
	afx_msg void OnUpdateHitMonitorSearch(CCmdUI* pCmdUI);
	afx_msg void OnHitMonitorSearch();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnUpdateBrowseLaunch(CCmdUI* pCmdUI);
	afx_msg void OnBrowseLaunch();
	afx_msg void OnSearchFilterRaw();
	afx_msg void OnUpdateSearchForThis(CCmdUI* pCmdUI);
	afx_msg void OnSearchForThis();
	afx_msg void OnUpdateSearchForSimilar(CCmdUI* pCmdUI);
	afx_msg void OnSearchForSimilar();
	afx_msg void OnUpdateSearchForArtist(CCmdUI* pCmdUI);
	afx_msg void OnSearchForArtist();
	afx_msg void OnUpdateSearchForAlbum(CCmdUI* pCmdUI);
	afx_msg void OnSearchForAlbum();
	afx_msg void OnUpdateSearchForSeries(CCmdUI* pCmdUI);
	afx_msg void OnSearchForSeries();
	//afx_msg void OnUpdateLibraryJigle(CCmdUI* pCmdUI);
	//afx_msg void OnLibraryJigle();
	afx_msg void OnKillFocusFilter();
	afx_msg void OnToolbarReturn();
	afx_msg void OnToolbarEscape();
	afx_msg void OnUpdateBlocker(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilters(CCmdUI* pCmdUI);
	afx_msg void OnFilters(UINT nID);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};