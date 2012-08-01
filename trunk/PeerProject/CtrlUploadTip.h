//
// CtrlUploadTip.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#include "CtrlCoolTip.h"

class CUpload;
class CUploadFile;
class CLineGraph;
class CGraphItem;


class CUploadTipCtrl : public CCoolTipCtrl
{
	DECLARE_DYNAMIC(CUploadTipCtrl)

public:
	CUploadTipCtrl();
	virtual ~CUploadTipCtrl();

public:
	void Show(CUploadFile* pContext, HWND hAltWnd = NULL)
	{
		bool bChanged = ( pContext != m_pUploadFile );
		m_pUploadFile = pContext;
		m_hAltWnd = hAltWnd;
		ShowImpl( bChanged );
	}

// Attributes
protected:
	CUploadFile*		m_pUploadFile;
	CLineGraph*			m_pGraph;
	CGraphItem*			m_pItem;
	CString				m_sAddress;
	CString				m_sSeedsPeers;
	int					m_nHeaderWidth;
	CArray< CString >	m_pHeaderName;
	CArray< CString >	m_pHeaderValue;

// Operations
protected:
	void DrawProgressBar(CDC* pDC, CPoint* pPoint, CUploadFile* pFile);

	virtual BOOL OnPrepare();
	virtual void OnShow();
	virtual void OnHide();
	virtual void OnCalcSize(CDC* pDC);
	virtual void OnPaint(CDC* pDC);

// Overrides
//public:
	//{{AFX_VIRTUAL(CUploadTipCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUploadTipCtrl)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
