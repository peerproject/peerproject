//
// DlgMediaVis.h
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

#if !defined(AFX_DLGMEDIAVIS_H__61DBA242_FBB7_4C1C_815A_470BC14F5ED1__INCLUDED_)
#define AFX_DLGMEDIAVIS_H__61DBA242_FBB7_4C1C_815A_470BC14F5ED1__INCLUDED_

#pragma once

#include "DlgSkinDialog.h"

class CMediaFrame;


class CMediaVisDlg : public CSkinDialog
{
// Construction
public:
	CMediaVisDlg(CMediaFrame* pFrame);
	virtual ~CMediaVisDlg();

	DECLARE_DYNAMIC(CMediaVisDlg)

// Dialog Data
public:
	//{{AFX_DATA(CMediaVisDlg)
	enum { IDD = IDD_MEDIA_VIS };
	CButton	m_wndSetup;
	CListCtrl	m_wndList;
	int		m_nSize;
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

// Overrides
public:
	//{{AFX_VIRTUAL(CMediaVisDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMediaVisDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblClkPlugins(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedPlugins(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGMEDIAVIS_H__61DBA242_FBB7_4C1C_815A_470BC14F5ED1__INCLUDED_)
