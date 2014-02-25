//
// PageProfileFavourites.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2014
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

#include "WndSettingsPage.h"


class CFavouritesProfilePage : public CSettingsPage
{
	DECLARE_DYNCREATE(CFavouritesProfilePage)

public:
	CFavouritesProfilePage();
	virtual ~CFavouritesProfilePage();

	enum { IDD = IDD_PROFILE_FAVOURITES };

public:
	CButton	m_wndRemove;
	CButton	m_wndAdd;
	CListCtrl m_wndList;
	CString	m_sURL;
	CString	m_sTitle;

	CImageList	m_gdiImageList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeWebName();
	afx_msg void OnChangeWebUrl();
	afx_msg void OnWebAdd();
	afx_msg void OnWebRemove();
	afx_msg void OnItemChangedWebList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};
