//
// WndSearch.h
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

#include "WndBaseMatch.h"
#include "CtrlSearchPanel.h"
#include "CtrlSearchDetailPanel.h"
#include "QuerySearch.h"

class CManagedSearch;


class CSearchWnd : public CBaseMatchWnd
{
public:
	CSearchWnd(auto_ptr< CQuerySearch > pSearch = auto_ptr< CQuerySearch >());
	virtual ~CSearchWnd();

	DECLARE_DYNCREATE(CSearchWnd)
	friend class CRemote;

// Attributes
private:
	CSearchPanel		m_wndPanel;
	BOOL				m_bPanel;
	BOOL				m_bSetFocus;
	CSearchDetailPanel	m_wndDetails;
	BOOL				m_bDetails;
	int					m_nDetails;
	typedef boost::ptr_list< CManagedSearch > List;
	List				m_oSearches;
public:
	typedef List::iterator iterator;
	typedef List::const_iterator const_iterator;
	typedef List::reverse_iterator reverse_iterator;
	typedef List::const_reverse_iterator const_reverse_iterator;

	iterator               begin()        { return m_oSearches.begin(); }
	const_iterator         begin()  const { return m_oSearches.begin(); }
	iterator               end()          { return m_oSearches.end(); }
	const_iterator         end()    const { return m_oSearches.end(); }
	reverse_iterator       rbegin()       { return m_oSearches.rbegin(); }
	const_reverse_iterator rbegin() const { return m_oSearches.rbegin(); }
	reverse_iterator       rend()         { return m_oSearches.rend(); }
	const_reverse_iterator rend()   const { return m_oSearches.rend(); }

	size_t size() const { return m_oSearches.size(); }
	bool empty() const { return m_oSearches.empty(); }

	DWORD				m_tSearch;
	DWORD				m_nCacheHits;
	DWORD				m_nCacheHubs;
	DWORD				m_nCacheLeaves;
	CString				m_sCaption;
	BOOL				m_bWaitMore;
	DWORD				m_nMaxResults;
	DWORD				m_nMaxED2KResults;
	DWORD				m_nMaxQueryCount;
	
// Operations
public:
	void			Serialize(CArchive& ar);
	CManagedSearch*	GetLastManager();
	CQuerySearch*	GetLastSearch();
	void			ExecuteSearch();
protected:
	BOOL			DoSizeDetails();
public:	
	virtual void	OnSkinChange();
	virtual BOOL	OnQueryHits(CQueryHit* pHits);
	virtual void	UpdateMessages(BOOL bActive = TRUE);
	virtual void	UpdateMessages(BOOL bActive, CManagedSearch* pManaged);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSelChangeMatches();
	afx_msg void OnUpdateSearchSearch(CCmdUI* pCmdUI);
	afx_msg void OnSearchSearch();
	afx_msg void OnSearchClear();
	afx_msg void OnUpdateSearchStop(CCmdUI* pCmdUI);
	afx_msg void OnSearchStop();
	afx_msg void OnUpdateSearchPanel(CCmdUI* pCmdUI);
	afx_msg void OnSearchPanel();
	afx_msg void OnUpdateSearchClear(CCmdUI* pCmdUI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnUpdateSearchDetails(CCmdUI* pCmdUI);
	afx_msg void OnSearchDetails();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnUpdateFilters(CCmdUI* pCmdUI);
	afx_msg void OnFilters(UINT nID);
};
