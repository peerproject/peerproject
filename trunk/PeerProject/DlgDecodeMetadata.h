//
// DlgDecodeMetadata.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#ifndef DLGDECODEMETADATA_H_INCLUDED
#define DLGDECODEMETADATA_H_INCLUDED

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	static const unsigned codePages[];

// Implementation
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
			void AddFile(CLibraryFile* pFile);
			void GetEncodedText(CString& strText, int nMethod = 0) const;

protected:
	int m_nMethod;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClickedMethod1();
	afx_msg void OnClickedMethod2();
	afx_msg void OnCloseupCodepages();
	afx_msg void OnSelchangeCodepages();
};

#endif // #ifndef DLGDECODEMETADATA_H_INCLUDED
