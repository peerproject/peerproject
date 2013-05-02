//
// DlgMediaVis.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "DlgSkinDialog.h"

class CMediaFrame;


class CMediaVisDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CMediaVisDlg)

// Construction
public:
	CMediaVisDlg(CMediaFrame* pFrame);
	virtual ~CMediaVisDlg();

	enum { IDD = IDD_MEDIA_VIS };

// Dialog Data
public:
	//{{AFX_DATA(CMediaVisDlg)
	CButton 	m_wndSetup;
	CListCtrl	m_wndList;
	int 		m_nSize;
	//}}AFX_DATA

// Attributes
protected:
	CMediaFrame*	m_pFrame;
	DWORD			m_nIcon;
	HICON			m_hIcon;

// Operations
protected:
	void	Enumerate();
	void	AddPlugin(LPCTSTR pszName, LPCTSTR pszCLSID, LPCTSTR pszPath);
	BOOL	EnumerateWrapped(LPCTSTR pszName, REFCLSID pCLSID, LPCTSTR pszCLSID);

// Implementation
protected:
	//{{AFX_MSG(CMediaVisDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblClkPlugins(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedPlugins(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
