//
// CtrlSearchPanel.h
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

#if !defined(AFX_CTRLSEARCHPANEL_H__EAFFA7F3_526D_45C3_8C17_17A265ED3240__INCLUDED_)
#define AFX_CTRLSEARCHPANEL_H__EAFFA7F3_526D_45C3_8C17_17A265ED3240__INCLUDED_

#pragma once

#include "CtrlTaskPanel.h"
#include "CtrlSchemaCombo.h"
#include "CtrlNetworkCombo.h"
#include "CtrlSchema.h"
#include "CtrlIconButton.h"
#include "ManagedSearch.h"

class CSearchInputBox : public CTaskBox
{
// Construction
public:
	CSearchInputBox();
	virtual ~CSearchInputBox();
	DECLARE_DYNAMIC(CSearchInputBox)

// Attributes
public:
	CEdit			m_wndSearch;
	CSchemaCombo	m_wndSchemas;
	CIconButtonCtrl	m_wndStart;
	CIconButtonCtrl	m_wndStop;
	CIconButtonCtrl	m_wndPrefix;

// Operations
public:
	void	OnSkinChange();

// Overrides
public:
	//{{AFX_VIRTUAL(CSearchInputBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSearchInputBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSelChangeSchemas();
	afx_msg void OnCloseUpSchemas();
	afx_msg void OnSearchStart();
	afx_msg void OnSearchStop();
	afx_msg void OnSearchPrefix();
	afx_msg void OnSearchPrefixSHA1();
	afx_msg void OnSearchPrefixTiger();
	afx_msg void OnSearchPrefixSHA1Tiger();
	afx_msg void OnSearchPrefixED2K();
	afx_msg void OnSearchPrefixBTH();
	afx_msg void OnSearchPrefixMD5();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CSearchAdvancedBox : public CTaskBox
{
// Construction
public:
	CSearchAdvancedBox();
	virtual ~CSearchAdvancedBox();
	DECLARE_DYNAMIC(CSearchAdvancedBox)

// Attributes
public:
	CStatic			m_wndSizeMinMax;
	CComboBox		m_wndSizeMin;
	CComboBox		m_wndSizeMax;
	CButton			m_wndCheckBoxG1;
	CButton			m_wndCheckBoxG2;
	CButton			m_wndCheckBoxED2K;
	CBrush			m_brBack;
	COLORREF		m_crBack;
	CImageList		m_gdiImageList;
	
// Operations
public:
	void	OnSkinChange();

// Overrides
public:
	//{{AFX_VIRTUAL(CSearchAdvancedBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSearchAdvancedBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnG2Clicked();
	afx_msg void OnG1Clicked();
	afx_msg void OnED2KClicked();
	afx_msg LRESULT OnCtlColorStatic(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CSearchSchemaBox : public CTaskBox
{
// Construction
public:
	CSearchSchemaBox();
	virtual ~CSearchSchemaBox();
	DECLARE_DYNAMIC(CSearchSchemaBox)
	
// Attributes
public:
	CSchemaCtrl		m_wndSchema;

// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CSearchSchemaBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSearchSchemaBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CSearchResultsBox : public CTaskBox
{
// Construction
public:
	CSearchResultsBox();
	virtual ~CSearchResultsBox();
	DECLARE_DYNAMIC(CSearchResultsBox)

// Attributes
public:
	BOOL	m_bActive;
	DWORD	m_nFiles;
	DWORD	m_nHits;
	DWORD	m_nHubs;
	DWORD	m_nLeaves;

// Operations
public:
	void	Update(BOOL bSearching, DWORD nFiles, DWORD nHits, DWORD nHubs, DWORD nLeaves);
protected:
	static void DrawText(CDC* pDC, int nX, int nY, UINT nFlags, LPCTSTR pszText);

	virtual void OnExpanded(BOOL bOpen);

// Overrides
public:
	//{{AFX_VIRTUAL(CSearchResultsBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSearchResultsBox)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CSearchPanel : public CTaskPanel
{
// Construction
public:
	CSearchPanel();

	DECLARE_DYNAMIC(CSearchPanel)

// Attributes
public:
	BOOL				m_bSendSearch;
	CSearchInputBox		m_boxSearch;
	CSearchAdvancedBox	m_boxAdvanced;
	CSearchSchemaBox	m_boxSchema;
	CSearchResultsBox	m_boxResults;
	BOOL				m_bAdvanced;

// Operations
public:
	void			SetSearchFocus();
	void			ShowSearch(CManagedSearch* pSearch);
	void			ShowStatus(BOOL bStarted, BOOL bSearching, DWORD nFiles, DWORD nHits, DWORD nHubs, DWORD nLeaves);
	void			OnSchemaChange();
	void			ExecuteSearch();
	auto_ptr< CManagedSearch > GetSearch();
	void			OnSkinChange();
	void			Disable();
	void			Enable();
	
// Overrides
public:
	//{{AFX_VIRTUAL(CSearchPanel)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSearchPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define IDC_SEARCH_SIZEMIN				108
#define IDC_SEARCH_SIZEMAX				109

#define IDC_SEARCH_PREFIX				120
#define IDC_SEARCH_PREFIX_SHA1			121
#define IDC_SEARCH_PREFIX_TIGER			122
#define IDC_SEARCH_PREFIX_SHA1_TIGER	123
#define IDC_SEARCH_PREFIX_ED2K			124
#define IDC_SEARCH_PREFIX_BTH			125
#define IDC_SEARCH_PREFIX_MD5			126

#define IDC_SEARCH_GNUTELLA1			277
#define IDC_SEARCH_GNUTELLA2			278
#define IDC_SEARCH_EDONKEY				279

#endif // !defined(AFX_CTRLSEARCHPANEL_H__EAFFA7F3_526D_45C3_8C17_17A265ED3240__INCLUDED_)
