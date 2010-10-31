//
// CtrlHomeSearch.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
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

#include "CtrlSchemaCombo.h"
#include "CtrlIconButton.h"

class CHomeSearchCtrl : public CWnd
{
	DECLARE_DYNCREATE(CHomeSearchCtrl)

public:
	CHomeSearchCtrl();

	void	OnSkinChange(COLORREF crWindow, COLORREF crText = 0);
	void	Activate();

	virtual BOOL Create(CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CComboBox		m_wndText;
	CSchemaCombo	m_wndSchema;
	CIconButtonCtrl	m_wndSearch;
	CIconButtonCtrl	m_wndAdvanced;
	COLORREF		m_crWindow;
	COLORREF		m_crText;

	void	FillHistory();
	void	Search(bool bAutostart);

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnCloseUpText();
	afx_msg void OnSelChangeText();
	afx_msg void OnSearchStart();
	afx_msg void OnSearchAdvanced();

	DECLARE_MESSAGE_MAP()
};
