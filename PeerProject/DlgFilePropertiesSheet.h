//
// DlgFilePropertiesSheet.h
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

#include "PagePropertyAdv.h"
#include "LibraryList.h"


class CFilePropertiesSheet : public CPropertySheetAdv
{
	DECLARE_DYNAMIC(CFilePropertiesSheet)

public:
	CFilePropertiesSheet(CLibraryListItem oObject = CLibraryListItem());

	CLibraryList	m_pList;

	void	Add(CLibraryListItem oObject);
	void	Add(CLibraryList* pList);

	virtual INT_PTR DoModal(int nPage = -1);

protected:
	CString			m_sGeneralTitle;
	CString			m_sMetadataTitle;
	CString			m_sCommentsTitle;
	CString			m_sSharingTitle;
	CString			m_sSourcesTitle;

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
