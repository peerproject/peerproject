//
// CtrlHomeView.h
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

#pragma once

#include "RichViewCtrl.h"
#include "RichDocument.h"
#include "CtrlHomeSearch.h"


class CHomeViewCtrl : public CRichViewCtrl
{
	DECLARE_DYNCREATE(CHomeViewCtrl)

public:
	CHomeViewCtrl();

	CRichDocument	m_pDocument;
	CRichElement*	m_peHeader;
	CRichElement*	m_peSearch;
	CRichElement*	m_peUpgrade;
	CRichElement*	m_peRemote1;
	CRichElement*	m_peRemote2;
	CHomeSearchCtrl	m_wndSearch;
	CBitmap			m_bmHeader1;
	CBitmap			m_bmHeader2;

	virtual BOOL	Create(const RECT& rect, CWnd* pParentWnd);
	void			OnSkinChange();
	void			Activate();
	void			Update();

protected:
	virtual void	OnLayoutComplete();
	virtual void	OnPaintBegin(CDC* pDC);
	virtual void	OnVScrolled();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

#define IDC_HOME_VIEW		150
#define IDC_HOME_SEARCH		151
