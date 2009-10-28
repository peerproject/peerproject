//
// CtrlTipFolder.h
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

#include "CtrlCoolTip.h"


class CFolderTipCtrl : public CCoolTipCtrl
{
// Construction
public:
	CFolderTipCtrl();
	virtual ~CFolderTipCtrl();

	DECLARE_DYNAMIC(CFolderTipCtrl)

// Attributes
protected:
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
public:
	//{{AFX_VIRTUAL(CFolderTipCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFolderTipCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
