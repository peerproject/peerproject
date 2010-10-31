//
// DlgFilterSearch.h
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

#include "DlgSkinDialog.h"

class CMatchList;
class CResultFilters;

class CFilterSearchDlg : public CSkinDialog
{
// Construction
public:
	CFilterSearchDlg(CWnd* pParent = NULL, CMatchList* pList = NULL);

// Dialog Data
public:
	enum { IDD = IDD_FILTER_SEARCH };
	CSpinButtonCtrl	m_wndSources;
	CString	m_sFilter;
	BOOL	m_bHideBusy;
	BOOL	m_bHideLocal;
	BOOL	m_bHidePush;
	BOOL	m_bHideReject;
	BOOL	m_bHideUnstable;
	BOOL	m_bHideBogus;
	BOOL	m_bHideDRM;
	BOOL	m_bHideRestricted;
	BOOL	m_bHideSuspicious;
	BOOL	m_bHideAdult;
	int		m_nSources;
	CString	m_sMaxSize;
	CString	m_sMinSize;
	BOOL    m_bDefault;
	BOOL	m_bRegExp;

	CMatchList*	m_pMatches;
	CResultFilters * m_pResultFilters;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedSaveFilter();
	afx_msg void OnBnClickedDeleteFilter();
	afx_msg void OnBnClickedSetDefaultFilter();
	afx_msg void OnCbnSelChangeFilters();
	afx_msg void OnEnKillFocusMinMaxSize();
	afx_msg void OnClickedRegexp();

private:
	void UpdateFields();
	void UpdateList();
	CComboBox m_Filters;
};
