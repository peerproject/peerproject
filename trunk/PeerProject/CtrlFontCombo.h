//
// CtrlFontCombo.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
// Portions copyright Shareaza Development Team, 2005-2007.
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
//  Created by:		Rolandas
//	Date:			"$Date: 2005/08/08 03:47:40 $"
//	Revision:		"$Revision: 1.1.2.1 $"
//  Last change by:	"$Author: rolandas $"
//

#pragma once

class CFontCombo : public CComboBox
{
	DECLARE_DYNAMIC(CFontCombo)

// Construction
public:
	CFontCombo();
//	virtual ~CFontCombo();

// Attributes
public:
	CString	m_sSelectedFont;
protected:
	CImageList m_pImages;
	CMapStringToPtr m_pFonts;
	int 	m_nFontHeight;

// Operations
public:
	void	Initialize();
	void	SelectFont(const CString& strFontName);
	CString	GetSelectedFont() const;
	int 	GetFontHeight() const;
	void	SetFontHeight(int nNewHeight, BOOL bReinitialize = TRUE);

protected:
	BOOL	AddFont(const CString& strFontName);
	void	DeleteAllFonts();
	static BOOL CALLBACK EnumFontProc(LPENUMLOGFONTEX lplf, NEWTEXTMETRICEX* lpntm, DWORD dwFontType, LPVOID lpData);

public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//{{AFX_MSG(CFontCombo)
	afx_msg void OnDropdown();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnOcmDrawItem(WPARAM /*wParam*/, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

void PASCAL DDX_FontCombo(CDataExchange* pDX, int nIDC, CString& strFontName);
