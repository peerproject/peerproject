//
// CtrlSchema.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#define NO_VALUE		_T("(~ns~)")
#define MULTI_VALUE		_T("(~mt~)")

#include "Schema.h"

class CXMLElement;


class CSchemaCtrl : public CWnd
{
// Construction
public:
	CSchemaCtrl();

// Attributes
public:
	int				m_nCaptionWidth;
	int				m_nItemHeight;
	BOOL			m_bShowBorder;

protected:
	CSchemaPtr		m_pSchema;
	CArray< CWnd* >	m_pControls;
	CArray< CString >	m_pCaptions;
	int				m_nScroll;
	CString			m_sMultipleString;

// Operations
public:
	void		SetSchema(CSchemaPtr pSchema, BOOL bPromptOnly = FALSE);
	BOOL		UpdateData(CXMLElement* pBase, BOOL bSaveAndValidate);
	CString		GetSchemaURI() const;
	void		Disable();
	void		Enable();
	void		ScrollBy(int nDelta);
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

protected:
	void		Layout();

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar = NULL);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnControlSetFocus();
	afx_msg void OnControlEdit();

	DECLARE_MESSAGE_MAP()
};

#define IDC_METADATA_CONTROL	99
