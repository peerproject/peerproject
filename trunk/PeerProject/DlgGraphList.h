//
// DlgGraphList.h
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

#if !defined(AFX_DLGGRAPHLIST_H__48D0AE69_037E_4F22_8C95_76585433DEF7__INCLUDED_)
#define AFX_DLGGRAPHLIST_H__48D0AE69_037E_4F22_8C95_76585433DEF7__INCLUDED_

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGGRAPHLIST_H__48D0AE69_037E_4F22_8C95_76585433DEF7__INCLUDED_)
