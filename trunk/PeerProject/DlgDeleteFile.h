//
// DlgDeleteFile.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
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

#pragma once

#include "DlgSkinDialog.h"
#include "afxwin.h"

class CDownload;
class CLibraryFile;


class CDeleteFileDlg : public CSkinDialog
{
// Construction
public:
	CDeleteFileDlg(CWnd* pParent = NULL);
	virtual ~CDeleteFileDlg();

	DECLARE_DYNAMIC(CDeleteFileDlg)
	enum { IDD = IDD_DELETE_FILE };

// Members
public:
	BOOL m_bAll;
	CStatic m_wndName;
	CString m_sComments;
	CString m_sName;
	int m_nRateValue;

private:
	CString m_sOriginalComments;
	int m_nOriginalRating;
	CButton m_wndOK;
	CEdit m_wndComments;
	CButton m_wndAll;
	CComboBox m_wndOptions;
	int m_nOption;
	CComboBox m_wndRating;
	BOOL m_bCreateGhost;
	CStatic m_wndPrompt;

// Operations
public:
	void	Apply(CLibraryFile* pFile);
	void	Create(CDownload* pDownload, BOOL bShare);

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDeleteAll();
	afx_msg void OnCbnChangeOptions();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnCbnChangeGhostRating();
	afx_msg void OnChangeComments();
	afx_msg void OnClickedCreateGhost();
	DECLARE_MESSAGE_MAP()
};
