//
// CtrlTipAlbum.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "MetaList.h"


class CAlbumTipCtrl : public CCoolTipCtrl
{
	DECLARE_DYNAMIC(CAlbumTipCtrl)

public:
	CAlbumTipCtrl();
	virtual ~CAlbumTipCtrl();

	void Show(CAlbumFolder* pContext, HWND hAltWnd = NULL)
	{
		bool bChanged = ( pContext != m_pAlbumFolder );
		m_pAlbumFolder = pContext;
		m_hAltWnd = hAltWnd;
		ShowImpl( bChanged );
	}

// Attributes
protected:
	CAlbumFolder*	m_pAlbumFolder;
	CString			m_sName;
	CString			m_sType;
	int				m_nIcon32;
	int				m_nIcon48;
	BOOL			m_bCollection;
	CMetaList		m_pMetadata;
	int				m_nKeyWidth;
	COLORREF		m_crLight;

// Operations
protected:
	void			DrawThumb(CDC* pDC, CRect& rcThumb);

	virtual BOOL	OnPrepare();
	virtual void	OnCalcSize(CDC* pDC);
	virtual void	OnPaint(CDC* pDC);

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
