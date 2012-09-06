//
// DlgBitziDownload.h
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
#include "BitziDownloader.h"


class CBitziDownloadDlg : public CSkinDialog
{
// Construction
public:
	CBitziDownloadDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_BITZI_DOWNLOAD };

// Attributes
protected:
	CBitziDownloader	m_pDownloader;
	DWORD				m_nFailures;

// Operations
public:
	void	AddFile(DWORD nIndex);
	void	OnNextFile(DWORD nIndex);
	void	OnRequesting(DWORD nIndex, LPCTSTR pszName);
	void	OnSuccess(DWORD nIndex);
	void	OnFailure(DWORD nIndex, LPCTSTR pszMessage);
	void	OnFinishedFile(DWORD nIndex);

// Dialog Data
public:
	//{{AFX_DATA(CBitziDownloadDlg)
	CStatic	m_wndWeb;
	CListCtrl	m_wndFiles;
	CButton	m_wndCancel;
	CProgressCtrl	m_wndProgress;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CBitziDownloadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CBitziDownloadDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
