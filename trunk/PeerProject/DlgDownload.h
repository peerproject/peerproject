//
// DlgDownload.h
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

#include "DlgSkinDialog.h"

class CDownload;
class CPeerProjectURL;


class CDownloadDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CDownloadDlg)

public:
	CDownloadDlg(CWnd* pParent = NULL, CDownload* pDownload = NULL);

	enum { IDD = IDD_DOWNLOAD };

	CList< CString >	m_pURLs;

protected:
	CDownload*			m_pDownload;
	CButton				m_wndTorrentFile;
	CButton				m_wndOK;
	CEdit				m_wndURL;
	CString				m_sURL;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeURL();
	virtual void OnOK();
	afx_msg void OnTorrentFile();

	DECLARE_MESSAGE_MAP()
};
