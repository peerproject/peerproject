//
// DlgGraphList.h
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

class CLiveItem;
class CLineGraph;
class CGraphItem;


class CGraphListDlg : public CSkinDialog
{
// Construction
public:
	CGraphListDlg(CWnd* pParent = NULL, CLineGraph* pGraph = NULL);

// Dialog Data
public:
	//{{AFX_DATA(CGraphListDlg)
	enum { IDD = IDD_GRAPH_LIST };
	CButton	m_wndCancel;
	CButton	m_wndOK;
	CSpinButtonCtrl	m_wndSpeed;
	CButton	m_wndRemove;
	CButton	m_wndEdit;
	CListCtrl	m_wndList;
	DWORD	m_nSpeed;
	BOOL	m_bShowGrid;
	BOOL	m_bShowAxis;
	BOOL	m_bShowLegend;
	CString	m_sName;
	//}}AFX_DATA

	CLineGraph*	m_pGraph;
	CImageList	m_gdiImageList;

	CLiveItem*	PrepareItem(CGraphItem* pItem);
	void		SetModified();

// Overrides
public:
	//{{AFX_VIRTUAL(CGraphListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGraphListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemChangedGraphItems(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGraphAdd();
	afx_msg void OnGraphEdit();
	afx_msg void OnGraphRemove();
	afx_msg void OnDblClkGraphItems(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDrawItems(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
