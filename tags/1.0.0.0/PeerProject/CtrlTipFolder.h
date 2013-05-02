//
// CtrlTipFolder.h
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

#include "CtrlCoolTip.h"
#include "LibraryFolders.h"


class CFolderTipCtrl : public CCoolTipCtrl
{
	DECLARE_DYNAMIC(CFolderTipCtrl)

public:
	CFolderTipCtrl();
	virtual ~CFolderTipCtrl();

public:
	void Show(CLibraryFolder* pContext, HWND hAltWnd = NULL)
	{
		bool bChanged = ( pContext != m_pLibraryFolder );
		m_pLibraryFolder = pContext;
		m_hAltWnd = hAltWnd;
		ShowImpl( bChanged );
	}

// Attributes
protected:
	CLibraryFolder* m_pLibraryFolder;
	CString		m_sName;
	CString		m_sPath;
	CString		m_sFiles;
	CString		m_sVolume;
	CString		m_sPercentage;

// Operations
public:
	virtual BOOL OnPrepare();
	virtual void OnCalcSize(CDC* pDC);
	virtual void OnPaint(CDC* pDC);

// Overrides
//public:
	//{{AFX_VIRTUAL(CFolderTipCtrl)
	//}}AFX_VIRTUAL

// Implementation
//protected:
	//{{AFX_MSG(CFolderTipCtrl)
	//}}AFX_MSG

	//DECLARE_MESSAGE_MAP()
};
