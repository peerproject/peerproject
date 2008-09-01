//
// CtrlTipAlbum.h
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

#if !defined(AFX_CTRLTIPALBUM_H__0E00863D_B269_42BF_9A76_D145F7560390__INCLUDED_)
#define AFX_CTRLTIPALBUM_H__0E00863D_B269_42BF_9A76_D145F7560390__INCLUDED_

#pragma once

#include "CtrlCoolTip.h"
#include "MetaList.h"


class CAlbumTipCtrl : public CCoolTipCtrl
{
// Construction
public:
	CAlbumTipCtrl();
	virtual ~CAlbumTipCtrl();

	DECLARE_DYNAMIC(CAlbumTipCtrl)

// Attributes
protected:
	CString			m_sName;
	CString			m_sType;
	int				m_nIcon32;
	int				m_nIcon48;
	BOOL			m_bCollection;
	CMetaList		m_pMetadata;
	int				m_nKeyWidth;
	COLORREF		m_crLight;

// Operations
public:
	virtual BOOL OnPrepare();
	virtual void OnCalcSize(CDC* pDC);
	virtual void OnPaint(CDC* pDC);
protected:
	void		DrawThumb(CDC* pDC, CRect& rcThumb);

// Overrides
public:
	//{{AFX_VIRTUAL(CAlbumTipCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAlbumTipCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CTRLTIPALBUM_H__0E00863D_B269_42BF_9A76_D145F7560390__INCLUDED_)
