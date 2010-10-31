//
// PageDownloadEdit.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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

#include "PagePropertyAdv.h"
#include "WndSettingsPage.h"	// For CEditPath
#include <afxdtctl.h>			// MFC date & time controls

class CDownload;


class CDownloadEditPage : public CPropertyPageAdv
{
public:
	CDownloadEditPage();
	virtual ~CDownloadEditPage();

	DECLARE_DYNAMIC(CDownloadEditPage)

	enum { IDD = IDD_DOWNLOAD_EDIT };

protected:
	CString	m_sName;
	CString	m_sDiskName;
	CString	m_sFileSize;
	CString	m_sSHA1;
	CString	m_sTiger;
	CString	m_sED2K;
	CString	m_sMD5;
	CString	m_sBTH;
	BOOL	m_bSHA1Trusted;
	BOOL	m_bTigerTrusted;
	BOOL	m_bED2KTrusted;
	BOOL	m_bMD5Trusted;
	BOOL	m_bBTHTrusted;

	CDateTimeCtrl	m_wndDate;
	CEditPath		m_wndPath;	// For Double-click aware Edit box

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

	DECLARE_MESSAGE_MAP()
};
