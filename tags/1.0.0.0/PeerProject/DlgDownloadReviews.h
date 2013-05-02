//
// DlgDownloadReviews.h
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
#include "CtrlDragList.h"
#include "LiveList.h"
class CDownload;


class CDownloadReviewDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CDownloadReviewDlg)

// Construction
public:
	CDownloadReviewDlg(CWnd* pParent = NULL, CDownload* pDownload = NULL);
	virtual ~CDownloadReviewDlg();

	enum { IDD = IDD_DOWNLOAD_REVIEWS };

// Dialog Data
public:
	CDragListCtrl	m_wndReviews;
	CString	m_sReviewFileName;
	CButton	m_wndOK;

	CDownload* m_pDownload;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//DECLARE_MESSAGE_MAP()
};
