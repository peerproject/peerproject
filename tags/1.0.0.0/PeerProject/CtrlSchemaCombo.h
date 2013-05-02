//
// CtrlSchemaCombo.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

#include "Schema.h"


class CSchemaCombo : public CComboBox
{
// Construction
public:
	CSchemaCombo();

// Attributes
public:
	CString		m_sNoSchemaText;
	int			m_nType;
	int			m_nAvailability;
protected:
	HWND		m_hListBox;
	WNDPROC		m_pWndProc;
	CString		m_sPreDrop;

// Operations
public:
	void		SetEmptyString(UINT nID);
	void		Load(LPCTSTR pszSelectURI = NULL, int nType = 0, int nAvailability = 0, BOOL bReset = TRUE);
	void		Select(LPCTSTR pszURI);
	void		Select(CSchemaPtr pSchema);
	CSchemaPtr	GetSelected() const;
	CString		GetSelectedURI() const;
protected:
	static LRESULT PASCAL ListWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	int			FindSchema(CSchemaPtr pSchema);
	BOOL		OnClickItem(int nItem, BOOL bDown);

// Overrides
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDropDown();

	DECLARE_MESSAGE_MAP()
};
