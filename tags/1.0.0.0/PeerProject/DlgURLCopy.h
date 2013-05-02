//
// DlgURLCopy.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

class CPeerProjectFile;


class CURLCopyDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CURLCopyDlg)

public:
	CURLCopyDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_URL_COPY };

public:
	void		Add(const CPeerProjectFile* pFile);

protected:
	const CPeerProjectFile*		m_pFile;
	CButton		m_wndIncludeSelf;
	CStatic		m_wndMessage;
	CString		m_sHost;
	CString		m_sMagnet;
	CString		m_sGnutella;
	CString		m_sED2K;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnIncludeSelf();
	afx_msg void OnStnClickedUrlHost();
	afx_msg void OnStnClickedUrlMagnet();
	afx_msg void OnStnClickedUrlEd2k();

	DECLARE_MESSAGE_MAP()
};
