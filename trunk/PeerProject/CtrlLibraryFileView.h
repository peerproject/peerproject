//
// CtrlLibraryFileView.h
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

#if !defined(CTRLLIBRARYFILEVIEW_H)
#define CTRLLIBRARYFILEVIEW_H

#pragma once

#include "CtrlLibraryView.h"

class CLibraryFile;
class CMetaPanel;

class CLibraryFileView : public CLibraryView
{
// Construction
public:
	CLibraryFileView();
	virtual ~CLibraryFileView();

	DECLARE_DYNAMIC(CLibraryFileView)

// Attributes
protected:
	POSITION			m_posSel;
	BOOL				m_bEditing;
	BOOL				m_bRequestingService;
	INT_PTR				m_nCurrentPage;
	CList<CMetaPanel*>	m_pServiceDataPages;
	BOOL				m_bServiceFailed;

protected:
	virtual BOOL		CheckAvailable(CLibraryTreeItem* pSel);
	virtual DWORD_PTR	HitTestIndex(const CPoint& point) const = 0;

protected:
	void			StartSelectedFileLoop();
	CLibraryFile*	GetNextSelectedFile();
	CLibraryFile*	GetSelectedFile();
	void			CheckDynamicBar();
	void			ClearServicePages();

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateLibraryLaunch(CCmdUI* pCmdUI);
	afx_msg void OnLibraryLaunch();
	afx_msg void OnUpdateLibraryEnqueue(CCmdUI* pCmdUI);
	afx_msg void OnLibraryEnqueue();
	afx_msg void OnUpdateLibraryURL(CCmdUI* pCmdUI);
	afx_msg void OnLibraryURL();
	afx_msg void OnUpdateLibraryMove(CCmdUI* pCmdUI);
	afx_msg void OnLibraryMove();
	afx_msg void OnUpdateLibraryCopy(CCmdUI* pCmdUI);
	afx_msg void OnLibraryCopy();
	afx_msg void OnUpdateLibraryDelete(CCmdUI* pCmdUI);
	afx_msg void OnLibraryDelete();
	afx_msg void OnUpdateLibraryBitziWeb(CCmdUI* pCmdUI);
	afx_msg void OnLibraryBitziWeb();
	afx_msg void OnUpdateLibraryBitziDownload(CCmdUI* pCmdUI);
	afx_msg void OnLibraryBitziDownload();
	afx_msg void OnUpdateLibraryRefreshMetadata(CCmdUI* pCmdUI);
	afx_msg void OnLibraryRefreshMetadata();
	afx_msg void OnUpdateLibraryShared(CCmdUI* pCmdUI);
	afx_msg void OnLibraryShared();
	afx_msg void OnUpdateLibraryProperties(CCmdUI* pCmdUI);
	afx_msg void OnLibraryProperties();
	afx_msg void OnUpdateMusicBrainzLookup(CCmdUI* pCmdUI);
	afx_msg void OnMusicBrainzLookup();
	afx_msg void OnUpdateMusicBrainzMatches(CCmdUI* pCmdUI);
	afx_msg void OnMusicBrainzMatches();
	afx_msg void OnUpdateMusicBrainzAlbums(CCmdUI* pCmdUI);
	afx_msg void OnMusicBrainzAlbums();
	afx_msg void OnUpdateShareMonkeyLookup(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeyLookup();
	afx_msg void OnUpdateShareMonkeyDownload(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeyDownload();
	afx_msg void OnUpdateShareMonkeySave(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeySave();
	afx_msg void OnUpdateShareMonkeySaveOption(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeySaveOption();
	afx_msg void OnUpdateShareMonkeyPrevious(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeyPrevious();
	afx_msg void OnUpdateShareMonkeyNext(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeyNext();
	afx_msg void OnUpdateShareMonkeyPrices(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeyPrices();
	afx_msg void OnUpdateShareMonkeyCompare(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeyCompare();
	afx_msg void OnUpdateShareMonkeyBuy(CCmdUI* pCmdUI);
	afx_msg void OnShareMonkeyBuy();
	afx_msg void OnUpdateLibraryUnlink(CCmdUI* pCmdUI);
	afx_msg void OnLibraryUnlink();
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
	afx_msg void OnUpdateLibraryCreateTorrent(CCmdUI* pCmdUI);
	afx_msg void OnLibraryCreateTorrent();
	afx_msg void OnUpdateLibraryRebuildAnsi(CCmdUI* pCmdUI);
	afx_msg void OnLibraryRebuildAnsi();
	afx_msg LRESULT OnServiceDone(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(CTRLLIBRARYFILEVIEW_H)
