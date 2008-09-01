//
// CtrlSchema.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

#if !defined(AFX_CTRLSCHEMA_H__98487F51_E425_4B0D_BC82_BCA8A7F5D952__INCLUDED_)
#define AFX_CTRLSCHEMA_H__98487F51_E425_4B0D_BC82_BCA8A7F5D952__INCLUDED_

#pragma once

#define NO_VALUE		(_T("(~ns~)"))
#define MULTI_VALUE		(_T("(~mt~)"))

class CSchema;
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
	CSchema*		m_pSchema;
	CArray< CWnd* >	m_pControls;
	CArray< CString >	m_pCaptions;
	int				m_nScroll;
	int				m_nScrollWheelLines;
	CString			strMultipleString;

// Operations
public:
	void		SetSchema(CSchema* pSchema, BOOL bPromptOnly = FALSE);
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
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnControlSetFocus();
	afx_msg void OnControlEdit();

	DECLARE_MESSAGE_MAP()
};

#define IDC_METADATA_CONTROL	99

#endif // !defined(AFX_CTRLSCHEMA_H__98487F51_E425_4B0D_BC82_BCA8A7F5D952__INCLUDED_)
