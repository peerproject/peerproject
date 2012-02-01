//
// DlgCollectionExport.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "CtrlWizard.h"

class CAlbumFolder;
class CXMLElement;


class CCollectionExportDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CCollectionExportDlg)

// Construction
public:
	CCollectionExportDlg(CAlbumFolder* pFolder, CWnd* pParent = NULL);
	virtual ~CCollectionExportDlg();

	enum { IDD = IDD_COLLECTION_EXPORT };

// Dialog Data
public:
	//{{AFX_DATA(CCollectionExportDlg)
	CButton		m_wndOK;
	CStatic		m_wndExplain;
	CStatic		m_wndLblAuthor;
	CStatic		m_wndLblName;
	CStatic		m_wndLblDesc;
	CStatic		m_wndGroupBox;
	CButton		m_wndDelete;
	CEdit		m_wndDesc;
	CStatic		m_wndName;
	CStatic		m_wndAuthor;
	CListCtrl	m_wndList;
	//}}AFX_DATA;

// Attributes
protected:
	CAlbumFolder* m_pFolder;
	CImageList	m_gdiImageList;
	int			m_nSelected;
	int			m_nStep;
	CString		m_sBtnBack;
	CString		m_sBtnDelete;
	CString		m_sBtnExport;
	CString		m_sBtnNext;
	CString		m_sLblExplain1;
	CString		m_sLblExplain2;
	CWizardCtrl	m_wndWizard;

// Operations
public:
	void		EnumerateTemplates(LPCTSTR pszPath = NULL);
	BOOL		AddTemplate(LPCTSTR pszPath, LPCTSTR pszName);
	CString		DirFromPath(LPCTSTR szPath);

// Overrides
protected:
	//{{AFX_VIRTUAL(CCollectionExportDlg)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCollectionExportDlg)
	afx_msg void OnTemplatesDeleteOrBack();
	afx_msg void OnItemChangedTemplates(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define IDC_WIZARD		99
