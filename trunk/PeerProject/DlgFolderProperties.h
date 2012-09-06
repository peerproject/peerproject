//
// DlgFolderProperties.h
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
#include "CtrlSchema.h"
#include "CtrlSchemaCombo.h"

class CAlbumFolder;


class CFolderPropertiesDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CFolderPropertiesDlg)

// Construction
public:
	CFolderPropertiesDlg(CWnd* pParent = NULL, CAlbumFolder* pFolder = NULL);

	enum { IDD = IDD_FOLDER_PROPERTIES };

// Dialog Data
public:
	//{{AFX_DATA(CFolderPropertiesDlg)
	CStatic			m_wndApply;
	CButton			m_wndCancel;
	CButton			m_wndOK;
	CEdit			m_wndTitle;
	CSchemaCombo	m_wndSchemas;
	CStatic			m_wndTitleLabel;
	CStatic			m_wndTypeLabel;
	//}}AFX_DATA

// Attributes
protected:
	CAlbumFolder*	m_pFolder;
	CSchemaCtrl		m_wndData;
	int				m_nWidth;
	BOOL			m_bUpdating;

	void	DoApply(BOOL bMetaToFiles);

// Overrides
public:
	//{{AFX_VIRTUAL(CFolderPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFolderPropertiesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelChangeSchemas();
	afx_msg void OnCloseUpSchemas();
	afx_msg void OnPaint();
	afx_msg void OnChangeTitle();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChangeData();
	afx_msg void OnCancel();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define IDC_METADATA		100
