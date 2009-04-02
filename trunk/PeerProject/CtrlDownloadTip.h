//
// CtrlDownloadTip.h
//
// This file is part of PeerProject (peerproject.org) � 2008
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

class CPeerProjectFile;
class CDownload;
class CDownloadSource;
class CLineGraph;
class CGraphItem;


class CDownloadTipCtrl : public CCoolTipCtrl
{
// Construction
public:
	CDownloadTipCtrl();
	virtual ~CDownloadTipCtrl();

	DECLARE_DYNAMIC(CDownloadTipCtrl)

// Attributes
protected:
	CString			m_sName;
	CString			m_sSHA1;
	CString			m_sTiger;
	CString			m_sED2K;
	CString			m_sBTH;
	CString			m_sMD5;
	CString			m_sURL;
	CString			m_sSize;
	CString			m_sType;
	CString			m_sCountryName;
	int				m_nIcon;
protected:
	CLineGraph*		m_pGraph;
	CGraphItem*		m_pItem;
protected:
	CArray< CString >	m_pHeaderName;
	CArray< CString >	m_pHeaderValue;
	int				m_nHeaderWidth;
	int				m_nStatWidth;
	BOOL			m_bDrawGraph;		//Draw the download graph?
	BOOL			m_bDrawError;		//Display the tracker error?

// Operations
protected:
	virtual BOOL OnPrepare();
	virtual void OnCalcSize(CDC* pDC);
	virtual void OnShow();
	virtual void OnHide();
	virtual void OnPaint(CDC* pDC);
protected:
	void OnCalcSize(CDC* pDC, CDownload* pDownload);
	void OnCalcSize(CDC* pDC, CDownloadSource* pSource);
	void OnPaint(CDC* pDC, CDownload* pDownload);
	void OnPaint(CDC* pDC, CDownloadSource* pSource);
protected:
	void PrepareDownloadInfo(CDownload* pDownload);
	void PrepareFileInfo(CPeerProjectFile* pDownload);
	void DrawProgressBar(CDC* pDC, CPoint* pPoint, CDownload* pDownload);
	void DrawProgressBar(CDC* pDC, CPoint* pPoint, CDownloadSource* pSource);

// Overrides
public:
	//{{AFX_VIRTUAL(CDownloadTipCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDownloadTipCtrl)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
