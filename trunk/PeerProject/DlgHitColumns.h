//
// DlgHitColumns.h
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
#include "CtrlSchemaCombo.h"
#include "Schema.h"


class CSchemaColumnsDlg : public CSkinDialog
{
// Construction
public:
	CSchemaColumnsDlg(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_SCHEMA_COLUMNS };

// Dialog Data
public:
	//{{AFX_DATA(CSchemaColumnsDlg)
	CListCtrl	m_wndColumns;
	CSchemaCombo	m_wndSchemas;
	//}}AFX_DATA

// Attributes
public:
	CSchemaPtr	m_pSchema;
	CList< CSchemaMember* >	m_pColumns;

// Operations
public:
	static CMenu*	BuildColumnMenu(CSchemaPtr pSchema, CList< CSchemaMember* >* pColumns = NULL);
	static BOOL		LoadColumns(CSchemaPtr pSchema, CList< CSchemaMember* >* pColumns);
	static BOOL		SaveColumns(CSchemaPtr pSchema, CList< CSchemaMember* >* pColumns);
	static BOOL		ToggleColumnHelper(CSchemaPtr pSchema, CList< CSchemaMember* >* pSource, CList< CSchemaMember* >* pTarget, UINT nToggleID, BOOL bSave = FALSE);

// Overrides
protected:
	//{{AFX_VIRTUAL(CSchemaColumnsDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSchemaColumnsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeSchemas();
	virtual void OnOK();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
