//
// CtrlLibraryMetaPanel.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "CtrlPanelClass.h"
#include "MetaPanel.h"

class CSchema;
class CLibraryFile;


class CLibraryMetaPanel :
	public CPanelCtrl,
	public CThreadImpl
{
	DECLARE_DYNCREATE(CLibraryMetaPanel)

public:
	CLibraryMetaPanel();
	virtual ~CLibraryMetaPanel();

	virtual void Update();

	BOOL		SetServicePanel(CMetaPanel* pPanel);
	CMetaPanel*	GetServicePanel();

protected:
	int				m_nSelected;
	DWORD			m_nIndex;
	BOOL			m_bNewFile; 		// Flag used to switch off thread if thumbnail cannot be extracted
	CString			m_sName;
	CString			m_sPath;			// Current file path
	CString			m_sFolder;
	CString			m_sType;
	CString			m_sSize;
	int				m_nIcon32;
	int				m_nIcon48;
	int				m_nRating;
	CSchema*		m_pSchema;
	CMetaPanel*		m_pMetadata;
	CMetaPanel*		m_pServiceData;
	CCriticalSection m_pSection;
	CRect			m_rcFolder;
	CRect			m_rcRating;
	BOOL			m_bForceUpdate;
	CString			m_sThumbnailURL;	// Use this URL to load thumbnail instead
	CString			m_sThumb;			// Loaded thumbnail file path or URL
	CBitmap			m_bmThumb;

	CLibraryList*	GetViewSelection();

	void	DrawText(CDC* pDC, int nX, int nY, LPCTSTR pszText, RECT* pRect = NULL, int nMaxWidth = -1);

	void	OnRun();

	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
