//
// PageSettingsProtocols.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
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


class CProtocolsSettingsPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CProtocolsSettingsPage)

public:
	CProtocolsSettingsPage();
	virtual ~CProtocolsSettingsPage();

	enum { IDD = IDD_SETTINGS_PROTOCOLS };

public:
	CTreeCtrl	m_wndTree;

	HTREEITEM	AddItem(HTREEITEM hParent, LPCTSTR pszText, LPCTSTR pszValue = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();

	//DECLARE_MESSAGE_MAP()
};
