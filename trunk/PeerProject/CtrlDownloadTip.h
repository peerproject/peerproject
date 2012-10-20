//
// CtrlDownloadTip.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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

class CPeerProjectFile;
class CDownload;
class CDownloadSource;
class CLineGraph;
class CGraphItem;


class CDownloadTipCtrl : public CCoolTipCtrl
{
	DECLARE_DYNAMIC(CDownloadTipCtrl)

// Construction
public:
	CDownloadTipCtrl();
	virtual ~CDownloadTipCtrl();

public:
	void Show(CDownload* pContext, HWND hAltWnd = NULL)
	{
		bool bChanged = ( pContext != m_pDownload );
		m_pDownload = pContext;
		m_pSource = NULL;
		m_hAltWnd = hAltWnd;
		ShowImpl( bChanged );
	}

	void Show(CDownloadSource* pContext, HWND hAltWnd = NULL)
	{
		bool bChanged = ( pContext != m_pSource );
		m_pDownload = NULL;
		m_pSource = pContext;
		m_hAltWnd = hAltWnd;
		ShowImpl( bChanged );
	}

// Attributes
protected:
	CDownload*			m_pDownload;
	CDownloadSource*	m_pSource;
	CString 			m_sName;
	CString 			m_sSHA1;
	CString 			m_sTiger;
	CString 			m_sED2K;
	CString 			m_sBTH;
	CString 			m_sMD5;
	CString 			m_sURL;
	CString 			m_sSize;
	CString 			m_sType;
	CString 			m_sSeedsPeers;
	CString 			m_sCountryName;
	int 				m_nIcon;
	int 				m_nHeaderWidth;
	int 				m_nStatWidth;
	CArray< CString >	m_pHeaderName;
	CArray< CString >	m_pHeaderValue;
	CLineGraph* 		m_pGraph;
	CGraphItem* 		m_pItem;
	BOOL				m_bDrawGraph;		// Draw the download graph?

// Operations
protected:
	virtual BOOL OnPrepare();
	virtual void OnCalcSize(CDC* pDC);
	virtual void OnShow();
	virtual void OnHide();
	virtual void OnPaint(CDC* pDC);

	void OnCalcSize(CDC* pDC, CDownload* pDownload);
	void OnCalcSize(CDC* pDC, CDownloadSource* pSource);
	void OnPaint(CDC* pDC, CDownload* pDownload);
	void OnPaint(CDC* pDC, CDownloadSource* pSource);

	void PrepareDownloadInfo(CDownload* pDownload);
	void PrepareFileInfo(CDownload* pDownload);	// CPeerProjectFile
	void DrawProgressBar(CDC* pDC, CPoint* pPoint, CDownload* pDownload);
	void DrawProgressBar(CDC* pDC, CPoint* pPoint, CDownloadSource* pSource);

// Overrides
//public:
	//{{AFX_VIRTUAL(CDownloadTipCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDownloadTipCtrl)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
