//
// DlgDeleteFile.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2006.
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
#include "afxwin.h"

class CDownload;
class CLibraryFile;


class CDeleteFileDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CDeleteFileDlg)

// Construction
public:
	CDeleteFileDlg(CWnd* pParent = NULL);
	virtual ~CDeleteFileDlg();

	enum { IDD = IDD_DELETE_FILE };

// Members
public:
	BOOL	m_bAll;
	CStatic m_wndName;
	CString m_sComments;
	CString m_sName;
	int 	m_nRateValue;

private:
	CString m_sOriginalComments;
	int 	m_nOriginalRating;
	CButton m_wndOK;
	CEdit	m_wndComments;
	CButton m_wndAll;
	CComboBox m_wndOptions;
	int 	m_nOption;
	CComboBox m_wndRating;
	BOOL	m_bCreateGhost;
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
