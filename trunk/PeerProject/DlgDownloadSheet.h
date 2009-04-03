//
// DlgDownloadSheet.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
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
	CString			m_sDownloadTitle;
	CString			m_sActionsTitle;
	CString			m_sGeneralTitle;
	CString			m_sFilesTitle;
	CString			m_sTrackersTitle;

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
