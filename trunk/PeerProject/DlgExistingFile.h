//
// DlgExistingFile.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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

class CLibraryFile;
//class CPeerProjectFile;
//class CPeerProjectURL;


class CExistingFileDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CExistingFileDlg)

public:
	CExistingFileDlg(const CLibraryFile* pFile, CWnd* pParent = NULL, bool bDuplicateSearch = false);

	enum { IDD = IDD_EXISTING_FILE };
	enum Action { ShowInLibrary, Download, DontDownload, Cancel };

	static Action CheckExisting(const CPeerProjectFile* pFile);
//	static Action CheckExisting(const CPeerProjectURL* pURL);

	inline Action GetResult() const
	{
		switch ( m_nAction )
		{
		case 0:
			return ShowInLibrary;
		case 1:
			return Download;
		case 2:
			return DontDownload;
		default:
			return Cancel;
		}
	}

	virtual INT_PTR DoModal();

protected:
	CButton	m_wndOK;
	CStatic	m_wndName;
	CString	m_sName;
	CString	m_sURN;
	int		m_nAction;
	CStatic m_wndComments;
	CStatic m_wndMessageAvailable;
	CStatic m_wndMessageDeleted;
	CStatic m_wndMessageDuplicates;
	CButton m_wndLocate;
	CButton m_wndDownload;
	CButton m_wndDontDownload;
	CString m_sComments;
	TRISTATE m_bAvailable;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnAction0();
	afx_msg void OnAction1();
	afx_msg void OnAction2();

	DECLARE_MESSAGE_MAP()
};
