//
// WndLibrary.h
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

#include "WndPanel.h"
#include "CtrlLibraryFrame.h"

class CLibraryFile;
class CAlbumFolder;


class CLibraryWnd : public CPanelWnd
{
	DECLARE_SERIAL(CLibraryWnd)

// Construction
public:
	CLibraryWnd();
	virtual ~CLibraryWnd();

// Attributes
public:
	CLibraryFrame		m_wndFrame;
	DWORD				m_tLast;

// Operations
public:
	BOOL	Display(CLibraryFile* pFile);
	BOOL	Display(CAlbumFolder* pFolder);
	BOOL	OnCollection(LPCTSTR pszPath);
public:
	virtual void OnSkinChange();
	virtual HRESULT	GetGenericView(IGenericView** ppView);

// Overrides
public:
	//{{AFX_VIRTUAL(CLibraryWnd)
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLibraryWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
