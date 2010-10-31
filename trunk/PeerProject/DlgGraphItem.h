//
// DlgGraphItem.h
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

#include "DlgSkinDialog.h"

class CGraphItem;


class CGraphItemDlg : public CSkinDialog
{
// Construction
public:
	CGraphItemDlg(CWnd* pParent = NULL, CGraphItem* pItem = NULL);

// Dialog Data
public:
	//{{AFX_DATA(CGraphItemDlg)
	enum { IDD = IDD_GRAPH_ITEM };
	CButton	m_wndOK;
	CEdit	m_wndUnits;
	CComboBox	m_wndSource;
	CButton	m_wndRemove;
	float m_nMultiplier;
	CStatic	m_wndColorBox;
	//}}AFX_DATA

	CGraphItem*		m_pItem;
	COLORREF		m_crColor;
	CImageList		m_gdiImageList;

// Overrides
public:
	//{{AFX_VIRTUAL(CGraphItemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGraphItemDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelChangeGraphSource();
	afx_msg void OnGraphColor();
	afx_msg void OnPaint();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

void PASCAL DDX_Float(CDataExchange* pDX, int nIDC, float& nValue);
