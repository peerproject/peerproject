//
// DlgDownloadSheet.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2006.
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
#include "Download.h"


class CDownloadSheet : public CPropertySheetAdv
{
	DECLARE_DYNAMIC(CDownloadSheet)

public:
	CDownloadSheet(CDownload* pDownload);

	CDownload*		m_pDownload;

	virtual INT_PTR DoModal(int nPage = -1);

protected:
	CString			m_sFilesTitle;
	CString			m_sTrackersTitle;
	CString			m_sGeneralTitle;
	CString			m_sDownloadTitle;
	CString			m_sActionsTitle;

	virtual BOOL	OnInitDialog();

//	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
//	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};
