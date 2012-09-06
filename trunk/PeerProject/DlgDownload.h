//
// DlgDownload.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeURL();
	virtual void OnOK();
	afx_msg void OnTorrentFile();

	DECLARE_MESSAGE_MAP()
};
