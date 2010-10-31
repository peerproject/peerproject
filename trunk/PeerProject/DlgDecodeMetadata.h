//
// DlgDecodeMetadata.h
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

class CDecodeMetadataDlg : public CSkinDialog
{
// Construction
public:
	CDecodeMetadataDlg(CWnd* pParent = NULL);

// Dialog Data
public:
	enum { IDD = IDD_CODEPAGES };
	CButton		m_wndOK;
	CComboBox	m_wndCodepages;
	CString		m_sOriginalWords;
	CString		m_sPreview1;
	CString		m_sPreview2;

	CList<DWORD> m_pFiles;

// Overrides
protected:
	int 		m_nMethod;

	static const unsigned codePages[];
	virtual void DoDataExchange(CDataExchange* pDX);

// Implementation
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
			void AddFile(CLibraryFile* pFile);
			void GetEncodedText(CString& strText, int nMethod = 0) const;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnClickedMethod1();
	afx_msg void OnClickedMethod2();
	afx_msg void OnCloseupCodepages();
	afx_msg void OnSelchangeCodepages();
};
