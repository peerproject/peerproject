//
// PageProfileFavourites.h
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

#if !defined(AFX_PAGEPROFILEFAVOURITES_H__B39399C9_17F5_4985_8A00_06A13705C8FD__INCLUDED_)
#define AFX_PAGEPROFILEFAVOURITES_H__B39399C9_17F5_4985_8A00_06A13705C8FD__INCLUDED_

#pragma once

#include "WndSettingsPage.h"


class CFavouritesProfilePage : public CSettingsPage
{
// Construction
public:
	CFavouritesProfilePage();
	virtual ~CFavouritesProfilePage();

	DECLARE_DYNCREATE(CFavouritesProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CFavouritesProfilePage)
	enum { IDD = IDD_PROFILE_FAVOURITES };
	CButton	m_wndRemove;
	CButton	m_wndAdd;
	CListCtrl	m_wndList;
	CString	m_sURL;
	CString	m_sTitle;
	//}}AFX_DATA

	CImageList	m_gdiImageList;

// Overrides
public:
	//{{AFX_VIRTUAL(CFavouritesProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFavouritesProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeWebName();
	afx_msg void OnChangeWebUrl();
	afx_msg void OnWebAdd();
	afx_msg void OnWebRemove();
	afx_msg void OnItemChangedWebList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEPROFILEFAVOURITES_H__B39399C9_17F5_4985_8A00_06A13705C8FD__INCLUDED_)
