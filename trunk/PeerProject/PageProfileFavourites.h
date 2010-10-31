//
// PageProfileFavourites.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "WndSettingsPage.h"


class CFavouritesProfilePage : public CSettingsPage
{
// Construction
public:
	CFavouritesProfilePage();
	virtual ~CFavouritesProfilePage();

	DECLARE_DYNCREATE(CFavouritesProfilePage)

	enum { IDD = IDD_PROFILE_FAVOURITES };

// Dialog Data
public:
	//{{AFX_DATA(CFavouritesProfilePage)
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
	virtual void DoDataExchange(CDataExchange* pDX);
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
