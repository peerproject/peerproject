//
// PageFileMetadata.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#pragma once

#include "DlgFilePropertiesPage.h"
#include "CtrlSchema.h"
#include "CtrlSchemaCombo.h"

class CFileMetadataPage : public CFilePropertiesPage
{
public:
	CFileMetadataPage();
	virtual ~CFileMetadataPage();

	DECLARE_DYNCREATE(CFileMetadataPage)
	enum { IDD = IDD_FILE_METADATA };

	CSchemaCombo	m_wndSchemas;
	CSchemaCtrl		m_wndData;
	CXMLElement*	m_pXML;

protected:
	CXMLElement*	m_pSchemaContainer;

	void AddCrossAttributes(CXMLElement* pXML, LPCTSTR pszTargetURI);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeSchemas();
	afx_msg void OnCloseUpSchemas();

	DECLARE_MESSAGE_MAP()
};

#define IDC_METADATA	100
