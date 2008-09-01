//
// DlgFilePropertiesPage.h
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

#if !defined(AFX_DLGFILEPROPERTIESPAGE_H__A39F7917_D5EC_493E_AE35_7B2543BE9650__INCLUDED_)
#define AFX_DLGFILEPROPERTIESPAGE_H__A39F7917_D5EC_493E_AE35_7B2543BE9650__INCLUDED_

#pragma once
#include "PagePropertyAdv.h"

class CLibraryFile;
class CLibraryList;

class CFilePropertiesPage : public CPropertyPageAdv
{
// Construction
public:
	CFilePropertiesPage(UINT nIDD);
	virtual ~CFilePropertiesPage();

	DECLARE_DYNAMIC(CFilePropertiesPage)

// Helpers
protected:
	CLibraryFile*	GetFile();
	CLibraryList*	GetList() const;
private:
	void	PaintStaticHeader(CDC* pDC, CRect* prc, LPCTSTR psz);

// Dialog Data
public:
	//{{AFX_DATA(CFilePropertiesPage)
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CFilePropertiesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFilePropertiesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGFILEPROPERTIESPAGE_H__A39F7917_D5EC_493E_AE35_7B2543BE9650__INCLUDED_)
